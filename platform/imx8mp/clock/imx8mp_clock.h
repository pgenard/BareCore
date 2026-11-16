#ifndef IMX8MP_CLOCK_H
#define IMX8MP_CLOCK_H

#include "imx8mp_clock_regs.h"
#include "imx8mp_clock_pll.h"
#include "imx8mp_clock_parent.h"

/* Clock root register fields */

#define ROOT_MUX_SHIFT  24
#define ROOT_MUX_MASK   (0x7U << ROOT_MUX_SHIFT)

#define ROOT_PRE_SHIFT  16
#define ROOT_PRE_MASK   (0x7U << ROOT_PRE_SHIFT)

#define ROOT_POST_SHIFT 0
#define ROOT_POST_MASK  0x3FU

#define ROOT_ENABLE     (1U << 28)

/*
 * ROOT_MUX_GET, ROOT_PRE_GET and ROOT_POST_GET
 * are already defined in imx8mp_clock_regs.h.
 */

/* Clock root information */

typedef struct {
    unsigned int mux;
    unsigned int pre_div;
    unsigned int post_div;
    unsigned int enabled;
    unsigned int valid;
    unsigned int raw_register;
    unsigned long parent_frequency;
    unsigned long frequency;
} imx8mp_root_info_t;

/* Clock root API */

int imx8mp_clock_root_get(
    unsigned long offset,
    const unsigned long *parents,
    unsigned int parent_count,
    imx8mp_root_info_t *info
);

/* Clock initialization */

void imx8mp_clock_init(void);

/* Clock diagnostics */

void imx8mp_clock_dump(void);

#endif
