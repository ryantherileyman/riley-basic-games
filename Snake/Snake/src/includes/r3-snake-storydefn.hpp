
#include <string>
#include <vector>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#pragma once

namespace r3 {

	namespace snake {

		typedef struct Snake_LoadCampaignOptionResult {
			int index;
			std::string folderName;
			std::wstring menuLabel;
			std::wstring menuDescription;

			std::string parseErrorString;
			bool optionValid;

			bool valid() const {
				bool result =
					parseErrorString.empty() &&
					optionValid;
				return result;
			}
		} LoadCampaignOptionResult;

		typedef struct Snake_LoadCampaignListResult {
			std::vector<LoadCampaignOptionResult> campaignOptionList;

			std::string parseErrorString;
			bool isArray;
			bool allEntriesValid;

			bool valid() const {
				bool result =
					(parseErrorString.empty()) &&
					isArray &&
					allEntriesValid;
				return result;
			}
		} LoadCampaignListResult;

		typedef struct Snake_StoryMapTileDefn {
			int tileId;
			std::string filename;
		} StoryMapTileDefn;

		typedef struct Snake_StoryMapDefn {
			sf::Vector2i fieldSize;
			std::unordered_map<int, StoryMapTileDefn> floorDefnMap;
			std::unordered_map<int, StoryMapTileDefn> barrierDefnMap;
			std::vector<std::vector<int>> floorGrid;
			std::vector<std::vector<int>> barrierGrid;
		} StoryMapDefn;

	}

}
