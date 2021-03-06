
#include "pong-lib.h"

namespace pong {

	using namespace r3::graphics2d;

	/* Available game options */
	const int OPTION_LAST = MatchOptionsController::OPTION_RIGHT_PADDLE_CONTROL_SOURCE;

	/* Paddle size option values */
	namespace PaddleSizeOptions {
		const float TINY = 25.0f;
		const float SMALL = 50.0f;
		const float MEDIUM = 80.0f;
		const float LARGE = 110.0f;
		const float ENORMOUS = 180.0f;
	}
	const int PADDLE_SIZE_LAST = MatchOptionsController::PADDLE_SIZE_ENORMOUS;

	/* Ball speed option values */
	namespace BallSpeedOptions {
		const float SLOW = 1.0f;
		const float NORMAL = 2.5f;
		const float FAST = 5.0f;
		const float BLAZING = 8.0f;
		const float LUDICROUS = 20.0f;
	}
	const int BALL_SPEED_LAST = MatchOptionsController::BALL_SPEED_LUDICROUS;

	const int PADDLE_CONTROL_SOURCE_LAST = MatchOptionsController::PADDLE_CONTROL_SOURCE_AI_SNOOKER_PRO;

	int MatchOptionsController::resolvePaddleSizeOptionValue(r3::graphics2d::Size2D paddleSize) {
		int result = PADDLE_SIZE_MEDIUM;
		if (paddleSize.height == PaddleSizeOptions::TINY) {
			result = PADDLE_SIZE_TINY;
		}
		else if (paddleSize.height == PaddleSizeOptions::SMALL) {
			result = PADDLE_SIZE_SMALL;
		}
		else if (paddleSize.height == PaddleSizeOptions::LARGE) {
			result = PADDLE_SIZE_LARGE;
		}
		else if ( paddleSize.height == PaddleSizeOptions::ENORMOUS) {
			result = PADDLE_SIZE_ENORMOUS;
		}
		return result;
	}

	int MatchOptionsController::resolveBallSpeedOptionValue(float ballSpeed) {
		int result = BALL_SPEED_NORMAL;
		if (ballSpeed == BallSpeedOptions::SLOW) {
			result = BALL_SPEED_SLOW;
		}
		else if (ballSpeed == BallSpeedOptions::FAST) {
			result = BALL_SPEED_FAST;
		}
		else if (ballSpeed == BallSpeedOptions::BLAZING) {
			result = BALL_SPEED_BLAZING;
		}
		else if (ballSpeed == BallSpeedOptions::LUDICROUS) {
			result = BALL_SPEED_LUDICROUS;
		}
		return result;
	}

	int MatchOptionsController::resolvePaddleControlSourceValue(PaddleControlSource paddleControlSource) {
		int result = PADDLE_CONTROL_SOURCE_PLAYER;
		if (paddleControlSource == PaddleControlSource::AI_GUESSER) {
			result = PADDLE_CONTROL_SOURCE_AI_GUESSER;
		}
		else if (paddleControlSource == PaddleControlSource::AI_LATE_FOLLOWER) {
			result = PADDLE_CONTROL_SOURCE_AI_LATE_FOLLOWER;
		}
		else if (paddleControlSource == PaddleControlSource::AI_FOLLOWER) {
			result = PADDLE_CONTROL_SOURCE_AI_FOLLOWER;
		}
		else if (paddleControlSource == PaddleControlSource::AI_CLOSE_FOLLOWER) {
			result = PADDLE_CONTROL_SOURCE_AI_CLOSE_FOLLOWER;
		}
		else if (paddleControlSource == PaddleControlSource::AI_SNOOKER_PRO) {
			result = PADDLE_CONTROL_SOURCE_AI_SNOOKER_PRO;
		}
		return result;
	}

	MatchOptionsController::MatchOptionsController(const MatchDefn* matchDefn) {
		this->currMatchOption = OPTION_PADDLE_SIZE;

		this->currOptionValueArray[OPTION_PADDLE_SIZE] = resolvePaddleSizeOptionValue(matchDefn->paddleSize);
		this->currOptionValueArray[OPTION_BALL_SPEED] = resolveBallSpeedOptionValue(matchDefn->ballSpeed);
		this->currOptionValueArray[OPTION_LEFT_PADDLE_CONTROL_SOURCE] = resolvePaddleControlSourceValue(matchDefn->leftPaddleControlSource);
		this->currOptionValueArray[OPTION_RIGHT_PADDLE_CONTROL_SOURCE] = resolvePaddleControlSourceValue(matchDefn->rightPaddleControlSource);

		this->maxOptionValueArray[OPTION_PADDLE_SIZE] = PADDLE_SIZE_LAST;
		this->maxOptionValueArray[OPTION_BALL_SPEED] = BALL_SPEED_LAST;
		this->maxOptionValueArray[OPTION_LEFT_PADDLE_CONTROL_SOURCE] = PADDLE_CONTROL_SOURCE_LAST;
		this->maxOptionValueArray[OPTION_RIGHT_PADDLE_CONTROL_SOURCE] = PADDLE_CONTROL_SOURCE_LAST;
	}

	int MatchOptionsController::getCurrOption() const {
		return this->currMatchOption;
	}

	int MatchOptionsController::getPaddleSizeOptionValue() const {
		return this->currOptionValueArray[OPTION_PADDLE_SIZE];
	}

	int MatchOptionsController::getBallSpeedOptionValue() const {
		return this->currOptionValueArray[OPTION_BALL_SPEED];
	}

	int MatchOptionsController::getLeftPaddleControlSourceValue() const {
		return this->currOptionValueArray[OPTION_LEFT_PADDLE_CONTROL_SOURCE];
	}

	int MatchOptionsController::getRightPaddleControlSourceValue() const {
		return this->currOptionValueArray[OPTION_RIGHT_PADDLE_CONTROL_SOURCE];
	}

	Size2D MatchOptionsController::getPaddleSize() {
		Size2D result;

		result.width = 10.0f;

		switch (this->currOptionValueArray[OPTION_PADDLE_SIZE]) {
		case PADDLE_SIZE_TINY:
			result.height = PaddleSizeOptions::TINY;
			break;
		case PADDLE_SIZE_SMALL:
			result.height = PaddleSizeOptions::SMALL;
			break;
		case PADDLE_SIZE_MEDIUM:
			result.height = PaddleSizeOptions::MEDIUM;
			break;
		case PADDLE_SIZE_LARGE:
			result.height = PaddleSizeOptions::LARGE;
			break;
		case PADDLE_SIZE_ENORMOUS:
			result.height = PaddleSizeOptions::ENORMOUS;
			break;
		}

		return result;
	}

	float MatchOptionsController::getBallSpeed() {
		float result = 0.0f;

		switch (this->currOptionValueArray[OPTION_BALL_SPEED]) {
		case BALL_SPEED_SLOW:
			result = BallSpeedOptions::SLOW;
			break;
		case BALL_SPEED_NORMAL:
			result = BallSpeedOptions::NORMAL;
			break;
		case BALL_SPEED_FAST:
			result = BallSpeedOptions::FAST;
			break;
		case BALL_SPEED_BLAZING:
			result = BallSpeedOptions::BLAZING;
			break;
		case BALL_SPEED_LUDICROUS:
			result = BallSpeedOptions::LUDICROUS;
			break;
		}

		return result;
	}

	PaddleControlSource MatchOptionsController::getLeftPaddleControlSource() {
		PaddleControlSource result = getPaddleControlSource(this->currOptionValueArray[OPTION_LEFT_PADDLE_CONTROL_SOURCE]);
		return result;
	}

	PaddleControlSource MatchOptionsController::getRightPaddleControlSource() {
		PaddleControlSource result = getPaddleControlSource(this->currOptionValueArray[OPTION_RIGHT_PADDLE_CONTROL_SOURCE]);
		return result;
	}

	PaddleControlSource MatchOptionsController::getPaddleControlSource(int paddleControlSourceValue) {
		PaddleControlSource result = PaddleControlSource::PLAYER;

		switch (paddleControlSourceValue) {
		case PADDLE_CONTROL_SOURCE_AI_GUESSER:
			result = PaddleControlSource::AI_GUESSER;
			break;
		case PADDLE_CONTROL_SOURCE_AI_LATE_FOLLOWER:
			result = PaddleControlSource::AI_LATE_FOLLOWER;
			break;
		case PADDLE_CONTROL_SOURCE_AI_FOLLOWER:
			result = PaddleControlSource::AI_FOLLOWER;
			break;
		case PADDLE_CONTROL_SOURCE_AI_CLOSE_FOLLOWER:
			result = PaddleControlSource::AI_CLOSE_FOLLOWER;
			break;
		case PADDLE_CONTROL_SOURCE_AI_SNOOKER_PRO:
			result = PaddleControlSource::AI_SNOOKER_PRO;
			break;
		}

		return result;
	}

	void MatchOptionsController::update(MatchOptionsInputType input) {
		switch (input) {
		case MatchOptionsInputType::PREV_VALUE:
			this->currOptionValueArray[this->currMatchOption]--;
			if (this->currOptionValueArray[this->currMatchOption] < 0) {
				this->currOptionValueArray[this->currMatchOption] = this->maxOptionValueArray[this->currMatchOption];
			}
			break;
		case MatchOptionsInputType::NEXT_VALUE:
			this->currOptionValueArray[this->currMatchOption]++;
			if (this->currOptionValueArray[this->currMatchOption] > this->maxOptionValueArray[this->currMatchOption]) {
				this->currOptionValueArray[this->currMatchOption] = 0;
			}
			break;
		case MatchOptionsInputType::PREV_OPTION:
			this->currMatchOption--;
			if (this->currMatchOption < 0) {
				this->currMatchOption = OPTION_LAST;
			}
			break;
		case MatchOptionsInputType::NEXT_OPTION:
			this->currMatchOption++;
			if (this->currMatchOption > OPTION_LAST) {
				this->currMatchOption = 0;
			}
			break;
		}
	}

}
