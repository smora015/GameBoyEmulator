/*
    Name:        mbc.cpp
    Author:      Sergio Morales
    Date:        08/30/2016
    Description: This file contains the logic for manipulating memory based upon the
                 corresponding Memory Bank Controller selected from $147 in the
                 cartridge header region.
*/
#include "mbc.h"
#include "GBCPU.h"
#include "GBCartridge.h"

// Define MBC variables
memory_model_types memory_model; // The current maximum memory model for MBC
byte current_rom_bank;           // The current switchable rom bank being used
byte current_ram_bank;           // The current switchable ram bank beign used
bool ram_bank_access_enabled;    // Indicates if RAM read/writes are enabled

// MBC1Write: Determines the appropriate bank switching selections 
void GBCPU::MBC1write(word addr, byte data)
{
    // A write (XXXX XXX1b) to the upper half of switchable ROM selects ROM/RAM configuration
    if ((addr >= 0x6000) && (addr < EXTERNAL_ROM_END))
    {
        if (data & 0x01)
        {
            cout << "MBC1 selects 16/8 memory model" << endl;
            memory_model = memory_model_16_8; // 16 Mbit ROM / 8 Kbyte RAM max model. (16 Mbit ROM = 2 Mbyte)
        }
        else
        {
            cout << "MBC1 selects to 4/32 memory model" << endl;
            memory_model = memory_model_4_32; // 4 Mbit ROM / 32 Kbyte RAM max model. (4 Mbit ROM = 512 Kbyte)
        }
    }

    // A write (XXXX XXBBb) to the lower half of switchable ROM selects either the RAM bank or RAM address lines
    else if ((addr > EXTERNAL_ROM_START) && (addr < 0x5FFF))
    {
        if (data & 0x03)
        {
            if (memory_model == memory_model_4_32)
            {
                current_ram_bank = (data & 0x03);
            }
            else if (memory_model == memory_model_16_8)
            {
                // @TODO: Selct the two most significant ROM address lines if in 16/8 mode
            }
        }
    }

    // A write (XXXB BBBBb) to the upper half of internal ROM selects the ROM bank to be used
    else if ((addr > 0x2000) && (addr < ROM_END))
    {
        if (data & 0x1F)
        {
            current_rom_bank = (data & 0x1F);
        }
    }

    // A write (XXXX BBBBb) to the lower half of internal ROM enables/disable switchable RAM read/write
    else if ((addr > 0x2000) && (addr < ROM_END)) // @TODO: Add condition for 4/32 mode.
    {
        if (data & 0x0A)
        {
            ram_bank_access_enabled = true;
        }
        else
        {
            ram_bank_access_enabled = false;
        }
    }

}

// MBC1read: Read data from RAM/ROM banks in a MBC1 memory model
void GBCPU::MBC1read(word addr)
{

}