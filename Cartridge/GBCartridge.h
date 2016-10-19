#ifndef _GBCARTRIDGE_H_
#define _GBCARTRIDGE_H_

#include "GBCPU.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

typedef enum MBC_TYPES
{
    ROM_ONLY,
    ROM_MBC1,
    ROM_MBC1_RAM,
    ROM_MBC1_RAM_BATT,
    ROM_MBC2,
    ROM_MBC2_BATT,
    ROM_RAM,
    ROM_RAM_BATT,
    ROM_MM01,
    ROM_MM01_SRAM,
    ROM_MM01_SRAM_BATT,
    ROM_MBC3_TIMER_BATT,
    ROM_MBC3_TIMER_RAM_BATT,
    ROM_MBC3,
    ROM_MBC3_RAM,
    ROM_MBC3_RAM_BATT,
    ROM_MBC5,
    ROM_MBC5_RAM,
    ROM_MBC5_RAM_BATT,
    ROM_MBC5_RUMBLE,
    ROM_MBC5_RUMBLE_SRAM,
    ROM_MBC5_RUMBLE_SRAM_BATT,
    POCKET_CAMERA,
    BANDAI_TAMA5,
    HUDSON_HUC_3,
    HUDSON_HUC_1,
    UNSUPPORTED
} MBC_TYPES;

extern byte * ext_rom;         // Holds all external (switchable) RAM, to be added in memory by 16 Kbyte banks
extern byte * ext_ram;         // Holds all external (switchable) ROM, to be added in memory by 8 Kbyte banks
extern size_t rom_size;        // Actual total ROM size in bytes
extern size_t ram_size;        // Actual total RAM size in bytes
extern size_t ext_rom_size;    // Size of external ROM in bytes
extern size_t ext_ram_size;    // Size of external RAM in bytes
extern MBC_TYPES rom_mbc_type; // The MBC cartridge type

void load_rom(string rom_name, GBCPU & cpu);

void extract_header(GBCPU & cpu);
void initialize_rom_ram_size();

#endif /* GBCartridge.h */
