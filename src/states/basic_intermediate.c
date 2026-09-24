#include "basic_intermediate.h"
#include "../fsm.h"
#include "../drivers/gpio.h"
#include "../drivers/ina219.h"
#include "../obclog.h"
#include <stdbool.h>

static bool task_finished = false;

void basic_intermediate_init(void) {
    task_finished = false;
    
    gpio_antenna_deploy_start();
}

void basic_intermediate_run(void) {
    if (task_finished) {
        return;
    }

    gpio_antenna_deploy_update();

    if (!gpio_antenna_deploy_done()) {
        return; 
    }

    Telemetry tm = {0};
    tm.bateria_v  = ina219_read_voltage();
    tm.corrente_a = ina219_read_current();

    obclog_telemetry(&tm);

    task_finished = true;

    fsm_handle_event(EV_TASK_DONE);
}