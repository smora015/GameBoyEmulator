#ifndef _GBCARTRIDGE_H_
#define _GBCARTRIDGE_H_

#include "GBCPU.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

// @TODO: Remove class usage
class GBCartridge
{
public:
	string rom_name;

	GBCartridge();
	GBCartridge(string rom_name);

	~GBCartridge();

	void load_rom(GBCPU & cpu);
	void load_rom(string rom_name, GBCPU & cpu);

    void extract_header(GBCPU & cpu);
};

#endif /* GBCartridge.h */
