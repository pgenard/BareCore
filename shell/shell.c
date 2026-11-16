#include "shell.h"
#include "shell_commands.h"
#include "uart_imx8.h"
#include "watchdog.h"
#include "string.h"

#define SHELL_BUFFER_SIZE 128

static char buffer[SHELL_BUFFER_SIZE];

static unsigned int pos = 0;

static void shell_reset_line(void) {
    buffer[0] = '\0';
    pos = 0;
}

static void shell_execute(void) {
    buffer[pos] = '\0';

    if (pos > 0)
    {
        if (!strcmp(buffer, "reboot"))
        {
            shell_reset_line();
            watchdog_reboot();
            return;
        }

        shell_command_execute(buffer);
    }

    shell_reset_line();
    uart_write("Command> ");
}

void shell_init(void) {
    shell_reset_line();
}

void shell_task(void) {
    unsigned char c;

    if (!uart_rx_available())
        return;

    c = uart_getchar();

    if (c == '\r') {
        uart_putchar('\n');
        shell_execute();
        return;
    }

    if (c == '\b' || c == 0x7F) {
        if (pos > 0) {
            pos--;
            uart_write("\b \b");
        }

        return;
    }

    if (pos < SHELL_BUFFER_SIZE - 1) {
        buffer[pos++] = c;
        uart_putchar(c);
    }
}
