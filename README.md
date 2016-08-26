# GameBoyEmulator
A Game Boy emulator implemented from scratch in C++. The following explains the different subdirectories:


## CPU
The main guts of the GameBoy. These files contain the logic that parse and execute instructions directly from the raw data of the game cartridge. There are dozens of different types of instructions, but with many variants of them, totaling over a hundred different op codes. The opcodes are consolidated into an array of function calls so that whenever an instruction is executed, the 2-byte op code will be the index to the function's location, causing it to execute. This is a more optimized approach, compared to creating a giant switch case or tons of if-elseif statements.

## PPU
The picture processing unit of the GameBoy. These files contain the logic that decodes the PRG and CHR ROM data to enable the rendering the sprite and tile data, with direction from the CPU.

## ROM Loading
These files contain the logic that parses through the cartridge (ROM) information and partitions the data according to the format.

## Graphics
OpenGL is utilized to enable rendering on Windows. Not yet tested on other formats, but FreeGLUT is used so hopefully it will be cross-platform.

## References

- GameBoy CPU Manual - http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
- Google

