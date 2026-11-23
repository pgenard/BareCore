#include "gic.h"
#include "uart_imx8.h"
#include "gpt.h"
#include "string.h"
#include "irq.h"
#include "systime.h"
#include "scheduler.h"
#include "imx8mp_clock.h"
#include "shell.h"
#include "gpio_imx8mp.h"
#include "soft_uart.h"

#define UART1_GIC_ID 58
#define GPT1_GIC_ID 87

static inline void route_irq_to_el2(void) {
    unsigned long hcr;

    asm volatile(
        "mrs %0, HCR_EL2"
        : "=r"(hcr));

    hcr |= (1 << 4);   /* IMO */

    asm volatile(
        "msr HCR_EL2, %0\n"
        "isb"
        :
        : "r"(hcr));
}

int main(void) {
    uart_init();
    gpt_init();
    gpio_init();
    soft_uart_init();

    imx8mp_clock_init();

    gpt_enable_irq();
    route_irq_to_el2();
    
    gic_init();

    irq_register(UART1_GIC_ID, uart_irq_handler);
    irq_register(GPT1_GIC_ID, gpt_irq_handler);

    asm volatile(
        "msr daifclr, #2\n"
        "isb"
    );

    uart_write("[i.MX8MP Board Emulated by QEMU]\n");

    scheduler_init();
    scheduler_add_task(shell_task, 1);

    uart_write("Command> ");

    scheduler_run();

    return 0;
}
