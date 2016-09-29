/*
    Name:        memory.cpp
    Author:      Sergio Morales
    Date:        09/16/2016
    Description: This file contains the definitions for functions that
                 load and parse through a GameBoy rom
*/
#include "GBCartridge.h"
#include "mbc.h"
#include "gameboy.h"

// Define cartridge header variables
byte * ext_rom;         // Holds all external (switchable) RAM, to be added in memory by 16 Kbyte banks
byte * ext_ram;         // Holds all external (switchable) ROM, to be added in memory by 8 Kbyte banks
size_t rom_size;        // Actual ROM size in bytes
size_t ram_size;        // Actual RAM size in bytes
size_t ext_rom_size;    // Size of external ROM in bytes
size_t ext_ram_size;    // Size of external RAM in bytes
MBC_TYPES rom_mbc_type; // The MBC cartridge typebc_type;

void load_rom(string rom_name, GBCPU & cpu)
{
    // Initialize temp variables for reading data
    char c = 0x00;
    unsigned int num_of_bytes = 0;

    // Open file and start reading bytes. Specify read as binary. '1A' would cause EOF as text read.
    // @TODO: check for invalid files
    cout << "Loading file: " << rom_name << "..." << endl << endl;
    ifstream file(rom_name, std::ios_base::binary);

    // Get ROM bank #0 - $0000 - $3FFF
    for (int i = 0; i <= ROM_END; ++i)
    {
        // Get next byte
        file.get(c);

        // Populate CPU Memory with data
        cpu.MEM[i + ROM_START] = c;
        ++num_of_bytes;
    }

    // Extract header data to determine MBC compatability and allocate enough ROM/RAM
    // @TODO: Ensure $0104-0133 contain the scrolling Nintendo graphic
    extract_header(cpu);

    // Calculate sizes of switchable memory to allocate memory properly
    initialize_rom_ram_size();

    // Get second ROM bank if only 2 banks exist
    if (ext_rom_size == 0)
    {
        for (int i = 0; i <= (EXTERNAL_ROM_END - 0x4000); ++i)
        {
            // Get next byte
            file.get(c);

            // Populate CPU Memory with data
            cpu.MEM[i + EXTERNAL_ROM_START] = c;
            ++num_of_bytes;
        }
    }
    // Otherwise get all ROM banks if more than 2 exist
    else
    {
        for (unsigned int i = 0; i < ext_rom_size; ++i)
        {
            // Get next byte
            file.get(c);

            // Populate external rom with data
            ext_rom[i] = c;
            ++num_of_bytes;
        }
    }

    file.close();
    cout << endl << "Finished reading data...total size: " << num_of_bytes << " bytes." << endl << endl;
}

void extract_header(GBCPU & cpu)
{
    cout << "Extracting cartridge header information..." << endl;

    // Determine cartridge type
    cout << "Cartridge type: ";
    switch (cpu.MEM[0x147])
    {
    case 0x0:
        cout << "ROM-only!" << endl;
        rom_mbc_type = ROM_ONLY;
        break;

    case 0x1:
        cout << "ROM + MBC1" << endl;
        rom_mbc_type = ROM_MBC1;
        break;

    case 0x2:
        cout << "ROM + MBC1 + RAM" << endl;
        rom_mbc_type = ROM_MBC1_RAM;
        break;

    case 0x3:
        cout << "ROM + MBC1 + RAM + BATTERY" << endl;
        rom_mbc_type = ROM_MBC1_RAM_BATT;
        break;

    case 0x5:
        cout << "ROM + MBC2" << endl;
        rom_mbc_type = ROM_MBC2;
        break;

    case 0x6:
        cout << "ROM + MBC2 + BATTERY" << endl;
        rom_mbc_type = ROM_MBC2_BATT;
        break;

    case 0x8:
        cout << "ROM + RAM" << endl;
        rom_mbc_type = ROM_RAM;
        break;

    case 0x9:
        cout << "ROM + RAM + BATTERY" << endl;
        rom_mbc_type = ROM_RAM_BATT;
        break;

    case 0xB:
        cout << "ROM + MM01" << endl;
        rom_mbc_type = ROM_MM01;
        break;

    case 0xC:
        cout << "ROM + MM01 + SRAM" << endl;
        rom_mbc_type = ROM_MM01_SRAM;
        break;

    case 0xD:
        cout << "ROM + MM01 + SRAM + BATTERY" << endl;
        rom_mbc_type = ROM_MM01_SRAM_BATT;
        break;

    case 0xF:
        cout << "ROM + MBC3 + TIMER + BATTERY" << endl;
        rom_mbc_type = ROM_MBC3_TIMER_BATT;
        break;

    case 0x10:
        cout << "ROM + MBC3 + TIMER + RAM + BATTERY" << endl;
        rom_mbc_type = ROM_MBC3_TIMER_RAM_BATT;
        break;

    case 0x11:
        cout << "ROM + MBC3" << endl;
        rom_mbc_type = ROM_MBC3;
        break;

    case 0x12:
        cout << "ROM + MBC3 + RAM" << endl;
        rom_mbc_type = ROM_MBC3_RAM;
        break;

    case 0x13:
        cout << "ROM + MBC3 + RAM + BATTERY" << endl;
        rom_mbc_type = ROM_MBC3_RAM_BATT;
        break;

    case 0x19:
        cout << "ROM + MBC5" << endl;
        rom_mbc_type = ROM_MBC5;
        break;

    case 0x1A:
        cout << "ROM + MBC5 + RAM" << endl;
        rom_mbc_type = ROM_MBC5_RAM;
        break;

    case 0x1B:
        cout << "ROM + MBC5 + RAM + BATTERY" << endl;
        rom_mbc_type = ROM_MBC5_RAM_BATT;
        break;

    case 0x1C:
        cout << "ROM + MBC5 + RUMBLE" << endl;
        rom_mbc_type = ROM_MBC5_RUMBLE;
        break;

    case 0x1D:
        cout << "ROM + MBC5 + RUMBLE + SRAM" << endl;
        rom_mbc_type = ROM_MBC5_RUMBLE_SRAM;
        break;

    case 0x1E:
        cout << "ROM + MBC5 + RUBMEL + SRAM + BATTERY" << endl;
        rom_mbc_type = ROM_MBC5_RUMBLE_SRAM_BATT;
        break;

    case 0x1F:
        cout << "Pocket Camera" << endl;
        rom_mbc_type = POCKET_CAMERA;
        break;

    case 0xFD:
        cout << "Bandai TAMA5" << endl;
        rom_mbc_type = BANDAI_TAMA5;
        break;

    case 0xFE:
        cout << "Hudson HuC - 3" << endl;
        rom_mbc_type = HUDSON_HUC_3;
        break;

    case 0xFF:
        cout << "Hudson HuC - 1 " << endl;
        rom_mbc_type = HUDSON_HUC_1;
        break;

    default:
        cout << "Undefined cartridge type or type not supported!" << endl;
        rom_mbc_type = UNSUPPORTED;
        break;
    }

    // Determine ROM size
    cout << "ROM size: ";
    switch (cpu.MEM[0x148])
    {
    case 0x0:
        cout << "256 Kbit / 32 KByte (2 Banks)" << endl;

        rom_size = 32 * 1024;
        break;

    case 0x1:
        cout << "512 Kbit / 64 KByte (4 Banks)" << endl;

        rom_size = 64 * 1024;
        break;

    case 0x2:
        cout << "1 Mbit / 128 KByte (8 Banks)" << endl;

        rom_size = 128 * 1024;
        break;

    case 0x3:
        cout << "2 Mbit / 256 KByte (16 Banks)" << endl;

        rom_size = 256 * 1024;
        break;

    case 0x4:
        cout << "4 Mbit / 512 KByte (32 Banks)" << endl;

        rom_size = 512 * 1024;
        break;

    case 0x5:
        cout << "8 Mbit / 1 MByte (64 Banks)" << endl;

        rom_size = 1024 * 1024;
        break;

    case 0x6:
        cout << "16 Mbit / 2 MByte (128 Banks)" << endl;

        rom_size = 2 * 1024 * 1024;
        break;

    case 0x52:
        cout << "9 Mbit / 1.1 MByte (72 Banks)" << endl;

        rom_size = 72 * 16 * 1024;
        break;

    case 0x53:
        cout << "10Mbit/ 1.2MBytes (80 Banks)" << endl;

        rom_size = 80 * 16 * 1024;
        break;

    case 0x54:
        cout << "12Mbit/ 1.5MBytes (96 Banks)" << endl;

        rom_size = 96 * 16 * 1024;
        break;

    default:
        cout << "Unsupported ROM size!" << endl;

        rom_size = 0;
        break;
    }

    // Determine RAM size
    cout << "RAM size: ";
    switch (cpu.MEM[0x149])
    {
    case 0x0:
        cout << "None!" << endl;

        ram_size = 0;
        break;

    case 0x1:
        cout << "2KBytes (1 Bank) " << endl;

        0x2000;
        ram_size = 2 * 1024;
        break;

    case 0x2:
        cout << "8KBytes (1 Bank) " << endl;

        ram_size = 8 * 1024;
        break;

    case 0x3:
        cout << "32KBytes (4 Bank) " << endl;

        ram_size = 32 * 1024;
        break;

    case 0x4:
        cout << "128KBytes (16 Banks) " << endl;

        ram_size = 128 * 1024;
        break;

     default:
        cout << "Unsupported RAM size!" << endl;

        ram_size = 0;
        break;
    }

}


void initialize_rom_ram_size()
{
    cout << "Initializing external ROM and RAM...";

    // Initialize ROM. We allocate -1 banks for ROM because the first bank will already be in CPU memory
    ext_rom_size = (rom_size < 0x4000 ? 0x00 : rom_size - 0x4000);
    ext_ram_size = ram_size; // (ram_size < 0x2000 ? 0x00 : ram_size - 0x2000); For RAM, we do not because this refers to external only!

    // Only zero out external ROM/RAM.
    if (ext_rom_size > 0)
    {
        ext_rom = (byte *)malloc(ext_rom_size); // 16 KByte banks = 16384 bytes per piece
        memset(ext_rom, 0, ext_rom_size);
    }

    if (ext_ram_size > 0)
    {
        ext_ram = (byte *)malloc(ext_ram_size); // 8 KByte banks = 8192 bytes per piece
        memset(ext_ram, 0, ext_ram_size);
    }


    cout << "done!" << endl;
}