#include "imx8mp_clock.h"
#include "imx8mp_clock_regs.h"

int imx8mp_clock_root_get(unsigned long offset, const unsigned long *parents, unsigned int parent_count, imx8mp_root_info_t *info) {
    unsigned int reg;
    unsigned int mux;
    unsigned int pre;
    unsigned int post;
    unsigned long parent_frequency;
    unsigned long long divisor;

    if (parents == 0 || info == 0)
        return -1;

    if (parent_count == 0 || parent_count > 8)
        return -1;

    if ((offset & 3UL) != 0)
        return -1;

    if (offset < CCM_ROOT_BASE)
        return -1;

    if (offset > 0xBFFFUL)
        return -1;

    if (((offset - CCM_ROOT_BASE) % CCM_ROOT_STRIDE) != 0)
        return -1;

    reg = clock_read32(IMX8MP_CCM_BASE + offset);

    mux = ROOT_MUX_GET(reg);
    pre = ROOT_PRE_GET(reg);
    post = ROOT_POST_GET(reg);

    if (mux >= parent_count)
        return -1;

    parent_frequency = parents[mux];

    if (parent_frequency == 0)
        return -1;

    info->raw_register = reg;

    info->mux = mux;

    info->pre_div = pre + 1U;
    info->post_div = post + 1U;

    info->enabled = !!(reg & ROOT_ENABLE);

    info->parent_frequency = parent_frequency;

    divisor = (unsigned long long)info->pre_div *
              (unsigned long long)info->post_div;

    if (divisor == 0)
        return -1;

    info->frequency =
        (unsigned long)(
            (unsigned long long)parent_frequency / divisor
        );

    return 0;
}
