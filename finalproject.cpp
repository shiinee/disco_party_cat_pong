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
GLfloat da = 10.0f;
GLfloat dt = 0.1f;

// Flag for pausing
bool paws = true;

/* Board boundaries */
int BOARD_LEFT = -10;
int BOARD_RIGHT = 10;
int BOARD_TOP = 10;
int BOARD_BOTTOM = -10;
int Z_HEIGHT = -5;
float BOARD_DEPTH = 0.1;

/* Paddle positions */
int PADDLE_WIDTH = 5;
int PADDLE_HEIGHT = 1;
int PADDLE_DEPTH = 1;
// Player
float playerPaddle = 0;
float PLAYER_SPEED = 0.2;
// Computer
float computerPaddle = 0;
float COMPUTER_SPEED = 0.3;

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

/* Lighting */
class Light {
public:
	GLint id;
	GLfloat diffuse[4];
	GLfloat position[4];

	Light(int, float, float, float, float, float, float);
};

Light::Light(int _id, float x, float y, float z, float r, float g, float b) {
	id = _id;

	position[0] = x;
	position[1] = y;
	position[2] = z;
	position[3] = 1.0;

	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
	diffuse[3] = 1.0;
}

vector<Light> lights;
vector<Light>::const_iterator l;

// Variables for light animation
GLfloat spin = 0.0f;
GLfloat LIGHT_RADIUS = 1.0;

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

void drawFloor() {
	glPushMatrix();
	glTranslatef(0.0, 0.0, Z_HEIGHT);
	glScalef(BOARD_RIGHT-BOARD_LEFT, BOARD_TOP-BOARD_BOTTOM, BOARD_DEPTH);
	glutSolidCube(1);
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

void start2D() {
	// 2D mode for drawing scores on board
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_LIGHTING);
}

void stop2D() {
	// Exit 2D mode
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void drawScore(int score, int x, int y) {
	glRasterPos2i(x, y);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, score + 48);
}

void drawText(string text, int x, int y) {
        glRasterPos2i(x, y);
        for (int i = 0; i < text.length(); i++)
                glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);
}

void drawGame() {
	glColor3f(0.5, 0.5, 0.5);
	drawFloor();
	drawBorders();

	glColor3f(1.0, 1.0, 1.0);
	drawPaddle(playerPaddle, BOARD_BOTTOM);
	drawPaddle(computerPaddle, BOARD_TOP + PADDLE_HEIGHT);

	drawCat(catX, catY);

	start2D();
	drawScore(playerScore, 10, 10);
	drawScore(computerScore, 10, winHeight - 20);
	drawText("ready to PARTY? press space to start", winWidth/2 - 140, 30);
	drawText("use left and right arrows to move your paddle", winWidth/2 - 175, 20);
	stop2D();
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
	// Check if cat has hit paddle
	if (y > minY && y < maxY && x > minX && x < maxX) {
		// Move cat clear of paddle
		y = resetY;
		// Bounce cat off paddle
		vy = -vy;
		// If cat hit edge of paddle, impart a little extra X-velocity
		// This is only an approximation to realistic pong physics
		vx += 0.9 * (x - (minX + maxX)/2) / PADDLE_WIDTH * BASE_V;
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
	if (y < BOARD_BOTTOM)
		// Computer scored
		computerScore++;
	else if (y > BOARD_TOP)
		// Player scored
		playerScore++;
	else
		return;

	nanosleep((const struct timespec[]){{0, 300000000L}}, NULL);
	resetBoard();
}

void computerMove(float &paddlePos, float catX) {
	// The computer player is very simple minded: it always moves its paddle toward
	// the cat at its maximum speed (without going off the board).
	if (paddlePos + COMPUTER_SPEED < catX && paddlePos + PADDLE_WIDTH/2 < BOARD_RIGHT)
		paddlePos += COMPUTER_SPEED;
	else if (paddlePos - COMPUTER_SPEED > catX && paddlePos - PADDLE_WIDTH/2 > BOARD_LEFT)
		paddlePos -= COMPUTER_SPEED;
}

void showLights() {
	glPushMatrix();
	glRotatef(0.0, 0.0, spin, 1.0);

	for (l = lights.begin(); l != lights.end(); ++l) {
		glLightfv(l->id, GL_POSITION, l->position);
		glLightfv(l->id, GL_DIFFUSE, l->diffuse);
	}

	glPopMatrix();
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

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	glEnable(GL_LIGHT5);
	// Create lights
	lights.push_back(Light(GL_LIGHT0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0));
	lights.push_back(Light(GL_LIGHT1, -0.5, 0.866, 0.0, 0.0, 1.0, 0.0));
	lights.push_back(Light(GL_LIGHT2, -0.5, -0.866, 0.0, 0.0, 0.0, 1.0));
	lights.push_back(Light(GL_LIGHT3, BOARD_LEFT, 0.0, Z_HEIGHT, 1.0, 0.0, 0.0));
	lights.push_back(Light(GL_LIGHT4, BOARD_RIGHT/2, 0.866*BOARD_TOP, Z_HEIGHT, 0.0, 1.0, 0.0));
	lights.push_back(Light(GL_LIGHT5, BOARD_RIGHT/2, 0.866*BOARD_BOTTOM, Z_HEIGHT, 0.0, 0.0, 1.0));

	// Set ambient light, specular color and shine
	GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
        GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat mat_shininess[] = { 50.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

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

	// Spin lights
	spin += da;

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

	// Add lights
	showLights();

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
		if (playerPaddle - PADDLE_WIDTH/2 > BOARD_LEFT)
			playerPaddle -= PLAYER_SPEED;
		break;
	case GLUT_KEY_RIGHT:
		if (playerPaddle + PADDLE_WIDTH/2 < BOARD_RIGHT)
			playerPaddle += PLAYER_SPEED;
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
