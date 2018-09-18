/*  Name:        gameboy.cpp
    Author:      Sergio Morales [sergiomorales.me]
    Created:     August 26th, 2016
    Modified:    September 18th, 2018
    Description: This file contains the logic to initialize and begin execution
                 of the Gameboy emulator. */

#include <SDL.h>
#include <Windows.h>

// Game Boy libraries
#include "render.h"       // Graphics Rendering library
#include "GBCartridge.h"  // ROM Cartridge library
#include "GBPPU.h"        // Game Boy PPU library
#include "joypad.h"       // SDL event/input processing
#include "timers.h"       // CPU timer logic
#include "interrupts.h"   // CPU interrupt logic

// Top-level emulator configurations
//#define DEBUG_GAMEBOY

int main(int argc, char **argv)
{
    // Initialize Simple DirectMedia Library for video rendering, audio, and keyboard events
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Window *window;
    
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(160*3, 144*3, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE, &window, &renderer);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 160, 144); // NOTE: RGBA format needs Alpha as first element, not last!

    // Clear screen with White
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Load ROM data and initialize CPU/PPU
    string default_rom = "Tetris (World).gb"; //"01 - special.gb"; //BIOS.gb"; //;//"02-interrupts.gb";//"cpu_instrs.gb"; // ; 
    GBCPU CPU = GBCPU();

    load_rom(argc < 2 ? default_rom : string(argv[1]), CPU);
    CPU.init();

    // After loading ROM, set the window title to be the name of the game
    //char window_name[40] = { "GameBoy Emulator: "  };
    //strcat(window_name, rom_name);
    //SDL_SetWindowTitle(window, window_name);
    //SDL_SetWindowTitle(window, rom_name);
    SDL_SetWindowTitle(window, "Gameboy Emulator");

    // Capture Intro screen from ROM $104-133
    getIntroScreen(CPU);

    // Capture memory data into a file for debug purposes after initialization of CPU
    //CPU.printMEM("memory_map.txt");
    
    // Limit max # of CPU executions from debug use
    unsigned int counter = 0;

    // This flag captures the user clicking 'X' in order to elegantly quit the program
    bool quit = false;


    // DEBUG: Max executions
    int counter_max = strtol((argc < 3 ? "200000" : argv[2]), NULL, 10);

    // Main execution loop
    while (1)
    {
        // Update the screen with the pixel buffer. The FPS is assumed to be capped at 60 by SDL
        renderPixelBuffer(renderer, texture);

        // Execute the CPU and PPU by the number of clock cycles executed during this frame
        int cycles_in_frame = 0; // GAMEBOY_CYCLES_FRAME;
        while (cycles_in_frame < GAMEBOY_CYCLES_FRAME )
        {
            //if (++counter > (GAMEBOY_CYCLES_FRAME / 2))
            //{
                // Get SDL events for joypad input and menu items
                quit = ProcessSDLEvents(SDL_GB_window_event, CPU);

              //  counter = 0;
            //}
                

            /* TODO: Infrastructure optimizations
                     1) Remove cycles as argument. CPU class already has cycles internally defined
                     2) Rename file to gameboy.cpp
                     3) Move Timer & DIV update logic to their own files. Will help with
                        making project robust to multi-platforms later down the line. */
                    
            // Otherwise, execute the current CPU instruction
            CPU.execute();

            // Update timers based on # of cycles the current instruction took
            UpdateTimer(CPU.cycles, CPU);

            // Update DIV registers
            UpdateDIV(CPU.cycles, CPU);

            // Execute the PPU based on the # of cycles the current instruction took
            ExecutePPU(CPU.cycles, CPU);

            // Check for any interrupts being requested if enabled
            CheckInterrupts(CPU);

            // Update current number of cycles in this frame
            cycles_in_frame += CPU.cycles;

            // Quit if X has been clicked            
            if (quit)// || ++counter > counter_max)
                break;
        }

        // Check again to quit outside of main game loop to avoid lag
        if (quit)
            break;

        //TestVideoRAM(CPU);

        // Render the screen
        //SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    std::cout << "Finished executing instructions..." << endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
