#ifndef GPIO_IMX8MP_H
#define GPIO_IMX8MP_H

#include "stdint.h"

#define GPIO1_BASE 0x30200000UL
#define GPIO2_BASE 0x30210000UL
#define GPIO3_BASE 0x30220000UL
#define GPIO4_BASE 0x30230000UL
#define GPIO5_BASE 0x30240000UL

#define GPIO_DR   0x00
#define GPIO_GDIR 0x04
#define GPIO_PSR  0x08

typedef enum {
    GPIO_INPUT = 0,
    GPIO_OUTPUT = 1
} gpio_direction_t;

void gpio_init(void);

void gpio_set_direction(
    uintptr_t base,
    unsigned int pin,
    gpio_direction_t direction
);

void gpio_write(
    uintptr_t base,
    unsigned int pin,
    unsigned int value
);

unsigned int gpio_read(
    uintptr_t base,
    unsigned int pin
);

#endif
