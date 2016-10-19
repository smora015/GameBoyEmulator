// Standard Libraries
#include <iostream>
#include <Windows.h>

// SDL Library
#include <SDL.h>

// Game Boy libraries
#include "gameboy.h"      // Data and Macro definitions
#include "GBCartridge.h"  // ROM Cartridge library
#include "GBCPU.h"        // Game Boy CPU library
#include "GBPPU.h"        // Game Boy PPU library
#include "render.h"       // Graphics Rendering library

using namespace std;


// Checks the CPU Interrupt flags and perform service routines if any are required
void CheckInterrupts(GBCPU & CPU);

// Updates CPU Timer register if enabled
void UpdateTimer(byte cycles, GBCPU & CPU);

// Update DIV register
void UpdateDIV(byte cycles, GBCPU & CPU);

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

    int exec_count = 2000;

    // Main execution loop
    while (1) 
    {
        // Execute the CPU and PPU by the number of clock cycles executed during this frame (assuming 60 FPS capped by SDL)
        int cycles_in_frame = 0; 
        while (cycles_in_frame < GAMEBOY_CYCLES_SEC)
        {
            // Otherwise, execute the current CPU instruction and get the # of cycles it took
            while ((exec_count--) > 0)
            {
                CPU.execute();
            }

            // Execute the PPU based on the # of cycles the current instruction took
            byte cycles = CPU.cycles;
            ExecutePPU(cycles);

            // Update timers based on # of cycles the current instruction took
            UpdateTimer(cycles, CPU);

            // Update DIV registers
            UpdateDIV(cycles, CPU);

            // Check for any interrupts being requested if enabled
            CheckInterrupts(CPU);

            // Update current number of cycles in this frame
            cycles_in_frame += cycles;
        }

        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;


        // Render the screen
        renderPixelBuffer(renderer, CPU);
        SDL_RenderPresent(renderer);
    }

    std::cout << "Finished executing instructions..." << endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

void CheckInterrupts(GBCPU & CPU)
{
    // Only process interrupts if Interrupt Master Enable Flag is TRUE
    if (CPU.IME == false)
        return;

    byte interrupt_enable = CPU.readByte(INTERRUPT_ENABLE);
    byte interrupt_req = CPU.readByte(INTERRUPT_FLAG);

    // Check for the interrupt requests with the highest priority, only if enabled
    if ((interrupt_enable & 0x01) &&  // V-Blank
        (interrupt_req & 0x01))
    {
        // Disable interrupt, reset Request bit and set the PC to V-Blank interrupt routine
        CPU.IME = false;
        CPU.writeByte(interrupt_req & (~0x01), INTERRUPT_FLAG);

        // Push the current PC onto stack before calling service routine.
        CPU.MEM[CPU.SP] = ((CPU.PC) & 0x00FF); --CPU.SP;
        CPU.MEM[CPU.SP] = ((CPU.PC) >> 8); --CPU.SP;

        CPU.PC = 0x0040;
    }    
    else if ((interrupt_enable & 0x02) && // LCD STAT
        (interrupt_req & 0x02))
    {
        // Disable interrupt, reset Request bit and set the PC to LCD interrupt routine
        CPU.IME = false;
        CPU.writeByte(interrupt_req & (~0x02), INTERRUPT_FLAG);

        // Push the current PC onto stack before calling service routine.
        CPU.MEM[CPU.SP] = ((CPU.PC) & 0x00FF); --CPU.SP;
        CPU.MEM[CPU.SP] = ((CPU.PC) >> 8); --CPU.SP;

        CPU.PC = 0x0048;
    }    
    else if ((interrupt_enable & 0x04) && // Timer Interrupt
        (interrupt_req & 0x04))
    {
        // Disable interrupt, reset Request bit and set the PC to Timer interrupt routine
        CPU.IME = false;
        CPU.writeByte(interrupt_req & (~0x04), INTERRUPT_FLAG);

        // Push the current PC onto stack before calling service routine.
        CPU.MEM[CPU.SP] = ((CPU.PC) & 0x00FF); --CPU.SP;
        CPU.MEM[CPU.SP] = ((CPU.PC) >> 8); --CPU.SP;

        CPU.PC = 0x0050;
    }    
    else if ((interrupt_enable & 0x08) && // Serial Interrupt
        (interrupt_req & 0x08))
    {
        // Disable interrupt, reset Request bit and set the PC to Serial interrupt routine
        CPU.IME = false;
        CPU.writeByte(interrupt_req & (~0x08), INTERRUPT_FLAG);

        // Push the current PC onto stack before calling service routine.
        CPU.MEM[CPU.SP] = ((CPU.PC) & 0x00FF); --CPU.SP;
        CPU.MEM[CPU.SP] = ((CPU.PC) >> 8); --CPU.SP;

        CPU.PC = 0x0058;
    }    
    else if ((interrupt_enable & 0x10) && // Joypad Interrupt
        (interrupt_req & 0x10))
    {
        // Disable interrupt, reset Request bit and set the PC to Joypad interrupt routine
        CPU.IME = false;
        CPU.writeByte(interrupt_req & (~0x10), INTERRUPT_FLAG);

        // Push the current PC onto stack before calling service routine.
        CPU.MEM[CPU.SP] = ((CPU.PC) & 0x00FF); --CPU.SP;
        CPU.MEM[CPU.SP] = ((CPU.PC) >> 8); --CPU.SP;

        CPU.PC = 0x0060;
    }

    return;
}

void UpdateTimer(byte cycles, GBCPU & CPU)
{
    // Update the timer register in memory depending on the frequency specified by the Timer Control

    // If Timer is enabled
    if (CPU.MEM[TCON] & 0x04)
    {
        switch (CPU.MEM[TCON] & 0x03)
        {
            // CPU Clock / 1024 = 4096 Hz
            case 0x00:
            {
                // Increment cycle counter. If we've hit the count, then update the counter
                CPU.TMA_counter += cycles;
                if (CPU.TMA_counter > (GAMEBOY_CLOCK_CYCLES/4096))
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

            // CPU Clock / 16 = 262144 Hz
            case 0x01:
            {
                // Increment cycle counter. If we've hit the count, then update the counter
                CPU.TMA_counter += cycles;
                if (CPU.TMA_counter > (GAMEBOY_CLOCK_CYCLES / 262144))
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

            // CPU Clock / 64 = 65536 Hz
            case 0x02:
            {
                // Increment cycle counter. If we've hit the count, then update the counter
                CPU.TMA_counter += cycles;
                if (CPU.TMA_counter > (GAMEBOY_CLOCK_CYCLES / 65536))
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
                if (CPU.TMA_counter > (GAMEBOY_CLOCK_CYCLES / 16384))
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
                break;
        }
    }
    
    return;
}


void UpdateDIV(byte cycles, GBCPU & CPU)
{
    // If we've counted up enough GB cycles at the DIV counter rate (16384 Hz), 
    // we need to count 4.194304 MHz / 16384 Hz = 256 times to increment the counter
    CPU.DIV_counter += cycles;

    if (CPU.DIV_counter > 255)
    {
        // Increment memory counter
        ++CPU.MEM[DIV];
        CPU.DIV_counter = 0;
    }

    return;
}