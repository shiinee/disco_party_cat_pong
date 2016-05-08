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
// Time delta for animation
GLfloat dt = 0.1f;

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
float PADDLE_MOVE = 0.2;
// Player
float playerPaddle = 0;
// Computer
float computerPaddle = 0;

/* Cat position */
float catX = 0;
float catY = 0;
float catVx = 0;
float catVy = 0;
float BASE_V = 5.0;

/* Score */
int playerScore = 0;
int computerScore = 0;

/* Timer */
chrono::duration<long,ratio<1,10>> DELTA_T(1);
auto start = std::chrono::high_resolution_clock::now();

float random(float min, float max)
{
    float random = ((float) rand()) / (float) RAND_MAX;
    float range = max - min;
    return random * range + min;
}

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

	glPushMatrix();
	glTranslatef(0.6, 0.55, 0);
	glRotatef(45.0, 0, 1, 0);
	glutSolidCone(0.5, 0.9, 15, 15);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.6, 0.55, 0);
	glRotatef(-45.0, 0, 1, 0);
	glutSolidCone(0.5, 0.9, 15, 15);
	glPopMatrix();

	glutSolidSphere(1, 15, 15);
	glPopMatrix();
}

void drawBorders() {
	glBegin(GL_LINES);

	glVertex3f(BOARD_LEFT, BOARD_BOTTOM, Z_HEIGHT);
	glVertex3f(BOARD_RIGHT, BOARD_BOTTOM, Z_HEIGHT);

	glVertex3f(BOARD_RIGHT, BOARD_BOTTOM, Z_HEIGHT);
	glVertex3f(BOARD_RIGHT, BOARD_TOP, Z_HEIGHT);

	glVertex3f(BOARD_LEFT, BOARD_TOP, Z_HEIGHT);
	glVertex3f(BOARD_RIGHT, BOARD_TOP, Z_HEIGHT);

	glVertex3f(BOARD_LEFT, BOARD_BOTTOM, Z_HEIGHT);
	glVertex3f(BOARD_LEFT, BOARD_TOP, Z_HEIGHT);

	glVertex3f(BOARD_LEFT, BOARD_BOTTOM, Z_HEIGHT + PADDLE_DEPTH);
	glVertex3f(BOARD_RIGHT, BOARD_BOTTOM, Z_HEIGHT + PADDLE_DEPTH);

	glVertex3f(BOARD_RIGHT, BOARD_BOTTOM, Z_HEIGHT + PADDLE_DEPTH);
	glVertex3f(BOARD_RIGHT, BOARD_TOP, Z_HEIGHT + PADDLE_DEPTH);

	glVertex3f(BOARD_LEFT, BOARD_TOP, Z_HEIGHT + PADDLE_DEPTH);
	glVertex3f(BOARD_RIGHT, BOARD_TOP, Z_HEIGHT + PADDLE_DEPTH);

	glVertex3f(BOARD_LEFT, BOARD_BOTTOM, Z_HEIGHT + PADDLE_DEPTH);
	glVertex3f(BOARD_LEFT, BOARD_TOP, Z_HEIGHT + PADDLE_DEPTH);

	glVertex3f(BOARD_LEFT, BOARD_BOTTOM, Z_HEIGHT);
	glVertex3f(BOARD_LEFT, BOARD_BOTTOM, Z_HEIGHT + PADDLE_DEPTH);

	glVertex3f(BOARD_LEFT, BOARD_TOP, Z_HEIGHT);
	glVertex3f(BOARD_LEFT, BOARD_TOP, Z_HEIGHT + PADDLE_DEPTH);

	glVertex3f(BOARD_RIGHT, BOARD_BOTTOM, Z_HEIGHT);
	glVertex3f(BOARD_RIGHT, BOARD_BOTTOM, Z_HEIGHT + PADDLE_DEPTH);

	glVertex3f(BOARD_RIGHT, BOARD_TOP, Z_HEIGHT);
	glVertex3f(BOARD_RIGHT, BOARD_TOP, Z_HEIGHT + PADDLE_DEPTH);

	glEnd();
}

void drawScore(int score, int x, int y) {
	glRasterPos2i(x, y);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, score + 48);
}

void drawGame() {
	glColor3f(1.0, 1.0, 1.0);

	drawBorders();

	drawPaddle(playerPaddle, BOARD_BOTTOM);
	drawPaddle(computerPaddle, BOARD_TOP + PADDLE_HEIGHT);

	drawCat(catX, catY);

	// 2D mode for drawing scores on board
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	drawScore(playerScore, 10, 10);
	drawScore(computerScore, 10, winHeight - 20);

	// Exit 2D mode
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void resetBoard() {
	// Reset cat to center
	catX = 0;
	catY = 0;

	// Randomize starting direction
	float direction = random(M_PI/4, 3*M_PI/4);
	if (rand() % 2)
		direction *= -1;

	catVx = BASE_V * cos(direction);
	catVy = BASE_V * sin(direction);
}

void checkHit(float minX, float maxX, float minY, float maxY, float resetY,
              float x, float y, float &vx, float &vy) {
	if (y > minY && y < maxY && x > minX && x < maxX) {
		y = resetY;
		vy = -vy;
		vx += 0.2 * abs(x - (minX + maxX)/2) / PADDLE_WIDTH * vx;
	}
}

void checkBounds(float &x, float &vx) {
	// Cat bounces off left side
	if (x < BOARD_LEFT) {
		x = BOARD_LEFT;
		vx = -vx;
	}
	// Cat bounces off right side
	if (x > BOARD_RIGHT) {
		x = BOARD_RIGHT;
		vx = -vx;
	}
}

void checkGoal(float &y) {
	// Computer scored
	if (y < BOARD_BOTTOM) {
		computerScore++;
		resetBoard();
	}
	// Player scored
	if (y > BOARD_TOP) {
		playerScore++;
		resetBoard();
	}
	// TODO: freeze screen briefly and indicate point scored before resetting
}

void computerMove(float &paddlePos, float catX) {
	if (paddlePos + PADDLE_MOVE < catX)
		paddlePos += PADDLE_MOVE;
	else if (paddlePos - PADDLE_MOVE > catX)
		paddlePos -= PADDLE_MOVE;
}

/* Initialize OpenGL Graphics */
void init() {
	// Set black background
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
	resetBoard();
	drawGame();
}

void transform(void)
{
	auto now = std::chrono::high_resolution_clock::now();
	if (now - start < DELTA_T)
		return;

	start = now;

	// Move computer paddle
	computerMove(computerPaddle, catX);

	// Move cat
	catX += catVx * dt;
	catY += catVy * dt;

	// Draw game
	drawGame();

	// Check collision with paddle
	checkHit(playerPaddle - PADDLE_WIDTH/2, playerPaddle + PADDLE_WIDTH/2,
		 BOARD_BOTTOM, BOARD_BOTTOM + PADDLE_HEIGHT, BOARD_BOTTOM + PADDLE_HEIGHT,
                 catX, catY, catVx, catVy);
	checkHit(computerPaddle - PADDLE_WIDTH/2, computerPaddle + PADDLE_WIDTH/2,
		 BOARD_TOP - PADDLE_HEIGHT, BOARD_TOP, BOARD_TOP - PADDLE_HEIGHT,
                 catX, catY, catVx, catVy);

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
	gluLookAt(0, -16, 8.2,
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
