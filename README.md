# rp2350_hall_sensor_test

RP2350 + AS5600 magnetic angle sensor over I2C0 (GP0/GP1), output via USB CDC.

## Build

```bash
cmake -S . -B build -DPICO_BOARD=pico2
cmake --build build -j
cp build/as5600_usb.uf2 /media/$USER/RP2350/
```

## Demo

[![asciicast](https://asciinema.org/a/H9wRiLPgU8JF6Ybl.svg)](https://asciinema.org/a/H9wRiLPgU8JF6Ybl)

## Record terminal demo

```bash
asciinema rec -c "stty -F /dev/ttyACM0 115200 raw -echo && timeout 12 cat /dev/ttyACM0" \
  -t "AS5600 hall sensor test on RP2350" --overwrite docs/as5600.cast
asciinema upload docs/as5600.cast
```
