
#include <Windows.h>
#include "GL/freeglut.h"
#include "pong-lib.h"

namespace pong {

	GameClient::GameClient() {
		this->matchDefn.courtSize.width = 500.0f;
		this->matchDefn.courtSize.height = 200.0f;
		this->matchDefn.paddleSize.width = 10.0f;
		this->matchDefn.paddleSize.height = PaddleSizeOptions::MEDIUM;
		this->matchDefn.paddleSpeed = 3.0f;
		this->matchDefn.leftPaddleControlSource = PaddleControlSource::PLAYER;
		this->matchDefn.rightPaddleControlSource = PaddleControlSource::AI_FOLLOWER;
		this->matchDefn.ballSize = 8.0f;
		this->matchDefn.ballSpeed = BallSpeedOptions::NORMAL;

		this->mode = ClientMode::WAIT_TO_START;

		this->match = new Match(&this->matchDefn);
		this->matchRenderer = new MatchRenderer(this->match);
		this->matchOptionsController = { nullptr };

		this->matchWinThreshold = 10;
		this->matchWonState.leftMatchWonCount = 0;
		this->matchWonState.rightMatchWonCount = 0;
	}

	GameClient::~GameClient() {
		if (this->matchOptionsController != nullptr) {
			delete this->matchOptionsController;
		}

		delete this->matchRenderer;
		delete this->match;
	}

	void GameClient::update() {
		switch (this->mode) {
		case ClientMode::MATCH_RUNNING:
			MatchInputRequest inputRequest = this->pollMatchRunningInputs();
			MatchUpdateResult matchUpdate = this->match->update(&inputRequest);

			if (matchUpdate.leftScoredFlag) {
				this->matchWonState.sideWon = PaddleSide::LEFT;

				if (this->match->getLeftScore() >= this->matchWinThreshold) {
					this->matchWonState.leftMatchWonCount++;
					this->mode = ClientMode::MATCH_WON;
				} else {
					this->match->startPoint(PaddleSide::LEFT);
				}
			}
			else if (matchUpdate.rightScoredFlag) {
				this->matchWonState.sideWon = PaddleSide::RIGHT;

				if (this->match->getRightScore() >= this->matchWinThreshold) {
					this->matchWonState.rightMatchWonCount++;
					this->mode = ClientMode::MATCH_WON;
				} else {
					this->match->startPoint(PaddleSide::RIGHT);
				}
			}
			break;
		}
	}

	void GameClient::draw() {
		switch (this->mode) {
		case ClientMode::WAIT_TO_START:
			this->matchRenderer->renderWaitToStart();
			break;
		case ClientMode::MATCH_RUNNING:
			this->matchRenderer->renderMatchRunning();
			break;
		case ClientMode::MATCH_PAUSED:
			this->matchRenderer->renderMatchPaused();
			break;
		case ClientMode::MATCH_WON:
			this->matchRenderer->renderMatchWon(this->matchWonState);
			break;
		case ClientMode::MATCH_OPTIONS:
			this->matchRenderer->renderMatchOptions(this->matchOptionsController);
			break;
		}
	}

	void GameClient::processKeystroke(unsigned char key) {
		switch (this->mode) {
		case ClientMode::WAIT_TO_START:
			this->processWaitToStartKeystroke(key);
			break;
		case ClientMode::MATCH_RUNNING:
			this->processMatchRunningKeystroke(key);
			break;
		case ClientMode::MATCH_PAUSED:
			this->processMatchPausedKeystroke(key);
			break;
		case ClientMode::MATCH_WON:
			if (this->processWaitToStartKeystroke(key)) {
				this->startNewMatch();
			}
			break;
		case ClientMode::MATCH_OPTIONS:
			this->processMatchOptionsKeystroke(key);
			break;
		}
	}

	void GameClient::processSpecialKeystroke(int key) {
		switch (this->mode) {
		case ClientMode::MATCH_OPTIONS:
			if (key == GLUT_KEY_LEFT) {
				this->matchOptionsController->update(MatchOptionsInputType::PREV_VALUE);
			}
			else if (key == GLUT_KEY_RIGHT) {
				this->matchOptionsController->update(MatchOptionsInputType::NEXT_VALUE);
			}
			else if (key == GLUT_KEY_UP) {
				this->matchOptionsController->update(MatchOptionsInputType::PREV_OPTION);
			}
			else if (key == GLUT_KEY_DOWN) {
				this->matchOptionsController->update(MatchOptionsInputType::NEXT_OPTION);
			}
			break;
		}
	}

	bool GameClient::processWaitToStartKeystroke(unsigned char key) {
		bool startMatchFlag =
			(key == 13) ||
			(key == ' ');

		bool matchOptionsFlag = (key == 27);
		
		if (startMatchFlag) {
			this->mode = ClientMode::MATCH_RUNNING;
		}

		if (matchOptionsFlag) {
			this->matchOptionsController = new MatchOptionsController(&this->matchDefn);
			this->mode = ClientMode::MATCH_OPTIONS;
		}

		return startMatchFlag;
	}

	void GameClient::processMatchRunningKeystroke(unsigned char key) {
		bool pauseFlag = (key == 'p');
		if (pauseFlag) {
			this->mode = ClientMode::MATCH_PAUSED;
		}
	}

	void GameClient::processMatchPausedKeystroke(unsigned char key) {
		bool unpauseFlag =
			(key == 13) ||
			(key == ' ') ||
			(key == 'p');
		if (unpauseFlag) {
			this->mode = ClientMode::MATCH_RUNNING;
		}
	}

	MatchInputRequest GameClient::pollMatchRunningInputs() {
		MatchInputRequest result;
		result.leftPaddleInput = PaddleInputType::NONE;
		result.rightPaddleInput = PaddleInputType::NONE;

		// Left paddle
		if (this->match->getLeftPaddle()->getControlSource() == PaddleControlSource::PLAYER) {
			if (GetAsyncKeyState('W')) {
				result.leftPaddleInput = PaddleInputType::MOVE_UP;
			}
			if (GetAsyncKeyState('S')) {
				result.leftPaddleInput = PaddleInputType::MOVE_DOWN;
			}
		} else {
			result.leftPaddleInput = this->match->getLeftPaddle()->resolvePaddleInputType(this->match);
		}

		// Right paddle
		if (this->match->getRightPaddle()->getControlSource() == PaddleControlSource::PLAYER) {
			if (GetAsyncKeyState(VK_UP)) {
				result.rightPaddleInput = PaddleInputType::MOVE_UP;
			}
			if (GetAsyncKeyState(VK_DOWN)) {
				result.rightPaddleInput = PaddleInputType::MOVE_DOWN;
			}
		} else {
			result.rightPaddleInput = this->match->getRightPaddle()->resolvePaddleInputType(this->match);
		}

		return result;
	}

	void GameClient::processMatchOptionsKeystroke(unsigned char key) {
		bool closeOptionsFlag =
			(key == 27) ||
			(key == 13) ||
			(key == ' ');

		if (closeOptionsFlag) {
			this->matchDefn.paddleSize = this->matchOptionsController->getPaddleSize();
			this->matchDefn.ballSpeed = this->matchOptionsController->getBallSpeed();
			this->matchDefn.leftPaddleControlSource = this->matchOptionsController->getLeftPaddleControlSource();
			this->matchDefn.rightPaddleControlSource = this->matchOptionsController->getRightPaddleControlSource();

			delete this->matchOptionsController;
			this->matchOptionsController = { nullptr };

			this->startNewMatch();

			this->mode = ClientMode::WAIT_TO_START;
		}
	}

	void GameClient::startNewMatch() {
		delete this->matchRenderer;
		delete this->match;

		this->match = new Match(&this->matchDefn);
		this->matchRenderer = new MatchRenderer(this->match);
	}

}
