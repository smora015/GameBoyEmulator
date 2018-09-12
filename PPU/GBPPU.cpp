/*  Name:        GBPPU.cpp
    Author:      Sergio Morales [sergiomorales.me]
    Created:     October 19th, 2016
    Modified:    September 12th, 2018
    Description: This file contains the logic for rendering sprites and
                 tiles for the GameBoy Picture Processing Unit. */

#include "GBPPU.h"

// Counter that keeps track of the number of cycles occured to increment the next scanline
unsigned short scanline_counter = 0;


/* Function: void ExecutePPU(BYTE cycles, GBCPU & CPU)
             Executes picture processor unit functionality by
             rendering scanlines based on the number of CPU cycles that have
             currently passed since the last opcode executed. */
void ExecutePPU(BYTE cycles, GBCPU & CPU)
{
    /* TODO: Optimize ExecutePPU
             1) Create macros from cycles per scanline and register masks
             2) 

             */
    // We render 60 frames per second, therefore we need 4.194304 Mhz / 60 / 153 = 456 cycles per scaneline
    
    // Check and Update the status of the LCD through the LCD STAT register
    UpdateLCDStatus(CPU);

    // Only render scanlines if the LCD Display is enabled
    if (CPU.readByte(LCDC) & 0x80)
        scanline_counter += cycles;
    else
        return;

    if (scanline_counter >= 456)
    {
        if (CPU.readByte(PPU_LY) > VBLANK_END)
        {
            // Reset scanline if we're past 153
            CPU.writeByte(0, PPU_LY);
        }
        else if (CPU.readByte(PPU_LY) == VBLANK_START)
        {
            // Set V-Blank interrupt if we're at LY = 144
            CPU.writeByte(CPU.readByte(INTERRUPT_FLAG) | 0x01, INTERRUPT_FLAG);
            ++CPU.MEM[PPU_LY]; // write directly to avoid resetting to 0 thru writeByte function.
        }
        else if (CPU.readByte(PPU_LY) < VBLANK_START)
        {
            // Render scanline if we're within range
            RenderScanline(CPU);
            ++CPU.MEM[PPU_LY]; // write directly to avoid resetting to 0 thru writeByte function.
        }
        else
        {
            // V-Blank active [144 >= scanline <= 153]
            ++CPU.MEM[PPU_LY]; // write directly to avoid resetting to 0 thru writeByte function.
        }

        // We increment scanlines in each if branch because incrementing it here was skipping scanline 0 
        // Reset scanline cycles counter
        scanline_counter -= 456;
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

        return;
    }

    // Get flags to check if interrupts are enabled for LCD Status
    bool coincidence_interrupt = (CPU.MEM[STAT] & 0x40) ? true: false; // LYC == LY Compare Interrupt Enable
    bool OAM_interrupt = (CPU.MEM[STAT] & 0x20) ? true : false;        // Mode 2 Interrupt Enable
    bool VBlank_interrupt = (CPU.MEM[STAT] & 0x10) ? true: false;      // Mode 1 V-Blank interrupt Enable
    bool HBlank_interrupt = (CPU.MEM[STAT] & 0x08) ? true: false;      // Mode 0 H-Blank interrupt Enable

    // If current Status mode changed, request an interrupt if enabled
    BYTE stat_mode = (CPU.MEM[STAT] & 0x03); // 0 - HBlank, 1 - V Blank, 2 - Searching OAM, 3 - Xfer to LCD
    bool request_LCD_interrupt = false;

    // V-Blank
    if (CPU.MEM[PPU_LY] >= VBLANK_START)
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
        if (scanline_counter >= 80)
        {
            // Set current mode
            CPU.MEM[STAT] = (CPU.MEM[STAT] & 0xFC) | 2;

            if (OAM_interrupt == true)
                request_LCD_interrupt = true;
        }

        // Mode 3 - OAM/VRAM (data xfer to LCD) Period [happens between 169-175 clocks per 456 clocks]
        if (scanline_counter >= 172)
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

    // Request interrupt if entered a new mode and if interrupts were enabled for the mode
    if (((CPU.MEM[STAT] & 0x03) != stat_mode) && request_LCD_interrupt == true)
    {
        CPU.MEM[INTERRUPT_FLAG] |= 0x02;
    }

    // Determine Coincidence Flag and interrupt
    if (CPU.MEM[PPU_LYC] == CPU.MEM[PPU_LY])
    {
        CPU.MEM[STAT] |= 0x04;

        if (coincidence_interrupt == true)
        {
            // Set LCD STAT interrupt if requested by coincidence flag
            CPU.MEM[INTERRUPT_FLAG] |= 0x02;
        }
    }
    else
    {
        // Reset coincidence flag
        CPU.MEM[STAT] &= ~0x04;
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
    WORD data_addr = (bg_tile_data ? 0x8000 : 0x8800);

    // Render the Tiles if enabled
    bool bg_disp_en = (CPU.MEM[LCDC] & 0x01) ? true : false; // BG becomes white for GameBoy. Differs for SGB/GBC
    if (bg_disp_en)
    {
        bool bg_map_used = (CPU.MEM[LCDC] & 0x08) ? true : false; // 1 = 9C00-9FFF, 0 = 9800-9BFF. Location of BG tile #s to use
        WORD tile_addr = (bg_map_used ? 0x9C00 : 0x9800);
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
        WORD win_addr = (window_map_used ? 0x9C00 : 0x9800);
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
        bool use_8x16_sprite = (CPU.MEM[LCDC] & 0x04) ? true : false;
        RenderSprite(CPU, use_8x16_sprite);
    }
    else
    {
        // Sprite rendering not enabled
    }

}

void RenderTile(WORD loc_addr, WORD data_addr, GBCPU & CPU)
{
    // Get the current scanline (base y-coordinate) to render
    BYTE scanline = CPU.readByte(PPU_LY);
    
    // Determine true Y-coordinate for background to be rendered using the SCROLL position for the Y coordinate and scanline
    // Then, determine which tile row we're currently on based on the y-position. Divide by 8 because each tile is 8-pixels vertically, with 32 tiles per row
    BYTE tile_position_y = CPU.readByte(PPU_SCROLLY) + scanline;
    WORD tile_row_index = (BYTE(tile_position_y / 8)) * 32;

    // Render the 160 area based on what Scroll position and scanline we're in
    for (int px = 0; px < 160; ++px)
    {
        // Get the current x-position based on the Scroll-X coordinate.
        // Based off this position, determine which tile within the tile_row_index is to be used to render the current pixel (horizontally)
        BYTE tile_position_x = px + CPU.readByte(PPU_SCROLLX);       
        WORD tile_col_index = tile_position_x / 8;

        // Get the final tile # address using the indexes calculated and the base tile use address
        WORD tile_num_address = tile_col_index + tile_row_index + loc_addr; 

        // Read the tile number from the memory. Either as signed or unsigned offset depending on data address selected
        SIGNED_WORD tile_num;  // Multiplied by 16 because each tile takes 16-bytes to render 8x8 pixels
        WORD start_tile_address;     // The starting address for the 16 bytes to render the 8x8 pixels
        if (data_addr == 0x8800) // signed data
        {
            tile_num = (SIGNED_BYTE)CPU.readByte(tile_num_address);
            start_tile_address = data_addr + (tile_num + 128) * 16; // Add 128 to negate the signed offset. e.g #-128 would be $0*16 = $0 + $8800 = $8800. 
        }
        else
        {
            tile_num = (BYTE)CPU.readByte(tile_num_address);
            start_tile_address = data_addr + tile_num * 16;
        }

        // Determine which byte out of the 16-byte tile we are in, using the current y-position modulo 8 to get a 0-7 range. x2 because we need 2 bytes per tile
        WORD current_tile_address = start_tile_address + (tile_position_y % 8) * 2;

        // TODO: Implement Tile palette data

        // Use bit shifting and bitwise OR to get a 2-bit number using the bit at position tile_position
        pixel color = getRBG( ((((CPU.readByte(current_tile_address)     >> (7 - (tile_position_x % 8))) & 0x01) << 1) & 0x02) +
                                ((CPU.readByte(current_tile_address + 1) >> (7 - (tile_position_x % 8))) & 0x01));

        // Populate pixel buffer with the tile color. Only use px and scanline in this area because this is the "true" pixel location.
        pixel_buffer[scanline][px][1] = color.r;
        pixel_buffer[scanline][px][2] = color.g;
        pixel_buffer[scanline][px][3] = color.b;
    }

    return;
}

void RenderWindow(WORD loc_addr, WORD data_addr, GBCPU & CPU)
{
    // Render the window stored in memory
    BYTE scanline = CPU.readByte(PPU_LY);

    // Only render Window tiles if the scanline is within window position
    if (scanline < CPU.readByte(PPU_WY))
        return;

    // Determine true Y-coordinate for background to be rendered using the scanline (y-coordinate)
    // Then, determine which tile row we're currently on based on the y-position. Divide by 8 because each tile is 8-pixels vertically, with 32 tiles per row
    BYTE tile_position_y = scanline;
    WORD tile_row_index = (BYTE(tile_position_y / 8)) * 32;

    // Render the 160 area based on what Scroll position and scanline we're in
    for (int px = CPU.readByte(PPU_WX) - 7; px < 160; ++px)
    {
        // Get the current x-position based on the WINDOW-X coordinate.
        // Based off this position, determine which tile within the tile_row_index is to be used to render the current pixel (horizontally)
        BYTE tile_position_x = px;
        WORD tile_col_index = tile_position_x / 8;

        // Get the final tile # address using the indexes calculated and the base tile use address
        WORD tile_num_address = tile_col_index + tile_row_index + loc_addr;

        // Read the tile number from the memory. Either as signed or unsigned offset depending on data address selected
        SIGNED_WORD tile_num;  // Multiplied by 16 because each tile takes 16-bytes to render 8x8 pixels
        WORD start_tile_address;     // The starting address for the 16 bytes to render the 8x8 pixels
        if (data_addr == 0x8800) // signed data
        {
            tile_num = (SIGNED_BYTE)CPU.readByte(tile_num_address);
            start_tile_address = data_addr + (tile_num + 128) * 16; // Add 128 to negate the signed offset. e.g #-128 would be $0*16 = $0 + $8800 = $8800. 
        }
        else
        {
            tile_num = (BYTE)CPU.readByte(tile_num_address);
            start_tile_address = data_addr + tile_num * 16;
        }

        // Determine which byte out of the 16-byte tile we are in, using the current y-position modulo 8 to get a 0-7 range. x2 because we need 2 bytes per tile
        WORD current_tile_address = start_tile_address + (tile_position_y % 8) * 2;

        // TODO: Implement Window palette data

        // Use bit shifting and bitwise OR to get a 2-bit number using the bit at position tile_position
        pixel color = getRBG( ((((CPU.readByte(current_tile_address)     >> (7 - (tile_position_x % 8))) & 0x01) << 1) & 0x02) +
                                ((CPU.readByte(current_tile_address + 1) >> (7 - (tile_position_x % 8))) & 0x01));

        // Populate pixel buffer with the tile color. Only use px and scanline in this area because this is the "true" pixel location.
        pixel_buffer[scanline][px][1] = color.r;
        pixel_buffer[scanline][px][2] = color.g;
        pixel_buffer[scanline][px][3] = color.b;
    }

    return;
}

void RenderSprite(GBCPU & CPU, bool use_8X16)
{
    // Render the sprite (OBJ) sotred in memory $8000 - $8FFF
    WORD loc_addr = 0x8000; // use unsigned numbering
    
    // Get the current scanline we're in
    BYTE scanline = CPU.readByte(PPU_LY);

    // Loop through Sprite Attribute memory to render sprites on current scanline
    for (BYTE sprite = 0; sprite < 40; ++sprite)
    {
        // Get sprite attribute information from the current index
        WORD sprite_addr = sprite * 4 + SPRITE_TABLE_START;

        // Get Y-position-16, X-position-8, Tile/Pattern #, and the Attribute/Flag
        BYTE sprite_y_position = CPU.readByte(sprite_addr) - 16;
        BYTE sprite_x_position = CPU.readByte(sprite_addr + 1) - 8;
        BYTE tile_num_index    = CPU.readByte(sprite_addr + 2);     // Multiplied by 16 because each 8x8 tile takes 16 bytes
        BYTE sprite_attribute  = CPU.readByte(sprite_addr + 3);

        // Check if we need to flip sprite along y-axis
        bool y_flip = (sprite_attribute & 0x04) ? true: false;
        bool x_flip = (sprite_attribute & 0x02) ? true: false;

        // Only render sprite if it falls in current scanline
        if ((scanline >= sprite_y_position) && 
            (scanline < sprite_y_position + (use_8X16 ? 16 : 8)) )
        {
            // Get the current byte of the sprite to render
            BYTE tile_num_y_offset = (scanline - sprite_y_position);
            if(y_flip)
            {
                // Mirror the sprite vertically if y_flip attribute is present
                tile_num_y_offset = (use_8X16 ? 16 : 8) - tile_num_y_offset;
            }

            // Get the index to the tile address through the base address, tile #, and the current horizontal line (*2 because each line is 2 bytes)
            WORD tile_addr = loc_addr + tile_num_index * 16 + tile_num_y_offset * 2; 
            BYTE tile1 = CPU.readByte(tile_addr);
            BYTE tile2 = CPU.readByte(tile_addr + 1);

            // Loop through the 2 bytes of data bit-by-bit, accounting 
            for (int x = 0; x < 8; ++x)
            {
                // TODO: Implement Sprite palette data

                // Use bit shifting and bitwise OR to get a 2-bit number using the bit at position tile_position
                pixel color = getRBG( ( ((tile1 >> (x_flip ? x : (7 - x))) & 0x01) << 1) +
                                        ((tile2 >> (x_flip ? x : (7 - x))) & 0x01));

                // Sprite pixels are transparent instead of white
                if (color.r == 255)
                    continue;

                // Populate pixel buffer with the tile color. Only use px and scanline in this area because this is the "true" pixel location.
                pixel_buffer[scanline][sprite_x_position + x][1] = color.r;
                pixel_buffer[scanline][sprite_x_position + x][2] = color.g;
                pixel_buffer[scanline][sprite_x_position + x][3] = color.b;

            }
        }
    }

    return;
}


pixel getRBG(BYTE value)
{
    pixel t;
    switch (value)
    {
    case 0:
        t.r = 255;
        t.b = 255;
        t.g = 255;
        break;
    case 1:
        t.r = 60;
        t.b = 60;
        t.g = 60;
        break;
    case 2:
        t.r = 125;
        t.b = 125;
        t.g = 125;
        break;
    case 3:
        t.r = 0;
        t.b = 0;
        t.g = 0;
        break;
    default:
        t.r = 255;
        t.b = 0;
        t.g = 0;
        break;
    }

    return t;
}


/* DEBUG FUNCTION: TestVideoRAM(GBCPU & CPU)
                   Fill video RAM with a temporary tile to ensure
                   tile rendering is working properly. */
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
        CPU.MEM[i] = (SIGNED_BYTE)-128;
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