#include "fsm.h"

static State actual_state;

void fsm_init(void){
    actual_state = PRE_TEST;
}

//logica de transicoes

void fsm_handle_event(Event ev){
    switch(actual_state){

        case PRE_TEST:
            switch(ev){
                case EV_TC_BASIC_INTER:  actual_state = BASIC_INTERMEDIATE; break;
                case EV_TC_ADVANCED:     actual_state = ADVANCED_AOCS;      break;
                case EV_TC_MISSION_ADSB: actual_state = MISSION_ADSB;       break;
                case EV_TC_DOWNLINK:     actual_state = MISSION_DOWNLINK;   break;
                case EV_TC_ENV_SURVIVAL: actual_state = ENV_SURVIVAL;       break;
                default: break;
            }
            break;
        
        case BASIC_INTERMEDIATE:
        if (ev == EV_TASK_DONE) actual_state = PRE_TEST;
        break;

        case ADVANCED_AOCS:
        if (ev == EV_TASK_DONE) actual_state = PRE_TEST;
        break;

        case MISSION_ADSB:
        if (ev == EV_ADSB_TIMEOUT || ev == EV_TC_DOWNLINK)
            actual_state = MISSION_DOWNLINK;
        break;

        case MISSION_DOWNLINK:
        if (ev == EV_TASK_DONE) actual_state = PRE_TEST;
        break;

        case ENV_SURVIVAL:
        //tem q ver como sai desse modo depois
        break;
    }
}

void fsm_run(void){
    switch(actual_state){
        case PRE_TEST: /* pre_test_run(); */ break;
        case BASIC_INTERMEDIATE: /* basic_intermediate_run(); */ break;
        case ADVANCED_AOCS: /* advanced_aocs_run(); */ break;
        case MISSION_ADSB: /* mission_adsb_run(); */ break;
        case MISSION_DOWNLINK: /* mission_downlink_run(); */ break;
        case ENV_SURVIVAL: /* env_survival_run(); */ break;
    }
}

State fsm_actual_state(void) {
    return actual_state;
}

const char* fsm_state_name(State s){
    switch(s){
        case PRE_TEST: return "PRE_TEST";
        case BASIC_INTERMEDIATE: return "BASIC_INTERMEDIATE";
        case ADVANCED_AOCS: return "ADVANCED_AOCS";
        case MISSION_ADSB: return "MISSION_ADSB";
        case MISSION_DOWNLINK: return "MISSION_DOWNLINK";
        case ENV_SURVIVAL: return "ENV_SURVIVAL";
    }
    return "UNKNOWN";
}