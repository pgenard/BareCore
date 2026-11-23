#!/usr/bin/env python3

import re
import sys

trace_file = sys.argv[1] if len(sys.argv) > 1 else "gpio-trace.log"

values = []

with open(trace_file) as f:
    for line in f:
        if "imx_gpio_write" not in line:
            continue

        if "/machine/soc/gpio1:[DR]" not in line:
            continue

        match = re.search(r"<- 0x([0-9a-fA-F]+)", line)

        if match:
            value = int(match.group(1), 16)
            values.append(value & 1)

# Find first start bit.
try:
    start = values.index(0)
except ValueError:
    print("No UART start bit found.")
    sys.exit(1)

values = values[start:]

decoded = bytearray()

while len(values) >= 10:

    frame = values[:10]
    values = values[10:]

    if frame[0] != 0:
        print("Invalid start bit:", frame)
        continue

    if frame[9] != 1:
        print("Invalid stop bit:", frame)
        continue

    data = 0

    for i in range(8):
        data |= frame[i + 1] << i

    decoded.append(data)

print("Decoded bytes:", decoded)
print("Decoded text :", decoded.decode("ascii", errors="replace"))
