
#include "pong-lib.h"

namespace pong {

	FollowerPaddleAi::FollowerPaddleAi(const FollowerPaddleAiDefn* aiDefn) {
		this->aiDefn = *aiDefn;
	}

	PaddleInputType FollowerPaddleAi::resolvePaddleInputType(PaddleAiInput input) {
		PaddleInputType result = PaddleInputType::NONE;

		BallState ballState = input.match->getBallState();

		bool ballIsApproaching =
			(
				(ballState.direction.x < 0) &&
				(input.paddle->getSide() == PaddleSide::LEFT)
			) ||
			(
				(ballState.direction.x > 0) &&
				(input.paddle->getSide() == PaddleSide::RIGHT)
			);

		if (
			!this->aiDefn.onlyFollowIfBallIsApproaching ||
			ballIsApproaching
		) {
			if (ballState.position.y > input.paddle->getPosition().y + (input.paddle->getSize().height * this->aiDefn.paddleHeightMultiplier)) {
				result = PaddleInputType::MOVE_UP;
			}
			else if (ballState.position.y < input.paddle->getPosition().y - (input.paddle->getSize().height * this->aiDefn.paddleHeightMultiplier)) {
				result = PaddleInputType::MOVE_DOWN;
			}
		}

		// To prevent two AI's from just sitting at their starting position
		if (ballState.position.y == input.paddle->getPosition().y) {
			result = PaddleInputType::MOVE_DOWN;
		}

		return result;
	}

}
