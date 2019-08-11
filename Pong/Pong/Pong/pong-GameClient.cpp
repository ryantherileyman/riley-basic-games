
#include <Windows.h>
#include "pong-lib.h"

namespace pong {

	GameClient::GameClient() {
		MatchDefn matchDefn;
		matchDefn.courtSize.width = 500.0f;
		matchDefn.courtSize.height = 200.0f;
		matchDefn.paddleSize.width = 10.0f;
		matchDefn.paddleSize.height = 80.0f;
		matchDefn.paddleSpeed = 3.0f;
		matchDefn.ballSize = 8.0f;
		matchDefn.ballSpeed = 2.0f;

		this->mode = ClientMode::WAIT_TO_START;

		this->match = new Match(&matchDefn);
		this->matchRenderer = new MatchRenderer(this->match);

		this->leftWinCount = 0;
		this->rightWinCount = 0;
	}

	GameClient::~GameClient() {
		delete this->match;
	}

	void GameClient::update() {
		switch (this->mode) {
		case ClientMode::WAIT_TO_START:
			if (this->pollWaitToStartInputs()) {
				this->mode = ClientMode::MATCH_RUNNING;
			}
			break;
		case ClientMode::MATCH_RUNNING:
			MatchInputRequest inputRequest = this->pollMatchRunningInputs();
			MatchUpdateResult matchUpdate = this->match->update(&inputRequest);

			if (matchUpdate.leftScoredFlag) {
				this->match->startPoint(PaddleSide::LEFT);
			} else if ( matchUpdate.rightScoredFlag ) {
				this->match->startPoint(PaddleSide::RIGHT);
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
		}
	}

	bool GameClient::pollWaitToStartInputs() {
		bool result =
			GetAsyncKeyState(13) ||
			GetAsyncKeyState(VK_SPACE);
		return result;
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

}
