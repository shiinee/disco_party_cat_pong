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
char title[] = "Skyler Amador - Homework 6 (1=start anim, 2=stop, click=fun)";
// Window display size
GLsizei winWidth = 800, winHeight = 600;
// Time for animation
GLfloat t = 0.0f;

const int WAVES = 6;

const int POINTS = 10;
const int G = 1.98;

chrono::duration<long,ratio<1,10>> DELTA_T(1);
auto start = std::chrono::high_resolution_clock::now();

float random(float min, float max)
{
    float random = ((float) rand()) / (float) RAND_MAX;
    float range = max - min;  
    return random * range + min;
}


class Cloud {
		float x, y, z;
		float start_t;
		float v;
	public:
		Cloud(float, float, float, float, float);
		void move(float);
		void draw() const;
};

Cloud::Cloud(float _x, float _y, float _z, float _v, float _t) : x(_x), y(_y), z(_z), v(_v), start_t(_t) {};

void Cloud::draw() const {
	glPushMatrix();
	glTranslatef(x + v * (t - start_t), y, z);
	glColor3f(0.5, 0.5, 0.6);

	glPushMatrix();
	glutSolidSphere(1.0, 30, 30);
	glTranslatef(1.2, 0, 0);
	glutSolidSphere(1.0, 30, 30);
	glTranslatef(1.2, 0, 0);
	glutSolidSphere(1.0, 30, 30);
	glTranslatef(-0.6, 1.0, 0);
	glutSolidSphere(1.0, 30, 30);
	glTranslatef(-1.2, 0, 0);
	glutSolidSphere(1.0, 30, 30);
	glPopMatrix();

	glPopMatrix();
}

vector<Cloud> clouds;
vector<Cloud>::const_iterator c;

class Wave {
		int x, y, z, sz;
		float r, g, b;
		float p;
	public:
		Wave(int, int, int, int, float, float, float, float);
		void draw();
};

Wave::Wave(int _x, int _y, int _z, int _sz, float _r, float _g, float _b, float _p) : 
	x(_x), y(_y), z(_z), sz(_sz), r(_r), g(_g), b(_b), p(_p) {};

void Wave::draw() {
	glPushMatrix();
	glTranslatef(x, y + 0.05*sin(t+p), z);
	glColor3f(r, g, b);
	glutSolidSphere(1.0, 30, 30);
	glPopMatrix();
}

Wave wave1(-1.5, -2.8, -3.2, 1.1, 0.2, 0.42, 0.75, 1);
Wave wave2(1.2, -2.4, -3.4, 1.2, 0.17, 0.4, 0.73, 0.6);
Wave wave3(-0.5, -2, -3.0, 1.0, 0.2, 0.44, 0.8, 0);
Wave wave4(-1.0, -2.6, -4.0, 0.8, 0.22, 0.48, 0.85, 0.2);
Wave wave5(1.8, -2.1, -4.2, 1.4, 0.25, 0.5, 0.88, 0.4);
Wave wave6(0.2, -2.0, -4.5, 1.2, 0.22, 0.5, 0.9, 0.8);

Wave waves[WAVES] = {wave6, wave4, wave5, wave1, wave2, wave3};


struct Point {
	float x, y, z;
};


class Firework {
		float x, y, z;
		float v;
		float start_t;
		float r, g, b;
		Point points[POINTS][POINTS][POINTS];

		float vv(float);
		float hv(float);
		float time(float);
	public:
		Firework(float, float, float, float, float, float, float, float);
		void explode(float);
		void draw();
};

Firework::Firework(float _x, float _y, float _z, float _v, float _t, float _r, float _g, float _b) : 
	x(_x), y(_y), z(_z), v(_v), start_t(_t), r(_r), g(_g), b(_b) {
	for (int i = 0; i < POINTS; i++)
		for (int j = 0; j < POINTS; j++)
			for (int k = 0; k < POINTS; k++)
				points[i][j][k] = {x, y, z};
}

float Firework::time(float tt) {
	return tt - start_t;
}

float Firework::vv(float t) {
	return v * (1 - exp(-t));
}

float Firework::hv(float t) {
	return v * exp(-t);
}

void Firework::explode(float t) {
	for (int i = 0; i < POINTS; i++)
		for (int j = 0; j < POINTS; j++)
			for (int k = 0; k < POINTS; k++) {
				points[i][j][k].x += (k * sin(j*POINTS/2/M_PI) * cos(i)) * hv(time(t)) * time(t);
				points[i][j][k].y += (k * sin(j) * sin(i)) * hv(time(t)) * time(t) - vv(time(t)) * time(t) - G/2 * time(t) * time(t);
				points[i][j][k].z += (k * cos(j)) * hv(time(t)) * time(t);
			}

	r *= 0.95 / pow(time(t)*.75,.25);
	g *= 0.95 / pow(time(t)*.75,.25);
	b *= 0.95 / pow(time(t)*.75,.25);
}

void Firework::draw() {
	glColor3f(r, g, b);
	Point p;

	for (int i = 0; i < POINTS; i++)
		for (int j = 0; j < POINTS; j++)
			for (int k = 0; k < POINTS; k++) {
				glPushMatrix();
				p = points[i][j][k];
				glTranslatef(p.x, p.y, p.z);
				glutSolidSphere(0.1/pow(t,.25), 10, 10);
				glPopMatrix();
			}
}

vector<Firework> fireworks;
vector<Firework>::iterator f;


/* Initialize OpenGL Graphics */
void init() {
	// Get and display your OpenGL version
	const GLubyte *Vstr;
	Vstr = glGetString(GL_VERSION);
	fprintf(stderr, "Your OpenGL version is %s\n", Vstr);

	// Draw a dark sky
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	// Maximize depth display
	glClearDepth(1.0f);
	// Enable depth testing for z-culling
	glEnable(GL_DEPTH_TEST);
	// Set the type of depth-test
	glDepthFunc(GL_LEQUAL);
	// Provide smooth shading
	glShadeModel(GL_SMOOTH);

	// Get ahead of the game on clouds
	clouds.push_back(Cloud(-20, random(4.0, 12.0), random(-32.0, -28.0), random(0.8, 1.5), t));
}

void transform(void)
{
	auto now = std::chrono::high_resolution_clock::now();
	if (now - start < DELTA_T)
		return;

	start = now;
	t += 0.1;

	if (random(0.0, 1.0) < 0.01)
		clouds.push_back(Cloud(-20, random(4.0, 12.0), random(-32.0, -28.0), random(0.8, 1.5), t));

	for (f = fireworks.begin(); f != fireworks.end(); ++f)
		f->explode(t);

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

	// Draw teh clouds
	for (c = clouds.begin(); c != clouds.end(); c++)
		c->draw();

	// Draw some wave
	for (int g = 0; g < WAVES; g++)
		waves[g].draw();

	// And some friggin fireworks!
	for (f = fireworks.begin(); f != fireworks.end(); ++f)
		f->draw();

	// Double buffering
	glutSwapBuffers();
}

void mouseFcn(GLint button, GLint action, GLint x, GLint y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (action == GLUT_DOWN) {
			float rx = random(-5, 5);
			float ry = random(0, 5);
			float rz = random(-28, -20);
			float rr = random(0.1, 1.0);
			float rg = random(0.1, 1.0);
			float rb = random(0.1, 1.0);
			fireworks.push_back(Firework(rx, ry, rz, 0.1, t, rr, rg, rb));
		}
		break;
	default:
		break;
	}
}

void keyboard(unsigned char key, int x, int y)
{
        switch (key) {
        case '1':   /*  1 key starts the animation  */
                glutIdleFunc(transform);
                break;
        case '2': /*  2 key stops the animation  */
                glutIdleFunc(NULL);
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
	// Mouse and Keboard functions calls
	glutMouseFunc(mouseFcn);
	glutKeyboardFunc(keyboard);

	// Process Loop
	glutMainLoop();
	return 0;
}
