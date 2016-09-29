#ifndef MBC_H
#define MBC_H

#include "gameboy.h"

typedef enum memory_model_types
{
    rom_banking,
    ram_banking
} memory_model_types;

extern memory_model_types memory_model; // The current maximum memory model for MBC
extern byte current_rom_bank;           // The current switchable rom bank being used
extern byte current_ram_bank;           // The current switchable ram bank beign used
extern bool ram_bank_access_enabled;    // Indicates if RAM read/writes are enabled
//extern bool rom_bank_access_enabled;


#endif
