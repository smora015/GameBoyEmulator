/*
    Name:        memory.cpp
    Author:      Sergio Morales
    Date:        08/30/2016
    Description: This file contains the definitions for functions that
                 access internal registers and memory for the CPU.

*/
#include "GBCPU.h"
#include "gameboy.h"


// writeByte - Write one byte to memory
void GBCPU::writeByte(byte data, word addr)
{
    // Check what memory region to write in
    MEM[addr] = data;

    // If memory is written to WRAM or ECHO WRAM, write to both. Note: last 512 bytes not echoed.
    if (addr >= WRAM_START && addr <= (WRAM_END - 0x200))
    {
        MEM[addr + (WRAM_ECHO_START - WRAM_START)] = data;
    }
    else if (addr >= WRAM_ECHO_START && addr <= WRAM_ECHO_END)
    {
        MEM[addr - (WRAM_ECHO_START - WRAM_START)] = data;
    }
}

void GBCPU::writeWord(word data, word addr)
{
    // Write LSB first
    MEM[addr] = (byte)(data & 0xFF);
    MEM[addr + 1] = (byte)((data >> 8) & 0xFF);


    // If memory is written to WRAM or ECHO WRAM, write to both
    if (addr >= WRAM_START && addr <= WRAM_END)
    {
        MEM[addr + (WRAM_ECHO_START - WRAM_START)] = (byte)(data & 0xFF);
        MEM[addr + 1 + (WRAM_ECHO_START - WRAM_START)] = (byte)((data >> 8) & 0xFF);
    }
    else if (addr >= WRAM_ECHO_START && addr <= WRAM_ECHO_START)
    {
        MEM[addr + (WRAM_ECHO_START - WRAM_START)] = (byte)(data & 0xFF);
        MEM[addr + 1 + (WRAM_ECHO_START - WRAM_START)] = (byte)((data >> 8) & 0xFF);
    }
}
// readByte - Read byte from memory
byte GBCPU::readByte(word addr)
{
    return MEM[addr];
}

// readImmByte - Read immediate byte from memory
byte GBCPU::readImmByte()
{
    return MEM[PC + 1];
}

// ReadWord - Read word from memory
word GBCPU::readWord(word addr)
{
    // Get LSB byte first
    byte LSB = MEM[addr];
    byte MSB = MEM[addr + 1];

    word temp = CASTWD(MSB, LSB);
    return temp;
}

// ReadWord - Read word from memory
word GBCPU::readImmWord()
{
    // Get LSB byte first
    byte LSB = MEM[PC + 1];
    byte MSB = MEM[PC + 2];

    word temp = CASTWD(MSB, LSB);
    return temp;
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
