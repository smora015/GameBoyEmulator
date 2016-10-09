#ifndef RENDER_H_
#define RENDER_H_


#include "gameboy.h"
#include "GBCPU.h"

// Additional Libraries
#include <math.h>
#include <SDL.h>

// Struct that defines the pixels to be rendered on screen
typedef struct pixel 
{
    byte r;
    byte g;
    byte b;
} pixel;

// Renders the Nintendo scrolling graphic
void getIntroScreen(GBCPU cpu);

// Renders entire GameBoy video buffer
void renderPixelBuffer(SDL_Renderer * renderer);

// Renders a pixel at point <X, Y>
void renderPixel(int x, int y, SDL_Renderer * renderer, pixel pixel);

#endif /* render.h */
