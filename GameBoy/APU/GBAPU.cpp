/*  Name:        GBAPU.cpp
    Author:      Sergio Morales [sergiomorales.me]
    Created:     October 3rd, 2018
    Modified:    October 8th, 2018
    Description: This file contains the logic to calculate sound waves from 
                 parsed from the audio processing unit registers and
                 output them to the user. */

#include "GBAPU.h"


SDL_AudioSpec SDL_GB_audio;


void SDLAudioCallback(void *UserData, Uint8 *AudioData, int Length)
{
    // Clear our audio buffer to silence.
    for (int i = 0; i < Length; ++i)
        AudioData[i] = 0x00;
}