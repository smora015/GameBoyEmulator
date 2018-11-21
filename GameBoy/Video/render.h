#ifndef RENDER_H
#define RENDER_H

#include "gameboy.h"
#include "GBCPU.h"

//extern pixel pixel_buffer[160][144];

// Renders the Nintendo scrolling graphic
void getIntroScreen(GBCPU cpu);

// Renders entire GameBoy video buffer
void renderPixelBuffer(SDL_Renderer * renderer, SDL_Texture * texture);

// Renders a pixel at point <X, Y>
void renderPixel(int x, int y, SDL_Renderer * renderer, pixel pixel);

#endif /* render.h */
