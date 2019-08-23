
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-quickgamescene.hpp"

namespace r3 {

	namespace snake {

		QuickGameController::QuickGameController(sf::RenderWindow& window) {
			this->window = &window;

			this->renderer = new QuickGameRenderer();

			this->mode = QuickGameMode::WAIT_TO_START;

			this->game = nullptr;

			this->longestSnakeLength = 0;
			this->lastGameBeatLongestSnakeLength = false;

			this->nextSnakeMovementInput = ObjectDirection::NONE;
		}

		QuickGameController::~QuickGameController() {
			if (this->game != nullptr) {
				delete this->game;
			}
			delete this->renderer;
		}

		QuickGameSceneClientRequest QuickGameController::processEvent(sf::Event& event) {
			QuickGameSceneClientRequest result = QuickGameSceneClientRequest::NONE;

			if (event.type == sf::Event::Closed) {
				result = QuickGameSceneClientRequest::EXIT_GAME;
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
			if (this->mode == QuickGameMode::GAME_RUNNING) {
				QuickGameInputRequest inputRequest;
				inputRequest.snakeMovementInput = this->nextSnakeMovementInput;

				QuickGameUpdateResult updateResult = this->game->update(&inputRequest);
				if (updateResult.snakeHitBarrierFlag) {
					this->mode = QuickGameMode::GAME_DONE_SUMMARY;

					this->lastGameBeatLongestSnakeLength = false;
					int endedGameSnakeLength = this->game->getSnake()->getLength();
					if (endedGameSnakeLength > this->longestSnakeLength) {
						this->lastGameBeatLongestSnakeLength = true;
						this->longestSnakeLength = this->game->getSnake()->getLength();
					}
				}

				this->nextSnakeMovementInput = ObjectDirection::NONE;
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
				break;
			case sf::Keyboard::Key::Enter:
				if (this->game != nullptr) {
					delete this->game;
				}

				this->startGame();
				this->mode = QuickGameMode::GAME_RUNNING;
				break;
			}

			return result;
		}

		QuickGameSceneClientRequest QuickGameController::processGameRunningKeyEvent(sf::Event& event) {
			QuickGameSceneClientRequest result = QuickGameSceneClientRequest::NONE;

			switch (event.key.code) {
			case sf::Keyboard::Key::Escape:
				this->mode = QuickGameMode::WAIT_TO_START;

				delete this->game;
				this->game = nullptr;
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
			QuickGameDefn gameDefn;
			gameDefn.fieldSize = sf::Vector2i(50, 25);
			gameDefn.snakeSpeedTilesPerSecond = 10.0f;
			gameDefn.snakeStartDefn.headPosition.x = 25;
			gameDefn.snakeStartDefn.headPosition.y = 10;
			gameDefn.snakeStartDefn.facingDirection = ObjectDirection::DOWN;
			gameDefn.snakeStartDefn.length = 3;

			this->game = new QuickGame(&gameDefn);
		}

	}

}
