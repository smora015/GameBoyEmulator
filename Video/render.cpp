#include "render.h"
#include <stdio.h>

// Define variables
pixel pixel_buffer[256][256]; // 256 x 256 pixels exist, but the true resolution is 160 x 144


// Renders the Nintendo scrolling graphic
void getIntroScreen(GBCPU cpu)
{
    // The Nintendo logo in $104 - $133 is not encoded, each bit 
    // refers to a colored pixel or not.

    // Initialize pixel buffer
    for (int x = 0; x < 256; ++x)
    {
        for (int y = 0; y < 256; ++y)
        {
            pixel_buffer[x][y].r = 55;
            pixel_buffer[x][y].g = 45;
            pixel_buffer[x][y].b = 25;
        }
    }

    // Populate a map of the Nintendo logo for rendering
    byte logo_map[12][4];
    unsigned short index = 0x104; // Beginning of logo in memory

    // Y axis
    for (int y = 0; y < 2; ++y)
    {
        // X axis
        for (int x = 0; x < 12; ++x)
        {
            logo_map[x][y*2] = cpu.MEM[index++];
            logo_map[x][y*2 + 1] = cpu.MEM[index++];
        }
    }

    unsigned short x_coord = 55;
    unsigned short y_coord = 65;

    // Now render the data based on the map
    for (int y = 0; y < 4; ++y)
    {
        x_coord = 55;
        for (int x = 0; x < 12; ++x)
        {
            // Get the byte at current location
            byte data = logo_map[x][y];
            pixel temp;

            // Render upper nibble
            for (int i = 0; i < 4; ++i)
            {
                if (data & (0x10 << (3 - i)))
                {
                    //cout << "1 ";
                    temp.r = 55;
                    temp.g = 55;
                    temp.b = 255;
                }
                else
                {
                    //cout << "  ";
                    temp.r = 55;
                    temp.g = 45;
                    temp.b = 25;
                }

                pixel_buffer[x_coord++][y_coord] = temp;
            }
            //cout << " ";
        }
        x_coord = 55;
        //cout << endl;

        for (int x = 0; x < 12; ++x)
        {
            // Get the byte at current location
            byte data = logo_map[x][y];
            pixel temp;

            // Render lower nibble
            for (int i = 0; i < 4; ++i)
            {
                if (data & (0x01 << (3 - i)))
                {
                    //cout << "1 ";
                    temp.r = 55;
                    temp.g = 55;
                    temp.b = 255;
                }
                else
                {
                    //cout << "  ";
                    temp.r = 55;
                    temp.g = 45;
                    temp.b = 25;
                }

                pixel_buffer[x_coord++][y_coord+1] = temp;
            }

            //cout << " ";
        }

        //cout << endl;
        y_coord += 2;

    }

    //cout << endl;
}

// Renders the GameBoy video buffer (256x256)
void renderPixelBuffer(SDL_Renderer * renderer, GBCPU & CPU)
{
    int scrolly = CPU.readByte(PPU_SCROLLY);
    int scrollx = CPU.readByte(PPU_SCROLLX);

    for (int y = scrolly; y < (144 + scrolly); ++y)
    {
        for (int x = scrollx; x < (160 + scrollx); ++x)
        {
            renderPixel(x, y, renderer, pixel_buffer[x][y]);
        }
    }
}

// Renders a quad at cell (x, y) with dimensions CELL_LENGTH
void renderPixel(int x, int y, SDL_Renderer * renderer, pixel pixel)
{
    SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawPoint(renderer, x, y);
}
