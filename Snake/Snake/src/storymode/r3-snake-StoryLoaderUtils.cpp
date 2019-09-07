
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

			const char* PATH_SLASH = "/";
			const char* EXT_JSON = ".json";
			const char* PATH_CAMPAIGNS = "resources/campaigns/";
			const char* PATH_TEXTURES = "textures";
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

			std::string resolveImageFilePath(const std::string& campaignFolderName, const std::string imageFilename) {
				std::string result(PATH_CAMPAIGNS);
				result.append(campaignFolderName);
				result.append(PATH_SLASH);
				result.append(PATH_TEXTURES);
				result.append(PATH_SLASH);
				result.append(imageFilename);
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

		}

	}

}
