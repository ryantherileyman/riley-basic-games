
#include "../includes/r3-json-JsonValidationUtils.hpp"
#include "../includes/r3-snake-storyloader.hpp"

namespace r3 {

	namespace snake {

		namespace StoryLoaderUtils {

			namespace LoadStoryMapValidation {

				const int MIN_WIDTH = 3;
				const int MAX_WIDTH = 200;

				const int MIN_HEIGHT = 3;
				const int MAX_HEIGHT = 100;

				bool widthValid(const Json::Value& jsonValue) {
					bool result = r3::json::ValidationUtils::requiredInt(jsonValue, StoryMapProperties::WIDTH);

					if (result) {
						int width = jsonValue[StoryMapProperties::WIDTH].asInt();
						result =
							(width >= MIN_WIDTH) &&
							(width <= MAX_WIDTH);
					}

					return result;
				}

				bool heightValid(const Json::Value& jsonValue) {
					bool result = r3::json::ValidationUtils::requiredInt(jsonValue, StoryMapProperties::HEIGHT);

					if (result) {
						int height = jsonValue[StoryMapProperties::HEIGHT].asInt();
						result =
							(height >= MIN_HEIGHT) &&
							(height <= MAX_HEIGHT);
					}

					return result;
				}

				bool tileValid(const Json::Value& jsonValue, int minTileId) {
					bool result = false;

					if (jsonValue.isObject()) {
						result =
							r3::json::ValidationUtils::requiredInt(jsonValue, StoryMapProperties::TILE_ID) &&
							r3::json::ValidationUtils::requiredString(jsonValue, StoryMapProperties::TILE_FILENAME);

						if (result) {
							int tileId = jsonValue[StoryMapProperties::TILE_ID].asInt();
							result = (tileId >= minTileId);
						}
					}

					return result;
				}

				bool tileListValid(const Json::Value& jsonValue, const char* propertyName, int minTileId) {
					bool result = r3::json::ValidationUtils::requiredArray(jsonValue, propertyName);

					if (result) {
						Json::Value tileList = jsonValue[propertyName];
						for (Json::ArrayIndex index = 0; index < tileList.size(); index++) {
							result = result && tileValid(tileList[index], minTileId);
						}
					}

					return result;
				}

				bool tileGridValid(const Json::Value& jsonValue, const char* propertyName) {
					bool result = r3::json::ValidationUtils::requiredArray(jsonValue, propertyName);

					if (result) {
						Json::Value tileGrid = jsonValue[propertyName];
						for (Json::ArrayIndex rowIndex = 0; rowIndex < tileGrid.size(); rowIndex++) {
							bool rowValid = tileGrid[rowIndex].isArray();
							if (rowValid) {
								Json::Value tileRow = tileGrid[rowIndex];
								for (Json::ArrayIndex colIndex = 0; colIndex < tileRow.size(); colIndex++) {
									rowValid = rowValid && tileRow[colIndex].isInt();
								}
							}

							result = result && rowValid;
						}
					}

					return result;
				}

				bool tileGridSizeValid(const Json::Value& jsonValue, const char* propertyName, const sf::Vector2i& fieldSize) {
					bool result = false;

					Json::Value tileGrid = jsonValue[propertyName];
					if (tileGrid.size() == fieldSize.y) {
						result = true;

						for (Json::ArrayIndex rowIndex = 0; rowIndex < tileGrid.size(); rowIndex++) {
							Json::Value tileRow = tileGrid[rowIndex];
							result = result && (tileRow.size() == fieldSize.x);
						}
					}

					return result;
				}

				void updateTextureFileInvalidList(std::vector<std::string>& textureFileInvalidList, const std::string& campaignFolderName, const std::unordered_map<int, StoryMapTileDefn>& storyMapTileDefnMap) {
					for (auto const& currStoryMapTileDefn : storyMapTileDefnMap) {
						bool imageValid = LoadAssetValidation::imageFileValid(campaignFolderName, currStoryMapTileDefn.second.filename);
						if (!imageValid) {
							textureFileInvalidList.push_back(currStoryMapTileDefn.second.filename);
						}
					}
				}

				void updateNonExistentTileIdRefSet(std::set<int>& nonExistentTileIdRefList, const std::unordered_map<int, StoryMapTileDefn>& storyMapTileDefnMap, const std::vector<std::vector<int>>& tileGrid) {
					for (auto const& tileRow : tileGrid) {
						for (auto const& tileCell : tileRow) {
							if (storyMapTileDefnMap.count(tileCell) == 0) {
								nonExistentTileIdRefList.insert(tileCell);
							}
						}
					}
				}

				std::string buildFloorFilenameErrorMessage(const std::string& filename) {
					std::string result("Floor texture <");
					result.append(filename);
					result.append("> could not be loaded from the campaign's textures folder.");
					return result;
				}

				std::string buildBarrierFilenameErrorMessage(const std::string& filename) {
					std::string result("Barrier texture <");
					result.append(filename);
					result.append("> could not be loaded from the campaign's textures folder.");
					return result;
				}

				std::string buildDuplicateFloorIdErrorMessage(int tileId) {
					char tileIdStr[12];
					sprintf(tileIdStr, "%d", tileId);

					std::string result("Floor \"id\" <");
					result.append(tileIdStr);
					result.append("> exists in the \"floorList\" more than once.");

					return result;
				}

				std::string buildDuplicateBarrierIdErrorMessage(int tileId) {
					char tileIdStr[12];
					sprintf(tileIdStr, "%d", tileId);

					std::string result("Barrier \"id\" <");
					result.append(tileIdStr);
					result.append("> exists in the \"barrierList\" more than once.");

					return result;
				}

				std::string buildNonExistentFloorIdRefErrorMessage(int tileId) {
					char tileIdStr[12];
					sprintf(tileIdStr, "%d", tileId);

					std::string result("Floor \"id\" <");
					result.append(tileIdStr);
					result.append("> in the \"floorGrid\" does not exist in the \"floorList\".");

					return result;
				}

				std::string buildNonExistentBarrierIdRefErrorMessage(int tileId) {
					char tileIdStr[12];
					sprintf(tileIdStr, "%d", tileId);

					std::string result("Barrier \"id\" <");
					result.append(tileIdStr);
					result.append("> in the \"barrierGrid\" does not exist in the \"barrierList\".");

					return result;
				}

				std::vector<std::string> buildErrorMessages(const LoadStoryMapValidationResult& validationResult) {
					std::vector<std::string> result;

					if (!validationResult.widthValid) {
						result.push_back("The \"width\" is invalid.  It must be an integer between 3 and 200");
					}
					if (!validationResult.heightValid) {
						result.push_back("The \"height\" is invalid.  It must be an integer between 3 and 100");
					}
					if (!validationResult.floorListValid) {
						result.push_back("The \"floorList\" is invalid.  Each entry in the array must specify an integer \"id\" of 0 or higher, and a \"filename\".");
					}
					if (!validationResult.barrierListValid) {
						result.push_back("The \"barrierList\" is invalid.  Each entry in the array must specify an integer \"id\" of 1 or higher, and a \"filename\".");
					}
					if (!validationResult.floorGridValid) {
						result.push_back("The \"floorGrid\" is invalid.  It must be a two-dimensional array of integers.  Each value must be a valid \"id\" from the \"floorList\".");
					}
					if (!validationResult.barrierGridValid) {
						result.push_back("The \"barrierGrid\" is invalid.  It must be a two-dimensional array of integers.  Each value must be 0, or a valid \"id\" from the \"barrierList\".");
					}

					if (!validationResult.floorGridSizeValid) {
						result.push_back("The \"floorGrid\" dimensions are incorrect.  It must contain \"height\" rows, each containing \"width\" integers.");
					}
					if (!validationResult.barrierGridSizeValid) {
						result.push_back("The \"barrierGrid\" dimensions are incorrect.  It must contain \"height\" rows, each containing \"width\" integers.");
					}

					for (auto const& currFloorFilename : validationResult.floorTextureFileInvalidList) {
						result.push_back(buildFloorFilenameErrorMessage(currFloorFilename));
					}
					for (auto const& currBarrierFilename : validationResult.barrierTextureFileInvalidList) {
						result.push_back(buildBarrierFilenameErrorMessage(currBarrierFilename));
					}

					for (auto currFloorId : validationResult.duplicateFloorIdList) {
						result.push_back(buildDuplicateFloorIdErrorMessage(currFloorId));
					}
					for (auto currBarrierId : validationResult.duplicateBarrierIdList) {
						result.push_back(buildDuplicateBarrierIdErrorMessage(currBarrierId));
					}

					for (auto currFloorId : validationResult.nonExistentFloorIdRefList) {
						result.push_back(buildNonExistentFloorIdRefErrorMessage(currFloorId));
					}
					for (auto currBarrierId : validationResult.nonExistentBarrierIdRefList) {
						result.push_back(buildNonExistentBarrierIdRefErrorMessage(currBarrierId));
					}

					return result;
				}

			}

		}

	}

}
