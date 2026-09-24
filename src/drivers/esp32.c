#include "esp32.h"

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define REG_ATTITUDE      0x10
#define ATTITUDE_LSB_DEG  0.01

static int i2c_fd = -1;

static int i2c_read_block(uint8_t reg, uint8_t *buf, size_t len) {
    if (i2c_fd < 0) return -1;

    if (write(i2c_fd, &reg, 1) != 1) {
        return -1;
    }
    if (read(i2c_fd, buf, len) != (ssize_t)len) {
        return -1;
    }
    return 0;
}

int esp32_init(int endereco_i2c) {
    if (endereco_i2c <= 0) {
        endereco_i2c = ESP32_I2C_ADDR_DEFAULT;
    }

    i2c_fd = open("/dev/i2c-1", O_RDWR);
    if (i2c_fd < 0) {
        perror("[ESP32] Falha ao abrir /dev/i2c-1");
        return 1;
    }

    if (ioctl(i2c_fd, I2C_SLAVE, endereco_i2c) < 0) {
        perror("[ESP32] Falha ao selecionar endereco escravo I2C");
        close(i2c_fd);
        i2c_fd = -1;
        return 1;
    }

    return 0;
}

void esp32_close(void) {
    if (i2c_fd >= 0) {
        close(i2c_fd);
        i2c_fd = -1;
    }
}

int esp32_read_attitude(double *roll_deg, double *pitch_deg, double *yaw_deg) {
    uint8_t buf[6];
    int16_t roll_raw, pitch_raw, yaw_raw;

    if (i2c_read_block(REG_ATTITUDE, buf, sizeof buf) != 0) {
        return -1;
    }
// verificar essa parte
    roll_raw  = (int16_t)((buf[0] << 8) | buf[1]);
    pitch_raw = (int16_t)((buf[2] << 8) | buf[3]);
    yaw_raw   = (int16_t)((buf[4] << 8) | buf[5]);

    if (roll_deg)  *roll_deg  = roll_raw  * ATTITUDE_LSB_DEG;
    if (pitch_deg) *pitch_deg = pitch_raw * ATTITUDE_LSB_DEG;
    if (yaw_deg)   *yaw_deg   = yaw_raw   * ATTITUDE_LSB_DEG;

    return 0;
}