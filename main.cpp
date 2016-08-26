// Standard Libraries
#include <iostream>

// Game Boy libraries
#include "gameboy.h"		// Data and Macro definitions
#include "render.h"			// Graphics Rendering library
#include "GBCartridge.h"	// ROM Cartridge library
#include "GBCPU.h"			// Game Boy CPU library
#include "GBPPU.h"			// Game Boy PPU library

using namespace std;

int main(int argc, char **argv)
{
	// Instantiate classes
	GBCartridge ROM = GBCartridge("");
	GBCPU CPU = GBCPU();
    CPU.execute();

    //cout << "Finished executing instructions..." << endl;

	//while (1);
	/* Implementation priorities (1 == do first, 2+ == do after, from top-bottom order)
		1- Cartridge (all)
		
		1- CPU Data initialization
		2- CPU Instruction execution
		3- CPU to PPU interactions

		2- PPU Data initialization
		3- PPU Sprite calculations

		3- Graphic rendering

		3- Audio
	*/


	//
	// Initialize/configure OpenGL Rendering function, then run main rendering loop.
	//GLInit(&argc, argv);
	//glColor3f(GLfloat(0.5), GLfloat(0.7), GLfloat(0.2) );
	//glutMainLoop();

	//
  return 0;

}