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
        case PRE_TEST: /**/ break;
        case MISSION_ADSB: /**/ break;
        // other states
    }
}

State fsm_actual_state(void) {
    return actual_state;
}