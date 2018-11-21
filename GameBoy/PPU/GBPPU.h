#ifndef GBPPU_H
#define GBPPU_H

#include "GBCPU.h"
#include "render.h"

void ExecutePPU(BYTE cycles, GBCPU & CPU);
void UpdateLCDStatus(GBCPU & CPU);
void RenderScanline(GBCPU & CPU);
void RenderTile(WORD loc_addr, WORD data_addr, GBCPU & CPU);
void RenderWindow(WORD loc_addr, WORD data_addr, GBCPU & CPU);
void RenderSprite(GBCPU & CPU, bool use_8X16);
struct pixel getRBG(BYTE value);

void TestVideoRAM(GBCPU & CPU);


#endif /* GBPPU.h */
