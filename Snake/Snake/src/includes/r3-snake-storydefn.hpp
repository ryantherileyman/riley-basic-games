
#include <string>
#include <vector>
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

	}

}
