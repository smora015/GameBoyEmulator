/*
    Name:        opcodes.cpp
    Author:      Sergio Morales
    Date:        08/30/2016
    Description: This file contains the definitions for every opcode and
                 prefix-CB opcode used in the GameBoy CPU.            
    
*/
#include "GBCPU.h"
#include "gameboy.h"

// ADD A, n
inline void GBCPU::ADD(byte & reg, byte arg)
{
    // TODO: remove reg parameter, as this will always be A...
    // Reset N flag
    SUBTRACT_FLAG = false;

    // Detect half carry
    if (((reg & 0x0F) + (arg & 0x0F)) & 0x10)
        HALF_CARRY_FLAG = true;
    else
        HALF_CARRY_FLAG = false;

    // Detect carry
    if (((reg & 0x0FF) + (arg & 0x0FF)) & 0x100)
        CARRY_FLAG = true;
    else
        CARRY_FLAG = false;

    reg += arg;

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// ADD HL, n
inline void GBCPU::ADD(word arg)
{
    word HL = CASTWD(H, L);

    // Reset N flag
    SUBTRACT_FLAG = false;

    // Detect half carry
    if (((HL & 0x0FF) + (arg & 0x0FF)) & 0x1000)
        HALF_CARRY_FLAG = true;
    else
        HALF_CARRY_FLAG = false;

    // Detects carry
    if (((HL & 0xFFFF) + (arg & 0xFFFF)) & 0x10000)
        CARRY_FLAG = true;
    else
        CARRY_FLAG = false;

    HL += arg;

    H = (HL & 0xFF00) >> 8;
    L = (HL & 0x00FF);

    // Increment PC
    ++PC;
}

// ADD SP, n
inline void GBCPU::ADDSP()
{
    // TODO: Confirm H and C flag get set based off 11th and 15th bit
    byte arg = MEM[++PC];

    // Reset Z, N flag
    SUBTRACT_FLAG = false;
    ZERO_FLAG = false;

    // Detect half carry
    if (((SP & 0x0FF) + (arg & 0x0FF)) & 0x1000)
        HALF_CARRY_FLAG = true;
    else
        HALF_CARRY_FLAG = false;

    // Detects carry
    if (((SP & 0xFFFF) + (arg & 0xFFFF)) & 0x10000)
        CARRY_FLAG = true;
    else
        CARRY_FLAG = false;

    SP += arg;

    // Increment PC
    ++PC;
}

inline void GBCPU::SUB(byte & reg, byte arg)
{
    // Set N falg
    SUBTRACT_FLAG = true;

    // Detect half carry
    if (((reg - arg) & 0x0F) >= (reg & 0xF))
        HALF_CARRY_FLAG = true;
    else
        HALF_CARRY_FLAG = false;

    // Detect carry
    if (((reg - arg) & 0xFF) > (reg & 0xFF))
        CARRY_FLAG = true;
    else
        CARRY_FLAG = false;

    reg -= arg;

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

inline void GBCPU::AND(byte & reg, byte arg)
{
    // Reset N, C flag
    SUBTRACT_FLAG = false;
    CARRY_FLAG = false;

    // Set Half carry flag
    HALF_CARRY_FLAG = true;

    reg &= arg;

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

inline void GBCPU::OR(byte & reg, byte arg)
{
    // Reset N, H, and C flag
    SUBTRACT_FLAG = false;
    CARRY_FLAG = false;
    HALF_CARRY_FLAG = false;

    reg |= arg;

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

inline void GBCPU::XOR(byte & reg, byte arg)
{
    // Reset N, H, and C flag
    SUBTRACT_FLAG = false;
    CARRY_FLAG = false;
    HALF_CARRY_FLAG = false;

    reg ^= arg;

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

inline void GBCPU::CP(byte & reg, byte arg)
{
    // Set N falg
    SUBTRACT_FLAG = true;

    // Detect half carry
    if (((reg - arg) & 0x0F) >= (reg & 0xF))
        HALF_CARRY_FLAG = true;
    else
        HALF_CARRY_FLAG = false;

    // Detect carry
    if (((reg - arg) & 0xFF) > (reg & 0xFF))
        CARRY_FLAG = true;
    else
        CARRY_FLAG = false;

    // Detect zero
    (((reg - arg) & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

inline void GBCPU::INCR(byte & reg)
{
    // Reset N flag
    SUBTRACT_FLAG = false;

    // Detect half carry
    if (((reg & 0x0F) + (1 & 0x0F)) & 0x10)
        HALF_CARRY_FLAG = true;
    else
        HALF_CARRY_FLAG = false;

    ++reg;

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

inline void GBCPU::DECR(byte & reg)
{
    // Set N flag
    SUBTRACT_FLAG = true;

    // Detect half carry
    if (((reg - 1) & 0x0F) >= (reg & 0xF))
        HALF_CARRY_FLAG = true;
    else
        HALF_CARRY_FLAG = false;

    --reg;

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

inline void GBCPU::SWAP(byte & reg)
{
    // Clear N, H, C
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;
    CARRY_FLAG = false;

    reg = (reg << 4) | ((reg >> 4) & 0x0F) & 0xFF;

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// Decimal Adjust Register A
inline void GBCPU::DAA()
{
    // Reset flag H
    HALF_CARRY_FLAG = false;

    byte temp = A;

    // Adjust the binary decimal digits if we have a N and C or H flag
    if (SUBTRACT_FLAG == true)
    {
        if (HALF_CARRY_FLAG == true)
            temp = (temp - 6) & 0xFF;

        if (CARRY_FLAG == true)
            temp -= 0x60;
    }
    else
    {
        if ((temp & 0xF)>9 || (HALF_CARRY_FLAG == true))
            temp += 6;

        if ((temp & 0xFFF)>0x9F || (CARRY_FLAG == true))
            temp += 0x60;
    }

    A = temp;

    if ((temp & 0x100) == 0x100)
        HALF_CARRY_FLAG = true;
    else
        HALF_CARRY_FLAG = false;

    // Detect zero
    ((A & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;

}

// Rotate A left
inline void GBCPU::RLCA()
{
    // Reset N and H flag
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;

    // If MSB is high, set carry flag
    if ((A & 0x80) >> 7)
    {
        CARRY_FLAG = true;
        A = ((A << 1) & 0xFE) | 0x01;
    }
    else
    {
        CARRY_FLAG = false;
        A = ((A << 1) & 0xFE);
    }

    // Detect zero
    ((A & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// Rotate n left
inline void GBCPU::RLC(byte & reg)
{
    // Reset N and H flag
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;

    // If MSB is high, set carry flag
    if ((reg & 0x80) >> 7)
    {
        CARRY_FLAG = true;
        reg = ((reg << 1) & 0xFE) | 0x01;
    }
    else
    {
        CARRY_FLAG = false;
        reg = ((reg << 1) & 0xFE);
    }

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// Rotate A left with carry
inline void GBCPU::RLA()
{
    // Reset N and H flag
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;


    // If MSB is high, set carry flag and add the previous carry flag bit to LSB
    if ((A & 0x80) >> 7)
    {
        A = (A << 1) & 0xFE;
        A |= (CARRY_FLAG == true ? 0x01 : 0x00);
        CARRY_FLAG = true;
    }
    else
    {
        A = (A << 1) & 0xFE;
        A |= (CARRY_FLAG == true ? 0x01 : 0x00);
        CARRY_FLAG = false;
    }

    // Detect zero
    ((A & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// Rotate n left with carry
inline void GBCPU::RL(byte & reg)
{
    // Reset N and H flag
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;


    // If MSB is high, set carry flag and add the previous carry flag bit to LSB
    if ((reg & 0x80) >> 7)
    {
        reg = (reg << 1) & 0xFE;
        reg |= (CARRY_FLAG == true ? 0x01 : 0x00);
        CARRY_FLAG = true;
    }
    else
    {
        reg = (reg << 1) & 0xFE;
        reg |= (CARRY_FLAG == true ? 0x01 : 0x00);
        CARRY_FLAG = false;
    }

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// Rotate A right
inline void GBCPU::RRCA()
{
    // Reset N and H flag
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;

    // If LSB is high, set carry flag
    if ((A & 0x01) >> 7)
    {
        CARRY_FLAG = true;
        A = ((A >> 1) & 0x7F) | 0x80;
    }
    else
    {
        CARRY_FLAG = false;
        A = ((A << 1) & 0x7F);
    }

    // Detect zero
    ((A & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// Rotate n right
inline void GBCPU::RRC(byte & reg)
{
    // Reset N and H flag
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;

    // If LSB is high, set carry flag
    if ((reg & 0x01) >> 7)
    {
        CARRY_FLAG = true;
        reg = ((reg >> 1) & 0x7F) | 0x80;
    }
    else
    {
        CARRY_FLAG = false;
        reg = ((reg << 1) & 0x7F);
    }

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// Rotate A right through carry
inline void GBCPU::RRA()
{
    // Reset N and H flag
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;

    // If LSB is high, set carry flag
    if ((A & 0x01) >> 7)
    {
        A = ((A >> 1) & 0x7F);
        A |= ((CARRY_FLAG == true ? 0x01 : 0x00) << 7);
        CARRY_FLAG = true;
    }
    else
    {
        A = ((A << 1) & 0x7F);
        A |= ((CARRY_FLAG == true ? 0x01 : 0x00) << 7);
        CARRY_FLAG = false;
    }

    // Detect zero
    ((A & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// Rotate n right through carry
inline void GBCPU::RR(byte & reg)
{
    // Reset N and H flag
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;

    // If LSB is high, set carry flag
    if ((reg & 0x01) >> 7)
    {
        reg = ((reg >> 1) & 0x7F);
        reg |= ((CARRY_FLAG == true ? 0x01 : 0x00) << 7);
        CARRY_FLAG = true;
    }
    else
    {
        reg = ((reg << 1) & 0x7F);
        reg |= ((CARRY_FLAG == true ? 0x01 : 0x00) << 7);
        CARRY_FLAG = false;
    }

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// SLA n
inline void GBCPU::SLA(byte & reg)
{
    // reset N and H
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;

    // Set Carry flag if the msb is high
    if ((reg & 0x80) >> 7)
    {
        CARRY_FLAG = true;
        reg = (reg << 1) & 0xFE;
    }
    else
    {
        CARRY_FLAG = false;
        reg = (reg << 1) & 0xFE;
    }

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// SRA n
inline void GBCPU::SRA(byte & reg)
{
    // reset N and H
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;

    // Set Carry flag if the lsb is high
    if ((reg & 0x01))
    {
        CARRY_FLAG = true;
        reg = (reg & 0xFF) >> 1;
    }
    else
    {
        CARRY_FLAG = false;
        reg = (reg & 0xFF) >> 1;
    }

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// SRA n
inline void GBCPU::SRL(byte & reg)
{
    // reset N and H
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;

    // Set Carry flag if the lsb is high
    if ((reg & 0x01))
    {
        CARRY_FLAG = true;
        reg = (reg >> 1) & 0x7F;
    }
    else
    {
        CARRY_FLAG = false;
        reg = (reg >> 1) & 0x7F;
    }

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Increment PC
    ++PC;
}

// BIT b,r
inline void GBCPU::BIT(byte bit, byte & reg)
{
    // Reset N flag
    SUBTRACT_FLAG = false;

    // Set H flag
    HALF_CARRY_FLAG = true;

    // Set Z flag if bit of reg is 0
    if ((reg & (1 << bit)))
        ZERO_FLAG = true;
    else
        ZERO_FLAG = false;

    // Increment PC
    ++PC;
}

// JP cc
inline void GBCPU::JP()
{
    // Get the LSB first
    byte lsb = MEM[++PC];
    byte msb = MEM[++PC];

    PC = CASTWD(msb, lsb);
    cout << "Jumping to: " << PC << "or ";
    printf("%X%X!\n", msb, lsb);
}

// JR cc
inline void GBCPU::JR()
{
    // Get the immediate byte
    byte n = MEM[++PC];

    // Add to the current address, which is AFTER the immediate byte so we add 1
    PC = PC + 1 + n;
}

// CALL cc
inline void GBCPU::CALL()
{
    // Get the immediate instruction
    byte lsb = MEM[++PC];
    byte msb = MEM[++PC];

    // Push the address of the next instruction (PC + 1) onto the stack
    PUSH(((PC + 1) >> 8), ((PC + 1) & 0x00FF));

    PC = CASTWD(msb, lsb);
}

// RESET
inline void GBCPU::RST(byte n)
{
    // Push the address of the current instruction onto the stack
    PUSH(((PC) >> 8), ((PC) & 0x00FF));

    // Jump to the address $0000 + n
    PC = CASTWD(0x00, n);
}

// RET cc
inline void GBCPU::RET()
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

// TODO: Remove LD macro and replace by MEMREAD and MEMWRITES
// TODO: Finish opcode $F8 with status flag setting
// TODO: CHECK TO SEE IF ALL WORD LOADS GET THE LSB FIRST
// TODO: Increment PC only at the end
// TODO: Add comments for each opcode about what the instruction does
void GBCPU::OP00() { cout << "NOP" << endl; cycles = 4; ++PC; } // NOP
void GBCPU::OP01() { LD(C, MEM[++PC]); LD(B, MEM[++PC]); cycles = 12; ++PC; } // LD BC, ##
void GBCPU::OP02() { LD(MEM[CASTWD(B, C)], A); ++PC; cycles = 8; } // LD (BC), A
void GBCPU::OP03() { INC(B, C); cycles = 8; ++PC; }
void GBCPU::OP04() { INCR(B); cycles = 4; }
void GBCPU::OP05() { DECR(B); cycles = 4; }
void GBCPU::OP06() { LD(B, MEM[++PC]); ++PC; cycles = 8; }
void GBCPU::OP07() { RLCA(); cycles = 4; }
void GBCPU::OP08() { LD(MEM[CASTWD(MEM[++PC], MEM[++PC])], SP & 0xFF); ++PC; cycles = 20; } // TODO: FIX
void GBCPU::OP09() { ADD(CASTWD(B, C)); cycles = 8; }
void GBCPU::OP0A() { LD(A, MEM[CASTWD(B, C)]); ++PC; cycles = 8; }
void GBCPU::OP0B() { DEC(B, C); ++PC; cycles = 8; }
void GBCPU::OP0C() { INCR(C); cycles = 4; }
void GBCPU::OP0D() { DECR(C); cycles = 4; }
void GBCPU::OP0E() { LD(C, MEM[++PC]); ++PC; cycles = 8; }
void GBCPU::OP0F() { RRCA(); cycles = 4; }

void GBCPU::OP10() { ++PC; cycles = 4; cout << "HALT!" << endl;/* TODO: HALT CPU and LCD display until button pressed */ } // STOP
void GBCPU::OP11() { LD(E, MEM[++PC]); LD(D, MEM[++PC]); ++PC; cycles = 12; }
void GBCPU::OP12() { LD(MEM[CASTWD(D, E)], A); ++PC; cycles = 8; }
void GBCPU::OP13() { INC(D, E); ++PC; cycles = 8; }
void GBCPU::OP14() { INCR(D); cycles = 4; }
void GBCPU::OP15() { DECR(D); cycles = 4; }
void GBCPU::OP16() { LD(D, MEM[++PC]); ++PC; cycles = 8; }
void GBCPU::OP17() { RLA(); cycles = 4; }
void GBCPU::OP18() { JR(); cycles = 8; }
void GBCPU::OP19() { ADD(CASTWD(D, E)); cycles = 8; }
void GBCPU::OP1A() { LD(A, MEM[CASTWD(D, L)]); ++PC; cycles = 8; }
void GBCPU::OP1B() { DEC(D, E); ++PC; cycles = 8; }
void GBCPU::OP1C() { INCR(E); cycles = 4; }
void GBCPU::OP1D() { DECR(E); cycles = 4; }
void GBCPU::OP1E() { LD(E, MEM[++PC]); ++PC; cycles = 8; }
void GBCPU::OP1F() { RRA(); cycles = 4; }

void GBCPU::OP20() { if (ZERO_FLAG == false) JR(); else PC += 2; cycles = 8; }
void GBCPU::OP21() { LD(L, MEM[++PC]); LD(H, MEM[++PC]); ++PC; cycles = 12; }
void GBCPU::OP22() { LD(MEM[CASTWD(H, L)], A); INC(H, L); ++PC; cycles = 8; }
void GBCPU::OP23() { INC(H, L); ++PC; cycles = 8; }
void GBCPU::OP24() { INCR(H); cycles = 4; }
void GBCPU::OP25() { DECR(H); cycles = 4; }
void GBCPU::OP26() { LD(H, MEM[++PC]); ++PC; cycles = 8; }
void GBCPU::OP27() { DAA(); cycles = 4; }
void GBCPU::OP28() { if (ZERO_FLAG == true) JR(); else PC += 2; cycles = 8; }
void GBCPU::OP29() { ADD(CASTWD(H, L)); cycles = 8; }
void GBCPU::OP2A() { LD(A, MEM[CASTWD(H, L)]); INC(H, L); ++PC; cycles = 8; }
void GBCPU::OP2B() { DEC(H, L); ++PC; cycles = 8; }
void GBCPU::OP2C() { INCR(L); cycles = 4; }
void GBCPU::OP2D() { DECR(L); cycles = 4; }
void GBCPU::OP2E() { LD(L, MEM[++PC]); ++PC; cycles = 8; }
void GBCPU::OP2F() { A = ~A; SUBTRACT_FLAG = true; HALF_CARRY_FLAG = true; ++PC; cycles = 4; }

void GBCPU::OP30() { if (CARRY_FLAG == false) JR(); else PC += 2; cycles = 8; }
void GBCPU::OP31() { byte lsb = MEM[++PC]; byte msb = MEM[++PC]; LD(SP, CASTWD(msb, lsb)); ++PC; cycles = 12; }
void GBCPU::OP32() { LD(MEM[CASTWD(H, L)], A); DEC(H, L); ++PC; cycles = 8; }
void GBCPU::OP33() { ++SP; cycles = 8; }
void GBCPU::OP34() { INCR(MEM[CASTWD(H, L)]); cycles = 12; }
void GBCPU::OP35() { DECR(MEM[CASTWD(H, L)]); cycles = 12; }
void GBCPU::OP36() { LD(MEM[CASTWD(H, L)], MEM[++PC]); ++PC; cycles = 12; }
void GBCPU::OP37() { HALF_CARRY_FLAG = true; SUBTRACT_FLAG = false; HALF_CARRY_FLAG = false; ++PC; cycles = 4; }
void GBCPU::OP38() { if (CARRY_FLAG == true) JR(); else PC += 2; cycles = 8; }
void GBCPU::OP39() { ADD(SP); cycles = 8; }
void GBCPU::OP3A() { LD(A, MEM[CASTWD(H, L)]); DEC(H, L); ++PC; cycles = 8; }
void GBCPU::OP3B() { --SP; cycles = 8; }
void GBCPU::OP3C() { INCR(A); cycles = 4; }
void GBCPU::OP3D() { DECR(A); cycles = 4; }
void GBCPU::OP3E() { LD(A, MEM[++PC]); ++PC; cycles = 8; }
void GBCPU::OP3F() {
    (CARRY_FLAG == true ? CARRY_FLAG = false : HALF_CARRY_FLAG = true); ++PC; cycles = 4;
    SUBTRACT_FLAG = true; HALF_CARRY_FLAG = true;
}

void GBCPU::OP40() { LD(B, B); ++PC; cycles = 4; }
void GBCPU::OP41() { LD(B, C); ++PC; cycles = 4; }
void GBCPU::OP42() { LD(B, D); ++PC; cycles = 4; }
void GBCPU::OP43() { LD(B, E); ++PC; cycles = 4; }
void GBCPU::OP44() { LD(B, H); ++PC; cycles = 4; }
void GBCPU::OP45() { LD(B, L); ++PC; cycles = 4; }
void GBCPU::OP46() { LD(B, MEM[CASTWD(H, L)]); ++PC; cycles = 8; }
void GBCPU::OP47() { LD(B, A); ++PC; cycles = 4; }
void GBCPU::OP48() { LD(C, B); ++PC; cycles = 4; }
void GBCPU::OP49() { LD(C, C); ++PC; cycles = 4; }
void GBCPU::OP4A() { LD(C, D); ++PC; cycles = 4; }
void GBCPU::OP4B() { LD(C, E); ++PC; cycles = 4; }
void GBCPU::OP4C() { LD(C, H); ++PC; cycles = 4; }
void GBCPU::OP4D() { LD(C, L); ++PC; cycles = 4; }
void GBCPU::OP4E() { LD(C, MEM[CASTWD(H, L)]); ++PC; cycles = 8; }
void GBCPU::OP4F() { LD(C, A); ++PC; cycles = 4; }

void GBCPU::OP50() { LD(D, B); ++PC; cycles = 4; }
void GBCPU::OP51() { LD(D, C); ++PC; cycles = 4; }
void GBCPU::OP52() { LD(D, D); ++PC; cycles = 4; }
void GBCPU::OP53() { LD(D, E); ++PC; cycles = 4; }
void GBCPU::OP54() { LD(D, H); ++PC; cycles = 4; }
void GBCPU::OP55() { LD(D, L); ++PC; cycles = 4; }
void GBCPU::OP56() { LD(D, MEM[CASTWD(H, L)]); ++PC; cycles = 8; }
void GBCPU::OP57() { LD(D, A); ++PC; cycles = 4; }
void GBCPU::OP58() { LD(E, B); ++PC; cycles = 4; }
void GBCPU::OP59() { LD(E, C); ++PC; cycles = 4; }
void GBCPU::OP5A() { LD(E, D); ++PC; cycles = 4; }
void GBCPU::OP5B() { LD(E, E); ++PC; cycles = 4; }
void GBCPU::OP5C() { LD(E, H); ++PC; cycles = 4; }
void GBCPU::OP5D() { LD(E, L); ++PC; cycles = 4; }
void GBCPU::OP5E() { LD(E, MEM[CASTWD(H, L)]); cycles = 8; }
void GBCPU::OP5F() { LD(E, A); ++PC; cycles = 4; }

void GBCPU::OP60() { LD(H, B); ++PC; cycles = 4; }
void GBCPU::OP61() { LD(H, C); ++PC; cycles = 4; }
void GBCPU::OP62() { LD(H, D); ++PC; cycles = 4; }
void GBCPU::OP63() { LD(H, E); ++PC; cycles = 4; }
void GBCPU::OP64() { LD(H, H); ++PC; cycles = 4; }
void GBCPU::OP65() { LD(H, L); ++PC; cycles = 4; }
void GBCPU::OP66() { LD(H, MEM[CASTWD(H, L)]); ++PC; cycles = 8; }
void GBCPU::OP67() { LD(H, A); ++PC; cycles = 4; }
void GBCPU::OP68() { LD(L, B); ++PC; cycles = 4; }
void GBCPU::OP69() { LD(L, C); ++PC; cycles = 4; }
void GBCPU::OP6A() { LD(L, D); ++PC; cycles = 4; }
void GBCPU::OP6B() { LD(L, E); ++PC; cycles = 4; }
void GBCPU::OP6C() { LD(L, H); ++PC; cycles = 4; }
void GBCPU::OP6D() { LD(L, L); ++PC; cycles = 4; }
void GBCPU::OP6E() { LD(L, MEM[CASTWD(H, L)]); ++PC; cycles = 8; }
void GBCPU::OP6F() { LD(L, A); ++PC; cycles = 4; }

void GBCPU::OP70() { LD(MEM[CASTWD(H, L)], B); ++PC; cycles = 8; }
void GBCPU::OP71() { LD(MEM[CASTWD(H, L)], C); ++PC; cycles = 8; }
void GBCPU::OP72() { LD(MEM[CASTWD(H, L)], D); ++PC; cycles = 8; }
void GBCPU::OP73() { LD(MEM[CASTWD(H, L)], E); ++PC; cycles = 8; }
void GBCPU::OP74() { LD(MEM[CASTWD(H, L)], H); ++PC; cycles = 8; }
void GBCPU::OP75() { LD(MEM[CASTWD(H, L)], L); ++PC; cycles = 8; }
void GBCPU::OP76() { while (MEM[INTERRUPT_FLAG] == 0x00); ++PC; cycles = 4; } // HALT until an INTERRUPT occurs
void GBCPU::OP77() { LD(MEM[CASTWD(H, L)], A); ++PC; cycles = 8; }
void GBCPU::OP78() { LD(A, B); ++PC; cycles = 4; }
void GBCPU::OP79() { LD(A, C); ++PC; cycles = 4; }
void GBCPU::OP7A() { LD(A, D); ++PC; cycles = 4; }
void GBCPU::OP7B() { LD(A, E); ++PC; cycles = 4; }
void GBCPU::OP7C() { LD(A, H); ++PC; cycles = 4; }
void GBCPU::OP7D() { LD(A, L); ++PC; cycles = 4; }
void GBCPU::OP7E() { LD(A, MEM[CASTWD(H, L)]); ++PC; cycles = 8; }
void GBCPU::OP7F() { LD(A, A); ++PC; cycles = 4; }

void GBCPU::OP80() { ADD(A, B); cycles = 4; }
void GBCPU::OP81() { ADD(A, C); cycles = 4; }
void GBCPU::OP82() { ADD(A, D); cycles = 4; }
void GBCPU::OP83() { ADD(A, E); cycles = 4; }
void GBCPU::OP84() { ADD(A, H); cycles = 4; }
void GBCPU::OP85() { ADD(A, L); cycles = 4; }
void GBCPU::OP86() { ADD(A, MEM[CASTWD(H, L)]); cycles = 8; }
void GBCPU::OP87() { ADD(A, A); cycles = 4; }
void GBCPU::OP88() { ADD(A, B + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }
void GBCPU::OP89() { ADD(A, C + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }
void GBCPU::OP8A() { ADD(A, D + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }
void GBCPU::OP8B() { ADD(A, E + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }
void GBCPU::OP8C() { ADD(A, H + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }
void GBCPU::OP8D() { ADD(A, L + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }
void GBCPU::OP8E() { ADD(A, MEM[CASTWD(H, L)] + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 8; }
void GBCPU::OP8F() { ADD(A, A + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }

void GBCPU::OP90() { SUB(A, B); cycles = 4; }
void GBCPU::OP91() { SUB(A, C); cycles = 4; }
void GBCPU::OP92() { SUB(A, D); cycles = 4; }
void GBCPU::OP93() { SUB(A, E); cycles = 4; }
void GBCPU::OP94() { SUB(A, H); cycles = 4; }
void GBCPU::OP95() { SUB(A, L); cycles = 4; }
void GBCPU::OP96() { SUB(A, MEM[CASTWD(H, L)]); cycles = 8; }
void GBCPU::OP97() { SUB(A, A); cycles = 4; }
void GBCPU::OP98() { SUB(A, B + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; } // SBC A, n
void GBCPU::OP99() { SUB(A, C + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }
void GBCPU::OP9A() { SUB(A, D + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }
void GBCPU::OP9B() { SUB(A, E + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }
void GBCPU::OP9C() { SUB(A, H + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }
void GBCPU::OP9D() { SUB(A, L + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }
void GBCPU::OP9E() { SUB(A, MEM[CASTWD(H, L)] + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 8; }
void GBCPU::OP9F() { SUB(A, A + (CARRY_FLAG == true ? 0x01 : 0x00)); cycles = 4; }

void GBCPU::OPA0() { AND(A, B); cycles = 4; }
void GBCPU::OPA1() { AND(A, C); cycles = 4; }
void GBCPU::OPA2() { AND(A, D); cycles = 4; }
void GBCPU::OPA3() { AND(A, E); cycles = 4; }
void GBCPU::OPA4() { AND(A, H); cycles = 4; }
void GBCPU::OPA5() { AND(A, L); cycles = 4; }
void GBCPU::OPA6() { AND(A, MEM[CASTWD(H, L)]); cycles = 8; }
void GBCPU::OPA7() { AND(A, B); cycles = 4; }
void GBCPU::OPA8() { XOR(A, B); cycles = 4; }
void GBCPU::OPA9() { XOR(A, C); cycles = 4; }
void GBCPU::OPAA() { XOR(A, D); cycles = 4; }
void GBCPU::OPAB() { XOR(A, E); cycles = 4; }
void GBCPU::OPAC() { XOR(A, H); cycles = 4; }
void GBCPU::OPAD() { XOR(A, L); cycles = 4; }
void GBCPU::OPAE() { XOR(A, MEM[CASTWD(H, L)]); cycles = 8; }
void GBCPU::OPAF() { XOR(A, A); cycles = 4; }

void GBCPU::OPB0() { OR(A, B); cycles = 4; }
void GBCPU::OPB1() { OR(A, C); cycles = 4; }
void GBCPU::OPB2() { OR(A, D); cycles = 4; }
void GBCPU::OPB3() { OR(A, E); cycles = 4; }
void GBCPU::OPB4() { OR(A, H); cycles = 4; }
void GBCPU::OPB5() { OR(A, L); cycles = 4; }
void GBCPU::OPB6() { OR(A, MEM[CASTWD(H, L)]); cycles = 8; }
void GBCPU::OPB7() { OR(A, A); cycles = 4; }
void GBCPU::OPB8() { CP(A, B); cycles = 4; }
void GBCPU::OPB9() { CP(A, C); cycles = 4; }
void GBCPU::OPBA() { CP(A, D); cycles = 4; }
void GBCPU::OPBB() { CP(A, E); cycles = 4; }
void GBCPU::OPBC() { CP(A, H); cycles = 4; }
void GBCPU::OPBD() { CP(A, L); cycles = 4; }
void GBCPU::OPBE() { CP(A, MEM[CASTWD(H, L)]); cycles = 8; }
void GBCPU::OPBF() { CP(A, A); cycles = 4; }

void GBCPU::OPC0() { if (ZERO_FLAG == false) RET(); else ++PC; cycles = 8; }
void GBCPU::OPC1() { POP(B, C); ++PC; cycles = 12; }
void GBCPU::OPC2() { if (ZERO_FLAG == false) JP(); else PC += 3; cycles = 12; }
void GBCPU::OPC3() { JP(); cycles = 12; }
void GBCPU::OPC4() { if (ZERO_FLAG == false) CALL(); else PC += 3; cycles = 12; }
void GBCPU::OPC5() { PUSH(B, C); ++PC; cycles = 16; }
void GBCPU::OPC6() { ADD(A, MEM[++PC]); cycles = 8; }
void GBCPU::OPC7() { RST(0x00); cycles = 32; }
void GBCPU::OPC8() { if (ZERO_FLAG == true) RET(); else ++PC; cycles = 8; }
void GBCPU::OPC9() { RET(); cycles = 8; }
void GBCPU::OPCA() { if (ZERO_FLAG == true) JP(); else PC += 3; cycles = 12; }
void GBCPU::OPCB() { cout << "CB Opcode called!" << endl; /* PREFIX CB OPCODES - DO NOT USE. */ }
void GBCPU::OPCC() { if (ZERO_FLAG == true) CALL(); else PC += 3; cycles = 12; }
void GBCPU::OPCD() { CALL(); cycles = 12; } // CALL nn
void GBCPU::OPCE() { ADD(A, MEM[++PC] + (CARRY_FLAG == true)); cycles = 8; }
void GBCPU::OPCF() { RST(0x08); cycles = 32; }

void GBCPU::OPD0() { if (CARRY_FLAG == false) RET(); else ++PC; cycles = 8; }
void GBCPU::OPD1() { POP(D, E); ++PC; cycles = 12; }
void GBCPU::OPD2() { if (CARRY_FLAG == false) JP(); else PC += 3; cycles = 12; }
void GBCPU::OPD3() { /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPD4() { if (CARRY_FLAG == false) CALL(); else PC += 3; cycles = 12; }
void GBCPU::OPD5() { PUSH(D, E); ++PC; cycles = 16; }
void GBCPU::OPD6() { SUB(A, MEM[++PC]); cycles = 8; }
void GBCPU::OPD7() { RST(0x10); cycles = 32; }
void GBCPU::OPD8() { if (CARRY_FLAG == true) RET(); else ++PC; cycles = 8; }
void GBCPU::OPD9() { RET(); MEM[INTERRUPT_ENABLE] = 0x01; cycles = 8; }
void GBCPU::OPDA() { if (CARRY_FLAG == true) JP(); else PC += 3; cycles = 12; }
void GBCPU::OPDB() { /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPDC() { if (CARRY_FLAG == true) CALL(); else PC += 3; cycles = 12; }
void GBCPU::OPDD() { /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPDE() { SUB(A, MEM[++PC] + (CARRY_FLAG == true)); cycles = 8; }
void GBCPU::OPDF() { RST(0x18); cycles = 32; }

void GBCPU::OPE0() { cout << " Loading A: " << A << " onto 0xFF00 + " << MEM[0xFF00 + MEM[PC + 1]] << "!\n"; LD(MEM[0xFF00 + MEM[++PC]], A); ++PC; cycles = 12; }
void GBCPU::OPE1() { POP(H, L); ++PC; cycles = 12; }
void GBCPU::OPE2() { cout << " Loading A: + " << A << " onto 0xFF00 + " << MEM[0xFF00 + C] << "!\n"; LD(MEM[0xFF00 + C], A); ++PC; cycles = 8; }
void GBCPU::OPE3() { /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPE4() { /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPE5() { PUSH(H, L); ++PC; cycles = 16; }
void GBCPU::OPE6() { AND(A, MEM[++PC]); cycles = 8; }
void GBCPU::OPE7() { RST(0x20); cycles = 32; }
void GBCPU::OPE8() { ADDSP(); cycles = 16; }
void GBCPU::OPE9() { PC = CASTWD(H, L); cycles = 4; } // JP (HL)
void GBCPU::OPEA() { LD(MEM[CASTWD(MEM[++PC], MEM[++PC])], A); ++PC; cycles = 16; }
void GBCPU::OPEB() { /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPEC() { /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPED() { /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPEE() { XOR(A, MEM[++PC]); cycles = 8; }
void GBCPU::OPEF() { RST(0x28); cycles = 32; }

void GBCPU::OPF0() { cout << " Loading " << MEM[0xFF00 + MEM[PC + 1]] << " onto A!\n"; LD(A, MEM[0xFF00 + MEM[++PC]]); ++PC; cycles = 12; }
void GBCPU::OPF1() { byte temp = 0x00; POP(A, temp); SetF(temp); ++PC; cycles = 12; }
void GBCPU::OPF2() { cout << " Loading " << MEM[0xFF00 + C] << " onto A!\n"; LD(A, MEM[0xFF00 + C]); ++PC; cycles = 8; }
void GBCPU::OPF3() { DI_Executed = true; ++PC; cycles = 4; } // DI
void GBCPU::OPF4() { /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPF5() { PUSH(A, GetF()); ++PC; cycles = 16; }
void GBCPU::OPF6() { OR(A, MEM[++PC]); cycles = 8; }
void GBCPU::OPF7() { RST(0x30); cycles = 32; }
void GBCPU::OPF8() {
    LD(H, (SP + MEM[++PC]) >> 8); LD(L, (SP + MEM[PC]) & 0xFF); ++PC; cycles = 12;
    SUBTRACT_FLAG = false; ZERO_FLAG = false;
} 
void GBCPU::OPF9() { LD(SP, CASTWD(H, L)); ++PC; cycles = 8; }
void GBCPU::OPFA() { LD(A, MEM[CASTWD(MEM[++PC], MEM[++PC])]); ++PC; cycles = 16; }
void GBCPU::OPFB() { DI_Executed = false; ++PC; cycles = 4; } // EI
void GBCPU::OPFC() { /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPFD() { /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPFE() { CP(A, MEM[++PC]); cycles = 8; }
void GBCPU::OPFF() { RST(0x38); cycles = 32; }


/* CB Operations */

void GBCPU::CBOP00() { RLC(B); cycles = 8; }
void GBCPU::CBOP01() { RLC(C); cycles = 8; }
void GBCPU::CBOP02() { RLC(D); cycles = 8; }
void GBCPU::CBOP03() { RLC(E); cycles = 8; }
void GBCPU::CBOP04() { RLC(H); cycles = 8; }
void GBCPU::CBOP05() { RLC(L); cycles = 8; }
void GBCPU::CBOP06() { RLC(MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP07() { RLC(A); cycles = 8; }
void GBCPU::CBOP08() { RRC(B); cycles = 8; }
void GBCPU::CBOP09() { RRC(C); cycles = 8; }
void GBCPU::CBOP0A() { RRC(D); cycles = 8; }
void GBCPU::CBOP0B() { RRC(E); cycles = 8; }
void GBCPU::CBOP0C() { RRC(H); cycles = 8; }
void GBCPU::CBOP0D() { RRC(L); cycles = 8; }
void GBCPU::CBOP0E() { RRC(MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP0F() { RRC(A); cycles = 8; }

void GBCPU::CBOP10() { RL(B); cycles = 8; }
void GBCPU::CBOP11() { RL(C); cycles = 8; }
void GBCPU::CBOP12() { RL(D); cycles = 8; }
void GBCPU::CBOP13() { RL(E); cycles = 8; }
void GBCPU::CBOP14() { RL(H); cycles = 8; }
void GBCPU::CBOP15() { RL(L); cycles = 8; }
void GBCPU::CBOP16() { RL(MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP17() { RL(A); cycles = 8; }
void GBCPU::CBOP18() { RR(B); cycles = 8; }
void GBCPU::CBOP19() { RR(C); cycles = 8; }
void GBCPU::CBOP1A() { RR(D); cycles = 8; }
void GBCPU::CBOP1B() { RR(E); cycles = 8; }
void GBCPU::CBOP1C() { RR(H); cycles = 8; }
void GBCPU::CBOP1D() { RR(L); cycles = 8; }
void GBCPU::CBOP1E() { RR(MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP1F() { RR(A); cycles = 8; }

void GBCPU::CBOP20() { SLA(B); cycles = 8; }
void GBCPU::CBOP21() { SLA(C); cycles = 8; }
void GBCPU::CBOP22() { SLA(D); cycles = 8; }
void GBCPU::CBOP23() { SLA(E); cycles = 8; }
void GBCPU::CBOP24() { SLA(H); cycles = 8; }
void GBCPU::CBOP25() { SLA(L); cycles = 8; }
void GBCPU::CBOP26() { SLA(MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP27() { SLA(A); cycles = 8; }
void GBCPU::CBOP28() { SRA(B); cycles = 8; }
void GBCPU::CBOP29() { SRA(C); cycles = 8; }
void GBCPU::CBOP2A() { SRA(D); cycles = 8; }
void GBCPU::CBOP2B() { SRA(E); cycles = 8; }
void GBCPU::CBOP2C() { SRA(H); cycles = 8; }
void GBCPU::CBOP2D() { SRA(L); cycles = 8; }
void GBCPU::CBOP2E() { SRA(MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP2F() { SRA(A); cycles = 8; }

void GBCPU::CBOP30() { SWAP(B); cycles = 8; }
void GBCPU::CBOP31() { SWAP(C); cycles = 8; }
void GBCPU::CBOP32() { SWAP(D); cycles = 8; }
void GBCPU::CBOP33() { SWAP(E); cycles = 8; }
void GBCPU::CBOP34() { SWAP(H); cycles = 8; }
void GBCPU::CBOP35() { SWAP(L); cycles = 8; }
void GBCPU::CBOP36() { SWAP(MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP37() { SWAP(A); cycles = 8; }
void GBCPU::CBOP38() { SRL(B); cycles = 8; }
void GBCPU::CBOP39() { SRL(C); cycles = 8; }
void GBCPU::CBOP3A() { SRL(D); cycles = 8; }
void GBCPU::CBOP3B() { SRL(E); cycles = 8; }
void GBCPU::CBOP3C() { SRL(H); cycles = 8; }
void GBCPU::CBOP3D() { SRL(L); cycles = 8; }
void GBCPU::CBOP3E() { SRL(MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP3F() { SRL(A); cycles = 8; }

void GBCPU::CBOP40() { BIT(0, B); cycles = 8; }
void GBCPU::CBOP41() { BIT(0, C); cycles = 8; }
void GBCPU::CBOP42() { BIT(0, D); cycles = 8; }
void GBCPU::CBOP43() { BIT(0, E); cycles = 8; }
void GBCPU::CBOP44() { BIT(0, H); cycles = 8; }
void GBCPU::CBOP45() { BIT(0, L); cycles = 8; }
void GBCPU::CBOP46() { BIT(0, MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP47() { BIT(0, A); cycles = 8; }
void GBCPU::CBOP48() { BIT(1, B); cycles = 8; }
void GBCPU::CBOP49() { BIT(1, C); cycles = 8; }
void GBCPU::CBOP4A() { BIT(1, D); cycles = 8; }
void GBCPU::CBOP4B() { BIT(1, E); cycles = 8; }
void GBCPU::CBOP4C() { BIT(1, H); cycles = 8; }
void GBCPU::CBOP4D() { BIT(1, L); cycles = 8; }
void GBCPU::CBOP4E() { BIT(1, MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP4F() { BIT(1, A); cycles = 8; }

void GBCPU::CBOP50() { BIT(2, B); cycles = 8; }
void GBCPU::CBOP51() { BIT(2, C); cycles = 8; }
void GBCPU::CBOP52() { BIT(2, D); cycles = 8; }
void GBCPU::CBOP53() { BIT(2, E); cycles = 8; }
void GBCPU::CBOP54() { BIT(2, H); cycles = 8; }
void GBCPU::CBOP55() { BIT(2, L); cycles = 8; }
void GBCPU::CBOP56() { BIT(2, MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP57() { BIT(2, A); cycles = 8; }
void GBCPU::CBOP58() { BIT(3, B); cycles = 8; }
void GBCPU::CBOP59() { BIT(3, C); cycles = 8; }
void GBCPU::CBOP5A() { BIT(3, D); cycles = 8; }
void GBCPU::CBOP5B() { BIT(3, E); cycles = 8; }
void GBCPU::CBOP5C() { BIT(3, H); cycles = 8; }
void GBCPU::CBOP5D() { BIT(3, L); cycles = 8; }
void GBCPU::CBOP5E() { BIT(3, MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP5F() { BIT(3, A); cycles = 8; }

void GBCPU::CBOP60() { BIT(4, B); cycles = 8; }
void GBCPU::CBOP61() { BIT(4, C); cycles = 8; }
void GBCPU::CBOP62() { BIT(4, D); cycles = 8; }
void GBCPU::CBOP63() { BIT(4, E); cycles = 8; }
void GBCPU::CBOP64() { BIT(4, H); cycles = 8; }
void GBCPU::CBOP65() { BIT(4, L); cycles = 8; }
void GBCPU::CBOP66() { BIT(4, MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP67() { BIT(4, A); cycles = 8; }
void GBCPU::CBOP68() { BIT(5, B); cycles = 8; }
void GBCPU::CBOP69() { BIT(5, C); cycles = 8; }
void GBCPU::CBOP6A() { BIT(5, D); cycles = 8; }
void GBCPU::CBOP6B() { BIT(5, E); cycles = 8; }
void GBCPU::CBOP6C() { BIT(5, H); cycles = 8; }
void GBCPU::CBOP6D() { BIT(5, L); cycles = 8; }
void GBCPU::CBOP6E() { BIT(5, MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP6F() { BIT(5, A); cycles = 8; }

void GBCPU::CBOP70() { BIT(6, B); cycles = 8; }
void GBCPU::CBOP71() { BIT(6, C); cycles = 8; }
void GBCPU::CBOP72() { BIT(6, D); cycles = 8; }
void GBCPU::CBOP73() { BIT(6, E); cycles = 8; }
void GBCPU::CBOP74() { BIT(6, H); cycles = 8; }
void GBCPU::CBOP75() { BIT(6, L); cycles = 8; }
void GBCPU::CBOP76() { BIT(6, MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP77() { BIT(6, A); cycles = 8; }
void GBCPU::CBOP78() { BIT(7, B); cycles = 8; }
void GBCPU::CBOP79() { BIT(7, C); cycles = 8; }
void GBCPU::CBOP7A() { BIT(7, D); cycles = 8; }
void GBCPU::CBOP7B() { BIT(7, E); cycles = 8; }
void GBCPU::CBOP7C() { BIT(7, H); cycles = 8; }
void GBCPU::CBOP7D() { BIT(7, L); cycles = 8; }
void GBCPU::CBOP7E() { BIT(7, MEM[CASTWD(H, L)]); cycles = 16; }
void GBCPU::CBOP7F() { BIT(7, A); cycles = 8; }

// RES
void GBCPU::CBOP80() { CLRBIT(B, 0); ++PC; cycles = 8; }
void GBCPU::CBOP81() { CLRBIT(C, 0); ++PC; cycles = 8; }
void GBCPU::CBOP82() { CLRBIT(D, 0); ++PC; cycles = 8; }
void GBCPU::CBOP83() { CLRBIT(E, 0); ++PC; cycles = 8; }
void GBCPU::CBOP84() { CLRBIT(H, 0); ++PC; cycles = 8; }
void GBCPU::CBOP85() { CLRBIT(L, 0); ++PC; cycles = 8; }
void GBCPU::CBOP86() { CLRBIT(MEM[CASTWD(H, L)], 0); ++PC; cycles = 8; }
void GBCPU::CBOP87() { CLRBIT(A, 1); ++PC; cycles = 8; }
void GBCPU::CBOP88() { CLRBIT(B, 1); ++PC; cycles = 8; }
void GBCPU::CBOP89() { CLRBIT(C, 1); ++PC; cycles = 8; }
void GBCPU::CBOP8A() { CLRBIT(D, 1); ++PC; cycles = 8; }
void GBCPU::CBOP8B() { CLRBIT(E, 1); ++PC; cycles = 8; }
void GBCPU::CBOP8C() { CLRBIT(H, 1); ++PC; cycles = 8; }
void GBCPU::CBOP8D() { CLRBIT(L, 1); ++PC; cycles = 8; }
void GBCPU::CBOP8E() { CLRBIT(MEM[CASTWD(H, L)], 1); ++PC; cycles = 8; }
void GBCPU::CBOP8F() { CLRBIT(A, 1); ++PC; cycles = 8; }

void GBCPU::CBOP90() { CLRBIT(B, 2); ++PC; cycles = 8; }
void GBCPU::CBOP91() { CLRBIT(C, 2); ++PC; cycles = 8; }
void GBCPU::CBOP92() { CLRBIT(D, 2); ++PC; cycles = 8; }
void GBCPU::CBOP93() { CLRBIT(E, 2); ++PC; cycles = 8; }
void GBCPU::CBOP94() { CLRBIT(H, 2); ++PC; cycles = 8; }
void GBCPU::CBOP95() { CLRBIT(L, 2); ++PC; cycles = 8; }
void GBCPU::CBOP96() { CLRBIT(MEM[CASTWD(H, L)], 2); ++PC; cycles = 8; }
void GBCPU::CBOP97() { CLRBIT(A, 2); ++PC; cycles = 8; }
void GBCPU::CBOP98() { CLRBIT(B, 3); ++PC; cycles = 8; }
void GBCPU::CBOP99() { CLRBIT(C, 3); ++PC; cycles = 8; }
void GBCPU::CBOP9A() { CLRBIT(D, 3); ++PC; cycles = 8; }
void GBCPU::CBOP9B() { CLRBIT(E, 3); ++PC; cycles = 8; }
void GBCPU::CBOP9C() { CLRBIT(H, 3); ++PC; cycles = 8; }
void GBCPU::CBOP9D() { CLRBIT(L, 3); ++PC; cycles = 8; }
void GBCPU::CBOP9E() { CLRBIT(MEM[CASTWD(H, L)], 3); ++PC; cycles = 8; }
void GBCPU::CBOP9F() { CLRBIT(A, 3); ++PC; cycles = 8; }

void GBCPU::CBOPA0() { CLRBIT(B, 4); ++PC; cycles = 8; }
void GBCPU::CBOPA1() { CLRBIT(C, 4); ++PC; cycles = 8; }
void GBCPU::CBOPA2() { CLRBIT(D, 4); ++PC; cycles = 8; }
void GBCPU::CBOPA3() { CLRBIT(E, 4); ++PC; cycles = 8; }
void GBCPU::CBOPA4() { CLRBIT(H, 4); ++PC; cycles = 8; }
void GBCPU::CBOPA5() { CLRBIT(L, 4); ++PC; cycles = 8; }
void GBCPU::CBOPA6() { CLRBIT(MEM[CASTWD(H, L)], 4); ++PC; cycles = 8; }
void GBCPU::CBOPA7() { CLRBIT(A, 4); ++PC; cycles = 8; }
void GBCPU::CBOPA8() { CLRBIT(B, 5); ++PC; cycles = 8; }
void GBCPU::CBOPA9() { CLRBIT(C, 5); ++PC; cycles = 8; }
void GBCPU::CBOPAA() { CLRBIT(D, 5); ++PC; cycles = 8; }
void GBCPU::CBOPAB() { CLRBIT(E, 5); ++PC; cycles = 8; }
void GBCPU::CBOPAC() { CLRBIT(H, 5); ++PC; cycles = 8; }
void GBCPU::CBOPAD() { CLRBIT(L, 5); ++PC; cycles = 8; }
void GBCPU::CBOPAE() { CLRBIT(MEM[CASTWD(H, L)], 5); ++PC; cycles = 8; }
void GBCPU::CBOPAF() { CLRBIT(A, 5); ++PC; cycles = 8; }

void GBCPU::CBOPB0() { CLRBIT(B, 6); ++PC; cycles = 8; }
void GBCPU::CBOPB1() { CLRBIT(C, 6); ++PC; cycles = 8; }
void GBCPU::CBOPB2() { CLRBIT(D, 6); ++PC; cycles = 8; }
void GBCPU::CBOPB3() { CLRBIT(E, 6); ++PC; cycles = 8; }
void GBCPU::CBOPB4() { CLRBIT(H, 6); ++PC; cycles = 8; }
void GBCPU::CBOPB5() { CLRBIT(L, 6); ++PC; cycles = 8; }
void GBCPU::CBOPB6() { CLRBIT(MEM[CASTWD(H, L)], 6); ++PC; cycles = 8; }
void GBCPU::CBOPB7() { CLRBIT(A, 6); ++PC; cycles = 8; }
void GBCPU::CBOPB8() { CLRBIT(B, 7); ++PC; cycles = 8; }
void GBCPU::CBOPB9() { CLRBIT(C, 7); ++PC; cycles = 8; }
void GBCPU::CBOPBA() { CLRBIT(D, 7); ++PC; cycles = 8; }
void GBCPU::CBOPBB() { CLRBIT(E, 7); ++PC; cycles = 8; }
void GBCPU::CBOPBC() { CLRBIT(H, 7); ++PC; cycles = 8; }
void GBCPU::CBOPBD() { CLRBIT(L, 7); ++PC; cycles = 8; }
void GBCPU::CBOPBE() { CLRBIT(MEM[CASTWD(H, L)], 7); ++PC; cycles = 8; }
void GBCPU::CBOPBF() { CLRBIT(A, 7); ++PC; cycles = 8; }

// SET
void GBCPU::CBOPC0() { SETBIT(B, 0); ++PC; cycles = 8; }
void GBCPU::CBOPC1() { SETBIT(C, 0); ++PC; cycles = 8; }
void GBCPU::CBOPC2() { SETBIT(D, 0); ++PC; cycles = 8; }
void GBCPU::CBOPC3() { SETBIT(E, 0); ++PC; cycles = 8; }
void GBCPU::CBOPC4() { SETBIT(H, 0); ++PC; cycles = 8; }
void GBCPU::CBOPC5() { SETBIT(L, 0); ++PC; cycles = 8; }
void GBCPU::CBOPC6() { SETBIT(MEM[CASTWD(H, L)], 0); ++PC; cycles = 8; }
void GBCPU::CBOPC7() { SETBIT(A, 1); ++PC; cycles = 8; }
void GBCPU::CBOPC8() { SETBIT(B, 1); ++PC; cycles = 8; }
void GBCPU::CBOPC9() { SETBIT(C, 1); ++PC; cycles = 8; }
void GBCPU::CBOPCA() { SETBIT(D, 1); ++PC; cycles = 8; }
void GBCPU::CBOPCB() { SETBIT(E, 1); ++PC; cycles = 8; }
void GBCPU::CBOPCC() { SETBIT(H, 1); ++PC; cycles = 8; }
void GBCPU::CBOPCD() { SETBIT(L, 1); ++PC; cycles = 8; }
void GBCPU::CBOPCE() { SETBIT(MEM[CASTWD(H, L)], 1); ++PC; cycles = 8; }
void GBCPU::CBOPCF() { SETBIT(A, 1); ++PC; cycles = 8; }

void GBCPU::CBOPD0() { SETBIT(B, 2); ++PC; cycles = 8; }
void GBCPU::CBOPD1() { SETBIT(C, 2); ++PC; cycles = 8; }
void GBCPU::CBOPD2() { SETBIT(D, 2); ++PC; cycles = 8; }
void GBCPU::CBOPD3() { SETBIT(E, 2); ++PC; cycles = 8; }
void GBCPU::CBOPD4() { SETBIT(H, 2); ++PC; cycles = 8; }
void GBCPU::CBOPD5() { SETBIT(L, 2); ++PC; cycles = 8; }
void GBCPU::CBOPD6() { SETBIT(MEM[CASTWD(H, L)], 2); ++PC; cycles = 8; }
void GBCPU::CBOPD7() { SETBIT(A, 2); ++PC; cycles = 8; }
void GBCPU::CBOPD8() { SETBIT(B, 3); ++PC; cycles = 8; }
void GBCPU::CBOPD9() { SETBIT(C, 3); ++PC; cycles = 8; }
void GBCPU::CBOPDA() { SETBIT(D, 3); ++PC; cycles = 8; }
void GBCPU::CBOPDB() { SETBIT(E, 3); ++PC; cycles = 8; }
void GBCPU::CBOPDC() { SETBIT(H, 3); ++PC; cycles = 8; }
void GBCPU::CBOPDD() { SETBIT(L, 3); ++PC; cycles = 8; }
void GBCPU::CBOPDE() { SETBIT(MEM[CASTWD(H, L)], 3); ++PC; cycles = 8; }
void GBCPU::CBOPDF() { SETBIT(A, 3); ++PC; cycles = 8; }

void GBCPU::CBOPE0() { SETBIT(B, 4); ++PC; cycles = 8; }
void GBCPU::CBOPE1() { SETBIT(C, 4); ++PC; cycles = 8; }
void GBCPU::CBOPE2() { SETBIT(D, 4); ++PC; cycles = 8; }
void GBCPU::CBOPE3() { SETBIT(E, 4); ++PC; cycles = 8; }
void GBCPU::CBOPE4() { SETBIT(H, 4); ++PC; cycles = 8; }
void GBCPU::CBOPE5() { SETBIT(L, 4); ++PC; cycles = 8; }
void GBCPU::CBOPE6() { SETBIT(MEM[CASTWD(H, L)], 4); ++PC; cycles = 8; }
void GBCPU::CBOPE7() { SETBIT(A, 4); ++PC; cycles = 8; }
void GBCPU::CBOPE8() { SETBIT(B, 5); ++PC; cycles = 8; }
void GBCPU::CBOPE9() { SETBIT(C, 5); ++PC; cycles = 8; }
void GBCPU::CBOPEA() { SETBIT(D, 5); ++PC; cycles = 8; }
void GBCPU::CBOPEB() { SETBIT(E, 5); ++PC; cycles = 8; }
void GBCPU::CBOPEC() { SETBIT(H, 5); ++PC; cycles = 8; }
void GBCPU::CBOPED() { SETBIT(L, 5); ++PC; cycles = 8; }
void GBCPU::CBOPEE() { SETBIT(MEM[CASTWD(H, L)], 5); ++PC; cycles = 8; }
void GBCPU::CBOPEF() { SETBIT(A, 5); ++PC; cycles = 8; }

void GBCPU::CBOPF0() { SETBIT(B, 6); ++PC; cycles = 8; }
void GBCPU::CBOPF1() { SETBIT(C, 6); ++PC; cycles = 8; }
void GBCPU::CBOPF2() { SETBIT(D, 6); ++PC; cycles = 8; }
void GBCPU::CBOPF3() { SETBIT(E, 6); ++PC; cycles = 8; }
void GBCPU::CBOPF4() { SETBIT(H, 6); ++PC; cycles = 8; }
void GBCPU::CBOPF5() { SETBIT(L, 6); ++PC; cycles = 8; }
void GBCPU::CBOPF6() { SETBIT(MEM[CASTWD(H, L)], 6); ++PC; cycles = 8; }
void GBCPU::CBOPF7() { SETBIT(A, 6); ++PC; cycles = 8; }
void GBCPU::CBOPF8() { SETBIT(B, 7); ++PC; cycles = 8; }
void GBCPU::CBOPF9() { SETBIT(C, 7); ++PC; cycles = 8; }
void GBCPU::CBOPFA() { SETBIT(D, 7); ++PC; cycles = 8; }
void GBCPU::CBOPFB() { SETBIT(E, 7); ++PC; cycles = 8; }
void GBCPU::CBOPFC() { SETBIT(H, 7); ++PC; cycles = 8; }
void GBCPU::CBOPFD() { SETBIT(L, 7); ++PC; cycles = 8; }
void GBCPU::CBOPFE() { SETBIT(MEM[CASTWD(H, L)], 7); ++PC; cycles = 8; }
void GBCPU::CBOPFF() { SETBIT(A, 7); ++PC; cycles = 8; }