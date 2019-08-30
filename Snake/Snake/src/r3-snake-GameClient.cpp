
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>

#include "includes/r3-snake-client.hpp"
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-splashscene.hpp"
#include "includes/r3-snake-quickgamescene.hpp"

namespace r3 {

	namespace snake {

		const wchar_t* WINDOW_TITLE = L"Riley Entertainment - The Garden of Eating";
		const unsigned int WINDOW_INITIAL_WIDTH = 800;
		const unsigned int WINDOW_INITIAL_HEIGHT = 450;

		const sf::Int64 MICROSECONDS_PER_FRAME = 1000000 / 60;

		GameClient::GameClient() {
			this->mode = ClientMode::SPLASH_SCREEN;

			this->window.create(sf::VideoMode(WINDOW_INITIAL_WIDTH, WINDOW_INITIAL_HEIGHT), WINDOW_TITLE);

			this->splashSceneController = new SplashSceneController(this->window);
			this->quickGameController = new QuickGameController(this->window);
		}

		GameClient::~GameClient() {
			delete this->splashSceneController;
			delete this->quickGameController;
		}

		void GameClient::run() {
			sf::Clock clock;

			while (window.isOpen()) {
				clock.restart();

				sf::Event event;
				while (window.pollEvent(event)) {
					switch (this->mode) {
					case ClientMode::SPLASH_SCREEN:
						this->processSplashScreenEvent(event);
						break;
					case ClientMode::QUICK_GAME:
						this->processQuickGameEvent(event);
						break;
					}
				}

				if (window.isOpen()) {
					switch (this->mode) {
					case ClientMode::SPLASH_SCREEN:
						this->splashSceneController->update();
						this->splashSceneController->render();
						break;
					case ClientMode::QUICK_GAME:
						this->quickGameController->update();
						this->quickGameController->render();
						break;
					}

				}

				sf::Int64 microSecondsElapsed = clock.getElapsedTime().asMicroseconds();
				// keep this line for development... printf("microseconds elapsed this frame = %lld\n", microSecondsElapsed);
				if (microSecondsElapsed < MICROSECONDS_PER_FRAME) {
					std::this_thread::sleep_for(std::chrono::microseconds(MICROSECONDS_PER_FRAME - microSecondsElapsed));
				}
			}
		}

		void GameClient::processSplashScreenEvent(sf::Event& event) {
			SplashSceneClientRequest request = this->splashSceneController->processEvent(event);

			switch (request) {
			case SplashSceneClientRequest::EXIT_GAME:
				window.close();
				break;
			case SplashSceneClientRequest::START_QUICK_GAME:
				this->quickGameController->setQuickGameOptions(this->splashSceneController->getQuickGameOptions());
				this->quickGameController->setSystemOptions(this->splashSceneController->getSystemOptions());
				this->mode = ClientMode::QUICK_GAME;
				break;
			case SplashSceneClientRequest::SWITCH_TO_WINDOW:
				this->window.close();
				this->window.create(sf::VideoMode(WINDOW_INITIAL_WIDTH, WINDOW_INITIAL_HEIGHT), WINDOW_TITLE);
				this->window.setView(ViewUtils::createView(WINDOW_INITIAL_WIDTH, WINDOW_INITIAL_HEIGHT));
				break;
			case SplashSceneClientRequest::SWITCH_TO_FULLSCREEN:
				sf::VideoMode fullScreenVideoMode = sf::VideoMode::getDesktopMode();

				this->window.close();
				this->window.create(fullScreenVideoMode, WINDOW_TITLE, sf::Style::Fullscreen);
				this->window.setView(ViewUtils::createView(fullScreenVideoMode.width, fullScreenVideoMode.height));
				break;
			}
		}

		void GameClient::processQuickGameEvent(sf::Event& event) {
			QuickGameSceneClientRequest request = this->quickGameController->processEvent(event);

			switch (request) {
			case QuickGameSceneClientRequest::EXIT_GAME:
				window.close();
				break;
			case QuickGameSceneClientRequest::RETURN_TO_SPLASH_SCREEN:
				this->mode = ClientMode::SPLASH_SCREEN;
				break;
			}

		}

	}

}
