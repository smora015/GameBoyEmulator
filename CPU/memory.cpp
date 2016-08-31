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
inline void GBCPU::writeByte(byte data, word addr)
{
    // Check what memory region to write in
    MEM[addr] = data;
}

// readByte - Read immediate byte from memory
inline byte GBCPU::readByte(word addr)
{
    return MEM[addr];
}

// ReadWord - Read immediate word from memory
inline word GBCPU::readWord(word addr)
{
    // Get LSB byte first
    byte LSB = MEM[addr];
    byte MSB = MEM[addr + 1];

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
inline word GBCPU::GetAF()
{
    return CASTWD(A, GetF());
}

// Set A and F from a word
inline void GBCPU::SetAF(word data)
{
    A = ((data >> 8) & 0xFF);
    SetF((data & 0xFF));
}

// Get B and C as a word
inline word GBCPU::GetBC()
{
    return CASTWD(B, C);
}

// Set B and C from a word
inline void GBCPU::SetBC(word data)
{
    B = ((data >> 8) & 0xFF);
    C = (data & 0xFF);
}

// Get D and E as a word
inline word GBCPU::GetDE()
{
    return CASTWD(D, E);
}

// Set D and E from a word
inline void GBCPU::SetDE(word data)
{
    D = ((data >> 8) & 0xFF);
    E = (data & 0xFF);
}

// Get H and L as a word
inline word GBCPU::GetHL()
{
    return CASTWD(H, L);
}

// Set H and L from a word
inline void GBCPU::SetHL(word data)
{
    H = ((data >> 8) & 0xFF);
    L = (data & 0xFF);
}
