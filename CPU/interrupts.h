#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include "GBCPU.h"

// Checks the CPU Interrupt flags and perform service routines if any are required
void CheckInterrupts(GBCPU & CPU);

#endif
