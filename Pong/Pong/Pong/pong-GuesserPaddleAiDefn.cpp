
#include <time.h>
#include "pong-lib.h"

namespace pong {

	GuesserPaddleAi::GuesserPaddleAi() {
		this->generator.seed(time(NULL));
	}

	PaddleInputType GuesserPaddleAi::resolvePaddleInputType(PaddleAiInput input) {
		PaddleInputType result = this->currInput;

		int threshold = 95;
		if (fabsf(input.paddle->getPosition().y) >= input.match->getCourtSize().height / 2.0f - input.paddle->getSize().height / 2.0f) {
			threshold = 50;
		}

		int chance = this->changeDirectionChanceDistribution(this->generator);

		if (chance > threshold) {
			int newDirection = this->newDirectionDistribution(this->generator);
			
			switch (newDirection) {
			case 1:
				result = PaddleInputType::NONE;
				break;
			case 2:
				result = PaddleInputType::MOVE_UP;
				break;
			case 3:
				result = PaddleInputType::MOVE_DOWN;
				break;
			}

			this->currInput = result;
		}

		return result;
	}

}
