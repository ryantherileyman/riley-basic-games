
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
				case StoryFoodType::CARROT:
					result = 2;
					break;
				case StoryFoodType::GREEN_APPLE:
					result = 20;
					break;
				case StoryFoodType::CHILI_PEPPER:
					result = 30;
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
			this->status = StoryGameStatus::NOT_STARTED;
			this->timeElapsedWhenEnded = sf::Time();
			this->levelDefn = nullptr;
			this->map = nullptr;
			this->snake = nullptr;
			this->snakeHealth = 0.0f;
			this->framesSinceSnakeMoved = 0;
			this->queuedSnakeGrowth = 0;
			this->nextSnakeMovementModifierId = 1;
			this->nextFoodInstanceId = 1;
			this->nextDangerInstanceId = 1;
			this->score = 0;
		}

		StoryGame::~StoryGame() {
			this->freeLevel();
		}

		void StoryGame::startNewCampaign() {
			this->score = 0;
		}

		void StoryGame::startNewLevel(const StoryMapDefn& mapDefn, const StoryLevelDefn& levelDefn) {
			this->freeLevel();

			this->status = StoryGameStatus::NOT_STARTED;

			this->levelDefn = &levelDefn;
			this->map = new StoryMap(mapDefn);
			this->snake = new Snake(levelDefn.snakeStart);

			this->snakeHealth = (float)levelDefn.maxSnakeHealth;
			this->framesSinceSnakeMoved = 0;
			while (!this->snakeMovementQueue.empty()) {
				this->snakeMovementQueue.pop();
			}
			this->queuedSnakeGrowth = 0;

			this->nextSnakeMovementModifierId = 1;
			this->snakeMovementModifierMap.clear();

			this->nextFoodInstanceId = 1;
			this->foodSpawnTrackerList.clear();
			for (auto const& currFoodDefn : levelDefn.foodDefnList) {
				this->foodSpawnTrackerList.push_back(StoryFoodSpawnTracker(currFoodDefn));
			}

			this->foodTileDistanceTrackingMap.clear();

			this->foodEatenCountMap.clear();
			for (auto const& currFoodDefn : levelDefn.foodDefnList) {
				this->foodEatenCountMap[currFoodDefn.foodType] = 0;
			}

			this->nextDangerInstanceId = 1;
			this->dangerSpawnTrackerList.clear();
			for (auto const& currDangerDefn : levelDefn.dangerDefnList) {
				this->dangerSpawnTrackerList.push_back(StoryDangerSpawnTracker(currDangerDefn));
			}

			this->dangerStruckSnakeMap.clear();

			this->remainingSoundFxDefnSet.clear();
			for (auto const& currSoundFxDefn : levelDefn.soundFxDefnList) {
				this->remainingSoundFxDefnSet.insert(&currSoundFxDefn);
			}
		}

		void StoryGame::startRunningLevel() {
			this->clock.restart();
			this->status = StoryGameStatus::RUNNING;
			printf("Clock restarted, is at %f seconds\n", this->clock.getElapsedTime().asSeconds());
		}

		void StoryGame::stopRunningLevel() {
			this->status = StoryGameStatus::ENDED;
			this->timeElapsedWhenEnded = this->clock.getElapsedTime();
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

		const std::vector<StoryDangerSpawnTracker>& StoryGame::getDangerSpawnTrackerList() const {
			return this->dangerSpawnTrackerList;
		}

		int StoryGame::getFoodEaten(StoryFoodType foodType) const {
			int result = this->foodEatenCountMap.at(foodType);
			return result;
		}

		sf::Time StoryGame::getTimeElapsed() const {
			sf::Time result;

			switch (this->status) {
			case StoryGameStatus::RUNNING:
				result = this->clock.getElapsedTime();
				break;
			case StoryGameStatus::ENDED:
				result = this->timeElapsedWhenEnded;
				break;
			}

			return result;
		}

		const StoryWinCondition& StoryGame::getWinCondition() const {
			return this->levelDefn->winCondition;
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
			result.completedLevelFlag = false;
			result.spawnedFoodInstanceList = this->checkForFoodSpawns();
			result.spawnedDangerInstanceList = this->checkForDangerSpawns();

			this->updateSnakeMovementModifierMap();
			float snakeSpeedTilesPerSecond = this->resolveSnakeSpeedTilesPerSecond();

			this->checkForDangerDespawns();

			this->addNewFoodSpawnsToFoodTileDistanceTrackingMap(result.spawnedFoodInstanceList);
			this->addNewDangerSpawnsToDangerStruckSnakeMap(result.spawnedDangerInstanceList);

			this->acceptSnakeMovementList(input.snakeMovementList);
			this->consumeAllUnusableSnakeInputs();

			this->framesSinceSnakeMoved++;
			if (this->framesSinceSnakeMoved >= (60.0f / snakeSpeedTilesPerSecond)) {
				ObjectDirection directionToMoveSnake = this->resolveDirectionToMoveSnake();
				if (this->snakeWouldHitBarrier(directionToMoveSnake)) {
					result.snakeHitBarrierFlag = true;
					this->updateHealthBy(-1.0f);
				}
				else {
					result.snakeMovementResult = directionToMoveSnake;
					result.snakeGrewFlag = this->moveSnakeForward(directionToMoveSnake);
					this->updateFoodTileDistanceTrackingMapAfterSnakeMoved();

					StoryCheckForFoodEatenBySnakeResult eatenResult = this->checkForFoodEatenBySnake();
					this->queuedSnakeGrowth += eatenResult.snakeGrowth;

					for (auto const& currEatenFood : eatenResult.eatenBySnakeFoodInstanceList) {
						this->foodEatenCountMap[currEatenFood.foodType] = this->foodEatenCountMap.at(currEatenFood.foodType) + 1;

						if (currEatenFood.foodType == StoryFoodType::CARROT) {
							this->updateHealthBy(0.1f);
						}
						else if (currEatenFood.foodType == StoryFoodType::GREEN_APPLE) {
							this->updateHealthBy(1.0f);
						}
						else if (currEatenFood.foodType == StoryFoodType::CHILI_PEPPER) {
							this->addNewSnakeMovementModifier(1.33f, 2.0f);
						}
					}

					std::unordered_map<int, StoryFoodEatenBySnakeScoreResult> foodEatenBySnakeScoreResultMap = this->buildFoodEatenBySnakeScoreResultMap(eatenResult.eatenBySnakeFoodInstanceList);
					this->score += StoryGameScoreUtils::calcTotalScore(foodEatenBySnakeScoreResultMap);

					for (auto const& currEatenFood : eatenResult.eatenBySnakeFoodInstanceList) {
						StoryFoodEatenResult currFoodEatenResult;
						currFoodEatenResult.foodInstance = currEatenFood;
						currFoodEatenResult.scoreResult = foodEatenBySnakeScoreResultMap.at(currEatenFood.foodInstanceId);

						result.foodEatenResultList.push_back(currFoodEatenResult);
					}
					
					result.dangerInstanceStruckSnakeList = this->checkForDangersStrikingSnake();
					for (auto const& currDangerInstance : result.dangerInstanceStruckSnakeList) {
						this->updateHealthBy(-1.0f);
					}

					result.completedLevelFlag = this->checkLevelCompleted();
				}

				result.snakeDiedFlag = (this->snakeHealth <= 0.0f);

				this->framesSinceSnakeMoved -= (int)(60.0f / snakeSpeedTilesPerSecond);
			}

			result.soundFileTriggeredList = this->buildSoundFileTriggeredList(result);

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

		void StoryGame::updateSnakeMovementModifierMap() {
			std::vector<int> modifierIdListToRemove;
			for (auto const& currSnakeMovementModifierPair : this->snakeMovementModifierMap) {
				float timeModifierEnds = currSnakeMovementModifierPair.second.timeModifierBegan.asSeconds() + currSnakeMovementModifierPair.second.secondsToModify;
				if (this->clock.getElapsedTime().asSeconds() > timeModifierEnds) {
					modifierIdListToRemove.push_back(currSnakeMovementModifierPair.first);
				}
			}

			for (int currModifierId : modifierIdListToRemove) {
				printf("Removing movement multiplier of %f\n", this->snakeMovementModifierMap[currModifierId].movementMultiplier);

				this->snakeMovementModifierMap.erase(currModifierId);
			}

			if (!modifierIdListToRemove.empty()) {
				printf("New speed will be %f\n", this->resolveSnakeSpeedTilesPerSecond());
			}
		}

		float StoryGame::resolveSnakeSpeedTilesPerSecond() {
			float result = (float)this->levelDefn->snakeSpeedTilesPerSecond;
			for (auto const& currSnakeMovementModifierPair : this->snakeMovementModifierMap) {
				result *= currSnakeMovementModifierPair.second.movementMultiplier;
			}
			return result;
		}

		void StoryGame::addNewSnakeMovementModifier(float movementMultiplier, float secondsToApply) {
			printf("Adding movement multiplier of %f\n", movementMultiplier);

			this->snakeMovementModifierMap[this->nextSnakeMovementModifierId] = StorySnakeMovementModifier();
			this->snakeMovementModifierMap[this->nextSnakeMovementModifierId].movementMultiplier = movementMultiplier;
			this->snakeMovementModifierMap[this->nextSnakeMovementModifierId].secondsToModify = secondsToApply;
			this->snakeMovementModifierMap[this->nextSnakeMovementModifierId].timeModifierBegan = this->clock.getElapsedTime();

			this->nextSnakeMovementModifierId++;

			printf("New speed will be %f\n", this->resolveSnakeSpeedTilesPerSecond());
		}

		std::vector<StoryFoodInstance> StoryGame::checkForFoodSpawns() {
			std::vector<StoryFoodInstance> result;

			for (auto& currFoodSpawnTracker : this->foodSpawnTrackerList) {
				StoryFoodSpawnCheckInput checkInput;
				checkInput.timeSinceLevelStarted = this->clock.getElapsedTime();
				checkInput.randomizer = &this->randomizer;
				checkInput.snakeLength = this->getSnake()->getLength();
				checkInput.snakeHealth = this->snakeHealth;

				if (currFoodSpawnTracker.shouldFoodSpawn(checkInput)) {
					std::vector<sf::Vector2i> availablePositionList = this->buildAvailableFoodSpawnPositionList(currFoodSpawnTracker.getFoodDefn());
					// TODO printf("There are %d positions on the map that the food can spawn\n", availablePositionList.size());
					if (availablePositionList.size() > 0) {
						StoryFoodInstance newFoodInstance = this->createFoodInstance(currFoodSpawnTracker.getFoodDefn().foodType, availablePositionList);
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
						!this->snake->occupiesPosition(sf::Vector2i(x, y)) &&
						!this->foodOccupiesPosition(sf::Vector2i(x, y));

					if (positionIsAvailable) {
						result.push_back(sf::Vector2i(x, y));
					}
				}
			}

			return result;
		}

		bool StoryGame::foodOccupiesPosition(const sf::Vector2i& position) {
			bool result = false;

			for (auto const& currFoodSpawnTracker : this->foodSpawnTrackerList) {
				StoryFoodPositionCheckResult checkResult = currFoodSpawnTracker.occupiesPosition(position);
				if (checkResult.foodExistsFlag) {
					result = true;
					break;
				}
			}

			return result;
		}

		StoryFoodInstance StoryGame::createFoodInstance(StoryFoodType foodType, const std::vector<sf::Vector2i>& availablePositionList) {
			std::uniform_int_distribution<int> tileDistribution(0, availablePositionList.size() - 1);
			int index = tileDistribution(this->randomizer);

			StoryFoodInstance result;
			result.foodInstanceId = this->nextFoodInstanceId;
			result.foodType = foodType;
			result.position = availablePositionList[index];
			return result;
		}

		void StoryGame::addNewFoodSpawnsToFoodTileDistanceTrackingMap(const std::vector<StoryFoodInstance>& spawnedFoodInstanceList) {
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

		std::unordered_map<int, StoryFoodEatenBySnakeScoreResult> StoryGame::buildFoodEatenBySnakeScoreResultMap(const std::vector<StoryFoodInstance>& eatenBySnakeFoodInstanceList) {
			std::unordered_map<int, StoryFoodEatenBySnakeScoreResult> result;

			for (auto const& currFoodInstance : eatenBySnakeFoodInstanceList) {
				StoryFoodTileDistanceTracking currFoodTileDistanceTracking = this->foodTileDistanceTrackingMap[currFoodInstance.foodInstanceId];

				result[currFoodInstance.foodInstanceId] = StoryGameScoreUtils::calcFoodEatenScore(currFoodInstance.foodType, currFoodTileDistanceTracking);
			}

			return result;
		}

		std::vector<StoryDangerInstance> StoryGame::checkForDangerSpawns() {
			std::vector<StoryDangerInstance> result;

			for (auto& currDangerSpawnTracker : this->dangerSpawnTrackerList) {
				StoryDangerSpawnCheckInput checkInput;
				checkInput.timeSinceLevelStarted = this->getTimeElapsed();
				checkInput.randomizer = &this->randomizer;
				checkInput.snakeLength = this->snake->getLength();

				if (currDangerSpawnTracker.shouldDangerSpawn(checkInput)) {
					std::vector<sf::Vector2i> availablePositionList = this->buildAvailableDangerSpawnPositionList(currDangerSpawnTracker.getDangerDefn());
					// TODO printf("There are %d positions on the map that the danger can spawn\n", availablePositionList.size());
					if (availablePositionList.size() > 0) {
						StoryDangerInstance newDangerInstance = this->createDangerInstance(currDangerSpawnTracker.getDangerDefn().dangerType, availablePositionList);
						currDangerSpawnTracker.spawnDanger(newDangerInstance);
						result.push_back(newDangerInstance);

						this->nextDangerInstanceId++;
					}
				}
			}

			return result;
		}

		std::vector<StoryDangerInstance> StoryGame::checkForDangersStrikingSnake() {
			std::vector<StoryDangerInstance> result;

			for (auto& currDangerSpawnTracker : this->dangerSpawnTrackerList) {
				for (auto const& currDangerInstance : currDangerSpawnTracker.getDangerInstanceList()) {
					if (
						!this->dangerStruckSnakeMap[currDangerInstance.dangerInstanceId] &&
						this->snake->occupiesPosition(currDangerInstance.position)
					) {
						this->dangerStruckSnakeMap[currDangerInstance.dangerInstanceId] = true;
						result.push_back(currDangerInstance);
					}
				}
			}

			return result;
		}

		void StoryGame::checkForDangerDespawns() {
			for (auto& currDangerSpawnTracker : this->dangerSpawnTrackerList) {
				for (auto const& currDangerInstance : currDangerSpawnTracker.getDangerInstanceList()) {
					StoryDangerDespawnCheckInput checkInput;
					checkInput.timeSinceLevelStarted = this->getTimeElapsed();
					checkInput.dangerInstanceId = currDangerInstance.dangerInstanceId;

					if (currDangerSpawnTracker.shouldDangerDespawn(checkInput)) {
						currDangerSpawnTracker.despawnDanger(currDangerInstance.dangerInstanceId);
					}
				}
			}
		}

		std::vector<sf::Vector2i> StoryGame::buildAvailableDangerSpawnPositionList(const StoryDangerDefn& dangerDefn) {
			std::vector<sf::Vector2i> result;

			for (int y = 0; y < this->map->getFieldSize().y; y++) {
				for (int x = 0; x < this->map->getFieldSize().x; x++) {
					int floorId = this->map->getFloorId(x, y);

					bool positionIsAvailable =
						!this->map->barrierAt(x, y) &&
						(floorId >= dangerDefn.minFloorId) &&
						(floorId <= dangerDefn.maxFloorId) &&
						!this->foodOccupiesPosition(sf::Vector2i(x, y)) &&
						!this->dangerOccupiesPosition(sf::Vector2i(x, y));

					if (positionIsAvailable) {
						result.push_back(sf::Vector2i(x, y));
					}
				}
			}

			return result;
		}

		bool StoryGame::dangerOccupiesPosition(const sf::Vector2i& position) {
			bool result = false;

			for (auto const& currDangerSpawnTracker : this->dangerSpawnTrackerList) {
				StoryDangerPositionCheckResult checkResult = currDangerSpawnTracker.occupiesPosition(position);
				if (checkResult.dangerExistsFlag) {
					result = true;
					break;
				}
			}

			return result;
		}

		StoryDangerInstance StoryGame::createDangerInstance(StoryDangerType dangerType, const std::vector<sf::Vector2i>& availablePositionList) {
			std::uniform_int_distribution<int> tileDistribution(0, availablePositionList.size() - 1);
			int index = tileDistribution(this->randomizer);

			StoryDangerInstance result;
			result.dangerInstanceId = this->nextDangerInstanceId;
			result.dangerType = dangerType;
			result.position = availablePositionList[index];
			return result;
		}

		void StoryGame::addNewDangerSpawnsToDangerStruckSnakeMap(const std::vector<StoryDangerInstance>& spawnedDangerInstanceList) {
			for (auto const& currDangerInstance : spawnedDangerInstanceList) {
				this->dangerStruckSnakeMap[currDangerInstance.dangerInstanceId] = false;
			}
		}

		std::vector<std::string> StoryGame::buildSoundFileTriggeredList(const StoryGameUpdateResult& updateResult) {
			std::vector<std::string> result;
			std::vector<const StorySoundFxDefn*> soundFxDefnToRemove;

			for (auto currSoundFxDefn : this->remainingSoundFxDefnSet) {
				bool soundTriggeredFlag = false;

				switch (currSoundFxDefn->triggerType) {
				case StorySoundFxTriggerType::ON_TIMER:
					soundTriggeredFlag = (this->clock.getElapsedTime().asSeconds() >= (float)currSoundFxDefn->timePassed);
					break;
				case StorySoundFxTriggerType::ON_FIRST_FOOD_SPAWN:
					for (auto const& currSpawnedFoodInstance : updateResult.spawnedFoodInstanceList) {
						soundTriggeredFlag = soundTriggeredFlag || (currSpawnedFoodInstance.foodType == currSoundFxDefn->foodType);
					}
					break;
				case StorySoundFxTriggerType::ON_FIRST_DANGER_SPAWN:
					for (auto const& currSpawnedDangerInstance : updateResult.spawnedDangerInstanceList) {
						soundTriggeredFlag = soundTriggeredFlag || (currSpawnedDangerInstance.dangerType == currSoundFxDefn->dangerType);
					}
					break;
				case StorySoundFxTriggerType::ON_LENGTH_REACHED:
					soundTriggeredFlag = (this->snake->getLength() >= currSoundFxDefn->lengthReached);
					break;
				case StorySoundFxTriggerType::ON_DAMAGED:
					soundTriggeredFlag = updateResult.snakeDamaged();
					break;
				}

				if (soundTriggeredFlag) {
					result.push_back(currSoundFxDefn->soundFilename);
					soundFxDefnToRemove.push_back(currSoundFxDefn);
				}
			}

			for (auto currSoundFxDefnToRemove : soundFxDefnToRemove) {
				this->remainingSoundFxDefnSet.erase(currSoundFxDefnToRemove);
			}

			return result;
		}

		void StoryGame::updateHealthBy(float amount) {
			this->snakeHealth += amount;

			if (this->snakeHealth < 0.0f) {
				this->snakeHealth = 0.0f;
			}

			if (this->snakeHealth > (float)this->levelDefn->maxSnakeHealth) {
				this->snakeHealth = (float)this->levelDefn->maxSnakeHealth;
			}
		}

		bool StoryGame::checkLevelCompleted() {
			bool result = false;

			switch (this->levelDefn->winCondition.conditionType) {
			case StoryWinConditionType::ON_FOOD_EATEN:
				result = (this->foodEatenCountMap.at(this->levelDefn->winCondition.foodType) >= this->levelDefn->winCondition.foodCount);
				if (result) {
					printf("Reached %d of required food to win the level!\n", this->levelDefn->winCondition.foodCount);
				}
				break;
			case StoryWinConditionType::ON_LENGTH_REACHED:
				result = (this->snake->getLength() >= this->levelDefn->winCondition.snakeLength);
				if (result) {
					printf("Snake reached length %d to win the level!\n", this->levelDefn->winCondition.snakeLength);
				}
				break;
			case StoryWinConditionType::ON_TIME_SURVIVED:
				result = (this->clock.getElapsedTime().asSeconds() >= (float)this->levelDefn->winCondition.timePassed);
				if (result) {
					printf("Survived for %d seconds to win the level!\n", this->levelDefn->winCondition.timePassed);
				}
				break;
			}

			return result;
		}

	}

}
