# GameBoyEmulator
A simple Game Boy emulator implemented from scratch in C++ using Simple DirectMedia Layer (SDL) 2 library, and Visual Studio 2017. This project is meant to be more of a learning experience to teach myself more about emulation and graphics rendering on Windows. My hope is to be able to port this project onto other platforms (such as Android or iOS) once fully complete.

## Table of Contents
- [Progress](#progress)
  * [Known Issues with emulator](#known-issues)
  * [To Dos for improving emulator](#to-dos)  
- [CPU](#cpu)
- [PPU](#ppu)
- [ROM Loading](#rom-loading)
- [Graphics](#graphics)
- [References](#references)


## Progress

Currently able to load intro screens on ROM_ONLY MBC1 type ROMs such as Tetris:

![Tetris Intro Loaded!](http://sergiomorales.me/assets/images/gb/tetris_intro.PNG)

Most CPU opcodes have been verified through Blargg's trusty  test ROMs:

![Blargg's cpu_instrs.gb test rom](http://sergiomorales.me/assets/images/gb/cpu_instrs.png)
![Blargg's 01-special.gb test rom](http://sergiomorales.me/assets/images/gb/01-special.png)
![Blargg's 01-special.gb test rom](http://sergiomorales.me/assets/images/gb/04-op_r,imm.png)
![Blargg's 01-special.gb test rom](http://sergiomorales.me/assets/images/gb/09-op_r,r.png)

_Not pictured- 02-interrupts.gb is failing on the timer test._

### Known Issues
- Interrrupts: Timer interrupt is not counting up the TMA counter properly. This is causing 01 test to hang.
- CPU: Need to finish up opcodes $F8, $08, and $10 to update the flags correctly
- CPU: All ADC and SUBC opcodes are causing 04 and 09 tests to fail. Need to investigate what is not correct.


### To Dos
- Cartridge: Need to verify the more complex MBC types.
- PPU: Only tile rendering has been verified through Blargg's cpu_instrs.gb rom. Sprite rendering and scrolling has not.


## CPU
The main guts of the GameBoy. These files contain the logic that parse and execute instructions directly from the raw data of the game cartridge. There are dozens of different types of instructions, but with many variants of them, totaling over a hundred different op codes. The opcodes are consolidated into an array of function calls so that whenever an instruction is executed, the 2-byte op code will be the index to the function's location, causing it to execute. This is a more optimized approach, compared to creating a giant switch case or tons of if-elseif statements.


## PPU
The picture processing unit of the GameBoy. These files contain the logic that decodes the ROM data to enable the rendering the sprite and tile data, with direction from the CPU.


## ROM Loading
These files contain the logic that parses through the cartridge (ROM) information and partitions the data according to the format. Files are read either from commandline input- if none specified, it will default to a ROM that is hardcoded.

Supports .gb ROM file format. Program currently must be launched via command prompt in order to specify the ROM name as an argument, otherwise default ROM (hard-coded) will be loaded. Once emulator is fully implemented, a simple GUI will be created to launch files from browser.


## Graphics
These files contain the functions used for rendering pixels onto the screen. Simple Direct-media Layer (SDL) is utilized for video rendering, input, and sound (TBD.) A 256x256 pixel buffer is used to display all pixels onto the window.

## References
- The GameBoy Pan docs
- GameBoy CPU Manual - http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
- GameBoy Opcodes Summary - http://pastraiser.com/cpu/gameboy/gameboy_opcodes.html
- Codeslinger's GameBoy emulation  site - http://www.codeslinger.co.uk/pages/projects/gameboy.html

