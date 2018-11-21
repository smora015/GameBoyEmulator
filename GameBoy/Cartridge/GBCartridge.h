#ifndef _GBCARTRIDGE_H
#define _GBCARTRIDGE_H

#include "GBCPU.h"
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

void load_rom(string rom_name, GBCPU & cpu);

void extract_header(GBCPU & cpu);
void initialize_rom_ram_size();

#endif /* GBCartridge.h */
