
#include <fstream>
#include <codecvt>
#include "../jsoncpp/json/json.h"
#include "../includes/r3-snake-storyloader.hpp"
#pragma once

namespace r3 {

	namespace snake {

		namespace StoryLoaderUtils {

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
				std::string result("resources/campaigns/");
				result.append(folderName);
				result.append("/campaign.json");
				return result;
			}

			LoadCampaignOptionResult retrieveCampaignOption(int index, const std::string& folderName) {
				std::string filePath = resolveCampaignFilePath(folderName);

				LoadJsonFromFileResult loadResult = loadFromJsonFile(filePath.c_str());

				bool menuLabelValid =
					loadResult.jsonValue.isMember("menuLabel") &&
					loadResult.jsonValue["menuLabel"].isString();

				bool menuDescriptionValid = true;
				if (loadResult.jsonValue.isMember("menuDescription")) {
					menuDescriptionValid = loadResult.jsonValue["menuDescription"].isString();
				}

				LoadCampaignOptionResult result;
				result.parseErrorString = loadResult.parseErrorString;
				result.optionValid =
					menuLabelValid &&
					menuDescriptionValid;

				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> stringConverter;

				std::string strLabel = loadResult.jsonValue["menuLabel"].asString();
				std::string strDescription = loadResult.jsonValue["menuDescription"].asString();

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

		}

	}

}
