#ifndef _GBCARTRIDGE_H_
#define _GBCARTRIDGE_H_

#include <iostream>
#include <string>
using namespace std;

class GBCartridge
{
public:
	string rom_name;

	GBCartridge();
	GBCartridge(string rom_name);

	~GBCartridge();

	void load_rom();
	void load_rom(string rom_name);
};

#endif /* GBCartridge.h */
