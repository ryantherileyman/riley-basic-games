
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

		typedef struct Snake_LoadStoryCutsceneEventValidationResult {
			bool rootValid = false;
			bool framesSinceLastEventValid = false;
			bool eventTypeValid = false;
			bool fadeFramesValid = true;
			bool colorValid = true;
			bool textureValid = true;
			bool mapValid = true;
			bool snakeStartValid = true;
			bool snakeStartPositionValid = true;
			bool snakeStartDirectionValid = true;
			bool snakeStartLengthValid = true;
			bool snakeMovementValid = true;
			bool showObjectValid = true;
			bool hideObjectValid = true;
			bool objectInstanceIdValid = true;
			bool objectFoodTypeValid = true;
			bool objectDangerTypeValid = true;
			bool objectPositionValid = true;

			bool valid() const {
				bool result =
					rootValid &&
					framesSinceLastEventValid &&
					eventTypeValid &&
					fadeFramesValid &&
					colorValid &&
					textureValid &&
					mapValid &&
					snakeStartValid &&
					snakeStartPositionValid &&
					snakeStartDirectionValid &&
					snakeStartLengthValid &&
					snakeMovementValid &&
					showObjectValid &&
					hideObjectValid &&
					objectInstanceIdValid &&
					objectFoodTypeValid &&
					objectDangerTypeValid &&
					objectPositionValid;
				return result;
			}
		} LoadStoryCutsceneEventValidationResult;

		typedef struct Snake_LoadStoryCutsceneValidationResult {
			bool existsFlag = false;
			bool rootValid = false;
			bool soundTrackValid = true;
			bool eventListValid = false;
			std::vector<LoadStoryCutsceneEventValidationResult> eventValidationResultList;

			bool valid() const {
				bool allEventEntriesValid = true;
				for (auto const& currEventValidationResult : eventValidationResultList) {
					allEventEntriesValid = allEventEntriesValid && currEventValidationResult.valid();
				}

				bool result =
					!existsFlag ||
					(
						rootValid &&
						soundTrackValid &&
						eventListValid &&
						allEventEntriesValid
					);
				return result;
			}
		} LoadStoryCutsceneValidationResult;

		typedef struct Snake_LoadStoryLevelFoodValidationResult {
			bool rootValid = false;
			bool foodTypeValid = false;
			bool spawnTypeValid = false;
			bool chancePctValid = false;
			bool maxSpawnCountValid = false;
			bool growthRateValid = false;
			bool timePassedValid = true;
			bool intervalValid = true;
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
					intervalValid &&
					lengthReachedValid &&
					floorIdRangeValid;
				return result;
			}
		} LoadStoryLevelFoodValidationResult;

		typedef struct Snake_LoadStoryLevelDangerValidationResult {
			bool rootValid = false;
			bool dangerTypeValid = false;
			bool spawnTypeValid = false;
			bool timePassedValid = false;
			bool chancePctValid = false;
			bool maxSpawnCountValid = false;
			bool lengthReachedValid = true;
			bool floorIdRangeValid = false;

			bool valid() const {
				bool result =
					rootValid &&
					dangerTypeValid &&
					spawnTypeValid &&
					timePassedValid &&
					chancePctValid &&
					maxSpawnCountValid &&
					lengthReachedValid &&
					floorIdRangeValid;
				return result;
			}
		} LoadStoryLevelDangerValidationResult;

		typedef struct Snake_LoadStoryLevelValidationResult {
			LoadStoryCutsceneValidationResult openingCutsceneValidationResult;
			LoadStoryCutsceneValidationResult winCutsceneValidationResult;
			LoadStoryCutsceneValidationResult lossCutsceneValidationResult;
			bool musicValid = false;
			bool mapValid = false;
			bool snakeStartValid = false;
			bool snakeStartPositionValid = false;
			bool snakeStartDirectionValid = false;
			bool snakeStartLengthValid = false;
			bool snakeSpeedValid = false;
			bool snakeHealthValid = false;
			bool winConditionValid = false;
			bool winConditionTypeValid = false;
			bool winConditionFoodTypeValid = true;
			bool winConditionFoodCountValid = true;
			bool winConditionSnakeLengthValid = true;
			bool winConditionTimePassedValid = true;
			bool foodListValid = false;
			std::vector<LoadStoryLevelFoodValidationResult> foodValidationResultList;
			bool dangerListValid = false;
			std::vector<LoadStoryLevelDangerValidationResult> dangerValidationResultList;

			std::vector<std::string> musicFileInvalidList;

			bool valid() const {
				bool allFoodEntriesValid = true;
				for (auto const& currFoodValidationResult : foodValidationResultList) {
					allFoodEntriesValid = allFoodEntriesValid && currFoodValidationResult.valid();
				}

				bool allDangerEntriesValid = true;
				for (auto const& currDangerValidationResult : dangerValidationResultList) {
					allDangerEntriesValid = allDangerEntriesValid && currDangerValidationResult.valid();
				}

				bool result =
					openingCutsceneValidationResult.valid() &&
					winCutsceneValidationResult.valid() &&
					lossCutsceneValidationResult.valid() &&
					musicValid &&
					mapValid &&
					snakeStartValid &&
					snakeStartPositionValid &&
					snakeStartDirectionValid &&
					snakeStartLengthValid &&
					snakeSpeedValid &&
					snakeHealthValid &&
					winConditionValid &&
					winConditionTypeValid &&
					winConditionFoodTypeValid &&
					winConditionFoodCountValid &&
					winConditionSnakeLengthValid &&
					winConditionTimePassedValid &&
					foodListValid &&
					allFoodEntriesValid &&
					dangerListValid &&
					allDangerEntriesValid &&
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

			namespace StoryCutsceneProperties {

				extern const char* SOUND_TRACK;
				extern const char* EVENT_LIST;

				extern const char* FRAMES_SINCE_LAST_EVENT;
				extern const char* EVENT_TYPE;
				extern const char* FADE_FRAMES;
				extern const char* COLOR;
				extern const char* COLOR_RED;
				extern const char* COLOR_GREEN;
				extern const char* COLOR_BLUE;
				extern const char* TEXTURE;
				extern const char* SNAKE_MOVEMENT;
				extern const char* SNAKE_MOVEMENT_GROW;
				extern const char* SHOW_OBJECT;
				extern const char* HIDE_OBJECT;
				extern const char* OBJECT_INSTANCE_ID;

			}

			namespace StoryLevelProperties {

				extern const char* POSITION;
				extern const char* POSITION_X;
				extern const char* POSITION_Y;
				extern const char* TIME_PASSED;

				extern const char* OPENING_CUTSCENE;
				extern const char* WIN_CUTSCENE;
				extern const char* LOSS_CUTSCENE;
				extern const char* MUSIC_FILENAME;
				extern const char* MAP_FILENAME;
				extern const char* SNAKE_START;
				extern const char* SNAKE_START_DIRECTION;
				extern const char* SNAKE_START_LENGTH;
				extern const char* SNAKE_SPEED;
				extern const char* SNAKE_HEALTH;
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
				extern const char* FOOD_INTERVAL;
				extern const char* DANGER_LIST;
				extern const char* DANGER_TYPE;

			}

			namespace DirectionValues {

				extern const char* UP;
				extern const char* DOWN;
				extern const char* LEFT;
				extern const char* RIGHT;

			}

			namespace CutsceneEventTypeValues {

				extern const char* COLOR;
				extern const char* TEXTURE;
				extern const char* SHOW_MAP;
				extern const char* SHOW_SNAKE;
				extern const char* MOVE_SNAKE;
				extern const char* HIDE_SNAKE;
				extern const char* SHOW_FOOD;
				extern const char* HIDE_FOOD;
				extern const char* SHOW_DANGER;
				extern const char* HIDE_DANGER;

			}

			namespace FoodTypeValues {

				extern const char* APPLE;
				extern const char* CARROT;
				extern const char* GREEN_APPLE;
				extern const char* CHILI_PEPPER;

			}

			namespace DangerTypeValues {
				
				extern const char* SPIKE_TRAP;

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

			namespace LoadStoryCampaignListValidation {

				std::vector<std::string> buildErrorMessages(const LoadCampaignListResult& loadResult);

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

			namespace LoadStoryCutsceneValidation {

				bool eventTypeValueValid(const Json::Value& jsonValue);

				bool colorValueValid(const Json::Value& jsonValue);

				bool eventTypeValid(const Json::Value& jsonValue);
				
				bool colorValid(const Json::Value& jsonValue);

				bool snakeMovementValid(const Json::Value& jsonValue);

				LoadStoryCutsceneEventValidationResult validateEventEntry(const Json::Value& jsonValue);

				LoadStoryCutsceneValidationResult validate(const Json::Value& jsonValue, const char* propertyName);

				std::vector<std::string> buildErrorMessages(const LoadStoryCutsceneValidationResult& validationResult);

			}

			namespace LoadStoryLevelValidation {

				bool positionValid(const Json::Value& jsonValue);

				bool snakeStartDirectionValid(const Json::Value& jsonValue);

				bool foodTypeValid(const Json::Value& jsonValue);

				bool dangerTypeValid(const Json::Value& jsonValue);

				bool winConditionTypeValid(const Json::Value& jsonValue);

				LoadStoryLevelFoodValidationResult validateFoodEntry(const Json::Value& jsonValue);

				LoadStoryLevelDangerValidationResult validateDangerEntry(const Json::Value& jsonValue);

				std::vector<std::string> buildErrorMessages(const LoadStoryLevelValidationResult& validationResult);

			}

			namespace LoadStoryCampaignValidation {

				std::vector<std::string> buildErrorMessages(const LoadStoryCampaignResult& loadResult);

			}

			std::string resolveImageFilePath(const std::string& campaignFolderName, const std::string& imageFilename);

			std::string resolveSoundFilePath(const std::string& campaignFolderName, const std::string& soundFilename);

			std::string resolveMusicFilePath(const std::string& campaignFolderName, const std::string& musicFilename);

			LoadCampaignListResult retrieveCampaignList();

			LoadStoryMapResult loadStoryMap(const std::string& campaignFolderName, const std::string& mapFilename);

			LoadStoryCampaignResult loadStoryCampaign(const std::string& campaignFolderName);

			void appendToErrorLog(const std::vector<std::string>& errorMessages);

			void appendFailedFilenameListToErrorLog(const std::vector<std::string>& failedFilenameList);

		}

	}

}
