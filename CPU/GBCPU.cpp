#include "GBCPU.h"
#include "gameboy.h"

// Game Boy CPU class function definitions
/*

*/
GBCPU::GBCPU()
{
	// Default constructor
	init();
}

GBCPU::~GBCPU()
{
	// Free up all dynamically allocated data
}

void GBCPU::execute()
{
	byte IR = 0x00;
	cout << "Executing instruction: " << endl;
	(this->*(opcodes)[IR])();
}

void GBCPU::init()
{
	// Initialize the SP and PC
	SP = 0x0000;
	PC = 0x0000;

	// Initialize memory
	for (int i = 0; i < MAX_GB_MEMORY; ++i)
		MEM[i] = 0x00;

	// Initialize cycle count
	cycles = 0;

	// Initialize OPCODE member function table
	opcodes[0x00] = &GBCPU::OP00; opcodes[0x01] = &GBCPU::OP01; opcodes[0x02] = &GBCPU::OP02; opcodes[0x03] = &GBCPU::OP03; 
	opcodes[0x04] = &GBCPU::OP04; opcodes[0x05] = &GBCPU::OP05;	opcodes[0x06] = &GBCPU::OP06; opcodes[0x07] = &GBCPU::OP07; 
	opcodes[0x08] = &GBCPU::OP08; opcodes[0x09] = &GBCPU::OP09;	opcodes[0x0A] = &GBCPU::OP0A; opcodes[0x0B] = &GBCPU::OP0B;
	opcodes[0x0C] = &GBCPU::OP00; opcodes[0x0D] = &GBCPU::OP0D;	opcodes[0x0E] = &GBCPU::OP0E; opcodes[0x0F] = &GBCPU::OP0F;

	opcodes[0x10] = &GBCPU::OP10; opcodes[0x11] = &GBCPU::OP11; opcodes[0x12] = &GBCPU::OP12; opcodes[0x13] = &GBCPU::OP13;
	opcodes[0x14] = &GBCPU::OP14; opcodes[0x15] = &GBCPU::OP15;	opcodes[0x16] = &GBCPU::OP16; opcodes[0x17] = &GBCPU::OP17;
	opcodes[0x18] = &GBCPU::OP18; opcodes[0x19] = &GBCPU::OP19;	opcodes[0x1A] = &GBCPU::OP1A; opcodes[0x1B] = &GBCPU::OP1B;
	opcodes[0x1C] = &GBCPU::OP10; opcodes[0x1D] = &GBCPU::OP1D;	opcodes[0x1E] = &GBCPU::OP1E; opcodes[0x1F] = &GBCPU::OP1F;

	opcodes[0x20] = &GBCPU::OP20; opcodes[0x21] = &GBCPU::OP21; opcodes[0x22] = &GBCPU::OP22; opcodes[0x23] = &GBCPU::OP23; 
	opcodes[0x24] = &GBCPU::OP24; opcodes[0x25] = &GBCPU::OP25;	opcodes[0x26] = &GBCPU::OP26; opcodes[0x27] = &GBCPU::OP27; 
	opcodes[0x28] = &GBCPU::OP28; opcodes[0x29] = &GBCPU::OP29;	opcodes[0x2A] = &GBCPU::OP2A; opcodes[0x2B] = &GBCPU::OP2B;
	opcodes[0x2C] = &GBCPU::OP20; opcodes[0x2D] = &GBCPU::OP2D;	opcodes[0x2E] = &GBCPU::OP2E; opcodes[0x2F] = &GBCPU::OP2F;
		
	opcodes[0x30] = &GBCPU::OP30; opcodes[0x31] = &GBCPU::OP31; opcodes[0x32] = &GBCPU::OP32; opcodes[0x33] = &GBCPU::OP33; 
	opcodes[0x34] = &GBCPU::OP34; opcodes[0x35] = &GBCPU::OP35;	opcodes[0x36] = &GBCPU::OP36; opcodes[0x37] = &GBCPU::OP37; 
	opcodes[0x38] = &GBCPU::OP38; opcodes[0x39] = &GBCPU::OP39;	opcodes[0x3A] = &GBCPU::OP3A; opcodes[0x3B] = &GBCPU::OP3B;
	opcodes[0x3C] = &GBCPU::OP30; opcodes[0x3D] = &GBCPU::OP3D;	opcodes[0x3E] = &GBCPU::OP3E; opcodes[0x3F] = &GBCPU::OP3F;

	opcodes[0x40] = &GBCPU::OP40; opcodes[0x41] = &GBCPU::OP41; opcodes[0x42] = &GBCPU::OP42; opcodes[0x43] = &GBCPU::OP43;
	opcodes[0x44] = &GBCPU::OP44; opcodes[0x45] = &GBCPU::OP45;	opcodes[0x46] = &GBCPU::OP46; opcodes[0x47] = &GBCPU::OP47;
	opcodes[0x48] = &GBCPU::OP48; opcodes[0x49] = &GBCPU::OP49;	opcodes[0x4A] = &GBCPU::OP4A; opcodes[0x4B] = &GBCPU::OP4B;
	opcodes[0x4C] = &GBCPU::OP40; opcodes[0x4D] = &GBCPU::OP4D;	opcodes[0x4E] = &GBCPU::OP4E; opcodes[0x4F] = &GBCPU::OP4F;

	opcodes[0x50] = &GBCPU::OP50; opcodes[0x51] = &GBCPU::OP51; opcodes[0x52] = &GBCPU::OP52; opcodes[0x53] = &GBCPU::OP53;
	opcodes[0x54] = &GBCPU::OP54; opcodes[0x55] = &GBCPU::OP55;	opcodes[0x56] = &GBCPU::OP56; opcodes[0x57] = &GBCPU::OP57;
	opcodes[0x58] = &GBCPU::OP58; opcodes[0x59] = &GBCPU::OP59;	opcodes[0x5A] = &GBCPU::OP5A; opcodes[0x5B] = &GBCPU::OP5B;
	opcodes[0x5C] = &GBCPU::OP50; opcodes[0x5D] = &GBCPU::OP5D;	opcodes[0x5E] = &GBCPU::OP5E; opcodes[0x5F] = &GBCPU::OP5F;

	opcodes[0x60] = &GBCPU::OP60; opcodes[0x61] = &GBCPU::OP61; opcodes[0x62] = &GBCPU::OP62; opcodes[0x63] = &GBCPU::OP63;
	opcodes[0x64] = &GBCPU::OP64; opcodes[0x65] = &GBCPU::OP65;	opcodes[0x66] = &GBCPU::OP66; opcodes[0x67] = &GBCPU::OP67;
	opcodes[0x68] = &GBCPU::OP68; opcodes[0x69] = &GBCPU::OP69;	opcodes[0x6A] = &GBCPU::OP6A; opcodes[0x6B] = &GBCPU::OP6B;
	opcodes[0x6C] = &GBCPU::OP60; opcodes[0x6D] = &GBCPU::OP6D;	opcodes[0x6E] = &GBCPU::OP6E; opcodes[0x6F] = &GBCPU::OP6F;

	opcodes[0x70] = &GBCPU::OP70; opcodes[0x71] = &GBCPU::OP71; opcodes[0x72] = &GBCPU::OP72; opcodes[0x73] = &GBCPU::OP73;
	opcodes[0x74] = &GBCPU::OP74; opcodes[0x75] = &GBCPU::OP75;	opcodes[0x76] = &GBCPU::OP76; opcodes[0x77] = &GBCPU::OP77;
	opcodes[0x78] = &GBCPU::OP78; opcodes[0x79] = &GBCPU::OP79;	opcodes[0x7A] = &GBCPU::OP7A; opcodes[0x7B] = &GBCPU::OP7B;
	opcodes[0x7C] = &GBCPU::OP70; opcodes[0x7D] = &GBCPU::OP7D;	opcodes[0x7E] = &GBCPU::OP7E; opcodes[0x7F] = &GBCPU::OP7F;

	opcodes[0x80] = &GBCPU::OP80; opcodes[0x81] = &GBCPU::OP81; opcodes[0x82] = &GBCPU::OP82; opcodes[0x83] = &GBCPU::OP83;
	opcodes[0x84] = &GBCPU::OP84; opcodes[0x85] = &GBCPU::OP85;	opcodes[0x86] = &GBCPU::OP86; opcodes[0x87] = &GBCPU::OP87;
	opcodes[0x88] = &GBCPU::OP88; opcodes[0x89] = &GBCPU::OP89;	opcodes[0x8A] = &GBCPU::OP8A; opcodes[0x8B] = &GBCPU::OP8B;
	opcodes[0x8C] = &GBCPU::OP80; opcodes[0x8D] = &GBCPU::OP8D;	opcodes[0x8E] = &GBCPU::OP8E; opcodes[0x8F] = &GBCPU::OP8F;

	opcodes[0x90] = &GBCPU::OP90; opcodes[0x91] = &GBCPU::OP91; opcodes[0x92] = &GBCPU::OP92; opcodes[0x93] = &GBCPU::OP93;
	opcodes[0x94] = &GBCPU::OP94; opcodes[0x95] = &GBCPU::OP95;	opcodes[0x96] = &GBCPU::OP96; opcodes[0x97] = &GBCPU::OP97;
	opcodes[0x98] = &GBCPU::OP98; opcodes[0x99] = &GBCPU::OP99;	opcodes[0x9A] = &GBCPU::OP9A; opcodes[0x9B] = &GBCPU::OP9B;
	opcodes[0x9C] = &GBCPU::OP90; opcodes[0x9D] = &GBCPU::OP9D;	opcodes[0x9E] = &GBCPU::OP9E; opcodes[0x9F] = &GBCPU::OP9F;

	opcodes[0xA0] = &GBCPU::OPA0; opcodes[0xA1] = &GBCPU::OPA1; opcodes[0xA2] = &GBCPU::OPA2; opcodes[0xA3] = &GBCPU::OPA3;
	opcodes[0xA4] = &GBCPU::OPA4; opcodes[0xA5] = &GBCPU::OPA5;	opcodes[0xA6] = &GBCPU::OPA6; opcodes[0xA7] = &GBCPU::OPA7;
	opcodes[0xA8] = &GBCPU::OPA8; opcodes[0xA9] = &GBCPU::OPA9;	opcodes[0xAA] = &GBCPU::OPAA; opcodes[0xAB] = &GBCPU::OPAB;
	opcodes[0xAC] = &GBCPU::OPA0; opcodes[0xAD] = &GBCPU::OPAD;	opcodes[0xAE] = &GBCPU::OPAE; opcodes[0xAF] = &GBCPU::OPAF;

	opcodes[0xB0] = &GBCPU::OPB0; opcodes[0xB1] = &GBCPU::OPB1; opcodes[0xB2] = &GBCPU::OPB2; opcodes[0xB3] = &GBCPU::OPB3;
	opcodes[0xB4] = &GBCPU::OPB4; opcodes[0xB5] = &GBCPU::OPB5;	opcodes[0xB6] = &GBCPU::OPB6; opcodes[0xB7] = &GBCPU::OPB7;
	opcodes[0xB8] = &GBCPU::OPB8; opcodes[0xB9] = &GBCPU::OPB9;	opcodes[0xBA] = &GBCPU::OPBA; opcodes[0xBB] = &GBCPU::OPBB;
	opcodes[0xBC] = &GBCPU::OPB0; opcodes[0xBD] = &GBCPU::OPBD;	opcodes[0xBE] = &GBCPU::OPBE; opcodes[0xBF] = &GBCPU::OPBF;

	opcodes[0xC0] = &GBCPU::OPC0; opcodes[0xC1] = &GBCPU::OPC1; opcodes[0xC2] = &GBCPU::OPC2; opcodes[0xC3] = &GBCPU::OPC3;
	opcodes[0xC4] = &GBCPU::OPC4; opcodes[0xC5] = &GBCPU::OPC5;	opcodes[0xC6] = &GBCPU::OPC6; opcodes[0xC7] = &GBCPU::OPC7;
	opcodes[0xC8] = &GBCPU::OPC8; opcodes[0xC9] = &GBCPU::OPC9;	opcodes[0xCA] = &GBCPU::OPCA; opcodes[0xCB] = &GBCPU::OPCB;
	opcodes[0xCC] = &GBCPU::OPC0; opcodes[0xCD] = &GBCPU::OPCD;	opcodes[0xCE] = &GBCPU::OPCE; opcodes[0xCF] = &GBCPU::OPCF;

	opcodes[0xD0] = &GBCPU::OPD0; opcodes[0xD1] = &GBCPU::OPD1; opcodes[0xD2] = &GBCPU::OPD2; opcodes[0xD3] = &GBCPU::OPD3;
	opcodes[0xD4] = &GBCPU::OPD4; opcodes[0xD5] = &GBCPU::OPD5;	opcodes[0xD6] = &GBCPU::OPD6; opcodes[0xD7] = &GBCPU::OPD7;
	opcodes[0xD8] = &GBCPU::OPD8; opcodes[0xD9] = &GBCPU::OPD9;	opcodes[0xDA] = &GBCPU::OPDA; opcodes[0xDB] = &GBCPU::OPDB;
	opcodes[0xDC] = &GBCPU::OPD0; opcodes[0xDD] = &GBCPU::OPDD;	opcodes[0xDE] = &GBCPU::OPDE; opcodes[0xDF] = &GBCPU::OPDF;

	opcodes[0xE0] = &GBCPU::OPE0; opcodes[0xE1] = &GBCPU::OPE1; opcodes[0xE2] = &GBCPU::OPE2; opcodes[0xE3] = &GBCPU::OPE3;
	opcodes[0xE4] = &GBCPU::OPE4; opcodes[0xE5] = &GBCPU::OPE5;	opcodes[0xE6] = &GBCPU::OPE6; opcodes[0xE7] = &GBCPU::OPE7;
	opcodes[0xE8] = &GBCPU::OPE8; opcodes[0xE9] = &GBCPU::OPE9;	opcodes[0xEA] = &GBCPU::OPEA; opcodes[0xEB] = &GBCPU::OPEB;
	opcodes[0xEC] = &GBCPU::OPE0; opcodes[0xED] = &GBCPU::OPED;	opcodes[0xEE] = &GBCPU::OPEE; opcodes[0xEF] = &GBCPU::OPEF;

	opcodes[0xF0] = &GBCPU::OPF0; opcodes[0xF1] = &GBCPU::OPF1; opcodes[0xF2] = &GBCPU::OPF2; opcodes[0xF3] = &GBCPU::OPF3;
	opcodes[0xF4] = &GBCPU::OPF4; opcodes[0xF5] = &GBCPU::OPF5;	opcodes[0xF6] = &GBCPU::OPF6; opcodes[0xF7] = &GBCPU::OPF7;
	opcodes[0xF8] = &GBCPU::OPF8; opcodes[0xF9] = &GBCPU::OPF9;	opcodes[0xFA] = &GBCPU::OPFA; opcodes[0xFB] = &GBCPU::OPFB;
	opcodes[0xFC] = &GBCPU::OPF0; opcodes[0xFD] = &GBCPU::OPFD;	opcodes[0xFE] = &GBCPU::OPFE; opcodes[0xFF] = &GBCPU::OPFF;
}


