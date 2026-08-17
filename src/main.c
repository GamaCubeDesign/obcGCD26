#include <stdio.h>
#include <time.h>

#include "fsm.h"
#include "drivers/radio.h"


#define OBC_CYCLE_PERIOD_MS 1000

/* Espera do ciclo.*/
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

    if (radio_init() != 0){
        fprintf(stderr, "[OBC] ERRO: falha ao inicializar o radio\n");
        return 1;
    }

    previous_state = fsm_actual_state();

    /* Primeiro log do sistema = sinal de vida */
    printf("[OBC] boot - periodo do ciclo: %d ms - estado inicial: %s\n",
           OBC_CYCLE_PERIOD_MS, fsm_state_name(previous_state));

    while(1){
        Event ev = radio_poll_tc();

        fsm_handle_event(ev);   /* decide para onde ir */
        fsm_run();              /* executa a acao do estado atual */

        if (fsm_actual_state() != previous_state){
            printf("[OBC] transicao: %s -> %s\n",
                   fsm_state_name(previous_state),
                   fsm_state_name(fsm_actual_state()));
            previous_state = fsm_actual_state();
        }

        obc_sleep_ms(OBC_CYCLE_PERIOD_MS);
    }
}
