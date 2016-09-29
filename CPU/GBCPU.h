#ifndef _GBCPU_H_
#define _GBCPU_H_

#include "gameboy.h"
#include <iostream>
#include <fstream>

using namespace std;

/*
	Class:		 GBCPU
	Description: Software implementation of the ZILOG Z80 & INTEL 8088 Hybrid CPU, named
				 the Sharp LR35902. */
class GBCPU
{
public:
	byte MEM[MAX_GB_MEMORY]; 	// CPU Memory (PRG) Currently 64K max size
	word ADDR;					// CPU Address Bus  - UNUSED
	byte DATA;					// CPU Data Bus     - UNUSED
	word SP;					// Stack Pointer
	word PC;					// Program Counter
	byte A;						// Accumulator Register

    bool DI_Executed;           // Flag that tells CPU to disable interrupt on this instruction. TODO: Have CPU check this before executing instruction

    bool CARRY_FLAG,           // Status Register (F)
         HALF_CARRY_FLAG,
         SUBTRACT_FLAG,
         ZERO_FLAG;

	byte B, C, 					// General Purpose Registers
		 D, E,					// These may be paired to form 16-bit registers
		 H, L;					// (same applies for A & F)

	word cycles;				// The number of cycles currently counted

	void (GBCPU::*opcodes[256])(); 	 // Array of pointers to Opcode member functions
	void (GBCPU::*CBopcodes[256])(); // Array of pointers to CB-prefix member Opcode functions

	// Function Declarations
	GBCPU();					// Constructor
	~GBCPU();					// Deconstructor

    /***** Memory Access functions - memory.cpp/mbc.cpp *****/
    void MBC1write(word addr, byte data);
    byte MBC1read(word addr);
    void writeByte(byte data, word addr);
    void writeWord(word data, word addr);
    byte readByte(word addr);
    byte readImmByte();
    word readWord(word addr);
    word readImmWord();

    byte GetF(); // Get Status Register as a byte
    void SetF(byte F); // Set Status register from a byte

    word GBCPU::GetAF();
    void GBCPU::SetAF(word data);
    word GBCPU::GetBC();
    void GBCPU::SetBC(word data);
    word GBCPU::GetDE();
    void GBCPU::SetDE(word data);
    word GBCPU::GetHL();
    void GBCPU::SetHL(word data);

    /***** Debug Functions - GBCPU.cpp *****/
    void printMEM(string name); // Print entire contents of CPU memory into a file

    /***** Main Functions - GBCPU.cpp *****/
	void init();				// Game Boy initialization
	void execute();				// CPU Instruction Execution



    /***** Opcode Functions - opcodes.cpp *****/
    // ADD A, n
    inline void ADD(byte & reg, byte arg);

    // ADD HL, n
    inline void ADD(word arg);

    // ADD SP, n
    inline void ADDSP();

    inline void SUB(byte & reg, byte arg);
    inline void AND(byte & reg, byte arg);
    inline void OR(byte & reg, byte arg);
    inline void XOR(byte & reg, byte arg);
    inline void CP(byte & reg, byte arg);
    inline void INCR(byte & reg);
    inline void DECR(byte & reg);
    inline void SWAP(byte & reg);    
    inline void DAA();  // Decimal Adjust Register A
    inline void RLCA(); // Rotate A left

    // Rotate n left
    inline void RLC(byte & reg);

    // Rotate A left with carry
    inline void RLA();

    // Rotate n left with carry
    inline void RL(byte & reg);

    // Rotate A right
    inline void RRCA();

    // Rotate n right
    inline void RRC(byte & reg);

    // Rotate A right through carry
    inline void RRA();

    // Rotate n right through carry
    inline void RR(byte & reg);

    // SLA n
    inline void SLA(byte & reg);

    // SRA n
    inline void SRA(byte & reg);

    // SRA n
    inline void SRL(byte & reg);

    // BIT b,r
    inline void BIT(byte bit, byte & reg);

    // JP cc
    inline void JP();

    // JR cc
    inline void JR();

    // CALL cc
    inline void CALL();

    // RESET
    inline void RST(byte n);

    // RET cc
    inline void RET();
	/*
	CPU Opcode Execution Instruction Macros
	*/

    void OP00();
    void OP01(); 
    void OP02(); 
    void OP03(); 
    void OP04(); 
    void OP05(); 
    void OP06(); 
    void OP07(); 
    void OP08(); 
    void OP09(); 
    void OP0A(); 
    void OP0B(); 
    void OP0C(); 
    void OP0D(); 
    void OP0E(); 
    void OP0F(); 
    void OP10(); 
    void OP11(); 
    void OP12(); 
    void OP13(); 
    void OP14(); 
    void OP15(); 
    void OP16(); 
    void OP17(); 
    void OP18(); 
    void OP19(); 
    void OP1A(); 
    void OP1B(); 
    void OP1C(); 
    void OP1D(); 
    void OP1E(); 
    void OP1F(); 
    void OP20(); 
    void OP21(); 
    void OP22(); 
    void OP23(); 
    void OP24(); 
    void OP25(); 
    void OP26(); 
    void OP27(); 
    void OP28(); 
    void OP29(); 
    void OP2A(); 
    void OP2B(); 
    void OP2C(); 
    void OP2D(); 
    void OP2E(); 
    void OP2F(); 
    void OP30(); 
    void OP31(); 
    void OP32(); 
    void OP33(); 
    void OP34(); 
    void OP35(); 
    void OP36(); 
    void OP37(); 
    void OP38(); 
    void OP39(); 
    void OP3A(); 
    void OP3B(); 
    void OP3C(); 
    void OP3D(); 
    void OP3E(); 
    void OP3F(); 
    void OP40(); 
    void OP41(); 
    void OP42(); 
    void OP43(); 
    void OP44(); 
    void OP45(); 
    void OP46(); 
    void OP47(); 
    void OP48(); 
    void OP49(); 
    void OP4A(); 
    void OP4B(); 
    void OP4C(); 
    void OP4D(); 
    void OP4E(); 
    void OP4F(); 
    void OP50(); 
    void OP51(); 
    void OP52(); 
    void OP53(); 
    void OP54(); 
    void OP55(); 
    void OP56(); 
    void OP57(); 
    void OP58(); 
    void OP59(); 
    void OP5A(); 
    void OP5B(); 
    void OP5C(); 
    void OP5D(); 
    void OP5E(); 
    void OP5F(); 
    void OP60(); 
    void OP61(); 
    void OP62(); 
    void OP63(); 
    void OP64(); 
    void OP65(); 
    void OP66(); 
    void OP67(); 
    void OP68(); 
    void OP69(); 
    void OP6A(); 
    void OP6B(); 
    void OP6C(); 
    void OP6D(); 
    void OP6E(); 
    void OP6F(); 
    void OP70(); 
    void OP71(); 
    void OP72(); 
    void OP73(); 
    void OP74(); 
    void OP75(); 
    void OP76(); 
    void OP77(); 
    void OP78(); 
    void OP79(); 
    void OP7A(); 
    void OP7B(); 
    void OP7C(); 
    void OP7D(); 
    void OP7E(); 
    void OP7F();
    void OP80(); 
    void OP81(); 
    void OP82(); 
    void OP83(); 
    void OP84(); 
    void OP85(); 
    void OP86(); 
    void OP87(); 
    void OP88(); 
    void OP89(); 
    void OP8A(); 
    void OP8B(); 
    void OP8C(); 
    void OP8D(); 
    void OP8E(); 
    void OP8F(); 
    void OP90(); 
    void OP91(); 
    void OP92(); 
    void OP93(); 
    void OP94(); 
    void OP95(); 
    void OP96(); 
    void OP97(); 
    void OP98(); 
    void OP99(); 
    void OP9A(); 
    void OP9B(); 
    void OP9C(); 
    void OP9D(); 
    void OP9E(); 
    void OP9F(); 
    void OPA0(); 
    void OPA1(); 
    void OPA2(); 
    void OPA3(); 
    void OPA4(); 
    void OPA5(); 
    void OPA6(); 
    void OPA7(); 
    void OPA8(); 
    void OPA9(); 
    void OPAA(); 
    void OPAB(); 
    void OPAC(); 
    void OPAD(); 
    void OPAE(); 
    void OPAF(); 
    void OPB0(); 
    void OPB1(); 
    void OPB2(); 
    void OPB3(); 
    void OPB4(); 
    void OPB5(); 
    void OPB6(); 
    void OPB7(); 
    void OPB8(); 
    void OPB9(); 
    void OPBA(); 
    void OPBB(); 
    void OPBC(); 
    void OPBD(); 
    void OPBE(); 
    void OPBF(); 
    void OPC0(); 
    void OPC1(); 
    void OPC2(); 
    void OPC3(); 
    void OPC4(); 
    void OPC5(); 
    void OPC6(); 
    void OPC7(); 
    void OPC8(); 
    void OPC9(); 
    void OPCA(); 
    void OPCB(); 
    void OPCC(); 
    void OPCD(); 
    void OPCE(); 
    void OPCF(); 
    void OPD0(); 
    void OPD1(); 
    void OPD2(); 
    void OPD3(); 
    void OPD4(); 
    void OPD5(); 
    void OPD6(); 
    void OPD7(); 
    void OPD8(); 
    void OPD9(); 
    void OPDA(); 
    void OPDB(); 
    void OPDC(); 
    void OPDD(); 
    void OPDE(); 
    void OPDF(); 
    void OPE0(); 
    void OPE1(); 
    void OPE2(); 
    void OPE3(); 
    void OPE4(); 
    void OPE5(); 
    void OPE6(); 
    void OPE7(); 
    void OPE8(); 
    void OPE9(); 
    void OPEA(); 
    void OPEB(); 
    void OPEC(); 
    void OPED(); 
    void OPEE(); 
    void OPEF();
    void OPF0(); 
    void OPF1(); 
    void OPF2(); 
    void OPF3(); 
    void OPF4(); 
    void OPF5(); 
    void OPF6(); 
    void OPF7(); 
    void OPF8(); 
    void OPF9(); 
    void OPFA(); 
    void OPFB(); 
    void OPFC(); 
    void OPFD(); 
    void OPFE(); 
    void OPFF(); 
    
    
    /* CB Operations */
    
    void CBOP00();
    void CBOP01();
    void CBOP02();
    void CBOP03();
    void CBOP04();
    void CBOP05();
    void CBOP06();
    void CBOP07();
    void CBOP08();
    void CBOP09();
    void CBOP0A();
    void CBOP0B();
    void CBOP0C();
    void CBOP0D();
    void CBOP0E();
    void CBOP0F();
    void CBOP10();
    void CBOP11();
    void CBOP12();
    void CBOP13();
    void CBOP14();
    void CBOP15();
    void CBOP16();
    void CBOP17();
    void CBOP18();
    void CBOP19();
    void CBOP1A();
    void CBOP1B();
    void CBOP1C();
    void CBOP1D();
    void CBOP1E();
    void CBOP1F();
    void CBOP20();
    void CBOP21();
    void CBOP22();
    void CBOP23();
    void CBOP24();
    void CBOP25();
    void CBOP26();
    void CBOP27();
    void CBOP28();
    void CBOP29();
    void CBOP2A();
    void CBOP2B();
    void CBOP2C();
    void CBOP2D();
    void CBOP2E();
    void CBOP2F();
    void CBOP30();
    void CBOP31();
    void CBOP32();
    void CBOP33();
    void CBOP34();
    void CBOP35();
    void CBOP36();
    void CBOP37();
    void CBOP38();
    void CBOP39();
    void CBOP3A();
    void CBOP3B();
    void CBOP3C();
    void CBOP3D();
    void CBOP3E();
    void CBOP3F();
    void CBOP40();
    void CBOP41();
    void CBOP42();
    void CBOP43();
    void CBOP44();
    void CBOP45();
    void CBOP46();
    void CBOP47();
    void CBOP48();
    void CBOP49();
    void CBOP4A();
    void CBOP4B();
    void CBOP4C();
    void CBOP4D();
    void CBOP4E();
    void CBOP4F();
    void CBOP50();
    void CBOP51();
    void CBOP52();
    void CBOP53();
    void CBOP54();
    void CBOP55();
    void CBOP56();
    void CBOP57();
    void CBOP58();
    void CBOP59();
    void CBOP5A();
    void CBOP5B();
    void CBOP5C();
    void CBOP5D();
    void CBOP5E();
    void CBOP5F();
    void CBOP60();
    void CBOP61();
    void CBOP62();
    void CBOP63();
    void CBOP64();
    void CBOP65();
    void CBOP66();
    void CBOP67();
    void CBOP68();
    void CBOP69();
    void CBOP6A();
    void CBOP6B();
    void CBOP6C();
    void CBOP6D();
    void CBOP6E();
    void CBOP6F();
    void CBOP70();
    void CBOP71();
    void CBOP72();
    void CBOP73();
    void CBOP74();
    void CBOP75();
    void CBOP76();
    void CBOP77();
    void CBOP78();
    void CBOP79();
    void CBOP7A();
    void CBOP7B();
    void CBOP7C();
    void CBOP7D();
    void CBOP7E();
    void CBOP7F();
    // RES
    void CBOP80();
    void CBOP81();
    void CBOP82();
    void CBOP83();
    void CBOP84();
    void CBOP85();
    void CBOP86();
    void CBOP87();
    void CBOP88();
    void CBOP89();
    void CBOP8A();
    void CBOP8B();
    void CBOP8C();
    void CBOP8D();
    void CBOP8E();
    void CBOP8F();
    void CBOP90();
    void CBOP91();
    void CBOP92();
    void CBOP93();
    void CBOP94();
    void CBOP95();
    void CBOP96();
    void CBOP97();
    void CBOP98();
    void CBOP99();
    void CBOP9A();
    void CBOP9B();
    void CBOP9C();
    void CBOP9D();
    void CBOP9E();
    void CBOP9F();
    void CBOPA0();
    void CBOPA1();
    void CBOPA2();
    void CBOPA3();
    void CBOPA4();
    void CBOPA5();
    void CBOPA6();
    void CBOPA7();
    void CBOPA8();
    void CBOPA9();
    void CBOPAA();
    void CBOPAB();
    void CBOPAC();
    void CBOPAD();
    void CBOPAE();
    void CBOPAF();
    void CBOPB0();
    void CBOPB1();
    void CBOPB2();
    void CBOPB3();
    void CBOPB4();
    void CBOPB5();
    void CBOPB6();
    void CBOPB7();
    void CBOPB8();
    void CBOPB9();
    void CBOPBA();
    void CBOPBB();
    void CBOPBC();
    void CBOPBD();
    void CBOPBE();
    void CBOPBF();
    // SET
    void CBOPC0();
    void CBOPC1();
    void CBOPC2();
    void CBOPC3();
    void CBOPC4();
    void CBOPC5();
    void CBOPC6();
    void CBOPC7();
    void CBOPC8();
    void CBOPC9();
    void CBOPCA();
    void CBOPCB();
    void CBOPCC();
    void CBOPCD();
    void CBOPCE();
    void CBOPCF();
    void CBOPD0();
    void CBOPD1();
    void CBOPD2();
    void CBOPD3();
    void CBOPD4();
    void CBOPD5();
    void CBOPD6();
    void CBOPD7();
    void CBOPD8();
    void CBOPD9();
    void CBOPDA();
    void CBOPDB();
    void CBOPDC();
    void CBOPDD();
    void CBOPDE();
    void CBOPDF();
    void CBOPE0();
    void CBOPE1();
    void CBOPE2();
    void CBOPE3();
    void CBOPE4();
    void CBOPE5();
    void CBOPE6();
    void CBOPE7();
    void CBOPE8();
    void CBOPE9();
    void CBOPEA();
    void CBOPEB();
    void CBOPEC();
    void CBOPED();
    void CBOPEE();
    void CBOPEF();
    void CBOPF0();
    void CBOPF1();
    void CBOPF2();
    void CBOPF3();
    void CBOPF4();
    void CBOPF5();
    void CBOPF6();
    void CBOPF7();
    void CBOPF8();
    void CBOPF9();
    void CBOPFA();
    void CBOPFB();
    void CBOPFC();
    void CBOPFD();
    void CBOPFE();
    void CBOPFF();
};



#endif