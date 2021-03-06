
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
				case StoryGameMode::PLAY_WIN_CUTSCENE:
					result = this->processPlayWinCutsceneKeyEvent(event);
					break;
				case StoryGameMode::PLAY_LOSS_CUTSCENE:
					result = this->processPlayLossCutsceneKeyEvent(event);
					break;
				case StoryGameMode::LEVEL_SCORE_SUMMARY:
					result = this->processLevelSummaryKeyEvent(event);
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
				this->updateOpeningCutscene();
				break;
			case StoryGameMode::GAME_RUNNING:
				this->updateGameRunning();
				break;
			case StoryGameMode::PLAY_WIN_CUTSCENE:
				this->updateWinCutscene();
				break;
			case StoryGameMode::PLAY_LOSS_CUTSCENE:
				this->updateLossCutscene();
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
			case StoryGameMode::PLAY_WIN_CUTSCENE:
			case StoryGameMode::PLAY_LOSS_CUTSCENE:
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
			case StoryGameMode::LEVEL_SCORE_SUMMARY:
			{
				StoryLevelSummaryRenderState renderState;
				renderState.gameRenderState.levelAssetBundle = this->levelAssetBundle;
				renderState.gameRenderState.storyGame = this->storyGame;
				renderState.gameRenderState.snakeDamagedFlag = false;
				renderState.foodEatenSummaryMap = &this->foodEatenSummaryMap;

				this->renderer->renderLevelSummary(*this->window, renderState);
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

		StoryGameSceneClientRequest StoryGameController::processPlayWinCutsceneKeyEvent(sf::Event& event) {
			StoryGameSceneClientRequest result = StoryGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::Escape:
			case sf::Keyboard::Key::Enter:
				this->mode = StoryGameMode::LEVEL_SCORE_SUMMARY;

				this->levelAssetBundle->getWinCutsceneMusic().stop();

				delete this->storyCutscene;
				this->storyCutscene = nullptr;
				break;
			}

			return result;
		}

		StoryGameSceneClientRequest StoryGameController::processPlayLossCutsceneKeyEvent(sf::Event& event) {
			StoryGameSceneClientRequest result = StoryGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::Escape:
			case sf::Keyboard::Key::Enter:
				this->mode = StoryGameMode::LEVEL_LOST;

				this->levelAssetBundle->getLossCutsceneMusic().stop();

				delete this->storyCutscene;
				this->storyCutscene = nullptr;
				break;
			}

			return result;
		}

		StoryGameSceneClientRequest StoryGameController::processLevelSummaryKeyEvent(sf::Event& event) {
			StoryGameSceneClientRequest result = StoryGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::Enter:
				this->moveToNextLevel();
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

		void StoryGameController::updateOpeningCutscene() {
			if (
				!this->levelDefnList[this->currLevelIndex].openingCutsceneDefn.soundTrackFilename.empty() &&
				this->storyCutscene->getCurrFrame() == 0
			) {
				this->soundManager.stopAllSounds();

				this->levelAssetBundle->getOpeningCutsceneMusic().setVolume((float)this->systemOptions.soundEffectsVolume);
				this->levelAssetBundle->getOpeningCutsceneMusic().play();
			}

			if (this->storyCutscene->update()) {
				this->mode = StoryGameMode::WAIT_TO_START;

				this->levelAssetBundle->getOpeningCutsceneMusic().stop();

				delete this->storyCutscene;
				this->storyCutscene = nullptr;
			}
		}

		void StoryGameController::updateWinCutscene() {
			if (
				!this->levelDefnList[this->currLevelIndex].winCutsceneDefn.soundTrackFilename.empty() &&
				this->storyCutscene->getCurrFrame() == 0
			) {
				this->soundManager.stopAllSounds();

				this->levelAssetBundle->getWinCutsceneMusic().setVolume((float)this->systemOptions.soundEffectsVolume);
				this->levelAssetBundle->getWinCutsceneMusic().play();
			}

			if (this->storyCutscene->update()) {
				this->mode = StoryGameMode::LEVEL_SCORE_SUMMARY;

				this->levelAssetBundle->getWinCutsceneMusic().stop();

				delete this->storyCutscene;
				this->storyCutscene = nullptr;
			}
		}

		void StoryGameController::updateLossCutscene() {
			if (
				!this->levelDefnList[this->currLevelIndex].lossCutsceneDefn.soundTrackFilename.empty() &&
				this->storyCutscene->getCurrFrame() == 0
			) {
				this->soundManager.stopAllSounds();

				this->levelAssetBundle->getLossCutsceneMusic().setVolume((float)this->systemOptions.soundEffectsVolume);
				this->levelAssetBundle->getLossCutsceneMusic().play();
			}

			if (this->storyCutscene->update()) {
				this->mode = StoryGameMode::LEVEL_LOST;

				this->levelAssetBundle->getLossCutsceneMusic().stop();

				delete this->storyCutscene;
				this->storyCutscene = nullptr;
			}
		}

		void StoryGameController::updateGameRunning() {
			StoryGameInputRequest inputRequest;
			inputRequest.snakeMovementList = this->snakeMovementInputQueue;

			sf::Music& music = this->levelAssetBundle->getMusic();
			if (music.getStatus() == sf::SoundSource::Status::Stopped) {
				music.setLoop(true);
				music.setVolume((float)this->systemOptions.musicVolume);
				music.play();
			}

			StoryGameUpdateResult updateResult = this->storyGame->update(inputRequest);

			if (!updateResult.spawnedFoodInstanceList.empty()) {
				this->soundManager.play(this->levelAssetBundle->getFoodSpawnedSoundBuffer());
			}

			if (!updateResult.foodEatenResultList.empty()) {
				for (auto const& currFoodEatenResult : updateResult.foodEatenResultList) {
					this->updateFoodEatenSummaryMap(currFoodEatenResult);
				}

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

			for (auto const& currSoundFilename : updateResult.soundFileTriggeredList) {
				this->soundManager.play(this->levelAssetBundle->getSoundBuffer(currSoundFilename));
			}

			if (updateResult.snakeDiedFlag) {
				this->storyGame->resetScoreToLevelStart();
				this->foodEatenSummaryMap.clear();
				this->stopRunningLevel();

				if (this->levelDefnList[this->currLevelIndex].lossCutsceneDefn.existsFlag) {
					this->storyCutscene = new StoryCutscene(this->levelDefnList[this->currLevelIndex].lossCutsceneDefn);

					this->mode = StoryGameMode::PLAY_LOSS_CUTSCENE;
				}
				else {
					this->mode = StoryGameMode::LEVEL_LOST;
				}
			}

			if (updateResult.completedLevelFlag) {
				this->stopRunningLevel();

				if (this->levelDefnList[this->currLevelIndex].winCutsceneDefn.existsFlag) {
					this->storyCutscene = new StoryCutscene(this->levelDefnList[this->currLevelIndex].winCutsceneDefn);

					this->mode = StoryGameMode::PLAY_WIN_CUTSCENE;
				}
				else {
					this->mode = StoryGameMode::LEVEL_SCORE_SUMMARY;
				}
			}

			this->snakeMovementInputQueue.clear();
		}

		void StoryGameController::updateFoodEatenSummaryMap(const StoryFoodEatenResult& foodEatenResult) {
			if (this->foodEatenSummaryMap.count(foodEatenResult.foodInstance.foodType) == 0) {
				StoryFoodEatenSummaryDt newFoodEatenSummaryDt;
				newFoodEatenSummaryDt.foodType = foodEatenResult.foodInstance.foodType;
				newFoodEatenSummaryDt.totalEaten = 0;
				newFoodEatenSummaryDt.totalBaseScore = 0;
				newFoodEatenSummaryDt.totalBonusScore = 0;
				newFoodEatenSummaryDt.totalScore = 0;

				this->foodEatenSummaryMap[foodEatenResult.foodInstance.foodType] = newFoodEatenSummaryDt;
			}

			this->foodEatenSummaryMap[foodEatenResult.foodInstance.foodType].totalEaten++;
			this->foodEatenSummaryMap[foodEatenResult.foodInstance.foodType].totalBaseScore += foodEatenResult.scoreResult.baseScore;
			this->foodEatenSummaryMap[foodEatenResult.foodInstance.foodType].totalBonusScore += (foodEatenResult.scoreResult.bonusPathScore + foodEatenResult.scoreResult.perfectPathScore);
			this->foodEatenSummaryMap[foodEatenResult.foodInstance.foodType].totalScore += foodEatenResult.scoreResult.totalScore;
		}

		void StoryGameController::stopRunningLevel() {
			this->storyGame->stopRunningLevel();
			this->levelAssetBundle->getMusic().stop();
		}

		void StoryGameController::moveToNextLevel() {
			if (this->currLevelIndex == (this->levelDefnList.size() - 1)) {
				this->mode = StoryGameMode::CAMPAIGN_WON;
			}
			else {
				this->currLevelIndex++;
				this->mode = StoryGameMode::LOAD_LEVEL;
				this->foodEatenSummaryMap.clear();
				this->storyGame->snapshotScoreAtLevelStart();
				this->initiateLoadLevel();
			}
		}

	}

}
