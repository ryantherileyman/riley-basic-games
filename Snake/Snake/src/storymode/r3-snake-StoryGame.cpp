
#include <time.h>
#include "../includes/r3-snake-storymodescene.hpp"

namespace r3 {

	namespace snake {

		StoryGame::StoryGame() {
			this->randomizer.seed((unsigned int)time(NULL));
			this->levelDefn = nullptr;
			this->map = nullptr;
			this->snake = nullptr;
			this->snakeSpeedTilesPerSecond = 0.0f;
			this->framesSinceSnakeMoved = 0;
			this->queuedSnakeGrowth = 0;
			this->nextFoodInstanceId = 1;
		}

		StoryGame::~StoryGame() {
			this->freeLevel();
		}

		void StoryGame::startNewLevel(const StoryMapDefn& mapDefn, const StoryLevelDefn& levelDefn) {
			this->freeLevel();

			this->levelDefn = &levelDefn;
			this->map = new StoryMap(mapDefn);
			this->snake = new Snake(levelDefn.snakeStart);

			this->snakeSpeedTilesPerSecond = (float)levelDefn.snakeSpeedTilesPerSecond;
			this->framesSinceSnakeMoved = 0;
			while (!this->snakeMovementQueue.empty()) {
				this->snakeMovementQueue.pop();
			}

			this->foodSpawnTrackerList.clear();
			for (auto const& currFoodDefn : levelDefn.foodDefnList) {
				this->foodSpawnTrackerList.push_back(StoryFoodSpawnTracker(currFoodDefn));
			}
		}

		void StoryGame::startRunningLevel() {
			this->clock.restart();
			printf("Clock restarted, is at %f seconds\n", this->clock.getElapsedTime().asSeconds());
		}

		StoryMap* StoryGame::getMap() const {
			return this->map;
		}

		Snake* StoryGame::getSnake() const {
			return this->snake;
		}

		const std::vector<StoryFoodSpawnTracker>& StoryGame::getFoodSpawnTrackerList() const {
			return this->foodSpawnTrackerList;
		}

		StoryGameUpdateResult StoryGame::update(const StoryGameInputRequest& input) {
			StoryGameUpdateResult result;
			result.snakeMovementResult = ObjectDirection::NONE;
			result.snakeHitBarrierFlag = false;
			result.spawnedFoodInstanceList = this->checkForFoodSpawns();

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
					result.snakeGrewFlag = this->moveSnakeForward(directionToMoveSnake);

					StoryCheckForFoodEatenBySnakeResult eatenResult = this->checkForFoodEatenBySnake();
					result.eatenBySnakeFoodInstanceList = eatenResult.eatenBySnakeFoodInstanceList;
					this->queuedSnakeGrowth += eatenResult.snakeGrowth;
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

		bool StoryGame::moveSnakeForward(ObjectDirection directionToMoveSnake) {
			bool result = false;

			if (this->queuedSnakeGrowth > 0) {
				this->snake->growForward(directionToMoveSnake);
				this->queuedSnakeGrowth--;

				result = true;
			}
			else {
				this->snake->moveForward(directionToMoveSnake);
			}

			return result;
		}

		std::vector<StoryFoodInstance> StoryGame::checkForFoodSpawns() {
			std::vector<StoryFoodInstance> result;

			for (auto& currFoodSpawnTracker : this->foodSpawnTrackerList) {
				StoryFoodSpawnCheckInput checkInput;
				checkInput.timeSinceLevelStarted = this->clock.getElapsedTime();

				if (currFoodSpawnTracker.shouldFoodSpawn(checkInput)) {
					std::vector<sf::Vector2i> availablePositionList = this->buildAvailableFoodSpawnPositionList(currFoodSpawnTracker.getFoodDefn());
					printf("There are %d positions on the map that the food can spawn\n", availablePositionList.size());
					if (availablePositionList.size() > 0) {
						StoryFoodInstance newFoodInstance = this->createFoodInstance(availablePositionList);
						currFoodSpawnTracker.spawnFood(newFoodInstance);
						result.push_back(newFoodInstance);

						this->nextFoodInstanceId++;
					}
				}
			}

			return result;
		}

		StoryCheckForFoodEatenBySnakeResult StoryGame::checkForFoodEatenBySnake() {
			StoryCheckForFoodEatenBySnakeResult result;
			result.snakeGrowth = 0;

			sf::Vector2i headPosition = this->snake->getHead().position;

			for (auto& currFoodSpawnTracker : this->foodSpawnTrackerList) {
				StoryFoodPositionCheckResult checkResult = currFoodSpawnTracker.occupiesPosition(headPosition);
				if (checkResult.foodExistsFlag) {
					result.eatenBySnakeFoodInstanceList.push_back(currFoodSpawnTracker.getFoodInstance(checkResult.foodInstanceId));
					result.snakeGrowth += currFoodSpawnTracker.getFoodDefn().growthRate;

					currFoodSpawnTracker.despawnFood(checkResult.foodInstanceId);
				}
			}

			return result;
		}

		std::vector<sf::Vector2i> StoryGame::buildAvailableFoodSpawnPositionList(const StoryFoodDefn& foodDefn) {
			std::vector<sf::Vector2i> result;

			for (int y = 0; y < this->map->getFieldSize().y; y++) {
				for (int x = 0; x < this->map->getFieldSize().x; x++) {
					int floorId = this->map->getFloorId(x, y);

					bool positionIsAvailable =
						!this->map->barrierAt(x, y) &&
						(floorId >= foodDefn.minFloorId) &&
						(floorId <= foodDefn.maxFloorId) &&
						!this->snake->occupiesPosition(sf::Vector2i(x, y));
					// TODO: also want to ensure no food object is present in this position?  (at least of the same food type)

					if (positionIsAvailable) {
						result.push_back(sf::Vector2i(x, y));
					}
				}
			}

			return result;
		}

		StoryFoodInstance StoryGame::createFoodInstance(const std::vector<sf::Vector2i>& availablePositionList) {
			std::uniform_int_distribution<int> tileDistribution(0, availablePositionList.size() - 1);
			int index = tileDistribution(this->randomizer);

			StoryFoodInstance result;
			result.foodInstanceId = this->nextFoodInstanceId;
			result.position = availablePositionList[index];
			return result;
		}

	}

}
