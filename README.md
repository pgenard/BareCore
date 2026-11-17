# BareCore from Silicon to Software on Emulated i.MX8MP with QEMU.

## About

**BareCore** demonstrates modular bare-metal software development on an **ARMv8-A** processor with a cooperative scheduler. The project configures the **Generic Interrupt Controller** (**GICv3**), **NXP i.MX8 UART** to receive interrupts, the **i.MX8MP General Purpose Timer** (**GPT**) and **i.MX8MP Clock Tree Description**.

A simple command-line interface illustrates how UART interrupts, exception handling, timer configuration and low-level hardware access work together in a minimal bare-metal environment.

### Features
- Bare-metal ARMv8-A startup code,
- EL2 exception vector table,
- GICv3 initialization and interrupt routing,
- Interrupt-driven UART receive,
- Circular receive buffer,
- Polling-based UART transmit,
- Simple serial command-line interface,
- Memory inspection with debug command,
- 16 MHz GPT clock source configuration,
- GPT timer delay,
- Software IRQ handler,
- Uptime command,
- Cooperative scheduler,
- Idle state,
- Clock tree description.

### Hints

Our QEMU emulated platform page is available here: [i.MX8MP](https://www.qemu.org/docs/master/system/arm/imx8m.html)

Our DDR base is mapped at *0x40000000* address.

Our UART base register is mapped at *0x30860000* address.

### Toolchain

This project uses the ARM GNU bare-metal toolchain: [ARM Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

## Makefile

### Compiling
```bash
$ make
...
./aarch64-none-elf-as -mcpu=cortex-a53 main.s -o main.o
./aarch64-none-elf-as -mcpu=cortex-a53 arch/armv8/startup.s -o arch/armv8/startup.o
./aarch64-none-elf-as -mcpu=cortex-a53 arch/armv8/vectors.s -o arch/armv8/vectors.o
./aarch64-none-elf-as -mcpu=cortex-a53 arch/armv8/exceptions.s -o arch/armv8/exceptions.o
./aarch64-none-elf-ld -T linker.ld -o hello.elf shell/shell.o shell/shell_commands.o kernel/irq/irq.o kernel/irq/gic.o kernel/scheduler/scheduler.o kernel/time/systime.o platform/imx8mp/drivers/gpt/gpt.o platform/imx8mp/drivers/uart/uart_imx8.o platform/imx8mp/drivers/watchdog/watchdog.o platform/imx8mp/clock/imx8mp_clock.o platform/imx8mp/clock/imx8mp_clock_root.o platform/imx8mp/clock/imx8mp_clock_pll.o platform/imx8mp/clock/imx8mp_clock_pll_desc.o platform/imx8mp/clock/imx8mp_clock_parent.o platform/imx8mp/clock/imx8mp_clock_diag.o lib/string.o main.o arch/armv8/startup.o arch/armv8/vectors.o arch/armv8/exceptions.o
./aarch64-none-elf-objcopy -O binary hello.elf hello.bin
```

### Cleaning
```bash
$ make clean
...
'lib/string.d' supprimé
'main.d' supprimé
'hello.elf' supprimé
'hello.bin' supprimé
```

## Executing our Serial Shell
```bash
$ ./qemu/build/qemu-system-aarch64 -M imx8mp-evk -m 512M -kernel hello.bin -display none -serial stdio -monitor telnet:127.0.0.1:1234,server,nowait
[i.MX8MP Board Emulated by QEMU]
Command> hello
Hello !
Command> reboot
Rebooting ...
[i.MX8MP Board Emulated by QEMU]
Command> aaaaaaaaaaaaaaaaaaaa
Unknown command
Command> a
Unknown command
Command> hello
Hello !
Command> uname
[Bare-Metal i.MX8MP by Pierre GENARD (PGEN) #10]
  UART Driver with RX Interrupts
  UART Driver with TX Polling
  UART Circular Buffer
  GICv3 Init
  Basic Shell with Debug Commands
  Timer Driver with Interrupts
  Reboot with Watchdog Timer
  Uptime Command
  Cooperative Scheduler
  Clock Tree Description
  Idle State (Low Power)
Command> help
Commands:
  clock
  gpt <seconds>
  hello
  help
  idle
  md <address>
  reboot
  time
  uname
Command> gpt 1
Waiting 1s ...
On Time !
Command>
```

### Debugging
```bash
$ telnet localhost 1234
```

### Notes
This **BareCore** version provides modularity and fixes several bugs.

## Author
**Pierre GENARD**
