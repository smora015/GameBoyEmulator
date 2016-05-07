#ifndef RENDER_H_
#define RENDER_H_


#include "gameboy.h"

// OpenGL Libraries
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <math.h>

// Renders a pixel at point <X, Y>
void renderPixel(int x, int y);

// Output functon to OpenGL Buffer
void GL_render();

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv);

#endif /* render.h */
