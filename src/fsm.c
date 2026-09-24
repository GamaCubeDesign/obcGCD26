#include "fsm.h"
#include "states/basic_intermediate.h"
#include "states/env_survival.h"

static State actual_state;

void fsm_init(void){
    actual_state = PRE_TEST;
}

void fsm_handle_event(Event ev){
    switch(actual_state){

        case PRE_TEST:
            switch(ev){
                case EV_TC_BASIC_INTER:  
                    actual_state = BASIC_INTERMEDIATE; 
                    basic_intermediate_init();
                    break;
                case EV_TC_AOCS:         
                    actual_state = ADVANCED_AOCS;      
                    break;
                case EV_TC_MISSION_ADSB: 
                    actual_state = MISSION_ADSB;       
                    break;
                case EV_TC_DOWNLINK:     
                    actual_state = MISSION_DOWNLINK;   
                    break;
                case EV_TC_SURVIVAL:     
                    actual_state = ENV_SURVIVAL;       
                    env_survival_init();
                    break;
                default: break;
            }
            break;
        
        case BASIC_INTERMEDIATE:
            if (ev == EV_TASK_DONE) {
                actual_state = PRE_TEST;
            }
            break;

        case ADVANCED_AOCS:
            if (ev == EV_TASK_DONE) {
                actual_state = PRE_TEST;
            }
            break;

        case MISSION_ADSB:
            if (ev == EV_ADSB_TIMEOUT || ev == EV_TC_DOWNLINK) {
                actual_state = MISSION_DOWNLINK;
            }
            break;

        case MISSION_DOWNLINK:
            if (ev == EV_TASK_DONE) {
                actual_state = PRE_TEST;
            }
            break;

        case ENV_SURVIVAL:
            
            if (ev == EV_TASK_DONE) {
                actual_state = PRE_TEST;
            }
            break;
    }
}

void fsm_run(void){
    switch(actual_state){
        case PRE_TEST: 
            break;
        case BASIC_INTERMEDIATE: 
            basic_intermediate_run(); 
            break;
        case ADVANCED_AOCS: 
            break;
        case MISSION_ADSB: 
            break;
        case MISSION_DOWNLINK: 
            break;
        case ENV_SURVIVAL: 
            env_survival_run(); 
            break;
    }
}

State fsm_actual_state(void) {
    return actual_state;
}

const char* fsm_event_name(Event ev){
    switch(ev){
        case EV_NONE:            return "EV_NONE";
        case EV_TC_BASIC_INTER:  return "EV_TC_BASIC_INTER";
        case EV_TC_AOCS:         return "EV_TC_AOCS";
        case EV_TC_MISSION_ADSB: return "EV_TC_MISSION_ADSB";
        case EV_TC_DOWNLINK:     return "EV_TC_DOWNLINK";
        case EV_TC_SURVIVAL:     return "EV_TC_SURVIVAL";
        case EV_TASK_DONE:       return "EV_TASK_DONE";
        case EV_ADSB_TIMEOUT:    return "EV_ADSB_TIMEOUT";
    }
    return "UNKNOWN";
}

const char* fsm_state_name(State s){
    switch(s){
        case PRE_TEST:           return "PRE_TEST";
        case BASIC_INTERMEDIATE: return "BASIC_INTERMEDIATE";
        case ADVANCED_AOCS:      return "ADVANCED_AOCS";
        case MISSION_ADSB:       return "MISSION_ADSB";
        case MISSION_DOWNLINK:   return "MISSION_DOWNLINK";
        case ENV_SURVIVAL:       return "ENV_SURVIVAL";
    }
    return "UNKNOWN";
}