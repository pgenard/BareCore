# 🖥️🚀 BareCore from Silicon to Software on Emulated i.MX8MP with QEMU.

## 📖 About

**BareCore** demonstrates modular bare-metal software development on an **ARMv8-A** processor with a cooperative scheduler. The project configures the **Generic Interrupt Controller** (**GICv3**), **NXP i.MX8 UART** to receive interrupts, the **i.MX8MP General Purpose Timer** (**GPT**) and **i.MX8MP Clock Tree** Description along with **i.MX8MP GPIO** test.

A simple command-line interface illustrates how UART interrupts, exception handling, timer configuration and low-level hardware access work together in a minimal bare-metal environment.

### ✨ Features
- 🧠 Bare-metal ARMv8-A startup code,
- ⚡ EL2 exception vector table,
- 🔀 GICv3 initialization and interrupt routing,
- 📡 Interrupt-driven UART reception,
- 🔄 Circular receive buffer,
- 📤 Polling-based UART transmission,
- 💻 Serial command-line interface,
- 🔍 Memory inspection and debugging commands,
- ⏱️ 16 MHz GPT clock source configuration,
- ⏳ GPT timer delays,
- ⚡ Software IRQ handler,
- 🕒 Uptime command,
- 🔄 Cooperative scheduler,
- 💤 IDLE state,
- 🎛️ Clock tree description,
- 🔌 GPIO configuration and testing,
- 📟 Software UART transmission through GPIO.

### 💡 Hints

Our QEMU emulated platform page is available here: [i.MX8MP](https://www.qemu.org/docs/master/system/arm/imx8m.html)

Our DDR base is mapped at *0x40000000* address.

Our UART base register is mapped at *0x30860000* address.

### 🛠️ Toolchain

This project uses the ARM GNU bare-metal toolchain: [ARM Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

## 🏗️ Makefile

### 🔨 Compiling
```bash
$ make
...
./aarch64-none-elf-as -mcpu=cortex-a53 main.s -o main.o
./aarch64-none-elf-as -mcpu=cortex-a53 arch/armv8/startup.s -o arch/armv8/startup.o
./aarch64-none-elf-as -mcpu=cortex-a53 arch/armv8/vectors.s -o arch/armv8/vectors.o
./aarch64-none-elf-as -mcpu=cortex-a53 arch/armv8/exceptions.s -o arch/armv8/exceptions.o
./aarch64-none-elf-ld -T linker.ld -o hello.elf shell/shell.o shell/shell_commands.o kernel/irq/irq.o kernel/irq/gic.o kernel/scheduler/scheduler.o kernel/time/systime.o platform/imx8mp/drivers/gpt/gpt.o platform/imx8mp/drivers/gpio/gpio_imx8mp.o platform/imx8mp/drivers/uart/uart_imx8.o platform/imx8mp/drivers/soft_uart/soft_uart.o platform/imx8mp/drivers/watchdog/watchdog.o platform/imx8mp/clock/imx8mp_clock.o platform/imx8mp/clock/imx8mp_clock_root.o platform/imx8mp/clock/imx8mp_clock_pll.o platform/imx8mp/clock/imx8mp_clock_pll_desc.o platform/imx8mp/clock/imx8mp_clock_parent.o platform/imx8mp/clock/imx8mp_clock_diag.o lib/string.o main.o arch/armv8/startup.o arch/armv8/vectors.o arch/armv8/exceptions.o
./aarch64-none-elf-objcopy -O binary hello.elf hello.bin
```

### 🧹 Cleaning
```bash
$ make clean
...
'lib/string.d' supprimé
'main.d' supprimé
'hello.elf' supprimé
'hello.bin' supprimé
```

## 🚀 Executing our Serial Shell
```bash
$ ./qemu/build/qemu-system-aarch64 -M imx8mp-evk -m 512M -kernel hello.bin -display none -serial stdio -monitor telnet:127.0.0.1:1234,server,nowait -trace enable=imx_gpio_write -msg timestamp=on
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
[Bare-Metal i.MX8MP by Pierre GENARD (PGEN) #11]
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
  IDLE State (Low Power)
  GPIO Test
Command> help
Commands:
  clock
  gpio
  gpt <seconds>
  hello
  help
  idle
  md <address>
  reboot
  softuart
  time
  uname
Command> gpt 1
Waiting 1s ...
On Time !
Command>
Command> gpio
GPIO1_IO00 diagnostic
2026-11-23T19:29:36.158472Z imx_gpio_write /machine/soc/gpio1:[GDIR] <- 0x1
2026-11-23T19:29:36.158657Z imx_gpio_write /machine/soc/gpio1:[DR] <- 0x1
HIGH: DR=00000001 GDIR=00000001
2026-11-23T19:29:36.659155Z imx_gpio_write /machine/soc/gpio1:[DR] <- 0x0
LOW:  DR=00000000
2026-11-23T19:29:37.159298Z imx_gpio_write /machine/soc/gpio1:[DR] <- 0x1
HIGH: DR=00000001 GDIR=00000001
2026-11-23T19:29:37.659361Z imx_gpio_write /machine/soc/gpio1:[DR] <- 0x0
LOW:  DR=00000000
2026-11-23T19:29:38.159393Z imx_gpio_write /machine/soc/gpio1:[DR] <- 0x1
HIGH: DR=00000001 GDIR=00000001
2026-11-23T19:29:38.659441Z imx_gpio_write /machine/soc/gpio1:[DR] <- 0x0
LOW:  DR=00000000
2026-11-23T19:29:39.159485Z imx_gpio_write /machine/soc/gpio1:[DR] <- 0x1
HIGH: DR=00000001 GDIR=00000001
2026-11-23T19:29:39.659534Z imx_gpio_write /machine/soc/gpio1:[DR] <- 0x0
LOW:  DR=00000000
2026-11-23T19:29:40.159567Z imx_gpio_write /machine/soc/gpio1:[DR] <- 0x1
HIGH: DR=00000001 GDIR=00000001
2026-11-23T19:29:40.659614Z imx_gpio_write /machine/soc/gpio1:[DR] <- 0x0
LOW:  DR=00000000
GPIO test completed.
```

### 🐛 Debugging
```bash
$ telnet localhost 1234
```

### 🔌 GPIO Traces

```text
$ -trace enable=imx_gpio_write -msg timestamp=on
```

### 📝 Notes
This **BareCore** version provides a GPIO test.

It is possible to decode the UART frames emitted through GPIO with the Python script.

## 👨‍💻 Author
**Pierre GENARD**
