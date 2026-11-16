#ifndef IMX8MP_CLOCK_PLL_H
#define IMX8MP_CLOCK_PLL_H

#include "imx8mp_clock_regs.h"

/* PLL architectures */

typedef enum {
    IMX8MP_PLL_1416X = 0,
    IMX8MP_PLL_1443X
} imx8mp_pll_type_t;


/* Reference clock configuration */

typedef struct {
    unsigned long osc_24m_hz;
} imx8mp_pll_reference_t;


/* Reference source */

typedef enum {
    IMX8MP_PLL_REF_OSC24M = 0,
    IMX8MP_PLL_REF_UNKNOWN
} imx8mp_pll_reference_source_t;


/* PLL description */

typedef struct {
    char name[20];
    unsigned long base;
    imx8mp_pll_type_t type;
    unsigned int gen_ctrl_offset;
    unsigned int div0_offset;
    unsigned int div1_offset;
    unsigned int lock_ctrl_offset;
    unsigned int bypass_mux_mask;
    unsigned int has_fractional_divider;
} imx8mp_pll_desc_t;


/* PLL information */

typedef struct {
    unsigned long reference_frequency;
    unsigned long configured_frequency;
    unsigned long output_frequency;
    unsigned long effective_frequency;

    /*
     * Compatibility field.
     * Equivalent to effective_frequency.
     */
    unsigned long frequency;
    unsigned int reference_source;
    unsigned int reference_valid;
    unsigned int configured_valid;
    unsigned int valid;
    unsigned int locked;
    unsigned int bypassed;
    unsigned int internal_bypass;
    unsigned int output_bypass;
    unsigned int enabled;
    unsigned int clke;
    unsigned int reset;
    unsigned int mdiv;
    unsigned int pdiv;
    unsigned int sdiv;
    int kdiv;
    unsigned int gen_ctrl;
    unsigned int fdiv_ctl0;
    unsigned int fdiv_ctl1;
    unsigned int lock_ctrl;
} imx8mp_pll_info_t;


/* PLL descriptors */

extern const imx8mp_pll_desc_t imx8mp_pll_descriptions[];
extern const unsigned int imx8mp_pll_description_count;


/* Default reference configuration */

extern const imx8mp_pll_reference_t imx8mp_pll_default_reference;


/* PLL API */

int imx8mp_pll_get_info_ex(
    unsigned long base,
    imx8mp_pll_type_t type,
    const imx8mp_pll_reference_t *reference,
    imx8mp_pll_info_t *info
);


/* Compatibility API */

int imx8mp_pll_get_info(
    unsigned long base,
    imx8mp_pll_type_t type,
    imx8mp_pll_info_t *info
);


unsigned long imx8mp_pll_get_frequency(
    unsigned long base,
    imx8mp_pll_type_t type
);

int imx8mp_pll_validate_descriptors(void);

#endif
