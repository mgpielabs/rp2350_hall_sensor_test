# rp2350_hall_sensor_test

RP2350 + AS5600 (I2C0 GP0/GP1), USB CDC output.

## Build

```bash
cmake -S . -B build -DPICO_BOARD=pico2
cmake --build build -j
cp build/as5600_usb.uf2 /media/$USER/RP2350/
```

## Demo

![demo](docs/demo.gif)

https://asciinema.org/a/H9wRiLPgU8JF6Ybl

## Re-record

```bash
./tools/record.sh
UPLOAD=1 GIF=1 ./tools/record.sh
```
