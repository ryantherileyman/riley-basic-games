
#include <stdio.h>
#include <Windows.h>
#include <gl/GL.h>
#include "riley-gl-utils.h"
#include "pong-lib.h"

namespace pong {

	using namespace r3::gl;

	MatchRenderer::MatchRenderer(const Match* match) {
		this->match = match;
	}

	void MatchRenderer::render() {
		// Clear contents of buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set transformation so that screen-center is (0,0)
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(300.0f, 150.0f, 0.0f);
		
		float paddleWidth = this->match->getLeftPaddle()->getSize().width;

		// Draw out-of-bounds area
		glColor3f(0.2f, 0.2f, 0.2f);
		drawRect(-300.0f, this->match->getCourtSize().height / 2, 600.0f, 50.0f);
		drawRect(-300.0f, -this->match->getCourtSize().height / 2, 600.0f, -50.0f);
		drawRect(-this->match->getCourtSize().width / 2, -150.0f, -50.0f, 300.0f);
		drawRect(this->match->getCourtSize().width / 2, -150.0f, 50.0f, 300.0f);

		// Draw center line
		glColor3f(0.5f, 0.5f, 0.5f);
		glPushAttrib(GL_ENABLE_BIT);
		glLineStipple(1, 0xAAAA);
		glEnable(GL_LINE_STIPPLE);
		glBegin(GL_LINES);
		glVertex2f(0.0f, -this->match->getCourtSize().height / 2);
		glVertex2f(0.0f, this->match->getCourtSize().height / 2);
		glEnd();
		glPopAttrib();

		// Draw top and bottom walls
		glColor3f(0.8f, 0.8f, 0.8f);
		drawRect(-this->match->getCourtSize().width / 2 + (paddleWidth * 2), -this->match->getCourtSize().height / 2, this->match->getCourtSize().width - (paddleWidth * 4), -paddleWidth);
		drawRect(-this->match->getCourtSize().width / 2 + (paddleWidth * 2), this->match->getCourtSize().height / 2, this->match->getCourtSize().width - (paddleWidth * 4), paddleWidth);

		// Set colour to white for paddles and ball
		glColor3f(1.0f, 1.0f, 1.0f);

		// Draw left paddle
		Paddle* leftPaddle = this->match->getLeftPaddle();
		drawRect(leftPaddle->getPosition().x - leftPaddle->getSize().width, leftPaddle->getPosition().y - (leftPaddle->getSize().height / 2), leftPaddle->getSize().width, leftPaddle->getSize().height);

		// Draw right paddle
		Paddle* rightPaddle = this->match->getRightPaddle();
		drawRect(rightPaddle->getPosition().x, rightPaddle->getPosition().y - (rightPaddle->getSize().height / 2), rightPaddle->getSize().width, rightPaddle->getSize().height);

		// Draw ball
		BallState ballState = this->match->getBallState();
		drawRect(ballState.position.x - (ballState.size / 2), ballState.position.y - (ballState.size / 2), ballState.size, ballState.size);

		// Draw score
		char scoreString[100];
		sprintf_s(scoreString, "%d : %d", this->match->getLeftScore(), this->match->getRightScore());
		drawText(0.0f, (this->match->getCourtSize().height / 2) + (paddleWidth * 1.5f), (unsigned char*)scoreString);
	}

}
