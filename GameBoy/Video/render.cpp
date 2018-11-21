/*  Name:        render.cpp
    Author:      Sergio Morales [sergiomorales.me]
    Created:     October 19th, 2016
    Modified:    September 12th, 2018
    Description: This file contains the logic to initialize and begin execution
                 of the Gameboy emulator. */

#include "render.h"


// Pixel Buffer follows the Alpha-Red-Green-Blue format for SDL texture to render copying
BYTE pixel_buffer[144][160][4]; // 144 px tall, 160 px wide, 4-bytes for color


// Renders the Nintendo scrolling graphic
void getIntroScreen(GBCPU cpu)
{
    // The Nintendo logo in $104 - $133 is not encoded, each bit 
    // refers to a colored pixel or not.

    // Initialize pixel buffer
    for (int x = 0; x < 160; ++x)
    {
        for (int y = 0; y < 144; ++y)
        {
            pixel_buffer[y][x][0] = 0;
            pixel_buffer[y][x][1] = 255;
            pixel_buffer[y][x][2] = 255;
            pixel_buffer[y][x][3] = 255;
        }
    }

    // Populate a map of the Nintendo logo for rendering
    BYTE logo_map[12][4];
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
            BYTE data = logo_map[x][y];
            //pixel temp;

            // Render upper nibble
            for (int i = 0; i < 4; ++i)
            {
                if (data & (0x10 << (3 - i)))
                {
                    //cout << "1 ";
                    pixel_buffer[y_coord][x_coord][1]   = 55;
                    pixel_buffer[y_coord][x_coord][2]   = 55;
                    pixel_buffer[y_coord][x_coord++][3] = 255;
                    //temp.r = 55;
                    //temp.g = 55;
                    //temp.b = 255;
                }
                else
                {
                    //cout << "  ";
                    pixel_buffer[y_coord][x_coord][1]   = 255;
                    pixel_buffer[y_coord][x_coord][2]   = 255;
                    pixel_buffer[y_coord][x_coord++][3] = 255;
                    //temp.r = 55;
                    //temp.g = 45;
                    //temp.b = 25;
                }

                //pixel_buffer[x_coord++][y_coord] = temp;
            }
            //cout << " ";
        }
        x_coord = 55;
        //cout << endl;

        for (int x = 0; x < 12; ++x)
        {
            // Get the byte at current location
            BYTE data = logo_map[x][y];
            //pixel temp;

            // Render lower nibble
            for (int i = 0; i < 4; ++i)
            {
                if (data & (0x01 << (3 - i)))
                {
                    //cout << "1 ";
                    pixel_buffer[y_coord+1][x_coord][1] = 55;
                    pixel_buffer[y_coord+1][x_coord][2] = 55;
                    pixel_buffer[y_coord+1][x_coord++][3] = 255;
                    //temp.r = 55;
                    //temp.g = 55;
                    //temp.b = 255;
                }
                else
                {
                    //cout << "  ";
                    pixel_buffer[y_coord + 1][x_coord][1] = 255;
                    pixel_buffer[y_coord + 1][x_coord][2] = 255;
                    pixel_buffer[y_coord + 1][x_coord++][3] = 255;
                    //temp.r = 55;
                    //temp.g = 45;
                    //temp.b = 25;
                }

                //pixel_buffer[x_coord++][y_coord+1] = temp;
            }

            //cout << " ";
        }

        //cout << endl;
        y_coord += 2;

    }

    //cout << endl;
}

// Renders the GameBoy video buffer (256x256)
void renderPixelBuffer(SDL_Renderer * renderer, SDL_Texture * texture)
{
    SDL_UpdateTexture(texture, NULL, pixel_buffer, 160 * sizeof(Uint32)); // Last parameter is the size of one full row in the display

    //for (int y = 0; y < 144; ++y)
    //{
    //    for (int x = 0; x < (160); ++x)
    //    {
    //        renderPixel(x, y, renderer, pixel_buffer[x][y]);
    //    }
    //}
}

// Renders a quad at cell (x, y) with dimensions CELL_LENGTH
void renderPixel(int x, int y, SDL_Renderer * renderer, pixel pixel)
{
    SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawPoint(renderer, x, y);
}
