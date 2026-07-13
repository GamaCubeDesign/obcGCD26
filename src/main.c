#include "fsm.h"

int main(void){
    fsm_init();

    while(1){ //main loop
        fsm_run();
    }
}