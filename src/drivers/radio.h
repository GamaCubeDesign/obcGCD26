#ifndef RADIO_H
#define RADIO_H

#include "fsm.h"

int radio_init(void);

Event radio_poll_tc(void);

#endif
