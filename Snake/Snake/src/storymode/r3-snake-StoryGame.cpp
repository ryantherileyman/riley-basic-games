
#include "../includes/r3-snake-storymodescene.hpp"

namespace r3 {

	namespace snake {

		StoryGame::StoryGame() {
			this->map = nullptr;
			this->snake = nullptr;
			this->snakeSpeedTilesPerSecond = 0.0f;
			this->framesSinceSnakeMoved = 0;
		}

		StoryGame::~StoryGame() {
			this->freeLevel();
		}

		void StoryGame::startNewLevel(const StoryMapDefn& mapDefn, const StoryLevelDefn& levelDefn) {
			this->freeLevel();

			this->map = new StoryMap(mapDefn);
			this->snake = new Snake(levelDefn.snakeStart);

			this->snakeSpeedTilesPerSecond = (float)levelDefn.snakeSpeedTilesPerSecond;
			this->framesSinceSnakeMoved = 0;
			while (!this->snakeMovementQueue.empty()) {
				this->snakeMovementQueue.pop();
			}
		}

		StoryMap* StoryGame::getMap() const {
			return this->map;
		}

		Snake* StoryGame::getSnake() const {
			return this->snake;
		}

		StoryGameUpdateResult StoryGame::update(const StoryGameInputRequest& input) {
			StoryGameUpdateResult result;
			result.snakeMovementResult = ObjectDirection::NONE;
			result.snakeHitBarrierFlag = false;

			this->acceptSnakeMovementList(input.snakeMovementList);
			this->consumeAllUnusableSnakeInputs();

			this->framesSinceSnakeMoved++;
			if (this->framesSinceSnakeMoved >= (60.0f / this->snakeSpeedTilesPerSecond)) {
				ObjectDirection directionToMoveSnake = this->resolveDirectionToMoveSnake();
				if (this->snakeWouldHitBarrier(directionToMoveSnake)) {
					result.snakeHitBarrierFlag = true;
				}
				else {
					result.snakeMovementResult = directionToMoveSnake;
					this->snake->moveForward(directionToMoveSnake);
				}

				this->framesSinceSnakeMoved -= (int)(60.0f / this->snakeSpeedTilesPerSecond);
			}

			return result;
		}

		void StoryGame::freeLevel() {
			if (this->map != nullptr) {
				delete this->map;
				this->map = nullptr;
			}
			if (this->snake != nullptr) {
				delete this->snake;
				this->snake = nullptr;
			}
		}

		void StoryGame::acceptSnakeMovementList(const std::vector<ObjectDirection>& snakeMovementList) {
			for (auto const& currSnakeMovement : snakeMovementList) {
				this->snakeMovementQueue.push(currSnakeMovement);
			}
		}

		void StoryGame::consumeAllUnusableSnakeInputs() {
			bool allUnusableInputsPoppedFlag = false;
			while (!allUnusableInputsPoppedFlag) {
				allUnusableInputsPoppedFlag = this->snakeMovementQueue.empty();
				if (!this->snakeMovementQueue.empty()) {
					allUnusableInputsPoppedFlag =
						(this->snakeMovementQueue.front() != this->snake->getHead().enterDirection) &&
						(this->snake->isValidMovementDirection(this->snakeMovementQueue.front()));

					if (!allUnusableInputsPoppedFlag) {
						this->snakeMovementQueue.pop();
					}
				}
			}
		}

		ObjectDirection StoryGame::resolveDirectionToMoveSnake() {
			ObjectDirection result = ObjectDirection::NONE;

			if (!this->snakeMovementQueue.empty()) {
				result = this->snakeMovementQueue.front();
				this->snakeMovementQueue.pop();
			}

			if (result == ObjectDirection::NONE) {
				result = this->snake->getHead().enterDirection;
			}

			return result;
		}

		bool StoryGame::snakeWouldHitBarrier(ObjectDirection direction) {
			sf::Vector2i newHeadPosition = this->snake->getHead().position + SnakeUtils::directionToVector(direction);

			bool result =
				(newHeadPosition.x < 0) ||
				(newHeadPosition.x >= this->getMap()->getFieldSize().x) ||
				(newHeadPosition.y < 0) ||
				(newHeadPosition.y >= this->getMap()->getFieldSize().y) ||
				(this->map->barrierAt(newHeadPosition.x, newHeadPosition.y)) ||
				(this->snake->bodyOccupiesPosition(newHeadPosition));
			return result;
		}

	}

}
