#include <cstdio>

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "pins.h"

static constexpr uint8_t AS5600_ADDR = 0x36;
static constexpr uint8_t REG_STATUS = 0x0B;
static constexpr uint8_t REG_RAW_ANGLE_H = 0x0C;
static constexpr uint8_t REG_ANGLE_H = 0x0E;
static constexpr uint32_t I2C_BAUD = 100 * 1000;

struct As5600Sample {
    int raw;
    int filtered;
    bool magnet_detected;
    bool magnet_too_weak;
    bool magnet_too_strong;
    bool ok;
};

static bool as5600_read_bytes(uint8_t reg, uint8_t* buf, size_t len) {
    if (i2c_write_blocking(i2c0, AS5600_ADDR, &reg, 1, true) != 1) {
        return false;
    }
    return i2c_read_blocking(i2c0, AS5600_ADDR, buf, len, false) == static_cast<int>(len);
}

static int as5600_read_angle12(uint8_t reg_h) {
    uint8_t buf[2] = {0, 0};
    if (!as5600_read_bytes(reg_h, buf, sizeof(buf))) {
        return -1;
    }
    return ((buf[0] & 0x0F) << 8) | buf[1];
}

static As5600Sample as5600_read_sample(void) {
    As5600Sample sample = {
        .raw = -1,
        .filtered = -1,
        .magnet_detected = false,
        .magnet_too_weak = false,
        .magnet_too_strong = false,
        .ok = false,
    };

    uint8_t status = 0;
    if (!as5600_read_bytes(REG_STATUS, &status, 1)) {
        return sample;
    }

    sample.magnet_detected = (status & (1u << 5)) != 0;
    sample.magnet_too_weak = (status & (1u << 4)) != 0;
    sample.magnet_too_strong = (status & (1u << 3)) != 0;

    sample.raw = as5600_read_angle12(REG_RAW_ANGLE_H);
    sample.filtered = as5600_read_angle12(REG_ANGLE_H);
    sample.ok = sample.raw >= 0 && sample.filtered >= 0;
    return sample;
}

static void i2c_scan(void) {
    printf("I2C scan (0x03-0x77):\n");
    int found = 0;

    for (uint8_t addr = 0x03; addr <= 0x77; addr++) {
        uint8_t dummy = 0;
        if (i2c_read_blocking(i2c0, addr, &dummy, 1, false) >= 0) {
            printf("  0x%02X\n", addr);
            found++;
        }
    }

    if (found == 0) {
        printf("  (no devices)\n");
    }
}

static void init_i2c(void) {
    i2c_init(i2c0, I2C_BAUD);
    gpio_set_function(PIN_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_I2C_SDA);
    gpio_pull_up(PIN_I2C_SCL);
}

int main(void) {
    stdio_init_all();
    sleep_ms(1500);

    init_i2c();
    i2c_scan();

    printf("AS5600 reader ready (I2C0 GP%d/GP%d, USB CDC)\n", PIN_I2C_SDA, PIN_I2C_SCL);
    printf("cols: raw filtered deg status\n");

    while (true) {
        As5600Sample sample = as5600_read_sample();

        if (!sample.ok) {
            printf("I2C read error\n");
        } else {
            const float deg = sample.raw * 360.0f / 4096.0f;
            char status = 'O';

            if (!sample.magnet_detected) {
                status = 'N';  // no magnet
            } else if (sample.magnet_too_weak) {
                status = 'L';  // magnet too low
            } else if (sample.magnet_too_strong) {
                status = 'H';  // magnet too high
            }

            printf("%4d %4d %7.2f %c\n", sample.raw, sample.filtered, deg, status);
        }

        sleep_ms(50);
    }
}
