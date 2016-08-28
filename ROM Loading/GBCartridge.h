#ifndef _GBCARTRIDGE_H_
#define _GBCARTRIDGE_H_

#include <iostream>
#include <string>
#include <fstream>
#include "GBCPU.h"
using namespace std;

class GBCartridge
{
public:
	string rom_name;

	GBCartridge();
	GBCartridge(string rom_name);

	~GBCartridge();

	void load_rom(GBCPU & cpu);
	void load_rom(string rom_name, GBCPU & cpu);
};

#endif /* GBCartridge.h */
