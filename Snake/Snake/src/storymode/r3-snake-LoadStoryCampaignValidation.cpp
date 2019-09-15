
#include "../includes/r3-snake-storyloader.hpp"

namespace r3 {

	namespace snake {

		namespace StoryLoaderUtils {

			namespace LoadStoryCampaignValidation {

				std::string buildLevelEntryErrorMessage(int index) {
					char resultStr[100];
					sprintf(resultStr, "Entry %d within the \"levelList\" array is invalid.  Individual error messages follow...", index);

					std::string result(resultStr);
					return result;
				}

				void updateErrorMessagesWithLevelValidationResult(std::vector<std::string>& errorMessages, const LoadStoryLevelValidationResult& levelValidationResult) {
					std::vector<std::string> levelErrorMessages = LoadStoryLevelValidation::buildErrorMessages(levelValidationResult);
					for (auto const& currLevelErrorMessage : levelErrorMessages) {
						errorMessages.push_back(currLevelErrorMessage);
					}
				}

				std::vector<std::string> buildErrorMessages(const LoadStoryCampaignResult& loadResult) {
					std::vector<std::string> result;

					if (!loadResult.parseErrorString.empty()) {
						result.push_back(loadResult.parseErrorString);
					}

					if (!loadResult.levelListValid) {
						result.push_back("The \"levelList\" property is invalid.  It must be an array of level definitions.");
					}

					for (size_t levelIndex = 0; levelIndex < loadResult.levelResultList.size(); levelIndex++) {
						LoadStoryLevelResult currLoadLevelResult = loadResult.levelResultList[levelIndex];
						if (!currLoadLevelResult.validationResult.valid()) {
							result.push_back(buildLevelEntryErrorMessage(levelIndex));
							updateErrorMessagesWithLevelValidationResult(result, currLoadLevelResult.validationResult);
						}
					}

					return result;
				}

			}

		}

	}

}
