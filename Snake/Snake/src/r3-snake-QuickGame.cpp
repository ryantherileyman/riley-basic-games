
#include <time.h>
#include "includes/r3-snake-quickgamescene.hpp"

namespace r3 {

	namespace snake {

		QuickGame::QuickGame(const QuickGameDefn* quickGameDefn) {
			this->randomizer.seed((unsigned int)time(NULL));

			this->fieldSize = quickGameDefn->fieldSize;
			this->snakeSpeedTilesPerSecond = quickGameDefn->snakeSpeedTilesPerSecond;

			this->snake = new Snake(quickGameDefn->snakeStartDefn);

			this->appleExistsFlag = false;
			this->applePosition = sf::Vector2i(0, 0);

			this->framesSinceSnakeMoved = 0;
			this->queuedSnakeInput = ObjectDirection::NONE;
		}

		QuickGame::~QuickGame() {
			delete this->snake;
		}

		sf::Vector2i QuickGame::getFieldSize() const {
			return this->fieldSize;
		}

		Snake* QuickGame::getSnake() const {
			return this->snake;
		}

		bool QuickGame::getAppleExists() const {
			return this->appleExistsFlag;
		}

		sf::Vector2i QuickGame::getApplePosition() const {
			return this->applePosition;
		}

		QuickGameUpdateResult QuickGame::update(const QuickGameInputRequest* input) {
			QuickGameUpdateResult result;
			result.snakeMovementResult = ObjectDirection::NONE;
			result.snakeHitBarrierFlag = false;
			result.snakeAteAppleFlag = false;

			if (!this->appleExistsFlag) {
				this->applePosition = this->resolveNewApplePosition();
				this->appleExistsFlag = true;
			}

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

					if (this->snake->getHead().position == this->applePosition) {
						result.snakeAteAppleFlag = true;
						this->appleExistsFlag = false;
					}
				}

				this->framesSinceSnakeMoved = 0;
			}

			return result;
		}

		sf::Vector2i QuickGame::resolveNewApplePosition() {
			sf::Vector2i result;

			std::uniform_int_distribution<int> xPositionDistribution(1, this->fieldSize.x - 2);
			std::uniform_int_distribution<int> yPositionDistribution(1, this->fieldSize.y - 2);

			bool foundFreePositionFlag = false;
			while (!foundFreePositionFlag) {
				result.x = xPositionDistribution(this->randomizer);
				result.y = yPositionDistribution(this->randomizer);

				foundFreePositionFlag = !this->snake->occupiesPosition(result);
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
