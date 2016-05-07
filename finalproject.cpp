#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>
using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>
#include <SDL.h>
#include <math.h>
// Need Math Defines before cmath to use Math constants
#define _USE_MATH_DEFINES
#include <cmath>

#include <unistd.h>


/* Global variables */
char title[] = "DISCO PARTY CAT PONG";
// Window display size
GLsizei winWidth = 800, winHeight = 600;
// Time for animation
GLfloat t = 0.0f;

// Flag for pausing
bool paws = true;


chrono::duration<long,ratio<1,10>> DELTA_T(1);
auto start = std::chrono::high_resolution_clock::now();

float random(float min, float max)
{
    float random = ((float) rand()) / (float) RAND_MAX;
    float range = max - min;  
    return random * range + min;
}



/* Initialize OpenGL Graphics */
void init() {
	// Get and display your OpenGL version
	const GLubyte *Vstr;
	Vstr = glGetString(GL_VERSION);
	fprintf(stderr, "Your OpenGL version is %s\n", Vstr);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Maximize depth display
	glClearDepth(1.0f);
	// Enable depth testing for z-culling
	glEnable(GL_DEPTH_TEST);
	// Set the type of depth-test
	glDepthFunc(GL_LEQUAL);
	// Provide smooth shading
	glShadeModel(GL_SMOOTH);
}

void transform(void)
{
	auto now = std::chrono::high_resolution_clock::now();
	if (now - start < DELTA_T)
		return;

	start = now;
	t += 0.1;


	glutPostRedisplay();
}

// Display Function
void displayfcn() {
	// Clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Model view matrix mode
	glMatrixMode(GL_MODELVIEW);

	// Reset the model-view matrix
	// This is done each time you need to clear the matrix operations
	// This is similar to glPopMatrix()
	glLoadIdentity();


	// Double buffering
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
        switch (key) {
        case ' ':
		paws = !paws;
		if (paws)
			glutIdleFunc(NULL);
		else
			glutIdleFunc(transform);
                break;
        default:
                break;
        }
}

void specialKeys(unsigned char key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		break;
	case GLUT_KEY_RIGHT:
		break;
	default:
		break;
	}
}

// Windows redraw function
void winReshapeFcn(GLsizei width, GLsizei height) {
	// Compute aspect ratio of the new window
	if (height == 0)
		height = 1;
	GLfloat aspect = (GLfloat)width / (GLfloat)height;

	// Set the viewport
	// Allows for resizing without major display issues
	glViewport(0, 0, width, height);

	// Set the aspect ratio of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Enable perspective projection with fovy, aspect, zNear and zFar
	// This is the camera view and objects align with view frustum
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	// Set the window's initial width & height
	glutInitWindowSize(winWidth, winHeight);
	// Position the window's initial top-left corner
	glutInitWindowPosition(50, 50);
	// Create window with the given title
	glutCreateWindow(title);
	// Display Function
	glutDisplayFunc(displayfcn);
	// Reshape function
	glutReshapeFunc(winReshapeFcn);
	// Initialize
	init();
	// Keyboard listener
	glutKeyboardFunc(keyboard);

	// Process Loop
	glutMainLoop();
	return 0;
}
