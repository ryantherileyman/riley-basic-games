
#include <fstream>
#include <codecvt>
#include <ctime>
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

			namespace StoryCutsceneProperties {

				const char* SOUND_TRACK = "soundTrack";
				const char* EVENT_LIST = "eventList";

				const char* FRAMES_SINCE_LAST_EVENT = "framesSinceLastEvent";
				const char* EVENT_TYPE = "eventType";
				const char* FADE_FRAMES = "fadeFrames";
				const char* COLOR = "color";
				const char* COLOR_RED = "red";
				const char* COLOR_GREEN = "green";
				const char* COLOR_BLUE = "blue";
				const char* TEXTURE = "texture";
				const char* SNAKE_MOVEMENT = "snakeMovement";
				const char* SNAKE_MOVEMENT_GROW = "grow";
				const char* SHOW_OBJECT = "showObject";
				const char* HIDE_OBJECT = "hideObject";
				const char* OBJECT_INSTANCE_ID = "instanceId";

			}

			namespace StoryLevelProperties {

				const char* POSITION = "position";
				const char* POSITION_X = "x";
				const char* POSITION_Y = "y";
				const char* TIME_PASSED = "timePassed";
				const char* INTERVAL = "interval";
				const char* REGION = "region";
				const char* REGION_LEFT = "left";
				const char* REGION_TOP = "top";
				const char* REGION_WIDTH = "width";
				const char* REGION_HEIGHT = "height";

				const char* OPENING_CUTSCENE = "openingCutscene";
				const char* WIN_CUTSCENE = "winCutscene";
				const char* LOSS_CUTSCENE = "lossCutscene";
				const char* MUSIC_FILENAME = "music";
				const char* MAP_FILENAME = "map";
				const char* SNAKE_START = "snakeStart";
				const char* SNAKE_START_DIRECTION = "direction";
				const char* SNAKE_START_LENGTH = "length";
				const char* SNAKE_SPEED = "snakeSpeed";
				const char* SNAKE_HEALTH = "snakeHealth";
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
				const char* EATEN_FOOD_TYPE = "eatenFoodType";
				const char* EATEN_FOOD_COUNT = "eatenFoodCount";
				const char* FOOD_LIST = "foodList";
				const char* FOOD_TYPE = "foodType";
				const char* FOOD_GROWTH_RATE = "growthRate";
				const char* FOOD_HEALTH = "health";
				const char* DANGER_LIST = "dangerList";
				const char* DANGER_TYPE = "dangerType";
				const char* SOUND_FX_LIST = "soundFxList";
				const char* SOUND_FX_TRIGGER_TYPE = "triggerType";
				const char* SOUND_FILENAME = "sound";

			}

			namespace DirectionValues {

				const char* UP = "up";
				const char* DOWN = "down";
				const char* LEFT = "left";
				const char* RIGHT = "right";

			}

			namespace CutsceneEventTypeValues {

				const char* COLOR = "color";
				const char* TEXTURE = "texture";
				const char* SHOW_MAP = "showMap";
				const char* SHOW_SNAKE = "showSnake";
				const char* MOVE_SNAKE = "moveSnake";
				const char* HIDE_SNAKE = "hideSnake";
				const char* SHOW_FOOD = "showFood";
				const char* HIDE_FOOD = "hideFood";
				const char* SHOW_DANGER = "showDanger";
				const char* HIDE_DANGER = "hideDanger";

			}

			namespace FoodTypeValues {

				const char* APPLE = "Apple";
				const char* CARROT = "Carrot";
				const char* GREEN_APPLE = "Green Apple";
				const char* CHILI_PEPPER = "Chili Pepper";
				const char* CHERRIES = "Cherries";
				const char* ORANGE = "Orange";
				const char* PEAR = "Pear";
				const char* BANANA = "Banana";

			}

			namespace DangerTypeValues {

				const char* SPIKE_TRAP = "Spike";

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
				const char* ON_HEALTH_FELL = "onHealthFell";
				const char* ON_SNAKE_POSITION = "onSnakePosition";
				const char* ON_FOOD_EATEN = "onFoodEaten";

			}

			namespace SoundTriggerTypeValues {

				const char* ON_TIMER = "onTimer";
				const char* ON_FIRST_FOOD_SPAWN = "onFirstFoodSpawn";
				const char* ON_FIRST_DANGER_SPAWN = "onFirstDangerSpawn";
				const char* ON_LENGTH_REACHED = "onLengthReached";
				const char* ON_DAMAGED = "onDamaged";

			}

			const char* PATH_SLASH = "/";
			const char* EXT_JSON = ".json";
			const char* PATH_CAMPAIGNS = "resources/campaigns/";
			const char* PATH_TEXTURES = "textures";
			const char* PATH_MUSIC = "music";
			const char* PATH_SOUND = "sound";
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

			std::string resolveSoundFilePath(const std::string& campaignFolderName, const std::string& soundFilename) {
				std::string result(PATH_CAMPAIGNS);
				result.append(campaignFolderName);
				result.append(PATH_SLASH);
				result.append(PATH_SOUND);
				result.append(PATH_SLASH);
				result.append(soundFilename);
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

				std::string strLabel = loadResult.jsonValue.get(CampaignListProperties::MENU_LABEL, folderName).asString();
				std::string strDescription = loadResult.jsonValue.get(CampaignListProperties::MENU_DESCRIPTION, "Error in campaign.json - check log.txt for details...").asString();

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
						std::string currFolderName = loadResult.jsonValue[index].asString();
						LoadCampaignOptionResult currCampaignOptionResult = retrieveCampaignOption(index, currFolderName);
						result.campaignOptionList.push_back(currCampaignOptionResult);
						result.allEntriesValid = result.allEntriesValid && currCampaignOptionResult.valid();
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
				else if (jsonValueStr.compare(FoodTypeValues::CARROT) == 0) {
					result = StoryFoodType::CARROT;
				}
				else if (jsonValueStr.compare(FoodTypeValues::GREEN_APPLE) == 0) {
					result = StoryFoodType::GREEN_APPLE;
				}
				else if (jsonValueStr.compare(FoodTypeValues::CHILI_PEPPER) == 0) {
					result = StoryFoodType::CHILI_PEPPER;
				}
				else if (jsonValueStr.compare(FoodTypeValues::CHERRIES) == 0) {
					result = StoryFoodType::CHERRIES;
				}
				else if (jsonValueStr.compare(FoodTypeValues::ORANGE) == 0) {
					result = StoryFoodType::ORANGE;
				}
				else if (jsonValueStr.compare(FoodTypeValues::PEAR) == 0) {
					result = StoryFoodType::PEAR;
				}
				else if (jsonValueStr.compare(FoodTypeValues::BANANA) == 0) {
					result = StoryFoodType::BANANA;
				}

				return result;
			}

			StoryDangerType convertJsonValueToDangerType(const Json::Value& jsonValue) {
				StoryDangerType result = StoryDangerType::SPIKE_TRAP;

				std::string jsonValueStr = jsonValue.asString();
				if (jsonValueStr.compare(DangerTypeValues::SPIKE_TRAP) == 0) {
					result = StoryDangerType::SPIKE_TRAP;
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
				else if (jsonValueStr.compare(SpawnTypeValues::ON_HEALTH_FELL) == 0) {
					result = StoryObjectSpawnType::ON_HEALTH_FELL;
				}
				else if (jsonValueStr.compare(SpawnTypeValues::ON_SNAKE_POSITION) == 0) {
					result = StoryObjectSpawnType::ON_SNAKE_POSITION;
				}
				else if (jsonValueStr.compare(SpawnTypeValues::ON_FOOD_EATEN) == 0 ) {
					result = StoryObjectSpawnType::ON_FOOD_EATEN;
				}

				return result;
			}

			sf::IntRect convertJsonValueToIntRect(const Json::Value& jsonValue) {
				sf::IntRect result;
				result.left = jsonValue[StoryLevelProperties::REGION_LEFT].asInt();
				result.top = jsonValue[StoryLevelProperties::REGION_TOP].asInt();
				result.width = jsonValue[StoryLevelProperties::REGION_WIDTH].asInt();
				result.height = jsonValue[StoryLevelProperties::REGION_HEIGHT].asInt();
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
					result.interval = jsonValue[StoryLevelProperties::INTERVAL].asInt();
				}

				if (result.spawnType == StoryObjectSpawnType::ON_HEALTH_FELL) {
					result.health = jsonValue[StoryLevelProperties::FOOD_HEALTH].asInt();
				}

				if (result.spawnType == StoryObjectSpawnType::ON_SNAKE_POSITION) {
					result.region = convertJsonValueToIntRect(jsonValue[StoryLevelProperties::REGION]);
				}

				if (result.spawnType == StoryObjectSpawnType::ON_FOOD_EATEN) {
					result.eatenFoodType = convertJsonValueToFoodType(jsonValue[StoryLevelProperties::EATEN_FOOD_TYPE]);
					result.eatenFoodCount = jsonValue[StoryLevelProperties::EATEN_FOOD_COUNT].asInt();
				}

				Json::Value floorIdRangeValue = jsonValue[StoryLevelProperties::OBJECT_FLOOR_ID_RANGE];
				result.minFloorId = floorIdRangeValue[StoryLevelProperties::OBJECT_FLOOR_MIN_ID].asInt();
				result.maxFloorId = floorIdRangeValue[StoryLevelProperties::OBJECT_FLOOR_MAX_ID].asInt();

				return result;
			}

			StoryDangerDefn loadStoryDanger(const Json::Value& jsonValue) {
				StoryDangerDefn result;

				result.dangerType = convertJsonValueToDangerType(jsonValue[StoryLevelProperties::DANGER_TYPE]);
				result.spawnType = convertJsonValueToObjectSpawnType(jsonValue[StoryLevelProperties::OBJECT_SPAWN_TYPE]);
				result.chancePct = jsonValue[StoryLevelProperties::OBJECT_CHANCE_PCT].asFloat();
				result.maxSpawnCount = jsonValue[StoryLevelProperties::OBJECT_MAX_SPAWN_COUNT].asInt();
				result.interval = jsonValue[StoryLevelProperties::INTERVAL].asInt();

				if (result.spawnType == StoryObjectSpawnType::ON_TIMER) {
					result.timePassed = jsonValue[StoryLevelProperties::TIME_PASSED].asInt();
				}

				if (result.spawnType == StoryObjectSpawnType::ON_LENGTH_REACHED) {
					result.lengthReached = jsonValue[StoryLevelProperties::OBJECT_LENGTH_REACHED].asInt();
				}

				if (result.spawnType == StoryObjectSpawnType::ON_SNAKE_POSITION) {
					result.region = convertJsonValueToIntRect(jsonValue[StoryLevelProperties::REGION]);
				}

				Json::Value floorIdRangeValue = jsonValue[StoryLevelProperties::OBJECT_FLOOR_ID_RANGE];
				result.minFloorId = floorIdRangeValue[StoryLevelProperties::OBJECT_FLOOR_MIN_ID].asInt();
				result.maxFloorId = floorIdRangeValue[StoryLevelProperties::OBJECT_FLOOR_MAX_ID].asInt();

				return result;
			}

			StorySoundFxTriggerType convertJsonValueToSoundFxTriggerType(const Json::Value& jsonValue) {
				StorySoundFxTriggerType result = StorySoundFxTriggerType::ON_TIMER;

				std::string jsonValueStr = jsonValue.asString();
				if (jsonValueStr.compare(SoundTriggerTypeValues::ON_TIMER) == 0) {
					result = StorySoundFxTriggerType::ON_TIMER;
				}
				else if (jsonValueStr.compare(SoundTriggerTypeValues::ON_FIRST_FOOD_SPAWN) == 0) {
					result = StorySoundFxTriggerType::ON_FIRST_FOOD_SPAWN;
				}
				else if (jsonValueStr.compare(SoundTriggerTypeValues::ON_FIRST_DANGER_SPAWN) == 0) {
					result = StorySoundFxTriggerType::ON_FIRST_DANGER_SPAWN;
				}
				else if (jsonValueStr.compare(SoundTriggerTypeValues::ON_LENGTH_REACHED) == 0) {
					result = StorySoundFxTriggerType::ON_LENGTH_REACHED;
				}
				else if (jsonValueStr.compare(SoundTriggerTypeValues::ON_DAMAGED) == 0) {
					result = StorySoundFxTriggerType::ON_DAMAGED;
				}

				return result;
			}

			StorySoundFxDefn loadStorySoundFx(const Json::Value& jsonValue) {
				StorySoundFxDefn result;

				result.triggerType = convertJsonValueToSoundFxTriggerType(jsonValue[StoryLevelProperties::SOUND_FX_TRIGGER_TYPE]);
				result.soundFilename = jsonValue[StoryLevelProperties::SOUND_FILENAME].asString();

				if (result.triggerType == StorySoundFxTriggerType::ON_TIMER) {
					result.timePassed = jsonValue[StoryLevelProperties::TIME_PASSED].asInt();
				}

				if (result.triggerType == StorySoundFxTriggerType::ON_FIRST_FOOD_SPAWN) {
					result.foodType = convertJsonValueToFoodType(jsonValue[StoryLevelProperties::FOOD_TYPE]);
				}

				if (result.triggerType == StorySoundFxTriggerType::ON_FIRST_DANGER_SPAWN) {
					result.dangerType = convertJsonValueToDangerType(jsonValue[StoryLevelProperties::DANGER_TYPE]);
				}

				if (result.triggerType == StorySoundFxTriggerType::ON_LENGTH_REACHED) {
					result.lengthReached = jsonValue[StoryLevelProperties::OBJECT_LENGTH_REACHED].asInt();
				}

				return result;
			}

			StoryCutsceneEventType convertJsonValueToCutsceneEventType(const Json::Value& jsonValue) {
				StoryCutsceneEventType result = StoryCutsceneEventType::COLOR;

				std::string jsonValueStr = jsonValue.asString();
				if (jsonValueStr.compare(CutsceneEventTypeValues::COLOR) == 0) {
					result = StoryCutsceneEventType::COLOR;
				}
				else if (jsonValueStr.compare(CutsceneEventTypeValues::TEXTURE) == 0) {
					result = StoryCutsceneEventType::TEXTURE;
				}
				else if (jsonValueStr.compare(CutsceneEventTypeValues::SHOW_MAP) == 0) {
					result = StoryCutsceneEventType::SHOW_MAP;
				}
				else if (jsonValueStr.compare(CutsceneEventTypeValues::SHOW_SNAKE) == 0) {
					result = StoryCutsceneEventType::SHOW_SNAKE;
				}
				else if (jsonValueStr.compare(CutsceneEventTypeValues::MOVE_SNAKE) == 0) {
					result = StoryCutsceneEventType::MOVE_SNAKE;
				}
				else if (jsonValueStr.compare(CutsceneEventTypeValues::HIDE_SNAKE) == 0) {
					result = StoryCutsceneEventType::HIDE_SNAKE;
				}
				else if (jsonValueStr.compare(CutsceneEventTypeValues::SHOW_FOOD) == 0) {
					result = StoryCutsceneEventType::SHOW_FOOD;
				}
				else if (jsonValueStr.compare(CutsceneEventTypeValues::HIDE_FOOD) == 0) {
					result = StoryCutsceneEventType::HIDE_FOOD;
				}
				else if (jsonValueStr.compare(CutsceneEventTypeValues::SHOW_DANGER) == 0) {
					result = StoryCutsceneEventType::SHOW_DANGER;
				}
				else if (jsonValueStr.compare(CutsceneEventTypeValues::HIDE_DANGER) == 0) {
					result = StoryCutsceneEventType::HIDE_DANGER;
				}

				return result;
			}

			StoryCutsceneEventDefn loadStoryCutsceneEvent(const Json::Value& jsonValue) {
				StoryCutsceneEventDefn result;

				result.framesSinceLastEvent = jsonValue[StoryCutsceneProperties::FRAMES_SINCE_LAST_EVENT].asInt();
				result.eventType = convertJsonValueToCutsceneEventType(jsonValue[StoryCutsceneProperties::EVENT_TYPE]);
				
				if (result.eventType == StoryCutsceneEventType::COLOR) {
					result.colorEvent.fadeFrames = jsonValue[StoryCutsceneProperties::FADE_FRAMES].asInt();

					Json::Value colorValue = jsonValue[StoryCutsceneProperties::COLOR];
					int red = colorValue[StoryCutsceneProperties::COLOR_RED].asInt();
					int green = colorValue[StoryCutsceneProperties::COLOR_GREEN].asInt();
					int blue = colorValue[StoryCutsceneProperties::COLOR_BLUE].asInt();

					result.colorEvent.color = sf::Color(red, green, blue, 255);
				}
				else if (result.eventType == StoryCutsceneEventType::TEXTURE) {
					result.textureEvent.fadeFrames = jsonValue[StoryCutsceneProperties::FADE_FRAMES].asInt();
					result.textureEvent.textureFilename = jsonValue[StoryCutsceneProperties::TEXTURE].asString();
				}
				else if (result.eventType == StoryCutsceneEventType::SHOW_MAP) {
					result.mapEvent.fadeFrames = jsonValue[StoryCutsceneProperties::FADE_FRAMES].asInt();
					result.mapEvent.mapFilename = jsonValue[StoryLevelProperties::MAP_FILENAME].asString();
				}
				else if (result.eventType == StoryCutsceneEventType::SHOW_SNAKE) {
					Json::Value snakeStartValue = jsonValue[StoryLevelProperties::SNAKE_START];

					Json::Value snakeStartPositionValue = snakeStartValue[StoryLevelProperties::POSITION];
					result.snakeEvent.snakeStart.headPosition.x = snakeStartPositionValue[StoryLevelProperties::POSITION_X].asInt();
					result.snakeEvent.snakeStart.headPosition.y = snakeStartPositionValue[StoryLevelProperties::POSITION_Y].asInt();

					result.snakeEvent.snakeStart.facingDirection = convertJsonValueToObjectDirection(snakeStartValue[StoryLevelProperties::SNAKE_START_DIRECTION]);

					result.snakeEvent.snakeStart.length = snakeStartValue[StoryLevelProperties::SNAKE_START_LENGTH].asInt();
				}
				else if (result.eventType == StoryCutsceneEventType::MOVE_SNAKE) {
					Json::Value snakeMovementValue = jsonValue[StoryCutsceneProperties::SNAKE_MOVEMENT];
					result.snakeEvent.snakeDirection = convertJsonValueToObjectDirection(snakeMovementValue[StoryLevelProperties::SNAKE_START_DIRECTION]);
					result.snakeEvent.snakeGrowFlag = snakeMovementValue[StoryCutsceneProperties::SNAKE_MOVEMENT_GROW].asBool();
				}
				else if (result.eventType == StoryCutsceneEventType::SHOW_FOOD) {
					Json::Value showObjectValue = jsonValue[StoryCutsceneProperties::SHOW_OBJECT];

					result.foodEvent.instanceId = showObjectValue[StoryCutsceneProperties::OBJECT_INSTANCE_ID].asInt();

					result.foodEvent.foodType = convertJsonValueToFoodType(showObjectValue[StoryLevelProperties::FOOD_TYPE]);

					Json::Value positionValue = showObjectValue[StoryLevelProperties::POSITION];
					result.foodEvent.position.x = positionValue[StoryLevelProperties::POSITION_X].asInt();
					result.foodEvent.position.y = positionValue[StoryLevelProperties::POSITION_Y].asInt();
				}
				else if (result.eventType == StoryCutsceneEventType::HIDE_FOOD) {
					Json::Value hideObjectValue = jsonValue[StoryCutsceneProperties::HIDE_OBJECT];
					result.foodEvent.instanceId = hideObjectValue[StoryCutsceneProperties::OBJECT_INSTANCE_ID].asInt();
				}
				else if (result.eventType == StoryCutsceneEventType::SHOW_DANGER) {
					Json::Value showObjectValue = jsonValue[StoryCutsceneProperties::SHOW_OBJECT];

					result.dangerEvent.instanceId = showObjectValue[StoryCutsceneProperties::OBJECT_INSTANCE_ID].asInt();

					result.dangerEvent.dangerType = convertJsonValueToDangerType(showObjectValue[StoryLevelProperties::DANGER_TYPE]);

					Json::Value positionValue = showObjectValue[StoryLevelProperties::POSITION];
					result.dangerEvent.position.x = positionValue[StoryLevelProperties::POSITION_X].asInt();
					result.dangerEvent.position.y = positionValue[StoryLevelProperties::POSITION_Y].asInt();
				}
				else if (result.eventType == StoryCutsceneEventType::HIDE_DANGER) {
					Json::Value hideObjectValue = jsonValue[StoryCutsceneProperties::HIDE_OBJECT];
					result.dangerEvent.instanceId = hideObjectValue[StoryCutsceneProperties::OBJECT_INSTANCE_ID].asInt();
				}

				return result;
			}

			StoryCutsceneDefn loadStoryCutscene(const Json::Value& jsonValue) {
				StoryCutsceneDefn result;

				result.existsFlag = true;
				result.soundTrackFilename = jsonValue.get(StoryCutsceneProperties::SOUND_TRACK, "").asString();

				Json::Value eventListValue = jsonValue[StoryCutsceneProperties::EVENT_LIST];
				for (Json::ArrayIndex index = 0; index < eventListValue.size(); index++) {
					Json::Value currEventValue = eventListValue[index];
					result.eventDefnList.push_back(loadStoryCutsceneEvent(currEventValue));
				}

				return result;
			}

			LoadStoryLevelResult loadStoryLevel(const std::string& campaignFolderName, const Json::Value& jsonValue) {
				LoadStoryLevelResult result;

				result.levelDefn.openingCutsceneDefn.existsFlag = jsonValue.isMember(StoryLevelProperties::OPENING_CUTSCENE);
				if (result.levelDefn.openingCutsceneDefn.existsFlag) {
					result.validationResult.openingCutsceneValidationResult = LoadStoryCutsceneValidation::validate(jsonValue, StoryLevelProperties::OPENING_CUTSCENE);
					if (result.validationResult.openingCutsceneValidationResult.valid()) {
						result.levelDefn.openingCutsceneDefn = loadStoryCutscene(jsonValue[StoryLevelProperties::OPENING_CUTSCENE]);
					}
				}

				result.levelDefn.winCutsceneDefn.existsFlag = jsonValue.isMember(StoryLevelProperties::WIN_CUTSCENE);
				if (result.levelDefn.winCutsceneDefn.existsFlag) {
					result.validationResult.winCutsceneValidationResult = LoadStoryCutsceneValidation::validate(jsonValue, StoryLevelProperties::WIN_CUTSCENE);
					if (result.validationResult.winCutsceneValidationResult.valid()) {
						result.levelDefn.winCutsceneDefn = loadStoryCutscene(jsonValue[StoryLevelProperties::WIN_CUTSCENE]);
					}
				}

				result.levelDefn.lossCutsceneDefn.existsFlag = jsonValue.isMember(StoryLevelProperties::LOSS_CUTSCENE);
				if (result.levelDefn.lossCutsceneDefn.existsFlag) {
					result.validationResult.lossCutsceneValidationResult = LoadStoryCutsceneValidation::validate(jsonValue, StoryLevelProperties::LOSS_CUTSCENE);
					if (result.validationResult.lossCutsceneValidationResult.valid()) {
						result.levelDefn.lossCutsceneDefn = loadStoryCutscene(jsonValue[StoryLevelProperties::LOSS_CUTSCENE]);
					}
				}

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

					result.validationResult.snakeStartPositionValid = LoadStoryLevelValidation::positionValid(snakeStartValue);
					if (result.validationResult.snakeStartPositionValid) {
						Json::Value snakeStartPositionValue = snakeStartValue[StoryLevelProperties::POSITION];
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

				result.validationResult.snakeHealthValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::SNAKE_HEALTH, 1);
				if (result.validationResult.snakeHealthValid) {
					result.levelDefn.maxSnakeHealth = jsonValue[StoryLevelProperties::SNAKE_HEALTH].asInt();
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

				result.validationResult.dangerListValid = r3::json::ValidationUtils::requiredArray(jsonValue, StoryLevelProperties::DANGER_LIST);
				if (result.validationResult.dangerListValid) {
					Json::Value dangerListValue = jsonValue[StoryLevelProperties::DANGER_LIST];
					for (Json::ArrayIndex index = 0; index < dangerListValue.size(); index++) {
						Json::Value currDangerValue = dangerListValue[index];

						LoadStoryLevelDangerValidationResult currDangerValidationResult = LoadStoryLevelValidation::validateDangerEntry(currDangerValue);
						result.validationResult.dangerValidationResultList.push_back(currDangerValidationResult);

						if (currDangerValidationResult.valid()) {
							result.levelDefn.dangerDefnList.push_back(loadStoryDanger(currDangerValue));
						}
					}
				}

				result.validationResult.soundFxListValid = r3::json::ValidationUtils::requiredArray(jsonValue, StoryLevelProperties::SOUND_FX_LIST);
				if (result.validationResult.soundFxListValid) {
					Json::Value soundFxListValue = jsonValue[StoryLevelProperties::SOUND_FX_LIST];
					for (Json::ArrayIndex index = 0; index < soundFxListValue.size(); index++) {
						Json::Value currSoundFxValue = soundFxListValue[index];

						LoadStoryLevelSoundFxValidationResult currSoundFxValidationResult = LoadStoryLevelValidation::validateSoundFxEntry(currSoundFxValue);
						result.validationResult.soundFxValidationResultList.push_back(currSoundFxValidationResult);

						if (currSoundFxValidationResult.valid()) {
							result.levelDefn.soundFxDefnList.push_back(loadStorySoundFx(currSoundFxValue));
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

			std::string resolveCurrTimeStr() {
				time_t currTime;
				struct tm* timeInfo;
				char timeStr[80];

				time(&currTime);
				timeInfo = localtime(&currTime);
				strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeInfo);

				std::string result(timeStr);
				return result;
			}

			void appendToErrorLog(const std::vector<std::string>& errorMessages) {
				std::ofstream errorLogStream;
				errorLogStream.open("log.txt", std::ios_base::app);

				errorLogStream << resolveCurrTimeStr() << "\n";
				for (auto const& currErrorMessage : errorMessages) {
					errorLogStream << currErrorMessage << "\n";
				}
				errorLogStream << "\n";
			}

			void appendFailedFilenameListToErrorLog(const std::vector<std::string>& failedFilenameList) {
				std::ofstream errorLogStream;
				errorLogStream.open("log.txt", std::ios_base::app);

				errorLogStream << resolveCurrTimeStr() << "\n";
				for (auto const& currFailedFilename : failedFilenameList) {
					errorLogStream << "Error loading asset file <" << currFailedFilename << ">\n";
				}
				errorLogStream << "\n";
			}

		}

	}

}
