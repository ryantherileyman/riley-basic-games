
#include "../includes/r3-snake-storyloader.hpp"

namespace r3 {

	namespace snake {

		namespace StoryLoaderUtils {

			namespace LoadStoryCampaignListValidation {

				std::string buildCampaignOptionErrorMessage(const LoadCampaignOptionResult& campaignOption) {
					char resultStr[200];
					sprintf(resultStr, "Parse error in campaign folder <%.150s> follows...", campaignOption.folderName.c_str());

					std::string result(resultStr);
					return result;
				}

				std::vector<std::string> buildErrorMessages(const LoadCampaignListResult& loadResult) {
					std::vector<std::string> result;

					if (!loadResult.parseErrorString.empty()) {
						result.push_back(loadResult.parseErrorString);
					}

					if (!loadResult.isArray) {
						result.push_back("The campaign-list.json file must be an array of strings, where each string is the base folder name of a campaign.");
					}

					for (auto const& currCampaignOption : loadResult.campaignOptionList) {
						if (!currCampaignOption.valid()) {
							result.push_back(buildCampaignOptionErrorMessage(currCampaignOption));

							if (!currCampaignOption.parseErrorString.empty()) {
								result.push_back(currCampaignOption.parseErrorString);
							}
						}

					}

					return result;
				}

			}

		}

	}

}
