
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

			bool widthValid;
			bool heightValid;
			bool floorListValid;
			bool barrierListValid;
			bool floorGridValid;
			bool barrierGridValid;

			bool floorGridSizeValid;
			bool barrierGridSizeValid;

			std::vector<std::string> floorTextureFileInvalidList;
			std::vector<std::string> barrierTextureFileInvalidList;

			std::vector<int> duplicateFloorIdList;
			std::vector<int> duplicateBarrierIdList;

			std::set<int> nonExistentFloorIdRefList;
			std::set<int> nonExistentBarrierIdRefList;

			bool valid() {
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

		namespace StoryLoaderUtils {

			namespace LoadAssetValidation {

				bool imageFileValid(const std::string& campaignFolderName, const std::string& imageFilename);

			}

			namespace CampaignListProperties {

				extern const char* MENU_LABEL;
				extern const char* MENU_DESCRIPTION;

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

			std::string resolveImageFilePath(const std::string& campaignFolderName, const std::string imageFilename);

			LoadCampaignListResult retrieveCampaignList();

			LoadStoryMapResult loadStoryMap(const std::string& campaignFolderName, const std::string& mapFilename);

		}

	}

}
