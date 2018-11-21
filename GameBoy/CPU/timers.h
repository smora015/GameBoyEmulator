#ifndef _TIMERS_H_
#define _TIMERS_H_

#include "GBCPU.h"

// Updates CPU Timer register if enabled
void UpdateTimer(BYTE cycles, GBCPU & CPU);

// Update DIV register
void UpdateDIV(BYTE cycles, GBCPU & CPU);


#endif
