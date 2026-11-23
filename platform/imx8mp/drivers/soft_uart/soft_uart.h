#ifndef SOFT_UART_H
#define SOFT_UART_H

#include "stdint.h"

void soft_uart_init(void);
void soft_uart_putc(char c);
void soft_uart_puts(const char *str);

#endif
