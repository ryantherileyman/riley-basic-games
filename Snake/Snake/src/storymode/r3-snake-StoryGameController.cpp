
#include "../includes/r3-snake-utils.hpp"
#include "../includes/r3-snake-storymodescene.hpp"
#include "../includes/r3-snake-storyloader.hpp"

namespace r3 {

	namespace snake {

		StoryGameController::StoryGameController(sf::RenderWindow& window) {
			this->window = &window;
			this->renderer = new StoryGameRenderer();

			this->mode = StoryGameMode::LOAD_CAMPAIGN;
			this->currLevelIndex = 0;
			this->levelAssetBundle = nullptr;
			this->level = nullptr;
		}

		StoryGameController::~StoryGameController() {
			delete this->renderer;

			if (this->levelAssetBundle != nullptr) {
				delete this->levelAssetBundle;
			}
			if (this->level != nullptr) {
				delete this->level;
			}
		}

		void StoryGameController::setCampaignFolder(const std::string& campaignFolder) {
			this->campaignFolder = campaignFolder;
		}

		void StoryGameController::setSystemOptions(const SystemOptionsDefn& systemOptions) {
			this->systemOptions = systemOptions;
		}

		StoryGameSceneClientRequest StoryGameController::processEvent(sf::Event& event) {
			StoryGameSceneClientRequest result = StoryGameSceneClientRequest::NONE;

			if (event.type == sf::Event::Closed) {
				result = StoryGameSceneClientRequest::EXIT_GAME;
			}
			else if (event.type == sf::Event::Resized) {
				this->window->setView(ViewUtils::createView(event.size.width, event.size.height));
			}
			else if (event.type == sf::Event::KeyPressed) {
				switch (this->mode) {
				case StoryGameMode::WAIT_TO_START:
					result = this->processWaitToStartKeyEvent(event);
					break;
				}
			}

			return result;
		}

		void StoryGameController::update() {
			switch (this->mode) {
			case StoryGameMode::LOAD_CAMPAIGN:
				this->loadCampaign();
				break;
			case StoryGameMode::LOAD_LEVEL:
				this->updateBasedOnLoadLevelStatus();
				break;
			}
		}

		void StoryGameController::render() {
			switch (this->mode) {
			case StoryGameMode::LOAD_CAMPAIGN_ERROR:
				this->renderer->renderLoadCampaignError(*this->window);
				this->window->display();
				break;
			case StoryGameMode::LOAD_LEVEL:
				this->renderer->renderLoadLevelStatus(*this->window, this->levelAssetBundle->getLoadingStatus());
				this->window->display();
				break;
			case StoryGameMode::LOAD_LEVEL_ERROR:
				this->renderer->renderLoadLevelError(*this->window);
				this->window->display();
				break;
			case StoryGameMode::WAIT_TO_START:
				StoryGameRenderState renderState;
				renderState.levelAssetBundle = this->levelAssetBundle;
				renderState.level = this->level;

				this->renderer->renderWaitToStart(*this->window, renderState);
				this->window->display();
				break;
			}
		}

		void StoryGameController::loadCampaign() {
			LoadStoryCampaignResult loadResult = StoryLoaderUtils::loadStoryCampaign(this->campaignFolder);
			if (!loadResult.valid()) {
				// TODO: output errors to log file

				this->mode = StoryGameMode::LOAD_CAMPAIGN_ERROR;
			}
			else {
				this->currLevelIndex = 0;
				this->levelDefnList.clear();
				for (auto const& currLevelResult : loadResult.levelResultList) {
					this->levelDefnList.push_back(currLevelResult.levelDefn);
				}

				this->mode = StoryGameMode::LOAD_LEVEL;

				this->initiateLoadLevel();
			}
		}

		void StoryGameController::initiateLoadLevel() {
			if (this->levelAssetBundle != nullptr) {
				delete this->levelAssetBundle;
			}

			this->levelAssetBundle = new StoryLevelAssetBundle();
			this->levelAssetBundle->loadLevelAsync(this->campaignFolder, this->levelDefnList[this->currLevelIndex]);
		}

		void StoryGameController::updateBasedOnLoadLevelStatus() {
			StoryLevelAssetLoadingStatus assetLoadingStatus = this->levelAssetBundle->getLoadingStatus();
			if (assetLoadingStatus.completionStatus == StoryLevelAssetLoadingCompletionStatus::FAILED) {
				// TODO: output errors to log file

				this->mode = StoryGameMode::LOAD_LEVEL_ERROR;
			}
			else if ( assetLoadingStatus.completionStatus == StoryLevelAssetLoadingCompletionStatus::COMPLETE) {
				this->level = new StoryLevel(this->levelAssetBundle->getMapDefn(), this->levelDefnList[this->currLevelIndex]);

				this->mode = StoryGameMode::WAIT_TO_START;
			}
		}

		StoryGameSceneClientRequest StoryGameController::processWaitToStartKeyEvent(sf::Event& event) {
			StoryGameSceneClientRequest result = StoryGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::Escape:
				result = StoryGameSceneClientRequest::RETURN_TO_SPLASH_SCREEN;
				break;
			}

			return result;
		}

	}

}
