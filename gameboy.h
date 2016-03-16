#ifndef GAMEBOY_H_
#define GAMEBOY_H_


// TODO:
// Audio file and macro definitions
// Confirm if byte data type should be unsigned or not (for negative offset operations)

// Datatype Definitions
typedef unsigned char byte;
typedef unsigned short word;

/*
	CPU Definitions
*/
#define MAX_GB_MEMORY        0x10000 // 64k of byte-addresable memory
#define MAX_GB_MAIN_RAM      0x2000  // 8k Main Working RAM
#define MAX_GB_VID_RAM       0x2000  // 8k Video RAM

// 8-bit LD MACROS
#define LD(nn, n)    nn = n
#define DEC(x, y)    x = ( ((((word)(x << 8)) | ((word) y)) - 1) >> 8);  \
						     y = ( ((((word)(x << 8)) | ((word) y)) - 1) & 0xFF)
#define INC(x, y)    x = ( ((((word)(x << 8)) | ((word) y)) + 1) >> 8);  \
						     y = ( ((((word)(x << 8)) | ((word) y)) + 1) & 0xFF)
#define CASTWD(x, y) ((((word)x) << 8) | ((word)y))
#define PUSH(x, y)   MEM[SP] = y; --SP; MEM[SP] = x; --SP;         // PUSH stores LSB first, then MSB
#define POP(x, y)    x = MEM[SP]; ++SP; y = MEM[SP]; ++SP;         // POP loads MSB first, then LSB

/*
	Picture Processing Unit Definitions
*/
#define MAX_SPRITES          40
#define MAX_SPRITES_PER_LINE 10

#define MAX_SPRITES_ROW      8
#define MAX_SPRITES_COL      16
#define MIN_SPRITES_ROW      8
#define MIN_SPRITES_COL      8

/*
	Status Register Definitions
*/
#define CARRY_FLAG			 0x08
#define HALF_CARRY_FLAG		 0x10
#define SUBTRACT_FLAG		 0x20
#define ZERO_FLAG			 0x40

/*
	Interrupt Flag Definitions - Highest Priority = Lowest Bit (0)
	FFFF - IE - Interrupt Enable (1 = Enabled)
	FF0F - IF - Interrupt Flag	 (1 = Request)

 b8|0		0		0		0		0		0		0		0|b0
	-		-		-		Joypad	Serial	Timer	LCD St	V-Blank	
*/
#define INTERRUPT_ENABLE	0xFFFF
#define INTERRUPT_FLAG		0xFF0F

#endif /* GAMEBOY_H_ */
