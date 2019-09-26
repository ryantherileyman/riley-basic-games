
#include "../includes/r3-json-JsonValidationUtils.hpp"
#include "../includes/r3-snake-storyloader.hpp"

namespace r3 {

	namespace snake {

		namespace StoryLoaderUtils {

			namespace LoadStoryLevelValidation {

				bool directionValueValid(const Json::Value& jsonValue) {
					std::string directionString = jsonValue.asString();

					bool result =
						( directionString.compare(DirectionValues::UP) == 0 ) ||
						( directionString.compare(DirectionValues::DOWN) == 0 ) ||
						( directionString.compare(DirectionValues::LEFT) == 0 ) ||
						( directionString.compare(DirectionValues::RIGHT) == 0 );

					return result;
				}

				bool foodTypeValueValid(const Json::Value& jsonValue) {
					std::string foodTypeString = jsonValue.asString();

					bool result =
						(foodTypeString.compare(FoodTypeValues::APPLE) == 0) ||
						(foodTypeString.compare(FoodTypeValues::CARROT) == 0);

					return result;
				}

				bool winConditionTypeValueValid(const Json::Value& jsonValue) {
					std::string winConditionTypeString = jsonValue.asString();

					bool result =
						(winConditionTypeString.compare(WinConditionTypeValues::ON_FOOD_EATEN) == 0) ||
						(winConditionTypeString.compare(WinConditionTypeValues::ON_LENGTH_REACHED) == 0) ||
						(winConditionTypeString.compare(WinConditionTypeValues::ON_TIME_SURVIVED) == 0);

					return result;
				}

				bool spawnTypeValueValid(const Json::Value& jsonValue) {
					std::string spawnTypeString = jsonValue.asString();

					bool result =
						(spawnTypeString.compare(SpawnTypeValues::ON_DESPAWN) == 0) ||
						(spawnTypeString.compare(SpawnTypeValues::ON_LENGTH_REACHED) == 0) ||
						(spawnTypeString.compare(SpawnTypeValues::ON_TIMER) == 0);

					return result;
				}

				bool snakeStartPositionValid(const Json::Value& jsonValue) {
					bool result = r3::json::ValidationUtils::requiredObject(jsonValue, StoryLevelProperties::SNAKE_START_POSITION);

					if (result) {
						Json::Value startPositionValue = jsonValue[StoryLevelProperties::SNAKE_START_POSITION];
						result =
							r3::json::ValidationUtils::requiredInt(startPositionValue, StoryLevelProperties::POSITION_X) &&
							r3::json::ValidationUtils::requiredInt(startPositionValue, StoryLevelProperties::POSITION_Y);

						if (result) {
							int x = jsonValue[StoryLevelProperties::POSITION_X].asInt();
							int y = jsonValue[StoryLevelProperties::POSITION_Y].asInt();

							result =
								(x >= 0) &&
								(y >= 0);
						}
					}

					return result;
				}

				bool snakeStartDirectionValid(const Json::Value& jsonValue) {
					bool result =
						r3::json::ValidationUtils::requiredString(jsonValue, StoryLevelProperties::SNAKE_START_DIRECTION) &&
						directionValueValid(jsonValue[StoryLevelProperties::SNAKE_START_DIRECTION]);
					return result;
				}

				bool foodTypeValid(const Json::Value& jsonValue) {
					bool result =
						r3::json::ValidationUtils::requiredString(jsonValue, StoryLevelProperties::FOOD_TYPE) &&
						foodTypeValueValid(jsonValue[StoryLevelProperties::FOOD_TYPE]);
					return result;
				}

				bool winConditionTypeValid(const Json::Value& jsonValue) {
					bool result =
						r3::json::ValidationUtils::requiredString(jsonValue, StoryLevelProperties::WIN_CONDITION_TYPE) &&
						winConditionTypeValueValid(jsonValue[StoryLevelProperties::WIN_CONDITION_TYPE]);
					return result;
				}

				bool spawnTypeValid(const Json::Value& jsonValue) {
					bool result =
						r3::json::ValidationUtils::requiredString(jsonValue, StoryLevelProperties::OBJECT_SPAWN_TYPE) &&
						spawnTypeValueValid(jsonValue[StoryLevelProperties::OBJECT_SPAWN_TYPE]);
					return result;
				}

				bool chancePctValid(const Json::Value& jsonValue) {
					bool result = r3::json::ValidationUtils::requiredFloat(jsonValue, StoryLevelProperties::OBJECT_CHANCE_PCT);

					if (result) {
						float chancePct = jsonValue[StoryLevelProperties::OBJECT_CHANCE_PCT].asFloat();
						result =
							(chancePct >= 0.01f) &&
							(chancePct <= 100.00f);
					}

					return result;
				}

				bool floorIdRangeValid(const Json::Value& jsonValue) {
					bool result =
						r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::OBJECT_FLOOR_MIN_ID, 0) &&
						r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::OBJECT_FLOOR_MAX_ID, 0);

					if (result) {
						int minId = jsonValue[StoryLevelProperties::OBJECT_FLOOR_MIN_ID].asInt();
						int maxId = jsonValue[StoryLevelProperties::OBJECT_FLOOR_MAX_ID].asInt();

						result = (minId <= maxId);
					}

					return result;
				}

				LoadStoryLevelFoodValidationResult validateFoodEntry(const Json::Value& jsonValue) {
					LoadStoryLevelFoodValidationResult result;

					result.rootValid = jsonValue.isObject();
					if (jsonValue.isObject()) {
						result.foodTypeValid = foodTypeValid(jsonValue);
						result.spawnTypeValid = spawnTypeValid(jsonValue);
						result.chancePctValid = chancePctValid(jsonValue);
						result.maxSpawnCountValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::OBJECT_MAX_SPAWN_COUNT, 1);
						result.growthRateValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::FOOD_GROWTH_RATE, 0);
						result.floorIdRangeValid =
							r3::json::ValidationUtils::requiredObject(jsonValue, StoryLevelProperties::OBJECT_FLOOR_ID_RANGE) &&
							floorIdRangeValid(jsonValue[StoryLevelProperties::OBJECT_FLOOR_ID_RANGE]);

						if (result.spawnTypeValid) {
							std::string spawnTypeStr = jsonValue[StoryLevelProperties::OBJECT_SPAWN_TYPE].asString();

							if (spawnTypeStr.compare(SpawnTypeValues::ON_TIMER) == 0) {
								result.timePassedValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::TIME_PASSED, 1);
							}

							if (spawnTypeStr.compare(SpawnTypeValues::ON_LENGTH_REACHED) == 0) {
								result.lengthReachedValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::OBJECT_LENGTH_REACHED, 2);
							}
						}
					}

					return result;
				}

				std::string buildFoodEntryErrorMessage(int index) {
					char resultStr[100];
					sprintf(resultStr, "Entry %d within the \"foodList\" array is invalid.  Individual error messages follow...", index);

					std::string result(resultStr);
					return result;
				}

				void updateErrorMessagesWithFoodValidationResult(std::vector<std::string>& errorMessages, const LoadStoryLevelFoodValidationResult& foodValidationResult) {
					if (!foodValidationResult.foodTypeValid) {
						errorMessages.push_back("The \"foodType\" is invalid.  It must be a string referencing an available food type.");
					}

					if (!foodValidationResult.spawnTypeValid) {
						errorMessages.push_back("The \"spawnType\" is invalid.  It must be one of \"onDespawn\", \"onTimer\", or \"onLengthReached\".");
					}

					if (!foodValidationResult.chancePctValid) {
						errorMessages.push_back("The \"chancePct\" is invalid.  It must be a real number between 0.01 and 100.00, and is a percentage chance the food will appear each second.");
					}

					if (!foodValidationResult.maxSpawnCountValid) {
						errorMessages.push_back("The \"maxSpawnCount\" is invalid.  It must be an integer of 1 or higher.");
					}

					if (!foodValidationResult.growthRateValid) {
						errorMessages.push_back("The \"growthRate\" is invalid.  It must be an integer or 0 or higher.");
					}

					if (!foodValidationResult.timePassedValid) {
						errorMessages.push_back("The \"timePassed\" is invalid.  It must be an integer or 1 or higher.");
					}

					if (!foodValidationResult.lengthReachedValid) {
						errorMessages.push_back("The \"lengthReached\" is invalid.  It must be an integer of 2 or higher.");
					}

					if (!foodValidationResult.floorIdRangeValid) {
						errorMessages.push_back("The \"floorIdRange\" is invalid.  It must be an object with \"minId\" and \"maxId\" integer properties of 0 or higher.  The \"minId\" must be less or equal to the \"maxId\".");
					}
				}

				std::string buildMusicFilenameErrorMessage(const std::string& filename) {
					std::string result("Music filename <");
					result.append(filename);
					result.append("> could not be loaded from the campaign's \"music\" folder.");
					return result;
				}

				std::vector<std::string> buildErrorMessages(const LoadStoryLevelValidationResult& validationResult) {
					std::vector<std::string> result;

					if (!validationResult.musicValid) {
						result.push_back("The \"music\" is invalid.  It must be a filename, including the extension.");
					}

					if (!validationResult.mapValid) {
						result.push_back("The \"map\" is invalid.  It must be the base filename of the map file, without the .json extension.");
					}

					if (!validationResult.snakeStartValid) {
						result.push_back("The \"snakeStart\" is invalid.  It must be an object with \"position\", \"direction\", and \"length\" properties.");
					}

					if (!validationResult.snakeStartPositionValid) {
						result.push_back("The \"position\" property in the \"snakeStart\" section is invalid.  It must be an object with \"x\" and \"y\" properties.");
					}

					if (!validationResult.snakeStartDirectionValid) {
						result.push_back("The \"direction\" property in the \"snakeStart\" section is invalid.  It must be one of \"up\", \"down\", \"left\", or \"right\".");
					}

					if (!validationResult.snakeStartLengthValid) {
						result.push_back("The \"length\" property in the \"snakeStart\" section is invalid.  It must be an integer of 2 or higher.");
					}

					if (!validationResult.snakeSpeedValid) {
						result.push_back("The \"snakeSpeed\" property is invalid.  It must be an integer of 1 or higher.");
					}

					if (!validationResult.snakeHealthValid) {
						result.push_back("The \"snakeHealth\" property is invalid.  It must be an integer of 1 or higher.");
					}

					if (!validationResult.winConditionValid) {
						result.push_back("The \"winCondition\" property is invalid.  It must be an object with a \"conditionType\" property.");
					}

					if (!validationResult.winConditionTypeValid) {
						result.push_back("The \"conditionType\" property in the \"winCondition\" section is invalid.  It must be one of \"onFoodEaten\", \"onLengthReached\", or \"onTimeSurvived\".");
					}

					if (!validationResult.winConditionFoodTypeValid) {
						result.push_back("The \"foodType\" property in the \"winCondition\" section is invalid.  It must be a string referencing an available food type.");
					}

					if (!validationResult.winConditionFoodCountValid) {
						result.push_back("The \"foodCount\" property in the \"winCondition\" section is invalid.  It must be an integer of 1 or higher.");
					}
					
					if (!validationResult.winConditionSnakeLengthValid) {
						result.push_back("The \"snakeLength\" property in the \"winCondition\" section is invalid.  It must be an integer of 2 or higher.");
					}

					if (!validationResult.winConditionTimePassedValid) {
						result.push_back("The \"timePassed\" property in the \"winCondition\" section is invalid.  It must be an integer of 1 or higher.");
					}

					if (!validationResult.foodListValid) {
						result.push_back("The \"foodList\" property is invalid.  It must be an array of food spawn objects.");
					}

					for (size_t index = 0; index < validationResult.foodValidationResultList.size(); index++) {
						if (!validationResult.foodValidationResultList[index].valid()) {
							result.push_back(buildFoodEntryErrorMessage(index));
							updateErrorMessagesWithFoodValidationResult(result, validationResult.foodValidationResultList[index]);
						}
					}

					for (auto const& currMusicFilename : validationResult.musicFileInvalidList) {
						result.push_back(buildMusicFilenameErrorMessage(currMusicFilename));
					}

					return result;
				}

			}

		}

	}

}
