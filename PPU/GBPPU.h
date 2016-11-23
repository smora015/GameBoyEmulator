#ifndef GBPPU_H
#define GBPPU_H

#include "gameboy.h"
#include "GBCPU.h"
#include "render.h"

extern unsigned short scanline_counter;

void ExecutePPU(byte cycles, GBCPU & CPU);
void UpdateLCDStatus(GBCPU & CPU);
void RenderScanline(GBCPU & CPU);
void RenderTile(word loc_addr, word data_addr, GBCPU & CPU);
void RenderWindow(word loc_addr, word data_addr, GBCPU & CPU);
void RenderSprite(GBCPU & CPU, bool use_8X16);
struct pixel getRBG(byte value);
void TestVideoRAM(GBCPU & CPU);

/* Macros for accessing LCD I/O and Status registers */
#define MAX_SPRITES          40
#define MAX_SPRITES_PER_LINE 10

#define MAX_SPRITES_ROW      8
#define MAX_SPRITES_COL      16
#define MIN_SPRITES_ROW      8
#define MIN_SPRITES_COL      8

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
// LCD Positioning and Scrolling
#define PPU_SCROLLY          0xFF42 // LCD Y display position 
#define PPU_SCROLLX          0xFF43 // LCD X display position

#define PPU_LY               0xFF44 // LCDC Y-Coordinate. 0 - 153. Writing to this value resets it.
#define PPU_LYC              0xFF45 // LCY - LY Compare

#define PPU_WY               0xFF4A // Window Y Position
#define PPU_WX               0xFF4B // Window X Position minus 7

// LCD Monochrome Palettes
#define PPU_BGP              0xFF47 // Background Palette Data
/*
    Bit 7-6 - Shade for Color Number 3
    Bit 5-4 - Shade for Color Number 2
    Bit 3-2 - Shade for Color Number 1
    Bit 1-0 - Shade for Color Number 0, where colors 0-3 = white, light gray, dark gray, black
*/

#define PPU_OBP0             0xFF48 // Object Palette 0 Data
#define PPU_OBP1             0xFF49 // Object Palette 1 Data

// Color GameBoy only
//#define PPU_BCPS 0xFF68 // Background Palette Index
//#define PPU_BCPD 0xFF69 // Background Palette Data
//#define PPU_OCPS 0xFF6A // Sprite Palette Index
//#define PPU_OCPD 0xFF6B // Sprite Palette Data



// LCD VRAM DMA Transfers - Color GameBoy Only
//FF51 - HDMA1 - CGB Mode Only - New DMA Source, High
//FF52 - HDMA2 - CGB Mode Only - New DMA Source, Low
//FF53 - HDMA3 - CGB Mode Only - New DMA Destination, High
//FF54 - HDMA4 - CGB Mode Only - New DMA Destination, Low
//FF55 - HDMA5 - CGB Mode Only - New DMA Length / Mode / Start

#endif /* GBPPU.h */
