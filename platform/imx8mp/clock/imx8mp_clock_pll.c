#include "stdint.h"
#include "imx8mp_clock_pll.h"

/*
 * MMIO access
 *
 * The same implementation is used on physical hardware
 * and QEMU. Both expose the i.MX8MP ANATOP register map.
 */

#define MMIO32(addr) (*(volatile uint32_t *)(uintptr_t)(addr))

static int pll_read_register(
    unsigned long base,
    unsigned int offset,
    unsigned int *value)
{
    unsigned long address;

    if (value == 0)
        return -1;

    if ((offset & 3U) != 0)
        return -1;

    if (base + offset > IMX8MP_ANALOG_SIZE - 4U)
        return -1;

    address = IMX8MP_ANALOG_BASE + base + offset;

    *value = MMIO32(address);

    return 0;
}


/*
 * PLL descriptor lookup
 */

static const imx8mp_pll_desc_t *pll_find_description(
    unsigned long base,
    imx8mp_pll_type_t type)
{
    unsigned int i;

    for (i = 0; i < imx8mp_pll_description_count; i++) {

        const imx8mp_pll_desc_t *desc =
            &imx8mp_pll_descriptions[i];

        if (desc->base == base && desc->type == type)
            return desc;
    }

    return 0;
}


/*
 * Signed KDIV conversion
 */

static int pll_signed_kdiv(unsigned int value)
{
    value &= PLL_KDIV_MASK;

    if (value & 0x8000U)
        return (int)value - 65536;

    return (int)value;
}


/*
 * 1416x frequency calculation
 *
 * Fout = Fin * MDIV / (PDIV * 2^SDIV)
 */

static unsigned long pll1416x_frequency(
    unsigned long reference,
    unsigned int mdiv,
    unsigned int pdiv,
    unsigned int sdiv)
{
    unsigned long long numerator;
    unsigned long long denominator;

    if (reference == 0 || mdiv == 0 || pdiv == 0)
        return 0;

    if (sdiv > 6)
        return 0;

    numerator =
        (unsigned long long)reference * mdiv;

    denominator =
        (unsigned long long)pdiv << sdiv;

    return (unsigned long)(numerator / denominator);
}


/*
 * 1443x frequency calculation
 *
 * Fout = Fin * (MDIV * 65536 + KDIV)
 *        / (PDIV * 65536 * 2^SDIV)
 */

static unsigned long pll1443x_frequency(
    unsigned long reference,
    unsigned int mdiv,
    unsigned int pdiv,
    unsigned int sdiv,
    int kdiv)
{
    unsigned long long numerator;
    unsigned long long denominator;

    long long multiplier;

    if (reference == 0 || mdiv == 0 || pdiv == 0)
        return 0;

    if (sdiv > 6)
        return 0;

    multiplier =
        ((long long)mdiv * 65536LL) + kdiv;

    if (multiplier <= 0)
        return 0;

    numerator =
        (unsigned long long)reference *
        (unsigned long long)multiplier;

    denominator =
        (unsigned long long)pdiv * 65536ULL;

    denominator <<= sdiv;

    return (unsigned long)(numerator / denominator);
}


/*
 * Reference clock selection
 */

static void pll_get_reference(
    unsigned int gen,
    const imx8mp_pll_reference_t *reference,
    imx8mp_pll_info_t *info)
{
    unsigned int selection;

    selection =
        (gen & PLL_REF_CLK_SEL_MASK)
        >> PLL_REF_CLK_SEL_SHIFT;

    info->reference_source = selection;

    info->reference_frequency = 0;

    info->reference_valid = 0;

    /*
     * i.MX8MP:
     *
     * 0 = OSC_24M
     * 1 = Dummy
     * 2 = Dummy
     * 3 = Dummy
     */

    if (selection != PLL_REF_CLK_OSC24M)
        return;

    if (reference == 0)
        return;

    if (reference->osc_24m_hz == 0)
        return;

    info->reference_frequency =
        reference->osc_24m_hz;

    info->reference_valid = 1;
}


/*
 * Frequency calculation
 */

static void pll_calculate_frequency(
    const imx8mp_pll_desc_t *desc,
    imx8mp_pll_info_t *info)
{
    unsigned long frequency;

    info->configured_frequency = 0;

    info->configured_valid = 0;

    if (!info->reference_valid)
        return;

    if (desc->type == IMX8MP_PLL_1416X) {

        frequency = pll1416x_frequency(
            info->reference_frequency,
            info->mdiv,
            info->pdiv,
            info->sdiv
        );

    } else if (desc->type == IMX8MP_PLL_1443X) {

        frequency = pll1443x_frequency(
            info->reference_frequency,
            info->mdiv,
            info->pdiv,
            info->sdiv,
            info->kdiv
        );

    } else {

        return;
    }

    if (frequency == 0)
        return;

    info->configured_frequency = frequency;

    info->configured_valid = 1;
}


/*
 * Initialize PLL information
 */

static void pll_info_clear(imx8mp_pll_info_t *info)
{
    unsigned int *p;
    unsigned int count;
    unsigned int i;

    /*
     * Explicit initialization keeps the implementation
     * independent of libc.
     *
     * Do not use memset().
     */

    p = (unsigned int *)info;

    count = sizeof(*info) / sizeof(unsigned int);

    for (i = 0; i < count; i++)
        p[i] = 0;

    info->reference_source =
        IMX8MP_PLL_REF_UNKNOWN;
}


/*
 * Extended PLL information API
 */

int imx8mp_pll_get_info_ex(
    unsigned long base,
    imx8mp_pll_type_t type,
    const imx8mp_pll_reference_t *reference,
    imx8mp_pll_info_t *info)
{
    const imx8mp_pll_desc_t *desc;

    unsigned int gen;
    unsigned int div0;
    unsigned int div1;

    if (info == 0)
        return -1;

    pll_info_clear(info);

    desc = pll_find_description(base, type);

    if (desc == 0)
        return -1;

    /*
     * Read GEN_CTRL.
     */

    if (pll_read_register(
            base,
            desc->gen_ctrl_offset,
            &gen) != 0)
        return -1;

    /*
     * Read FDIV_CTL0.
     */

    if (pll_read_register(
            base,
            desc->div0_offset,
            &div0) != 0)
        return -1;

    info->gen_ctrl = gen;

    info->fdiv_ctl0 = div0;

    /*
     * Architecture-specific register.
     */

    if (desc->has_fractional_divider) {

        if (pll_read_register(
                base,
                desc->div1_offset,
                &div1) != 0)
            return -1;

        info->fdiv_ctl1 = div1;

        info->kdiv = pll_signed_kdiv(div1);

    } else {

        if (pll_read_register(
                base,
                desc->lock_ctrl_offset,
                &info->lock_ctrl) != 0)
            return -1;
    }

    /*
     * Divider decoding.
     */

    info->mdiv =
        (div0 & PLL_MDIV_MASK)
        >> PLL_MDIV_SHIFT;

    info->pdiv =
        (div0 & PLL_PDIV_MASK)
        >> PLL_PDIV_SHIFT;

    info->sdiv =
        (div0 & PLL_SDIV_MASK)
        >> PLL_SDIV_SHIFT;

    /*
     * PLL status.
     */

    info->locked =
        !!(gen & PLL_LOCK);

    info->clke =
        !!(gen & PLL_CLKE);

    /*
     * PLL_RST is active-low.
     *
     * reset = 1 means reset asserted.
     */

    info->reset =
        !(gen & PLL_RST);

    info->enabled =
        info->clke && !info->reset;

    /*
     * Bypass decoding.
     */

    info->internal_bypass =
        !!(gen & PLL_BYPASS);

    info->output_bypass =
        !!(gen & PLL_OUTPUT_BYPASS_MUX);

    /*
     * Calculate reference frequency.
     */

    pll_get_reference(
        gen,
        reference,
        info
    );

    /*
     * Calculate configured frequency.
     */

    pll_calculate_frequency(
        desc,
        info
    );

    /*
     * Generated PLL output.
     */

    if (info->configured_valid &&
        info->enabled &&
        info->locked &&
        !info->internal_bypass &&
        !(gen & PLL_EXT_BYPASS)) {

        info->output_frequency =
            info->configured_frequency;
    }

    /*
     * Effective output.
     *
     * The output bypass mux and internal bypass
     * select the reference instead of the PLL.
     *
     * External bypass is reported separately.
     */

    if (info->internal_bypass ||
        info->output_bypass ||
        (gen & PLL_EXT_BYPASS)) {

        if (info->reference_valid) {

            info->effective_frequency =
                info->reference_frequency;

            info->valid = 1;
        }

    } else {

        if (info->output_frequency != 0) {

            info->effective_frequency =
                info->output_frequency;

            info->valid = 1;
        }
    }

    /*
     * Backward compatibility.
     */

    info->frequency =
        info->effective_frequency;

    return 0;
}


/*
 * Compatibility API
 */

int imx8mp_pll_get_info(
    unsigned long base,
    imx8mp_pll_type_t type,
    imx8mp_pll_info_t *info)
{
    return imx8mp_pll_get_info_ex(
        base,
        type,
        &imx8mp_pll_default_reference,
        info
    );
}


/*
 * Compatibility frequency API
 */

unsigned long imx8mp_pll_get_frequency(
    unsigned long base,
    imx8mp_pll_type_t type)
{
    imx8mp_pll_info_t info;

    if (imx8mp_pll_get_info(
            base,
            type,
            &info) != 0)
        return 0;

    if (!info.configured_valid)
        return 0;

    return info.configured_frequency;
}

int imx8mp_pll_validate_descriptors(void)
{
    unsigned int i;
    unsigned int j;

    for (i = 0; i < imx8mp_pll_description_count; i++) {

        const imx8mp_pll_desc_t *desc =
            &imx8mp_pll_descriptions[i];

        if (desc->name[0] == '\0')
            return -1;

        if (desc->base > IMX8MP_ANALOG_SIZE)
            return -2;

        if (desc->type != IMX8MP_PLL_1416X &&
            desc->type != IMX8MP_PLL_1443X)
            return -3;

        for (j = i + 1; j < imx8mp_pll_description_count; j++) {
            if (desc->base ==
                imx8mp_pll_descriptions[j].base)
                return -4;
        }
    }

    return 0;
}
