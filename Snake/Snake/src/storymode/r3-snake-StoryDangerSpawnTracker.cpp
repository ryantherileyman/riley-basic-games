
#include <assert.h>
#include "../includes/r3-snake-storymodescene.hpp"

namespace r3 {

	namespace snake {

		StoryDangerSpawnTracker::StoryDangerSpawnTracker(const StoryDangerDefn& dangerDefn) {
			this->dangerDefn = &dangerDefn;

			this->chancePctDistribution = std::uniform_real_distribution<float>(0.0f, 100.0f);

			this->spawnCount = 0;
			this->timeOfLastChanceCheck = sf::seconds(-1.0f);
			this->timeOfLastSpawn = sf::seconds(0.0f);
		}

		const StoryDangerDefn& StoryDangerSpawnTracker::getDangerDefn() const {
			return *this->dangerDefn;
		}

		const std::vector<StoryDangerInstance>& StoryDangerSpawnTracker::getDangerInstanceList() const {
			return this->dangerInstanceList;
		}

		StoryDangerInstance StoryDangerSpawnTracker::getDangerInstance(int dangerInstanceId) const {
			int index = this->findDangerInstanceIndex(dangerInstanceId);
			assert(index >= 0);

			return( this->dangerInstanceList[index] );
		}

		bool StoryDangerSpawnTracker::shouldDangerSpawn(const StoryDangerSpawnCheckInput& input) {
			sf::Time timeSinceLastChanceCheck = input.timeSinceLevelStarted - this->timeOfLastChanceCheck;
			sf::Time timeSinceLastSpawn = input.timeSinceLevelStarted - this->timeOfLastSpawn;

			bool conditionsMet = false;
			switch (this->dangerDefn->spawnType) {
			case StoryObjectSpawnType::ON_TIMER:
				conditionsMet =
					(this->dangerInstanceList.empty()) &&
					(this->spawnCount < this->dangerDefn->maxSpawnCount) &&
					(
						(
							(this->spawnCount == 0) &&
							(timeSinceLastSpawn.asSeconds() >= (float)this->dangerDefn->timePassed)
						) ||
						(
							(this->spawnCount > 0 ) &&
							(timeSinceLastSpawn.asSeconds() >= (float)this->dangerDefn->interval)
						)
					) &&
					(timeSinceLastChanceCheck.asSeconds() >= 1.0f);
				break;
			case StoryObjectSpawnType::ON_LENGTH_REACHED:
				conditionsMet =
					(this->dangerInstanceList.empty()) &&
					(input.snakeLength >= this->dangerDefn->lengthReached) &&
					(this->spawnCount < this->dangerDefn->maxSpawnCount) &&
					(timeSinceLastSpawn.asSeconds() >= (float)this->dangerDefn->interval) &&
					(timeSinceLastChanceCheck.asSeconds() >= 1.0f);
				break;
			}

			bool result = false;
			if (conditionsMet) {
				printf("Checking if danger will spawn...\n");
				float chanceComparison = this->chancePctDistribution(*input.randomizer);
				printf("Time since last chance check is %f seconds\n", timeSinceLastChanceCheck.asSeconds());
				printf("Rolled %f, with chance of %f\n", chanceComparison, this->dangerDefn->chancePct);
				this->timeOfLastChanceCheck = input.timeSinceLevelStarted;
				result = (chanceComparison <= this->dangerDefn->chancePct);
				if (result) {
					this->timeOfLastSpawn = input.timeSinceLevelStarted;
				}
			}

			return result;
		}

		StoryDangerPositionCheckResult StoryDangerSpawnTracker::occupiesPosition(const sf::Vector2i& position) const {
			StoryDangerPositionCheckResult result;
			result.dangerExistsFlag = false;
			result.dangerInstanceId = 0;

			for (auto const& currDangerInstance : this->dangerInstanceList) {
				if (currDangerInstance.position == position) {
					result.dangerExistsFlag = true;
					result.dangerInstanceId = currDangerInstance.dangerInstanceId;
					break;
				}
			}

			return result;
		}

		bool StoryDangerSpawnTracker::shouldDangerDespawn(const StoryDangerDespawnCheckInput& input) const {
			assert(this->findDangerInstanceIndex(input.dangerInstanceId) >= 0);

			sf::Time timeSinceLastSpawn = input.timeSinceLevelStarted - this->timeOfLastSpawn;
			bool result = (timeSinceLastSpawn.asSeconds() >= 1.0f);
			return result;
		}

		void StoryDangerSpawnTracker::spawnDanger(const StoryDangerInstance& dangerInstance) {
			this->dangerInstanceList.push_back(dangerInstance);
			this->spawnCount++;
		}

		void StoryDangerSpawnTracker::despawnDanger(int dangerInstanceId) {
			int index = this->findDangerInstanceIndex(dangerInstanceId);
			assert(index >= 0);

			this->dangerInstanceList.erase(this->dangerInstanceList.begin() + index);
		}

		int StoryDangerSpawnTracker::findDangerInstanceIndex(int dangerInstanceId) const {
			int result = -1;
			for (size_t index = 0; index < this->dangerInstanceList.size(); index++) {
				if (this->dangerInstanceList[index].dangerInstanceId == dangerInstanceId) {
					result = index;
					break;
				}
			}
			return result;
		}

	}

}
