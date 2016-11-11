/*
    Name:        GBPPU.cpp
    Author:      Sergio Morales
    Date:        10/19/2016
    Description: This file contains the logic for rendering sprites and
                 tiles for the GameBoy Picture Processing Unit.
*/
#include "GBPPU.h"
#include "gameboy.h"
#include "GBCPU.h"
#include "render.h"

unsigned short scanline_counter = 0;

// TODO: Utilize palette colors for rendering
// TODO: Window/Sprite Rendering

void ExecutePPU(byte cycles, GBCPU & CPU)
{
    // We render 60 frames per second, therefore we need 4.194304 Mhz / 60 / 153 = 456 cycles per scaneline
    
    // Check and Update the status of the LCD through the LCD STAT register
    UpdateLCDStatus(CPU);

    // Only render scanlines if the LCD Display is enabled
    if( CPU.MEM[LCDC] & 0x80)
        scanline_counter += cycles;

    if (scanline_counter >= 456)
    {
        // Reset scanline if we're past 153
        if (CPU.MEM[PPU_LY] > 153)
            CPU.MEM[PPU_LY] = 0;
    
        // Set V-Blank interrupt if we're at LY = 144
        else if (CPU.MEM[PPU_LY] == 144)
            CPU.MEM[INTERRUPT_FLAG] |= 0x01;
        
        // Render scanline if we're within range
        else if( CPU.MEM[PPU_LY] < 144)
            RenderScanline(CPU);

        // Reset scanline cycles counter
        scanline_counter = 0;
    }


    return;
}

void UpdateLCDStatus(GBCPU & CPU)
{
    // Reset flags if the LCD is disabled
    if ((CPU.MEM[LCDC] & 0x80) == 0x00)
    {
        scanline_counter = 0; // Reset Scanline cycles counter
        CPU.MEM[PPU_LY] = 0;  // Reset Y-Coordinate
        CPU.MEM[STAT] = ((CPU.MEM[STAT] & 0xFC) | 0x01); // Set mode to V-Blank
    }

    // Get flags to check if interrupts are enabled for LCD Status
    bool coincidence_interrupt = (CPU.MEM[STAT] & 0x40) ? true: false; // LYC == LY Compare Interrupt Enable
    bool OAM_interrupt = (CPU.MEM[STAT] & 0x20) ? true : false;        // Mode 2 Interrupt Enable
    bool VBlank_interrupt = (CPU.MEM[STAT] & 0x10) ? true: false;      // Mode 1 V-Blank interrupt Enable
    bool HBlank_interrupt = (CPU.MEM[STAT] & 0x08) ? true: false;      // Mode 0 H-Blank interrupt Enable

    // If current Status mode changed, request an interrupt if enabled
    byte stat_mode = (CPU.MEM[STAT] & 0x03); // 0 - HBlank, 1 - V Blank, 2 - Searching OAM, 3 - Xfer to LCD
    bool request_LCD_interrupt = false;

    // V-Blank
    if (CPU.MEM[PPU_LY] >= 144)
    {
        // Set current mode
        CPU.MEM[STAT] = (CPU.MEM[STAT] & 0xFC) | 1;

        if (VBlank_interrupt == true)
            request_LCD_interrupt = true;
    }
    // H-Blank/
    else
    {
        // Mode 2 - OAM Period [happens between 77-83 clocks per 465 clocks]
        if (CPU.MEM[PPU_LY] >= 80)
        {
            // Set current mode
            CPU.MEM[STAT] = (CPU.MEM[STAT] & 0xFC) | 2;

            if (OAM_interrupt == true)
                request_LCD_interrupt = true;
        }

        // Mode 3 - OAM/VRAM (data xfer to LCD) Period [happens between 169-175 clocks per 456 clocks]
        if (CPU.MEM[PPU_LY] >= 172)
        {
            // Set current mode
            CPU.MEM[STAT] = (CPU.MEM[STAT] & 0xFC) | 3;
        }

        // Mode 0 - H-Blank Period [happens between 201-207 clocks per 456 clocks]
        else
        {
            // Set current mode
            CPU.MEM[STAT] = (CPU.MEM[STAT] & 0xFC);

            if(HBlank_interrupt == true)
                request_LCD_interrupt = true;
        }
    }


    // Determine Coincidence Flag if enabled
    if (CPU.MEM[PPU_LYC] == CPU.MEM[PPU_LY])
    {
        CPU.MEM[STAT] |= 0x04;

        if (coincidence_interrupt == true)
            request_LCD_interrupt = true;
    }
    else
    {
        CPU.MEM[STAT] &= ~0x04;
    }

    // Set LCD STAT interrupt if requested by coincidence flag, V blank, H blank, or OAM/VRAM period
    if (request_LCD_interrupt == true)
    {
        CPU.MEM[INTERRUPT_FLAG] |= 0x02;
    }
}

void RenderScanline(GBCPU & CPU)
{
    /*
    Tile Data is stored in VRAM at addresses 8000h-97FFh, this area defines the Bitmaps for 192 Tiles. In CGB Mode 384 Tiles can be defined, because memory at 0:8000h-97FFh and at 1:8000h-97FFh is used.
    Each tile is sized 8x8 pixels and has a color depth of 4 colors/gray shades. Tiles can be displayed as part of the Background/Window map, and/or as OAM tiles (foreground sprites). Note that foreground sprites may have only 3 colors, because color 0 is transparent.
    As it was said before, there are two Tile Pattern Tables at $8000-8FFF and at $8800-97FF. The first one can be used for sprites and the background. Its tiles are numbered from 0 to 255. The second table can be used for the background and the window display and its tiles are numbered from -128 to 127.
    Each Tile occupies 16 bytes, where each 2 bytes represent a line:

    Byte 0-1  First Line (Upper 8 pixels)
    Byte 2-3  Next Line
    etc.

    For each line, the first byte defines the least significant bits of the color numbers for each pixel, and the second byte defines the upper bits of the color numbers. In either case, Bit 7 is the leftmost pixel, and Bit 0 the rightmost.
    So, each pixel is having a color number in range from 0-3. The color numbers are translated into real colors (or gray shades) depending on the current palettes. The palettes are defined through registers FF47-FF49 (Non CGB Mode), and FF68-FF6B (CGB Mode).
    */

    // Get LCD Control flags      
    bool bg_tile_data = (CPU.MEM[LCDC] & 0x10) ? true : false; // 1 = 8000-8FFF, 0 = 8800-97FF. Location of data bytes. NOTE: if using 8800-97FF, used signed btyes
    word data_addr = (bg_tile_data ? 0x8000 : 0x8800);

    // Render the Tiles if enabled
    bool bg_disp_en = (CPU.MEM[LCDC] & 0x01) ? true : false; // BG becomes white for GameBoy. Differs for SGB/GBC
    if (bg_disp_en)
    {
        bool bg_map_used = (CPU.MEM[LCDC] & 0x08) ? true : false; // 1 = 9C00-9FFF, 0 = 9800-9BFF. Location of BG tile #s to use
        word tile_addr = (bg_map_used ? 0x9C00 : 0x9800);
        RenderTile(tile_addr, data_addr, CPU);
    }
    else
    {
        // Tile rendering not enabled
    }

    // Render the Window if enabled
    bool window_display_en = (CPU.MEM[LCDC] & 0x20) ? true : false;
    if( window_display_en )
    {
        bool window_map_used = (CPU.MEM[LCDC] & 0x40) ? true : false; // 1 = 9C00-9FFF, 0 = 9800-9BFF. Location of Window tile #s to use    
        word win_addr = (window_map_used ? 0x9C00 : 0x9800);
        RenderWindow(win_addr, data_addr, CPU);
    }
    else
    {
        // Window rendering not enabled
    }

    // Render the Sprites if enabled
    bool sprite_disp_en = (CPU.MEM[LCDC] & 0x02) ? true : false;
    if (sprite_disp_en)
    {
        // Sprite rendering not enabled
        bool use_8x16_sprite = (CPU.MEM[LCDC] & 0x04) ? true : false;
        word sprite_addr = 0x0000;
        RenderSprite(sprite_addr, data_addr, CPU);
    }
    else
    {
        // Sprite rendering not enabled
    }


}

void RenderTile(word loc_addr, word data_addr, GBCPU & CPU)
{
    // Get the current scanline (base y-coordinate) to render
    byte scanline = CPU.readByte(PPU_LY);
    
    // Determine true Y-coordinate for background to be rendered using the SCROLL position for the Y coordinate and scanline
    // Then, determine which tile row we're currently on based on the y-position. Divide by 8 because each tile is 8-pixels vertically, with 32 tiles per row
    byte tile_position_y = CPU.readByte(PPU_SCROLLY) + scanline;
    word tile_row_index = (byte(tile_position_y / 8)) * 32;

    // Render the 160 area based on what Scroll position and scanline we're in
    for (int px = 0; px < 160; ++px)
    {
        // Get the current x-position based on the Scroll-X coordinate.
        // Based off this position, determine which tile within the tile_row_index is to be used to render the current pixel (horizontally)
        byte tile_position_x = px + CPU.readByte(PPU_SCROLLX);       
        word tile_col_index = tile_position_x / 8;

        // Get the final tile # address using the indexes calculated and the base tile use address
        word tile_num_address = tile_col_index + tile_row_index + loc_addr; 

        // Read the tile number from the memory. Either as signed or unsigned offset depending on data address selected
        signed_word tile_num;  // Multiplied by 16 because each tile takes 16-bytes to render 8x8 pixels
        word start_tile_address;     // The starting address for the 16 bytes to render the 8x8 pixels
        if (data_addr == 0x8800) // signed data
        {
            tile_num = (signed_byte)CPU.readByte(tile_num_address);
            start_tile_address = data_addr + (tile_num + 128) * 16; // Add 128 to negate the signed offset. e.g #-128 would be $0*16 = $0 + $8800 = $8800. 
        }
        else
        {
            tile_num = (byte)CPU.readByte(tile_num_address);
            start_tile_address = data_addr + tile_num * 16;
        }

        // Determine which byte out of the 16-byte tile we are in, using the current y-position modulo 8 to get a 0-7 range. x2 because we need 2 bytes per tile
        word current_tile_address = start_tile_address + (tile_position_y % 8) * 2;

        // Use bit shifting and bitwise OR to get a 2-bit number using the bit at position tile_position
        pixel color = getRBG( ((CPU.readByte(current_tile_address) >> (6 - (tile_position_x % 8))) & 0x02) +
                              ((CPU.readByte(current_tile_address + 1) >> (7 - (tile_position_x % 8))) & 0x01));

        // Populate pixel buffer with the tile color. Only use px and scanline in this area because this is the "true" pixel location.
        pixel_buffer[px][scanline] = color;
    }

    return;
}

void RenderWindow(word loc_addr, word data_addr, GBCPU & CPU)
{
    // Render the window stored in memory


    byte window_posX = CPU.readByte(PPU_WX);
    byte window_posY = CPU.readByte(PPU_WY);

    // The location #s are unsigned if they come from $8000, otherwise they're unsigned
    byte tile_num = CPU.readByte(data_addr);

    return;

}

void RenderSprite(word loc_addr, word data_addr, GBCPU & CPU)
{
    // Render the sprite (OBJ) sotred in memory

    return;
}


pixel getRBG(byte value)
{
    pixel t;
    switch (value)
    {
    case 0:
        t.r = 0;
        t.b = 0;
        t.g = 0;
        break;
    case 1:
        t.r = 255;
        t.b = 255;
        t.g = 255;
        break;
    case 2:
        t.r = 255;
        t.b = 255;
        t.g = 255;
        break;
    case 3:
        t.r = 255;
        t.b = 255;
        t.g = 255;
        break;
    default:
        t.r = 255;
        t.b = 0;
        t.g = 100;
        break;
    }

    return t;
}


void TestVideoRAM(GBCPU & CPU)
{
    int i = 0x8800;
    // file tile data with all As
    CPU.MEM[i++] = 0x7C;
    CPU.MEM[i++] = 0x7C;
    CPU.MEM[i++] = 0xC6;
    CPU.MEM[i++] = 0xC6;
    CPU.MEM[i++] = 0xC6;
    CPU.MEM[i++] = 0xC6;
    CPU.MEM[i++] = 0xFE;
    CPU.MEM[i++] = 0xFE;
    CPU.MEM[i++] = 0xC6;
    CPU.MEM[i++] = 0xC6;
    CPU.MEM[i++] = 0xC6;
    CPU.MEM[i++] = 0xC6;
    CPU.MEM[i++] = 0xC6;
    CPU.MEM[i++] = 0xC6;
    CPU.MEM[i++] = 0x00;
    CPU.MEM[i++] = 0x00;


    for (int i = 0x9C00; i < 0x9FFF; ++i)
    {
        // file tile #s with all As (tile #0)
        CPU.MEM[i] = (signed_byte)-128;
    }

    // trim off half of top left corner
    CPU.writeByte(4, PPU_SCROLLY);
    CPU.writeByte(4, PPU_SCROLLX);

    for (int i = 0; i < 145; ++i)
    {
        CPU.MEM[PPU_LY] = i; // scanline
        RenderTile(0x9C00, 0x8800, CPU);
    }
    return;
}