
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
						(foodTypeString.compare(FoodTypeValues::CARROT) == 0) ||
						(foodTypeString.compare(FoodTypeValues::GREEN_APPLE) == 0) ||
						(foodTypeString.compare(FoodTypeValues::CHILI_PEPPER) == 0);

					return result;
				}

				bool dangerTypeValueValid(const Json::Value& jsonValue) {
					std::string dangerTypeString = jsonValue.asString();

					bool result =
						(dangerTypeString.compare(DangerTypeValues::SPIKE_TRAP) == 0);

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

				bool foodSpawnTypeValueValid(const Json::Value& jsonValue) {
					std::string spawnTypeString = jsonValue.asString();

					bool result =
						(spawnTypeString.compare(SpawnTypeValues::ON_DESPAWN) == 0) ||
						(spawnTypeString.compare(SpawnTypeValues::ON_LENGTH_REACHED) == 0) ||
						(spawnTypeString.compare(SpawnTypeValues::ON_TIMER) == 0) ||
						(spawnTypeString.compare(SpawnTypeValues::ON_HEALTH_FELL) == 0);

					return result;
				}

				bool dangerSpawnTypeValueValid(const Json::Value& jsonValue) {
					std::string spawnTypeString = jsonValue.asString();

					bool result =
						(spawnTypeString.compare(SpawnTypeValues::ON_LENGTH_REACHED) == 0) ||
						(spawnTypeString.compare(SpawnTypeValues::ON_TIMER) == 0);

					return result;
				}

				bool soundTriggerTypeValueValid(const Json::Value& jsonValue) {
					std::string triggerTypeValue = jsonValue.asString();

					bool result =
						(triggerTypeValue.compare(SoundTriggerTypeValues::ON_TIMER) == 0) ||
						(triggerTypeValue.compare(SoundTriggerTypeValues::ON_FIRST_FOOD_SPAWN) == 0) ||
						(triggerTypeValue.compare(SoundTriggerTypeValues::ON_FIRST_DANGER_SPAWN) == 0) ||
						(triggerTypeValue.compare(SoundTriggerTypeValues::ON_LENGTH_REACHED) == 0) ||
						(triggerTypeValue.compare(SoundTriggerTypeValues::ON_DAMAGED) == 0);

					return result;
				}

				bool positionValid(const Json::Value& jsonValue) {
					bool result = r3::json::ValidationUtils::requiredObject(jsonValue, StoryLevelProperties::POSITION);

					if (result) {
						Json::Value startPositionValue = jsonValue[StoryLevelProperties::POSITION];
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

				bool dangerTypeValid(const Json::Value& jsonValue) {
					bool result =
						r3::json::ValidationUtils::requiredString(jsonValue, StoryLevelProperties::DANGER_TYPE) &&
						dangerTypeValueValid(jsonValue[StoryLevelProperties::DANGER_TYPE]);
					return result;
				}

				bool winConditionTypeValid(const Json::Value& jsonValue) {
					bool result =
						r3::json::ValidationUtils::requiredString(jsonValue, StoryLevelProperties::WIN_CONDITION_TYPE) &&
						winConditionTypeValueValid(jsonValue[StoryLevelProperties::WIN_CONDITION_TYPE]);
					return result;
				}

				bool foodSpawnTypeValid(const Json::Value& jsonValue) {
					bool result =
						r3::json::ValidationUtils::requiredString(jsonValue, StoryLevelProperties::OBJECT_SPAWN_TYPE) &&
						foodSpawnTypeValueValid(jsonValue[StoryLevelProperties::OBJECT_SPAWN_TYPE]);
					return result;
				}

				bool dangerSpawnTypeValid(const Json::Value& jsonValue) {
					bool result =
						r3::json::ValidationUtils::requiredString(jsonValue, StoryLevelProperties::OBJECT_SPAWN_TYPE) &&
						dangerSpawnTypeValueValid(jsonValue[StoryLevelProperties::OBJECT_SPAWN_TYPE]);
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

				bool soundTriggerTypeValid(const Json::Value& jsonValue) {
					bool result =
						r3::json::ValidationUtils::requiredString(jsonValue, StoryLevelProperties::SOUND_FX_TRIGGER_TYPE) &&
						soundTriggerTypeValueValid(jsonValue[StoryLevelProperties::SOUND_FX_TRIGGER_TYPE]);
					return result;
				}

				LoadStoryLevelFoodValidationResult validateFoodEntry(const Json::Value& jsonValue) {
					LoadStoryLevelFoodValidationResult result;

					result.rootValid = jsonValue.isObject();
					if (jsonValue.isObject()) {
						result.foodTypeValid = foodTypeValid(jsonValue);
						result.spawnTypeValid = foodSpawnTypeValid(jsonValue);
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
								result.intervalValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::FOOD_INTERVAL, 1);
							}

							if (spawnTypeStr.compare(SpawnTypeValues::ON_LENGTH_REACHED) == 0) {
								result.lengthReachedValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::OBJECT_LENGTH_REACHED, 2);
							}

							if (spawnTypeStr.compare(SpawnTypeValues::ON_HEALTH_FELL) == 0) {
								result.healthValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::FOOD_HEALTH, 1);
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
						errorMessages.push_back("The \"spawnType\" is invalid.  It must be one of \"onDespawn\", \"onTimer\", \"onLengthReached\", or \"onHealthFell\".");
					}

					if (!foodValidationResult.chancePctValid) {
						errorMessages.push_back("The \"chancePct\" is invalid.  It must be a real number between 0.01 and 100.00, and is a percentage chance the food will appear each second.");
					}

					if (!foodValidationResult.maxSpawnCountValid) {
						errorMessages.push_back("The \"maxSpawnCount\" is invalid.  It must be an integer of 1 or higher.");
					}

					if (!foodValidationResult.growthRateValid) {
						errorMessages.push_back("The \"growthRate\" is invalid.  It must be an integer of 0 or higher.");
					}

					if (!foodValidationResult.timePassedValid) {
						errorMessages.push_back("The \"timePassed\" is invalid.  It must be an integer of 1 or higher.");
					}

					if (!foodValidationResult.intervalValid) {
						errorMessages.push_back("The \"interval\" is invalid.  It must be an integer of 1 or higher.");
					}

					if (!foodValidationResult.lengthReachedValid) {
						errorMessages.push_back("The \"lengthReached\" is invalid.  It must be an integer of 2 or higher.");
					}

					if (!foodValidationResult.healthValid) {
						errorMessages.push_back("The \"health\" is invalid.  It must be an integer of 1 or higher.");
					}

					if (!foodValidationResult.floorIdRangeValid) {
						errorMessages.push_back("The \"floorIdRange\" is invalid.  It must be an object with \"minId\" and \"maxId\" integer properties of 0 or higher.  The \"minId\" must be less or equal to the \"maxId\".");
					}
				}

				LoadStoryLevelDangerValidationResult validateDangerEntry(const Json::Value& jsonValue) {
					LoadStoryLevelDangerValidationResult result;

					result.rootValid = jsonValue.isObject();
					if (jsonValue.isObject()) {
						result.dangerTypeValid = dangerTypeValid(jsonValue);
						result.spawnTypeValid = dangerSpawnTypeValid(jsonValue);
						result.timePassedValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::TIME_PASSED, 1);
						result.chancePctValid = chancePctValid(jsonValue);
						result.maxSpawnCountValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::OBJECT_MAX_SPAWN_COUNT, 1);
						result.floorIdRangeValid =
							r3::json::ValidationUtils::requiredObject(jsonValue, StoryLevelProperties::OBJECT_FLOOR_ID_RANGE) &&
							floorIdRangeValid(jsonValue[StoryLevelProperties::OBJECT_FLOOR_ID_RANGE]);

						if (result.spawnTypeValid) {
							std::string spawnTypeStr = jsonValue[StoryLevelProperties::OBJECT_SPAWN_TYPE].asString();

							if (spawnTypeStr.compare(SpawnTypeValues::ON_LENGTH_REACHED) == 0) {
								result.lengthReachedValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::OBJECT_LENGTH_REACHED, 2);
							}
						}
					}

					return result;
				}

				std::string buildDangerEntryErrorMessage(int index) {
					char resultStr[100];
					sprintf(resultStr, "Entry %d within the \"dangerList\" array is invalid.  Individual error messages follow...", index);

					std::string result(resultStr);
					return result;
				}

				void updateErrorMessagesWithDangerValidationResult(std::vector<std::string>& errorMessages, const LoadStoryLevelDangerValidationResult& dangerValidationResult) {
					if (!dangerValidationResult.dangerTypeValid) {
						errorMessages.push_back("The \"dangerType\" is invalid.  It must be a string referencing an available danger type.");
					}

					if (!dangerValidationResult.spawnTypeValid) {
						errorMessages.push_back("The \"spawnType\" is invalid.  It must be one of \"onTimer\" or \"onLengthReached\".");
					}

					if (!dangerValidationResult.timePassedValid) {
						errorMessages.push_back("The \"timePassed\" is invalid.  It must be an integer of 1 or higher.");
					}

					if (!dangerValidationResult.chancePctValid) {
						errorMessages.push_back("The \"chancePct\" is invalid.  It must be a real number between 0.01 and 100.00, and is a percentage chance the danger will appear each second.");
					}

					if (!dangerValidationResult.maxSpawnCountValid) {
						errorMessages.push_back("The \"maxSpawnCount\" is invalid.  It must be an integer of 1 or higher.");
					}

					if (!dangerValidationResult.lengthReachedValid) {
						errorMessages.push_back("The \"lengthReached\" is invalid.  It must be an integer of 2 or higher.");
					}

					if (!dangerValidationResult.floorIdRangeValid) {
						errorMessages.push_back("The \"floorIdRange\" is invalid.  It must be an object with \"minId\" and \"maxId\" integer properties of 0 or higher.  The \"minId\" must be less or equal to the \"maxId\".");
					}
				}

				LoadStoryLevelSoundFxValidationResult validateSoundFxEntry(const Json::Value& jsonValue) {
					LoadStoryLevelSoundFxValidationResult result;

					result.rootValid = jsonValue.isObject();
					if (jsonValue.isObject()) {
						result.triggerTypeValid = soundTriggerTypeValid(jsonValue);
						result.soundValid = r3::json::ValidationUtils::requiredString(jsonValue, StoryLevelProperties::SOUND_FILENAME);
						
						if (result.triggerTypeValid) {
							std::string triggerTypeStr = jsonValue[StoryLevelProperties::SOUND_FX_TRIGGER_TYPE].asString();

							if (triggerTypeStr.compare(SoundTriggerTypeValues::ON_TIMER) == 0) {
								result.timePassedValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::TIME_PASSED);
							}

							if (triggerTypeStr.compare(SoundTriggerTypeValues::ON_FIRST_FOOD_SPAWN) == 0) {
								result.foodTypeValid = foodTypeValid(jsonValue);
							}

							if (triggerTypeStr.compare(SoundTriggerTypeValues::ON_FIRST_DANGER_SPAWN) == 0) {
								result.dangerTypeValid = dangerTypeValid(jsonValue);
							}

							if (triggerTypeStr.compare(SoundTriggerTypeValues::ON_LENGTH_REACHED) == 0) {
								result.lengthReachedValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryLevelProperties::OBJECT_LENGTH_REACHED, 2);
							}
						}
					}

					return result;
				}

				std::string buildSoundFxEntryErrorMessage(int index) {
					char resultStr[100];
					sprintf(resultStr, "Entry %d within the \"soundFxList\" entry is invalid.  Individual error messages follow...", index);

					std::string result(resultStr);
					return result;
				}

				void updateErrorMessagesWithSoundFxValidationResult(std::vector<std::string>& errorMessages, const LoadStoryLevelSoundFxValidationResult& soundFxValidationResult) {
					if (!soundFxValidationResult.triggerTypeValid) {
						errorMessages.push_back("The \"triggerType\" is invalid.  It must be one of \"onTimer\", \"onFirstFoodSpawn\", \"onFirstDangerSpawn\", \"onLengthReached\", or \"onDamaged\".");
					}

					if (!soundFxValidationResult.soundValid) {
						errorMessages.push_back("The \"sound\" is invalid.  It must be a filename, including the extension.");
					}

					if (!soundFxValidationResult.timePassedValid) {
						errorMessages.push_back("The \"timePassed\" is invalid.  It must be an integer of 1 or higher.");
					}

					if (!soundFxValidationResult.foodTypeValid) {
						errorMessages.push_back("The \"foodType\" is invalid.  It must be a string referencing an available food type.");
					}

					if (!soundFxValidationResult.dangerTypeValid) {
						errorMessages.push_back("The \"dangerType\" is invalid.  It must be a string referencing an available danger type.");
					}

					if (!soundFxValidationResult.lengthReachedValid) {
						errorMessages.push_back("The \"lengthReached\" is invalid.  It must be an integer of 2 or higher.");
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

					if (!validationResult.openingCutsceneValidationResult.valid()) {
						result.push_back("The \"openingCutscene\" is invalid.  It must be an object representing a cutscene.  Individual error messages follow...");

						std::vector<std::string> openingCutsceneErrorMessages = LoadStoryCutsceneValidation::buildErrorMessages(validationResult.openingCutsceneValidationResult);
						result.insert(result.end(), openingCutsceneErrorMessages.begin(), openingCutsceneErrorMessages.end());
					}

					if (!validationResult.winCutsceneValidationResult.valid()) {
						result.push_back("The \"winCutscene\" is invalid.  It must be an object representing a cutscene.  Individual error messages follow...");

						std::vector<std::string> winCutsceneErrorMessages = LoadStoryCutsceneValidation::buildErrorMessages(validationResult.winCutsceneValidationResult);
						result.insert(result.end(), winCutsceneErrorMessages.begin(), winCutsceneErrorMessages.end());
					}

					if (!validationResult.lossCutsceneValidationResult.valid()) {
						result.push_back("The \"lossCutscene\" is invalid.  It must be an object representing a cutscene.  Individual error messages follow...");

						std::vector<std::string> lossCutsceneErrorMessages = LoadStoryCutsceneValidation::buildErrorMessages(validationResult.lossCutsceneValidationResult);
						result.insert(result.end(), lossCutsceneErrorMessages.begin(), lossCutsceneErrorMessages.end());
					}

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

					if (!validationResult.dangerListValid) {
						result.push_back("The \"dangerList\" property is invalid.  It must be an array of danger spawn objects.");
					}

					for (size_t index = 0; index < validationResult.dangerValidationResultList.size(); index++) {
						if (!validationResult.dangerValidationResultList[index].valid()) {
							result.push_back(buildDangerEntryErrorMessage(index));
							updateErrorMessagesWithDangerValidationResult(result, validationResult.dangerValidationResultList[index]);
						}
					}

					if (!validationResult.soundFxListValid) {
						result.push_back("The \"soundFxList\" property is invalid.  It must be an array of sound fx objects.");
					}

					for (size_t index = 0; index < validationResult.soundFxValidationResultList.size(); index++) {
						if (!validationResult.soundFxValidationResultList[index].valid()) {
							result.push_back(buildSoundFxEntryErrorMessage(index));
							updateErrorMessagesWithSoundFxValidationResult(result, validationResult.soundFxValidationResultList[index]);
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
