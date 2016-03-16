#include "GBCartridge.h"
#include "gameboy.h"


GBCartridge::GBCartridge()
{
	// Default constructor
	this->rom_name = "";
}

GBCartridge::GBCartridge(string rom_name)
{
	this->rom_name = rom_name;
}

GBCartridge::~GBCartridge()
{
	// Free up all dynamically allocated data
}

void GBCartridge::load_rom()
{
	cout << "Loading file: " << rom_name << "..." << endl;

}

void GBCartridge::load_rom(string rom_name)
{
	this->rom_name = rom_name;
	this->load_rom();

	return;
}