
#include <Windows.h>
#include <gl/GL.h>
#include "GL/freeglut.h"
#include "pong-lib.h"

// Window size and update rate (60 fps)
int width = 600;
int height = 300;
int interval = 1000 / 60;

// Game client
pong::GameClient* pongGameClient{ nullptr };

void enable2d(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void draw() {
	pongGameClient->draw();

	// Swap the buffer
	glutSwapBuffers();
}

void update(int value) {
	// Update the game client
	pongGameClient->update();

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
	glutCreateWindow("Pong - Riley Entertainment");

	pongGameClient = new pong::GameClient();

	// Register callbacks
	glutDisplayFunc(draw);
	glutTimerFunc(interval, update, 0);

	// Set up scene in 2D, and draw colour to white
	enable2d(width, height);
	glColor3f(1.0f, 1.0f, 1.0f);

	// Start the main loop
	glutMainLoop();

	delete pongGameClient;

	return 0;
}
