/*
    Name:        memory.cpp
    Author:      Sergio Morales
    Date:        08/30/2016
    Description: This file contains the definitions for functions that
                 access internal registers and memory for the CPU.

*/
#include "GBCPU.h"
#include "GBPPU.h"
#include "GBCartridge.h"
#include "gameboy.h"


// writeByte - Write one byte to memory
void GBCPU::writeByte(byte data, word addr)
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

void GBCPU::writeWord(word data, word addr)
{
    // Write LSB first
    writeByte((byte)(data & 0xFF), addr);
    writeByte((byte)((data >> 8) & 0xFF), addr + 1);
}

// readByte - Read byte from memory
byte GBCPU::readByte(word addr)
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
            // TODO: Remove this once joypad functionality has been implemented
            return 0x0F;
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

// readImmByte - Read immediate byte from memory
byte GBCPU::readImmByte()
{
    return readByte(PC + 1);
}

// ReadWord - Read word from memory
word GBCPU::readWord(word addr)
{
    // Get LSB byte first
    byte LSB = readByte(addr);
    byte MSB = readByte(addr + 1);

    word temp = CASTWD(MSB, LSB);
    return temp;
}

// ReadWord - Read word from memory
word GBCPU::readImmWord()
{
    // Get LSB byte first
    byte LSB = readByte(PC + 1);
    byte MSB = readByte(PC + 2);

    word temp = CASTWD(MSB, LSB);
    return temp;
}

void GBCPU::PerformDMATransfer(byte source)
{
    // The real source address is multiplied by 0x100 which is 256, which essentially left shift by 8. 
    for (word addr = 0x00; addr <= 0x9F; ++addr)
    {
        writeByte(readByte((((word)source << 8) | (addr))), SPRITE_TABLE_START + addr);
    }
}


// Cannot inline these because they're used in other .cpp files! Note this may have to be done on
// on the rest of these memory access functions.
// GetF - Get Status Register as a byte
byte GBCPU::GetF()
{
    byte temp = 0x00;
    temp |= ((CARRY_FLAG ? 0x01 : 0x00) << 4);
    temp |= ((HALF_CARRY_FLAG ? 0x01 : 0x00) << 5);
    temp |= ((SUBTRACT_FLAG ? 0x01 : 0x00) << 6);
    temp |= ((ZERO_FLAG ? 0x01 : 0x00) << 7);

    return temp;
}

// SetF - Set Status register from a byte
void GBCPU::SetF(byte F)
{
    CARRY_FLAG = (F & 0x10) ? true : false;
    HALF_CARRY_FLAG = (F & 0x20) ? true : false;
    SUBTRACT_FLAG = (F & 0x40) ? true : false;
    ZERO_FLAG = (F & 0x80) ? true : false;
}

// Get A and F as a word
word GBCPU::GetAF()
{
    return CASTWD(A, GetF());
}

// Set A and F from a word
void GBCPU::SetAF(word data)
{
    A = ((data >> 8) & 0xFF);
    SetF((data & 0xFF));
}

// Get B and C as a word
word GBCPU::GetBC()
{
    return CASTWD(B, C);
}

// Set B and C from a word
void GBCPU::SetBC(word data)
{
    B = ((data >> 8) & 0xFF);
    C = (data & 0xFF);
}

// Get D and E as a word
word GBCPU::GetDE()
{
    return CASTWD(D, E);
}

// Set D and E from a word
void GBCPU::SetDE(word data)
{
    D = ((data >> 8) & 0xFF);
    E = (data & 0xFF);
}

// Get H and L as a word
word GBCPU::GetHL()
{
    return CASTWD(H, L);
}

// Set H and L from a word
void GBCPU::SetHL(word data)
{
    H = ((data >> 8) & 0xFF);
    L = (data & 0xFF);
}
