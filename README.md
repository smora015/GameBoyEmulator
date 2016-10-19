# GameBoyEmulator
A Game Boy emulator implemented from scratch in C++. The following explains the different subdirectories:

## CPU
The main guts of the GameBoy. These files contain the logic that parse and execute instructions directly from the raw data of the game cartridge. There are dozens of different types of instructions, but with many variants of them, totaling over a hundred different op codes. The opcodes are consolidated into an array of function calls so that whenever an instruction is executed, the 2-byte op code will be the index to the function's location, causing it to execute. This is a more optimized approach, compared to creating a giant switch case or tons of if-elseif statements.

### Features
- Supports all opcodes
- Supports all CB-prefix Opcodes 
- Supports all interrupt routines (not fully tested)
- Supports Timer and Divider counting (not fully tested)
- Compatibility with MBC1 cartridges

#### To Do
##### CPU Memory Access (Memory Bank Controller logic)
- Finish up remaining MBC cartridge compatability
 
##### CPU Opcodes
- Clean up opcode helper functions
- Add comments for all opcodes to describe functionality
- Finish up opcodes $F8, $08, and $10 to update the flags correctly

## PPU
The picture processing unit of the GameBoy. These files contain the logic that decodes the ROM data to enable the rendering the sprite and tile data, with direction from the CPU.

### Features
- Currently being implemented

## ROM Loading
These files contain the logic that parses through the cartridge (ROM) information and partitions the data according to the format. Files are read either from commandline input- if none specified, it will default to a ROM that is hardcoded.

### Features
- Supports .gb ROM file format
- Allocates external ROM and RAM banks depending on MBC type definitions in the header

## Graphics
These files contain the functions used for rendering pixels onto the screen. Simple Direct-media Layer (SDL) is utilized for video rendering, input, and sound (TBD.) A 256x256 pixel buffer is used to display all pixels onto the window.

## References
- The GameBoy Pan docs
- GameBoy CPU Manual - http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
- GameBoy Opcodes Summary - http://pastraiser.com/cpu/gameboy/gameboy_opcodes.html
- Codeslinger's GameBoy emulation  site - http://www.codeslinger.co.uk/pages/projects/gameboy.html

