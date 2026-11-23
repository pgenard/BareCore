CROSS = ./aarch64-none-elf-

CC      = $(CROSS)gcc
AS      = $(CROSS)as
LD      = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy

TARGET = hello

CFLAGS = -mcpu=cortex-a53 -ffreestanding -Wall -Werror -Wextra -MMD -MP \
         -Iinclude -Ilib -Ishell -Ikernel/irq \
         -Ikernel/scheduler -Ikernel/time \
         -Iplatform/imx8mp -Iplatform/imx8mp/clock \
         -Iplatform/imx8mp/drivers/uart \
         -Iplatform/imx8mp/drivers/gpt \
	 -Iplatform/imx8mp/drivers/watchdog \
         -Iplatform/imx8mp/drivers/gpio \
         -Iplatform/imx8mp/drivers/soft_uart

ASFLAGS = -mcpu=cortex-a53
LDFLAGS = -T linker.ld

C_SRCS = shell/shell.c \
	 shell/shell_commands.c \
         kernel/irq/irq.c \
         kernel/irq/gic.c \
         kernel/scheduler/scheduler.c \
         kernel/time/systime.c \
         platform/imx8mp/drivers/gpt/gpt.c \
         platform/imx8mp/drivers/gpio/gpio_imx8mp.c \
         platform/imx8mp/drivers/uart/uart_imx8.c \
         platform/imx8mp/drivers/soft_uart/soft_uart.c \
         platform/imx8mp/drivers/watchdog/watchdog.c \
         platform/imx8mp/clock/imx8mp_clock.c \
         platform/imx8mp/clock/imx8mp_clock_root.c \
         platform/imx8mp/clock/imx8mp_clock_pll.c \
         platform/imx8mp/clock/imx8mp_clock_pll_desc.c \
         platform/imx8mp/clock/imx8mp_clock_parent.c \
         platform/imx8mp/clock/imx8mp_clock_diag.c \
         lib/string.c \
         main.c

ASM_SRCS = arch/armv8/startup.s \
           arch/armv8/vectors.s \
           arch/armv8/exceptions.s

C_ASM = $(C_SRCS:.c=.s)

C_OBJS = $(C_SRCS:.c=.o)
ASM_OBJS = $(ASM_SRCS:.s=.o)

OBJS = $(C_OBJS) $(ASM_OBJS)
DEPS = $(C_OBJS:.o=.d)

all: $(TARGET).bin

# C -> Assembly
$(C_ASM): %.s: %.c
	$(CC) $(CFLAGS) -MF $(@:.s=.d) -MT $(@:.s=.o) -S $< -o $@

# Generated assembly -> Object
$(C_OBJS): %.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Handwritten assembly -> Object
$(ASM_OBJS): %.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

$(TARGET).elf: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -v $(C_ASM) $(OBJS) $(DEPS) $(TARGET).elf $(TARGET).bin

-include $(DEPS)

.PHONY: all clean
