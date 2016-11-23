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
}

// ADD HL, n
inline void GBCPU::ADD(word arg)
{
    word HL = GetHL();

    // Reset N flag
    SUBTRACT_FLAG = false;

    // Detect half carry if carry from bit 11
    if ( ((HL & 0xFFF) + (arg & 0xFFF)) & 0x1000 )
        HALF_CARRY_FLAG = true;
    else
        HALF_CARRY_FLAG = false;

    // Detects carry if carry from bit 15
    if (((HL & 0xFFFF) + (arg & 0xFFFF)) & 0x10000)
        CARRY_FLAG = true;
    else
        CARRY_FLAG = false;

    // Update HL register
    HL += arg;
    SetHL(HL);
}

// ADD SP, n
inline void GBCPU::ADDSP()
{
    signed_byte arg = signed_byte(readImmByte());

    // Reset Z, N flag
    SUBTRACT_FLAG = false;
    ZERO_FLAG = false;

    // Detect half carry
    if (((SP & 0x0F) + (arg & 0x0F) ) & 0x10)
        HALF_CARRY_FLAG = true;
    else
        HALF_CARRY_FLAG = false;

    // Detects carry
    if (((SP & 0xFF) + (arg & 0xFF)) & 0x100)
        CARRY_FLAG = true;
    else
        CARRY_FLAG = false;

    SP += arg;
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
}

inline void GBCPU::INCR(byte & reg)
{
    // Reset N flag
    SUBTRACT_FLAG = false;

    // Detect half carry
    if (((reg & 0x0F) + 1) > 0x0F)
        HALF_CARRY_FLAG = true;
    else
        HALF_CARRY_FLAG = false;

    ++reg;

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);
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
}

// Decimal Adjust Register A. This Opcode is still a mystery to me and has been taken from forums, sites, etc.
inline void GBCPU::DAA()
{
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

    // Set Carry 
    if ((temp & 0x100) == 0x100)
        CARRY_FLAG = true;
    else
        CARRY_FLAG = false;

    // Detect Zero
    ((A & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);

    // Clear Half-Carry
    CARRY_FLAG = false;
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
}

// Rotate A right
inline void GBCPU::RRCA()
{
    // Reset N and H flag
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;

    // If LSB is high, set carry flag
    if ((A & 0x01))
    {
        CARRY_FLAG = true;
        A = ((A >> 1) & 0x7F) | 0x80;
    }
    else
    {
        CARRY_FLAG = false;
        A = ((A >> 1) & 0x7F);
    }

    // Detect zero
    ((A & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);
}

// Rotate n right
inline void GBCPU::RRC(byte & reg)
{
    // Reset N and H flag
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;

    // If LSB is high, set carry flag
    if ((reg & 0x01))
    {
        CARRY_FLAG = true;
        reg = ((reg >> 1) & 0x7F) | 0x80;
    }
    else
    {
        CARRY_FLAG = false;
        reg = ((reg >> 1) & 0x7F);
    }

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);
}

// Rotate A right through carry
inline void GBCPU::RRA()
{
    // Reset N and H flag
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;
    ZERO_FLAG = false;

    // If LSB is high, set carry flag
    if ((A & 0x01))
    {
        A = ((A >> 1) & 0x7F);
        A |= ((CARRY_FLAG == true ? 0x01 : 0x00) << 7);
        CARRY_FLAG = true;
    }
    else
    {
        A = ((A >> 1) & 0x7F);
        A |= ((CARRY_FLAG == true ? 0x01 : 0x00) << 7);
        CARRY_FLAG = false;
    }

    // Detect zero
    //((A & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);
}

// Rotate n right through carry
inline void GBCPU::RR(byte & reg)
{
    // Reset N and H flag
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;
    ZERO_FLAG = false;

    // If LSB is high, set carry flag
    if ((reg & 0x01))
    {
        reg = ((reg >> 1) & 0x7F);
        reg |= ((CARRY_FLAG == true ? 0x01 : 0x00) << 7);
        CARRY_FLAG = true;
    }
    else
    {
        reg = ((reg >> 1) & 0x7F);
        reg |= ((CARRY_FLAG == true ? 0x01 : 0x00) << 7);
        CARRY_FLAG = false;
    }

    // Detect zero
    //((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);
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
}

// SRL n
inline void GBCPU::SRL(byte & reg)
{
    // reset N and H
    SUBTRACT_FLAG = false;
    HALF_CARRY_FLAG = false;

    // Set Carry flag if the lsb is high
    if ((reg & 0x01))
    {
        CARRY_FLAG = true;
    }
    else
    {
        CARRY_FLAG = false;
    }


    reg = (reg & 0xFF) >> 1;

    // Detect zero
    ((reg & 0xFF) == 0x00 ? ZERO_FLAG = true : ZERO_FLAG = false);
}

// BIT b,r
inline void GBCPU::BIT(byte bit, byte & reg)
{
    // Reset N flag
    SUBTRACT_FLAG = false;

    // Set H flag
    HALF_CARRY_FLAG = true;

    // Set Z flag if bit n of reg is 0
    (((reg & (1 << bit)) == 0x00) ? ZERO_FLAG = true : ZERO_FLAG = false);
}

// JP cc
inline void GBCPU::JP()
{
    PC = readImmWord();
    //cout << "Jumping to: " << PC << "or ";
    //printf("%X!\n", PC);
}

// JR cc
inline void GBCPU::JR()
{
    // Get the immediate byte
    signed_byte n = readImmByte();

    // Add the immediate byte (n) to the current PC (after this instruction so we add 2)
    PC += n + 2;
}

// CALL cc
inline void GBCPU::CALL()
{
    // Push the address of the next instruction (PC + 1) onto the stack
    PUSH(((PC + 3) >> 8), ((PC + 3) & 0x00FF));

    // Get the immediate instruction
    PC = readImmWord();
}

// RESET
inline void GBCPU::RST(byte n)
{
    // Push the address of the current instruction onto the stack
    PUSH(((PC+1) >> 8), ((PC+1) & 0x00FF));

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

// TODO: CHECK TO SEE IF ALL WORD LOADS GET THE LSB FIRST
// TODO: Add comments for each opcode about what the instruction does
void GBCPU::OP00() { /*cout << "NOP" << endl;*/ ++PC; cycles = 4; }                 // NOP
void GBCPU::OP01() { SetBC( readImmWord() ); PC += 3; cycles = 12; }            // LD BC, ##
void GBCPU::OP02() { writeByte(A, GetBC()); ++PC; cycles = 8; }                 // LD (BC), A
void GBCPU::OP03() { word temp = GetBC() + 1; SetBC(temp); ++PC; cycles = 8; }  // INC BC
void GBCPU::OP04() { INCR(B); ++PC; cycles = 4; }                               // INC B
void GBCPU::OP05() { DECR(B); ++PC; cycles = 4; }                               // DEC B
void GBCPU::OP06() { B = readImmByte(); PC += 2; cycles = 8; }                  // LD B, #
void GBCPU::OP07() { RLCA(); ++PC; cycles = 4; }                                // RLCA
void GBCPU::OP08() { writeWord(SP, readImmWord()); PC += 3; cycles = 20; }      // LD (SP), ##
void GBCPU::OP09() { ADD(GetBC()); ++PC; cycles = 8; }                          // ADD HL, BC
void GBCPU::OP0A() { A = readByte(GetBC()); ++PC; cycles = 8; }                 // LD A, (BC)
void GBCPU::OP0B() { word temp = GetBC() - 1; SetBC(temp);  ++PC; cycles = 8; } // DEC BC
void GBCPU::OP0C() { INCR(C); ++PC; cycles = 4; }                               // INC C
void GBCPU::OP0D() { DECR(C); ++PC; cycles = 4; }                               // DEC C
void GBCPU::OP0E() { C = readImmByte(); PC += 2; cycles = 8; }                  // LD C, #
void GBCPU::OP0F() { RRCA(); ++PC; cycles = 4; }                                // RRCA
                                                                               
void GBCPU::OP10() { ++PC; cycles = 4; cout << "HALT!" << endl;                 /* TODO: HALT CPU and LCD display until button pressed */ } // STOP
void GBCPU::OP11() { SetDE( readImmWord() ); PC += 3; cycles = 12; }            // LD DE, ##
void GBCPU::OP12() { writeByte(A, GetDE()); ++PC; cycles = 8; }                 // LD (DE), A
void GBCPU::OP13() { word temp = GetDE() + 1; SetDE(temp); ++PC; cycles = 8; }  // INC DE
void GBCPU::OP14() { INCR(D); ++PC; cycles = 4; }                               // INC D
void GBCPU::OP15() { DECR(D); ++PC; cycles = 4; }                               // DEC D
void GBCPU::OP16() { D = readImmByte(); PC += 2; cycles = 8; }                  // LD A, #
void GBCPU::OP17() { RLA(); ++PC; cycles = 4; }                                 // RLA
void GBCPU::OP18() { JR(); cycles = 8; }                                        // JR
void GBCPU::OP19() { ADD(GetDE()); ++PC; cycles = 8; }                          // ADD HL, DE
void GBCPU::OP1A() { A = readByte(GetDE()); ++PC; cycles = 8; }                 // LD A, (DE)
void GBCPU::OP1B() { word temp = GetDE() - 1; SetDE(temp); ++PC; cycles = 8; }  // DEC DE
void GBCPU::OP1C() { INCR(E); ++PC; cycles = 4; }                               // INC E
void GBCPU::OP1D() { DECR(E); ++PC; cycles = 4; }                               // DEC E
void GBCPU::OP1E() { E = readImmByte(); PC += 2; cycles = 8; }                  // LD E, #
void GBCPU::OP1F() { RRA(); ++PC; cycles = 4; }                                 // RRA

void GBCPU::OP20() { if (ZERO_FLAG == false) JR(); else PC += 2; cycles = 8; }  // JR NZ
void GBCPU::OP21() { SetHL(readImmWord()); PC += 3; cycles = 12; }              // LD HL, ##
void GBCPU::OP22() { writeByte(A, GetHL()); INC(H, L); ++PC; cycles = 8; }      // LD (HL++), A
void GBCPU::OP23() { word temp = GetHL() + 1; SetHL(temp); ++PC; cycles = 8; }  // INC HL
void GBCPU::OP24() { INCR(H); ++PC; cycles = 4; }                               // INC H
void GBCPU::OP25() { DECR(H); ++PC; cycles = 4; }                               // DEC H
void GBCPU::OP26() { H = readImmByte(); PC += 2; cycles = 8; }                  // LD H, #
void GBCPU::OP27() { DAA(); ++PC; cycles = 4; }                                 // DAA
void GBCPU::OP28() { if (ZERO_FLAG == true) JR(); else PC += 2; cycles = 8; }   // JR z
void GBCPU::OP29() { ADD(GetHL()); ++PC; cycles = 8; }                          // ADD HL, HL
void GBCPU::OP2A() { A = readByte(GetHL()); INC(H, L); ++PC; cycles = 8; }      // LD A, (HL++)
void GBCPU::OP2B() { word temp = GetHL() - 1; SetHL(temp); ++PC; cycles = 8; }  // DEC HL
void GBCPU::OP2C() { INCR(L); ++PC; cycles = 4; }                               // INC L
void GBCPU::OP2D() { DECR(L); ++PC; cycles = 4; }                               // DEC L
void GBCPU::OP2E() { L = readImmByte(); PC += 2; cycles = 8; }                  // LD L, #
void GBCPU::OP2F() { A = ~A; SUBTRACT_FLAG = true;                              // CPL (flip all bits)
                     HALF_CARRY_FLAG = true; ++PC; cycles = 4; }

void GBCPU::OP30() { if (CARRY_FLAG == false) JR(); else PC += 2; cycles = 8; }                                  // JR nc
void GBCPU::OP31() { SP = readImmWord(); PC += 3; cycles = 12; }                                                 // LD SP, ##
void GBCPU::OP32() { writeByte(A, GetHL()); DEC(H, L); ++PC; cycles = 8; }                                       // LD (HL--), A
void GBCPU::OP33() { ++SP; ++PC; cycles = 8; }                                                                   // INC SP
void GBCPU::OP34() { byte t = readByte(GetHL()); INCR(t); writeByte(t, GetHL()); ++PC; cycles = 12; }            // INC (HL)
void GBCPU::OP35() { byte t = readByte(GetHL()); DECR(t); writeByte(t, GetHL()); ++PC; cycles = 12; }            // DEC (HL)
void GBCPU::OP36() { writeByte(readImmByte(), GetHL()); PC += 2; cycles = 12; }                                  // LD (HL), #
void GBCPU::OP37() { HALF_CARRY_FLAG = true; SUBTRACT_FLAG = false; HALF_CARRY_FLAG = false; ++PC; cycles = 4; } // SCF
void GBCPU::OP38() { if (CARRY_FLAG == true) JR(); else PC += 2; cycles = 8; }                                   // JR, c
void GBCPU::OP39() { ADD(SP); ++PC; cycles = 8; }                                                                // ADD HL, SP 
void GBCPU::OP3A() { A = readByte(GetHL()); DEC(H, L); ++PC; cycles = 8; }                                       // LD A, (HL--)
void GBCPU::OP3B() { --SP; ++PC; cycles = 8; }                                                                   // --SP
void GBCPU::OP3C() { INCR(A); ++PC; cycles = 4; }                                                                // INC A
void GBCPU::OP3D() { DECR(A); ++PC; cycles = 4; }                                                                // DEC A
void GBCPU::OP3E() { A = readImmByte(); PC += 2; cycles = 8; }                                                   // LD A, #
void GBCPU::OP3F() { (CARRY_FLAG == true ? CARRY_FLAG = false : CARRY_FLAG = true); SUBTRACT_FLAG = false;       // CCF
                     HALF_CARRY_FLAG = false; ++PC; cycles = 4; }

void GBCPU::OP40() { B = B; ++PC; cycles = 4; }                 // LD B, B
void GBCPU::OP41() { B = C; ++PC; cycles = 4; }                 // LD B, C
void GBCPU::OP42() { B = D; ++PC; cycles = 4; }                 // LD B, D
void GBCPU::OP43() { B = E; ++PC; cycles = 4; }                 // LD B, E
void GBCPU::OP44() { B = H; ++PC; cycles = 4; }                 // LD B, H
void GBCPU::OP45() { B = L; ++PC; cycles = 4; }                 // LD B, L
void GBCPU::OP46() { B = readByte(GetHL()); ++PC; cycles = 8; } // LD B, (HL)
void GBCPU::OP47() { B = A; ++PC; cycles = 4; }                 // LD B, A
void GBCPU::OP48() { C = B; ++PC; cycles = 4; }                 // LD C, B
void GBCPU::OP49() { C = C; ++PC; cycles = 4; }                 // LD C, C
void GBCPU::OP4A() { C = D; ++PC; cycles = 4; }                 // LD C, D
void GBCPU::OP4B() { C = E; ++PC; cycles = 4; }                 // LD C, E
void GBCPU::OP4C() { C = H; ++PC; cycles = 4; }                 // LD C, H
void GBCPU::OP4D() { C = L; ++PC; cycles = 4; }                 // LD C, L
void GBCPU::OP4E() { C = readByte(GetHL()); ++PC; cycles = 8; } // LD C, (HL)
void GBCPU::OP4F() { C = A; ++PC; cycles = 4; }                 // LD C, A

void GBCPU::OP50() { D = B; ++PC; cycles = 4; }                 // LD D, B
void GBCPU::OP51() { D = C; ++PC; cycles = 4; }                 // LD D, C
void GBCPU::OP52() { D = D; ++PC; cycles = 4; }                 // LD D, D
void GBCPU::OP53() { D = E; ++PC; cycles = 4; }                 // LD D, E
void GBCPU::OP54() { D = H; ++PC; cycles = 4; }                 // LD D, H
void GBCPU::OP55() { D = L; ++PC; cycles = 4; }                 // LD D, L
void GBCPU::OP56() { D = readByte(GetHL()); ++PC; cycles = 8; } // LD D, (HL)
void GBCPU::OP57() { D = A; ++PC; cycles = 4; }                 // LD D, A
void GBCPU::OP58() { E = B; ++PC; cycles = 4; }                 // LD E, B
void GBCPU::OP59() { E = C; ++PC; cycles = 4; }                 // LD E, C
void GBCPU::OP5A() { E = D; ++PC; cycles = 4; }                 // LD E, D
void GBCPU::OP5B() { E = E; ++PC; cycles = 4; }                 // LD E, E
void GBCPU::OP5C() { E = H; ++PC; cycles = 4; }                 // LD E, H
void GBCPU::OP5D() { E = L; ++PC; cycles = 4; }                 // LD E, L
void GBCPU::OP5E() { E = readByte(GetHL()); ++PC; cycles = 8; } // LD E, (HL)
void GBCPU::OP5F() { E = A; ++PC; cycles = 4; }                 // LD E, A

void GBCPU::OP60() { H = B; ++PC; cycles = 4; }                 // LD H, B
void GBCPU::OP61() { H = C; ++PC; cycles = 4; }                 // LD H, C
void GBCPU::OP62() { H = D; ++PC; cycles = 4; }                 // LD H, D
void GBCPU::OP63() { H = E; ++PC; cycles = 4; }                 // LD H, E
void GBCPU::OP64() { H = H; ++PC; cycles = 4; }                 // LD H, H
void GBCPU::OP65() { H = L; ++PC; cycles = 4; }                 // LD H, L
void GBCPU::OP66() { H = readByte(GetHL()); ++PC; cycles = 8; } // LD H, (HL)
void GBCPU::OP67() { H = A; ++PC; cycles = 4; }                 // LD H, A
void GBCPU::OP68() { L = B; ++PC; cycles = 4; }                 // LD L, B
void GBCPU::OP69() { L = C; ++PC; cycles = 4; }                 // LD L, C
void GBCPU::OP6A() { L = D; ++PC; cycles = 4; }                 // LD L, D
void GBCPU::OP6B() { L = E; ++PC; cycles = 4; }                 // LD L, E
void GBCPU::OP6C() { L = H; ++PC; cycles = 4; }                 // LD L, H
void GBCPU::OP6D() { L = L; ++PC; cycles = 4; }                 // LD L, L
void GBCPU::OP6E() { L = readByte(GetHL()); ++PC; cycles = 8; } // LD L, (HL)
void GBCPU::OP6F() { /*cout << "START FAST CHECKSUM!" << endl;*/ L = A; ++PC; cycles = 4; }                 // LD L, A


void GBCPU::OP70() { writeByte(B, GetHL()); ++PC; cycles = 8; }               // LD (HL), B
void GBCPU::OP71() { writeByte(C, GetHL()); ++PC; cycles = 8; }               // LD (HL), C
void GBCPU::OP72() { writeByte(D, GetHL()); ++PC; cycles = 8; }               // LD (HL), D
void GBCPU::OP73() { writeByte(E, GetHL()); ++PC; cycles = 8; }               // LD (HL), E
void GBCPU::OP74() { writeByte(H, GetHL()); ++PC; cycles = 8; }               // LD (HL), H
void GBCPU::OP75() { writeByte(L, GetHL()); ++PC; cycles = 8; }               // LD (HL), L
void GBCPU::OP76() { while (MEM[INTERRUPT_FLAG] == 0x00); ++PC; cycles = 4; } // HALT until an INTERRUPT occurs
void GBCPU::OP77() { writeByte(A, GetHL()); ++PC; cycles = 8; }               // LD (HL), A
void GBCPU::OP78() { A = B; ++PC; cycles = 4; }                               // LD A, B
void GBCPU::OP79() { A = C; ++PC; cycles = 4; }                               // LD A, C
void GBCPU::OP7A() { A = D; ++PC; cycles = 4; }                               // LD A, D
void GBCPU::OP7B() { A = E; ++PC; cycles = 4; }                               // LD A, E
void GBCPU::OP7C() { A = H; ++PC; cycles = 4; }                               // LD A, H
void GBCPU::OP7D() { A = L; ++PC; cycles = 4; }                               // LD A, L
void GBCPU::OP7E() { A = readByte(GetHL()); ++PC; cycles = 8; }               // LD A, (HL)
void GBCPU::OP7F() { A = A; ++PC; cycles = 4; }                               // LD A, A

void GBCPU::OP80() { ADD(A, B); ++PC; cycles = 4; }                                                 // ADD B
void GBCPU::OP81() { ADD(A, C); ++PC; cycles = 4; }                                                 // ADD C
void GBCPU::OP82() { ADD(A, D); ++PC; cycles = 4; }                                                 // ADD D
void GBCPU::OP83() { ADD(A, E); ++PC; cycles = 4; }                                                 // ADD E
void GBCPU::OP84() { ADD(A, H); ++PC; cycles = 4; }                                                 // ADD H
void GBCPU::OP85() { ADD(A, L); ++PC; cycles = 4; }                                                 // ADD L
void GBCPU::OP86() { ADD(A, readByte(GetHL())); ++PC; cycles = 8; }                                 // ADD (HL)
void GBCPU::OP87() { ADD(A, A); ++PC; cycles = 4; }                                                 // ADD A
void GBCPU::OP88() { ADD(A, B + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // ADC B
void GBCPU::OP89() { ADD(A, C + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // ADC C
void GBCPU::OP8A() { ADD(A, D + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // ADC D
void GBCPU::OP8B() { ADD(A, E + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // ADC E
void GBCPU::OP8C() { ADD(A, H + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // ADC H
void GBCPU::OP8D() { ADD(A, L + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // ADC L
void GBCPU::OP8E() { ADD(A, readByte(GetHL()) + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 8; } // ADC (HL)
void GBCPU::OP8F() { ADD(A, A + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // ADC A

void GBCPU::OP90() { SUB(A, B); ++PC; cycles = 4; }                                                 // SUB B
void GBCPU::OP91() { SUB(A, C); ++PC; cycles = 4; }                                                 // SUB C
void GBCPU::OP92() { SUB(A, D); ++PC; cycles = 4; }                                                 // SUB D
void GBCPU::OP93() { SUB(A, E); ++PC; cycles = 4; }                                                 // SUB E
void GBCPU::OP94() { SUB(A, H); ++PC; cycles = 4; }                                                 // SUB H
void GBCPU::OP95() { SUB(A, L); ++PC; cycles = 4; }                                                 // SUB L
void GBCPU::OP96() { SUB(A, readByte(GetHL())); ++PC; cycles = 8; }                                 // SUB (HL)
void GBCPU::OP97() { SUB(A, A); ++PC; cycles = 4; }                                                 // SUB A
void GBCPU::OP98() { SUB(A, B + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // SBC B
void GBCPU::OP99() { SUB(A, C + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // SBC C
void GBCPU::OP9A() { SUB(A, D + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // SBC D
void GBCPU::OP9B() { SUB(A, E + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // SBC E
void GBCPU::OP9C() { SUB(A, H + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // SBC H
void GBCPU::OP9D() { SUB(A, L + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // SBC L
void GBCPU::OP9E() { SUB(A, readByte(GetHL()) + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 8; } // SBC (HL)
void GBCPU::OP9F() { SUB(A, A + (CARRY_FLAG == true ? 0x01 : 0x00)); ++PC; cycles = 4; }            // SBC A

void GBCPU::OPA0() { AND(A, B); ++PC; cycles = 4; }            // AND, B 
void GBCPU::OPA1() { AND(A, C); ++PC; cycles = 4; }            // AND, C 
void GBCPU::OPA2() { AND(A, D); ++PC; cycles = 4; }            // AND, D 
void GBCPU::OPA3() { AND(A, E); ++PC; cycles = 4; }            // AND, E 
void GBCPU::OPA4() { AND(A, H); ++PC; cycles = 4; }            // AND, H 
void GBCPU::OPA5() { AND(A, L); ++PC; cycles = 4; }            // AND, L
void GBCPU::OPA6() { AND(A, readByte(GetHL())); ++PC; cycles = 8; } // AND, (HL)
void GBCPU::OPA7() { AND(A, B); ++PC; cycles = 4; }            // AND, A
void GBCPU::OPA8() { XOR(A, B); ++PC; cycles = 4; }            // XOR, B 
void GBCPU::OPA9() { XOR(A, C); ++PC; cycles = 4; }            // XOR, C 
void GBCPU::OPAA() { XOR(A, D); ++PC; cycles = 4; }            // XOR, D 
void GBCPU::OPAB() { XOR(A, E); ++PC; cycles = 4; }            // XOR, E 
void GBCPU::OPAC() { XOR(A, H); ++PC; cycles = 4; }            // XOR, H 
void GBCPU::OPAD() { XOR(A, L); ++PC; cycles = 4; }            // XOR, L
void GBCPU::OPAE() { XOR(A, readByte(GetHL())); ++PC; cycles = 8; } // XOR, (HL)
void GBCPU::OPAF() { XOR(A, A); ++PC; cycles = 4; }            // XOR, A

void GBCPU::OPB0() { OR(A, B); ++PC; cycles = 4; }            // OR, B 
void GBCPU::OPB1() { OR(A, C); ++PC; cycles = 4; }            // OR, C 
void GBCPU::OPB2() { OR(A, D); ++PC; cycles = 4; }            // OR, D 
void GBCPU::OPB3() { OR(A, E); ++PC; cycles = 4; }            // OR, E 
void GBCPU::OPB4() { OR(A, H); ++PC; cycles = 4; }            // OR, H 
void GBCPU::OPB5() { OR(A, L); ++PC; cycles = 4; }            // OR, L
void GBCPU::OPB6() { OR(A, readByte(GetHL())); ++PC; cycles = 8; } // OR, (HL)
void GBCPU::OPB7() { OR(A, A); ++PC; cycles = 4; }            // OR, A
void GBCPU::OPB8() { CP(A, B); ++PC; cycles = 4; }            // CP, B 
void GBCPU::OPB9() { CP(A, C); ++PC; cycles = 4; }            // CP, C 
void GBCPU::OPBA() { CP(A, D); ++PC; cycles = 4; }            // CP, D 
void GBCPU::OPBB() { CP(A, E); ++PC; cycles = 4; }            // CP, E 
void GBCPU::OPBC() { CP(A, H); ++PC; cycles = 4; }            // CP, H 
void GBCPU::OPBD() { CP(A, L); ++PC; cycles = 4; }            // CP, L
void GBCPU::OPBE() { CP(A, readByte(GetHL())); ++PC; cycles = 8; } // CP, (HL)
void GBCPU::OPBF() { CP(A, A); ++PC; cycles = 4; }            // CP, A

void GBCPU::OPC0() { if (ZERO_FLAG == false) RET(); else ++PC; cycles = 8; }
void GBCPU::OPC1() { POP(B, C); ++PC; cycles = 12; }
void GBCPU::OPC2() { if (ZERO_FLAG == false) JP(); else PC += 3; cycles = 12; }
void GBCPU::OPC3() { JP(); cycles = 12; }
void GBCPU::OPC4() { if (ZERO_FLAG == false) CALL(); else PC += 3; cycles = 12; }
void GBCPU::OPC5() { PUSH(B, C); ++PC; cycles = 16; }
void GBCPU::OPC6() { ADD(A, readImmByte() ); PC += 2;  cycles = 8; }
void GBCPU::OPC7() { RST(0x00); cycles = 32; }
void GBCPU::OPC8() { if (ZERO_FLAG == true) RET(); else ++PC; cycles = 8; }
void GBCPU::OPC9() { RET(); cycles = 8; }
void GBCPU::OPCA() { if (ZERO_FLAG == true) JP(); else PC += 3; cycles = 12; }
void GBCPU::OPCB() { (this->*(CBopcodes)[MEM[PC+1]])(); /*cout << "CB Opcode called!" << endl;*/ /* PREFIX CB OPCODES - DO NOT USE. */ }
void GBCPU::OPCC() { if (ZERO_FLAG == true) CALL(); else PC += 3; cycles = 12; }
void GBCPU::OPCD() { CALL(); cycles = 12; } // CALL nn
void GBCPU::OPCE() { ADD(A, readImmByte() + (CARRY_FLAG == true)); PC += 2; cycles = 8; }
void GBCPU::OPCF() { RST(0x08); cycles = 32; }

void GBCPU::OPD0() { if (CARRY_FLAG == false) RET(); else ++PC; cycles = 8; }
void GBCPU::OPD1() { POP(D, E); ++PC; cycles = 12; }
void GBCPU::OPD2() { if (CARRY_FLAG == false) JP(); else PC += 3; cycles = 12; }
void GBCPU::OPD3() { cout << "ILLEGAL OPCODE CALLED!" << endl; /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPD4() { if (CARRY_FLAG == false) CALL(); else PC += 3; cycles = 12; }
void GBCPU::OPD5() { PUSH(D, E); ++PC; cycles = 16; }
void GBCPU::OPD6() { SUB(A, readImmByte() ); PC += 2; cycles = 8; }
void GBCPU::OPD7() { RST(0x10); cycles = 32; }
void GBCPU::OPD8() { if (CARRY_FLAG == true) RET(); else ++PC; cycles = 8; }
void GBCPU::OPD9() { RET(); IME = true; cycles = 8; }
void GBCPU::OPDA() { if (CARRY_FLAG == true) JP(); else PC += 3; cycles = 12; }
void GBCPU::OPDB() { cout << "ILLEGAL OPCODE CALLED!" << endl; /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPDC() { if (CARRY_FLAG == true) CALL(); else PC += 3; cycles = 12; }
void GBCPU::OPDD() { cout << "ILLEGAL OPCODE CALLED!" << endl; /* DO NOTHING - BLANK OPCODE */ }
void GBCPU::OPDE() { SUB(A, readImmByte() + (CARRY_FLAG == true)); PC += 2; cycles = 8; }
void GBCPU::OPDF() { RST(0x18); cycles = 32; }

void GBCPU::OPE0() { /*printf("Loading A into address %X!\n", 0xFF00 + readImmByte());*/ writeByte(A, 0xFF00 + readImmByte()); PC += 2; cycles = 12; } // LD ($FF00 + #), A
void GBCPU::OPE1() { POP(H, L); ++PC; cycles = 12; }
void GBCPU::OPE2() { /*printf("Loading A into address %X!\n", 0xFF00 + C); */ writeByte(A, 0xFF00 + C); ++PC; cycles = 8; } // LD ($FF00 + C), A
void GBCPU::OPE3() { cout << "ILLEGAL OPCODE CALLED!" << endl; }
void GBCPU::OPE4() { cout << "ILLEGAL OPCODE CALLED!" << endl; }
void GBCPU::OPE5() { PUSH(H, L); ++PC; cycles = 16; }
void GBCPU::OPE6() { AND(A, readImmByte() ); PC += 2;  cycles = 8; }
void GBCPU::OPE7() { RST(0x20); cycles = 32; }
void GBCPU::OPE8() { ADDSP(); PC += 2; cycles = 16; }
void GBCPU::OPE9() { PC = GetHL(); cycles = 4; } // JP (HL)
void GBCPU::OPEA() { writeByte(A, readImmWord()); PC += 3; cycles = 16; } // LD (##), A
void GBCPU::OPEB() { cout << "ILLEGAL OPCODE CALLED!" << endl; }
void GBCPU::OPEC() { cout << "ILLEGAL OPCODE CALLED!" << endl; }
void GBCPU::OPED() { cout << "ILLEGAL OPCODE CALLED!" << endl; }
void GBCPU::OPEE() { XOR( A, readImmByte() ); PC += 2; cycles = 8; }
void GBCPU::OPEF() { RST(0x28); cycles = 32; }

void GBCPU::OPF0() { /*printf("Loading %X onto A from address %X!\n", MEM[0xFF00 + readImmByte()], 0xFF00 + readImmByte());*/ A = readByte(0xFF00 + readImmByte()); PC += 2; cycles = 12; } // LD A, ($FF00 + #)
void GBCPU::OPF1() { byte temp = 0x00; POP(A, temp); SetF(temp); ++PC; cycles = 12; }
void GBCPU::OPF2() { /*printf("Loading %X onto A from address %X!\n", MEM[0xFF00 + C], 0xF00 + C);*/ A = readByte(0xFF00 + C); ++PC; cycles = 8; } // LD A, ($FF00 + C)
void GBCPU::OPF3() { IME = false; ++PC; cycles = 4; }                     // DI
void GBCPU::OPF4() { cout << "ILLEGAL OPCODE CALLED!" << endl; }
void GBCPU::OPF5() { PUSH(A, GetF()); ++PC; cycles = 16; }
void GBCPU::OPF6() { OR(A, readImmByte() ); PC += 2;  cycles = 8; }
void GBCPU::OPF7() { RST(0x30); cycles = 32; }
void GBCPU::OPF8() { SetHL( word(SP + (signed_byte)readImmByte()) ); PC += 2; cycles = 12; SUBTRACT_FLAG = false; ZERO_FLAG = false; 
                     // Detect half carry and carry
                     ((SP & 0x0F) +  ((signed_byte)readImmByte() & 0x0F)) ? HALF_CARRY_FLAG = true : HALF_CARRY_FLAG = false;
                     ((SP & 0x0FF) + ((signed_byte)readImmByte() & 0xFF)) ? CARRY_FLAG = true : CARRY_FLAG = false; }                      // LD HL SP, n
void GBCPU::OPF9() { SP = GetHL(); ++PC; cycles = 8; }                                                                                     // LD SP, HL
void GBCPU::OPFA() { A = readByte(readImmWord()); PC += 3; cycles = 16; } // LD A, (##)
void GBCPU::OPFB() { IME = true; ++PC; cycles = 4; }                      // EI
void GBCPU::OPFC() { cout << "ILLEGAL OPCODE CALLED!" << endl; }
void GBCPU::OPFD() { cout << "ILLEGAL OPCODE CALLED!" << endl; }
void GBCPU::OPFE() { CP(A, readImmByte() ); PC += 2; cycles = 8; }
void GBCPU::OPFF() { RST(0x38); cycles = 32; }


/* CB Operations */

void GBCPU::CBOP00() { RLC(B); PC += 2; cycles = 8; }
void GBCPU::CBOP01() { RLC(C); PC += 2; cycles = 8; }
void GBCPU::CBOP02() { RLC(D); PC += 2; cycles = 8; }
void GBCPU::CBOP03() { RLC(E); PC += 2; cycles = 8; }
void GBCPU::CBOP04() { RLC(H); PC += 2; cycles = 8; }
void GBCPU::CBOP05() { RLC(L); PC += 2; cycles = 8; }
void GBCPU::CBOP06() { byte temp = readByte(GetHL()); RLC(temp); writeByte(temp, GetHL()); PC += 2; cycles = 16; }
void GBCPU::CBOP07() { RLC(A); PC += 2; cycles = 8; }
void GBCPU::CBOP08() { RRC(B); PC += 2; cycles = 8; }
void GBCPU::CBOP09() { RRC(C); PC += 2; cycles = 8; }
void GBCPU::CBOP0A() { RRC(D); PC += 2; cycles = 8; }
void GBCPU::CBOP0B() { RRC(E); PC += 2; cycles = 8; }
void GBCPU::CBOP0C() { RRC(H); PC += 2; cycles = 8; }
void GBCPU::CBOP0D() { RRC(L); PC += 2; cycles = 8; }
void GBCPU::CBOP0E() { byte temp = readByte(GetHL()); RRC(temp); writeByte(temp, GetHL()); PC += 2; cycles = 16; }
void GBCPU::CBOP0F() { RRC(A); PC += 2; cycles = 8; }

void GBCPU::CBOP10() { RL(B); PC += 2; cycles = 8; }
void GBCPU::CBOP11() { RL(C); PC += 2; cycles = 8; }
void GBCPU::CBOP12() { RL(D); PC += 2; cycles = 8; }
void GBCPU::CBOP13() { RL(E); PC += 2; cycles = 8; }
void GBCPU::CBOP14() { RL(H); PC += 2; cycles = 8; }
void GBCPU::CBOP15() { RL(L); PC += 2; cycles = 8; }
void GBCPU::CBOP16() { byte temp = readByte(GetHL()); RL(temp); writeByte(temp, GetHL()); PC += 2; cycles = 16; }
void GBCPU::CBOP17() { RL(A); PC += 2; cycles = 8; }
void GBCPU::CBOP18() { RR(B); PC += 2; cycles = 8; }
void GBCPU::CBOP19() { RR(C); PC += 2; cycles = 8; }
void GBCPU::CBOP1A() { RR(D); PC += 2; cycles = 8; }
void GBCPU::CBOP1B() { RR(E); PC += 2; cycles = 8; }
void GBCPU::CBOP1C() { RR(H); PC += 2; cycles = 8; }
void GBCPU::CBOP1D() { RR(L); PC += 2; cycles = 8; }
void GBCPU::CBOP1E() { byte temp = readByte(GetHL()); RR(temp); writeByte(temp, GetHL()); PC += 2; cycles = 16; }
void GBCPU::CBOP1F() { RR(A); PC += 2; cycles = 8; }

void GBCPU::CBOP20() { SLA(B); PC += 2; cycles = 8; }
void GBCPU::CBOP21() { SLA(C); PC += 2; cycles = 8; }
void GBCPU::CBOP22() { SLA(D); PC += 2; cycles = 8; }
void GBCPU::CBOP23() { SLA(E); PC += 2; cycles = 8; }
void GBCPU::CBOP24() { SLA(H); PC += 2; cycles = 8; }
void GBCPU::CBOP25() { SLA(L); PC += 2; cycles = 8; }
void GBCPU::CBOP26() { byte temp = readByte(GetHL()); SLA(temp); writeByte(temp, GetHL()); PC += 2; cycles = 16; }
void GBCPU::CBOP27() { SLA(A); PC += 2; cycles = 8; }
void GBCPU::CBOP28() { SRA(B); PC += 2; cycles = 8; }
void GBCPU::CBOP29() { SRA(C); PC += 2; cycles = 8; }
void GBCPU::CBOP2A() { SRA(D); PC += 2; cycles = 8; }
void GBCPU::CBOP2B() { SRA(E); PC += 2; cycles = 8; }
void GBCPU::CBOP2C() { SRA(H); PC += 2; cycles = 8; }
void GBCPU::CBOP2D() { SRA(L); PC += 2; cycles = 8; }
void GBCPU::CBOP2E() { byte temp = readByte(GetHL()); SRA(temp); writeByte(temp, GetHL()); PC += 2; cycles = 16; }
void GBCPU::CBOP2F() { SRA(A); PC += 2; cycles = 8; }

void GBCPU::CBOP30() { SWAP(B); PC += 2; cycles = 8; }
void GBCPU::CBOP31() { SWAP(C); PC += 2; cycles = 8; }
void GBCPU::CBOP32() { SWAP(D); PC += 2; cycles = 8; }
void GBCPU::CBOP33() { SWAP(E); PC += 2; cycles = 8; }
void GBCPU::CBOP34() { SWAP(H); PC += 2; cycles = 8; }
void GBCPU::CBOP35() { SWAP(L); PC += 2; cycles = 8; }
void GBCPU::CBOP36() { byte temp = readByte(GetHL()); SWAP(temp); writeByte(temp, GetHL()); PC += 2; cycles = 16; }
void GBCPU::CBOP37() { SWAP(A); PC += 2; cycles = 8; }
void GBCPU::CBOP38() { SRL(B); PC += 2; cycles = 8; }
void GBCPU::CBOP39() { SRL(C); PC += 2; cycles = 8; }
void GBCPU::CBOP3A() { SRL(D); PC += 2; cycles = 8; }
void GBCPU::CBOP3B() { SRL(E); PC += 2; cycles = 8; }
void GBCPU::CBOP3C() { SRL(H); PC += 2; cycles = 8; }
void GBCPU::CBOP3D() { SRL(L); PC += 2; cycles = 8; }
void GBCPU::CBOP3E() { byte temp = readByte(GetHL()); SRL(temp); writeByte(temp, GetHL()); PC += 2; cycles = 16; }
void GBCPU::CBOP3F() { SRL(A); PC += 2; cycles = 8; }

void GBCPU::CBOP40() { BIT(0, B); PC += 2; cycles = 8; }
void GBCPU::CBOP41() { BIT(0, C); PC += 2; cycles = 8; }
void GBCPU::CBOP42() { BIT(0, D); PC += 2; cycles = 8; }
void GBCPU::CBOP43() { BIT(0, E); PC += 2; cycles = 8; }
void GBCPU::CBOP44() { BIT(0, H); PC += 2; cycles = 8; }
void GBCPU::CBOP45() { BIT(0, L); PC += 2; cycles = 8; }
void GBCPU::CBOP46() { byte temp = readByte(GetHL()); BIT(0, temp); writeByte(temp, GetHL()); PC += 2; cycles = 16; }
void GBCPU::CBOP47() { BIT(0, A); PC += 2; cycles = 8; }
void GBCPU::CBOP48() { BIT(1, B); PC += 2; cycles = 8; }
void GBCPU::CBOP49() { BIT(1, C); PC += 2; cycles = 8; }
void GBCPU::CBOP4A() { BIT(1, D); PC += 2; cycles = 8; }
void GBCPU::CBOP4B() { BIT(1, E); PC += 2; cycles = 8; }
void GBCPU::CBOP4C() { BIT(1, H); PC += 2; cycles = 8; }
void GBCPU::CBOP4D() { BIT(1, L); PC += 2; cycles = 8; }
void GBCPU::CBOP4E() { byte temp = readByte(GetHL()); BIT(1, temp); writeByte(temp, GetHL());  PC += 2; cycles = 16; }
void GBCPU::CBOP4F() { BIT(1, A); PC += 2; cycles = 8; }

void GBCPU::CBOP50() { BIT(2, B); PC += 2; cycles = 8; }
void GBCPU::CBOP51() { BIT(2, C); PC += 2; cycles = 8; }
void GBCPU::CBOP52() { BIT(2, D); PC += 2; cycles = 8; }
void GBCPU::CBOP53() { BIT(2, E); PC += 2; cycles = 8; }
void GBCPU::CBOP54() { BIT(2, H); PC += 2; cycles = 8; }
void GBCPU::CBOP55() { BIT(2, L); PC += 2; cycles = 8; }
void GBCPU::CBOP56() { byte temp = readByte(GetHL()); BIT(2, temp); writeByte(temp, GetHL());  PC += 2; cycles = 16; }
void GBCPU::CBOP57() { BIT(2, A); PC += 2; cycles = 8; }
void GBCPU::CBOP58() { BIT(3, B); PC += 2; cycles = 8; }
void GBCPU::CBOP59() { BIT(3, C); PC += 2; cycles = 8; }
void GBCPU::CBOP5A() { BIT(3, D); PC += 2; cycles = 8; }
void GBCPU::CBOP5B() { BIT(3, E); PC += 2; cycles = 8; }
void GBCPU::CBOP5C() { BIT(3, H); PC += 2; cycles = 8; }
void GBCPU::CBOP5D() { BIT(3, L); PC += 2; cycles = 8; }
void GBCPU::CBOP5E() { byte temp = readByte(GetHL()); BIT(3, temp); writeByte(temp, GetHL());  PC += 2; cycles = 16; }
void GBCPU::CBOP5F() { BIT(3, A); PC += 2; cycles = 8; }

void GBCPU::CBOP60() { BIT(4, B); PC += 2; cycles = 8; }
void GBCPU::CBOP61() { BIT(4, C); PC += 2; cycles = 8; }
void GBCPU::CBOP62() { BIT(4, D); PC += 2; cycles = 8; }
void GBCPU::CBOP63() { BIT(4, E); PC += 2; cycles = 8; }
void GBCPU::CBOP64() { BIT(4, H); PC += 2; cycles = 8; }
void GBCPU::CBOP65() { BIT(4, L); PC += 2; cycles = 8; }
void GBCPU::CBOP66() { byte temp = readByte(GetHL()); BIT(4, temp); writeByte(temp, GetHL());  PC += 2; cycles = 16; }
void GBCPU::CBOP67() { BIT(4, A); PC += 2; cycles = 8; }
void GBCPU::CBOP68() { BIT(5, B); PC += 2; cycles = 8; }
void GBCPU::CBOP69() { BIT(5, C); PC += 2; cycles = 8; }
void GBCPU::CBOP6A() { BIT(5, D); PC += 2; cycles = 8; }
void GBCPU::CBOP6B() { BIT(5, E); PC += 2; cycles = 8; }
void GBCPU::CBOP6C() { BIT(5, H); PC += 2; cycles = 8; }
void GBCPU::CBOP6D() { BIT(5, L); PC += 2; cycles = 8; }
void GBCPU::CBOP6E() { byte temp = readByte(GetHL()); BIT(5, temp); writeByte(temp, GetHL());  PC += 2; cycles = 16; }
void GBCPU::CBOP6F() { BIT(5, A); PC += 2; cycles = 8; }

void GBCPU::CBOP70() { BIT(6, B); PC += 2; cycles = 8; }
void GBCPU::CBOP71() { BIT(6, C); PC += 2; cycles = 8; }
void GBCPU::CBOP72() { BIT(6, D); PC += 2; cycles = 8; }
void GBCPU::CBOP73() { BIT(6, E); PC += 2; cycles = 8; }
void GBCPU::CBOP74() { BIT(6, H); PC += 2; cycles = 8; }
void GBCPU::CBOP75() { BIT(6, L); PC += 2; cycles = 8; }
void GBCPU::CBOP76() { byte temp = readByte(GetHL()); BIT(6, temp); writeByte(temp, GetHL()); PC += 2; cycles = 16; }
void GBCPU::CBOP77() { BIT(6, A); PC += 2; cycles = 8; }
void GBCPU::CBOP78() { BIT(7, B); PC += 2; cycles = 8; }
void GBCPU::CBOP79() { BIT(7, C); PC += 2; cycles = 8; }
void GBCPU::CBOP7A() { BIT(7, D); PC += 2; cycles = 8; }
void GBCPU::CBOP7B() { BIT(7, E); PC += 2; cycles = 8; }
void GBCPU::CBOP7C() { BIT(7, H); PC += 2; cycles = 8; }
void GBCPU::CBOP7D() { BIT(7, L); PC += 2; cycles = 8; }
void GBCPU::CBOP7E() { byte temp = readByte(GetHL()); BIT(7, temp); writeByte(temp, GetHL());  PC += 2; cycles = 16; }
void GBCPU::CBOP7F() { BIT(7, A); PC += 2; cycles = 8; }

void GBCPU::CBOP80() { CLRBIT(B, 0); PC += 2; cycles = 8; } // RES
void GBCPU::CBOP81() { CLRBIT(C, 0); PC += 2; cycles = 8; }
void GBCPU::CBOP82() { CLRBIT(D, 0); PC += 2; cycles = 8; }
void GBCPU::CBOP83() { CLRBIT(E, 0); PC += 2; cycles = 8; }
void GBCPU::CBOP84() { CLRBIT(H, 0); PC += 2; cycles = 8; }
void GBCPU::CBOP85() { CLRBIT(L, 0); PC += 2; cycles = 8; }
void GBCPU::CBOP86() { byte temp = readByte(GetHL()); CLRBIT(temp, 0); writeByte(temp, GetHL());  PC += 2; cycles = 8; }
void GBCPU::CBOP87() { CLRBIT(A, 1); PC += 2; cycles = 8; }
void GBCPU::CBOP88() { CLRBIT(B, 1); PC += 2; cycles = 8; }
void GBCPU::CBOP89() { CLRBIT(C, 1); PC += 2; cycles = 8; }
void GBCPU::CBOP8A() { CLRBIT(D, 1); PC += 2; cycles = 8; }
void GBCPU::CBOP8B() { CLRBIT(E, 1); PC += 2; cycles = 8; }
void GBCPU::CBOP8C() { CLRBIT(H, 1); PC += 2; cycles = 8; }
void GBCPU::CBOP8D() { CLRBIT(L, 1); PC += 2; cycles = 8; }
void GBCPU::CBOP8E() { byte temp = readByte(GetHL()); CLRBIT(temp, 1); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOP8F() { CLRBIT(A, 1); PC += 2; cycles = 8; }

void GBCPU::CBOP90() { CLRBIT(B, 2); PC += 2; cycles = 8; }
void GBCPU::CBOP91() { CLRBIT(C, 2); PC += 2; cycles = 8; }
void GBCPU::CBOP92() { CLRBIT(D, 2); PC += 2; cycles = 8; }
void GBCPU::CBOP93() { CLRBIT(E, 2); PC += 2; cycles = 8; }
void GBCPU::CBOP94() { CLRBIT(H, 2); PC += 2; cycles = 8; }
void GBCPU::CBOP95() { CLRBIT(L, 2); PC += 2; cycles = 8; }
void GBCPU::CBOP96() { byte temp = readByte(GetHL()); CLRBIT(temp, 2); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOP97() { CLRBIT(A, 2); PC += 2; cycles = 8; }
void GBCPU::CBOP98() { CLRBIT(B, 3); PC += 2; cycles = 8; }
void GBCPU::CBOP99() { CLRBIT(C, 3); PC += 2; cycles = 8; }
void GBCPU::CBOP9A() { CLRBIT(D, 3); PC += 2; cycles = 8; }
void GBCPU::CBOP9B() { CLRBIT(E, 3); PC += 2; cycles = 8; }
void GBCPU::CBOP9C() { CLRBIT(H, 3); PC += 2; cycles = 8; }
void GBCPU::CBOP9D() { CLRBIT(L, 3); PC += 2; cycles = 8; }
void GBCPU::CBOP9E() { byte temp = readByte(GetHL()); CLRBIT(temp, 3); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOP9F() { CLRBIT(A, 3); PC += 2; cycles = 8; }

void GBCPU::CBOPA0() { CLRBIT(B, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPA1() { CLRBIT(C, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPA2() { CLRBIT(D, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPA3() { CLRBIT(E, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPA4() { CLRBIT(H, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPA5() { CLRBIT(L, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPA6() { byte temp = readByte(GetHL()); CLRBIT(temp, 4); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOPA7() { CLRBIT(A, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPA8() { CLRBIT(B, 5); PC += 2; cycles = 8; }
void GBCPU::CBOPA9() { CLRBIT(C, 5); PC += 2; cycles = 8; }
void GBCPU::CBOPAA() { CLRBIT(D, 5); PC += 2; cycles = 8; }
void GBCPU::CBOPAB() { CLRBIT(E, 5); PC += 2; cycles = 8; }
void GBCPU::CBOPAC() { CLRBIT(H, 5); PC += 2; cycles = 8; }
void GBCPU::CBOPAD() { CLRBIT(L, 5); PC += 2; cycles = 8; }
void GBCPU::CBOPAE() { byte temp = readByte(GetHL()); CLRBIT(temp, 5); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOPAF() { CLRBIT(A, 5); PC += 2; cycles = 8; }

void GBCPU::CBOPB0() { CLRBIT(B, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPB1() { CLRBIT(C, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPB2() { CLRBIT(D, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPB3() { CLRBIT(E, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPB4() { CLRBIT(H, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPB5() { CLRBIT(L, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPB6() { byte temp = readByte(GetHL()); CLRBIT(temp, 6); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOPB7() { CLRBIT(A, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPB8() { CLRBIT(B, 7); PC += 2; cycles = 8; }
void GBCPU::CBOPB9() { CLRBIT(C, 7); PC += 2; cycles = 8; }
void GBCPU::CBOPBA() { CLRBIT(D, 7); PC += 2; cycles = 8; }
void GBCPU::CBOPBB() { CLRBIT(E, 7); PC += 2; cycles = 8; }
void GBCPU::CBOPBC() { CLRBIT(H, 7); PC += 2; cycles = 8; }
void GBCPU::CBOPBD() { CLRBIT(L, 7); PC += 2; cycles = 8; }
void GBCPU::CBOPBE() { byte temp = readByte(GetHL()); CLRBIT(temp, 7); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOPBF() { CLRBIT(A, 7); PC += 2; cycles = 8; }

void GBCPU::CBOPC0() { SETBIT(B, 0); PC += 2; cycles = 8; } // SET
void GBCPU::CBOPC1() { SETBIT(C, 0); PC += 2; cycles = 8; }
void GBCPU::CBOPC2() { SETBIT(D, 0); PC += 2; cycles = 8; }
void GBCPU::CBOPC3() { SETBIT(E, 0); PC += 2; cycles = 8; }
void GBCPU::CBOPC4() { SETBIT(H, 0); PC += 2; cycles = 8; }
void GBCPU::CBOPC5() { SETBIT(L, 0); PC += 2; cycles = 8; }
void GBCPU::CBOPC6() { byte temp = readByte(GetHL());  SETBIT(temp, 0); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOPC7() { SETBIT(A, 1); PC += 2; cycles = 8; }
void GBCPU::CBOPC8() { SETBIT(B, 1); PC += 2; cycles = 8; }
void GBCPU::CBOPC9() { SETBIT(C, 1); PC += 2; cycles = 8; }
void GBCPU::CBOPCA() { SETBIT(D, 1); PC += 2; cycles = 8; }
void GBCPU::CBOPCB() { SETBIT(E, 1); PC += 2; cycles = 8; }
void GBCPU::CBOPCC() { SETBIT(H, 1); PC += 2; cycles = 8; }
void GBCPU::CBOPCD() { SETBIT(L, 1); PC += 2; cycles = 8; }
void GBCPU::CBOPCE() { byte temp = readByte(GetHL());  SETBIT(temp, 1); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOPCF() { SETBIT(A, 1); PC += 2; cycles = 8; }

void GBCPU::CBOPD0() { SETBIT(B, 2); PC += 2; cycles = 8; }
void GBCPU::CBOPD1() { SETBIT(C, 2); PC += 2; cycles = 8; }
void GBCPU::CBOPD2() { SETBIT(D, 2); PC += 2; cycles = 8; }
void GBCPU::CBOPD3() { SETBIT(E, 2); PC += 2; cycles = 8; }
void GBCPU::CBOPD4() { SETBIT(H, 2); PC += 2; cycles = 8; }
void GBCPU::CBOPD5() { SETBIT(L, 2); PC += 2; cycles = 8; }
void GBCPU::CBOPD6() { byte temp = readByte(GetHL());  SETBIT(temp, 2); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOPD7() { SETBIT(A, 2); PC += 2; cycles = 8; }
void GBCPU::CBOPD8() { SETBIT(B, 3); PC += 2; cycles = 8; }
void GBCPU::CBOPD9() { SETBIT(C, 3); PC += 2; cycles = 8; }
void GBCPU::CBOPDA() { SETBIT(D, 3); PC += 2; cycles = 8; }
void GBCPU::CBOPDB() { SETBIT(E, 3); PC += 2; cycles = 8; }
void GBCPU::CBOPDC() { SETBIT(H, 3); PC += 2; cycles = 8; }
void GBCPU::CBOPDD() { SETBIT(L, 3); PC += 2; cycles = 8; }
void GBCPU::CBOPDE() { byte temp = readByte(GetHL());  SETBIT(temp, 3); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOPDF() { SETBIT(A, 3); PC += 2; cycles = 8; }

void GBCPU::CBOPE0() { SETBIT(B, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPE1() { SETBIT(C, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPE2() { SETBIT(D, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPE3() { SETBIT(E, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPE4() { SETBIT(H, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPE5() { SETBIT(L, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPE6() { byte temp = readByte(GetHL());  SETBIT(temp, 4); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOPE7() { SETBIT(A, 4); PC += 2; cycles = 8; }
void GBCPU::CBOPE8() { SETBIT(B, 5); PC += 2; cycles = 8; }
void GBCPU::CBOPE9() { SETBIT(C, 5); PC += 2; cycles = 8; }
void GBCPU::CBOPEA() { SETBIT(D, 5); PC += 2; cycles = 8; }
void GBCPU::CBOPEB() { SETBIT(E, 5); PC += 2; cycles = 8; }
void GBCPU::CBOPEC() { SETBIT(H, 5); PC += 2; cycles = 8; }
void GBCPU::CBOPED() { SETBIT(L, 5); PC += 2; cycles = 8; }
void GBCPU::CBOPEE() { byte temp = readByte(GetHL());  SETBIT(temp, 5); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOPEF() { SETBIT(A, 5); PC += 2; cycles = 8; }

void GBCPU::CBOPF0() { SETBIT(B, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPF1() { SETBIT(C, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPF2() { SETBIT(D, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPF3() { SETBIT(E, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPF4() { SETBIT(H, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPF5() { SETBIT(L, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPF6() { byte temp = readByte(GetHL());  SETBIT(temp, 6); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOPF7() { SETBIT(A, 6); PC += 2; cycles = 8; }
void GBCPU::CBOPF8() { SETBIT(B, 7); PC += 2; cycles = 8; }
void GBCPU::CBOPF9() { SETBIT(C, 7); PC += 2; cycles = 8; }
void GBCPU::CBOPFA() { SETBIT(D, 7); PC += 2; cycles = 8; }
void GBCPU::CBOPFB() { SETBIT(E, 7); PC += 2; cycles = 8; }
void GBCPU::CBOPFC() { SETBIT(H, 7); PC += 2; cycles = 8; }
void GBCPU::CBOPFD() { SETBIT(L, 7); PC += 2; cycles = 8; }
void GBCPU::CBOPFE() { byte temp = readByte(GetHL());  SETBIT(temp, 7); writeByte(temp, GetHL()); PC += 2; cycles = 8; }
void GBCPU::CBOPFF() { SETBIT(A, 7); PC += 2; cycles = 8; }