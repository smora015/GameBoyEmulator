#ifndef RENDER_H
#define RENDER_H


#include "gameboy.h"
#include "GBCPU.h"
#include "GBPPU.h"
#include "GBCartridge.h"
#include "memory.h"

// Additional Libraries
#include <math.h>
#include <SDL.h>

// Struct that defines the pixels to be rendered on screen
typedef struct pixel {
    byte r;
    byte g;
    byte b;
} pixel;

//extern pixel pixel_buffer[160][144];
extern byte pixel_buffer[144][160][4];

// Renders the Nintendo scrolling graphic
void getIntroScreen(GBCPU cpu);

// Renders entire GameBoy video buffer
void renderPixelBuffer(SDL_Renderer * renderer, SDL_Texture * texture);

// Renders a pixel at point <X, Y>
void renderPixel(int x, int y, SDL_Renderer * renderer, pixel pixel);

#endif /* render.h */
