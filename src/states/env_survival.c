#include "env_survival.h"
#include "drivers/ina219.h"
#include "drivers/temp.h"
#include "drivers/gpio.h" // Incluido para acesso aos stubs
#include "obclog.h"
#include <stdlib.h>       // Incluido para chamadas de sistema (system)

static int cargas_desligadas = 0;

void env_survival_init(void){
    cargas_desligadas = 0;
}

void env_survival_run(void){
    if (!cargas_desligadas){
        // Aciona os stubs para desligar periféricos
        gpio_esp32_power_set(0);
        gpio_sdr_power_set(0);

        // Encerra processos pesados do SO
        // Ajustar para o nome do servico real do decodificador de ADS-B
        // system("sudo systemctl stop adsb-decoder.service > /dev/null 2>&1");

        cargas_desligadas = 1;
    }

    Telemetry tm = {0};
    tm.bateria_v = ina219_read_voltage();

    obclog_telemetry(&tm);
}