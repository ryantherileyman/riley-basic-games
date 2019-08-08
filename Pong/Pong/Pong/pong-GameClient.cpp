
#include "pong-lib.h"

namespace pong {

	GameClient::GameClient() {
		MatchDefn matchDefn;
		matchDefn.courtSize.width = 500;
		matchDefn.courtSize.height = 200;
		matchDefn.paddleSize.width = 10;
		matchDefn.paddleSize.height = 80;
		matchDefn.ballSize = 8;

		this->mode = ClientMode::WAIT_TO_START;

		this->match = new Match(&matchDefn);

		this->leftWinCount = 0;
		this->rightWinCount = 0;
	}

}
