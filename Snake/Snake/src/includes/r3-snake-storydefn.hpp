
#include <string>
#include <vector>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "r3-snake-gamestate.hpp"
#pragma once

namespace r3 {

	namespace snake {

		typedef struct Snake_LoadCampaignOptionResult {
			int index;
			std::string folderName;
			std::wstring menuLabel;
			std::wstring menuDescription;

			std::string parseErrorString;
			bool optionValid;

			bool valid() const {
				bool result =
					parseErrorString.empty() &&
					optionValid;
				return result;
			}
		} LoadCampaignOptionResult;

		typedef struct Snake_LoadCampaignListResult {
			std::vector<LoadCampaignOptionResult> campaignOptionList;

			std::string parseErrorString;
			bool isArray;
			bool allEntriesValid;

			bool valid() const {
				bool result =
					(parseErrorString.empty()) &&
					isArray &&
					allEntriesValid;
				return result;
			}
		} LoadCampaignListResult;

		typedef struct Snake_StoryMapTileDefn {
			int tileId;
			std::string filename;
		} StoryMapTileDefn;

		typedef struct Snake_StoryMapDefn {
			sf::Vector2i fieldSize;
			std::unordered_map<int, StoryMapTileDefn> floorDefnMap;
			std::unordered_map<int, StoryMapTileDefn> barrierDefnMap;
			std::vector<std::vector<int>> floorGrid;
			std::vector<std::vector<int>> barrierGrid;
		} StoryMapDefn;

		typedef enum class Snake_StoryFoodType {
			APPLE,
		} StoryFoodType;

		typedef enum class Snake_StoryWinConditionType {
			ON_FOOD_EATEN,
			ON_LENGTH_REACHED,
			ON_TIME_SURVIVED,
		} StoryWinConditionType;

		typedef struct Snake_StoryWinCondition {
			StoryWinConditionType conditionType;
			StoryFoodType foodType;
			int foodCount;
			int snakeLength;
			int timePassed;
		} StoryWinCondition;

		typedef enum class Snake_StoryObjectSpawnType {
			ON_DESPAWN,
			ON_TIMER,
			ON_LENGTH_REACHED,
		} StoryObjectSpawnType;

		typedef struct Snake_StoryFoodDefn {
			StoryFoodType foodType;
			StoryObjectSpawnType spawnType;
			float chancePct;
			int maxSpawnCount;
			int growthRate;
			int lengthReached;
			int timePassed;
			int minFloorId;
			int maxFloorId;
		} StoryFoodDefn;

		typedef struct Snake_StoryLevelDefn {
			std::string musicFilename;
			std::string mapFilename;
			SnakeStartDefn snakeStart;
			int snakeSpeedTilesPerSecond;
			StoryWinCondition winCondition;
			std::vector<StoryFoodDefn> foodDefnList;
		} StoryLevelDefn;

		class StoryMap;

		class StoryMap {

		private:
			sf::Vector2i fieldSize;

			int* allocatedFloorArray;
			int** floorGrid;

			int* allocatedBarrierArray;
			int** barrierGrid;

		public:
			StoryMap(const StoryMapDefn& mapDefn);

		public:
			~StoryMap();

		public:
			sf::Vector2i getFieldSize() const;
			int getFloorId(int x, int y) const;
			int getBarrierId(int x, int y) const;
			bool barrierAt(int x, int y) const;

		};

	}

}
