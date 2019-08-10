
#include <Windows.h>
#include <gl/GL.h>
#include "GL/freeglut.h"
#include "riley-gl-utils.h"

namespace r3 {
	namespace gl {

		void drawText(float x, float y, const unsigned char* text) {
			int displayWidth = glutBitmapLength(GLUT_BITMAP_8_BY_13, text);
			glRasterPos2f(x - (displayWidth / 2), y);

			glutBitmapString(GLUT_BITMAP_8_BY_13, text);
		}

		void drawRect(float x, float y, float width, float height) {
			glBegin(GL_QUADS);

			glVertex2f(x, y);
			glVertex2f(x + width, y);
			glVertex2f(x + width, y + height);
			glVertex2f(x, y + height);

			glEnd();

		}

	}
}

