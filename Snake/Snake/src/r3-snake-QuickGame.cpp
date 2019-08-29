
#include <time.h>
#include "includes/r3-snake-quickgamescene.hpp"

namespace r3 {

	namespace snake {

		QuickGame::QuickGame(const QuickGameDefn* quickGameDefn) {
			this->randomizer.seed((unsigned int)time(NULL));

			this->fieldSize = quickGameDefn->fieldSize;
			this->snakeSpeedTilesPerSecond = quickGameDefn->snakeSpeedTilesPerSecond;
			this->snakeGrowthPerApple = quickGameDefn->snakeGrowthPerApple;

			this->snake = new Snake(quickGameDefn->snakeStartDefn);

			this->appleExistsFlag = false;
			this->applePosition = sf::Vector2i(0, 0);

			this->framesSinceSnakeMoved = 0;
			this->queuedSnakeGrowth = 0;
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
			result.snakeGrewFlag = false;

			if (!this->appleExistsFlag) {
				this->applePosition = this->resolveNewApplePosition();
				this->appleExistsFlag = true;
			}

			this->framesSinceSnakeMoved++;

			if (this->snake->isValidMovementDirection(input->snakeMovementInput)) {
				this->snakeInputQueue.push(input->snakeMovementInput);
			}

			if (this->framesSinceSnakeMoved >= (60.0f / this->snakeSpeedTilesPerSecond)) {
				ObjectDirection directionToMoveSnake = ObjectDirection::NONE;

				// First pop all queued inputs that are going the same direction that the snake is already going
				// This is necessary in the case that the user held down an arrow key
				bool allSameDirectionPoppedFlag = false;
				while (!allSameDirectionPoppedFlag) {
					allSameDirectionPoppedFlag = this->snakeInputQueue.empty();
					if (!this->snakeInputQueue.empty()) {
						allSameDirectionPoppedFlag = (this->snakeInputQueue.front() != this->snake->getHead().enterDirection);
						if (!allSameDirectionPoppedFlag) {
							this->snakeInputQueue.pop();
						}
					}
				}

				// Now pull off the next element for an actual change in direction
				if (!this->snakeInputQueue.empty()) {
					directionToMoveSnake = this->snakeInputQueue.front();
					this->snakeInputQueue.pop();
				}

				if (directionToMoveSnake == ObjectDirection::NONE) {
					directionToMoveSnake = this->snake->getHead().enterDirection;
				}

				if (this->snakeWouldHitBarrier(directionToMoveSnake)) {
					result.snakeHitBarrierFlag = true;
				} else {
					if (this->queuedSnakeGrowth > 0) {
						this->snake->growForward(directionToMoveSnake);
						this->queuedSnakeGrowth--;

						result.snakeGrewFlag = true;
					}
					else {
						this->snake->moveForward(directionToMoveSnake);
					}

					result.snakeMovementResult = directionToMoveSnake;

					if (this->snake->getHead().position == this->applePosition) {
						result.snakeAteAppleFlag = true;

						this->appleExistsFlag = false;
						this->queuedSnakeGrowth += this->snakeGrowthPerApple;
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

			bool result =
				(newHeadPosition.x <= 0) ||
				(newHeadPosition.x >= (this->fieldSize.x - 1)) ||
				(newHeadPosition.y <= 0) ||
				(newHeadPosition.y >= (this->fieldSize.y - 1)) ||
				this->snake->bodyOccupiesPosition(newHeadPosition);
			return result;
		}

	}

}
