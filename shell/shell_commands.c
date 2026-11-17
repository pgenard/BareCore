#include "shell_commands.h"
#include "uart_imx8.h"
#include "gpt.h"
#include "string.h"
#include "systime.h"
#include "imx8mp_clock.h"
#include "scheduler.h"

#define GPT1_BASE 0x302D0000UL
#define GICD_BASE 0x38800000UL

#define GPT_PR   (*(volatile unsigned int *)(GPT1_BASE + 0x04))
#define GPT_IR   (*(volatile unsigned int *)(GPT1_BASE + 0x0C))
#define GPT_CR   (*(volatile unsigned int *)(GPT1_BASE + 0x00))
#define GPT_SR   (*(volatile unsigned int *)(GPT1_BASE + 0x08))
#define GPT_CNT  (*(volatile unsigned int *)(GPT1_BASE + 0x24))
#define GPT_OCR1 (*(volatile unsigned int *)(GPT1_BASE + 0x10))

#define GICD_ISENABLER(n) (*(volatile unsigned int *)(GICD_BASE + 0x100 + (((n) / 32) * 4)))
#define GICD_IGROUPR(n)   (*(volatile unsigned int *)(GICD_BASE + 0x080 + (((n) / 32) * 4)))
#define GICD_IROUTER(n)   (*(volatile unsigned long long *)(GICD_BASE + 0x6000 + ((n) * 8)))

#define GICD_CTLR         (*(volatile unsigned int *)(GICD_BASE + 0x0000))

#define GPT1_GIC_ID 87

static void command_help(void) {
    uart_write("Commands:\n");
    uart_write("  clock\n");
    uart_write("  gpt <seconds>\n");
    uart_write("  hello\n");
    uart_write("  help\n");
    uart_write("  idle\n");
    uart_write("  md <address>\n");
    uart_write("  reboot\n");
    uart_write("  time\n");
    uart_write("  uname\n");
}

static void command_uname(void) {
    uart_write("[BareCore by Pierre GENARD (PGEN) #11]\n");
    uart_write("  UART Driver with RX Interrupts\n");
    uart_write("  UART Driver with TX Polling\n");
    uart_write("  UART Circular Buffer\n");
    uart_write("  GICv3 Init\n");
    uart_write("  Basic Shell with Debug Commands\n");
    uart_write("  Timer Driver with Interrupts\n");
    uart_write("  Reboot with Watchdog Timer\n");
    uart_write("  Uptime Command\n");
    uart_write("  Cooperative Scheduler\n");
    uart_write("  IDLE State (Low Power)\n");
    uart_write("  Clock Tree Description\n");
}

void command_idle(void) {
    unsigned long entries = scheduler_get_idle_count();
    unsigned long wakeups = scheduler_get_idle_wake_count();

    uart_write("Idle entries : ");
    uart_print_dec(entries);
    uart_write("\n");

    uart_write("Idle wakeups : ");
    uart_print_dec(wakeups);
    uart_write("\n");
}

static void command_hello(void) {
    uart_write("Hello !\n");
}

static void command_unknown(void) {
    uart_write("Unknown command\n");
}

static void command_md(unsigned long address) {
    volatile unsigned int *ptr = (volatile unsigned int*) address;

    uart_print_hex((unsigned int)address);
    uart_write(": ");
    uart_print_hex(*ptr);
    uart_write("\n");
}

static void command_gpt(unsigned int seconds) {
    if (seconds == 0) {
        uart_write("Waiting 0s ...\n");
        uart_write("On Time !\n");
        return;
    }

    uart_write("Waiting ");
    uart_print_dec(seconds);
    uart_write("s ...\n");

    gpt_delay_seconds(seconds);

    uart_write("On Time !\n");
}

static void command_gpt_status(void) {
    uart_write("GPT_PR=");
    uart_print_hex(GPT_PR);
    uart_write("\n");

    uart_write("GPT_IR=");
    uart_print_hex(GPT_IR);
    uart_write("\n");

    uart_write("GPT_CR=");
    uart_print_hex(GPT_CR);
    uart_write("\n");

    uart_write("GPT_SR=");
    uart_print_hex(GPT_SR);
    uart_write("\n");

    uart_write("GPT_CNT=");
    uart_print_hex(GPT_CNT);
    uart_write("\n");

    uart_write("GPT_OCR1=");
    uart_print_hex(GPT_OCR1);
    uart_write("\n");
}

static void command_gpt_gic(void) {
    unsigned int n = GPT1_GIC_ID;

    uart_write("GIC ISENABLER=");
    uart_print_hex(GICD_ISENABLER(n));
    uart_write("\n");

    uart_write("GIC IGROUPR=");
    uart_print_hex(GICD_IGROUPR(n));
    uart_write("\n");

    uart_write("GIC IROUTER=");
    uart_print_hex((unsigned int)GICD_IROUTER(n));
    uart_write("\n");

    uart_write("GICD_CTLR=");
    uart_print_hex(GICD_CTLR);
    uart_write("\n");
}

static void command_gpt_pending(void) {
    unsigned int reg;
    unsigned int bit;

    reg = *(volatile unsigned int *)
        (GICD_BASE + 0x200 + ((GPT1_GIC_ID / 32) * 4));

    bit = 1u << (GPT1_GIC_ID % 32);

    uart_write("ISPENDR=");
    uart_print_hex(reg);

    uart_write(" bit=");
    uart_print_hex(bit);

    uart_write("\n");
}

static void command_time(void) {
    uart_write("systime = ");
    uart_print_dec(systime_get());
    uart_write(" ms\n");
}

void shell_command_execute(const char *cmd) {
    if (!strcmp(cmd, "help")) {
        command_help();
    }
    else if (!strcmp(cmd, "clock")) {
        imx8mp_clock_dump();
    }
    else if (!strcmp(cmd, "gicgpt")) {
        command_gpt_gic();
    }
    else if (!strncmp(cmd, "gpt ", 4)) {
        unsigned int seconds;

        seconds = strtoul_dec(cmd + 4);

        command_gpt(seconds);
    }
    else if (!strcmp(cmd, "gptp")) {
        command_gpt_pending();
    }
    else if (!strcmp(cmd, "gpts")) {
        command_gpt_status();
    }
    else if (!strcmp(cmd, "hello")) {
        command_hello();
    }
    else if (!strncmp(cmd, "md ", 3)) {
        unsigned long addr;

        addr = strtoul_hex(cmd + 3);

        command_md(addr);
    }
    else if (!strcmp(cmd, "time")) {
        command_time();
    }
    else if (!strcmp(cmd, "uname")) {
        command_uname();
    }
    else if (!strcmp(cmd, "idle")) {
        command_idle();
    }
    else {
        command_unknown();
    }
}
