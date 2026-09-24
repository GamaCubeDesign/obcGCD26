#ifndef INA219_H
#define INA219_H

#define INA219_I2C_ADDR_DEFAULT  0x40  // ou 0x41, 0x44, 0x45, etc.

int   ina219_init(int endereco_i2c);
void  ina219_close(void);
float ina219_read_voltage(void);
float ina219_read_current(void);

#endif // INA219_H