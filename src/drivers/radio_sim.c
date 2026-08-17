#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "drivers/radio.h"

int radio_init(void){
    printf("[SIM] telecomandos (tecla + Enter):\n"
           "  1 basico/intermediario   2 aocs      3 missao adsb\n"
           "  4 downlink               5 ambiental\n"
           "  d tarefa concluida       t timeout adsb\n");
    return 0;
}

Event radio_poll_tc(void){
    struct pollfd entrada = { STDIN_FILENO, POLLIN, 0 };
    char c;

    if (poll(&entrada, 1, 0) <= 0) return EV_NONE;
    if (read(STDIN_FILENO, &c, 1) != 1) return EV_NONE;

    switch(c){
        case '1': return EV_TC_BASIC_INTER;
        case '2': return EV_TC_AOCS;
        case '3': return EV_TC_MISSION_ADSB;
        case '4': return EV_TC_DOWNLINK;
        case '5': return EV_TC_SURVIVAL;
        case 'd': return EV_TASK_DONE;
        case 't': return EV_ADSB_TIMEOUT;
        default:  return EV_NONE;
    }
}
