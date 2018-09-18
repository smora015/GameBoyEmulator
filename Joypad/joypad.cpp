/*  Name:        joypad.cpp
    Author:      Sergio Morales [sergiomorales.me]
    Created:     September 18th, 2018
    Modified:    September 18th, 2018
    Description: This file contains the logic to parse keyboard input and relay
                 the information over to the GBCPU. SDL Key Events are used
                 to determine when keys have been pressed/released. */

#include "joypad.h"

// Define variables used for joypad logic
const Uint8 *SDL_GB_keyboard_state;
SDL_Event SDL_GB_window_event;
BYTE GB_buttons = 0x0F;
BYTE GB_dpad = 0x0F;


/* Function: bool ProcessSDLEvents(SDL_Event & SDL_GB_window_event, GBCPU & CPU)
             Parses keyboard inputs in order to relay
             it over to JOYPAD_P1 register in memory.

             Also handles non-emulator functions such as
             quitting and the menu system.*/
bool ProcessSDLEvents(SDL_Event & SDL_GB_window_event, GBCPU & CPU)
{
    // Poll for new events and keyboard input
    SDL_PollEvent(&SDL_GB_window_event);

    //SDL_GB_keyboard_state = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));

    // Quit if X has been clicked            
    if (SDL_GB_window_event.type == SDL_QUIT)
        return true;

    else if (SDL_GB_window_event.key.type == SDL_KEYDOWN)
    {
        // Check if valid key has been pressed and store in JOYPAD_P1. Request interrupt if needed
        switch (SDL_GB_window_event.key.keysym.sym)
        {
        case BUTTON_A:      SetJoypadKey(CPU, P1_BUTTONS, P1_A);      break;

        case BUTTON_B:      SetJoypadKey(CPU, P1_BUTTONS, P1_B);      break;

        case BUTTON_START:  SetJoypadKey(CPU, P1_BUTTONS, P1_START);  break;

        case BUTTON_SELECT: SetJoypadKey(CPU, P1_BUTTONS, P1_SELECT); break;

        case BUTTON_LEFT:   SetJoypadKey(CPU, P1_DPAD, P1_LEFT);      break;

        case BUTTON_RIGHT:  SetJoypadKey(CPU, P1_DPAD, P1_RIGHT);     break;

        case BUTTON_UP:     SetJoypadKey(CPU, P1_DPAD, P1_UP);        break;

        case BUTTON_DOWN:   SetJoypadKey(CPU, P1_DPAD, P1_DOWN);      break;

        default: break; // Button not suppported
        }
    }
    else if (SDL_GB_window_event.key.type == SDL_KEYUP)
    {
        // Check if valid key has been depressed and reset in JOYPAD_P1.
        switch (SDL_GB_window_event.key.keysym.sym)
        {
        case BUTTON_A:      ResetJoypadKey(CPU, P1_BUTTONS, P1_A);      break;

        case BUTTON_B:      ResetJoypadKey(CPU, P1_BUTTONS, P1_B);      break;

        case BUTTON_START:  ResetJoypadKey(CPU, P1_BUTTONS, P1_START);  break;

        case BUTTON_SELECT: ResetJoypadKey(CPU, P1_BUTTONS, P1_SELECT); break;

        case BUTTON_LEFT:   ResetJoypadKey(CPU, P1_DPAD, P1_LEFT);      break;

        case BUTTON_RIGHT:  ResetJoypadKey(CPU, P1_DPAD, P1_RIGHT);     break;

        case BUTTON_UP:     ResetJoypadKey(CPU, P1_DPAD, P1_UP);        break;

        case BUTTON_DOWN:   ResetJoypadKey(CPU, P1_DPAD, P1_DOWN);      break;

        default: break; // Button released that is not supported
        }
    }

    // Indicate no quit has been requested (via click on 'X' or Alt-F4)
    return false;
}

void SetJoypadKey(GBCPU & CPU, BYTE enable_bit, BYTE key_bit)
{
    // If key has not already been pressed, request interrupt
    if (!((~CPU.MEM[JOYPAD_P1]) & key_bit))
        CPU.MEM[INTERRUPT_FLAG] |= 0x10;

    // Set keys (0 means set)
    if (enable_bit == P1_DPAD)
        GB_dpad &= (~key_bit);

    else if (enable_bit == P1_BUTTONS)
        GB_buttons &= (~key_bit);
}

void ResetJoypadKey(GBCPU & CPU, BYTE enable_bit, BYTE key_bit)
{
    // Reset keys(1 means not set)
    if (enable_bit == P1_DPAD)
        GB_dpad |= key_bit;

    else if (enable_bit == P1_BUTTONS)
        GB_buttons |= key_bit;
}