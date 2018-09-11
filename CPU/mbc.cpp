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
#include "GBPPU.h"
#include "GBCartridge.h"

// Define MBC variables
memory_model_types memory_model; // The current maximum memory model for MBC
byte current_rom_bank;           // The current switchable rom bank being used
byte current_ram_bank;           // The current switchable ram bank beign used
bool ram_bank_access_enabled;    // Indicates if RAM read/writes are enabled

// MBC1Write: Determines the appropriate bank switching selections 
void GBCPU::MBC1write(word addr, byte data)
{
    // Write to external RAM if enabled
    if ((addr >= EXTERNAL_RAM_START) && (addr <= EXTERNAL_RAM_END))
    {
        // Read the address at the current RAM bank number if enabled
        if(ram_bank_access_enabled == true)
            ext_ram[current_ram_bank * 0x2000 + (addr - EXTERNAL_RAM_START)] = data;
    }

    // A write (XXXX XXX1b) to the upper half of switchable ROM selects ROM/RAM configuration
    else if ((addr >= 0x6000) && (addr <= EXTERNAL_ROM_END))
    {
        if (data & 0x01)
        {
            cout << "MBC1 selects to 4/32 memory model (RAM banking)" << endl;
            memory_model = ram_banking; // 4 Mbit ROM / 32 Kbyte RAM max model. (4 Mbit ROM = 512 Kbyte)
        }
        else
        {
            cout << "MBC1 selects 16/8 memory model (ROM banking)" << endl;
            memory_model = rom_banking; // 16 Mbit ROM / 8 Kbyte RAM max model. (16 Mbit ROM = 2 Mbyte)
        }
    }

    // A write (XXXX XXBBb) to the lower half of switchable ROM selects either the RAM bank or upper ROM address lines
    else if ((addr >= EXTERNAL_ROM_START) && (addr <= 0x5FFF))
    {
        if (data & 0x03)
        {
            if (memory_model == ram_banking)
            {
                // Select the current RAM bank #
                current_ram_bank = (data & 0x03);
            }
            else if (memory_model == rom_banking)
            {
                // Select the two most significant bits for the ROM bank #
                current_rom_bank = (current_rom_bank & 0x1F) | ((data & 0x03) << 5);
            }
        }
    }

    // A write (XXXB BBBBb) to the upper half of internal ROM selects the lower 5 bits of the ROM bank to be used
    else if ((addr >= 0x2000) && (addr <= ROM_END))
    {
        // Do not allow bank switching to #0, #20, #40, or #60, use next instead
        if ((data == 0) || (data == 20) || (data == 40) || (data == 60))
            ++data;

        if (data & 0x1F)
        {
            current_rom_bank = (current_rom_bank & 0xE0) | (data & 0x1F);
        }
    }

    // A write (XXXX BBBBb) to the lower half of internal ROM enables/disable switchable RAM read/write
    else if ((addr >= ROM_START) && (addr <= 0x1FFF))
    {
        if (memory_model == ram_banking)
        {
            // Only allow access if we are in RAM banking mode (4/32)
            if (data & 0x0A)
            {
                // Enable RAM (default)
                ram_bank_access_enabled = true;
            }
            else
            {
                // Disable RAM
                ram_bank_access_enabled = false;
            }
        }

    }

    /*
    // If memory is written to WRAM or ECHO WRAM, write to both. Note: last 512 bytes not echoed.
    else if (addr >= WRAM_START && addr <= (WRAM_END - 0x200))
    {
        MEM[addr] = data;
        MEM[addr + (WRAM_ECHO_START - WRAM_START)] = data;
        // Only copy data when we write to WRAM_ECHO, not WRAM
    }*/
    
    // Write data from ECHO_WRAM to WRAM as well
    else if (addr >= WRAM_ECHO_START && addr <= WRAM_ECHO_END)
    {
        //MEM[addr] = data;
        MEM[addr - (WRAM_ECHO_START - WRAM_START)] = data;         
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

    // Debugging Blargg's tests. GB link registers used to output info.
    else if (addr == SIO_CONTROL && data == 0x81)
        printf("%c", readByte(SERIAL_XFER));

    else if (addr == INTERRUPT_FLAG)
    {
        //cout << "MANUALLY SETTING AN INTERRUPT!" << endl;
        MEM[addr] = data;
    }

    // Sprite DMA Transfer
    else if (addr == PPU_DMA)
        PerformDMATransfer(data);

    // Write to other areas of memory normally
    else
    {
        MEM[addr] = data;
    }

}

// MBC1read: Read data from RAM/ROM banks in a MBC1 memory model
byte GBCPU::MBC1read(word addr)
{
    // External ROM read from current bank #
    if ((addr >= EXTERNAL_ROM_START) && (addr <= EXTERNAL_ROM_END))
        return ext_rom[current_rom_bank * 0x4000 + (addr - EXTERNAL_ROM_START)];

    // External RAM read
    else if ((addr >= EXTERNAL_RAM_START) && (addr <= EXTERNAL_RAM_END) )
    {
        // Read the address at the current RAM bank number if enabled
        if (ram_bank_access_enabled == true)
            return ext_ram[current_ram_bank * 0x2000 + (addr - EXTERNAL_RAM_START)];
        else
        {
            cout << "Attempting to read external RAM when not enabled!!" << endl;
            return 0x00;
        }
            
    }

    // Read ECHO WRAM from WRAM
    else if (addr >= WRAM_ECHO_START && addr <= WRAM_ECHO_END)
        return MEM[addr - (WRAM_ECHO_START - WRAM_START)];

    // Reading from unused area in Memory Map
    else if (addr >= 0xFEA0 && addr < 0xFEFF)
    {
        cout << "Restricted memory region!" << endl;
        return 0x00;
    }

    else if (addr == JOYPAD_P1)
    {
        // TODO: Remove this once joypad functionality has been implemented
        return 0x0F;
    }

    // Read from other areas of memory normally
    else
        return MEM[addr];
}