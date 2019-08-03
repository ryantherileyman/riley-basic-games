
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

// Racket properties
int racket_width = 10;
int racket_height = 80;
int racket_speed = 3;

// Left racket position
float racket_left_x = 10.0f;
float racket_left_y = 50.0f;

// Right racket position
float racket_right_x = width - racket_width - 20;
float racket_right_y = 50.0f;

// Ball properties
int ball_size = 8;
int ball_speed = 2;

// Ball position
float ball_pos_x = width / 2;
float ball_pos_y = height / 2;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;


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

	// Draw ball
	drawRect(ball_pos_x - ball_size / 2, ball_pos_y - ball_size / 2, ball_size, ball_size);

	// Draw score
	drawText(width / 2 - 10, height - 20, int2str(score_left) + ":" + int2str(score_right));

	// Swap the buffer
	glutSwapBuffers();
}

float moveRacketUp(float currYPos) {
	float result = currYPos + racket_speed;
	if (result + racket_height > height) {
		result = height - racket_height;
	}
	return result;
}

float moveRacketDown(float currYPos) {
	float result = currYPos - racket_speed;
	if (result < 0) {
		result = 0;
	}
	return result;
}

void pollKeyboardInputs() {
	// Left racket
	if (GetAsyncKeyState('W')) {
		racket_left_y = moveRacketUp(racket_left_y);
	}
	if (GetAsyncKeyState('S')) {
		racket_left_y = moveRacketDown(racket_left_y);
	}

	// Right racket
	if (GetAsyncKeyState(VK_UP)) {
		racket_right_y = moveRacketUp(racket_right_y);
	}
	if (GetAsyncKeyState(VK_DOWN)) {
		racket_right_y = moveRacketDown(racket_right_y);
	}
}

void vec2_norm(float& x, float& y) {
	float length = sqrt((x * x) + (y * y));
	if (length != 0.0f) {
		length = 1.0f / length;
		x *= length;
		y *= length;
	}
}

void updateBall() {
	// Fly in current direction
	ball_pos_x += ball_dir_x * ball_speed;
	ball_pos_y += ball_dir_y * ball_speed;

	// Check for collision with left racket
	if (
		(ball_pos_x < racket_left_x + racket_width) &&
		(ball_pos_x > racket_left_x) &&
		(ball_pos_y < racket_left_y + racket_height) &&
		(ball_pos_y > racket_left_y)
	) {
		float t = ((ball_pos_y - racket_left_y) / racket_height) - 0.5f;
		ball_dir_x = fabs(ball_dir_x);
		ball_dir_y = t;
	}

	// Check for collision with right racket
	if (
		(ball_pos_x > racket_right_x) &&
		(ball_pos_x < racket_right_x + racket_width) &&
		(ball_pos_y < racket_right_y + racket_height) &&
		(ball_pos_y > racket_right_y)
	) {
		float t = ((ball_pos_y - racket_right_y) / racket_height) - 0.5f;
		ball_dir_x = -fabs(ball_dir_x);
		ball_dir_y = t;
	}

	// Check for collision with left wall
	if (ball_pos_x < 0) {
		score_right++;
		ball_pos_x = width / 2;
		ball_pos_y = height / 2;
		ball_dir_x = fabs(ball_dir_x);
		ball_dir_y = 0;
	}

	// Check for collision with right wall
	if (ball_pos_x > width) {
		score_left++;
		ball_pos_x = width / 2;
		ball_pos_y = height / 2;
		ball_dir_x = -fabs(ball_dir_x);
		ball_dir_y = 0;
	}

	// Check for collision with top wall
	if (ball_pos_y > height) {
		ball_dir_y = -fabs(ball_dir_y);
	}

	// Check for collision with bottom wall
	if (ball_pos_y < 0) {
		ball_dir_y = fabs(ball_dir_y);
	}

	// Make sure the length of the ball direction stays at 1
	vec2_norm(ball_dir_x, ball_dir_y);
}

void update(int value) {
	// Handle inputs
	pollKeyboardInputs();

	// Update ball position
	updateBall();

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
