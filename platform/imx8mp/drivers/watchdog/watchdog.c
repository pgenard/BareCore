#include "watchdog.h"
#include "uart_imx8.h"

#define WDOG1_BASE 0x30280000UL

#define WDOG_WCR (*(volatile unsigned short *)(WDOG1_BASE + 0x00))

#define WDOG_REBOOT_VALUE 0x0014

void watchdog_reboot(void) {
    uart_write("Rebooting ...\r\n");

    WDOG_WCR = WDOG_REBOOT_VALUE;

    while (1)
    {
        asm volatile("wfi");
    }
}
