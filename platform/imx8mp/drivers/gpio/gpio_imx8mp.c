#include "gpio_imx8mp.h"

#define GPIO_REG(base, offset) (*(volatile uint32_t *)((base) + (offset)))

void gpio_init(void) {
    /* Nothing required globally for now. */
}

void gpio_set_direction(uintptr_t base, unsigned int pin, gpio_direction_t direction) {
    if (pin >= 32)
        return;

    uint32_t value = GPIO_REG(base, GPIO_GDIR);

    if (direction == GPIO_OUTPUT)
        value |= (1U << pin);
    else
        value &= ~(1U << pin);

    GPIO_REG(base, GPIO_GDIR) = value;
}

void gpio_write(uintptr_t base, unsigned int pin, unsigned int value) {
    if (pin >= 32)
        return;

    uint32_t reg = GPIO_REG(base, GPIO_DR);

    if (value)
        reg |= (1U << pin);
    else
        reg &= ~(1U << pin);

    GPIO_REG(base, GPIO_DR) = reg;
}

unsigned int gpio_read(uintptr_t base, unsigned int pin) {
    if (pin >= 32)
        return 0;

    uint32_t gdir = GPIO_REG(base, GPIO_GDIR);
    uint32_t offset = (gdir & (1U << pin))
                    ? GPIO_DR
                    : GPIO_PSR;

    return (GPIO_REG(base, offset) >> pin) & 1U;
}
