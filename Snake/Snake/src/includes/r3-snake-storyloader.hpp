
#include <string>
#include <vector>
#include <set>
#include "../jsoncpp/json/json.h"
#include "r3-snake-storydefn.hpp"
#pragma once

namespace r3 {

	namespace snake {

		typedef struct Snake_LoadStoryMapValidationResult {
			std::string parseErrorString;

			bool widthValid = false;
			bool heightValid = false;
			bool floorListValid = false;
			bool barrierListValid = false;
			bool floorGridValid = false;
			bool barrierGridValid = false;

			bool floorGridSizeValid = true;
			bool barrierGridSizeValid = true;

			std::vector<std::string> floorTextureFileInvalidList;
			std::vector<std::string> barrierTextureFileInvalidList;

			std::vector<int> duplicateFloorIdList;
			std::vector<int> duplicateBarrierIdList;

			std::set<int> nonExistentFloorIdRefList;
			std::set<int> nonExistentBarrierIdRefList;

			bool valid() const {
				bool result =
					widthValid &&
					heightValid &&
					floorListValid &&
					barrierListValid &&
					floorGridValid &&
					barrierGridValid &&
					floorGridSizeValid &&
					barrierGridSizeValid &&
					floorTextureFileInvalidList.empty() &&
					barrierTextureFileInvalidList.empty() &&
					duplicateFloorIdList.empty() &&
					duplicateBarrierIdList.empty() &&
					nonExistentFloorIdRefList.empty() &&
					nonExistentBarrierIdRefList.empty();
				return result;
			}
		} LoadStoryMapValidationResult;

		typedef struct Snake_LoadStoryMapResult {
			StoryMapDefn mapDefn;
			LoadStoryMapValidationResult validationResult;
		} LoadStoryMapResult;

		typedef struct Snake_LoadStoryLevelFoodValidationResult {
			bool rootValid = false;
			bool foodTypeValid = false;
			bool spawnTypeValid = false;
			bool chancePctValid = false;
			bool maxSpawnCountValid = false;
			bool growthRateValid = false;
			bool timePassedValid = true;
			bool lengthReachedValid = true;
			bool floorIdRangeValid = false;

			bool valid() const {
				bool result =
					rootValid &&
					foodTypeValid &&
					spawnTypeValid &&
					chancePctValid &&
					maxSpawnCountValid &&
					growthRateValid &&
					timePassedValid &&
					lengthReachedValid &&
					floorIdRangeValid;
				return result;
			}
		} LoadStoryLevelFoodValidationResult;

		typedef struct Snake_LoadStoryLevelValidationResult {
			bool musicValid = false;
			bool mapValid = false;
			bool snakeStartValid = false;
			bool snakeStartPositionValid = false;
			bool snakeStartDirectionValid = false;
			bool snakeStartLengthValid = false;
			bool snakeSpeedValid = false;
			bool winConditionValid = false;
			bool winConditionTypeValid = false;
			bool winConditionFoodTypeValid = true;
			bool winConditionFoodCountValid = true;
			bool winConditionSnakeLengthValid = true;
			bool winConditionTimePassedValid = true;
			bool foodListValid = false;
			std::vector<LoadStoryLevelFoodValidationResult> foodValidationResultList;

			std::vector<std::string> musicFileInvalidList;

			bool valid() const {
				bool allFoodEntriesValid = true;
				for (auto const& currFoodValidationResult : foodValidationResultList) {
					allFoodEntriesValid = allFoodEntriesValid && currFoodValidationResult.valid();
				}

				bool result =
					musicValid &&
					mapValid &&
					snakeStartValid &&
					snakeStartPositionValid &&
					snakeStartDirectionValid &&
					snakeStartLengthValid &&
					snakeSpeedValid &&
					winConditionValid &&
					winConditionTypeValid &&
					winConditionFoodTypeValid &&
					winConditionFoodCountValid &&
					winConditionSnakeLengthValid &&
					winConditionTimePassedValid &&
					foodListValid &&
					allFoodEntriesValid &&
					musicFileInvalidList.empty();
				return result;
			}
		} LoadStoryLevelValidationResult;

		typedef struct Snake_LoadStoryLevelResult {
			StoryLevelDefn levelDefn;
			LoadStoryLevelValidationResult validationResult;
		} LoadStoryLevelResult;

		typedef struct Snake_LoadStoryCampaignResult {
			std::string parseErrorString;

			bool levelListValid = false;

			std::vector<LoadStoryLevelResult> levelResultList;

			bool valid() const {
				bool allLevelEntriesValid = true;
				for (auto const& currLevelResult : levelResultList) {
					allLevelEntriesValid = allLevelEntriesValid && currLevelResult.validationResult.valid();
				}

				bool result =
					levelListValid &&
					allLevelEntriesValid;
				return result;
			}
		} LoadStoryCampaignResult;

		namespace StoryLoaderUtils {

			namespace LoadAssetValidation {

				bool imageFileValid(const std::string& campaignFolderName, const std::string& imageFilename);

				bool musicFileValid(const std::string& campaignFolderName, const std::string& musicFilename);

			}

			namespace CampaignListProperties {

				extern const char* MENU_LABEL;
				extern const char* MENU_DESCRIPTION;
				extern const char* LEVEL_LIST;

			}

			namespace StoryMapProperties {

				extern const char* WIDTH;
				extern const char* HEIGHT;
				extern const char* FLOOR_LIST;
				extern const char* BARRIER_LIST;
				extern const char* TILE_ID;
				extern const char* TILE_FILENAME;
				extern const char* FLOOR_GRID;
				extern const char* BARRIER_GRID;

			}

			namespace StoryLevelProperties {

				extern const char* POSITION_X;
				extern const char* POSITION_Y;
				extern const char* TIME_PASSED;

				extern const char* MUSIC_FILENAME;
				extern const char* MAP_FILENAME;
				extern const char* SNAKE_START;
				extern const char* SNAKE_START_POSITION;
				extern const char* SNAKE_START_DIRECTION;
				extern const char* SNAKE_START_LENGTH;
				extern const char* SNAKE_SPEED;
				extern const char* WIN_CONDITION;
				extern const char* WIN_CONDITION_TYPE;
				extern const char* WIN_CONDITION_FOOD_COUNT;
				extern const char* WIN_CONDITION_SNAKE_LENGTH;
				extern const char* OBJECT_SPAWN_TYPE;
				extern const char* OBJECT_CHANCE_PCT;
				extern const char* OBJECT_LENGTH_REACHED;
				extern const char* OBJECT_MAX_SPAWN_COUNT;
				extern const char* OBJECT_FLOOR_ID_RANGE;
				extern const char* OBJECT_FLOOR_MIN_ID;
				extern const char* OBJECT_FLOOR_MAX_ID;
				extern const char* FOOD_LIST;
				extern const char* FOOD_TYPE;
				extern const char* FOOD_GROWTH_RATE;

			}

			namespace DirectionValues {

				extern const char* UP;
				extern const char* DOWN;
				extern const char* LEFT;
				extern const char* RIGHT;

			}

			namespace FoodTypeValues {

				extern const char* APPLE;

			}

			namespace WinConditionTypeValues {

				extern const char* ON_FOOD_EATEN;
				extern const char* ON_LENGTH_REACHED;
				extern const char* ON_TIME_SURVIVED;

			}

			namespace SpawnTypeValues {

				extern const char* ON_DESPAWN;
				extern const char* ON_TIMER;
				extern const char* ON_LENGTH_REACHED;

			}

			namespace LoadStoryMapValidation {

				bool widthValid(const Json::Value& jsonValue);

				bool heightValid(const Json::Value& jsonValue);

				bool tileValid(const Json::Value& jsonValue, int minTileId);

				bool tileListValid(const Json::Value& jsonValue, const char* propertyName, int minTileId);

				bool tileGridValid(const Json::Value& jsonValue, const char* propertyName);

				bool tileGridSizeValid(const Json::Value& jsonValue, const char* propertyName, const sf::Vector2i& fieldSize);

				void updateTextureFileInvalidList(std::vector<std::string>& textureFileInvalidList, const std::string& campaignFolderName, const std::unordered_map<int, StoryMapTileDefn>& storyMapTileDefnMap);

				void updateNonExistentTileIdRefSet(std::set<int>& nonExistentTileIdRefList, const std::unordered_map<int, StoryMapTileDefn>& storyMapTileDefnMap, const std::vector<std::vector<int>>& tileGrid);

				std::vector<std::string> buildErrorMessages(const LoadStoryMapValidationResult& validationResult);

			}

			namespace LoadStoryLevelValidation {

				bool snakeStartPositionValid(const Json::Value& jsonValue);

				bool snakeStartDirectionValid(const Json::Value& jsonValue);

				bool foodTypeValid(const Json::Value& jsonValue);

				bool winConditionTypeValid(const Json::Value& jsonValue);

				LoadStoryLevelFoodValidationResult validateFoodEntry(const Json::Value& jsonValue);

				std::vector<std::string> buildErrorMessages(const LoadStoryLevelValidationResult& validationResult);

			}

			namespace LoadStoryCampaignValidation {

				std::vector<std::string> buildErrorMessages(const LoadStoryCampaignResult& loadResult);

			}

			std::string resolveImageFilePath(const std::string& campaignFolderName, const std::string& imageFilename);

			std::string resolveMusicFilePath(const std::string& campaignFolderName, const std::string& musicFilename);

			LoadCampaignListResult retrieveCampaignList();

			LoadStoryMapResult loadStoryMap(const std::string& campaignFolderName, const std::string& mapFilename);

			LoadStoryCampaignResult loadStoryCampaign(const std::string& campaignFolderName);

			void appendToErrorLog(const std::vector<std::string>& errorMessages);

			void appendFailedFilenameListToErrorLog(const std::vector<std::string>& failedFilenameList);

		}

	}

}
