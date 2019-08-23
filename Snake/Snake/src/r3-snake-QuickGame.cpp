
#include "includes/r3-snake-quickgamescene.hpp"

namespace r3 {

	namespace snake {

		QuickGame::QuickGame(const QuickGameDefn* quickGameDefn) {
			this->fieldSize.x = 50;
			this->fieldSize.y = 25;

			this->snakeSpeedTilesPerSecond = 4.0f;

			SnakeStartDefn snakeStartDefn;
			snakeStartDefn.headPosition.x = 25;
			snakeStartDefn.headPosition.y = 10;
			snakeStartDefn.facingDirection = ObjectDirection::DOWN;
			snakeStartDefn.length = 3;

			this->snake = new Snake(snakeStartDefn);

			this->framesSinceSnakeMoved = 0;
			this->queuedSnakeInput = ObjectDirection::NONE;
		}

		QuickGame::~QuickGame() {
			delete this->snake;
		}

		QuickGameUpdateResult QuickGame::update(const QuickGameInputRequest* input) {
			QuickGameUpdateResult result;
			result.snakeMovementResult = ObjectDirection::NONE;
			result.snakeHitBarrierFlag = false;

			this->framesSinceSnakeMoved++;

			if (this->snake->isValidMovementDirection(input->snakeMovementInput)) {
				this->queuedSnakeInput = input->snakeMovementInput;
			}

			if (this->framesSinceSnakeMoved >= (60.0f / this->snakeSpeedTilesPerSecond)) {
				ObjectDirection directionToMoveSnake = this->queuedSnakeInput;
				if (directionToMoveSnake == ObjectDirection::NONE) {
					directionToMoveSnake = this->snake->getHead().enterDirection;
				}

				if (this->snakeWouldHitBarrier(directionToMoveSnake)) {
					result.snakeHitBarrierFlag = true;
				} else {
					this->snake->moveForward(directionToMoveSnake);
					result.snakeMovementResult = directionToMoveSnake;
				}

				this->framesSinceSnakeMoved = 0;
			}

			return result;
		}

		bool QuickGame::snakeWouldHitBarrier(ObjectDirection direction) {
			sf::Vector2i newHeadPosition = this->snake->getHead().position + SnakeUtils::directionToVector(direction);

			// TODO: this should also check if any of the snake's body segments occupies this position

			bool result =
				(newHeadPosition.x <= 0) ||
				(newHeadPosition.x >= (this->fieldSize.x - 1)) ||
				(newHeadPosition.y <= 0) ||
				(newHeadPosition.y >= (this->fieldSize.y - 1));
			return result;
		}

	}

}
