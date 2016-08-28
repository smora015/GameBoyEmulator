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

void GBCartridge::load_rom(GBCPU & cpu)
{
    char c;
    int index = 0;
	cout << "Loading file: " << rom_name << "..." << endl;

    // Open file and start reading bytes. Specify read as binary. '1A' would cause EOF as text read.
    ifstream file(rom_name, std::ios_base::binary);

    // Populate CPU Memory with data
    while (file.get(c))
    {
        cpu.MEM[index++] = c;
        //cout << c << " ";
        //if (index % 16 == 0)
        //    cout << endl;
    }

    file.close();
    cout << "\nFinished reading data...total size: " << index << " bytes." << endl;


    // Start parsing through data.
    // Ensure $0104-0133 contain the scrolling Nintendo graphic

    cout << "Cartridge type: " << (short)cpu.MEM[0x147] << endl;
    cout << "ROM size: " << (short)cpu.MEM[0x148] << endl;
    cout << "RAM size: " << (short)cpu.MEM[0x149] << endl;

}

void GBCartridge::load_rom(string rom_name, GBCPU & cpu)
{
	rom_name = rom_name;
	load_rom(cpu);

	return;
}