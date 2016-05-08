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

/* Board boundaries */
int BOARD_LEFT = -10;
int BOARD_RIGHT = 10;
int BOARD_TOP = 10;
int BOARD_BOTTOM = -10;
int Z_HEIGHT = -5;

/* Paddle positions */
int PADDLE_WIDTH = 5;
int PADDLE_HEIGHT = 1;
int PADDLE_DEPTH = 1;
float PADDLE_MOVE = 0.1;
// Player
float playerPaddle = 0;
// Computer
float computerPaddle = 0;

/* Cat position */
float catX = 0;
float catY = 0;
float catVx = 0;
float catVy = -1;

/* Score */
int playerScore = 0;
int computerScore = 0;

/* Timer */
chrono::duration<long,ratio<1,10>> DELTA_T(1);
auto start = std::chrono::high_resolution_clock::now();

void drawPaddle(float x, float y) {
	glPushMatrix();
	glTranslatef(x, y, Z_HEIGHT);
	glScalef(PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_DEPTH);
	glutSolidCube(1);
	glPopMatrix();
}

void drawCat(float x, float y) {
	glPushMatrix();
	glTranslatef(x, y, Z_HEIGHT);
	glutSolidSphere(1, 15, 15);
	glPopMatrix();
}

void drawGame() {
	glColor3f(1.0, 1.0, 1.0);
	drawPaddle(playerPaddle, BOARD_BOTTOM);
	drawPaddle(computerPaddle, BOARD_TOP);
	drawCat(catX, catY);
}

void resetBoard() {
	catX = 0;
	catY = 0;
	catVx = 0;
	catVy = -1;
}

void checkBounds(float &x, float &vx) {
	if (x < BOARD_LEFT) {
		x = BOARD_LEFT;
		vx = -vx;
	}
	if (x > BOARD_RIGHT) {
		x = BOARD_RIGHT;
		vx = -vx;
	}
}

void checkGoal(float &y) {
	if (y < BOARD_BOTTOM) {
		computerScore++;
		resetBoard();
	}
	if (y > BOARD_TOP) {
		playerScore++;
		resetBoard();
	}
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

	// Draw game board
	drawGame();
}

void transform(void)
{
	auto now = std::chrono::high_resolution_clock::now();
	if (now - start < DELTA_T)
		return;

	start = now;
	t += 0.1;

	// Draw paddles
	drawPaddle(playerPaddle, BOARD_BOTTOM);
	drawPaddle(computerPaddle, BOARD_TOP);

	// Move and draw cat
	catX += catVx * t;
	catY += catVy * t;
	drawCat(catX, catY);

	// Check collision with paddle

	// Check bounds
	checkBounds(catX, catVx);
	checkGoal(catY);

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

	// Draw game board
	drawGame();

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

void specialKeys(int key, int x, int y)
{
	if (paws)
		return;

	switch (key) {
	case GLUT_KEY_LEFT:
		playerPaddle -= PADDLE_MOVE;
		break;
	case GLUT_KEY_RIGHT:
		playerPaddle += PADDLE_MOVE;
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
	gluPerspective(60.0f, aspect, 0.1f, 100.0f);
	// Move camera to position above gameboard on player side
	gluLookAt(0, -15, 5,
		  0, -9, 0,
                  0, 1, 5);
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
	glutSpecialFunc(specialKeys);
	// Process Loop
	glutMainLoop();
	return 0;
}
