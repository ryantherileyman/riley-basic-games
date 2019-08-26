
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-quickgamescene.hpp"

namespace r3 {

	namespace snake {

		const char* QUICK_GAME_EAT_APPLE_SOUND_PATH = "resources/sounds/eat-apple.wav";
		const char* QUICK_GAME_HIT_BARRIER_SOUND_PATH = "resources/sounds/hit-barrier.wav";

		const char* QUICK_GAME_RUNNING_MUSIC_PATH = "resources/music/WhimsyGroove.ogg";
		const char* QUICK_GAME_DONE_SUMMARY_MUSIC_PATH = "resources/music/AngloZulu.ogg";

		QuickGameController::QuickGameController(sf::RenderWindow& window) {
			this->window = &window;

			this->renderer = new QuickGameRenderer();

			this->mode = QuickGameMode::WAIT_TO_START;
			this->game = nullptr;

			this->eatAppleSoundBuffer = new sf::SoundBuffer();
			this->hitBarrierSoundBuffer = new sf::SoundBuffer();

			this->gameRunningMusic = nullptr;
			this->gameRunningMusicLoaded = false;

			this->gameDoneSummaryMusic = nullptr;
			this->gameDoneSummaryMusicLoaded = false;

			this->longestSnakeLength = 0;
			this->lastGameBeatLongestSnakeLength = false;

			this->nextSnakeMovementInput = ObjectDirection::NONE;

			if (
				!this->eatAppleSoundBuffer->loadFromFile(QUICK_GAME_EAT_APPLE_SOUND_PATH) ||
				!this->hitBarrierSoundBuffer->loadFromFile(QUICK_GAME_HIT_BARRIER_SOUND_PATH)
				) {
				throw "Could not load sound effects";
			}
			this->eatAppleSound.setBuffer(*this->eatAppleSoundBuffer);
			this->hitBarrierSound.setBuffer(*this->hitBarrierSoundBuffer);

			this->startGame();
		}

		QuickGameController::~QuickGameController() {
			if (this->game != nullptr) {
				delete this->game;
			}
			delete this->renderer;

			delete this->eatAppleSoundBuffer;
			delete this->hitBarrierSoundBuffer;

			if (this->gameRunningMusic != nullptr) {
				delete this->gameRunningMusic;
			}
			if (this->gameDoneSummaryMusic != nullptr) {
				delete this->gameDoneSummaryMusic;
			}
		}

		void QuickGameController::setQuickGameOptions(const QuickGameOptionsDefn& gameOptions) {
			this->gameOptions = gameOptions;
			this->startGame();
		}

		QuickGameSceneClientRequest QuickGameController::processEvent(sf::Event& event) {
			QuickGameSceneClientRequest result = QuickGameSceneClientRequest::NONE;

			if (event.type == sf::Event::Closed) {
				result = QuickGameSceneClientRequest::EXIT_GAME;

				this->freeGameRunningMusic();
				this->freeGameDoneSummaryMusic();
			}
			else if (event.type == sf::Event::Resized) {
				this->window->setView(ViewUtils::createView(event.size.width, event.size.height));
			}
			else if (event.type == sf::Event::KeyPressed) {
				switch (this->mode) {
				case QuickGameMode::WAIT_TO_START:
				case QuickGameMode::GAME_DONE_SUMMARY:
					result = this->processWaitToStartKeyEvent(event);
					break;
				case QuickGameMode::GAME_RUNNING:
					result = this->processGameRunningKeyEvent(event);
					break;
				}
			}

			return result;
		}

		void QuickGameController::update() {
			if (this->mode == QuickGameMode::WAIT_TO_START) {
				if (!this->gameRunningMusicLoaded) {
					this->beginWaitToStartMusic();
				}
			}

			if (this->mode == QuickGameMode::GAME_RUNNING) {
				QuickGameInputRequest inputRequest;
				inputRequest.snakeMovementInput = this->nextSnakeMovementInput;

				QuickGameUpdateResult updateResult = this->game->update(&inputRequest);
				if (updateResult.snakeAteAppleFlag) {
					this->eatAppleSound.play();
				}

				if (updateResult.snakeHitBarrierFlag) {
					this->hitBarrierSound.play();

					this->mode = QuickGameMode::GAME_DONE_SUMMARY;

					this->lastGameBeatLongestSnakeLength = false;
					int endedGameSnakeLength = this->game->getSnake()->getLength();
					if (endedGameSnakeLength > this->longestSnakeLength) {
						this->lastGameBeatLongestSnakeLength = true;
						this->longestSnakeLength = this->game->getSnake()->getLength();

						this->beginGameDoneSummaryMusic();
					} else {
						this->beginWaitToStartMusic();
					}
				}

				this->nextSnakeMovementInput = ObjectDirection::NONE;
			}

			if (this->mode == QuickGameMode::GAME_DONE_SUMMARY) {
				if (this->gameDoneSummaryMusicLoaded) {
					bool allMusicStopped =
						(this->gameDoneSummaryMusic->getStatus() != sf::SoundSource::Status::Playing) &&
						(this->gameRunningMusic->getStatus() != sf::SoundSource::Status::Playing);
					if (allMusicStopped) {
						this->beginWaitToStartMusic();
					}
				}
			}
		}

		void QuickGameController::render() {
			QuickGameRenderState renderState;
			renderState.game = this->game;
			renderState.longestSnake = this->longestSnakeLength;
			renderState.lastGameBeatLongestSnakeLength = this->lastGameBeatLongestSnakeLength;

			switch (this->mode) {
			case QuickGameMode::WAIT_TO_START:
				this->renderer->renderWaitToStart(*this->window, renderState);
				break;
			case QuickGameMode::GAME_RUNNING:
				this->renderer->renderGameRunning(*this->window, renderState);
				break;
			case QuickGameMode::GAME_DONE_SUMMARY:
				this->renderer->renderGameDoneSummary(*this->window, renderState);
				break;
			}
			this->window->display();
		}

		QuickGameSceneClientRequest QuickGameController::processWaitToStartKeyEvent(sf::Event& event) {
			QuickGameSceneClientRequest result = QuickGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::Escape:
				result = QuickGameSceneClientRequest::RETURN_TO_SPLASH_SCREEN;

				this->freeGameRunningMusic();
				this->freeGameDoneSummaryMusic();
				break;
			case sf::Keyboard::Key::Enter:
				this->startGame();
				this->mode = QuickGameMode::GAME_RUNNING;

				this->beginGameRunningMusic();
				break;
			}

			return result;
		}

		QuickGameSceneClientRequest QuickGameController::processGameRunningKeyEvent(sf::Event& event) {
			QuickGameSceneClientRequest result = QuickGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::Escape:
				this->mode = QuickGameMode::WAIT_TO_START;
				this->beginWaitToStartMusic();
				break;
			case sf::Keyboard::Key::W:
			case sf::Keyboard::Key::Up:
				this->nextSnakeMovementInput = ObjectDirection::UP;
				break;
			case sf::Keyboard::Key::D:
			case sf::Keyboard::Key::Right:
				this->nextSnakeMovementInput = ObjectDirection::RIGHT;
				break;
			case sf::Keyboard::Key::S:
			case sf::Keyboard::Key::Down:
				this->nextSnakeMovementInput = ObjectDirection::DOWN;
				break;
			case sf::Keyboard::Key::A:
			case sf::Keyboard::Key::Left:
				this->nextSnakeMovementInput = ObjectDirection::LEFT;
				break;
			}

			return result;
		}

		void QuickGameController::startGame() {
			if (this->game != nullptr) {
				delete this->game;
			}

			QuickGameDefn gameDefn;
			gameDefn.fieldSize = this->gameOptions.fieldSize;
			gameDefn.snakeSpeedTilesPerSecond = (float)this->gameOptions.snakeSpeedTilesPerSecond;
			gameDefn.snakeStartDefn.headPosition.x = this->gameOptions.fieldSize.x / 2;
			gameDefn.snakeStartDefn.headPosition.y = this->gameOptions.fieldSize.y / 2 - 1;
			gameDefn.snakeStartDefn.facingDirection = ObjectDirection::DOWN;
			gameDefn.snakeStartDefn.length = 3;

			this->game = new QuickGame(&gameDefn);
		}

		void QuickGameController::ensureGameRunningMusicLoaded() {
			if (!this->gameRunningMusicLoaded) {
				if (this->gameRunningMusic == nullptr) {
					this->gameRunningMusic = new sf::Music();
				}
				this->gameRunningMusicLoaded = this->gameRunningMusic->openFromFile(QUICK_GAME_RUNNING_MUSIC_PATH);
			}
		}

		void QuickGameController::freeGameRunningMusic() {
			if (this->gameRunningMusicLoaded) {
				this->gameRunningMusic->stop();

				delete this->gameRunningMusic;
				this->gameRunningMusic = nullptr;
				this->gameRunningMusicLoaded = false;
			}
		}

		void QuickGameController::ensureGameDoneSummaryMusicLoaded() {
			if (!this->gameDoneSummaryMusicLoaded) {
				if (this->gameDoneSummaryMusic == nullptr) {
					this->gameDoneSummaryMusic = new sf::Music();
				}
				this->gameDoneSummaryMusicLoaded = this->gameDoneSummaryMusic->openFromFile(QUICK_GAME_DONE_SUMMARY_MUSIC_PATH);
			}
		}

		void QuickGameController::freeGameDoneSummaryMusic() {
			if (this->gameDoneSummaryMusicLoaded) {
				this->gameDoneSummaryMusic->stop();

				delete this->gameDoneSummaryMusic;
				this->gameDoneSummaryMusic = nullptr;
				this->gameDoneSummaryMusicLoaded = false;
			}
		}

		void QuickGameController::beginWaitToStartMusic() {
			this->ensureGameRunningMusicLoaded();

			if (this->gameDoneSummaryMusicLoaded) {
				this->gameDoneSummaryMusic->stop();
			}

			this->gameRunningMusic->setLoopPoints(sf::Music::TimeSpan(sf::seconds(0.445f), sf::seconds(9.149f)));
			this->gameRunningMusic->setLoop(true);

			this->gameRunningMusic->play();
			this->gameRunningMusic->setLoop(true);
		}

		void QuickGameController::beginGameRunningMusic() {
			this->ensureGameRunningMusicLoaded();

			if (this->gameDoneSummaryMusicLoaded) {
				this->gameDoneSummaryMusic->stop();
			}

			this->gameRunningMusic->setLoopPoints(sf::Music::TimeSpan(sf::seconds(0.0f), this->gameRunningMusic->getDuration()));
			this->gameRunningMusic->setLoop(true);

			if (this->gameRunningMusic->getStatus() != sf::SoundSource::Status::Playing) {
				this->gameRunningMusic->play();
				this->gameRunningMusic->setLoop(true);
			}
		}

		void QuickGameController::beginGameDoneSummaryMusic() {
			if (this->gameRunningMusicLoaded) {
				this->gameRunningMusic->stop();
			}

			this->ensureGameDoneSummaryMusicLoaded();
			this->gameDoneSummaryMusic->play();
		}

	}

}
