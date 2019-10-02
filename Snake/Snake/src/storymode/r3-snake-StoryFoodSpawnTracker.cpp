
#include <assert.h>
#include "../includes/r3-snake-storymodescene.hpp"

namespace r3 {

	namespace snake {

		StoryFoodSpawnTracker::StoryFoodSpawnTracker(const StoryFoodDefn& foodDefn) {
			this->foodDefn = &foodDefn;

			this->chancePctDistribution = std::uniform_real_distribution<float>(0.0f, 100.0f);

			this->spawnCount = 0;
			this->timeOfLastChanceCheck = sf::seconds(-1.0f);
			this->timeOfLastSpawn = sf::seconds(0.0f - (float)foodDefn.timePassed);
		}

		const StoryFoodDefn& StoryFoodSpawnTracker::getFoodDefn() const {
			return *this->foodDefn;
		}

		const std::vector<StoryFoodInstance>& StoryFoodSpawnTracker::getFoodInstanceList() const {
			return this->foodInstanceList;
		}

		StoryFoodInstance StoryFoodSpawnTracker::getFoodInstance(int foodInstanceId) const {
			int index = this->findFoodInstanceIndex(foodInstanceId);
			assert(index >= 0);

			return( this->foodInstanceList[index] );
		}

		bool StoryFoodSpawnTracker::shouldFoodSpawn(const StoryFoodSpawnCheckInput& input) {
			sf::Time timeSinceLastChanceCheck = input.timeSinceLevelStarted - this->timeOfLastChanceCheck;
			sf::Time timeSinceLastSpawn = input.timeSinceLevelStarted - this->timeOfLastSpawn;

			bool conditionsMet = false;

			switch (this->foodDefn->spawnType) {
			case StoryObjectSpawnType::ON_DESPAWN:
				conditionsMet =
					(this->foodInstanceList.empty()) &&
					(this->spawnCount < this->foodDefn->maxSpawnCount) &&
					(timeSinceLastChanceCheck.asSeconds() >= 1.0f);
				break;
			case StoryObjectSpawnType::ON_TIMER:
				conditionsMet =
					(this->spawnCount < this->foodDefn->maxSpawnCount) &&
					(timeSinceLastSpawn.asSeconds() >= (float)this->foodDefn->timePassed) &&
					(timeSinceLastChanceCheck.asSeconds() >= 1.0f);
				break;
			case StoryObjectSpawnType::ON_LENGTH_REACHED:
				conditionsMet =
					(input.snakeLength >= this->foodDefn->lengthReached) &&
					(this->foodInstanceList.empty()) &&
					(this->spawnCount < this->foodDefn->maxSpawnCount) &&
					(timeSinceLastChanceCheck.asSeconds() >= 1.0f);
				break;
			}

			bool result = false;
			if (conditionsMet) {
				printf("Checking if food with growth of %d will spawn...\n", this->foodDefn->growthRate);
				float chanceComparison = this->chancePctDistribution(*input.randomizer);
				printf("Time since last chance check is %f seconds\n", timeSinceLastChanceCheck.asSeconds());
				printf("Rolled %f, with chance of %f\n", chanceComparison, this->foodDefn->chancePct);
				this->timeOfLastChanceCheck = input.timeSinceLevelStarted;
				result = (chanceComparison <= this->foodDefn->chancePct);
				if (result) {
					this->timeOfLastSpawn = input.timeSinceLevelStarted;
				}
			}

			return result;
		}

		StoryFoodPositionCheckResult StoryFoodSpawnTracker::occupiesPosition(const sf::Vector2i& position) const {
			StoryFoodPositionCheckResult result;
			result.foodExistsFlag = false;
			result.foodInstanceId = 0;

			for (auto const currFoodInstance : this->foodInstanceList) {
				if (currFoodInstance.position == position) {
					result.foodExistsFlag = true;
					result.foodInstanceId = currFoodInstance.foodInstanceId;
					break;
				}
			}

			return result;
		}

		void StoryFoodSpawnTracker::spawnFood(const StoryFoodInstance& foodInstance) {
			this->foodInstanceList.push_back(foodInstance);
			this->spawnCount++;
		}

		void StoryFoodSpawnTracker::despawnFood(int foodInstanceId) {
			int index = this->findFoodInstanceIndex(foodInstanceId);
			assert(index >= 0);

			this->foodInstanceList.erase(this->foodInstanceList.begin() + index);
		}

		int StoryFoodSpawnTracker::findFoodInstanceIndex(int foodInstanceId) const {
			int result = -1;
			for (size_t index = 0; index < this->foodInstanceList.size(); index++) {
				if (this->foodInstanceList[index].foodInstanceId == foodInstanceId) {
					result = index;
					break;
				}
			}
			return result;
		}

	}

}
