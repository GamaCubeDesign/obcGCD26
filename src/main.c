#include <stdio.h>
#include <time.h>

#include "fsm.h"
#include "obclog.h"
#include "drivers/radio.h"
#include "drivers/gpio.h"
#include "drivers/ina219.h"

#define OBC_CYCLE_PERIOD_MS 1000
#define OBC_LOG_PATH "obc_mission.jsonl"
#define INA219_I2C_ADDR 0x40 


static void obc_sleep_ms(unsigned int ms){
    struct timespec ts;
    ts.tv_sec  = (time_t)(ms / 1000u);
    ts.tv_nsec = (long)(ms % 1000u) * 1000000L;
    nanosleep(&ts, NULL);
}

int main(void){
    State previous_state;

    setvbuf(stdout, NULL, _IONBF, 0);

    fsm_init();


    if (obclog_init(OBC_LOG_PATH) != 0){
        fprintf(stderr, "[OBC] ERRO: nao foi possivel abrir %s\n", OBC_LOG_PATH);
        return 1;
    }


    gpio_init();

    if (radio_init() != 0){
        fprintf(stderr, "[OBC] ERRO: falha ao inicializar o radio\n");
        return 1;
    }

    if (ina219_init(INA219_I2C_ADDR) < 0){
        fprintf(stderr, "[OBC] AVISO: falha ao inicializar sensor INA219 no endereco 0x%02X\n", INA219_I2C_ADDR);
    }

    previous_state = fsm_actual_state();

    obclog_boot(OBC_CYCLE_PERIOD_MS);
    printf("[OBC] Boot concluido - periodo: %d ms - estado inicial: %s\n",
        OBC_CYCLE_PERIOD_MS, fsm_state_name(previous_state));

    while(1){

        Event ev = radio_poll_tc();

        if (ev != EV_NONE) {
            obclog_tc(ev);
        }

        fsm_handle_event(ev);

        if (fsm_actual_state() != previous_state){
            obclog_transition(previous_state, fsm_actual_state());
            printf("[OBC] Transicao: %s -> %s\n",
                fsm_state_name(previous_state),
                fsm_state_name(fsm_actual_state()));
            previous_state = fsm_actual_state();
        }

        fsm_run();

        obc_sleep_ms(OBC_CYCLE_PERIOD_MS);
    }

    return 0;
}