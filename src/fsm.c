#include "fsm.h"

static = State actual_state;

void fsm_init(void){
    actual_state = PRE_TEST;
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