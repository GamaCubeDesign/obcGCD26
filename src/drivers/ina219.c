#include "ina219.h"
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define REG_CONFIG      0x00
#define REG_SHUNT_V     0x01
#define REG_BUS_V       0x02
#define REG_CURRENT     0x04
#define REG_CALIBRATION 0x05

#define CONFIG_VALUE    0x399F
#define CAL_VALUE       4096
#define CURRENT_LSB_A   0.0001f 

static int i2c_fd = -1;


static int i2c_write_reg16(uint8_t reg, uint16_t value) {
    if (i2c_fd < 0) return -1;

    uint8_t buf[3];
    buf[0] = reg;
    buf[1] = (uint8_t)((value >> 8) & 0xFF); 
    buf[2] = (uint8_t)(value & 0xFF);        

    if (write(i2c_fd, buf, 3) != 3) {
        return -1;
    }
    return 0;
}


static int i2c_read_reg16(uint8_t reg, uint16_t *value) {
    if (i2c_fd < 0) return -1;


    if (write(i2c_fd, &reg, 1) != 1) {
        return -1;
    }

    uint8_t buf[2];
    if (read(i2c_fd, buf, 2) != 2) {
        return -1;
    }

    *value = (uint16_t)((buf[0] << 8) | buf[1]);
    return 0;
}

int ina219_init(int endereco_i2c) {
    const char *device = "/dev/i2c-1";
    i2c_fd = open(device, O_RDWR);
    if (i2c_fd < 0) {
        perror("[INA219] Falha ao abrir /dev/i2c-1");
        return -1;
    }

    if (ioctl(i2c_fd, I2C_SLAVE, endereco_i2c) < 0) {
        perror("[INA219] Falha ao selecionar endereco escravo I2C");
        close(i2c_fd);
        i2c_fd = -1;
        return -1;
    }


    if (i2c_write_reg16(REG_CONFIG, CONFIG_VALUE) < 0 ||
        i2c_write_reg16(REG_CALIBRATION, CAL_VALUE) < 0) {
        perror("[INA219] Falha ao configurar registradores");
        close(i2c_fd);
        i2c_fd = -1;
        return -1;
    }

    return i2c_fd;
}

void ina219_close(void) {
    if (i2c_fd >= 0) {
        close(i2c_fd);
        i2c_fd = -1;
    }
}

float ina219_read_voltage(void) {
    uint16_t raw = 0;
    if (i2c_read_reg16(REG_BUS_V, &raw) < 0) {
        return 0.0f;
    }
    raw >>= 3;
    return (float)raw * 0.004f;
}

float ina219_read_current(void) {
    uint16_t raw = 0;
    if (i2c_read_reg16(REG_CURRENT, &raw) < 0) {
        return 0.0f;
    }

    int16_t signed_raw = (int16_t)raw;
    return (float)signed_raw * CURRENT_LSB_A;
}