// Standard Libraries
#include <iostream>
#include <Windows.h>

// SDL Library
#include <SDL.h>

// Game Boy libraries
#include "gameboy.h"        // Data and Macro definitions
#include "render.h"         // Graphics Rendering library
#include "GBCartridge.h"    // ROM Cartridge library
#include "GBCPU.h"          // Game Boy CPU library
#include "GBPPU.h"          // Game Boy PPU library

using namespace std;

int main(int argc, char **argv)
{
    // Initialize Simple DirectMedia Library for video rendering, audio, and keyboard events
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(160, 144, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Load ROM data and initialize CPU/PPU
    string default_rom = "cpu_instrs.gb";        // "Tetris (World).gb"
    GBCPU CPU = GBCPU();

    load_rom(argc < 2 ? default_rom : string(argv[1]), CPU);
    CPU.init();

    // Capture Intro screen from ROM $104-133
    getIntroScreen(CPU);

    // Capture memory data into a file for debug purposes after initialization of CPU
    //CPU.printMEM("memory_map.txt");

    // Main execution loop
    while (1) 
    {
        int cycles_in_frame = 0; // The number of clock cycles executed during this frame
        while (cycles_in_frame < GAMEBOY_CYCLES_SEC)
        {
            // Execute the current CPU instruction and get the # of cycles it took
            //int count = 0;
            //while ((count--) > 0)
            //{
            //    CPU.execute();
            //    //Sleep(1000);
            //}

            // Update timers based on # of cycles the current instruction took

            // Execute the PPU based on the # of cycles the current instruction took


            // Update current number of cycles in this frame
            ++cycles_in_frame;
        }

        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;


        // Render the screen
        renderPixelBuffer(renderer);
        SDL_RenderPresent(renderer);
    }

    cout << "Finished executing instructions..." << endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}