
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
			this->storyGame = new StoryGame();
			this->storyCutscene = nullptr;

			this->timeSnakeLastDamaged = sf::seconds(-0.5f);
		}

		StoryGameController::~StoryGameController() {
			delete this->renderer;

			if (this->levelAssetBundle != nullptr) {
				delete this->levelAssetBundle;
			}
			delete this->storyGame;
			if (this->storyCutscene != nullptr) {
				delete this->storyCutscene;
			}
		}

		void StoryGameController::setCampaignFolder(const std::string& campaignFolder) {
			this->campaignFolder = campaignFolder;
		}

		void StoryGameController::setSystemOptions(const SystemOptionsDefn& systemOptions) {
			this->systemOptions = systemOptions;

			this->soundManager.setVolume(systemOptions.soundEffectsVolume);
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
				case StoryGameMode::LOAD_CAMPAIGN_ERROR:
				case StoryGameMode::LOAD_LEVEL_ERROR:
					result = this->processLoadErrorKeyEvent(event);
					break;
				case StoryGameMode::PLAY_OPENING_CUTSCENE:
					result = this->processPlayOpeningCutsceneKeyEvent(event);
					break;
				case StoryGameMode::WAIT_TO_START:
					result = this->processWaitToStartKeyEvent(event);
					break;
				case StoryGameMode::GAME_RUNNING:
					result = this->processGameRunningKeyEvent(event);
					break;
				case StoryGameMode::LEVEL_LOST:
					result = this->processLevelLostKeyEvent(event);
					break;
				case StoryGameMode::CAMPAIGN_WON:
					result = this->processCampaignWonKeyEvent(event);
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
			case StoryGameMode::PLAY_OPENING_CUTSCENE:
				this->updateCutscene();
				break;
			case StoryGameMode::GAME_RUNNING:
				this->updateGameRunning();
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
			{
				StoryGameRenderState renderState;
				renderState.levelAssetBundle = this->levelAssetBundle;
				renderState.storyGame = this->storyGame;
				renderState.snakeDamagedFlag = false;

				this->renderer->renderWaitToStart(*this->window, renderState);
				this->window->display();
			}
				break;
			case StoryGameMode::PLAY_OPENING_CUTSCENE:
			{
				StoryCutsceneRenderState renderState;
				renderState.levelAssetBundle = this->levelAssetBundle;
				renderState.storyCutscene = this->storyCutscene;

				this->renderer->renderCutscene(*this->window, renderState);
				this->window->display();

			}
				break;
			case StoryGameMode::GAME_RUNNING:
			{
				StoryGameRenderState renderState;
				renderState.levelAssetBundle = this->levelAssetBundle;
				renderState.storyGame = this->storyGame;
				renderState.snakeDamagedFlag = ((this->storyGame->getTimeElapsed().asSeconds() - this->timeSnakeLastDamaged.asSeconds()) < 0.5f);

				this->renderer->renderGameRunning(*this->window, renderState);
				this->window->display();
			}
				break;
			case StoryGameMode::LEVEL_LOST:
			{
				StoryGameRenderState renderState;
				renderState.levelAssetBundle = this->levelAssetBundle;
				renderState.storyGame = this->storyGame;
				renderState.snakeDamagedFlag = true;

				this->renderer->renderLevelLost(*this->window, renderState);
				this->window->display();
			}
				break;
			case StoryGameMode::CAMPAIGN_WON:
			{
				StoryGameRenderState renderState;
				renderState.levelAssetBundle = this->levelAssetBundle;
				renderState.storyGame = this->storyGame;
				renderState.snakeDamagedFlag = false;

				this->renderer->renderCampaignWon(*this->window, renderState);
				this->window->display();
			}
				break;
			}
		}

		void StoryGameController::loadCampaign() {
			LoadStoryCampaignResult loadResult = StoryLoaderUtils::loadStoryCampaign(this->campaignFolder);
			if (!loadResult.valid()) {
				StoryLoaderUtils::appendToErrorLog(StoryLoaderUtils::LoadStoryCampaignValidation::buildErrorMessages(loadResult));

				this->mode = StoryGameMode::LOAD_CAMPAIGN_ERROR;
			}
			else {
				this->currLevelIndex = 0;
				this->levelDefnList.clear();
				for (auto const& currLevelResult : loadResult.levelResultList) {
					this->levelDefnList.push_back(currLevelResult.levelDefn);
				}

				this->mode = StoryGameMode::LOAD_LEVEL;
				this->storyGame->startNewCampaign();
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
				LoadStoryMapValidationResult loadMapValidationResult = this->levelAssetBundle->getLoadMapValidationResult();
				if (!loadMapValidationResult.valid()) {
					StoryLoaderUtils::appendToErrorLog(StoryLoaderUtils::LoadStoryMapValidation::buildErrorMessages(loadMapValidationResult));
				}

				std::vector<std::string> failedFilenameList = this->levelAssetBundle->getFailedFilenameList();
				if (!failedFilenameList.empty()) {
					StoryLoaderUtils::appendFailedFilenameListToErrorLog(this->levelAssetBundle->getFailedFilenameList());
				}

				this->mode = StoryGameMode::LOAD_LEVEL_ERROR;
			}
			else if ( assetLoadingStatus.completionStatus == StoryLevelAssetLoadingCompletionStatus::COMPLETE) {
				this->storyGame->startNewLevel(this->levelAssetBundle->getMapDefn(), this->levelDefnList[this->currLevelIndex]);

				if (this->levelDefnList[this->currLevelIndex].openingCutsceneDefn.existsFlag) {
					this->storyCutscene = new StoryCutscene(this->levelDefnList[this->currLevelIndex].openingCutsceneDefn);
					if (!this->levelDefnList[this->currLevelIndex].openingCutsceneDefn.soundTrackFilename.empty()) {
						this->levelAssetBundle->getOpeningCutsceneMusic().setVolume((float)this->systemOptions.soundEffectsVolume);
						this->levelAssetBundle->getOpeningCutsceneMusic().play();
					}

					this->mode = StoryGameMode::PLAY_OPENING_CUTSCENE;
				}
				else {
					this->mode = StoryGameMode::WAIT_TO_START;
				}
			}
		}

		StoryGameSceneClientRequest StoryGameController::processLoadErrorKeyEvent(sf::Event& event) {
			StoryGameSceneClientRequest result = StoryGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::Escape:
				this->soundManager.stopAllSounds();
				this->mode = StoryGameMode::LOAD_CAMPAIGN;

				result = StoryGameSceneClientRequest::RETURN_TO_SPLASH_SCREEN;
				break;
			}

			return result;
		}

		StoryGameSceneClientRequest StoryGameController::processPlayOpeningCutsceneKeyEvent(sf::Event& event) {
			StoryGameSceneClientRequest result = StoryGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::Escape:
			case sf::Keyboard::Key::Enter:
				this->mode = StoryGameMode::WAIT_TO_START;

				this->levelAssetBundle->getOpeningCutsceneMusic().stop();

				delete this->storyCutscene;
				this->storyCutscene = nullptr;
				break;
			}

			return result;
		}

		StoryGameSceneClientRequest StoryGameController::processWaitToStartKeyEvent(sf::Event& event) {
			StoryGameSceneClientRequest result = StoryGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::Enter:
				this->storyGame->startRunningLevel();
				this->timeSnakeLastDamaged = sf::seconds(-0.5f);
				this->renderer->clearAnimations();

				this->mode = StoryGameMode::GAME_RUNNING;
				break;
			case sf::Keyboard::Key::Escape:
				this->soundManager.stopAllSounds();
				this->renderer->clearAnimations();

				this->mode = StoryGameMode::LOAD_CAMPAIGN;

				result = StoryGameSceneClientRequest::RETURN_TO_SPLASH_SCREEN;
				break;
			}

			return result;
		}

		StoryGameSceneClientRequest StoryGameController::processGameRunningKeyEvent(sf::Event& event) {
			StoryGameSceneClientRequest result = StoryGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::W:
			case sf::Keyboard::Key::Up:
				this->snakeMovementInputQueue.push_back(ObjectDirection::UP);
				break;
			case sf::Keyboard::Key::D:
			case sf::Keyboard::Key::Right:
				this->snakeMovementInputQueue.push_back(ObjectDirection::RIGHT);
				break;
			case sf::Keyboard::Key::S:
			case sf::Keyboard::Key::Down:
				this->snakeMovementInputQueue.push_back(ObjectDirection::DOWN);
				break;
			case sf::Keyboard::Key::A:
			case sf::Keyboard::Key::Left:
				this->snakeMovementInputQueue.push_back(ObjectDirection::LEFT);
				break;
			}

			return result;
		}

		StoryGameSceneClientRequest StoryGameController::processLevelLostKeyEvent(sf::Event& event) {
			StoryGameSceneClientRequest result = StoryGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::Enter:
				this->storyGame->startNewLevel(this->levelAssetBundle->getMapDefn(), this->levelDefnList[this->currLevelIndex]);
				this->storyGame->startRunningLevel();
				this->timeSnakeLastDamaged = sf::seconds(-0.5f);
				this->renderer->clearAnimations();

				this->mode = StoryGameMode::GAME_RUNNING;
				break;
			case sf::Keyboard::Key::Escape:
				this->soundManager.stopAllSounds();
				this->renderer->clearAnimations();

				this->mode = StoryGameMode::LOAD_CAMPAIGN;

				result = StoryGameSceneClientRequest::RETURN_TO_SPLASH_SCREEN;
				break;
			}

			return result;
		}

		StoryGameSceneClientRequest StoryGameController::processCampaignWonKeyEvent(sf::Event& event) {
			StoryGameSceneClientRequest result = StoryGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::Escape:
				this->soundManager.stopAllSounds();
				this->mode = StoryGameMode::LOAD_CAMPAIGN;

				result = StoryGameSceneClientRequest::RETURN_TO_SPLASH_SCREEN;
				break;
			}

			return result;
		}

		void StoryGameController::updateCutscene() {
			if (this->storyCutscene->update()) {
				this->mode = StoryGameMode::WAIT_TO_START;

				delete this->storyCutscene;
				this->storyCutscene = nullptr;
			}
		}

		void StoryGameController::updateGameRunning() {
			StoryGameInputRequest inputRequest;
			inputRequest.snakeMovementList = this->snakeMovementInputQueue;

			sf::Music& music = this->levelAssetBundle->getMusic();
			if (music.getStatus() == sf::SoundSource::Status::Stopped) {
				music.setVolume((float)this->systemOptions.musicVolume);
				music.play();
			}

			StoryGameUpdateResult updateResult = this->storyGame->update(inputRequest);

			if (!updateResult.spawnedFoodInstanceList.empty()) {
				this->soundManager.play(this->levelAssetBundle->getFoodSpawnedSoundBuffer());
			}

			if (!updateResult.foodEatenResultList.empty()) {
				this->renderer->queueFoodEatenAnimations(updateResult.foodEatenResultList, this->storyGame);

				this->soundManager.play(this->levelAssetBundle->getEatFoodSoundBuffer());
			}

			if (updateResult.snakeHitBarrierFlag) {
				this->soundManager.play(this->levelAssetBundle->getHitBarrierSoundBuffer());
			}

			if (!updateResult.dangerInstanceStruckSnakeList.empty()) {
				this->soundManager.play(this->levelAssetBundle->getSnakeHissSoundBuffer());
			}

			if (updateResult.snakeDamaged()) {
				this->timeSnakeLastDamaged = this->storyGame->getTimeElapsed();
			}

			if (updateResult.snakeDiedFlag) {
				this->storyGame->stopRunningLevel();
				music.stop();

				this->mode = StoryGameMode::LEVEL_LOST;
			}

			if (updateResult.completedLevelFlag) {
				this->storyGame->stopRunningLevel();
				music.stop();

				if (this->currLevelIndex == (this->levelDefnList.size() - 1)) {
					this->mode = StoryGameMode::CAMPAIGN_WON;
				} else {
					this->currLevelIndex++;
					this->mode = StoryGameMode::LOAD_LEVEL;
					this->initiateLoadLevel();
				}
			}

			this->snakeMovementInputQueue.clear();
		}

	}

}
