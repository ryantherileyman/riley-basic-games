
#include "../includes/r3-json-JsonValidationUtils.hpp"
#include "../includes/r3-snake-storyloader.hpp"

namespace r3 {

	namespace snake {

		namespace StoryLoaderUtils {

			namespace LoadStoryCutsceneValidation {

				bool eventTypeValueValid(const Json::Value& jsonValue) {
					std::string eventTypeString = jsonValue.asString();
					
					bool result =
						(eventTypeString.compare(CutsceneEventTypeValues::COLOR) == 0) ||
						(eventTypeString.compare(CutsceneEventTypeValues::TEXTURE) == 0) ||
						(eventTypeString.compare(CutsceneEventTypeValues::SHOW_MAP) == 0) ||
						(eventTypeString.compare(CutsceneEventTypeValues::SHOW_SNAKE) == 0) ||
						(eventTypeString.compare(CutsceneEventTypeValues::MOVE_SNAKE) == 0) ||
						(eventTypeString.compare(CutsceneEventTypeValues::HIDE_SNAKE) == 0) ||
						(eventTypeString.compare(CutsceneEventTypeValues::SHOW_FOOD) == 0) ||
						(eventTypeString.compare(CutsceneEventTypeValues::HIDE_FOOD) == 0) ||
						(eventTypeString.compare(CutsceneEventTypeValues::SHOW_DANGER) == 0) ||
						(eventTypeString.compare(CutsceneEventTypeValues::HIDE_DANGER) == 0);

					return result;
				}

				bool colorValueValid(const Json::Value& jsonValue) {
					int colorValue = jsonValue.asInt();

					bool result =
						(colorValue >= 0) &&
						(colorValue <= 255);
					return result;
				}

				bool eventTypeValid(const Json::Value& jsonValue) {
					bool result =
						r3::json::ValidationUtils::requiredString(jsonValue, StoryCutsceneProperties::EVENT_TYPE) &&
						eventTypeValueValid(jsonValue[StoryCutsceneProperties::EVENT_TYPE]);
					return result;
				}

				bool colorValid(const Json::Value& jsonValue) {
					bool result =
						r3::json::ValidationUtils::requiredInt(jsonValue, StoryCutsceneProperties::COLOR_RED) &&
						r3::json::ValidationUtils::requiredInt(jsonValue, StoryCutsceneProperties::COLOR_GREEN) &&
						r3::json::ValidationUtils::requiredInt(jsonValue, StoryCutsceneProperties::COLOR_BLUE) &&
						colorValueValid(jsonValue[StoryCutsceneProperties::COLOR_RED]) &&
						colorValueValid(jsonValue[StoryCutsceneProperties::COLOR_GREEN]) &&
						colorValueValid(jsonValue[StoryCutsceneProperties::COLOR_BLUE]);
					return result;
				}

				bool snakeMovementValid(const Json::Value& jsonValue) {
					bool result = r3::json::ValidationUtils::requiredObject(jsonValue, StoryCutsceneProperties::SNAKE_MOVEMENT);

					if (result) {
						Json::Value snakeMovementValue = jsonValue[StoryCutsceneProperties::SNAKE_MOVEMENT];

						result =
							LoadStoryLevelValidation::snakeStartDirectionValid(snakeMovementValue) &&
							r3::json::ValidationUtils::requiredBool(snakeMovementValue, StoryCutsceneProperties::SNAKE_MOVEMENT_GROW);
					}

					return result;
				}

				LoadStoryCutsceneEventValidationResult validateEventEntry(const Json::Value& jsonValue) {
					LoadStoryCutsceneEventValidationResult result;

					result.rootValid = jsonValue.isObject();
					if (result.rootValid) {
						result.framesSinceLastEventValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryCutsceneProperties::FRAMES_SINCE_LAST_EVENT, 0);
						result.eventTypeValid = eventTypeValid(jsonValue);

						if (result.eventTypeValid) {
							std::string eventTypeString = jsonValue[StoryCutsceneProperties::EVENT_TYPE].asString();

							if (eventTypeString.compare(CutsceneEventTypeValues::COLOR) == 0) {
								result.fadeFramesValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryCutsceneProperties::FADE_FRAMES, 0);
								result.colorValid =
									r3::json::ValidationUtils::requiredObject(jsonValue, StoryCutsceneProperties::COLOR) &&
									colorValid(jsonValue[StoryCutsceneProperties::COLOR]);
							}

							if (eventTypeString.compare(CutsceneEventTypeValues::TEXTURE) == 0) {
								result.fadeFramesValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryCutsceneProperties::FADE_FRAMES, 0);
								result.textureValid = r3::json::ValidationUtils::requiredString(jsonValue, StoryCutsceneProperties::TEXTURE);
							}

							if (eventTypeString.compare(CutsceneEventTypeValues::SHOW_MAP) == 0) {
								result.fadeFramesValid = r3::json::ValidationUtils::requiredInt(jsonValue, StoryCutsceneProperties::FADE_FRAMES, 0);
								result.mapValid = r3::json::ValidationUtils::requiredString(jsonValue, StoryLevelProperties::MAP_FILENAME);
							}

							if (eventTypeString.compare(CutsceneEventTypeValues::SHOW_SNAKE) == 0) {
								result.snakeStartValid = r3::json::ValidationUtils::requiredObject(jsonValue, StoryLevelProperties::SNAKE_START);
								if (result.snakeStartValid) {
									Json::Value snakeStartValue = jsonValue[StoryLevelProperties::SNAKE_START];

									result.snakeStartPositionValid = LoadStoryLevelValidation::positionValid(snakeStartValue);
									result.snakeStartDirectionValid = LoadStoryLevelValidation::snakeStartDirectionValid(snakeStartValue);
									result.snakeStartLengthValid = r3::json::ValidationUtils::requiredInt(snakeStartValue, StoryLevelProperties::SNAKE_START_LENGTH, 2);
								}
							}

							if (eventTypeString.compare(CutsceneEventTypeValues::MOVE_SNAKE) == 0) {
								result.snakeMovementValid = snakeMovementValid(jsonValue);
							}

							if (eventTypeString.compare(CutsceneEventTypeValues::SHOW_FOOD) == 0) {
								result.showObjectValid = r3::json::ValidationUtils::requiredObject(jsonValue, StoryCutsceneProperties::SHOW_OBJECT);
								if (result.showObjectValid) {
									Json::Value showObjectValue = jsonValue[StoryCutsceneProperties::SHOW_OBJECT];

									result.objectInstanceIdValid = r3::json::ValidationUtils::requiredInt(showObjectValue, StoryCutsceneProperties::OBJECT_INSTANCE_ID, 1);
									result.objectFoodTypeValid = LoadStoryLevelValidation::foodTypeValid(showObjectValue);
									result.objectPositionValid = LoadStoryLevelValidation::positionValid(showObjectValue);
								}
							}

							if (eventTypeString.compare(CutsceneEventTypeValues::HIDE_FOOD) == 0) {
								result.hideObjectValid = r3::json::ValidationUtils::requiredObject(jsonValue, StoryCutsceneProperties::HIDE_OBJECT);
								if (result.hideObjectValid) {
									Json::Value hideObjectValue = jsonValue[StoryCutsceneProperties::HIDE_OBJECT];

									result.objectInstanceIdValid = r3::json::ValidationUtils::requiredInt(hideObjectValue, StoryCutsceneProperties::OBJECT_INSTANCE_ID, 1);
								}
							}

							if (eventTypeString.compare(CutsceneEventTypeValues::SHOW_DANGER) == 0) {
								result.showObjectValid = r3::json::ValidationUtils::requiredObject(jsonValue, StoryCutsceneProperties::SHOW_OBJECT);
								if (result.showObjectValid) {
									Json::Value showObjectValue = jsonValue[StoryCutsceneProperties::SHOW_OBJECT];

									result.objectInstanceIdValid = r3::json::ValidationUtils::requiredInt(showObjectValue, StoryCutsceneProperties::OBJECT_INSTANCE_ID, 1);
									result.objectDangerTypeValid = LoadStoryLevelValidation::dangerTypeValid(showObjectValue);
									result.objectPositionValid = LoadStoryLevelValidation::positionValid(showObjectValue);
								}
							}

							if (eventTypeString.compare(CutsceneEventTypeValues::HIDE_DANGER) == 0) {
								result.hideObjectValid = r3::json::ValidationUtils::requiredObject(jsonValue, StoryCutsceneProperties::HIDE_OBJECT);
								if (result.hideObjectValid) {
									Json::Value hideObjectValue = jsonValue[StoryCutsceneProperties::HIDE_OBJECT];

									result.objectInstanceIdValid = r3::json::ValidationUtils::requiredInt(hideObjectValue, StoryCutsceneProperties::OBJECT_INSTANCE_ID, 1);
								}
							}
						}
					}

					return result;
				}

				LoadStoryCutsceneValidationResult validate(const Json::Value& jsonValue, const char* propertyName) {
					LoadStoryCutsceneValidationResult result;

					result.existsFlag = jsonValue.isMember(propertyName);
					result.rootValid = r3::json::ValidationUtils::optionalObject(jsonValue, StoryLevelProperties::OPENING_CUTSCENE);
					if (result.rootValid) {
						Json::Value cutsceneValue = jsonValue[propertyName];

						result.soundTrackValid = r3::json::ValidationUtils::optionalString(cutsceneValue, StoryCutsceneProperties::SOUND_TRACK);
						result.eventListValid = r3::json::ValidationUtils::requiredArray(cutsceneValue, StoryCutsceneProperties::EVENT_LIST);
						if (result.eventListValid) {
							Json::Value eventListValue = cutsceneValue[StoryCutsceneProperties::EVENT_LIST];
							for (Json::ArrayIndex index = 0; index < eventListValue.size(); index++) {
								Json::Value currEventValue = eventListValue[index];

								LoadStoryCutsceneEventValidationResult currEventValidationResult = validateEventEntry(currEventValue);
								result.eventValidationResultList.push_back(currEventValidationResult);
							}
						}
					}

					return result;
				}

				std::string buildEventEntryErrorMessage(int index) {
					char resultStr[100];
					sprintf(resultStr, "Entry %d within the \"eventList\" array is invalid.  Individual error messages follow...", index);

					std::string result(resultStr);
					return result;
				}

				void updateErrorMessagesWithEventValidationResult(std::vector<std::string>& errorMessages, const LoadStoryCutsceneEventValidationResult& validationResult) {
					if (!validationResult.framesSinceLastEventValid) {
						errorMessages.push_back("The \"framesSinceLastEvent\" is invalid.  It must be an integer of 0 or higher.");
					}

					if (!validationResult.eventTypeValid) {
						errorMessages.push_back("The \"eventType\" is invalid.  It must be one of "
							"\"color\", \"texture\", \"showMap\", \"showSnake\", \"moveSnake\", \"hideSnake\", \"showFood\", \"hideFood\", \"showDanger\", or \"hideDanger\".");
					}

					if (!validationResult.fadeFramesValid) {
						errorMessages.push_back("The \"fadeFrames\" is invalid.  It must be an integer of 0 or higher.");
					}

					if (!validationResult.colorValid) {
						errorMessages.push_back("The \"color\" is invalid.  It must be an object with \"red\", \"green\", and \"blue\" properties, each an integer from 0 to 255.");
					}

					if (!validationResult.textureValid) {
						errorMessages.push_back("The \"texture\" is invalid.  It must be a filename, including the extension.");
					}

					if (!validationResult.mapValid) {
						errorMessages.push_back("The \"map\" is invalid.  It must be the base filename of the map file, without the .json extension.");
					}

					if (!validationResult.snakeStartValid) {
						errorMessages.push_back("The \"snakeStart\" is invalid.  It must be an object with \"position\", \"direction\", and \"length\" properties.");
					}

					if (!validationResult.snakeStartPositionValid) {
						errorMessages.push_back("The \"position\" property in the \"snakeStart\" section is invalid.  It must be an object with \"x\" and \"y\" properties.");
					}

					if (!validationResult.snakeStartDirectionValid) {
						errorMessages.push_back("The \"direction\" property in the \"snakeStart\" section is invalid.  It must be one of \"up\", \"down\", \"left\", or \"right\".");
					}

					if (!validationResult.snakeStartLengthValid) {
						errorMessages.push_back("The \"length\" property in the \"snakeStart\" section is invalid.  It must be an integer of 2 or higher.");
					}

					if (!validationResult.snakeMovementValid) {
						errorMessages.push_back("The \"snakeMovement\" is invalid.  It must be an object with \"direction\" and \"grow\" properties.");
					}

					if (!validationResult.showObjectValid) {
						errorMessages.push_back("The \"showObject\" is invalid.  It must be an object with \"instanceId\", \"foodType\" or \"dangerType\", and \"position\" properties.");
					}

					if (!validationResult.hideObjectValid) {
						errorMessages.push_back("The \"hideObject\" is invalid.  It must be an object with an \"instanceId\" property.");
					}

					if (!validationResult.objectInstanceIdValid) {
						errorMessages.push_back("The \"instanceId\" is invalid.  It must be an integer of 1 or higher.");
					}

					if (!validationResult.objectFoodTypeValid) {
						errorMessages.push_back("The \"foodType\" in the \"showObject\" section is invalid.  It must be a string referencing an available food type.");
					}

					if (!validationResult.objectDangerTypeValid) {
						errorMessages.push_back("The \"dangerType\" in the \"showObject\" section is invalid.  It must be a string referencing an available danger type.");
					}

					if (!validationResult.objectPositionValid) {
						errorMessages.push_back("The \"position\" in the \"showObject\" section is invalid.  It must be an object with \"x\" and \"y\" properties.");
					}
				}

				std::vector<std::string> buildErrorMessages(const LoadStoryCutsceneValidationResult& validationResult) {
					std::vector<std::string> result;

					if (!validationResult.soundTrackValid) {
						result.push_back("The \"soundTrack\" is invalid.  It must be a filename, including the extension.");
					}

					if (!validationResult.eventListValid) {
						result.push_back("The \"eventList\" is invalid.  It must be an array of cutscene event objects.");
					}

					for (size_t index = 0; index < validationResult.eventValidationResultList.size(); index++) {
						if (!validationResult.eventValidationResultList[index].valid()) {
							result.push_back(buildEventEntryErrorMessage(index));
							updateErrorMessagesWithEventValidationResult(result, validationResult.eventValidationResultList[index]);
						}
					}

					return result;
				}

			}

		}

	}

}
