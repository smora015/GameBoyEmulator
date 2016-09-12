/*
Name:        mbc.cpp
Author:      Sergio Morales
Date:        08/30/2016
Description: This file contains the logic for manipulating memory based upon the
             corresponding Memory Bank Controller selected from $147 in the
             cartridge header region.
*/
#include "GBCPU.h"


// MBC1Write: Determines the appropriate bank switching selections 
void GBCPU::MBC1write(word addr, byte data)
{
    // A write (XXXX XXX1b) to the upper half of switchable ROM selects ROM/RAM configuration
    if ((addr >= 0x6000) && (addr < EXTERNAL_ROM_END))
    {
        if (data & 0x01)
        {
            cout << "MBC1 selects 16/8 memory model" << endl;
            // @TODO: Select configuration for memory variables to be 16Mbit ROM / 8KByte RAM
        }
        else
        {
            cout << "MBC1 selects to 4/32 memory model" << endl;
            // @TODO: Select configuration for memory variables to be 4Mbit ROM/32KByte RAM
        }
    }

    // A write (XXXX XXBBb) to the lower half of switchable ROM selects either the RAM bank or RAM address lines
    else if ((addr > EXTERNAL_ROM_START) && (addr < 0x5FFF))
    {
        if (data & 0x03)
        {
            // @TODO: Select RAM bank if in 4/32 mode
            // @TODO: Selct the two most significant ROM address lines if in 16/8 mode
        }
    }

    // A write (XXXB BBBBb) to the upper half of internal ROM selects the ROM bank to be used
    else if ((addr > 0x2000) && (addr < ROM_END))
    {
        if (data & 0x1F)
        {
            // @TODO: Select the external ROM bank based on the value
        }
    }

    // A write (XXXX BBBBb) to the lower half of internal ROM enables/disable switchable RAM read/write
    else if ((addr > 0x2000) && (addr < ROM_END)) // @TODO: Add condition for 4/32 mode.
    {
        if (data & 0x0A)
        {
            // @TODO: Enable switchable RAM bank reading/writing
        }
        else
        {
            // @TODO: Disable switchable RAM bank reading/writing
        }
    }

}

// MBC1read: Read data from RAM/ROM banks in a MBC1 memory model
void MBC1read(word addr)
{

}