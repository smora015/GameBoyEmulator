/*  Name:        memory.cpp
    Author:      Sergio Morales [sergiomorales.me]
    Created:     August 30th, 2016
    Modified:    September 12th, 2018
    Description: Description: This file contains the definitions for functions that
                 access internal registers and memory for the CPU. */

#include "GBCartridge.h"
#include "GBPPU.h"


// writeByte - Write one byte to memory
void GBCPU::writeByte(BYTE data, WORD addr)
{
    if (rom_mbc_type == ROM_MBC1)
    {
        MBC1write(addr, data);
    }
    else if (rom_mbc_type == ROM_ONLY)
    {
        // Do not write to read only memory
        if (addr <= EXTERNAL_ROM_END)
        {
            printf("Writing to ROM bank at %X!\n", addr);
        }

        // If memory is written to WRAM or ECHO WRAM, write to both. Note: last 512 bytes not echoed.
        /*else if (addr >= WRAM_START && addr <= (WRAM_END - 0x200))
        {
            MEM[addr] = data;
            writeByte(data, addr + (WRAM_ECHO_START - WRAM_START));

            // Only copy data when we write to WRAM_ECHO, not WRAM
        }*/

        // Write data from ECHO_WRAM to WRAM as well
        else if (addr >= WRAM_ECHO_START && addr <= WRAM_ECHO_END)
        {
            MEM[addr] = data;
            writeByte(data, addr - (WRAM_ECHO_START - WRAM_START));
        }

        // Writing to unused area in Memory Map
        else if (addr >= 0xFEA0 && addr < 0xFEFF)
        {
            cout << "Restricted memory region!" << endl;
        }

        // Writing to JOYPAD_P1 should only set P14 and P15 outputs
        else if (addr == JOYPAD_P1)
            MEM[JOYPAD_P1] = (MEM[JOYPAD_P1] & 0x0F) | (data & 0x30);

        // Reset the DIV register if we're writing to it
        else if (addr == DIV)
            MEM[DIV] = 0;

        // Reset scanline counter if written to
        else if (addr == PPU_LY)
            MEM[PPU_LY] = 0;

        // Sprite DMA Trasnfer
        else if (addr == PPU_DMA)
            PerformDMATransfer(data);

        // Debugging Blargg's tests. GB link registers used to output info.
        else if (addr == SIO_CONTROL && data == 0x81) 
            printf("%c", readByte(SERIAL_XFER));
        
        // Normal write
        else
            MEM[addr] = data;
    }
    else
    {
        cout << "Unsupported MBC type!!!" << endl;
    }

}

void GBCPU::writeWord(WORD data, WORD addr)
{
    // Write LSB first
    writeByte((BYTE)(data & 0xFF), addr);
    writeByte((BYTE)((data >> 8) & 0xFF), addr + 1);
}

// readByte - Read byte from memory
BYTE GBCPU::readByte(WORD addr)
{
    if (rom_mbc_type == ROM_MBC1)
    {
        return MBC1read(addr);
    }
    else if (rom_mbc_type == ROM_ONLY)
    {
        // Read ECHO WRAM from WRAM
        if (addr >= WRAM_ECHO_START && addr <= WRAM_ECHO_END)
        {
            return MEM[addr - (WRAM_ECHO_START - WRAM_START)];
        }

        // Reading from unused area in Memory Map
        else if (addr >= 0xFEA0 && addr < 0xFEFF)
        {
            cout << "Restricted memory region!" << endl;
            return 0xFF;
        }

        else if (addr == JOYPAD_P1)
        {
            // Trap reads to JOYPAD_P1 in order to read input from SDL, otherwise
            // processing input in the main loop will overwrite key presses
            ProcessJoyPad();

            return MEM[JOYPAD_P1];
        }
        // Otherwise read from wherever
        else
            return MEM[addr];
    }
    else
    {
        cout << "Unsupported MBC type!!!" << endl;
        return 0x00;
    }

}


void GBCPU::ProcessJoyPad()
{
    //bool interrupt_req_prev = (CPU.MEM[INTERRUPT_FLAG] & 0x10) ? true : false;
    // Use temproary variables to store key presses. Checking JOYPAD_P1 itself will
    // result in unexpected interrupts due to the fact that it may have been set to 0.
    BYTE buttons = 0xC0;
    BYTE dpad = 0xC0;

    // Poll event
    SDL_PollEvent(&SDL_GB_window_event);
    SDL_GB_keyboard_state = SDL_GetKeyboardState(NULL);

    if ((SDL_GB_window_event.key.type == SDL_KEYUP) ||
        (SDL_GB_window_event.key.type == SDL_KEYDOWN))
    {
        if (!(MEM[JOYPAD_P1] & JOYPAD_P14))
        {
            //cout << "KEY PRESSED! " << event.key.keysym.sym << endl;

            // ROM requesting read of LEFT DOWN RIGHT UP 
            if (SDL_GB_keyboard_state[SDL_SCANCODE_LEFT])
                dpad |= JOYPAD_P11;
            //CPU.MEM[JOYPAD_P1] &= ~(JOYPAD_P11);

            else if (SDL_GB_keyboard_state[SDL_SCANCODE_RIGHT])
                dpad |= JOYPAD_P10;
            //CPU.MEM[JOYPAD_P1] &= ~(JOYPAD_P10);

            if (SDL_GB_keyboard_state[SDL_SCANCODE_DOWN])
                dpad |= JOYPAD_P13;
            //CPU.MEM[JOYPAD_P1] &= ~(JOYPAD_P13);

            else if (SDL_GB_keyboard_state[SDL_SCANCODE_UP])
                dpad |= JOYPAD_P12;
            //CPU.MEM[JOYPAD_P1] &= ~(JOYPAD_P12);

        }

        else if (!(MEM[JOYPAD_P1] & JOYPAD_P15))
        {
            //cout << "KEY PRESSED! " << event.key.keysym.sym << endl;

            // ROM requesting read of A B SELECT START
            if (SDL_GB_keyboard_state[SDL_SCANCODE_Z])
                buttons |= JOYPAD_P11;
            //CPU.MEM[JOYPAD_P1] &= ~(JOYPAD_P11);

            if (SDL_GB_keyboard_state[SDL_SCANCODE_X])
                buttons |= JOYPAD_P10;
            //CPU.MEM[JOYPAD_P1] &= ~(JOYPAD_P10);

            if (SDL_GB_keyboard_state[SDL_SCANCODE_PERIOD])
                buttons |= JOYPAD_P12;
            //CPU.MEM[JOYPAD_P1] &= ~(JOYPAD_P12);

            if (SDL_GB_keyboard_state[SDL_SCANCODE_SLASH])
                buttons |= JOYPAD_P13;
            //CPU.MEM[JOYPAD_P1] &= ~(JOYPAD_P13);
        }
    }

    //if ((CPU.MEM[JOYPAD_P1] & 0x0F) != 0x0F)
        //CPU.MEM[INTERRUPT_FLAG] |= 0x10;

    if (dpad & 0x0F)
    {
        // Update JOYPAD_P1 in memory and request interrupt (if needed)
        MEM[JOYPAD_P1] = (MEM[JOYPAD_P1] & 0x30) | (~dpad);
        MEM[INTERRUPT_FLAG] |= 0x10;
    }

    else if (buttons & 0x0F)
    {
        // Update JOYPAD_P1 in memory and request interrupt (if needed)
        MEM[JOYPAD_P1] = (MEM[JOYPAD_P1] & 0x30) | (~buttons);
        MEM[INTERRUPT_FLAG] |= 0x10;
    }
}

// readImmByte - Read immediate byte from memory
BYTE GBCPU::readImmByte()
{
    return readByte(PC + 1);
}

// ReadWord - Read word from memory
WORD GBCPU::readWord(WORD addr)
{
    // Get LSB byte first
    BYTE LSB = readByte(addr);
    BYTE MSB = readByte(addr + 1);

    WORD temp = CASTWD(MSB, LSB);
    return temp;
}

// ReadWord - Read word from memory
WORD GBCPU::readImmWord()
{
    // Get LSB byte first
    BYTE LSB = readByte(PC + 1);
    BYTE MSB = readByte(PC + 2);

    WORD temp = CASTWD(MSB, LSB);
    return temp;
}

void GBCPU::PerformDMATransfer(BYTE source)
{
    // The real source address is multiplied by 0x100 which is 256, which essentially left shift by 8. 
    for (WORD addr = 0x00; addr <= 0x9F; ++addr)
    {
        writeByte(readByte((((WORD)source << 8) | (addr))), SPRITE_TABLE_START + addr);
    }
}


// Cannot inline these because they're used in other .cpp files! Note this may have to be done on
// on the rest of these memory access functions.
// GetF - Get Status Register as a byte
BYTE GBCPU::GetF()
{
    BYTE temp = 0x00;
    temp |= ((CARRY_FLAG ? 0x01 : 0x00) << 4);
    temp |= ((HALF_CARRY_FLAG ? 0x01 : 0x00) << 5);
    temp |= ((SUBTRACT_FLAG ? 0x01 : 0x00) << 6);
    temp |= ((ZERO_FLAG ? 0x01 : 0x00) << 7);

    return temp;
}

// SetF - Set Status register from a byte
void GBCPU::SetF(BYTE F)
{
    CARRY_FLAG = (F & 0x10) ? true : false;
    HALF_CARRY_FLAG = (F & 0x20) ? true : false;
    SUBTRACT_FLAG = (F & 0x40) ? true : false;
    ZERO_FLAG = (F & 0x80) ? true : false;
}

// Get A and F as a word
WORD GBCPU::GetAF()
{
    return CASTWD(A, GetF());
}

// Set A and F from a word
void GBCPU::SetAF(WORD data)
{
    A = ((data >> 8) & 0xFF);
    SetF((data & 0xFF));
}

// Get B and C as a word
WORD GBCPU::GetBC()
{
    return CASTWD(B, C);
}

// Set B and C from a word
void GBCPU::SetBC(WORD data)
{
    B = ((data >> 8) & 0xFF);
    C = (data & 0xFF);
}

// Get D and E as a word
WORD GBCPU::GetDE()
{
    return CASTWD(D, E);
}

// Set D and E from a word
void GBCPU::SetDE(WORD data)
{
    D = ((data >> 8) & 0xFF);
    E = (data & 0xFF);
}

// Get H and L as a word
WORD GBCPU::GetHL()
{
    return CASTWD(H, L);
}

// Set H and L from a word
void GBCPU::SetHL(WORD data)
{
    H = ((data >> 8) & 0xFF);
    L = (data & 0xFF);
}
