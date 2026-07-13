#ifndef FSM_H
#define FSM_H

typedef enum {
    PRE_TEST,
    BASIC_INTERMEDIATE,
    ADVANCED_AOCS,
    MISSION_ADSB,
    MISSION_DOWNLINK,
    ENV_SURVIVAL
} State;

void fsm_init(void); // start - pre test mode
void fsm_run(void); //one cycle
State fsm_actual_state(void); 

#endif