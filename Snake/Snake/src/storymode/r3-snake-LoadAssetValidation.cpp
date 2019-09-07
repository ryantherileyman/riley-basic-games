
#include <SFML/Graphics.hpp>
#include "../includes/r3-snake-storyloader.hpp"

namespace r3 {

	namespace snake {

		namespace StoryLoaderUtils {

			namespace LoadAssetValidation {

				bool imageFileValid(const std::string& campaignFolderName, const std::string& imageFilename) {
					std::string imageFilePath = resolveImageFilePath(campaignFolderName, imageFilename);
					
					// Is there a better way to validate than to actually load the image into memory?
					// Don't like that we'll be loading assets twice, but this does work...
					sf::Image image;
					bool result = image.loadFromFile(imageFilePath);
					return result;
				}

			}

		}

	}

}
