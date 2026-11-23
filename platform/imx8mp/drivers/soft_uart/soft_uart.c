#include "soft_uart.h"
#include "gpio_imx8mp.h"
#include "gpt.h"

#define SOFT_UART_TX_BASE GPIO1_BASE
#define SOFT_UART_TX_PIN  0

#define SOFT_UART_BAUDRATE 115200U
#define GPT_FREQUENCY     16000000U

#define BIT_TICKS_NUMERATOR GPT_FREQUENCY
#define BIT_TICKS_DENOMINATOR SOFT_UART_BAUDRATE

static uint32_t bit_fraction;

static void soft_uart_delay_bit(void) {
    uint32_t ticks = GPT_FREQUENCY / SOFT_UART_BAUDRATE;

    bit_fraction += GPT_FREQUENCY % SOFT_UART_BAUDRATE;

    if (bit_fraction >= SOFT_UART_BAUDRATE) {
        ticks++;
        bit_fraction -= SOFT_UART_BAUDRATE;
    }

    uint32_t start = gpt_read_cnt();

    while ((uint32_t)(gpt_read_cnt() - start) < ticks) {
        __asm__ volatile("nop");
    }
}

void soft_uart_init(void) {
    bit_fraction = 0;

    gpio_set_direction(
        SOFT_UART_TX_BASE,
        SOFT_UART_TX_PIN,
        GPIO_OUTPUT
    );

    /* UART idle state */
    gpio_write(SOFT_UART_TX_BASE, SOFT_UART_TX_PIN, 1);
}

void soft_uart_putc(char c) {
    /* Start bit */
    gpio_write(SOFT_UART_TX_BASE, SOFT_UART_TX_PIN, 0);
    soft_uart_delay_bit();

    /* Eight data bits, LSB first */
    for (unsigned int i = 0; i < 8; i++) {
        gpio_write(
            SOFT_UART_TX_BASE,
            SOFT_UART_TX_PIN,
            (c >> i) & 1
        );

        soft_uart_delay_bit();
    }

    /* Stop bit */
    gpio_write(SOFT_UART_TX_BASE, SOFT_UART_TX_PIN, 1);
    soft_uart_delay_bit();
}

void soft_uart_puts(const char *str) {
    while (*str) {
        soft_uart_putc(*str++);
    }
}
