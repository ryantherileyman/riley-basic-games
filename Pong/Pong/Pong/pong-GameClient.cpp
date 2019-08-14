
#include <Windows.h>
#include "GL/freeglut.h"
#include "pong-lib.h"

namespace pong {

	GameClient::GameClient() {
		this->matchDefn.courtSize.width = 500.0f;
		this->matchDefn.courtSize.height = 200.0f;
		this->matchDefn.paddleSize.width = 10.0f;
		this->matchDefn.paddleSize.height = 80.0f;
		this->matchDefn.paddleSpeed = 3.0f;
		this->matchDefn.ballSize = 8.0f;
		this->matchDefn.ballSpeed = 2.0f;

		this->mode = ClientMode::WAIT_TO_START;

		this->match = new Match(&this->matchDefn);
		this->matchRenderer = new MatchRenderer(this->match);

		this->matchWinThreshold = 10;
		this->matchWonState.leftMatchWonCount = 0;
		this->matchWonState.rightMatchWonCount = 0;
	}

	GameClient::~GameClient() {
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
		}
	}

	void GameClient::processKeystroke(unsigned char key) {
		bool unpauseFlag = false;
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
		}
	}

	bool GameClient::processWaitToStartKeystroke(unsigned char key) {
		bool startMatchFlag =
			(key == 13) ||
			(key == ' ');
		
		if (startMatchFlag) {
			this->mode = ClientMode::MATCH_RUNNING;
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
		if (GetAsyncKeyState('W')) {
			result.leftPaddleInput = PaddleInputType::MOVE_UP;
		}
		if (GetAsyncKeyState('S')) {
			result.leftPaddleInput = PaddleInputType::MOVE_DOWN;
		}

		// Right paddle
		if (GetAsyncKeyState(VK_UP)) {
			result.rightPaddleInput = PaddleInputType::MOVE_UP;
		}
		if (GetAsyncKeyState(VK_DOWN)) {
			result.rightPaddleInput = PaddleInputType::MOVE_DOWN;
		}

		return result;
	}

	void GameClient::startNewMatch() {
		delete this->matchRenderer;
		delete this->match;

		this->match = new Match(&this->matchDefn);
		this->matchRenderer = new MatchRenderer(this->match);
	}

}