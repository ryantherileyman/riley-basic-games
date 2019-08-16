
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

	void MatchRenderer::renderWaitToStart() {
		this->clearScene();
		this->prepareTransform();

		this->renderCourt();

		glColor3f(1.0f, 1.0f, 1.0f);
		drawCenteredText(0.0f, 0.0f, (unsigned char*)"Press ENTER to start!");
	}

	void MatchRenderer::renderMatchRunning() {
		this->clearScene();
		this->prepareTransform();

		this->renderCourt();
		this->renderMatchScore();
		this->renderMatchObjects();
	}

	void MatchRenderer::renderMatchPaused() {
		this->clearScene();
		this->prepareTransform();

		this->renderCourt();

		glColor3f(1.0f, 1.0f, 1.0f);
		drawCenteredText(0.0f, 0.0f, (unsigned char*)"Paused - Press ENTER to continue");
	}

	void MatchRenderer::renderMatchWon(MatchWonState matchWonState) {
		this->clearScene();
		this->prepareTransform();

		this->renderCourt();

		glColor3f(1.0f, 1.0f, 1.0f);
		if (matchWonState.sideWon == PaddleSide::LEFT) {
			drawCenteredText(0.0f, 30.0f, (unsigned char*)"Left Wins!");
		}
		else {
			drawCenteredText(0.0f, 30.0f, (unsigned char*)"Right Wins!");
		}

		char matchScoreString[100];
		sprintf_s(matchScoreString, "Games Won:  Left %d -- Right %d", matchWonState.leftMatchWonCount, matchWonState.rightMatchWonCount);
		drawCenteredText(0.0f, 0.0f, (unsigned char*)matchScoreString);

		drawCenteredText(0.0f, -30.0f, (unsigned char*)"Press ENTER to play again");
	}

	void MatchRenderer::renderMatchOptions(const MatchOptionsController* matchOptionsController) {
		this->clearScene();
		this->prepareTransform();

		glColor3f(1.0f, 1.0f, 1.0f);
		drawCenteredText(0.0f, 105.0f, (unsigned char*)"Press Up or Down Arrow to navigate between options");

		glColor3f(1.0f, 1.0f, 1.0f);
		drawCenteredText(0.0f, 80.0f, (unsigned char*)"Press Left or Right Arrow to change an option value");

		glColor3f(1.0f, 1.0f, 1.0f);
		if (matchOptionsController->getCurrOption() == MatchOptionsController::OPTION_PADDLE_SIZE) {
			glColor3f(1.0f, 1.0f, 0.0f);
			switch (matchOptionsController->getPaddleSizeOptionValue()) {
			case MatchOptionsController::PADDLE_SIZE_TINY:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"Best of luck!");
				break;
			case MatchOptionsController::PADDLE_SIZE_SMALL:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"This could be a challenge...");
				break;
			case MatchOptionsController::PADDLE_SIZE_MEDIUM:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"How typical...");
				break;
			case MatchOptionsController::PADDLE_SIZE_LARGE:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"This should be easy...");
				break;
			case MatchOptionsController::PADDLE_SIZE_ENORMOUS:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"You can't miss!");
				break;
			}
		}
		drawText(-200.0f, 50.0f, (unsigned char*)"Paddle Size:");
		switch (matchOptionsController->getPaddleSizeOptionValue()) {
		case MatchOptionsController::PADDLE_SIZE_TINY:
			drawText(0.0f, 50.0f, (unsigned char*)"Tiny");
			break;
		case MatchOptionsController::PADDLE_SIZE_SMALL:
			drawText(0.0f, 50.0f, (unsigned char*)"Small");
			break;
		case MatchOptionsController::PADDLE_SIZE_MEDIUM:
			drawText(0.0f, 50.0f, (unsigned char*)"Medium");
			break;
		case MatchOptionsController::PADDLE_SIZE_LARGE:
			drawText(0.0f, 50.0f, (unsigned char*)"Large");
			break;
		case MatchOptionsController::PADDLE_SIZE_ENORMOUS:
			drawText(0.0f, 50.0f, (unsigned char*)"Enormous");
			break;
		}

		glColor3f(1.0f, 1.0f, 1.0f);
		if (matchOptionsController->getCurrOption() == MatchOptionsController::OPTION_BALL_SPEED) {
			glColor3f(1.0f, 1.0f, 0.0f);
			switch (matchOptionsController->getBallSpeedOptionValue()) {
			case MatchOptionsController::BALL_SPEED_SLOW:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"Go for a bio break...");
				break;
			case MatchOptionsController::BALL_SPEED_NORMAL:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"Kinda leisurely...");
				break;
			case MatchOptionsController::BALL_SPEED_FAST:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"The way it's meant to be played");
				break;
			case MatchOptionsController::BALL_SPEED_BLAZING:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"Now this has some kick to it!");
				break;
			case MatchOptionsController::BALL_SPEED_LUDICROUS:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"They've gone to plaid!");
				break;
			}
		}
		drawText(-200.0f, 25.0f, (unsigned char*)"Ball Speed:");
		switch (matchOptionsController->getBallSpeedOptionValue()) {
		case MatchOptionsController::BALL_SPEED_SLOW:
			drawText(0.0f, 25.0f, (unsigned char*)"Slow");
			break;
		case MatchOptionsController::BALL_SPEED_NORMAL:
			drawText(0.0f, 25.0f, (unsigned char*)"Normal");
			break;
		case MatchOptionsController::BALL_SPEED_FAST:
			drawText(0.0f, 25.0f, (unsigned char*)"Fast");
			break;
		case MatchOptionsController::BALL_SPEED_BLAZING:
			drawText(0.0f, 25.0f, (unsigned char*)"Blazing");
			break;
		case MatchOptionsController::BALL_SPEED_LUDICROUS:
			drawText(0.0f, 25.0f, (unsigned char*)"Ludicrous");
			break;
		}

		glColor3f(1.0f, 1.0f, 1.0f);
		if (matchOptionsController->getCurrOption() == MatchOptionsController::OPTION_LEFT_PADDLE_CONTROL_SOURCE) {
			glColor3f(1.0f, 1.0f, 0.0f);
			switch (matchOptionsController->getLeftPaddleControlSourceValue()) {
			case MatchOptionsController::PADDLE_CONTROL_SOURCE_PLAYER:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"It's all you");
				break;
			case MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_LATE_FOLLOWER:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"I'll follow the ball when it's coming at me");
				break;
			case MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_FOLLOWER:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"I'll follow the ball");
				break;
			case MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_CLOSE_FOLLOWER:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"I'm perfect at following the ball");
				break;
			}
		}
		drawText(-200.0f, 0.0f, (unsigned char*)"Left Paddle:");
		switch (matchOptionsController->getLeftPaddleControlSourceValue()) {
		case MatchOptionsController::PADDLE_CONTROL_SOURCE_PLAYER:
			drawText(0.0f, 0.0f, (unsigned char*)"Player");
			break;
		case MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_LATE_FOLLOWER:
			drawText(0.0f, 0.0f, (unsigned char*)"AI - Late Follower");
			break;
		case MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_FOLLOWER:
			drawText(0.0f, 0.0f, (unsigned char*)"AI - Follower");
			break;
		case MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_CLOSE_FOLLOWER:
			drawText(0.0f, 0.0f, (unsigned char*)"AI - Close Follower");
			break;
		}

		glColor3f(1.0f, 1.0f, 1.0f);
		if (matchOptionsController->getCurrOption() == MatchOptionsController::OPTION_RIGHT_PADDLE_CONTROL_SOURCE) {
			glColor3f(1.0f, 1.0f, 0.0f);
			switch (matchOptionsController->getRightPaddleControlSourceValue()) {
			case MatchOptionsController::PADDLE_CONTROL_SOURCE_PLAYER:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"It's all you");
				break;
			case MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_LATE_FOLLOWER:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"I'll follow the ball when it's coming at me");
				break;
			case MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_FOLLOWER:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"I'll follow the ball");
				break;
			case MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_CLOSE_FOLLOWER:
				drawCenteredText(0.0f, -80.0f, (unsigned char*)"I'm perfect at following the ball");
				break;
			}
		}
		switch (matchOptionsController->getRightPaddleControlSourceValue()) {
		case MatchOptionsController::PADDLE_CONTROL_SOURCE_PLAYER:
			drawText(0.0f, -25.0f, (unsigned char*)"Player");
			break;
		case MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_LATE_FOLLOWER:
			drawText(0.0f, -25.0f, (unsigned char*)"AI - Late Follower");
			break;
		case MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_FOLLOWER:
			drawText(0.0f, -25.0f, (unsigned char*)"AI - Follower");
			break;
		case MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_CLOSE_FOLLOWER:
			drawText(0.0f, -25.0f, (unsigned char*)"AI - Close Follower");
			break;
		}
		drawText(-200.0f, -25.0f, (unsigned char*)"Right Paddle:");

		glColor3f(1.0f, 1.0f, 1.0f);
		drawCenteredText(0.0f, -110.0f, (unsigned char*)"Press ENTER to accept options");
	}

	void MatchRenderer::clearScene() {
		// Clear contents of buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}

	void MatchRenderer::prepareTransform() {
		// Set transformation so that screen-center is (0,0)
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(300.0f, 150.0f, 0.0f);
	}

	void MatchRenderer::renderCourt() {
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
	}

	void MatchRenderer::renderMatchScore() {
		char scoreString[100];
		sprintf_s(scoreString, "%d : %d", this->match->getLeftScore(), this->match->getRightScore());

		glColor3f(1.0f, 1.0f, 1.0f);
		drawCenteredText(0.0f, (this->match->getCourtSize().height / 2) + (this->match->getLeftPaddle()->getSize().width * 1.5f), (unsigned char*)scoreString);
	}

	void MatchRenderer::renderMatchObjects() {
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
	}

}
