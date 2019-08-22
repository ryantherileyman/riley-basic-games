
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>
#include "r3-snake-lib.hpp"

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
			this->splashSceneController->start();
		}

		GameClient::~GameClient() {
			delete this->splashSceneController;
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
					}
				}

				if (window.isOpen()) {
					switch (this->mode) {
					case ClientMode::SPLASH_SCREEN:
						this->splashSceneController->render();
						break;
					}

				}

				sf::Int64 microSecondsElapsed = clock.getElapsedTime().asMicroseconds();
				if (microSecondsElapsed < MICROSECONDS_PER_FRAME) {
					std::this_thread::sleep_for(std::chrono::microseconds(MICROSECONDS_PER_FRAME - microSecondsElapsed));
				}
			}
		}

		void GameClient::processSplashScreenEvent(sf::Event& event) {
			SplashSceneClientRequest request = this->splashSceneController->processEvent(event);

			switch (request) {
			case SplashSceneClientRequest::EXIT_GAME:
				this->splashSceneController->finish();
				window.close();
				break;
			}
		}

	}

}
