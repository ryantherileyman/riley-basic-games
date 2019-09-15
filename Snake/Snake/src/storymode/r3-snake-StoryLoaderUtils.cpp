
#include <fstream>
#include <codecvt>
#include <set>
#include "../jsoncpp/json/json.h"
#include "../includes/r3-json-JsonValidationUtils.hpp"
#include "../includes/r3-snake-storyloader.hpp"
#pragma once

namespace r3 {

	namespace snake {

		namespace StoryLoaderUtils {

			namespace CampaignListProperties {

				const char* MENU_LABEL = "menuLabel";
				const char* MENU_DESCRIPTION = "menuDescription";
				const char* LEVEL_LIST = "levelList";

			}

			namespace StoryMapProperties {

				const char* WIDTH = "width";
				const char* HEIGHT = "height";
				const char* FLOOR_LIST = "floorList";
				const char* BARRIER_LIST = "barrierList";
				const char* TILE_ID = "id";
				const char* TILE_FILENAME = "filename";
				const char* FLOOR_GRID = "floorGrid";
				const char* BARRIER_GRID = "barrierGrid";

			}

			namespace StoryLevelProperties {

				const char* POSITION_X = "x";
				const char* POSITION_Y = "y";
				const char* TIME_PASSED = "timePassed";

				const char* MUSIC_FILENAME = "music";
				const char* MAP_FILENAME = "map";
				const char* SNAKE_START = "snakeStart";
				const char* SNAKE_START_POSITION = "position";
				const char* SNAKE_START_DIRECTION = "direction";
				const char* SNAKE_START_LENGTH = "length";
				const char* SNAKE_SPEED = "snakeSpeed";
				const char* WIN_CONDITION = "winCondition";
				const char* WIN_CONDITION_TYPE = "conditionType";
				const char* WIN_CONDITION_FOOD_COUNT = "foodCount";
				const char* WIN_CONDITION_SNAKE_LENGTH = "snakeLength";
				const char* OBJECT_SPAWN_TYPE = "spawnType";
				const char* OBJECT_CHANCE_PCT = "chancePct";
				const char* OBJECT_LENGTH_REACHED = "lengthReached";
				const char* OBJECT_MAX_SPAWN_COUNT = "maxSpawnCount";
				const char* OBJECT_FLOOR_ID_RANGE = "floorIdRange";
				const char* OBJECT_FLOOR_MIN_ID = "minId";
				const char* OBJECT_FLOOR_MAX_ID = "maxId";
				const char* FOOD_LIST = "foodList";
				const char* FOOD_TYPE = "foodType";
				const char* FOOD_GROWTH_RATE = "growthRate";

			}

			namespace DirectionValues {

				const char* UP = "up";
				const char* DOWN = "down";
				const char* LEFT = "left";
				const char* RIGHT = "right";

			}

			namespace FoodTypeValues {

				const char* APPLE = "Apple";

			}

			namespace WinConditionTypeValues {

				const char* ON_FOOD_EATEN = "onFoodEaten";
				const char* ON_LENGTH_REACHED = "onLengthReached";
				const char* ON_TIME_SURVIVED = "onTimeSurvived";

			}

			namespace SpawnTypeValues {

				const char* ON_DESPAWN = "onDespawn";
				const char* ON_TIMER = "onTimer";
				const char* ON_LENGTH_REACHED = "onLengthReached";

			}

			const char* PATH_SLASH = "/";
			const char* EXT_JSON = ".json";
			const char* PATH_CAMPAIGNS = "resources/campaigns/";
			const char* PATH_TEXTURES = "textures";
			const char* PATH_MUSIC = "music";
			const char* CAMPAIGN_FILENAME = "campaign.json";

			typedef struct Json_LoadJsonFromFileResult {
				Json::Value jsonValue;
				std::string parseErrorString;
			} LoadJsonFromFileResult;

			LoadJsonFromFileResult loadFromJsonFile(const char* filePath) {
				std::ifstream inputStream(filePath);

				LoadJsonFromFileResult result;

				Json::Value jsonValue;
				Json::Reader jsonReader(Json::Features::strictMode());
				jsonReader.parse(inputStream, result.jsonValue);

				result.parseErrorString = jsonReader.getFormattedErrorMessages();

				return result;
			}

			std::string resolveCampaignFilePath(const std::string& folderName) {
				std::string result(PATH_CAMPAIGNS);
				result.append(folderName);
				result.append(PATH_SLASH);
				result.append(CAMPAIGN_FILENAME);
				return result;
			}

			std::string resolveMapFilePath(const std::string& campaignFolderName, const std::string& mapFilename) {
				std::string result(PATH_CAMPAIGNS);
				result.append(campaignFolderName);
				result.append(PATH_SLASH);
				result.append(mapFilename);
				result.append(EXT_JSON);
				return result;
			}

			std::string resolveImageFilePath(const std::string& campaignFolderName, const std::string& imageFilename) {
				std::string result(PATH_CAMPAIGNS);
				result.append(campaignFolderName);
				result.append(PATH_SLASH);
				result.append(PATH_TEXTURES);
				result.append(PATH_SLASH);
				result.append(imageFilename);
				return result;
			}

			std::string resolveMusicFilePath(const std::string& campaignFolderName, const std::string& musicFilename) {
				std::string result(PATH_CAMPAIGNS);
				result.append(campaignFolderName);
				result.append(PATH_SLASH);
				result.append(PATH_MUSIC);
				result.append(PATH_SLASH);
				result.append(musicFilename);
				return result;
			}

			LoadCampaignOptionResult retrieveCampaignOption(int index, const std::string& folderName) {
				std::string filePath = resolveCampaignFilePath(folderName);

				LoadJsonFromFileResult loadResult = loadFromJsonFile(filePath.c_str());

				bool menuLabelValid = r3::json::ValidationUtils::requiredString(loadResult.jsonValue, CampaignListProperties::MENU_LABEL);
				bool menuDescriptionValid = r3::json::ValidationUtils::optionalString(loadResult.jsonValue, CampaignListProperties::MENU_DESCRIPTION);

				LoadCampaignOptionResult result;
				result.parseErrorString = loadResult.parseErrorString;
				result.optionValid =
					menuLabelValid &&
					menuDescriptionValid;

				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> stringConverter;

				std::string strLabel = loadResult.jsonValue[CampaignListProperties::MENU_LABEL].asString();
				std::string strDescription = loadResult.jsonValue[CampaignListProperties::MENU_DESCRIPTION].asString();

				result.index = index;
				result.folderName = folderName;
				result.menuLabel = stringConverter.from_bytes(strLabel);
				result.menuDescription = stringConverter.from_bytes(strDescription);

				return result;
			}

			LoadCampaignListResult retrieveCampaignList() {
				LoadJsonFromFileResult loadResult = loadFromJsonFile("resources/campaign-list.json");

				LoadCampaignListResult result;
				result.parseErrorString = loadResult.parseErrorString;
				result.isArray = loadResult.jsonValue.isArray();
				result.allEntriesValid = true;

				if (result.isArray) {
					for (Json::ArrayIndex index = 0; index < loadResult.jsonValue.size(); index++) {
						if (loadResult.jsonValue[index].isString()) {
							LoadCampaignOptionResult currCampaignOptionResult = retrieveCampaignOption(index, loadResult.jsonValue[index].asString());
							bool currEntryIsValid = currCampaignOptionResult.valid();
							result.campaignOptionList.push_back(currCampaignOptionResult);
							result.allEntriesValid = result.allEntriesValid && currCampaignOptionResult.valid();
						}
					}
				}

				return result;
			}

			StoryMapTileDefn loadStoryMapTile(Json::Value& jsonValue) {
				StoryMapTileDefn result;
				result.tileId = jsonValue[StoryMapProperties::TILE_ID].asInt();
				result.filename = jsonValue[StoryMapProperties::TILE_FILENAME].asString();
				return result;
			}

			void updateStoryMapTileDefnMap(std::unordered_map<int, StoryMapTileDefn>& tileDefnMap, const Json::Value& jsonValue) {
				for (Json::ArrayIndex index = 0; index < jsonValue.size(); index++) {
					Json::Value currTileDefnValue = jsonValue[index];
					StoryMapTileDefn currMapTileDefn = loadStoryMapTile(currTileDefnValue);
					tileDefnMap[currMapTileDefn.tileId] = currMapTileDefn;
				}
			}

			void updateDuplicateTileIdList(std::vector<int>& duplicateTileIdList, const Json::Value& jsonValue) {
				std::set<int> tileIdSet;
				for (Json::ArrayIndex index = 0; index < jsonValue.size(); index++) {
					Json::Value currTileDefnValue = jsonValue[index];
					StoryMapTileDefn currMapTileDefn = loadStoryMapTile(currTileDefnValue);
					if (tileIdSet.count(currMapTileDefn.tileId) == 0) {
						tileIdSet.insert(currMapTileDefn.tileId);
					}
					else {
						duplicateTileIdList.push_back(currMapTileDefn.tileId);
					}
				}
			}

			void updateStoryMapGrid(std::vector<std::vector<int>>& grid, const Json::Value& jsonValue) {
				for (Json::ArrayIndex rowIndex = 0; rowIndex < jsonValue.size(); rowIndex++) {
					std::vector<int> currGridRow;
					Json::Value currRowValue = jsonValue[rowIndex];

					for (Json::ArrayIndex cellIndex = 0; cellIndex < currRowValue.size(); cellIndex++) {
						Json::Value currCellValue = currRowValue[cellIndex];
						currGridRow.push_back(currCellValue.asInt());
					}

					grid.push_back(currGridRow);
				}
			}

			LoadStoryMapResult loadStoryMap(const std::string& campaignFolderName, const std::string& mapFilename) {
				std::string mapFilePath = resolveMapFilePath(campaignFolderName, mapFilename);
				LoadJsonFromFileResult loadResult = loadFromJsonFile(mapFilePath.c_str());

				LoadStoryMapResult result;

				result.validationResult.parseErrorString = loadResult.parseErrorString;
				result.validationResult.widthValid = r3::json::ValidationUtils::requiredInt(loadResult.jsonValue, StoryMapProperties::WIDTH);
				result.validationResult.heightValid = r3::json::ValidationUtils::requiredInt(loadResult.jsonValue, StoryMapProperties::HEIGHT);
				result.validationResult.floorListValid = LoadStoryMapValidation::tileListValid(loadResult.jsonValue, StoryMapProperties::FLOOR_LIST, 0);
				result.validationResult.barrierListValid = LoadStoryMapValidation::tileListValid(loadResult.jsonValue, StoryMapProperties::BARRIER_LIST, 1);
				result.validationResult.floorGridValid = LoadStoryMapValidation::tileGridValid(loadResult.jsonValue, StoryMapProperties::FLOOR_GRID);
				result.validationResult.barrierGridValid = LoadStoryMapValidation::tileGridValid(loadResult.jsonValue, StoryMapProperties::BARRIER_GRID);

				bool fieldSizeValid =
					result.validationResult.widthValid &&
					result.validationResult.heightValid;

				result.validationResult.floorGridSizeValid = true;
				result.validationResult.barrierGridSizeValid = true;
				if (fieldSizeValid) {
					result.mapDefn.fieldSize = sf::Vector2i(loadResult.jsonValue[StoryMapProperties::WIDTH].asInt(), loadResult.jsonValue[StoryMapProperties::HEIGHT].asInt());
					result.validationResult.floorGridSizeValid = LoadStoryMapValidation::tileGridSizeValid(loadResult.jsonValue, StoryMapProperties::FLOOR_GRID, result.mapDefn.fieldSize);
					result.validationResult.barrierGridSizeValid = LoadStoryMapValidation::tileGridSizeValid(loadResult.jsonValue, StoryMapProperties::BARRIER_GRID, result.mapDefn.fieldSize);
				}

				if (result.validationResult.floorListValid) {
					updateStoryMapTileDefnMap(result.mapDefn.floorDefnMap, loadResult.jsonValue[StoryMapProperties::FLOOR_LIST]);
					updateDuplicateTileIdList(result.validationResult.duplicateFloorIdList, loadResult.jsonValue[StoryMapProperties::FLOOR_LIST]);
					LoadStoryMapValidation::updateTextureFileInvalidList(result.validationResult.floorTextureFileInvalidList, campaignFolderName, result.mapDefn.floorDefnMap);
				}

				if (result.validationResult.barrierListValid) {
					updateStoryMapTileDefnMap(result.mapDefn.barrierDefnMap, loadResult.jsonValue[StoryMapProperties::BARRIER_LIST]);
					updateDuplicateTileIdList(result.validationResult.duplicateBarrierIdList, loadResult.jsonValue[StoryMapProperties::BARRIER_LIST]);
					LoadStoryMapValidation::updateTextureFileInvalidList(result.validationResult.barrierTextureFileInvalidList, campaignFolderName, result.mapDefn.barrierDefnMap);
				}

				if (
					result.validationResult.floorGridValid &&
					result.validationResult.floorGridSizeValid
				) {
					updateStoryMapGrid(result.mapDefn.floorGrid, loadResult.jsonValue[StoryMapProperties::FLOOR_GRID]);
					LoadStoryMapValidation::updateNonExistentTileIdRefSet(result.validationResult.nonExistentFloorIdRefList, result.mapDefn.floorDefnMap, result.mapDefn.floorGrid);
				}

				if (
					result.validationResult.barrierGridValid &&
					result.validationResult.barrierGridSizeValid
				) {
					updateStoryMapGrid(result.mapDefn.barrierGrid, loadResult.jsonValue[StoryMapProperties::BARRIER_GRID]);
					LoadStoryMapValidation::updateNonExistentTileIdRefSet(result.validationResult.nonExistentBarrierIdRefList, result.mapDefn.barrierDefnMap, result.mapDefn.barrierGrid);
					result.validationResult.nonExistentBarrierIdRefList.erase(0);
				}

				return result;
			}

			ObjectDirection convertJsonValueToObjectDirection(const Json::Value& jsonValue) {
				ObjectDirection result = ObjectDirection::NONE;

				std::string jsonValueStr = jsonValue.asString();
				if (jsonValueStr.compare(DirectionValues::UP) == 0) {
					result = ObjectDirection::UP;
				}
				else if (jsonValueStr.compare(DirectionValues::DOWN) == 0) {
					result = ObjectDirection::DOWN;
				}
				else if (jsonValueStr.compare(DirectionValues::LEFT) == 0) {
					result = ObjectDirection::LEFT;
				}
				else if (jsonValueStr.compare(DirectionValues::RIGHT) == 0) {
					result = ObjectDirection::RIGHT;
				}

				return result;
			}

			StoryFoodType convertJsonValueToFoodType(const Json::Value& jsonValue) {
				StoryFoodType result = StoryFoodType::APPLE;

				std::string jsonValueStr = jsonValue.asString();
				if (jsonValueStr.compare(FoodTypeValues::APPLE) == 0) {
					result = StoryFoodType::APPLE;
				}

				return result;
			}

			StoryWinConditionType convertJsonValueToWinConditionType(const Json::Value& jsonValue) {
				StoryWinConditionType result = StoryWinConditionType::ON_FOOD_EATEN;

				std::string jsonValueStr = jsonValue.asString();
				if (jsonValueStr.compare(WinConditionTypeValues::ON_FOOD_EATEN) == 0) {
					result = StoryWinConditionType::ON_FOOD_EATEN;
				}
				else if (jsonValueStr.compare(WinConditionTypeValues::ON_LENGTH_REACHED) == 0) {
					result = StoryWinConditionType::ON_LENGTH_REACHED;
				}
				else if (jsonValueStr.compare(WinConditionTypeValues::ON_TIME_SURVIVED) == 0) {
					result = StoryWinConditionType::ON_TIME_SURVIVED;
				}

				return result;
			}

			StoryObjectSpawnType convertJsonValueToObjectSpawnType(const Json::Value& jsonValue) {
				StoryObjectSpawnType result = StoryObjectSpawnType::ON_DESPAWN;

				std::string jsonValueStr = jsonValue.asString();
				if (jsonValueStr.compare(SpawnTypeValues::ON_DESPAWN) == 0) {
					result = StoryObjectSpawnType::ON_DESPAWN;
				}
				else if (jsonValueStr.compare(SpawnTypeValues::ON_LENGTH_REACHED) == 0) {
					result = StoryObjectSpawnType::ON_LENGTH_REACHED;
				}
				else if (jsonValueStr.compare(SpawnTypeValues::ON_TIMER) == 0) {
					result = StoryObjectSpawnType::ON_TIMER;
				}

				return result;
			}

			StoryFoodDefn loadStoryFood(const Json::Value& jsonValue) {
				StoryFoodDefn result;

				result.foodType = convertJsonValueToFoodType(jsonValue[StoryLevelProperties::FOOD_TYPE]);
				result.spawnType = convertJsonValueToObjectSpawnType(jsonValue[StoryLevelProperties::OBJECT_SPAWN_TYPE]);
				result.chancePct = jsonValue[StoryLevelProperties::OBJECT_CHANCE_PCT].asFloat();
				result.maxSpawnCount = jsonValue[StoryLevelProperties::OBJECT_MAX_SPAWN_COUNT].asInt();
				result.growthRate = jsonValue[StoryLevelProperties::FOOD_GROWTH_RATE].asInt();

				if (result.spawnType == StoryObjectSpawnType::ON_LENGTH_REACHED) {
					result.lengthReached = jsonValue[StoryLevelProperties::OBJECT_LENGTH_REACHED].asInt();
				}

				if (result.spawnType == StoryObjectSpawnType::ON_TIMER) {
					result.timePassed = jsonValue[StoryLevelProperties::TIME_PASSED].asInt();
				}

				Json::Value floorIdRangeValue = jsonValue[StoryLevelProperties::OBJECT_FLOOR_ID_RANGE];
				result.minFloorId = floorIdRangeValue[StoryLevelProperties::OBJECT_FLOOR_MIN_ID].asInt();
				result.maxFloorId = floorIdRangeValue[StoryLevelProperties::OBJECT_FLOOR_MAX_ID].asInt();

				return result;
			}

			LoadStoryLevelResult loadStoryLevel(const std::string& campaignFolderName, const Json::Value& jsonValue) {
				LoadStoryLevelResult result;

				result.validationResult.musicValid = r3::json::ValidationUtils::optionalString(jsonValue, StoryLevelProperties::MUSIC_FILENAME);
				if (result.validationResult.musicValid) {
					result.levelDefn.musicFilename = jsonValue.get(StoryLevelProperties::MUSIC_FILENAME, "").asString();
				}

				result.validationResult.mapValid = r3::json::ValidationUtils::requiredString(jsonValue, StoryLevelProperties::MAP_FILENAME);
				if (result.validationResult.mapValid) {
					result.levelDefn.mapFilename = jsonValue[StoryLevelProperties::MAP_FILENAME].asString();
				}

				result.validationResult.snakeStartValid = r3::json::ValidationUtils::requiredObject(jsonValue, StoryLevelProperties::SNAKE_START);
				if (result.validationResult.snakeStartValid) {
					Json::Value snakeStartValue = jsonValue[StoryLevelProperties::SNAKE_START];

					result.validationResult.snakeStartPositionValid = LoadStoryLevelValidation::snakeStartPositionValid(snakeStartValue);
					if (result.validationResult.snakeStartPositionValid) {
						Json::Value snakeStartPositionValue = snakeStartValue[StoryLevelProperties::SNAKE_START_POSITION];
						result.levelDefn.snakeStart.headPosition.x = snakeStartPositionValue[StoryLevelProperties::POSITION_X].asInt();
						result.levelDefn.snakeStart.headPosition.y = snakeStartPositionValue[StoryLevelProperties::POSITION_Y].asInt();
					}

					result.validationResult.snakeStartDirectionValid = LoadStoryLevelValidation::snakeStartDirectionValid(snakeStartValue);
					if (result.validationResult.snakeStartDirectionValid) {
						result.levelDefn.snakeStart.facingDirection = convertJsonValueToObjectDirection(snakeStartValue[StoryLevelProperties::SNAKE_START_DIRECTION]);
					}

					result.validationResult.snakeStartLengthValid = r3::json::ValidationUtils::requiredInt(snakeStartValue, StoryLevelProperties::SNAKE_START_LENGTH, 2);
					if (result.validationResult.snakeStartLengthValid) {
						result.levelDefn.snakeStart.length = snakeStartValue[StoryLevelProperties::SNAKE_START_LENGTH].asInt();
					}
				}

				result.validationResult.snakeSpeedValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::SNAKE_SPEED, 1);
				if (result.validationResult.snakeSpeedValid) {
					result.levelDefn.snakeSpeedTilesPerSecond = jsonValue[StoryLevelProperties::SNAKE_SPEED].asInt();
				}

				result.validationResult.winConditionValid = r3::json::ValidationUtils::requiredObject(jsonValue, StoryLevelProperties::WIN_CONDITION);
				if (result.validationResult.winConditionValid) {
					Json::Value winConditionValue = jsonValue[StoryLevelProperties::WIN_CONDITION];

					result.validationResult.winConditionTypeValid = LoadStoryLevelValidation::winConditionTypeValid(winConditionValue);
					if (result.validationResult.winConditionTypeValid) {
						result.levelDefn.winCondition.conditionType = convertJsonValueToWinConditionType(winConditionValue[StoryLevelProperties::WIN_CONDITION_TYPE]);

						if (result.levelDefn.winCondition.conditionType == StoryWinConditionType::ON_FOOD_EATEN) {
							result.validationResult.winConditionFoodTypeValid = LoadStoryLevelValidation::foodTypeValid(winConditionValue);
							if (result.validationResult.winConditionFoodTypeValid) {
								result.levelDefn.winCondition.foodType = convertJsonValueToFoodType(winConditionValue[StoryLevelProperties::FOOD_TYPE]);
							}

							result.validationResult.winConditionFoodCountValid = r3::json::ValidationUtils::requiredInt(winConditionValue, StoryLevelProperties::WIN_CONDITION_FOOD_COUNT, 1);
							if (result.validationResult.winConditionFoodCountValid) {
								result.levelDefn.winCondition.foodCount = winConditionValue[StoryLevelProperties::WIN_CONDITION_FOOD_COUNT].asInt();
							}
						}

						if (result.levelDefn.winCondition.conditionType == StoryWinConditionType::ON_LENGTH_REACHED) {
							result.validationResult.winConditionSnakeLengthValid = r3::json::ValidationUtils::requiredInt(winConditionValue, StoryLevelProperties::WIN_CONDITION_SNAKE_LENGTH, 2);
							if (result.validationResult.winConditionSnakeLengthValid) {
								result.levelDefn.winCondition.snakeLength = winConditionValue[StoryLevelProperties::WIN_CONDITION_SNAKE_LENGTH].asInt();
							}
						}

						if (result.levelDefn.winCondition.conditionType == StoryWinConditionType::ON_TIME_SURVIVED) {
							result.validationResult.winConditionTimePassedValid = r3::json::ValidationUtils::requiredInt(winConditionValue, StoryLevelProperties::TIME_PASSED, 1);
							if (result.validationResult.winConditionTimePassedValid) {
								result.levelDefn.winCondition.timePassed = winConditionValue[StoryLevelProperties::TIME_PASSED].asInt();
							}
						}
					}
				}

				result.validationResult.foodListValid = r3::json::ValidationUtils::requiredArray(jsonValue, StoryLevelProperties::FOOD_LIST);
				if (result.validationResult.foodListValid) {
					Json::Value foodListValue = jsonValue[StoryLevelProperties::FOOD_LIST];
					for (Json::ArrayIndex index = 0; index < foodListValue.size(); index++) {
						Json::Value currFoodValue = foodListValue[index];

						LoadStoryLevelFoodValidationResult currFoodValidationResult = LoadStoryLevelValidation::validateFoodEntry(currFoodValue);
						result.validationResult.foodValidationResultList.push_back(currFoodValidationResult);

						if (currFoodValidationResult.valid()) {
							result.levelDefn.foodDefnList.push_back(loadStoryFood(currFoodValue));
						}
					}
				}

				if (result.validationResult.musicValid) {
					std::string musicStr = jsonValue[StoryLevelProperties::MUSIC_FILENAME].asString();
					if (
						(!musicStr.empty()) &&
						!LoadAssetValidation::musicFileValid(campaignFolderName, musicStr)
					) {
						result.validationResult.musicFileInvalidList.push_back(musicStr);
					}
				}

				return result;
			}

			LoadStoryCampaignResult loadStoryCampaign(const std::string& campaignFolderName) {
				std::string campaignFilename = resolveCampaignFilePath(campaignFolderName);
				LoadJsonFromFileResult loadResult = loadFromJsonFile(campaignFilename.c_str());

				LoadStoryCampaignResult result;
				result.parseErrorString = loadResult.parseErrorString;

				result.levelListValid = r3::json::ValidationUtils::requiredArray(loadResult.jsonValue, CampaignListProperties::LEVEL_LIST);
				if (result.levelListValid) {
					result.levelListValid = (loadResult.jsonValue[CampaignListProperties::LEVEL_LIST].size() > 0);
				}

				if (result.levelListValid) {
					Json::Value levelListValue = loadResult.jsonValue[CampaignListProperties::LEVEL_LIST];
					for (Json::ArrayIndex index = 0; index < levelListValue.size(); index++) {
						LoadStoryLevelResult currLevelResult = loadStoryLevel(campaignFolderName, levelListValue[index]);
						result.levelResultList.push_back(currLevelResult);
					}
				}

				return result;
			}

		}

	}

}
