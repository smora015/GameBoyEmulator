/*  Name:        main.cpp
    Author:      Sergio Morales [sergiomorales.me]
    Created:     August 26th, 2016
    Modified:    September 12th, 2018
    Description: This file contains the logic to initialize and begin execution
                 of the Gameboy emulator. */

#include <SDL.h>
#include <Windows.h>

// Game Boy libraries
#include "GBPPU.h"        // Game Boy PPU library
#include "GBCartridge.h"  // ROM Cartridge library
#include "render.h"       // Graphics Rendering library

// Top-level emulator configurations
//#define DEBUG_GAMEBOY

// Global SDL variables
const Uint8 *SDL_GB_keyboard_state;
SDL_Event SDL_GB_window_event;

// Checks the CPU Interrupt flags and perform service routines if any are required
void CheckInterrupts(GBCPU & CPU);

// Updates CPU Timer register if enabled
void UpdateTimer(BYTE cycles, GBCPU & CPU);

// Update DIV register
void UpdateDIV(BYTE cycles, GBCPU & CPU);

// Process Inputs from SDL events
bool ProcessSDLEvents(SDL_Event &event, GBCPU & CPU);

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
    int counter = 0;

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

            // Get SDL events for joypad input and menu items
            quit = ProcessSDLEvents(SDL_GB_window_event, CPU);

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

/* Function: bool ProcessInputs(SDL_Event & event, GBCPU & CPU)
             Parses keyboard inputs in order to relay
             it over to JOYPAD_P1 register in memory. 
             
             Also handles non-emulator functions such as 
             quitting and the menu system.*/
bool ProcessSDLEvents(SDL_Event & SDL_GB_window_event, GBCPU & CPU)
{
    // Poll event
    SDL_PollEvent(&SDL_GB_window_event);

    // Quit if X has been clicked            
    if (SDL_GB_window_event.type == SDL_QUIT)
        return true;

    // Indicate no quit has been requested (via click on 'X' or Alt-F4)
    return false;
}

void CheckInterrupts(GBCPU & CPU)
{
    /* @TODO: Optimization/Cleanup of CheckInterrupts:
              1) Change interrupt routine locations to macros
              2) Move function to interrupts.c (within CPU folder) */
    // Only process interrupts if Interrupt Master Enable Flag is TRUE
    if (CPU.IME == false)
        return;

    BYTE interrupt_enable = CPU.readByte(INTERRUPT_ENABLE);
    BYTE interrupt_req = CPU.readByte(INTERRUPT_FLAG);

    // Check for the interrupt requests with the highest priority, only if enabled
    if ((interrupt_enable & 0x01) &&  // V-Blank
        (interrupt_req & 0x01))
    {
        // Disable interrupt, reset Request bit and set the PC to V-Blank interrupt routine
        CPU.IME = false;
        CPU.writeByte(interrupt_req & (~0x01), INTERRUPT_FLAG);

        // Push the current PC onto stack before calling service routine.
        --CPU.SP;
        CPU.writeByte(((CPU.PC) >> 8), CPU.SP);
        --CPU.SP;
        CPU.writeByte(((CPU.PC) & 0x00FF), CPU.SP);

        /*CPU.MEM[CPU.SP] = ((CPU.PC) & 0x00FF); --CPU.SP;
        CPU.MEM[CPU.SP] = ((CPU.PC) >> 8); --CPU.SP;*/

        CPU.PC = 0x0040;
    }    
    else if ((interrupt_enable & 0x02) && // LCD STAT
             (interrupt_req & 0x02))
    {
        // Disable interrupt, reset Request bit and set the PC to LCD interrupt routine
        CPU.IME = false;
        CPU.writeByte(interrupt_req & (~0x02), INTERRUPT_FLAG);

        // Push the current PC onto stack before calling service routine.
        --CPU.SP;
        CPU.writeByte(((CPU.PC) >> 8), CPU.SP);
        --CPU.SP;
        CPU.writeByte(((CPU.PC) & 0x00FF), CPU.SP);

        /*CPU.MEM[CPU.SP] = ((CPU.PC) & 0x00FF); --CPU.SP;
        CPU.MEM[CPU.SP] = ((CPU.PC) >> 8); --CPU.SP;*/

        CPU.PC = 0x0048;
    }    
    else if ((interrupt_enable & 0x04) && // Timer Interrupt
             (interrupt_req & 0x04))
    {
        cout << "TIMER INTERRUPT!" << endl;
        // Disable interrupt, reset Request bit and set the PC to Timer interrupt routine
        CPU.IME = false;
        CPU.writeByte(interrupt_req & (~0x04), INTERRUPT_FLAG);

        // Push the current PC onto stack before calling service routine.
        --CPU.SP;
        CPU.writeByte(((CPU.PC) >> 8), CPU.SP);
        --CPU.SP;
        CPU.writeByte(((CPU.PC) & 0x00FF), CPU.SP);

        /*CPU.MEM[CPU.SP] = ((CPU.PC) & 0x00FF); --CPU.SP;
        CPU.MEM[CPU.SP] = ((CPU.PC) >> 8); --CPU.SP;*/

        CPU.PC = 0x0050;
    }    
    else if ((interrupt_enable & 0x08) && // Serial Interrupt
             (interrupt_req & 0x08))
    {
        // Disable interrupt, reset Request bit and set the PC to Serial interrupt routine
        CPU.IME = false;
        CPU.writeByte(interrupt_req & (~0x08), INTERRUPT_FLAG);

        // Push the current PC onto stack before calling service routine.
        --CPU.SP;
        CPU.writeByte(((CPU.PC) >> 8), CPU.SP);
        --CPU.SP;
        CPU.writeByte(((CPU.PC) & 0x00FF), CPU.SP);

        /*CPU.MEM[CPU.SP] = ((CPU.PC) & 0x00FF); --CPU.SP;
        CPU.MEM[CPU.SP] = ((CPU.PC) >> 8); --CPU.SP;*/

        CPU.PC = 0x0058;
    }    
    else if ((interrupt_enable & 0x10) && // Joypad Interrupt
             (interrupt_req & 0x10))
    {
        // Disable interrupt, reset Request bit and set the PC to Joypad interrupt routine
        CPU.IME = false;
        CPU.writeByte(interrupt_req & (~0x10), INTERRUPT_FLAG);

        // Push the current PC onto stack before calling service routine.
        --CPU.SP;
        CPU.writeByte(((CPU.PC) >> 8), CPU.SP);
        --CPU.SP;
        CPU.writeByte(((CPU.PC) & 0x00FF), CPU.SP);

        /*CPU.MEM[CPU.SP] = ((CPU.PC) & 0x00FF); --CPU.SP;
        CPU.MEM[CPU.SP] = ((CPU.PC) >> 8); --CPU.SP;*/

        CPU.PC = 0x0060;
    }

    return;
}

void UpdateTimer(BYTE cycles, GBCPU & CPU)
{
    /* @TODO: Optimize/clean up UpdateTimer function:
              1) Clean up macro names to be consistent with GBCPU docs
              2) Optimize cycle calculations
              3) Use macros for timer interrupt flag mask
        */
    // Timer is enabled if TAC register, bit 2 is HIGH
    if (CPU.readByte(TCON) & 0x04)
    {
        // TAC register, bits 1 and 0 determine the timer frequency
        //cout << "t enabled..." << endl;
        switch (CPU.readByte(TCON) & 0x03)
        {
            // CPU Clock / 1024 = 4096 Hz
            case 0x00:
            {
                // Increment cycle counter. If we've hit the count, then update the counter
                CPU.TMA_counter += cycles;
                if (CPU.TMA_counter > 256)//(GAMEBOY_CLOCK_CYCLES/4096))
                {
                    // Check for overflow
                    if (CPU.readByte(TIMA) == 255)
                    {
                        // Set the counter to the value of the Timer Modulo flag 
                        CPU.MEM[TIMA] = CPU.MEM[TMOD];

                        // Request timer interrupt
                        CPU.MEM[INTERRUPT_FLAG] |= 0x04;
                    }
                    else
                    {
                        // Otherwise, increment memory counter
                        ++CPU.MEM[TIMA];
                    }

                    // Reset cycle counter
                    CPU.TMA_counter = 0;
                }
                break;
            }

            // CPU Clock / 16 = 262144 Hz. Need to / 4 for some reason
            case 0x01:
            {
                // Increment cycle counter. If we've hit the count, then update the counter.
                CPU.TMA_counter += cycles;
                if (CPU.TMA_counter > 4)//(GAMEBOY_CLOCK_CYCLES / 262144))
                {
                    // Check for overflow
                    if (CPU.MEM[TIMA] == 255)//(((16 / CPU.TMA_counter) + CPU.MEM[TIMA]) >= 255)
                    {
                        // Set the counter to the value of the Timer Modulo flag 
                        CPU.MEM[TIMA] = CPU.MEM[TMOD];

                        // Request timer interrupt
                        CPU.MEM[INTERRUPT_FLAG] |= 0x04;

                        // Reset cycle counter
                        //CPU.TMA_counter = 0;
                    }
                    else
                    {
                        // Otherwise, increment memory counter by the number of times the CPU cycle counter has gone over the
                        // selected frequency. This is not done incrementally because we are simulating multiple cycles happening
                        // over incremental loops in the software.
                        //CPU.MEM[TIMA] += (16 / CPU.TMA_counter);
                        ++CPU.MEM[TIMA];
                        // Leave only remainder of cycles in counter.
                        //CPU.TMA_counter = 16 % CPU.TMA_counter;

                    }

                    // Reset cycle counter
                    CPU.TMA_counter = 0;

                }
                break;
            }

            // CPU Clock / 64 = 65536 Hz
            case 0x02:
            {
                // Increment cycle counter. If we've hit the count, then update the counter
                CPU.TMA_counter += cycles;
                if (CPU.TMA_counter > 16)//(GAMEBOY_CLOCK_CYCLES / 65536))
                {
                    // Check for overflow
                    if (CPU.MEM[TIMA] == 255)
                    {
                        // Set the counter to the value of the Timer Modulo flag 
                        CPU.MEM[TIMA] = CPU.MEM[TMOD];

                        // Request timer interrupt
                        CPU.MEM[INTERRUPT_FLAG] |= 0x04;
                    }
                    else
                    {
                        // Otherwise, increment memory counter
                        ++CPU.MEM[TIMA];
                    }

                    // Reset cycle counter
                    CPU.TMA_counter = 0;
                }
                break;
            }

            // CPU Clock / 256 = 16384 Hz
            case 0x03:
            {
                // Increment cycle counter. If we've hit the count, then update the counter
                CPU.TMA_counter += cycles;
                if (CPU.TMA_counter > 64) //(GAMEBOY_CLOCK_CYCLES / 16384))
                {
                    // Check for overflow
                    if (CPU.MEM[TIMA] == 255)
                    {
                        // Set the counter to the value of the Timer Modulo flag 
                        CPU.MEM[TIMA] = CPU.MEM[TMOD];

                        // Request timer interrupt
                        CPU.MEM[INTERRUPT_FLAG] |= 0x04;
                    }
                    else
                    {
                        // Otherwise, increment memory counter
                        ++CPU.MEM[TIMA];
                    }

                    // Reset cycle counter
                    CPU.TMA_counter = 0;
                }
                break;
            }

            default:
            {
                cout << "TIMER NOT WORKING" << endl;
                break;
            }
                
        }
    }
    
    return;
}

void UpdateDIV(BYTE cycles, GBCPU & CPU)
{
    // If we've counted up enough GB cycles at the DIV counter rate (16384 Hz), 
    // we need to count 4.194304 MHz / 16384 Hz = 256 times to increment the counter
    CPU.DIV_counter += cycles;

    if (CPU.DIV_counter > 255)
    {
        // Increment memory counter
        ++CPU.MEM[DIV];
        CPU.DIV_counter = 0;
        //cout << CPU.MEM[DIV] << " ";
    }

    return;
}