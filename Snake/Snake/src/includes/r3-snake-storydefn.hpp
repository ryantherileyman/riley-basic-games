
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

		typedef enum class Snake_StoryCutsceneEventType {
			COLOR,
			TEXTURE,
			SHOW_MAP,
			HIDE_MAP,
			SHOW_SNAKE,
			MOVE_SNAKE,
			HIDE_SNAKE,
			SHOW_FOOD,
			HIDE_FOOD,
			SHOW_DANGER,
			HIDE_DANGER,
		} StoryCutsceneEventType;
		
		typedef enum class Snake_StoryFoodType {
			APPLE,
			CARROT,
			GREEN_APPLE,
		} StoryFoodType;

		typedef enum class Snake_StoryDangerType {
			SPIKE_TRAP,
		} StoryDangerType;

		typedef struct Snake_StoryCutsceneColorEventDefn {
			int fadeFrames;
			sf::Color color;
		} StoryCutsceneColorEventDefn;

		typedef struct Snake_StoryCutsceneTextureEventDefn {
			int fadeFrames;
			std::string textureFilename;
		} StoryCutsceneTextureEventDefn;

		typedef struct Snake_StoryCutsceneMapEventDefn {
			int fadeFrames;
			std::string mapFilename;
		} StoryCutsceneMapEventDefn;

		typedef struct Snake_StoryCutsceneSnakeEventDefn {
			SnakeStartDefn snakeStart;
			ObjectDirection snakeDirection;
			bool snakeGrowFlag;
		} StoryCutsceneSnakeEventDefn;

		typedef struct Snake_StoryCutsceneFoodEventDefn {
			int instanceId;
			StoryFoodType foodType;
			sf::Vector2i position;
		} StoryCutsceneFoodEventDefn;

		typedef struct Snake_StoryCutsceneDangerEventDefn {
			int instanceId;
			StoryDangerType dangerType;
			sf::Vector2i position;
		} StoryCutsceneDangerEventDefn;

		typedef struct Snake_StoryCutsceneEventDefn {
			int framesSinceLastEvent;
			StoryCutsceneEventType eventType;
			StoryCutsceneColorEventDefn colorEvent;
			StoryCutsceneTextureEventDefn textureEvent;
			StoryCutsceneMapEventDefn mapEvent;
			StoryCutsceneSnakeEventDefn snakeEvent;
			StoryCutsceneFoodEventDefn foodEvent;
			StoryCutsceneDangerEventDefn dangerEvent;
		} StoryCutsceneEventDefn;

		typedef struct Snake_StoryCutsceneDefn {
			bool existsFlag;
			std::string soundTrackFilename;
			std::vector<StoryCutsceneEventDefn> eventDefnList;
		} StoryCutsceneDefn;

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
			int interval;
			int minFloorId;
			int maxFloorId;
		} StoryFoodDefn;

		typedef struct Snake_StoryDangerDefn {
			StoryDangerType dangerType;
			StoryObjectSpawnType spawnType;
			int timePassed;
			float chancePct;
			int maxSpawnCount;
			int lengthReached;
			int minFloorId;
			int maxFloorId;
		} StoryDangerDefn;

		typedef struct Snake_StoryLevelDefn {
			StoryCutsceneDefn openingCutsceneDefn;
			StoryCutsceneDefn winCutsceneDefn;
			StoryCutsceneDefn lossCutsceneDefn;
			std::string musicFilename;
			std::string mapFilename;
			SnakeStartDefn snakeStart;
			int snakeSpeedTilesPerSecond;
			int maxSnakeHealth;
			StoryWinCondition winCondition;
			std::vector<StoryFoodDefn> foodDefnList;
			std::vector<StoryDangerDefn> dangerDefnList;
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
