#ifndef IMX8MP_CLOCK_REGS_H
#define IMX8MP_CLOCK_REGS_H

#include "stdint.h"

#define IMX8MP_CCM_BASE       0x30380000UL
#define IMX8MP_ANALOG_BASE    0x30360000UL

#define IMX8MP_CCM_SIZE       0x10000UL
#define IMX8MP_ANALOG_SIZE    0x10000UL

#define IMX8MP_CCM_MAX_OFFSET 0xC6FCU

#define IMX8MP_OSC_FREQ       24000000UL

/* PLL base offsets */

#define PLL_AUDIO1_BASE      0x000
#define PLL_AUDIO2_BASE      0x014
#define PLL_VIDEO1_BASE      0x028
#define PLL_DRAM_BASE        0x050

#define PLL_GPU_BASE         0x064
#define PLL_VPU_BASE         0x074
#define PLL_ARM_BASE         0x084

#define PLL_SYS1_BASE        0x094
#define PLL_SYS2_BASE        0x104
#define PLL_SYS3_BASE        0x114

/* PLL register offsets */

#define PLL_GEN_CTRL             0x00U
#define PLL_FDIV_CTL0             0x04U
#define PLL_FDIV_CTL1             0x08U
#define PLL_LOCK_CTRL             0x08U

/* GEN_CTRL fields */

#define PLL_LOCK                 (1U << 31)

#define PLL_LOCK_SEL             (1U << 29)

#define PLL_EXT_BYPASS           (1U << 28)

#define PLL_CLKE                 (1U << 11)
#define PLL_CLKE_OVERRIDE        (1U << 10)

#define PLL_RST                  (1U << 9)
#define PLL_RST_OVERRIDE         (1U << 8)

#define PLL_BYPASS                (1U << 4)

/* Reference selection */

#define PLL_REF_CLK_SEL_SHIFT    0U
#define PLL_REF_CLK_SEL_MASK     (3U << PLL_REF_CLK_SEL_SHIFT)

#define PLL_REF_CLK_OSC24M       0U

/* Output bypass mux */

#define PLL_OUTPUT_BYPASS_MUX    (1U << 16)

/* Divider fields */

#define PLL_MDIV_SHIFT           12U
#define PLL_MDIV_MASK            (0x3FFU << PLL_MDIV_SHIFT)

#define PLL_PDIV_SHIFT           4U
#define PLL_PDIV_MASK            (0x3FU << PLL_PDIV_SHIFT)

#define PLL_SDIV_SHIFT           0U
#define PLL_SDIV_MASK            0x7U

#define PLL_KDIV_MASK            0xFFFFU

/* CCM */

#define CCM_A53_ROOT        0x8000
#define CCM_UART1_ROOT      0xAF00
#define CCM_GPT1_ROOT       0xB580

#define CCM_ROOT_BASE       0x8000
#define CCM_ROOT_STRIDE     0x80

#define ROOT_MUX_SHIFT      24
#define ROOT_MUX_MASK       (0x7U << ROOT_MUX_SHIFT)

#define ROOT_PRE_SHIFT      16
#define ROOT_PRE_MASK       (0x7U << ROOT_PRE_SHIFT)

#define ROOT_POST_SHIFT     0
#define ROOT_POST_MASK      0x3FU

#define ROOT_ENABLE         (1U << 28)

#define ROOT_MUX_GET(reg) \
    (((reg) & ROOT_MUX_MASK) >> ROOT_MUX_SHIFT)

#define ROOT_PRE_GET(reg) \
    (((reg) & ROOT_PRE_MASK) >> ROOT_PRE_SHIFT)

#define ROOT_POST_GET(reg) \
    ((reg) & ROOT_POST_MASK)

/*
 * Basic MMIO access
 *
 * Shared by all clock drivers.
 */

#define REG32(addr) \
    (*(volatile uint32_t *)(uintptr_t)(addr))

static inline uint32_t clock_read32(unsigned long addr) {
    return REG32(addr);
}

static inline void clock_write32(unsigned long addr, uint32_t value) {
    REG32(addr) = value;
}

#endif
