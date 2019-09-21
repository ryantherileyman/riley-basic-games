
#include <time.h>
#include <math.h>
#include "../includes/r3-snake-storymodescene.hpp"

namespace r3 {

	namespace snake {

		namespace StoryGameScoreUtils {

			int getBaseFoodScore(StoryFoodType foodType) {
				int result = 0;

				switch (foodType) {
				case StoryFoodType::APPLE:
					result = 10;
					break;
				}

				return result;
			}

			StoryFoodEatenBySnakeScoreResult calcFoodEatenScore(StoryFoodType foodType, const StoryFoodTileDistanceTracking& tileDistanceTracking) {
				StoryFoodEatenBySnakeScoreResult result;

				result.baseScore = getBaseFoodScore(foodType);

				result.bonusPathScore = 0;
				if (tileDistanceTracking.tileDistanceSnakeTravelled <= (tileDistanceTracking.origTileDistanceFromSnake * 2)) {
					float pathPct = 1.0f - (float)(tileDistanceTracking.tileDistanceSnakeTravelled - tileDistanceTracking.origTileDistanceFromSnake) / (float)tileDistanceTracking.origTileDistanceFromSnake;
					result.bonusPathScore = (int)floorf((float)result.baseScore * pathPct);
				}

				result.perfectPathScore = 0;
				if (tileDistanceTracking.tileDistanceSnakeTravelled <= tileDistanceTracking.origTileDistanceFromSnake ) {
					result.perfectPathScore = result.baseScore;
				}

				result.totalScore = result.baseScore + result.bonusPathScore + result.perfectPathScore;

				printf("Scoring for food eaten: Base=%d + BonusPath=%d + PerfectPath=%d = %d\n", result.baseScore, result.bonusPathScore, result.perfectPathScore, result.totalScore);

				return result;
			}

			int calcTotalScore(const std::unordered_map<int, StoryFoodEatenBySnakeScoreResult>& foodEatenBySnakeScoreResultMap) {
				int result = 0;
				for (const auto& currScoreResultPair : foodEatenBySnakeScoreResultMap) {
					result += currScoreResultPair.second.totalScore;
				}
				return result;
			}

		}

		StoryGame::StoryGame() {
			this->randomizer.seed((unsigned int)time(NULL));
			this->levelDefn = nullptr;
			this->map = nullptr;
			this->snake = nullptr;
			this->snakeSpeedTilesPerSecond = 0.0f;
			this->snakeHealth = 0.0f;
			this->framesSinceSnakeMoved = 0;
			this->queuedSnakeGrowth = 0;
			this->nextFoodInstanceId = 1;
			this->score = 0;
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
			this->snakeHealth = (float)levelDefn.maxSnakeHealth;
			this->framesSinceSnakeMoved = 0;
			while (!this->snakeMovementQueue.empty()) {
				this->snakeMovementQueue.pop();
			}

			this->foodSpawnTrackerList.clear();
			for (auto const& currFoodDefn : levelDefn.foodDefnList) {
				this->foodSpawnTrackerList.push_back(StoryFoodSpawnTracker(currFoodDefn));
			}

			this->foodTileDistanceTrackingMap.clear();
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

		float StoryGame::getCurrSnakeHealth() const {
			return this->snakeHealth;
		}

		float StoryGame::getMaxSnakeHealth() const {
			return (float)this->levelDefn->maxSnakeHealth;
		}

		const std::vector<StoryFoodSpawnTracker>& StoryGame::getFoodSpawnTrackerList() const {
			return this->foodSpawnTrackerList;
		}

		int StoryGame::getScore() const {
			return this->score;
		}

		StoryGameUpdateResult StoryGame::update(const StoryGameInputRequest& input) {
			StoryGameUpdateResult result;
			result.snakeMovementResult = ObjectDirection::NONE;
			result.snakeHitBarrierFlag = false;
			result.snakeDiedFlag = false;
			result.snakeGrewFlag = false;
			result.spawnedFoodInstanceList = this->checkForFoodSpawns();

			this->addNewFoodSpawnsToFoodTileDistanceTrackingMap(result.spawnedFoodInstanceList);

			this->acceptSnakeMovementList(input.snakeMovementList);
			this->consumeAllUnusableSnakeInputs();

			this->framesSinceSnakeMoved++;
			if (this->framesSinceSnakeMoved >= (60.0f / this->snakeSpeedTilesPerSecond)) {
				ObjectDirection directionToMoveSnake = this->resolveDirectionToMoveSnake();
				if (this->snakeWouldHitBarrier(directionToMoveSnake)) {
					result.snakeHitBarrierFlag = true;

					this->snakeHealth -= 1.0f;
					result.snakeDiedFlag = (this->snakeHealth <= 0.0f);
				}
				else {
					result.snakeMovementResult = directionToMoveSnake;
					result.snakeGrewFlag = this->moveSnakeForward(directionToMoveSnake);
					this->updateFoodTileDistanceTrackingMapAfterSnakeMoved();

					StoryCheckForFoodEatenBySnakeResult eatenResult = this->checkForFoodEatenBySnake();
					result.eatenBySnakeFoodInstanceList = eatenResult.eatenBySnakeFoodInstanceList;
					this->queuedSnakeGrowth += eatenResult.snakeGrowth;

					std::unordered_map<int, StoryFoodEatenBySnakeScoreResult> foodEatenBySnakeScoreResultMap = this->buildFoodEatenBySnakeScoreResultMap(eatenResult.eatenBySnakeFoodInstanceList);
					this->score += StoryGameScoreUtils::calcTotalScore(foodEatenBySnakeScoreResultMap);
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
				checkInput.snakeLength = this->getSnake()->getLength();

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

		void StoryGame::addNewFoodSpawnsToFoodTileDistanceTrackingMap(const std::vector<StoryFoodInstance> spawnedFoodInstanceList) {
			sf::Vector2i headPosition = this->getSnake()->getHead().position;
			for (auto const& currSpawnedFoodInstance : spawnedFoodInstanceList) {
				StoryFoodTileDistanceTracking currFoodTileDistanceTracking;
				currFoodTileDistanceTracking.origTileDistanceFromSnake =
					abs(currSpawnedFoodInstance.position.x - headPosition.x) +
					abs(currSpawnedFoodInstance.position.y - headPosition.y);
				currFoodTileDistanceTracking.tileDistanceSnakeTravelled = 0;

				this->foodTileDistanceTrackingMap[currSpawnedFoodInstance.foodInstanceId] = currFoodTileDistanceTracking;
			}
		}

		void StoryGame::updateFoodTileDistanceTrackingMapAfterSnakeMoved() {
			for (auto const& currFoodTileDistanceTrackingPair : this->foodTileDistanceTrackingMap) {
				StoryFoodTileDistanceTracking currFoodTileDistanceTracking = currFoodTileDistanceTrackingPair.second;
				currFoodTileDistanceTracking.tileDistanceSnakeTravelled++;

				this->foodTileDistanceTrackingMap[currFoodTileDistanceTrackingPair.first] = currFoodTileDistanceTracking;
			}
		}

		std::unordered_map<int, StoryFoodEatenBySnakeScoreResult> StoryGame::buildFoodEatenBySnakeScoreResultMap(const std::vector<StoryFoodInstance> eatenBySnakeFoodInstanceList) {
			std::unordered_map<int, StoryFoodEatenBySnakeScoreResult> result;

			for (auto const& currFoodInstance : eatenBySnakeFoodInstanceList) {
				StoryFoodTileDistanceTracking currFoodTileDistanceTracking = this->foodTileDistanceTrackingMap[currFoodInstance.foodInstanceId];

				// TODO: want to determine the actual food type based on the foodInstanceId somehow...
				result[currFoodInstance.foodInstanceId] = StoryGameScoreUtils::calcFoodEatenScore(StoryFoodType::APPLE, currFoodTileDistanceTracking);
			}

			return result;
		}

	}

}
