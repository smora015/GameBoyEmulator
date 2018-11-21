/*  Name:        joypad.cpp
    Author:      Sergio Morales [sergiomorales.me]
    Created:     October 19th, 2016
    Modified:    September 18th, 2018
    Description: This file contains the logic to handle the GBCPU timer and DIV
                 counters, and will set the appropriate interrupt requests upon
                 hitting the configured frequency. */
#include "timers.h"

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
                    
                    ++CPU.MEM[TIMA];
                    //CPU.MEM[TIMA] += (16 / CPU.TMA_counter);
                    
                    /*if (CPU.MEM[TIMA] + (CPU.TMA_counter / 4) > 0xFF)
                        CPU.MEM[TIMA] = 0xFF;
                    else
                        CPU.MEM[TIMA] += CPU.TMA_counter / 4;*/

                    // Leave only remainder of cycles in counter.
                    //CPU.TMA_counter %= 4;

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
