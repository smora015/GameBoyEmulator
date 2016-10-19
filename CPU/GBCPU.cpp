/*
Name:        GBCPU.cpp
Author:      Sergio Morales
Date:        09/27/2016
Description: This file contains the functions utilized for initializing
             and executing CPU opcodes.
*/

#include "GBCPU.h"
#include "gameboy.h"

GBCPU::GBCPU()
{
	// Default constructor
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
    opcodes[0x0C] = &GBCPU::OP0C; opcodes[0x0D] = &GBCPU::OP0D;	opcodes[0x0E] = &GBCPU::OP0E; opcodes[0x0F] = &GBCPU::OP0F;

    opcodes[0x10] = &GBCPU::OP10; opcodes[0x11] = &GBCPU::OP11; opcodes[0x12] = &GBCPU::OP12; opcodes[0x13] = &GBCPU::OP13;
    opcodes[0x14] = &GBCPU::OP14; opcodes[0x15] = &GBCPU::OP15;	opcodes[0x16] = &GBCPU::OP16; opcodes[0x17] = &GBCPU::OP17;
    opcodes[0x18] = &GBCPU::OP18; opcodes[0x19] = &GBCPU::OP19;	opcodes[0x1A] = &GBCPU::OP1A; opcodes[0x1B] = &GBCPU::OP1B;
    opcodes[0x1C] = &GBCPU::OP1C; opcodes[0x1D] = &GBCPU::OP1D;	opcodes[0x1E] = &GBCPU::OP1E; opcodes[0x1F] = &GBCPU::OP1F;

    opcodes[0x20] = &GBCPU::OP20; opcodes[0x21] = &GBCPU::OP21; opcodes[0x22] = &GBCPU::OP22; opcodes[0x23] = &GBCPU::OP23;
    opcodes[0x24] = &GBCPU::OP24; opcodes[0x25] = &GBCPU::OP25;	opcodes[0x26] = &GBCPU::OP26; opcodes[0x27] = &GBCPU::OP27;
    opcodes[0x28] = &GBCPU::OP28; opcodes[0x29] = &GBCPU::OP29;	opcodes[0x2A] = &GBCPU::OP2A; opcodes[0x2B] = &GBCPU::OP2B;
    opcodes[0x2C] = &GBCPU::OP2C; opcodes[0x2D] = &GBCPU::OP2D;	opcodes[0x2E] = &GBCPU::OP2E; opcodes[0x2F] = &GBCPU::OP2F;

    opcodes[0x30] = &GBCPU::OP30; opcodes[0x31] = &GBCPU::OP31; opcodes[0x32] = &GBCPU::OP32; opcodes[0x33] = &GBCPU::OP33;
    opcodes[0x34] = &GBCPU::OP34; opcodes[0x35] = &GBCPU::OP35;	opcodes[0x36] = &GBCPU::OP36; opcodes[0x37] = &GBCPU::OP37;
    opcodes[0x38] = &GBCPU::OP38; opcodes[0x39] = &GBCPU::OP39;	opcodes[0x3A] = &GBCPU::OP3A; opcodes[0x3B] = &GBCPU::OP3B;
    opcodes[0x3C] = &GBCPU::OP3C; opcodes[0x3D] = &GBCPU::OP3D;	opcodes[0x3E] = &GBCPU::OP3E; opcodes[0x3F] = &GBCPU::OP3F;

    opcodes[0x40] = &GBCPU::OP40; opcodes[0x41] = &GBCPU::OP41; opcodes[0x42] = &GBCPU::OP42; opcodes[0x43] = &GBCPU::OP43;
    opcodes[0x44] = &GBCPU::OP44; opcodes[0x45] = &GBCPU::OP45;	opcodes[0x46] = &GBCPU::OP46; opcodes[0x47] = &GBCPU::OP47;
    opcodes[0x48] = &GBCPU::OP48; opcodes[0x49] = &GBCPU::OP49;	opcodes[0x4A] = &GBCPU::OP4A; opcodes[0x4B] = &GBCPU::OP4B;
    opcodes[0x4C] = &GBCPU::OP4C; opcodes[0x4D] = &GBCPU::OP4D;	opcodes[0x4E] = &GBCPU::OP4E; opcodes[0x4F] = &GBCPU::OP4F;

    opcodes[0x50] = &GBCPU::OP50; opcodes[0x51] = &GBCPU::OP51; opcodes[0x52] = &GBCPU::OP52; opcodes[0x53] = &GBCPU::OP53;
    opcodes[0x54] = &GBCPU::OP54; opcodes[0x55] = &GBCPU::OP55;	opcodes[0x56] = &GBCPU::OP56; opcodes[0x57] = &GBCPU::OP57;
    opcodes[0x58] = &GBCPU::OP58; opcodes[0x59] = &GBCPU::OP59;	opcodes[0x5A] = &GBCPU::OP5A; opcodes[0x5B] = &GBCPU::OP5B;
    opcodes[0x5C] = &GBCPU::OP5C; opcodes[0x5D] = &GBCPU::OP5D;	opcodes[0x5E] = &GBCPU::OP5E; opcodes[0x5F] = &GBCPU::OP5F;

    opcodes[0x60] = &GBCPU::OP60; opcodes[0x61] = &GBCPU::OP61; opcodes[0x62] = &GBCPU::OP62; opcodes[0x63] = &GBCPU::OP63;
    opcodes[0x64] = &GBCPU::OP64; opcodes[0x65] = &GBCPU::OP65;	opcodes[0x66] = &GBCPU::OP66; opcodes[0x67] = &GBCPU::OP67;
    opcodes[0x68] = &GBCPU::OP68; opcodes[0x69] = &GBCPU::OP69;	opcodes[0x6A] = &GBCPU::OP6A; opcodes[0x6B] = &GBCPU::OP6B;
    opcodes[0x6C] = &GBCPU::OP6C; opcodes[0x6D] = &GBCPU::OP6D;	opcodes[0x6E] = &GBCPU::OP6E; opcodes[0x6F] = &GBCPU::OP6F;

    opcodes[0x70] = &GBCPU::OP70; opcodes[0x71] = &GBCPU::OP71; opcodes[0x72] = &GBCPU::OP72; opcodes[0x73] = &GBCPU::OP73;
    opcodes[0x74] = &GBCPU::OP74; opcodes[0x75] = &GBCPU::OP75;	opcodes[0x76] = &GBCPU::OP76; opcodes[0x77] = &GBCPU::OP77;
    opcodes[0x78] = &GBCPU::OP78; opcodes[0x79] = &GBCPU::OP79;	opcodes[0x7A] = &GBCPU::OP7A; opcodes[0x7B] = &GBCPU::OP7B;
    opcodes[0x7C] = &GBCPU::OP7C; opcodes[0x7D] = &GBCPU::OP7D;	opcodes[0x7E] = &GBCPU::OP7E; opcodes[0x7F] = &GBCPU::OP7F;

    opcodes[0x80] = &GBCPU::OP80; opcodes[0x81] = &GBCPU::OP81; opcodes[0x82] = &GBCPU::OP82; opcodes[0x83] = &GBCPU::OP83;
    opcodes[0x84] = &GBCPU::OP84; opcodes[0x85] = &GBCPU::OP85;	opcodes[0x86] = &GBCPU::OP86; opcodes[0x87] = &GBCPU::OP87;
    opcodes[0x88] = &GBCPU::OP88; opcodes[0x89] = &GBCPU::OP89;	opcodes[0x8A] = &GBCPU::OP8A; opcodes[0x8B] = &GBCPU::OP8B;
    opcodes[0x8C] = &GBCPU::OP8C; opcodes[0x8D] = &GBCPU::OP8D;	opcodes[0x8E] = &GBCPU::OP8E; opcodes[0x8F] = &GBCPU::OP8F;

    opcodes[0x90] = &GBCPU::OP90; opcodes[0x91] = &GBCPU::OP91; opcodes[0x92] = &GBCPU::OP92; opcodes[0x93] = &GBCPU::OP93;
    opcodes[0x94] = &GBCPU::OP94; opcodes[0x95] = &GBCPU::OP95;	opcodes[0x96] = &GBCPU::OP96; opcodes[0x97] = &GBCPU::OP97;
    opcodes[0x98] = &GBCPU::OP98; opcodes[0x99] = &GBCPU::OP99;	opcodes[0x9A] = &GBCPU::OP9A; opcodes[0x9B] = &GBCPU::OP9B;
    opcodes[0x9C] = &GBCPU::OP9C; opcodes[0x9D] = &GBCPU::OP9D;	opcodes[0x9E] = &GBCPU::OP9E; opcodes[0x9F] = &GBCPU::OP9F;

    opcodes[0xA0] = &GBCPU::OPA0; opcodes[0xA1] = &GBCPU::OPA1; opcodes[0xA2] = &GBCPU::OPA2; opcodes[0xA3] = &GBCPU::OPA3;
    opcodes[0xA4] = &GBCPU::OPA4; opcodes[0xA5] = &GBCPU::OPA5;	opcodes[0xA6] = &GBCPU::OPA6; opcodes[0xA7] = &GBCPU::OPA7;
    opcodes[0xA8] = &GBCPU::OPA8; opcodes[0xA9] = &GBCPU::OPA9;	opcodes[0xAA] = &GBCPU::OPAA; opcodes[0xAB] = &GBCPU::OPAB;
    opcodes[0xAC] = &GBCPU::OPAC; opcodes[0xAD] = &GBCPU::OPAD;	opcodes[0xAE] = &GBCPU::OPAE; opcodes[0xAF] = &GBCPU::OPAF;

    opcodes[0xB0] = &GBCPU::OPB0; opcodes[0xB1] = &GBCPU::OPB1; opcodes[0xB2] = &GBCPU::OPB2; opcodes[0xB3] = &GBCPU::OPB3;
    opcodes[0xB4] = &GBCPU::OPB4; opcodes[0xB5] = &GBCPU::OPB5;	opcodes[0xB6] = &GBCPU::OPB6; opcodes[0xB7] = &GBCPU::OPB7;
    opcodes[0xB8] = &GBCPU::OPB8; opcodes[0xB9] = &GBCPU::OPB9;	opcodes[0xBA] = &GBCPU::OPBA; opcodes[0xBB] = &GBCPU::OPBB;
    opcodes[0xBC] = &GBCPU::OPBC; opcodes[0xBD] = &GBCPU::OPBD;	opcodes[0xBE] = &GBCPU::OPBE; opcodes[0xBF] = &GBCPU::OPBF;

    opcodes[0xC0] = &GBCPU::OPC0; opcodes[0xC1] = &GBCPU::OPC1; opcodes[0xC2] = &GBCPU::OPC2; opcodes[0xC3] = &GBCPU::OPC3;
    opcodes[0xC4] = &GBCPU::OPC4; opcodes[0xC5] = &GBCPU::OPC5;	opcodes[0xC6] = &GBCPU::OPC6; opcodes[0xC7] = &GBCPU::OPC7;
    opcodes[0xC8] = &GBCPU::OPC8; opcodes[0xC9] = &GBCPU::OPC9;	opcodes[0xCA] = &GBCPU::OPCA; opcodes[0xCB] = &GBCPU::OPCB;
    opcodes[0xCC] = &GBCPU::OPCC; opcodes[0xCD] = &GBCPU::OPCD;	opcodes[0xCE] = &GBCPU::OPCE; opcodes[0xCF] = &GBCPU::OPCF;

    opcodes[0xD0] = &GBCPU::OPD0; opcodes[0xD1] = &GBCPU::OPD1; opcodes[0xD2] = &GBCPU::OPD2; opcodes[0xD3] = &GBCPU::OPD3;
    opcodes[0xD4] = &GBCPU::OPD4; opcodes[0xD5] = &GBCPU::OPD5;	opcodes[0xD6] = &GBCPU::OPD6; opcodes[0xD7] = &GBCPU::OPD7;
    opcodes[0xD8] = &GBCPU::OPD8; opcodes[0xD9] = &GBCPU::OPD9;	opcodes[0xDA] = &GBCPU::OPDA; opcodes[0xDB] = &GBCPU::OPDB;
    opcodes[0xDC] = &GBCPU::OPDC; opcodes[0xDD] = &GBCPU::OPDD;	opcodes[0xDE] = &GBCPU::OPDE; opcodes[0xDF] = &GBCPU::OPDF;

    opcodes[0xE0] = &GBCPU::OPE0; opcodes[0xE1] = &GBCPU::OPE1; opcodes[0xE2] = &GBCPU::OPE2; opcodes[0xE3] = &GBCPU::OPE3;
    opcodes[0xE4] = &GBCPU::OPE4; opcodes[0xE5] = &GBCPU::OPE5;	opcodes[0xE6] = &GBCPU::OPE6; opcodes[0xE7] = &GBCPU::OPE7;
    opcodes[0xE8] = &GBCPU::OPE8; opcodes[0xE9] = &GBCPU::OPE9;	opcodes[0xEA] = &GBCPU::OPEA; opcodes[0xEB] = &GBCPU::OPEB;
    opcodes[0xEC] = &GBCPU::OPEC; opcodes[0xED] = &GBCPU::OPED;	opcodes[0xEE] = &GBCPU::OPEE; opcodes[0xEF] = &GBCPU::OPEF;

    opcodes[0xF0] = &GBCPU::OPF0; opcodes[0xF1] = &GBCPU::OPF1; opcodes[0xF2] = &GBCPU::OPF2; opcodes[0xF3] = &GBCPU::OPF3;
    opcodes[0xF4] = &GBCPU::OPF4; opcodes[0xF5] = &GBCPU::OPF5;	opcodes[0xF6] = &GBCPU::OPF6; opcodes[0xF7] = &GBCPU::OPF7;
    opcodes[0xF8] = &GBCPU::OPF8; opcodes[0xF9] = &GBCPU::OPF9;	opcodes[0xFA] = &GBCPU::OPFA; opcodes[0xFB] = &GBCPU::OPFB;
    opcodes[0xFC] = &GBCPU::OPFC; opcodes[0xFD] = &GBCPU::OPFD;	opcodes[0xFE] = &GBCPU::OPFE; opcodes[0xFF] = &GBCPU::OPFF;

    // Initialize CB-prefix OPCODE member function table
    CBopcodes[0x00] = &GBCPU::CBOP00; CBopcodes[0x01] = &GBCPU::CBOP01; CBopcodes[0x02] = &GBCPU::CBOP02; CBopcodes[0x03] = &GBCPU::CBOP03;
    CBopcodes[0x04] = &GBCPU::CBOP04; CBopcodes[0x05] = &GBCPU::CBOP05;	CBopcodes[0x06] = &GBCPU::CBOP06; CBopcodes[0x07] = &GBCPU::CBOP07;
    CBopcodes[0x08] = &GBCPU::CBOP08; CBopcodes[0x09] = &GBCPU::CBOP09;	CBopcodes[0x0A] = &GBCPU::CBOP0A; CBopcodes[0x0B] = &GBCPU::CBOP0B;
    CBopcodes[0x0C] = &GBCPU::CBOP0C; CBopcodes[0x0D] = &GBCPU::CBOP0D;	CBopcodes[0x0E] = &GBCPU::CBOP0E; CBopcodes[0x0F] = &GBCPU::CBOP0F;

    CBopcodes[0x10] = &GBCPU::CBOP10; CBopcodes[0x11] = &GBCPU::CBOP11; CBopcodes[0x12] = &GBCPU::CBOP12; CBopcodes[0x13] = &GBCPU::CBOP13;
    CBopcodes[0x14] = &GBCPU::CBOP14; CBopcodes[0x15] = &GBCPU::CBOP15;	CBopcodes[0x16] = &GBCPU::CBOP16; CBopcodes[0x17] = &GBCPU::CBOP17;
    CBopcodes[0x18] = &GBCPU::CBOP18; CBopcodes[0x19] = &GBCPU::CBOP19;	CBopcodes[0x1A] = &GBCPU::CBOP1A; CBopcodes[0x1B] = &GBCPU::CBOP1B;
    CBopcodes[0x1C] = &GBCPU::CBOP1C; CBopcodes[0x1D] = &GBCPU::CBOP1D;	CBopcodes[0x1E] = &GBCPU::CBOP1E; CBopcodes[0x1F] = &GBCPU::CBOP1F;

    CBopcodes[0x20] = &GBCPU::CBOP20; CBopcodes[0x21] = &GBCPU::CBOP21; CBopcodes[0x22] = &GBCPU::CBOP22; CBopcodes[0x23] = &GBCPU::CBOP23;
    CBopcodes[0x24] = &GBCPU::CBOP24; CBopcodes[0x25] = &GBCPU::CBOP25;	CBopcodes[0x26] = &GBCPU::CBOP26; CBopcodes[0x27] = &GBCPU::CBOP27;
    CBopcodes[0x28] = &GBCPU::CBOP28; CBopcodes[0x29] = &GBCPU::CBOP29;	CBopcodes[0x2A] = &GBCPU::CBOP2A; CBopcodes[0x2B] = &GBCPU::CBOP2B;
    CBopcodes[0x2C] = &GBCPU::CBOP2C; CBopcodes[0x2D] = &GBCPU::CBOP2D;	CBopcodes[0x2E] = &GBCPU::CBOP2E; CBopcodes[0x2F] = &GBCPU::CBOP2F;

    CBopcodes[0x30] = &GBCPU::CBOP30; CBopcodes[0x31] = &GBCPU::CBOP31; CBopcodes[0x32] = &GBCPU::CBOP32; CBopcodes[0x33] = &GBCPU::CBOP33;
    CBopcodes[0x34] = &GBCPU::CBOP34; CBopcodes[0x35] = &GBCPU::CBOP35;	CBopcodes[0x36] = &GBCPU::CBOP36; CBopcodes[0x37] = &GBCPU::CBOP37;
    CBopcodes[0x38] = &GBCPU::CBOP38; CBopcodes[0x39] = &GBCPU::CBOP39;	CBopcodes[0x3A] = &GBCPU::CBOP3A; CBopcodes[0x3B] = &GBCPU::CBOP3B;
    CBopcodes[0x3C] = &GBCPU::CBOP3C; CBopcodes[0x3D] = &GBCPU::CBOP3D;	CBopcodes[0x3E] = &GBCPU::CBOP3E; CBopcodes[0x3F] = &GBCPU::CBOP3F;

    CBopcodes[0x40] = &GBCPU::CBOP40; CBopcodes[0x41] = &GBCPU::CBOP41; CBopcodes[0x42] = &GBCPU::CBOP42; CBopcodes[0x43] = &GBCPU::CBOP43;
    CBopcodes[0x44] = &GBCPU::CBOP44; CBopcodes[0x45] = &GBCPU::CBOP45;	CBopcodes[0x46] = &GBCPU::CBOP46; CBopcodes[0x47] = &GBCPU::CBOP47;
    CBopcodes[0x48] = &GBCPU::CBOP48; CBopcodes[0x49] = &GBCPU::CBOP49;	CBopcodes[0x4A] = &GBCPU::CBOP4A; CBopcodes[0x4B] = &GBCPU::CBOP4B;
    CBopcodes[0x4C] = &GBCPU::CBOP4C; CBopcodes[0x4D] = &GBCPU::CBOP4D;	CBopcodes[0x4E] = &GBCPU::CBOP4E; CBopcodes[0x4F] = &GBCPU::CBOP4F;

    CBopcodes[0x50] = &GBCPU::CBOP50; CBopcodes[0x51] = &GBCPU::CBOP51; CBopcodes[0x52] = &GBCPU::CBOP52; CBopcodes[0x53] = &GBCPU::CBOP53;
    CBopcodes[0x54] = &GBCPU::CBOP54; CBopcodes[0x55] = &GBCPU::CBOP55;	CBopcodes[0x56] = &GBCPU::CBOP56; CBopcodes[0x57] = &GBCPU::CBOP57;
    CBopcodes[0x58] = &GBCPU::CBOP58; CBopcodes[0x59] = &GBCPU::CBOP59;	CBopcodes[0x5A] = &GBCPU::CBOP5A; CBopcodes[0x5B] = &GBCPU::CBOP5B;
    CBopcodes[0x5C] = &GBCPU::CBOP5C; CBopcodes[0x5D] = &GBCPU::CBOP5D;	CBopcodes[0x5E] = &GBCPU::CBOP5E; CBopcodes[0x5F] = &GBCPU::CBOP5F;

    CBopcodes[0x60] = &GBCPU::CBOP60; CBopcodes[0x61] = &GBCPU::CBOP61; CBopcodes[0x62] = &GBCPU::CBOP62; CBopcodes[0x63] = &GBCPU::CBOP63;
    CBopcodes[0x64] = &GBCPU::CBOP64; CBopcodes[0x65] = &GBCPU::CBOP65;	CBopcodes[0x66] = &GBCPU::CBOP66; CBopcodes[0x67] = &GBCPU::CBOP67;
    CBopcodes[0x68] = &GBCPU::CBOP68; CBopcodes[0x69] = &GBCPU::CBOP69;	CBopcodes[0x6A] = &GBCPU::CBOP6A; CBopcodes[0x6B] = &GBCPU::CBOP6B;
    CBopcodes[0x6C] = &GBCPU::CBOP6C; CBopcodes[0x6D] = &GBCPU::CBOP6D;	CBopcodes[0x6E] = &GBCPU::CBOP6E; CBopcodes[0x6F] = &GBCPU::CBOP6F;

    CBopcodes[0x70] = &GBCPU::CBOP70; CBopcodes[0x71] = &GBCPU::CBOP71; CBopcodes[0x72] = &GBCPU::CBOP72; CBopcodes[0x73] = &GBCPU::CBOP73;
    CBopcodes[0x74] = &GBCPU::CBOP74; CBopcodes[0x75] = &GBCPU::CBOP75;	CBopcodes[0x76] = &GBCPU::CBOP76; CBopcodes[0x77] = &GBCPU::CBOP77;
    CBopcodes[0x78] = &GBCPU::CBOP78; CBopcodes[0x79] = &GBCPU::CBOP79;	CBopcodes[0x7A] = &GBCPU::CBOP7A; CBopcodes[0x7B] = &GBCPU::CBOP7B;
    CBopcodes[0x7C] = &GBCPU::CBOP7C; CBopcodes[0x7D] = &GBCPU::CBOP7D;	CBopcodes[0x7E] = &GBCPU::CBOP7E; CBopcodes[0x7F] = &GBCPU::CBOP7F;

    CBopcodes[0x80] = &GBCPU::CBOP80; CBopcodes[0x81] = &GBCPU::CBOP81; CBopcodes[0x82] = &GBCPU::CBOP82; CBopcodes[0x83] = &GBCPU::CBOP83;
    CBopcodes[0x84] = &GBCPU::CBOP84; CBopcodes[0x85] = &GBCPU::CBOP85;	CBopcodes[0x86] = &GBCPU::CBOP86; CBopcodes[0x87] = &GBCPU::CBOP87;
    CBopcodes[0x88] = &GBCPU::CBOP88; CBopcodes[0x89] = &GBCPU::CBOP89;	CBopcodes[0x8A] = &GBCPU::CBOP8A; CBopcodes[0x8B] = &GBCPU::CBOP8B;
    CBopcodes[0x8C] = &GBCPU::CBOP8C; CBopcodes[0x8D] = &GBCPU::CBOP8D;	CBopcodes[0x8E] = &GBCPU::CBOP8E; CBopcodes[0x8F] = &GBCPU::CBOP8F;

    CBopcodes[0x90] = &GBCPU::CBOP90; CBopcodes[0x91] = &GBCPU::CBOP91; CBopcodes[0x92] = &GBCPU::CBOP92; CBopcodes[0x93] = &GBCPU::CBOP93;
    CBopcodes[0x94] = &GBCPU::CBOP94; CBopcodes[0x95] = &GBCPU::CBOP95;	CBopcodes[0x96] = &GBCPU::CBOP96; CBopcodes[0x97] = &GBCPU::CBOP97;
    CBopcodes[0x98] = &GBCPU::CBOP98; CBopcodes[0x99] = &GBCPU::CBOP99;	CBopcodes[0x9A] = &GBCPU::CBOP9A; CBopcodes[0x9B] = &GBCPU::CBOP9B;
    CBopcodes[0x9C] = &GBCPU::CBOP9C; CBopcodes[0x9D] = &GBCPU::CBOP9D;	CBopcodes[0x9E] = &GBCPU::CBOP9E; CBopcodes[0x9F] = &GBCPU::CBOP9F;

    CBopcodes[0xA0] = &GBCPU::CBOPA0; CBopcodes[0xA1] = &GBCPU::CBOPA1; CBopcodes[0xA2] = &GBCPU::CBOPA2; CBopcodes[0xA3] = &GBCPU::CBOPA3;
    CBopcodes[0xA4] = &GBCPU::CBOPA4; CBopcodes[0xA5] = &GBCPU::CBOPA5;	CBopcodes[0xA6] = &GBCPU::CBOPA6; CBopcodes[0xA7] = &GBCPU::CBOPA7;
    CBopcodes[0xA8] = &GBCPU::CBOPA8; CBopcodes[0xA9] = &GBCPU::CBOPA9;	CBopcodes[0xAA] = &GBCPU::CBOPAA; CBopcodes[0xAB] = &GBCPU::CBOPAB;
    CBopcodes[0xAC] = &GBCPU::CBOPAC; CBopcodes[0xAD] = &GBCPU::CBOPAD;	CBopcodes[0xAE] = &GBCPU::CBOPAE; CBopcodes[0xAF] = &GBCPU::CBOPAF;

    CBopcodes[0xB0] = &GBCPU::CBOPB0; CBopcodes[0xB1] = &GBCPU::CBOPB1; CBopcodes[0xB2] = &GBCPU::CBOPB2; CBopcodes[0xB3] = &GBCPU::CBOPB3;
    CBopcodes[0xB4] = &GBCPU::CBOPB4; CBopcodes[0xB5] = &GBCPU::CBOPB5;	CBopcodes[0xB6] = &GBCPU::CBOPB6; CBopcodes[0xB7] = &GBCPU::CBOPB7;
    CBopcodes[0xB8] = &GBCPU::CBOPB8; CBopcodes[0xB9] = &GBCPU::CBOPB9;	CBopcodes[0xBA] = &GBCPU::CBOPBA; CBopcodes[0xBB] = &GBCPU::CBOPBB;
    CBopcodes[0xBC] = &GBCPU::CBOPBC; CBopcodes[0xBD] = &GBCPU::CBOPBD;	CBopcodes[0xBE] = &GBCPU::CBOPBE; CBopcodes[0xBF] = &GBCPU::CBOPBF;

    CBopcodes[0xC0] = &GBCPU::CBOPC0; CBopcodes[0xC1] = &GBCPU::CBOPC1; CBopcodes[0xC2] = &GBCPU::CBOPC2; CBopcodes[0xC3] = &GBCPU::CBOPC3;
    CBopcodes[0xC4] = &GBCPU::CBOPC4; CBopcodes[0xC5] = &GBCPU::CBOPC5;	CBopcodes[0xC6] = &GBCPU::CBOPC6; CBopcodes[0xC7] = &GBCPU::CBOPC7;
    CBopcodes[0xC8] = &GBCPU::CBOPC8; CBopcodes[0xC9] = &GBCPU::CBOPC9;	CBopcodes[0xCA] = &GBCPU::CBOPCA; CBopcodes[0xCB] = &GBCPU::CBOPCB;
    CBopcodes[0xCC] = &GBCPU::CBOPCC; CBopcodes[0xCD] = &GBCPU::CBOPCD;	CBopcodes[0xCE] = &GBCPU::CBOPCE; CBopcodes[0xCF] = &GBCPU::CBOPCF;

    CBopcodes[0xD0] = &GBCPU::CBOPD0; CBopcodes[0xD1] = &GBCPU::CBOPD1; CBopcodes[0xD2] = &GBCPU::CBOPD2; CBopcodes[0xD3] = &GBCPU::CBOPD3;
    CBopcodes[0xD4] = &GBCPU::CBOPD4; CBopcodes[0xD5] = &GBCPU::CBOPD5;	CBopcodes[0xD6] = &GBCPU::CBOPD6; CBopcodes[0xD7] = &GBCPU::CBOPD7;
    CBopcodes[0xD8] = &GBCPU::CBOPD8; CBopcodes[0xD9] = &GBCPU::CBOPD9;	CBopcodes[0xDA] = &GBCPU::CBOPDA; CBopcodes[0xDB] = &GBCPU::CBOPDB;
    CBopcodes[0xDC] = &GBCPU::CBOPDC; CBopcodes[0xDD] = &GBCPU::CBOPDD;	CBopcodes[0xDE] = &GBCPU::CBOPDE; CBopcodes[0xDF] = &GBCPU::CBOPDF;

    CBopcodes[0xE0] = &GBCPU::CBOPE0; CBopcodes[0xE1] = &GBCPU::CBOPE1; CBopcodes[0xE2] = &GBCPU::CBOPE2; CBopcodes[0xE3] = &GBCPU::CBOPE3;
    CBopcodes[0xE4] = &GBCPU::CBOPE4; CBopcodes[0xE5] = &GBCPU::CBOPE5;	CBopcodes[0xE6] = &GBCPU::CBOPE6; CBopcodes[0xE7] = &GBCPU::CBOPE7;
    CBopcodes[0xE8] = &GBCPU::CBOPE8; CBopcodes[0xE9] = &GBCPU::CBOPE9;	CBopcodes[0xEA] = &GBCPU::CBOPEA; CBopcodes[0xEB] = &GBCPU::CBOPEB;
    CBopcodes[0xEC] = &GBCPU::CBOPEC; CBopcodes[0xED] = &GBCPU::CBOPED;	CBopcodes[0xEE] = &GBCPU::CBOPEE; CBopcodes[0xEF] = &GBCPU::CBOPEF;

    CBopcodes[0xF0] = &GBCPU::CBOPF0; CBopcodes[0xF1] = &GBCPU::CBOPF1; CBopcodes[0xF2] = &GBCPU::CBOPF2; CBopcodes[0xF3] = &GBCPU::CBOPF3;
    CBopcodes[0xF4] = &GBCPU::CBOPF4; CBopcodes[0xF5] = &GBCPU::CBOPF5;	CBopcodes[0xF6] = &GBCPU::CBOPF6; CBopcodes[0xF7] = &GBCPU::CBOPF7;
    CBopcodes[0xF8] = &GBCPU::CBOPF8; CBopcodes[0xF9] = &GBCPU::CBOPF9;	CBopcodes[0xFA] = &GBCPU::CBOPFA; CBopcodes[0xFB] = &GBCPU::CBOPFB;
    CBopcodes[0xFC] = &GBCPU::CBOPFC; CBopcodes[0xFD] = &GBCPU::CBOPFD;	CBopcodes[0xFE] = &GBCPU::CBOPFE; CBopcodes[0xFF] = &GBCPU::CBOPFF;
}

GBCPU::~GBCPU()
{
	// Free up all dynamically allocated data
}

void GBCPU::execute()
{
    printf("PC: $%X OPCODE: %X AF: 0x%X%X BC: 0x%X%X DE: 0x%X%X HL: 0x%X%X SP: 0x%X \n", PC, MEM[PC], A, GetF(), B, C, D, E, H, L, SP);
	(this->*(opcodes)[MEM[PC]])();
}

// printMEM - Debug function that prints entire contents of CPU memory into a file.
void GBCPU::printMEM(string name)
{
    ofstream file(name, std::ios_base::binary);
    file.write((char*)MEM, MAX_GB_MEMORY);
    file.close();
}

void GBCPU::init()
{
    cout << "GBCPU initializng...";

    // Initialize internal variables
    IME = true; // TODO: confirm initial value
    DIV_counter = 0;
    TMA_counter = 0;

    // Initialize the registers with default values for program execution
    PC = 0x100;

    // For GB, set to this value. For others, will be different
    A = (byte) 0x11; //0x01;
    SetF(0x80);      //0xB0;
    B = (byte) 0x00;
    C = (byte) 0x00; //0x13;
    D = (byte) 0xFF; //0x00; 
    E = (byte) 0x56; //0xD8;
    H = (byte) 0x00; //0x01; 
    L = (byte) 0x0D; //0x4D;
    SP = (word) 0xFFFE;

    MEM[0xFF05] = (byte) 0x00; // TIMA
    MEM[0xFF06] = (byte) 0x00; // TMA
    MEM[0xFF07] = (byte) 0x00; // TAC
    MEM[0xFF10] = (byte) 0x80; // NR10
    MEM[0xFF11] = (byte) 0xBF; // NR11
    MEM[0xFF12] = (byte) 0xF3; // NR12
    MEM[0xFF14] = (byte) 0xBF; // NR14
    MEM[0xFF16] = (byte) 0x3F; // NR21
    MEM[0xFF17] = (byte) 0x00; // NR22
    MEM[0xFF19] = (byte) 0xBF; // NR24
    MEM[0xFF1A] = (byte) 0x7F; // NR30
    MEM[0xFF1B] = (byte) 0xFF; // NR31
    MEM[0xFF1C] = (byte) 0x9F; // NR32
    MEM[0xFF1E] = (byte) 0xBF; // NR33
    MEM[0xFF20] = (byte) 0xFF; // NR41
    MEM[0xFF21] = (byte) 0x00; // NR42
    MEM[0xFF22] = (byte) 0x00; // NR43
    MEM[0xFF23] = (byte) 0xBF; // NR30
    MEM[0xFF24] = (byte) 0x77; // NR50
    MEM[0xFF25] = (byte) 0xF3; // NR51
    MEM[0xFF26] = (byte) 0xF1; // For GB 0xF1, for SGB 0xF0
    MEM[0xFF40] = (byte) 0x91; // LCDC
    MEM[0xFF42] = (byte) 0x00; // SCY
    MEM[0xFF43] = (byte) 0x00; // SCX
    MEM[0xFF45] = (byte) 0x00; // LYC
    MEM[0xFF47] = (byte) 0xFC; // BGP
    MEM[0xFF48] = (byte) 0xFF; // OBP0
    MEM[0xFF49] = (byte) 0xFF; // OBP1
    MEM[0xFF4A] = (byte) 0x00; // WY
    MEM[0xFF4B] = (byte) 0x00; // WX
    MEM[0xFFFF] = (byte) 0x00; // IE

    cout << "done!" << endl;
}

