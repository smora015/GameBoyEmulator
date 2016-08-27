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

    bool DI_Executed;           // Flag that tells CPU to disable interrupt on this instruction. TODO: Have CPU check this before executing instruction

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

    // ADD A, n
    inline void ADD(byte & reg, byte arg)
    {
        // TODO: remove reg parameter, as this will always be A...
        // Reset N falg
        CLRBIT(F, SUBTRACT_FLAG_BIT);

        // Detect half carry
        if (((reg & 0x0F) + (arg & 0x0F)) & 0x10)
            SETBIT(F, HALF_CARRY_FLAG_BIT);
        else
            CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // Detect carry
        if (((reg & 0x0FF) + (arg & 0x0FF)) & 0x100)
            SETBIT(F, CARRY_FLAG_BIT);
        else
            CLRBIT(F, CARRY_FLAG_BIT);

        reg += arg;

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // ADD HL, n
    inline void ADD(word arg)
    {
        word HL = CASTWD(H, L);

        // Reset N flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);

        // Detect half carry
        if (((HL & 0x0FF) + (arg & 0x0FF)) & 0x1000)
            SETBIT(F, HALF_CARRY_FLAG_BIT);
        else
            CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // Detects carry
        if (((HL & 0xFFFF) + (arg & 0xFFFF)) & 0x10000)
            SETBIT(F, CARRY_FLAG_BIT);
        else
            CLRBIT(F, CARRY_FLAG_BIT);

        HL += arg;

        H = (HL & 0xFF00) >> 8;
        L = (HL & 0x00FF);
    }

    // ADD SP, n
    inline void ADDSP()
    {
        // TODO: Confirm H and C flag get set based off 11th and 15th bit
        byte arg = MEM[PC++];

        // Reset Z, N flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, ZERO_FLAG_BIT);

        // Detect half carry
        if (((SP & 0x0FF) + (arg & 0x0FF)) & 0x1000)
            SETBIT(F, HALF_CARRY_FLAG_BIT);
        else
            CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // Detects carry
        if (((SP & 0xFFFF) + (arg & 0xFFFF)) & 0x10000)
            SETBIT(F, CARRY_FLAG_BIT);
        else
            CLRBIT(F, CARRY_FLAG_BIT);

        SP += arg;
    }

    inline void SUB(byte & reg, byte arg)
    {
        // Set N falg
        SETBIT(F, SUBTRACT_FLAG_BIT);

        // Detect half carry
        if ( ((reg - arg) & 0x0F) >= (reg & 0xF) )
            SETBIT(F, HALF_CARRY_FLAG_BIT);
        else
            CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // Detect carry
        if ( ((reg - arg) & 0xFF) > (reg & 0xFF) )
            SETBIT(F, CARRY_FLAG_BIT);
        else
            CLRBIT(F, CARRY_FLAG_BIT);

        reg -= arg;

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    inline void AND(byte & reg, byte arg)
    {
        // Reset N, C flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, CARRY_FLAG_BIT);

        // Set Half carry flag
        SETBIT(F, HALF_CARRY_FLAG_BIT);

        reg &= arg;

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    inline void OR(byte & reg, byte arg)
    {
        // Reset N, H, and C flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, CARRY_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);

        reg |= arg;

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    inline void XOR(byte & reg, byte arg)
    {
        // Reset N, H, and C flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, CARRY_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);

        reg ^= arg;

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    inline void CP(byte & reg, byte arg)
    {
        // Set N falg
        SETBIT(F, SUBTRACT_FLAG_BIT);

        // Detect half carry
        if (((reg - arg) & 0x0F) >= (reg & 0xF))
            SETBIT(F, HALF_CARRY_FLAG_BIT);
        else
            CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // Detect carry
        if (((reg - arg) & 0xFF) > (reg & 0xFF))
            SETBIT(F, CARRY_FLAG_BIT);
        else
            CLRBIT(F, CARRY_FLAG_BIT);

        // Detect zero
        (((reg - arg) & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    inline void INCR(byte & reg)
    {
        // Reset N falg
        CLRBIT(F, SUBTRACT_FLAG_BIT);

        // Detect half carry
        if (((reg & 0x0F) + (1 & 0x0F)) & 0x10)
            SETBIT(F, HALF_CARRY_FLAG_BIT);
        else
            CLRBIT(F, HALF_CARRY_FLAG_BIT);
        
        ++reg;

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    inline void DECR(byte & reg)
    {
        // Set N falg
        SETBIT(F, SUBTRACT_FLAG_BIT);

        // Detect half carry
        if (((reg - 1) & 0x0F) >= (reg & 0xF))
            SETBIT(F, HALF_CARRY_FLAG_BIT);
        else
            CLRBIT(F, HALF_CARRY_FLAG_BIT);

        --reg;

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    inline void SWAP(byte & reg)
    {
        // Clear N, H, C
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);
        CLRBIT(F, CARRY_FLAG_BIT);

        reg = (reg << 4) | ((reg >> 4) & 0x0F) & 0xFF;

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // Decimal Adjust Register A
    inline void DAA()
    {
        // Reset flag H
        CLRBIT(F, HALF_CARRY_FLAG_BIT);

        byte temp = A;

        // Adjust the binary decimal digits if we have a N and C or H flag
        if ( (F & SUBTRACT_FLAG) >> SUBTRACT_FLAG_BIT ) 
        {
            if ((F & HALF_CARRY_FLAG) >> HALF_CARRY_FLAG_BIT )
                temp = (temp - 6) & 0xFF;

            if ((F & CARRY_FLAG) >> CARRY_FLAG_BIT )
                temp -= 0x60;
        }
        else 
        {
            if ((temp & 0xF)>9 || (((F & HALF_CARRY_FLAG) >> HALF_CARRY_FLAG_BIT) == 1) )
                temp += 6;

            if ((temp & 0xFFF)>0x9F || (((F & CARRY_FLAG) >> CARRY_FLAG_BIT) == 1) ) 
                temp += 0x60;
        }

        A = temp;

        if ((temp & 0x100) == 0x100)
            SETBIT(F, HALF_CARRY_FLAG_BIT);
        else
            CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // Detect zero
        ((A & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));

    }

    // Rotate A left
    inline void RLCA()
    {
        // Reset N and H flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // If MSB is high, set carry flag
        if ((A & 0x80) >> 7)
        {
            SETBIT(F, CARRY_FLAG_BIT);
            A = ((A << 1) & 0xFE) | 0x01;
        }
        else
        {
            CLRBIT(F, CARRY_FLAG_BIT);
            A = ((A << 1) & 0xFE);
        }

        // Detect zero
        ((A & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // Rotate n left
    inline void RLC(byte & reg)
    {
        // Reset N and H flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // If MSB is high, set carry flag
        if ((reg & 0x80) >> 7)
        {
            SETBIT(F, CARRY_FLAG_BIT);
            reg = ((reg << 1) & 0xFE) | 0x01;
        }
        else
        {
            CLRBIT(F, CARRY_FLAG_BIT);
            reg = ((reg << 1) & 0xFE);
        }

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // Rotate A left with carry
    inline void RLA()
    {
        // Reset N and H flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);


        // If MSB is high, set carry flag and add the previous carry flag bit to LSB
        if ((A & 0x80) >> 7)
        {
            A = (A << 1) & 0xFE;
            A |= ((F & CARRY_FLAG) >> CARRY_FLAG_BIT);
            SETBIT(F, CARRY_FLAG_BIT);
        }
        else
        {
            A = (A << 1) & 0xFE;
            A |= ((F & CARRY_FLAG) >> CARRY_FLAG_BIT);
            CLRBIT(F, CARRY_FLAG_BIT);
        }

        // Detect zero
        ((A & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // Rotate n left with carry
    inline void RL(byte & reg)
    {
        // Reset N and H flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);


        // If MSB is high, set carry flag and add the previous carry flag bit to LSB
        if ((reg & 0x80) >> 7)
        {
            reg = (reg << 1) & 0xFE;
            reg |= ((F & CARRY_FLAG) >> CARRY_FLAG_BIT);
            SETBIT(F, CARRY_FLAG_BIT);
        }
        else
        {
            reg = (reg << 1) & 0xFE;
            reg |= ((F & CARRY_FLAG) >> CARRY_FLAG_BIT);
            CLRBIT(F, CARRY_FLAG_BIT);
        }

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // Rotate A right
    inline void RRCA()
    {
        // Reset N and H flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // If LSB is high, set carry flag
        if ((A & 0x01) >> 7)
        {
            SETBIT(F, CARRY_FLAG_BIT);
            A = ((A >> 1) & 0x7F) | 0x80;
        }
        else
        {
            CLRBIT(F, CARRY_FLAG_BIT);
            A = ((A << 1) & 0x7F);
        }

        // Detect zero
        ((A & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // Rotate n right
    inline void RRC(byte & reg)
    {
        // Reset N and H flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // If LSB is high, set carry flag
        if ((reg & 0x01) >> 7)
        {
            SETBIT(F, CARRY_FLAG_BIT);
            reg = ((reg >> 1) & 0x7F) | 0x80;
        }
        else
        {
            CLRBIT(F, CARRY_FLAG_BIT);
            reg = ((reg << 1) & 0x7F);
        }

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // Rotate A right through carry
    inline void RRA()
    {
        // Reset N and H flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // If LSB is high, set carry flag
        if ((A & 0x01) >> 7)
        {
            A = ((A >> 1) & 0x7F);
            A |= (((F & CARRY_FLAG) >> CARRY_FLAG_BIT) << 7);
            SETBIT(F, CARRY_FLAG_BIT);
        }
        else
        {
            A = ((A << 1) & 0x7F);
            A |= (((F & CARRY_FLAG) >> CARRY_FLAG_BIT) << 7);
            CLRBIT(F, CARRY_FLAG_BIT);
        }

        // Detect zero
        ((A & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // Rotate n right through carry
    inline void RR(byte & reg)
    {
        // Reset N and H flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // If LSB is high, set carry flag
        if ((reg & 0x01) >> 7)
        {
            reg = ((reg >> 1) & 0x7F);
            reg |= (((F & CARRY_FLAG) >> CARRY_FLAG_BIT) << 7);
            SETBIT(F, CARRY_FLAG_BIT);
        }
        else
        {
            reg = ((reg << 1) & 0x7F);
            reg |= (((F & CARRY_FLAG) >> CARRY_FLAG_BIT) << 7);
            CLRBIT(F, CARRY_FLAG_BIT);
        }

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // SLA n
    inline void SLA(byte & reg)
    {
        // reset N and H
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // Set Carry flag if the msb is high
        if ((reg & 0x80) >> 7)
        {
            SETBIT(F, CARRY_FLAG_BIT);
            reg = (reg << 1) & 0xFE;
        }
        else
        {
            CLRBIT(F, CARRY_FLAG_BIT);
            reg = (reg << 1) & 0xFE;
        }

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // SRA n
    inline void SRA(byte & reg)
    {
        // reset N and H
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // Set Carry flag if the lsb is high
        if ((reg & 0x01) )
        {
            SETBIT(F, CARRY_FLAG_BIT);
            reg = (reg & 0xFF) >> 1;
        }
        else
        {
            CLRBIT(F, CARRY_FLAG_BIT);
            reg = (reg & 0xFF) >> 1;
        }

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // SRA n
    inline void SRL(byte & reg)
    {
        // reset N and H
        CLRBIT(F, SUBTRACT_FLAG_BIT);
        CLRBIT(F, HALF_CARRY_FLAG_BIT);

        // Set Carry flag if the lsb is high
        if ((reg & 0x01))
        {
            SETBIT(F, CARRY_FLAG_BIT);
            reg = (reg >> 1) & 0x7F;
        }
        else
        {
            CLRBIT(F, CARRY_FLAG_BIT);
            reg = (reg >> 1) & 0x7F;
        }

        // Detect zero
        ((reg & 0xFF) == 0x00 ? SETBIT(F, ZERO_FLAG_BIT) : CLRBIT(F, ZERO_FLAG_BIT));
    }

    // BIT b,r
    inline void BIT(byte bit, byte & reg)
    {
        // Reset N flag
        CLRBIT(F, SUBTRACT_FLAG_BIT);

        // Set H flag
        SETBIT(F, HALF_CARRY_FLAG_BIT);

        // Set Z flag if bit of reg is 0
        if ((reg & (1 << bit)))
            SETBIT(F, ZERO_FLAG_BIT);
        else
            CLRBIT(F, ZERO_FLAG_BIT);
    }

    // JP cc
    inline void JP()
    {
        // Get the LSB first
        byte lsb = MEM[PC++];
        byte msb = MEM[PC++];

        PC = CASTWD(msb, lsb);
    }

    // JR cc
    inline void JR()
    {
        // Get the immediate byte
        byte n = MEM[PC++];

        PC += n;
    }

    // CALL cc
    inline void CALL()
    {
        // Get the immediate instruction
        byte lsb = MEM[PC++];
        byte msb = MEM[PC++];

        // Push the address of the next instruction onto the stack
        PUSH(((PC) >> 8), ((PC) & 0x00FF));

        PC = CASTWD(msb, lsb);
    }

    // RESET
    inline void RST(byte n)
    {
        // Push the address of the current instruction onto the stack
        PUSH(((PC) >> 8), ((PC) & 0x00FF));

        // Jump to the address $0000 + n
        PC = CASTWD(0x00, n);
    }

    // RET cc
    inline void RET()
    {
        // Get the immediate instruction
        byte lsb;
        byte msb;

        // Push the address of the next instruction onto the stack
        POP(msb, lsb);

        PC = CASTWD(msb, lsb);
    }

	/*
	CPU Opcode Execution Instruction Macros
	*/

	void OP00(){ cout << "Hello world!" << endl; cycles += 4;  } // NOP
	void OP01(){ LD(B, MEM[PC++]); LD(C, MEM[PC++]); cycles += 12; }
	void OP02(){ LD(MEM[CASTWD(B, C)], A); cycles += 8; }
	void OP03(){ INC(B, C); cycles += 8; }
	void OP04(){ INCR(B); cycles += 4; }
	void OP05(){ DECR(B); cycles += 4; }
	void OP06(){ LD(B, MEM[PC++]); cycles += 8; }
	void OP07(){ RLCA(); cycles += 4; }
	void OP08(){ LD(MEM[CASTWD(MEM[PC++], MEM[PC++]) ], SP & 0xFF); cycles += 20; }
	void OP09(){ ADD(CASTWD(B, C)); cycles += 8; }
	void OP0A(){ LD(A, MEM[CASTWD(B, C)]); cycles += 8; }
	void OP0B(){ DEC(B, C); cycles += 8; }
	void OP0C(){ INCR(C); cycles += 4; }
	void OP0D(){ DECR(C); cycles += 4; }
	void OP0E(){ LD(C, MEM[PC++]); cycles += 8; }
	void OP0F(){ RRCA(); cycles += 4; }

	void OP10(){ cycles += 4; /* TODO: HALT CPU and LCD display until button pressed */ } // STOP
	void OP11(){ LD(D, MEM[PC++]); LD(E, MEM[PC++]); cycles += 12; }
	void OP12(){ LD(MEM[CASTWD(D, E)], A); cycles += 8; }
	void OP13(){ INC(D, E); cycles += 8; }
	void OP14(){ INCR(D); cycles += 4; }
	void OP15(){ DECR(D); cycles += 4; }
	void OP16(){ LD(D, MEM[PC++]); cycles += 8; }
	void OP17(){ RLA(); cycles += 4; }
	void OP18(){ JR(); cycles += 8; }
	void OP19(){ ADD(CASTWD(D, E)); cycles += 8; }
	void OP1A(){ LD(A, MEM[CASTWD(D, L)]); cycles += 8; }
	void OP1B(){ DEC(D, E); cycles += 8; }
	void OP1C(){ INCR(E); cycles += 4; }
	void OP1D(){ DECR(E); cycles += 4; }
	void OP1E(){ LD(E, MEM[PC++]); cycles += 8; }
	void OP1F(){ RRA(); cycles += 4; }

	void OP20(){ if (!(F & ZERO_FLAG)) JR(); cycles += 8; }
	void OP21(){ LD(H, MEM[PC++]); LD(L, MEM[PC++]); cycles += 12; }
	void OP22(){ LD(MEM[CASTWD(H, L)], A); INC(H, L); cycles += 8; }
	void OP23(){ INC(H, L); cycles += 8; }
	void OP24(){ INCR(H); cycles += 4; }
	void OP25(){ DECR(H); cycles += 4; }
	void OP26(){ LD(H, MEM[PC++]); cycles += 8; }
	void OP27(){ DAA(); cycles += 4; }
	void OP28(){ if ((F & ZERO_FLAG)) JR(); cycles += 8; }
	void OP29(){ ADD(CASTWD(H, L)); cycles += 8; }
	void OP2A(){ LD(A, MEM[CASTWD(H, L)]); INC(H, L); cycles += 8; }
	void OP2B(){ DEC(H, L); cycles += 8; }
	void OP2C(){ INCR(L); cycles += 4; }
	void OP2D(){ DECR(L); cycles += 4; }
	void OP2E(){ LD(L, MEM[PC++]); cycles += 8; }
	void OP2F(){ A = ~A; SETBIT(F, SUBTRACT_FLAG_BIT); SETBIT(F, HALF_CARRY_FLAG_BIT); cycles += 4; }

	void OP30(){ if (!(F & CARRY_FLAG)) JR(); cycles += 8; }
	void OP31(){ LD(SP, CASTWD(MEM[PC++], MEM[PC++])); cycles += 12; }
	void OP32(){ LD(MEM[CASTWD(H, L)], A); DEC(H, L); cycles += 8; }
	void OP33(){ ++SP; cycles += 8; }
	void OP34(){ INCR(MEM[CASTWD(H, L)] ); cycles += 12; }
	void OP35(){ DECR(MEM[CASTWD(H, L)]); cycles += 12; }
	void OP36(){ LD(MEM[CASTWD(H, L)], MEM[PC++]); cycles += 12; }
	void OP37(){ SETBIT(F, HALF_CARRY_FLAG_BIT); CLRBIT(F, SUBTRACT_FLAG_BIT); CLRBIT(F, HALF_CARRY_FLAG_BIT); cycles += 4; }
	void OP38(){ if ((F & CARRY_FLAG)) JR(); cycles += 8; }
	void OP39(){ ADD(SP); cycles += 8; }
	void OP3A(){ LD(A, MEM[CASTWD(H, L)]); DEC(H, L); cycles += 8; }
	void OP3B(){ --SP; cycles += 8; }
	void OP3C(){ INCR(A); cycles += 4; }
	void OP3D(){ DECR(A); cycles += 4; }
	void OP3E(){ LD(A, MEM[PC++]); cycles += 8; }
	void OP3F(){ ((F & CARRY_FLAG) ? CLRBIT(F, CARRY_FLAG_BIT) : SETBIT(F, HALF_CARRY_FLAG_BIT)); cycles += 4; 
                 SETBIT(F, SUBTRACT_FLAG_BIT); SETBIT(F, HALF_CARRY_FLAG_BIT); }

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
	void OP76(){ while( MEM[INTERRUPT_FLAG] == 0x00 ); cycles += 4; } // HALT until an INTERRUPT occurs
	void OP77(){ LD(MEM[CASTWD(H, L)], A); cycles += 8; }
	void OP78(){ LD(A, B); cycles += 4; }
	void OP79(){ LD(A, C); cycles += 4; }
	void OP7A(){ LD(A, D); cycles += 4; }
	void OP7B(){ LD(A, E); cycles += 4; }
	void OP7C(){ LD(A, H); cycles += 4; }
	void OP7D(){ LD(A, L); cycles += 4; }
	void OP7E(){ LD(A, MEM[CASTWD(H, L)]); cycles += 8; }
	void OP7F(){ LD(A, A); cycles += 4; }

    void OP80(){ ADD(A, B); cycles += 4; }
	void OP81(){ ADD(A, C); cycles += 4; }
	void OP82(){ ADD(A, D); cycles += 4; }
	void OP83(){ ADD(A, E); cycles += 4; }
	void OP84(){ ADD(A, H); cycles += 4; }
	void OP85(){ ADD(A, L); cycles += 4; }
	void OP86(){ ADD(A, MEM[CASTWD(H, L)]); cycles += 8; }
    void OP87(){ ADD(A, A); cycles += 4; }
	void OP88(){ ADD(A, B + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }
	void OP89(){ ADD(A, C + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }
	void OP8A(){ ADD(A, D + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }
	void OP8B(){ ADD(A, E + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }
	void OP8C(){ ADD(A, H + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }
	void OP8D(){ ADD(A, L + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }
	void OP8E(){ ADD(A, MEM[CASTWD(H,L)] + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 8; }
	void OP8F(){ ADD(A, A + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }

	void OP90(){ SUB(A, B); cycles += 4; }
	void OP91(){ SUB(A, C); cycles += 4; }
	void OP92(){ SUB(A, D); cycles += 4; }
	void OP93(){ SUB(A, E); cycles += 4; }
	void OP94(){ SUB(A, H); cycles += 4; }
	void OP95(){ SUB(A, L); cycles += 4; }
	void OP96(){ SUB(A, MEM[CASTWD(H, L)]); cycles += 8; }
	void OP97(){ SUB(A, A); cycles += 4; }
	void OP98(){ SUB(A, B + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }
	void OP99(){ SUB(A, C + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }
	void OP9A(){ SUB(A, D + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }
	void OP9B(){ SUB(A, E + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }
	void OP9C(){ SUB(A, H + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }
	void OP9D(){ SUB(A, L + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }
	void OP9E(){ SUB(A, MEM[CASTWD(H,L)] + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 8; }
	void OP9F(){ SUB(A, A + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 4; }

	void OPA0(){ AND(A, B); cycles += 4; }
	void OPA1(){ AND(A, C); cycles += 4; }
	void OPA2(){ AND(A, D); cycles += 4; }
	void OPA3(){ AND(A, E); cycles += 4; }
	void OPA4(){ AND(A, H); cycles += 4; }
	void OPA5(){ AND(A, L); cycles += 4; }
	void OPA6(){ AND(A, MEM[CASTWD(H,L)]); cycles += 8; }
	void OPA7(){ AND(A, B); cycles += 4; }
	void OPA8(){ XOR(A, B); cycles += 4; }
	void OPA9(){ XOR(A, C); cycles += 4; }
    void OPAA(){ XOR(A, D); cycles += 4; }
    void OPAB(){ XOR(A, E); cycles += 4; }
    void OPAC(){ XOR(A, H); cycles += 4; }
    void OPAD(){ XOR(A, L); cycles += 4; }
    void OPAE(){ XOR(A, MEM[CASTWD(H, L)]); cycles += 8; }
	void OPAF(){ XOR(A, A); cycles += 4; }

	void OPB0(){ OR(A, B); cycles += 4; }
	void OPB1(){ OR(A, C); cycles += 4; }
	void OPB2(){ OR(A, D); cycles += 4; }
	void OPB3(){ OR(A, E); cycles += 4; }
	void OPB4(){ OR(A, H); cycles += 4; }
	void OPB5(){ OR(A, L); cycles += 4; }
	void OPB6(){ OR(A, MEM[CASTWD(H,L)]); cycles += 8; }
	void OPB7(){ OR(A, A); cycles += 4; }
	void OPB8(){ CP(A, B); cycles += 4; }
	void OPB9(){ CP(A, C); cycles += 4; }
	void OPBA(){ CP(A, D); cycles += 4; }
	void OPBB(){ CP(A, E); cycles += 4; }
	void OPBC(){ CP(A, H); cycles += 4; }
	void OPBD(){ CP(A, L); cycles += 4; }
	void OPBE(){ CP(A, MEM[CASTWD(H, L)]); cycles += 8; }
	void OPBF(){ CP(A, A); cycles += 4; }

	void OPC0(){ if (!(F & ZERO_FLAG)) RET(); cycles += 8; }
	void OPC1(){ POP(B, C); cycles += 12; }
	void OPC2(){ if( !(F & ZERO_FLAG) ) JP(); cycles += 12; }
	void OPC3(){ JP(); cycles += 12; }
	void OPC4(){ if (!(F & ZERO_FLAG)) CALL(); cycles += 12; }
	void OPC5(){ PUSH(B, C); cycles += 16; }
	void OPC6(){ ADD(A, MEM[PC++]); cycles += 8; }
	void OPC7(){ RST( 0x00 ); cycles += 32; }
	void OPC8(){ if ((F & ZERO_FLAG)) RET(); cycles += 8; }
	void OPC9(){ RET(); cycles += 8; }
	void OPCA(){ if ((F & ZERO_FLAG)) JP(); cycles += 12; }
	void OPCB(){ /* PREFIX CB OPCODES - DO NOT USE. */}
	void OPCC(){ if ((F & ZERO_FLAG)) CALL(); cycles += 12; }
	void OPCD(){ CALL(); cycles += 12; } // CALL nn
	void OPCE(){ ADD(A, MEM[PC++] + ((F & CARRY_FLAG) >> CARRY_FLAG_BIT)); cycles += 8; }
	void OPCF(){ RST(0x08); cycles += 32; }

	void OPD0(){ if (!(F & CARRY_FLAG)) RET(); cycles += 8; }
	void OPD1(){ POP(D, E); cycles += 12; }
	void OPD2(){ if (!(F & CARRY_FLAG)) JP(); cycles += 12; }
	void OPD3(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPD4(){ if (!(F & CARRY_FLAG)) CALL(); cycles += 12; }
	void OPD5(){ PUSH(D, E); cycles += 16; }
	void OPD6(){ SUB(A, MEM[PC++]); cycles += 8; }
	void OPD7(){ RST(0x10); cycles += 32; }
	void OPD8(){ if ((F & CARRY_FLAG)) RET(); cycles += 8; }
	void OPD9(){ RET(); MEM[INTERRUPT_ENABLE] = 0x01; cycles += 8;}
	void OPDA(){ if ((F & CARRY_FLAG)) JP(); cycles += 12; }
	void OPDB(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPDC(){ if ((F & CARRY_FLAG)) CALL(); cycles += 12; }
	void OPDD(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPDE(){ } // TODO: Check if this opcode exists
	void OPDF(){ RST(0x18); cycles += 32; }

	void OPE0(){ LD(MEM[0xFF00 + MEM[PC++]], A); cycles += 12; }
	void OPE1(){ POP(H, L); cycles += 12; }
	void OPE2(){ LD(MEM[0xFF00 + C], A); cycles += 8; }
	void OPE3(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPE4(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPE5(){ PUSH(H, L); cycles += 16; }
	void OPE6(){ AND(A, MEM[PC++]); cycles += 8; }
	void OPE7(){ RST(0x20); cycles += 32; }
	void OPE8(){ ADDSP(); cycles += 16; }
	void OPE9(){ PC = CASTWD(H,L); cycles += 4; } // JP (HL)
	void OPEA(){ LD(MEM[CASTWD(MEM[PC++], MEM[PC++])], A); cycles += 16; }
	void OPEB(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPEC(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPED(){ /* DO NOTHING - BLANK OPCODE */ }
    void OPEE(){ XOR(A, MEM[PC++]); cycles += 8; }
	void OPEF(){ RST(0x28); cycles += 32; }

	void OPF0(){ LD(A, MEM[0xFF00 + MEM[PC++]]); cycles += 12; }
	void OPF1(){ POP(A, F); cycles += 12; }
	void OPF2(){ LD(A, MEM[0xFF00 + C]); cycles += 8; }
	void OPF3(){ DI_Executed = true; cycles += 4; } // DI
	void OPF4(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPF5(){ PUSH(A, F); cycles += 16; }
	void OPF6(){ OR(A, MEM[PC++]); cycles += 8; }
	void OPF7(){ RST(0x30); cycles += 32; }
	void OPF8(){ LD(H, (SP + MEM[PC]) >> 8); LD(L, (SP + MEM[PC++]) & 0xFF); cycles += 12; } // TODO: Status flag settings
	void OPF9(){ LD(SP, CASTWD(H, L)); cycles += 8; }
	void OPFA(){ LD(A, MEM[CASTWD(MEM[PC++], MEM[PC++])]); cycles += 16; }
    void OPFB(){ DI_Executed = false; cycles += 4; } // EI
	void OPFC(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPFD(){ /* DO NOTHING - BLANK OPCODE */ }
	void OPFE(){ CP(A, MEM[PC++]); cycles += 8; }
	void OPFF(){ RST(0x38); cycles += 32; }
	

    /* CB Operations */

    void CBOP00(){ RLC(B); cycles += 8;}
    void CBOP01(){ RLC(C); cycles += 8;}
    void CBOP02(){ RLC(D); cycles += 8;}
    void CBOP03(){ RLC(E); cycles += 8;}
    void CBOP04(){ RLC(H); cycles += 8;}
    void CBOP05(){ RLC(L); cycles += 8;}
    void CBOP06(){ RLC(MEM[CASTWD(H, L)]); cycles += 16;}
    void CBOP07(){ RLC(A); cycles += 8;}
    void CBOP08(){ RRC(B); cycles += 8; }
    void CBOP09(){ RRC(C); cycles += 8; }
    void CBOP0A(){ RRC(D); cycles += 8; }
    void CBOP0B(){ RRC(E); cycles += 8; }
    void CBOP0C(){ RRC(H); cycles += 8; }
    void CBOP0D(){ RRC(L); cycles += 8; }
    void CBOP0E(){ RRC(MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP0F(){ RRC(A); cycles += 8; }

    void CBOP10(){ RL(B); cycles += 8; }
    void CBOP11(){ RL(C); cycles += 8; }
    void CBOP12(){ RL(D); cycles += 8; }
    void CBOP13(){ RL(E); cycles += 8; }
    void CBOP14(){ RL(H); cycles += 8; }
    void CBOP15(){ RL(L); cycles += 8; }
    void CBOP16(){ RL(MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP17(){ RL(A); cycles += 8; }
    void CBOP18(){ RR(B); cycles += 8; }
    void CBOP19(){ RR(C); cycles += 8; }
    void CBOP1A(){ RR(D); cycles += 8; }
    void CBOP1B(){ RR(E); cycles += 8; }
    void CBOP1C(){ RR(H); cycles += 8; }
    void CBOP1D(){ RR(L); cycles += 8; }
    void CBOP1E(){ RR(MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP1F(){ RR(A); cycles += 8; }

    void CBOP20(){ SLA(B); cycles += 8; }
    void CBOP21(){ SLA(C); cycles += 8; }
    void CBOP22(){ SLA(D); cycles += 8; }
    void CBOP23(){ SLA(E); cycles += 8; }
    void CBOP24(){ SLA(H); cycles += 8; }
    void CBOP25(){ SLA(L); cycles += 8; }
    void CBOP26(){ SLA(MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP27(){ SLA(A); cycles += 8; }
    void CBOP28(){ SRA(B); cycles += 8; }
    void CBOP29(){ SRA(C); cycles += 8; }
    void CBOP2A(){ SRA(D); cycles += 8; }
    void CBOP2B(){ SRA(E); cycles += 8; }
    void CBOP2C(){ SRA(H); cycles += 8; }
    void CBOP2D(){ SRA(L); cycles += 8; }
    void CBOP2E(){ SRA(MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP2F(){ SRA(A); cycles += 8; }

    void CBOP30(){ SWAP(B); cycles += 8; }
    void CBOP31(){ SWAP(C); cycles += 8; }
    void CBOP32(){ SWAP(D); cycles += 8; }
    void CBOP33(){ SWAP(E); cycles += 8; }
    void CBOP34(){ SWAP(H); cycles += 8; }
    void CBOP35(){ SWAP(L); cycles += 8; }
    void CBOP36(){ SWAP(MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP37(){ SWAP(A); cycles += 8; }
    void CBOP38(){ SRL(B); cycles += 8; }
    void CBOP39(){ SRL(C); cycles += 8; }
    void CBOP3A(){ SRL(D); cycles += 8; }
    void CBOP3B(){ SRL(E); cycles += 8; }
    void CBOP3C(){ SRL(H); cycles += 8; }
    void CBOP3D(){ SRL(L); cycles += 8; }
    void CBOP3E(){ SRL(MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP3F(){ SRL(A); cycles += 8; }

    void CBOP40(){ BIT(0, B); cycles += 8; }
    void CBOP41(){ BIT(0, C); cycles += 8; }
    void CBOP42(){ BIT(0, D); cycles += 8; }
    void CBOP43(){ BIT(0, E); cycles += 8; }
    void CBOP44(){ BIT(0, H); cycles += 8; }
    void CBOP45(){ BIT(0, L); cycles += 8; }
    void CBOP46(){ BIT(0, MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP47(){ BIT(0, A); cycles += 8; }
    void CBOP48(){ BIT(1, B); cycles += 8; }
    void CBOP49(){ BIT(1, C); cycles += 8; }
    void CBOP4A(){ BIT(1, D); cycles += 8; }
    void CBOP4B(){ BIT(1, E); cycles += 8; }
    void CBOP4C(){ BIT(1, H); cycles += 8; }
    void CBOP4D(){ BIT(1, L); cycles += 8; }
    void CBOP4E(){ BIT(1, MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP4F(){ BIT(1, A); cycles += 8; }

    void CBOP50(){ BIT(2, B); cycles += 8; }
    void CBOP51(){ BIT(2, C); cycles += 8; }
    void CBOP52(){ BIT(2, D); cycles += 8; }
    void CBOP53(){ BIT(2, E); cycles += 8; }
    void CBOP54(){ BIT(2, H); cycles += 8; }
    void CBOP55(){ BIT(2, L); cycles += 8; }
    void CBOP56(){ BIT(2, MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP57(){ BIT(2, A); cycles += 8; }
    void CBOP58(){ BIT(3, B); cycles += 8; }
    void CBOP59(){ BIT(3, C); cycles += 8; }
    void CBOP5A(){ BIT(3, D); cycles += 8; }
    void CBOP5B(){ BIT(3, E); cycles += 8; }
    void CBOP5C(){ BIT(3, H); cycles += 8; }
    void CBOP5D(){ BIT(3, L); cycles += 8; }
    void CBOP5E(){ BIT(3, MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP5F(){ BIT(3, A); cycles += 8; }

    void CBOP60(){ BIT(4, B); cycles += 8; }
    void CBOP61(){ BIT(4, C); cycles += 8; }
    void CBOP62(){ BIT(4, D); cycles += 8; }
    void CBOP63(){ BIT(4, E); cycles += 8; }
    void CBOP64(){ BIT(4, H); cycles += 8; }
    void CBOP65(){ BIT(4, L); cycles += 8; }
    void CBOP66(){ BIT(4, MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP67(){ BIT(4, A); cycles += 8; }
    void CBOP68(){ BIT(5, B); cycles += 8; }
    void CBOP69(){ BIT(5, C); cycles += 8; }
    void CBOP6A(){ BIT(5, D); cycles += 8; }
    void CBOP6B(){ BIT(5, E); cycles += 8; }
    void CBOP6C(){ BIT(5, H); cycles += 8; }
    void CBOP6D(){ BIT(5, L); cycles += 8; }
    void CBOP6E(){ BIT(5, MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP6F(){ BIT(5, A); cycles += 8; }

    void CBOP70(){ BIT(6, B); cycles += 8; }
    void CBOP71(){ BIT(6, C); cycles += 8; }
    void CBOP72(){ BIT(6, D); cycles += 8; }
    void CBOP73(){ BIT(6, E); cycles += 8; }
    void CBOP74(){ BIT(6, H); cycles += 8; }
    void CBOP75(){ BIT(6, L); cycles += 8; }
    void CBOP76(){ BIT(6, MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP77(){ BIT(6, A); cycles += 8; }
    void CBOP78(){ BIT(7, B); cycles += 8; }
    void CBOP79(){ BIT(7, C); cycles += 8; }
    void CBOP7A(){ BIT(7, D); cycles += 8; }
    void CBOP7B(){ BIT(7, E); cycles += 8; }
    void CBOP7C(){ BIT(7, H); cycles += 8; }
    void CBOP7D(){ BIT(7, L); cycles += 8; }
    void CBOP7E(){ BIT(7, MEM[CASTWD(H, L)]); cycles += 16; }
    void CBOP7F(){ BIT(7, A); cycles += 8; }

    // RES
    void CBOP80(){ CLRBIT(B, 0); cycles += 8; }
    void CBOP81(){ CLRBIT(C, 0); cycles += 8; }
    void CBOP82(){ CLRBIT(D, 0); cycles += 8; }
    void CBOP83(){ CLRBIT(E, 0); cycles += 8; }
    void CBOP84(){ CLRBIT(H, 0); cycles += 8; }
    void CBOP85(){ CLRBIT(L, 0); cycles += 8; }
    void CBOP86(){ CLRBIT(MEM[CASTWD(H,L)], 0); cycles += 8; }
    void CBOP87(){ CLRBIT(A, 1); cycles += 8; }
    void CBOP88(){ CLRBIT(B, 1); cycles += 8; }
    void CBOP89(){ CLRBIT(C, 1); cycles += 8; }
    void CBOP8A(){ CLRBIT(D, 1); cycles += 8; }
    void CBOP8B(){ CLRBIT(E, 1); cycles += 8; }
    void CBOP8C(){ CLRBIT(H, 1); cycles += 8; }
    void CBOP8D(){ CLRBIT(L, 1); cycles += 8; }
    void CBOP8E(){ CLRBIT(MEM[CASTWD(H,L)], 1); cycles += 8; }
    void CBOP8F(){ CLRBIT(A, 1); cycles += 8; }

    void CBOP90(){ CLRBIT(B, 2); cycles += 8; }
    void CBOP91(){ CLRBIT(C, 2); cycles += 8; }
    void CBOP92(){ CLRBIT(D, 2); cycles += 8; }
    void CBOP93(){ CLRBIT(E, 2); cycles += 8; }
    void CBOP94(){ CLRBIT(H, 2); cycles += 8; }
    void CBOP95(){ CLRBIT(L, 2); cycles += 8; }
    void CBOP96(){ CLRBIT(MEM[CASTWD(H, L)], 2); cycles += 8; }
    void CBOP97(){ CLRBIT(A, 2); cycles += 8; }
    void CBOP98(){ CLRBIT(B, 3); cycles += 8; }
    void CBOP99(){ CLRBIT(C, 3); cycles += 8; }
    void CBOP9A(){ CLRBIT(D, 3); cycles += 8; }
    void CBOP9B(){ CLRBIT(E, 3); cycles += 8; }
    void CBOP9C(){ CLRBIT(H, 3); cycles += 8; }
    void CBOP9D(){ CLRBIT(L, 3); cycles += 8; }
    void CBOP9E(){ CLRBIT(MEM[CASTWD(H, L)], 3); cycles += 8; }
    void CBOP9F(){ CLRBIT(A, 3); cycles += 8; }

    void CBOPA0(){ CLRBIT(B, 4); cycles += 8; }
    void CBOPA1(){ CLRBIT(C, 4); cycles += 8; }
    void CBOPA2(){ CLRBIT(D, 4); cycles += 8; }
    void CBOPA3(){ CLRBIT(E, 4); cycles += 8; }
    void CBOPA4(){ CLRBIT(H, 4); cycles += 8; }
    void CBOPA5(){ CLRBIT(L, 4); cycles += 8; }
    void CBOPA6(){ CLRBIT(MEM[CASTWD(H, L)], 4); cycles += 8; }
    void CBOPA7(){ CLRBIT(A, 4); cycles += 8; }
    void CBOPA8(){ CLRBIT(B, 5); cycles += 8; }
    void CBOPA9(){ CLRBIT(C, 5); cycles += 8; }
    void CBOPAA(){ CLRBIT(D, 5); cycles += 8; }
    void CBOPAB(){ CLRBIT(E, 5); cycles += 8; }
    void CBOPAC(){ CLRBIT(H, 5); cycles += 8; }
    void CBOPAD(){ CLRBIT(L, 5); cycles += 8; }
    void CBOPAE(){ CLRBIT(MEM[CASTWD(H, L)], 5); cycles += 8; }
    void CBOPAF(){ CLRBIT(A, 5); cycles += 8; }

    void CBOPB0(){ CLRBIT(B, 6); cycles += 8; }
    void CBOPB1(){ CLRBIT(C, 6); cycles += 8; }
    void CBOPB2(){ CLRBIT(D, 6); cycles += 8; }
    void CBOPB3(){ CLRBIT(E, 6); cycles += 8; }
    void CBOPB4(){ CLRBIT(H, 6); cycles += 8; }
    void CBOPB5(){ CLRBIT(L, 6); cycles += 8; }
    void CBOPB6(){ CLRBIT(MEM[CASTWD(H, L)], 6); cycles += 8; }
    void CBOPB7(){ CLRBIT(A, 6); cycles += 8; }
    void CBOPB8(){ CLRBIT(B, 7); cycles += 8; }
    void CBOPB9(){ CLRBIT(C, 7); cycles += 8; }
    void CBOPBA(){ CLRBIT(D, 7); cycles += 8; }
    void CBOPBB(){ CLRBIT(E, 7); cycles += 8; }
    void CBOPBC(){ CLRBIT(H, 7); cycles += 8; }
    void CBOPBD(){ CLRBIT(L, 7); cycles += 8; }
    void CBOPBE(){ CLRBIT(MEM[CASTWD(H, L)], 7); cycles += 8; }
    void CBOPBF(){ CLRBIT(A, 7); cycles += 8; }

    // SET
    void CBOPC0(){ SETBIT(B, 0); cycles += 8; }
    void CBOPC1(){ SETBIT(C, 0); cycles += 8; }
    void CBOPC2(){ SETBIT(D, 0); cycles += 8; }
    void CBOPC3(){ SETBIT(E, 0); cycles += 8; }
    void CBOPC4(){ SETBIT(H, 0); cycles += 8; }
    void CBOPC5(){ SETBIT(L, 0); cycles += 8; }
    void CBOPC6(){ SETBIT(MEM[CASTWD(H,L)], 0); cycles += 8; }
    void CBOPC7(){ SETBIT(A, 1); cycles += 8; }
    void CBOPC8(){ SETBIT(B, 1); cycles += 8; }
    void CBOPC9(){ SETBIT(C, 1); cycles += 8; }
    void CBOPCA(){ SETBIT(D, 1); cycles += 8; }
    void CBOPCB(){ SETBIT(E, 1); cycles += 8; }
    void CBOPCC(){ SETBIT(H, 1); cycles += 8; }
    void CBOPCD(){ SETBIT(L, 1); cycles += 8; }
    void CBOPCE(){ SETBIT(MEM[CASTWD(H,L)], 1); cycles += 8; }
    void CBOPCF(){ SETBIT(A, 1); cycles += 8; }
  
    void CBOPD0(){ SETBIT(B, 2); cycles += 8; }
    void CBOPD1(){ SETBIT(C, 2); cycles += 8; }
    void CBOPD2(){ SETBIT(D, 2); cycles += 8; }
    void CBOPD3(){ SETBIT(E, 2); cycles += 8; }
    void CBOPD4(){ SETBIT(H, 2); cycles += 8; }
    void CBOPD5(){ SETBIT(L, 2); cycles += 8; }
    void CBOPD6(){ SETBIT(MEM[CASTWD(H, L)], 2); cycles += 8; }
    void CBOPD7(){ SETBIT(A, 2); cycles += 8; }
    void CBOPD8(){ SETBIT(B, 3); cycles += 8; }
    void CBOPD9(){ SETBIT(C, 3); cycles += 8; }
    void CBOPDA(){ SETBIT(D, 3); cycles += 8; }
    void CBOPDB(){ SETBIT(E, 3); cycles += 8; }
    void CBOPDC(){ SETBIT(H, 3); cycles += 8; }
    void CBOPDD(){ SETBIT(L, 3); cycles += 8; }
    void CBOPDE(){ SETBIT(MEM[CASTWD(H, L)], 3); cycles += 8; }
    void CBOPDF(){ SETBIT(A, 3); cycles += 8; }

    void CBOPE0(){ SETBIT(B, 4); cycles += 8; }
    void CBOPE1(){ SETBIT(C, 4); cycles += 8; }
    void CBOPE2(){ SETBIT(D, 4); cycles += 8; }
    void CBOPE3(){ SETBIT(E, 4); cycles += 8; }
    void CBOPE4(){ SETBIT(H, 4); cycles += 8; }
    void CBOPE5(){ SETBIT(L, 4); cycles += 8; }
    void CBOPE6(){ SETBIT(MEM[CASTWD(H, L)], 4); cycles += 8; }
    void CBOPE7(){ SETBIT(A, 4); cycles += 8; }
    void CBOPE8(){ SETBIT(B, 5); cycles += 8; }
    void CBOPE9(){ SETBIT(C, 5); cycles += 8; }
    void CBOPEA(){ SETBIT(D, 5); cycles += 8; }
    void CBOPEB(){ SETBIT(E, 5); cycles += 8; }
    void CBOPEC(){ SETBIT(H, 5); cycles += 8; }
    void CBOPED(){ SETBIT(L, 5); cycles += 8; }
    void CBOPEE(){ SETBIT(MEM[CASTWD(H, L)], 5); cycles += 8; }
    void CBOPEF(){ SETBIT(A, 5); cycles += 8; }

    void CBOPF0(){ SETBIT(B, 6); cycles += 8; }
    void CBOPF1(){ SETBIT(C, 6); cycles += 8; }
    void CBOPF2(){ SETBIT(D, 6); cycles += 8; }
    void CBOPF3(){ SETBIT(E, 6); cycles += 8; }
    void CBOPF4(){ SETBIT(H, 6); cycles += 8; }
    void CBOPF5(){ SETBIT(L, 6); cycles += 8; }
    void CBOPF6(){ SETBIT(MEM[CASTWD(H, L)], 6); cycles += 8; }
    void CBOPF7(){ SETBIT(A, 6); cycles += 8; }
    void CBOPF8(){ SETBIT(B, 7); cycles += 8; }
    void CBOPF9(){ SETBIT(C, 7); cycles += 8; }
    void CBOPFA(){ SETBIT(D, 7); cycles += 8; }
    void CBOPFB(){ SETBIT(E, 7); cycles += 8; }
    void CBOPFC(){ SETBIT(H, 7); cycles += 8; }
    void CBOPFD(){ SETBIT(L, 7); cycles += 8; }
    void CBOPFE(){ SETBIT(MEM[CASTWD(H, L)], 7); cycles += 8; }
    void CBOPFF(){ SETBIT(A, 7); cycles += 8; }
};



#endif