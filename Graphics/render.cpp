#include "render.h"
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <stdio.h>
//#include <GL/glut.h>

// Renders a quad at cell (x, y) with dimensions CELL_LENGTH
void renderPixel(int x, int y)
{
	glBegin(GL_POINTS);

	// Render a point based off a vertex
	glVertex2f(GLfloat(x), GLfloat(y));

	glEnd();
}

// Output functon to OpenGL Buffer
void GL_render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(GLfloat(0.5), GLfloat(0.7), GLfloat(1.0));
	renderPixel(200, 100);
	renderPixel(201, 100);
	renderPixel(202, 100);
	renderPixel(203, 100);
	renderPixel(204, 100);
	renderPixel(206, 100);
	renderPixel(207, 100);
	renderPixel(208, 100);
	renderPixel(208, 100);
	renderPixel(209, 100);
	renderPixel(210, 100);
	renderPixel(211, 100);
	renderPixel(212, 100);

	glutSwapBuffers();
}


//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);

	// Create window
	glutCreateWindow("GameBoy Emulator (BETA)");

	// NEW: Register rendering function
	glutDisplayFunc(GL_render);

	// The default view coordinates is (-1.0, -1.0) bottom left & (1.0, 1.0) top right.
	// For the purposes of this lab, this is set to the number of pixels
	// in each dimension.
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(0.0, 800, 0.0, 800, -1.0, 1.0);
}