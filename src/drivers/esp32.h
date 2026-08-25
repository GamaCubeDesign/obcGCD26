#ifndef ESP32_H
#define ESP32_H

#define ESP32_I2C_ADDR_DEFAULT 0x42  // confirmar com o firmware do ESP32

int  esp32_init(int endereco_i2c);
void esp32_close(void);

int esp32_read_attitude(double *roll_deg, double *pitch_deg, double *yaw_deg);

#endif