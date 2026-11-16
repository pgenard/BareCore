#include "stdint.h"
#include "imx8mp_clock.h"
#include "imx8mp_clock_regs.h"

#define MMIO32(addr) (*(volatile uint32_t *)(uintptr_t)(addr))

uint32_t imx8mp_clock_read_ccm(uint32_t offset) {
    if ((offset & 3U) != 0)
        return 0;

    if (offset > IMX8MP_CCM_MAX_OFFSET)
        return 0;

    return MMIO32(IMX8MP_CCM_BASE + offset);
}

uint32_t imx8mp_clock_read_analog(uint32_t offset) {
    if ((offset & 3U) != 0)
        return 0;

    if (offset > 0x128U)
        return 0;

    return MMIO32(IMX8MP_ANALOG_BASE + offset);
}


void imx8mp_clock_init(void) {
    /*
     * This driver is intentionally read-only.
     *
     * Clock configuration is assumed to have been
     * performed by the bootloader or platform firmware.
     *
     * Do not reprogram PLLs while the CPU is executing
     * from clocks derived from those PLLs.
     */

    asm volatile("dsb sy");
    asm volatile("isb");
}
