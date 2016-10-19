#ifndef _GBPPU_H_
#define _GBPPU_H_

#include "gameboy.h"
#include "GBCPU.h"

void TestVideoRAM();

void RenderTile(word addr);


void ExecutePPU(byte cycles);


/* Macros for accessing LCD I/O and Status registers */
#define PPU_LCDC             0xFF40 // LCD Control (R/W)
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

#define PPU_LCDSTAT          0xFF41 // LCD Status (R/W
/*
    Bit 6 - LYC=LY Coincidence Interrupt (1=Enable) (Read/Write)
    Bit 5 - Mode 2 OAM Interrupt         (1=Enable) (Read/Write)
    Bit 4 - Mode 1 V-Blank Interrupt     (1=Enable) (Read/Write)
    Bit 3 - Mode 0 H-Blank Interrupt     (1=Enable) (Read/Write)
    Bit 2 - Coincidence Flag  (0:LYC<>LY, 1:LYC=LY) (Read Only)
    Bit 1-0 - Mode Flag       (Mode 0-3, see below) (Read Only)
    0: During H-Blank
    1: During V-Blank
    2: During Searching OAM
    3: During Transferring Data to LCD Driver
*/

#define PPU_SCROLLY          0xFF42 // LCD Y display position 
#define PPU_SCROLLX          0xFF43 // LCD X display position

#define PPU_LY               0xFF44 // LCDC Y-Coordinate. 0 - 153. Writing to this value resets it.
#define PPU_LX               0xFF45 // LCDC X-Coordinate
#endif /* GBPPU.h */
