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

    while (file.get(c))
    {
        // Populate CPU Memory with data
        cpu.MEM[index++] = c;
    }

    file.close();
    cout << "\nFinished reading data...total size: " << index << " bytes." << endl;
    

    // Copy all of WRAM to ECHO memory region
    for (int i = WRAM_ECHO_START; i <= WRAM_ECHO_END; ++i)
    {
        cpu.MEM[i] = cpu.MEM[i - (WRAM_ECHO_START - WRAM_START)];
    }

    // Start parsing through data.
    // Ensure $0104-0133 contain the scrolling Nintendo graphic
    extract_header(cpu);

}

void GBCartridge::extract_header(GBCPU & cpu)
{
    cout << "Extracting cartridge header information..." << endl;

    // Determine cartridge type
    cout << "Cartridge type: ";
    // @TODO: Create variable that holds the current MBC type
    switch (cpu.MEM[0x147])
    {
    case 0x0:
        cout << "ROM-only!" << endl;
        break;

    case 0x1:
        // @TODO: For all MBC1, set default max memory model to 16 Mbit ROM / 8 KByte RAM
        cout << "ROM + MBC1" << endl;
        break;

    case 0x2:
        cout << "ROM + MBC1 + RAM" << endl;
        break;

    case 0x3:
        cout << "ROM + MBC1 + RAM + BATTERY" << endl;
        break;

    case 0x5:
        cout << "ROM + MBC2" << endl;
        break;

    case 0x6:
        cout << "ROM + MBC2 + BATTERY" << endl;
        break;

    case 0x8:
        cout << "ROM + RAM" << endl;
        break;

    case 0x9:
        cout << "ROM + RAM + BATTERY" << endl;
        break;

    case 0xB:
        cout << "ROM + MM01" << endl;
        break;

    case 0xC:
        cout << "ROM + MM01 + SRAM" << endl;
        break;

    case 0xD:
        cout << "ROM + MM01 + SRAM + BATTERY" << endl;
        break;

    case 0xF:
        cout << "ROM + MBC3 + TIMER + BATTERY" << endl;
        break;

    case 0x10:
        cout << "ROM + MBC3 + TIMER + RAM + BATTERY" << endl;
        break;

    case 0x11:
        cout << "ROM + MBC3" << endl;
        break;

    case 0x12:
        cout << "ROM + MBC3 + RAM" << endl;
        break;

    case 0x13:
        cout << "ROM + MBC3 + RAM + BATTERY" << endl;
        break;

    case 0x19:
        cout << "ROM + MBC5" << endl;
        break;

    case 0x1A:
        cout << "ROM + MBC5 + RAM" << endl;
        break;

    case 0x1B:
        cout << "ROM + MBC5 + RAM + BATTERY" << endl;
        break;

    case 0x1C:
        cout << "ROM + MBC5 + RUMBLE" << endl;
        break;

    case 0x1D:
        cout << "ROM + MBC5 + RUMBLE + SRAM" << endl;
        break;

    case 0x1E:
        cout << "ROM + MBC5 + RUBMEL + SRAM + BATTERY" << endl;
        break;

    case 0x1F:
        cout << "Pocket Camera" << endl;
        break;

    case 0xFD:
        cout << "Bandai TAMA5" << endl;
        break;

    case 0xFE:
        cout << "Hudson HuC - 3" << endl;
        break;

    case 0xFF:
        cout << "Hudson HuC - 1 " << endl;
        break;

    default:
        cout << "Undefined cartridge type or type not supported!" << endl;
        break;
    }

    // Determine ROM size
    // @TODO: Create variable that holds the ROM size
    cout << "ROM size: ";
    switch (cpu.MEM[0x148])
    {
    case 0x0:
        cout << "256 Kbit / 32 KByte (2 Banks)" << endl;
        break;

    case 0x1:
        cout << "512 Kbit / 64 KByte (4 Banks)" << endl;
        break;

    case 0x2:
        cout << "1 Mbit / 128 KByte (8 Banks)" << endl;
        break;

    case 0x3:
        cout << "2 Mbit / 256 KByte (16 Banks)" << endl;
        break;

    case 0x4:
        cout << "4 Mbit / 512 KByte (32 Banks)" << endl;
        break;

    case 0x5:
        cout << "8 Mbit / 1 MByte (64 Banks)" << endl;
        break;

    case 0x6:
        cout << "16 Mbit / 2 MByte (128 Banks)" << endl;
        break;

    case 0x52:
        cout << "9 Mbit / 1.1 MByte (72 Banks)" << endl;
        break;

    case 0x53:
        cout << "10Mbit/ 1.2MBytes (80 Banks)" << endl;
        break;

    case 0x54:
        cout << "12Mbit/ 1.5MBytes (96 Banks)" << endl;
        break;

    default:
        cout << "Unsupported ROM size!" << endl;
        break;
    }

    // Determine RAM size
    // @TODO: Create variable that holds the RAM size
    cout << "RAM size: ";
    switch (cpu.MEM[0x149])
    {
    case 0x0:
        cout << "None!" << endl;
        break;

    case 0x1:
        cout << "2KBytes (1 Bank) " << endl;
        break;

    case 0x2:
        cout << "8KBytes (1 Bank) " << endl;
        break;

    case 0x3:
        cout << "32KBytes (4 Bank) " << endl;
        break;

    case 0x4:
        cout << "128KBytes (16 Banks) " << endl;
        break;

     default:
        cout << "Unsupported RAM size!" << endl;
        break;
    }

}

void GBCartridge::load_rom(string rom_name, GBCPU & cpu)
{
	rom_name = rom_name;
	load_rom(cpu);

	return;
}