#ifndef GAMEBOY_H
#define GAMEBOY_H


// @TODO:
// Audio file and macro definitions
// Confirm if byte data type should be unsigned or not (for negative offset operations)
// CPU Start up procedure - half done
// Research interrupt flag bits

// Datatype Definitions
typedef unsigned char byte;
typedef signed char signed_byte;
typedef unsigned short word;
typedef signed short signed_word;

/*
	CPU Definitions and Macros.

    Notes:

    1) 'x' is denoted as the MSB, while 'y' is the LSB.
*/

// Cast LSB and MSB as a WORD (16 bits)
#define CASTWD(x, y) (((((word)x) << 8) & 0xFF00) | ((word)y) & 0x00FF)

// Decrement macro (note: this is not the actual DEC and INC opcode. See DECR, INCR)
#define DEC(x, y) word temp = (CASTWD(x,y) - 1) & 0xFFFF; \
                      x = (byte)((temp >> 8) & 0xFF); \
                      y = (byte)((temp) & 0xFF);

// Increment macro (note: this is not the actual DEC and INC opcode. See DECR, INCR)
#define INC(x, y) word temp = (CASTWD(x,y) + 1) & 0xFFFF; \
                      x = (byte)((temp >> 8) & 0xFF); \
                      y = (byte)((temp) & 0xFF);

// PUSH/POP to/from stack
#define PUSH(x, y)   --SP; GBCPU::writeByte(x, SP); --SP; GBCPU::writeByte(y, SP);         // PUSH stores MSB first, then LSB (little-endian CPU)
#define POP(x, y)    y = GBCPU::readByte(SP); ++SP; x = GBCPU::readByte(SP); ++SP;         // POP loads LSB first, then MSB

// Set and clear bits (used mainly for setting status register)
#define SETBIT(val, bit) (val |= (0x01 << bit))
#define CLRBIT(val, bit) (val &= ~(0x01 << bit))


/*	CPU Address Space Definitions
Courtesy: https://realboyemulator.wordpress.com/

0x0000 - 0x3FFF Permanently-mapped ROM bank
0x4000 - 0x7FFF Area for switchable ROM banks

0xFE00 - 0xFEFF Sprite Attribute Table

0xFF00 - 0xFF7F Device's Mappings. I/O Device Access

0xFF90 - 0xFFFE High RAM Area
*/

#define GAMEBOY_CLOCK_CYCLES 4194304 // The number of CPU clock cycles executed per second
#define GAMEBOY_FRAME_RATE   60      // The number of LCD frames rendered per second

#define GAMEBOY_CYCLES_FRAME   \
        GAMEBOY_CLOCK_CYCLES / GAMEBOY_FRAME_RATE // The number of cycles per frame

#define INTERRUPT_ENABLE	 0xFFFF  // $FFFF - IE - Interrupt Enable (1 = Enabled) address

#define HRAM_END             0xFFFE  // High RAM ending address
#define HRAM_START           0xFF80  // High RAM, otherwise known as Zero Page beginning address

#define INTERRUPT_FLAG		 0xFF0F  // $FF0F - IF - Interrupt Flag - (1 = Requested) address
                                     // Bits 4 - 0: Joypad - Serial - Timer - LCD St - V-Blank (lower bit = high pri.)

// 0xFEA0 - 0xFEFF is Unusable!

#define SPRITE_TABLE_END     0xFE9F  // Sprite Attribute table ending address
#define SPRITE_TABLE_START   0xFE00  // Sprite Attribute table begninning address

#define WRAM_ECHO_END        0xFDFF  // Working RAM bank 0/1 echo data ending address
#define WRAM_ECHO_START      0xE000  // Working RAM bank 0/1 echo data beginning address

#define WRAM_END             0xDFFF  // Working RAM ending address
#define WRAM_START           0xC000  // Working RAM beginning address

#define EXTERNAL_RAM_END     0xBFFF  // External RAM ending address (switchable RAM)
#define EXTERNAL_RAM_START   0xA000  // External RAM beginning address (switchable RAM)

#define VRAM_END             0x9FFF  // Video RAM ending address
#define BG_MAP_1_START       0x9C00  // BG Map Data 2
#define BG_MAP_0_START       0x9800  // BG Map Data 1
#define VRAM_START           0x8000  // Video RAM beginning address

#define EXTERNAL_ROM_END     0x7FFF  // External ROM ending address (switchable ROM)
#define EXTERNAL_ROM_START   0x4000  // External ROM beginning address (switchable ROM)

#define ROM_END              0x3FFF  // ROM Bank #0 ending address
#define ROM_START            0x0000  // ROM Bank #0 beginning address

#define MAX_GB_MEMORY        0x10000 // 64k of byte-addresable memory


// TIMERS and DIVIDER registers
#define TIMA        0xFF05       // Timer Counter (TIMA)
#define TMOD        0xFF06       // Timer Modulo (TMA)
#define TCON        0xFF07       // Timer Control (TAC)

#define DIV         0xFF04       // Divider Register (R/W)
#define DIV_RATE    16384        // Divider Register rate in Hz (1/16384 cycles/sec)

// I/O Registers

#define JOYPAD_P1   0xFF00
#define SERIAL_XFER 0xFF01
#define SIO_CONTROL 0xFF02

// LCD OAM DMA Transfers
#define PPU_DMA             0xFF46 // DMA Transfer and Start Address

#endif /* GAMEBOY_H_ */
