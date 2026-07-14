#ifndef FSM_H
#define FSM_H

//modos

typedef enum {
    PRE_TEST, //modo de espera
    BASIC_INTERMEDIATE, // provas comuns
    ADVANCED_AOCS, //controle
    MISSION_ADSB, //adsb
    MISSION_DOWNLINK, //downlink dos dados
    ENV_SURVIVAL //modo de sobrevivencia
} State;

//gatilhos

typedef enum {
    EV_NONE, //sem eventos
    EV_TC_BASIC_INTER, //iniciar basic intermediate
    EV_TC_AOCS, //iniciar aocs
    EV_TC_MISSION_ADSB, //iniciar missao adsb
    EV_TC_DOWNLINK, //iniciar downlink mode
    EV_TC_SURVIVAL, //Entrar em modo ambiental
    EV_TASK_DONE, //a prova atual terminou
    EV_ADSB_TIMEOUT //fim dos 10 minutos da missao adsb
} Event;


void fsm_init(void); // start - pre test mode
void fsm_run(void); // ciclo unico
void  fsm_handle_event(Event ev);
State fsm_actual_state(void); //consulta estado atual
const char* fsm_state_name(State s); //nome para o log

#endif