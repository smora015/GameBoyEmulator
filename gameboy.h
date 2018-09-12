#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <SDL.h>
// @TODO: Audio file and macro definitions

/********************************* Datatype Definitions *********************************/
typedef unsigned char BYTE;
typedef signed char SIGNED_BYTE;
typedef unsigned short WORD;
typedef signed short SIGNED_WORD;

// Struct that defines the pixels to be rendered on screen
typedef struct pixel {
    BYTE r;
    BYTE g;
    BYTE b;
} pixel;

// Struct that defines the current memory model for MBC cartridge types
typedef enum memory_model_types
{
    rom_banking,
    ram_banking
} memory_model_types;

// Struct that defines the cartridge type. Based on cartridge header
typedef enum MBC_TYPES
{
    ROM_ONLY,
    ROM_MBC1,
    ROM_MBC1_RAM,
    ROM_MBC1_RAM_BATT,
    ROM_MBC2,
    ROM_MBC2_BATT,
    ROM_RAM,
    ROM_RAM_BATT,
    ROM_MM01,
    ROM_MM01_SRAM,
    ROM_MM01_SRAM_BATT,
    ROM_MBC3_TIMER_BATT,
    ROM_MBC3_TIMER_RAM_BATT,
    ROM_MBC3,
    ROM_MBC3_RAM,
    ROM_MBC3_RAM_BATT,
    ROM_MBC5,
    ROM_MBC5_RAM,
    ROM_MBC5_RAM_BATT,
    ROM_MBC5_RUMBLE,
    ROM_MBC5_RUMBLE_SRAM,
    ROM_MBC5_RUMBLE_SRAM_BATT,
    POCKET_CAMERA,
    BANDAI_TAMA5,
    HUDSON_HUC_3,
    HUDSON_HUC_1,
    UNSUPPORTED
} MBC_TYPES;



/**************************** Global Variables ********************************/
/* Global SDL variables */
extern const Uint8 *SDL_GB_keyboard_state; // Holds the keys pressed
extern SDL_Event SDL_GB_window_event;

/* Memory-related variables from extracting ROM (Cartridge.cpp) */
extern BYTE * ext_rom;         // Holds all external (switchable) RAM, to be added in memory by 16 Kbyte banks
extern BYTE * ext_ram;         // Holds all external (switchable) ROM, to be added in memory by 8 Kbyte banks
extern size_t rom_size;        // Actual total ROM size in bytes
extern size_t ram_size;        // Actual total RAM size in bytes
extern size_t ext_rom_size;    // Size of external ROM in bytes
extern size_t ext_ram_size;    // Size of external RAM in bytes
extern MBC_TYPES rom_mbc_type; // The MBC cartridge type
extern char rom_name[17];      // The 16-byte name specified in the cartridge from $134-143

/* MBC management related variables (mbc.cpp) */
extern memory_model_types memory_model; // The current maximum memory model for MBC
extern BYTE current_rom_bank;           // The current switchable rom bank being used
extern BYTE current_ram_bank;           // The current switchable ram bank beign used
extern bool ram_bank_access_enabled;    // Indicates if RAM read/writes are enabled

/* Video-rendering related variables (render.cpp) */
extern BYTE pixel_buffer[144][160][4];

/* PPU-related variables (GBPPU.cpp) */
//extern unsigned short scanline_counter;




/***************************  Helper Functions ********************************/
// Cast LSB and MSB as a WORD (16 bits)
#define CASTWD(x, y) (((((WORD)x) << 8) & 0xFF00) | ((WORD)y) & 0x00FF)

// Decrement macro (note: this is not the actual DEC and INC opcode. See DECR, INCR)
#define DEC(x, y) WORD temp = (CASTWD(x,y) - 1) & 0xFFFF; \
                      x = (BYTE)((temp >> 8) & 0xFF); \
                      y = (BYTE)((temp) & 0xFF);

// Increment macro (note: this is not the actual DEC and INC opcode. See DECR, INCR)
#define INC(x, y) WORD temp = (CASTWD(x,y) + 1) & 0xFFFF; \
                      x = (BYTE)((temp >> 8) & 0xFF); \
                      y = (BYTE)((temp) & 0xFF);

// PUSH/POP to/from stack
#define PUSH(x, y)   --SP; GBCPU::writeByte(x, SP); --SP; GBCPU::writeByte(y, SP);         // PUSH stores MSB first, then LSB (little-endian CPU)
#define POP(x, y)    y = GBCPU::readByte(SP); ++SP; x = GBCPU::readByte(SP); ++SP;         // POP loads LSB first, then MSB

// Set and clear bits (used mainly for setting status register)
#define SETBIT(val, bit) (val |= (0x01 << bit))
#define CLRBIT(val, bit) (val &= ~(0x01 << bit))




/************************  Memory, Rates, & Bit Masking Macros ****************/

/* Clock cycle rates for CPU, PPU, etc. */
#define GAMEBOY_CLOCK_CYCLES 4194304/4 // The number of CPU clock cycles executed per second
#define GAMEBOY_FRAME_RATE   30      // The number of LCD frames rendered per second

#define GAMEBOY_CYCLES_FRAME   \
        GAMEBOY_CLOCK_CYCLES / GAMEBOY_FRAME_RATE // The number of cycles per frame


/*	CPU Address Space Definitions */
#define HRAM_END             0xFFFE  // High RAM ending address
#define HRAM_START           0xFF80  // High RAM, otherwise known as Zero Page beginning address

#define UNUSED_END           0xFEFF  // Unused memory region ending address
#define UNUSED_START         0xFEA0  // Unused memory region beginning address

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

/* Interrupt registers and masks */
#define INTERRUPT_ENABLE	 0xFFFF  // $FFFF - IE - Interrupt Enable (1 = Enabled) address
#define INTERRUPT_FLAG		 0xFF0F  // $FF0F - IF - Interrupt Flag - (1 = Requested) address
                                     // Bits 4 - 0: Joypad - Serial - Timer - LCD St - V-Blank (lower bit = high pri.)

/* Timer and DIV registers and masks */
#define TIMA        0xFF05       // Timer Counter (TIMA)
#define TMOD        0xFF06       // Timer Modulo (TMA)
#define TCON        0xFF07       // Timer Control (TAC)

#define DIV         0xFF04       // Divider Register (R/W)
#define DIV_RATE    16384        // Divider Register rate in Hz (1/16384 cycles/sec)

/* Joypad registers and masks */
#define JOYPAD_P1  0xFF00
#define JOYPAD_P15 0x20          //      |  [x]
#define JOYPAD_P14 0x10          //  [x] | 
#define JOYPAD_P13 0x08          // DOWN | START
#define JOYPAD_P12 0x04          // UP   | SELECT
#define JOYPAD_P11 0x02          // LEFT | B
#define JOYPAD_P10 0x01          // RIGHT| A

/* IO registers and masks */
#define SERIAL_XFER 0xFF01
#define SIO_CONTROL 0xFF02

// LCD OAM DMA Transfers
#define PPU_DMA             0xFF46 // DMA Transfer and Start Address


// JOYPAD_P1 Bit Masks



// PPU Macros

/* Macros for rendering */
#define VBLANK_START         144

#define VBLANK_END           153
#define MAX_SPRITES          40
#define MAX_SPRITES_PER_LINE 10

#define MAX_SPRITES_ROW      8
#define MAX_SPRITES_COL      16
#define MIN_SPRITES_ROW      8
#define MIN_SPRITES_COL      8


/* Macros for accessing LCD I/O and PPU Status registers */
#define LCDC        0xFF40       // LCD Control
/*
Bit 7 - LCD Display Enable             (0=Off, 1=On)
Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
Bit 5 - Window Display Enable          (0=Off, 1=On)
Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
Bit 3 - BG Tile Map Display Select     (0=9800-9BFF, 1=9C00-9FFF)
Bit 2 - OBJ (Sprite) Size              (0=8x8, 1=8x16)
Bit 1 - OBJ (Sprite) Display Enable    (0=Off, 1=On)
Bit 0 - BG Display (for CGB see below) (0=Off, 1=On)
*/

#define STAT        0xFF41       // LCDC Status
/*
Bit 6 - LYC = LY Coincidence Interrupt(1 = Enable) (Read / Write)
Bit 5 - Mode 2 OAM Interrupt(1 = Enable) (Read / Write)
Bit 4 - Mode 1 V - Blank Interrupt(1 = Enable) (Read / Write)
Bit 3 - Mode 0 H - Blank Interrupt(1 = Enable) (Read / Write)
Bit 2 - Coincidence Flag(0:LYC<>LY, 1 : LYC = LY) (Read Only)
Bit 1 - 0 - Mode Flag(Mode 0 - 3, see below) (Read Only)
0: During H - Blank
1 : During V - Blank
2 : During Searching OAM
3 : During Transferring Data to LCD Driver
*/


/*
    Sprite data byte 3 - Sprite Attributes/Flags
    Bit7   OBJ-to-BG Priority (0=OBJ Above BG, 1=OBJ Behind BG color 1-3)
    (Used for both BG and Window. BG color 0 is always behind OBJ)
    Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
    Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
    Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
    Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
    Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)
*/

/* LCD Positioning and Scrolling registers */
#define PPU_SCROLLY          0xFF42 // LCD Y display position 
#define PPU_SCROLLX          0xFF43 // LCD X display position

#define PPU_LY               0xFF44 // LCDC Y-Coordinate. 0 - 153. Writing to this value resets it.
#define PPU_LYC              0xFF45 // LCY - LY Compare

#define PPU_WY               0xFF4A // Window Y Position
#define PPU_WX               0xFF4B // Window X Position minus 7

/* DMG LCD Monochrome Palette registers*/
#define PPU_BGP              0xFF47 // Background Palette Data

/*  Bit 7-6 - Shade for Color Number 3
    Bit 5-4 - Shade for Color Number 2
    Bit 3-2 - Shade for Color Number 1
    Bit 1-0 - Shade for Color Number 0, where colors 0-3 = white, light gray, dark gray, black */
#define PPU_OBP0             0xFF48 // Object Palette 0 Data
#define PPU_OBP1             0xFF49 // Object Palette 1 Data

/* Color GameBoy only Palette registers*/
//#define PPU_BCPS 0xFF68 // Background Palette Index
//#define PPU_BCPD 0xFF69 // Background Palette Data
//#define PPU_OCPS 0xFF6A // Sprite Palette Index
//#define PPU_OCPD 0xFF6B // Sprite Palette Data



/* Color GameBoy Only LCD VRAM DMA Transfer registers */
//FF51 - HDMA1 - CGB Mode Only - New DMA Source, High
//FF52 - HDMA2 - CGB Mode Only - New DMA Source, Low
//FF53 - HDMA3 - CGB Mode Only - New DMA Destination, High
//FF54 - HDMA4 - CGB Mode Only - New DMA Destination, Low
//FF55 - HDMA5 - CGB Mode Only - New DMA Length / Mode / Start


#endif /* GAMEBOY_H_ */
