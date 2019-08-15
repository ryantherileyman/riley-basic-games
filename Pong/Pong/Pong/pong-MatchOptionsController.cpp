
#include "pong-lib.h"

namespace pong {

	using namespace r3::graphics2d;

	/* Available game options */
	const int OPTION_PADDLE_SIZE = 0;
	const int OPTION_LAST = OPTION_PADDLE_SIZE;

	/* Paddle option values */
	namespace PaddleSizeOptions {
		const float TINY = 25.0f;
		const float SMALL = 50.0f;
		const float MEDIUM = 80.0f;
		const float LARGE = 110.0f;
		const float ENORMOUS = 180.0f;
	}
	const int PADDLE_SIZE_LAST = MatchOptionsController::PADDLE_SIZE_ENORMOUS;

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

	MatchOptionsController::MatchOptionsController(const MatchDefn* matchDefn) {
		this->currMatchOption = OPTION_PADDLE_SIZE;

		this->currOptionValueArray[OPTION_PADDLE_SIZE] = resolvePaddleSizeOptionValue(matchDefn->paddleSize);

		this->maxOptionValueArray[OPTION_PADDLE_SIZE] = PADDLE_SIZE_LAST;
	}

	int MatchOptionsController::getCurrOption() const {
		return this->currMatchOption;
	}

	int MatchOptionsController::getPaddleSizeOptionValue() const {
		return this->currOptionValueArray[OPTION_PADDLE_SIZE];
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
		}
	}

}
