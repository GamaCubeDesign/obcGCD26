//gcc -o healthdata healthdata.c -lm

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <math.h>

#include <stdint.h>

#define I2C_DEVICE "/dev/i2c-1"
#define INA219_ADDR 0x40

// ids dos sensores (MUDAR NO IARASAT INTEGRADO)
#define S1 "28-000000000001"
#define S2 "28-000000000002"
#define S3 "28-000000000003"

const char *sids[3] = {S1, S2, S3};

double read_ds18b20(const char *id){
    char path[128];
    snprintf(path, sizeof(path), "/sys/bus/w1/devices/%s/w1_slave", id);

    FILE *f = fopen(path, "r");
    if(!f) return NAN;

    char line[256];
    fgets(line, sizeof(line), f);
    fgets(line, sizeof(line), f);
    fclose(f);

    char *tpos = strstr(line, "t=");
    if(!tpos) return NAN;

    long t = strtol(tpos + 2, NULL, 10);
    return t / 1000.0;
}

double read_ina219_voltage(){
    int fd = open(I2C_DEVICE, O_RDWR);
    if(fd < 0) return NAN;

    if(ioctl(fd, I2C_SLAVE, INA219_ADDR) < 0){
        close(fd);
        return NAN;
    }

    uint8_t reg = 0x02;
    write(fd, &reg, 1);

    uint8_t buf[2];
    if(read(fd, buf, 2) != 2){
        close(fd);
        return NAN;
    }

    close(fd);

    int16_t raw = (buf[0] << 8) | buf[1];
    int16_t shifted = raw >> 3;
    return shifted * 0.004;
}

int main(){
    double temps[3];
    for(int i = 0; i < 3; i++)
        temps[i] = read_ds18b20(sids[i]);

    double vbus = read_ina219_voltage();

    //json
    printf("{\n");
    printf("  \"ina219_voltage_v\": %.3f,\n", isnan(vbus) ? 0.0 : vbus);
    printf("  \"temperatures_c\": [\n");

    for(int i = 0; i < 3; i++){
        printf("    {\"id\": \"%s\", \"temp_c\": ", sids[i]);
        if(isnan(temps[i])) printf("null}");
        else printf("%.3f}", temps[i]);

        if(i < 2) printf(",");
        printf("\n");
    }

    printf("  ]\n");
    printf("}\n");

    return 0;
}