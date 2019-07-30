
#include <string>
#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <sstream>
#include <math.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "GL/freeglut.h"
#pragma comment(lib, "OpenGL32.lib")

// Window size and update rate (60 fps)
int width = 500;
int height = 200;
int interval = 1000 / 60;

// Score
int score_left = 0;
int score_right = 0;

// Rackets shape
int racket_width = 10;
int racket_height = 80;
int racket_speed = 3;

// Left racket position
float racket_left_x = 10.0f;
float racket_left_y = 50.0f;

// Right racket position
float racket_right_x = width - racket_width - 20;
float racket_right_y = 50.0f;

std::string int2str(int x) {
	std::stringstream result;
	result << x;
	return result.str();
}

void enable2d(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void drawText(float x, float y, std::string text) {
	glRasterPos2f(x, y);
	glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)text.c_str());
}

void drawRect(float x, float y, float width, float height) {
	glBegin(GL_QUADS);

	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);

	glEnd();
}

void draw() {
	// Clear contents of buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Draw rackets
	drawRect(racket_left_x, racket_left_y, racket_width, racket_height);
	drawRect(racket_right_x, racket_right_y, racket_width, racket_height);

	// Draw score
	drawText(width / 2 - 10, height - 20, int2str(score_left) + ":" + int2str(score_right));

	// Swap the buffer
	glutSwapBuffers();
}

void pollKeyboardInputs() {
	// Left racket
	if (GetAsyncKeyState('W')) {
		racket_left_y += racket_speed;
	}
	if (GetAsyncKeyState('S')) {
		racket_left_y -= racket_speed;
	}

	// Right racket
	if (GetAsyncKeyState(VK_UP)) {
		racket_right_y += racket_speed;
	}
	if (GetAsyncKeyState(VK_DOWN)) {
		racket_right_y -= racket_speed;
	}
}

void update(int value) {
	// Handle inputs
	pollKeyboardInputs();

	// Call 'update' again in interval seconds
	glutTimerFunc(interval, update, 0);

	// Redisplay frame
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	// Initialize OpenGL (via Glut)
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("noobtuts.com Pong");

	// Register callbacks
	glutDisplayFunc(draw);
	glutTimerFunc(interval, update, 0);

	// Set up scene in 2D, and draw colour to white
	enable2d(width, height);
	glColor3f(1.0f, 1.0f, 1.0f);

	// Start the main loop
	glutMainLoop();

	return 0;
}
