/*  Name:        memory.cpp
    Author:      Sergio Morales [sergiomorales.me]
    Created:     August 30th, 2016
    Modified:    September 18th, 2018
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
        {
            // If we switch between checking DPAD and BUTTONS, we need to reset the currently
            // pressed button in order to prevent the program from thinking buttons are already pressed.
            //if ((MEM[JOYPAD_P1] & 0x30) != data)
                //MEM[JOYPAD_P1] = ((data & 0x30) | 0x0F);
            MEM[JOYPAD_P1] = (MEM[JOYPAD_P1] & 0x0F) | data;
        }

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
        // MBC type not supported
        string type = "Selected ROM type is currently not supported! \n";

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
            "Error 02: Unsupported ROM type",
            type.c_str(),
            NULL);

        exit(0x0002);
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
        // MBC type not supported
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "Error 02: Unsupported ROM type",
                                 "Selected ROM type is currently not supported!",
                                 NULL);

        exit(0x002);
        return 0x00;
    }

}


void GBCPU::ProcessJoyPad()
{
    // Check whether DPAD or BUTTONS are enabled and store in JOYPAD_P1.
    // This relay process is done to prevent the issue where the program
    // thinks START is pressed when it's really DOWN, etc etc. due
    // to the nature of how key presses are stored in memory
    if (!(MEM[JOYPAD_P1] & P1_BUTTONS))
        // Button key presses enabled
        MEM[JOYPAD_P1] = (MEM[JOYPAD_P1] & 0x30) | (GB_buttons & 0x0F);

    else if (!(MEM[JOYPAD_P1] & P1_DPAD))
        // DPAD key presses enabled
        MEM[JOYPAD_P1] = (MEM[JOYPAD_P1] & 0x30) | (GB_dpad & 0x0F);

    //else
        // No key presses enabled
        //MEM[JOYPAD_P1] |= 0x0F;
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
