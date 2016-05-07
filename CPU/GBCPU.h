#ifndef _GBCPU_H_
#define _GBCPU_H_

#include "gameboy.h"
#include <iostream>

using namespace std;

/*
	Class:		 GBCPU
	Description: Software implementation of the ZILOG Z80 & INTEL 8088 Hybrid CPU, named
				 the Sharp LR35902. */
class GBCPU
{
public:
	/*	CPU Address Space Definitions
		Courtesy: https://realboyemulator.wordpress.com/

		0x0000 - 0x3FFF Permanently-mapped ROM bank
		0x4000 - 0x7FFF Area for switchable ROM banks

		0x8000 - 0x9FFF Video RAM
		0xA000 - 0xBFFF Switchable External RAM bank
		0xC000 - 0xCFFF Game Boy's working RAM bank 0
		0xD000 - 0xDFFF Game Boy's working RAM bank 1
		0xE000 - 0xFDFF Echo of Game Boy's working RAM bank 0 and 1

		0xFE00 - 0xFEFF Sprite Attribute Table

		0xFF00 - 0xFF7F Device's Mappings. I/O Device Access

		0xFF90 - 0xFFFE High RAM Area

		0xFFFF			Interrupt Enable Register	*/
	byte MEM[MAX_GB_MEMORY]; 	// Memory
	word ADDR;					// CPU Address Bus
	byte DATA;					// CPU Data Bus
	word SP;					// Stack Pointer
	word PC;					// Program Counter
	byte A;						// Accumulator Register

	/*	Status Register Bit Flags
		b8|0	0	0	0	0	0	0	0|b0
		-  Z	S	H	C	-	-	-            	*/
	byte F;						// Status Register
	byte B, C, 					// General Purpose Registers
		 D, E,					// These may be paired to form 16-bit registers
		 H, L;					// (same applies for A & F)

	word cycles;				// The number of cycles currently counted

	void (GBCPU::*opcodes[0xFF])(); 	// Array of pointers to Opcode member functions
	void (GBCPU::*CBopcodes[0xFF])();   // Array of pointers to CB-prefix member Opcode functions

	// Function Declarations
	GBCPU();					// Constructor
	~GBCPU();					// Deconstructor
	void init();				// Game Boy initialization
	void execute();				// CPU Instruction Execution


	/*
	CPU Opcode Execution Instruction Macros
	*/

	void OP00(){ cout << "Hello world!" << endl;  }
	void OP01(){ LD(B, MEM[PC++]); LD(C, MEM[PC++]); cycles += 12; }
	void OP02(){ LD(MEM[CASTWD(B, C)], A); cycles += 8; }
	void OP03(){ }
	void OP04(){ }
	void OP05(){ }
	void OP06(){ LD(B, MEM[PC++]); cycles += 8; }
	void OP07(){ }
	void OP08(){ LD(MEM[CASTWD(MEM[PC++], MEM[PC++]) ], SP & 0xFF); cycles += 20; }
	void OP09(){ }
	void OP0A(){ LD(A, MEM[CASTWD(B, C)]); cycles += 8; }
	void OP0B(){ }
	void OP0C(){ }
	void OP0D(){ }
	void OP0E(){ LD(C, MEM[PC++]); cycles += 8; }
	void OP0F(){ }

	void OP10(){ }
	void OP11(){ LD(D, MEM[PC++]); LD(E, MEM[PC++]); cycles += 12; }
	void OP12(){ LD(MEM[CASTWD(D, E)], A); cycles += 8; }
	void OP13(){ }
	void OP14(){ }
	void OP15(){ }
	void OP16(){ LD(D, MEM[PC++]); cycles += 8; }
	void OP17(){ }
	void OP18(){ }
	void OP19(){ }
	void OP1A(){ LD(A, MEM[CASTWD(D, L)]); cycles += 8; }
	void OP1B(){ }
	void OP1C(){ }
	void OP1D(){ }
	void OP1E(){ LD(E, MEM[PC++]); cycles += 8; }
	void OP1F(){ }

	void OP20(){ }
	void OP21(){ LD(H, MEM[PC++]); LD(L, MEM[PC++]); cycles += 12; }
	void OP22(){ LD(MEM[CASTWD(H, L)], A); INC(H, L); cycles += 8; }
	void OP23(){ }
	void OP24(){ }
	void OP25(){ }
	void OP26(){ LD(H, MEM[PC++]); cycles += 8; }
	void OP27(){ }
	void OP28(){ }
	void OP29(){ }
	void OP2A(){ LD(A, MEM[CASTWD(H, L)]); INC(H, L); cycles += 8; }
	void OP2B(){ }
	void OP2C(){ }
	void OP2D(){ }
	void OP2E(){ LD(L, MEM[PC++]); cycles += 8; }
	void OP2F(){ }

	void OP30(){ }
	void OP31(){ LD(SP, CASTWD(MEM[PC++], MEM[PC++])); cycles += 12; }
	void OP32(){ LD(MEM[CASTWD(H, L)], A); DEC(H, L); cycles += 8; }
	void OP33(){ }
	void OP34(){ }
	void OP35(){ }
	void OP36(){ LD(MEM[CASTWD(H, L)], MEM[PC++]); cycles += 12; }
	void OP37(){ }
	void OP38(){ }
	void OP39(){ }
	void OP3A(){ LD(A, MEM[CASTWD(H, L)]); DEC(H, L); cycles += 8; }
	void OP3B(){ }
	void OP3C(){ }
	void OP3D(){ }
	void OP3E(){ LD(A, MEM[PC++]); cycles += 8; }
	void OP3F(){ }

	void OP40(){ LD(B, B); cycles += 4; }
	void OP41(){ LD(B, C); cycles += 4; }
	void OP42(){ LD(B, D); cycles += 4; }
	void OP43(){ LD(B, E); cycles += 4; }
	void OP44(){ LD(B, H); cycles += 4; }
	void OP45(){ LD(B, L); cycles += 4; }
	void OP46(){ LD(B, MEM[CASTWD(H, L)]); cycles += 8; }
	void OP47(){ LD(B, A); cycles += 4; }
	void OP48(){ LD(C, B); cycles += 4; }
	void OP49(){ LD(C, C); cycles += 4; }
	void OP4A(){ LD(C, D); cycles += 4; }
	void OP4B(){ LD(C, E); cycles += 4; }
	void OP4C(){ LD(C, H); cycles += 4; }
	void OP4D(){ LD(C, L); cycles += 4; }
	void OP4E(){ LD(C, MEM[CASTWD(H, L)]); cycles += 8; }
	void OP4F(){ LD(C, A); cycles += 4; }

	void OP50(){ LD(D, B); cycles += 4; }
	void OP51(){ LD(D, C); cycles += 4; }
	void OP52(){ LD(D, D); cycles += 4; }
	void OP53(){ LD(D, E); cycles += 4; }
	void OP54(){ LD(D, H); cycles += 4; }
	void OP55(){ LD(D, L); cycles += 4; }
	void OP56(){ LD(D, MEM[CASTWD(H, L)]); cycles += 8; }
	void OP57(){ LD(D, A); cycles += 4; }
	void OP58(){ LD(E, B); cycles += 4; }
	void OP59(){ LD(E, C); cycles += 4; }
	void OP5A(){ LD(E, D); cycles += 4; }
	void OP5B(){ LD(E, E); cycles += 4; }
	void OP5C(){ LD(E, H); cycles += 4; }
	void OP5D(){ LD(E, L); cycles += 4; }
	void OP5E(){ LD(E, MEM[CASTWD(H, L)]); cycles += 8; }
	void OP5F(){ LD(E, A); cycles += 4; }
	
	void OP60(){ LD(H, B); cycles += 4; }
	void OP61(){ LD(H, C); cycles += 4; }
	void OP62(){ LD(H, D); cycles += 4; }
	void OP63(){ LD(H, E); cycles += 4; }
	void OP64(){ LD(H, H); cycles += 4; }
	void OP65(){ LD(H, L); cycles += 4; }
	void OP66(){ LD(H, MEM[CASTWD(H, L)]); cycles += 8; }
	void OP67(){ LD(H, A); cycles += 4; }
	void OP68(){ LD(L, B); cycles += 4; }
	void OP69(){ LD(L, C); cycles += 4; }
	void OP6A(){ LD(L, D); cycles += 4; }
	void OP6B(){ LD(L, E); cycles += 4; }
	void OP6C(){ LD(L, H); cycles += 4; }
	void OP6D(){ LD(L, L); cycles += 4; }
	void OP6E(){ LD(L, MEM[CASTWD(H, L)]); cycles += 8; }
	void OP6F(){ LD(L, A); cycles += 4; }

	void OP70(){ LD(MEM[CASTWD(H, L)], B); cycles += 8; }
	void OP71(){ LD(MEM[CASTWD(H, L)], C); cycles += 8; }
	void OP72(){ LD(MEM[CASTWD(H, L)], D); cycles += 8; }
	void OP73(){ LD(MEM[CASTWD(H, L)], E); cycles += 8; }
	void OP74(){ LD(MEM[CASTWD(H, L)], H); cycles += 8; }
	void OP75(){ LD(MEM[CASTWD(H, L)], L); cycles += 8; }
	void OP76(){ }
	void OP77(){ LD(MEM[CASTWD(H, L)], A); cycles += 8; }
	void OP78(){ LD(A, B); cycles += 4; }
	void OP79(){ LD(A, C); cycles += 4; }
	void OP7A(){ LD(A, D); cycles += 4; }
	void OP7B(){ LD(A, E); cycles += 4; }
	void OP7C(){ LD(A, H); cycles += 4; }
	void OP7D(){ LD(A, L); cycles += 4; }
	void OP7E(){ LD(A, MEM[CASTWD(H, L)]); cycles += 8; }
	void OP7F(){ LD(A, A); cycles += 4; }

	void OP80(){ }
	void OP81(){ }
	void OP82(){ }
	void OP83(){ }
	void OP84(){ }
	void OP85(){ }
	void OP86(){ }
	void OP87(){ }
	void OP88(){ }
	void OP89(){ }
	void OP8A(){ }
	void OP8B(){ }
	void OP8C(){ }
	void OP8D(){ }
	void OP8E(){ }
	void OP8F(){ }

	void OP90(){ }
	void OP91(){ }
	void OP92(){ }
	void OP93(){ }
	void OP94(){ }
	void OP95(){ }
	void OP96(){ }
	void OP97(){ }
	void OP98(){ }
	void OP99(){ }
	void OP9A(){ }
	void OP9B(){ }
	void OP9C(){ }
	void OP9D(){ }
	void OP9E(){ }
	void OP9F(){ }

	void OPA0(){ }
	void OPA1(){ }
	void OPA2(){ }
	void OPA3(){ }
	void OPA4(){ }
	void OPA5(){ }
	void OPA6(){ }
	void OPA7(){ }
	void OPA8(){ }
	void OPA9(){ }
	void OPAA(){ }
	void OPAB(){ }
	void OPAC(){ }
	void OPAD(){ }
	void OPAE(){ }
	void OPAF(){ }

	void OPB0(){ }
	void OPB1(){ }
	void OPB2(){ }
	void OPB3(){ }
	void OPB4(){ }
	void OPB5(){ }
	void OPB6(){ }
	void OPB7(){ }
	void OPB8(){ }
	void OPB9(){ }
	void OPBA(){ }
	void OPBB(){ }
	void OPBC(){ }
	void OPBD(){ }
	void OPBE(){ }
	void OPBF(){ }

	void OPC0(){ }
	void OPC1(){ POP(B, C); cycles += 12; }
	void OPC2(){ }
	void OPC3(){ }
	void OPC4(){ }
	void OPC5(){ PUSH(B, C); cycles += 16; }
	void OPC6(){ }
	void OPC7(){ }
	void OPC8(){ }
	void OPC9(){ }
	void OPCA(){ }
	void OPCB(){ /* PREFIX CB OPCODES - DO NOT USE. */}
	void OPCC(){ }
	void OPCD(){ }
	void OPCE(){ }
	void OPCF(){ }

	void OPD0(){ }
	void OPD1(){ POP(D, E); cycles += 12; }
	void OPD2(){ }
	void OPD3(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPD4(){ }
	void OPD5(){ PUSH(D, E); cycles += 16; }
	void OPD6(){ }
	void OPD7(){ }
	void OPD8(){ }
	void OPD9(){ }
	void OPDA(){ }
	void OPDB(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPDC(){ }
	void OPDD(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPDE(){ }
	void OPDF(){ }

	void OPE0(){ LD(MEM[0xFF00 + MEM[PC++]], A); cycles += 12; }
	void OPE1(){ POP(H, L); cycles += 12; }
	void OPE2(){ LD(MEM[0xFF00 + C], A); cycles += 8; }
	void OPE3(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPE4(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPE5(){ PUSH(H, L); cycles += 16; }
	void OPE6(){ }
	void OPE7(){ }
	void OPE8(){ }
	void OPE9(){ }
	void OPEA(){ LD(MEM[CASTWD(MEM[PC++], MEM[PC++])], A); cycles += 16; }
	void OPEB(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPEC(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPED(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPEE(){ }
	void OPEF(){ }

	void OPF0(){ LD(A, MEM[0xFF00 + MEM[PC++]]); cycles += 12; }
	void OPF1(){ POP(A, F); cycles += 12; }
	void OPF2(){ LD(A, MEM[0xFF00 + C]); cycles += 8; }
	void OPF3(){ }
	void OPF4(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPF5(){ PUSH(A, F); cycles += 16; }
	void OPF6(){ }
	void OPF7(){ }
	void OPF8(){ LD(H, (SP + MEM[PC]) >> 8); LD(L, (SP + MEM[PC++]) & 0xFF); cycles += 12; } // TODO: Status flag settings
	void OPF9(){ LD(SP, CASTWD(H, L)); cycles += 8; }
	void OPFA(){ LD(A, MEM[CASTWD(MEM[PC++], MEM[PC++])]); cycles += 16; }
	void OPFB(){ }
	void OPFC(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPFD(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPFE(){ }
	void OPFF(){ }
	

	// left off on pg 80

};



#endif