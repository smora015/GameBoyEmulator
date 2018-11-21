/*  Name:        interrupts.cpp
    Author:      Sergio Morales [sergiomorales.me]
    Created:     September 18th, 2018
    Modified:    September 18th, 2018
    Description: This file handles games interrupts, called at the main loop
                 by the emulator and configured by the CPU (enable and
                 request bits). */

#include "interrupts.h"

bool halt_waited = false; // Flag used to freeze PC for one cycle before exiting HALT.

void CheckInterrupts(GBCPU & CPU)
{
    BYTE interrupt_enable = CPU.readByte(INTERRUPT_ENABLE);
    BYTE interrupt_req = CPU.readByte(INTERRUPT_FLAG);

    /* @TODO: Optimization/Cleanup of CheckInterrupts:
              1) Change interrupt routine locations to macros
              2) Move function to interrupts.c (within CPU folder) */
              // Only process interrupts if Interrupt Master Enable Flag is TRUE
    if (CPU.IME == false)
    {
        // If a HALT execution has occurred, but IME is FALSE, we need to move to the 
        // next instruction if IE/IF is non-zero. Note that there is a bug with Gameboy
        // where the next instruction is executed twice- implemented here.
        if (CPU.halted == true)
        {
            /*
            if (halt_waited == false)
            {
                // HALT bug - wait for
                halt_waited = true;
            }
            else */if ((interrupt_enable != 0x00) &&
                     (interrupt_req != 0x00)/* &&
                     (halt_waited == true)*/)
            {
            ++CPU.PC;
            CPU.halted = false;
            }
        }

        return;
    }

    // Check for the interrupt requests with the highest priority, only if enabled
    if ((interrupt_enable & 0x01) &&  // V-Blank
        (interrupt_req & 0x01))
    {
        // Disable interrupt, reset Request bit and set the PC to V-Blank interrupt routine
        CPU.IME = false;
        CPU.writeByte(interrupt_req & (~0x01), INTERRUPT_FLAG);

        // Push the current PC onto stack before calling service routine.
        StorePCOnStack(CPU);
        
        /*
        --CPU.SP;
        CPU.writeByte(((CPU.PC) >> 8), CPU.SP);
        --CPU.SP;
        CPU.writeByte(((CPU.PC) & 0x00FF), CPU.SP);*/

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
        StorePCOnStack(CPU);

        /*--CPU.SP;
        CPU.writeByte(((CPU.PC) >> 8), CPU.SP);
        --CPU.SP;
        CPU.writeByte(((CPU.PC) & 0x00FF), CPU.SP);*/

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
        StorePCOnStack(CPU);

        /*--CPU.SP;
        CPU.writeByte(((CPU.PC) >> 8), CPU.SP);
        --CPU.SP;
        CPU.writeByte(((CPU.PC) & 0x00FF), CPU.SP);*/

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
        StorePCOnStack(CPU);
        
        /*--CPU.SP;
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
        StorePCOnStack(CPU);
        
        /*--CPU.SP;
        CPU.writeByte(((CPU.PC) >> 8), CPU.SP);
        --CPU.SP;
        CPU.writeByte(((CPU.PC) & 0x00FF), CPU.SP);

        /*CPU.MEM[CPU.SP] = ((CPU.PC) & 0x00FF); --CPU.SP;
        CPU.MEM[CPU.SP] = ((CPU.PC) >> 8); --CPU.SP;*/

        CPU.PC = 0x0060;
    }

    return;
}


void StorePCOnStack(GBCPU & CPU)
{
    // In the case that we are coming off a HALT, reset internal flag
    CPU.halted = false;

    // Push MSB first
    --CPU.SP;
    CPU.MEM[CPU.SP] = ((CPU.PC) >> 8);
    --CPU.SP;

    // Push LSB second
    CPU.MEM[CPU.SP] = ((CPU.PC) & 0x00FF);
}