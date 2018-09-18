#ifndef _JOYPAD_H_
#define _JOYPAD_H_

#include "GBCPU.h"

// Process Inputs from SDL events
bool ProcessSDLEvents(SDL_Event &event, GBCPU & CPU);

// Set specific joypad key in GB memory
void SetJoypadKey(GBCPU & CPU, BYTE enable_bit, BYTE key_bit);

// Set specific joypad key in GB memory
void ResetJoypadKey(GBCPU & CPU, BYTE enable_bit, BYTE key_bit);



#endif
