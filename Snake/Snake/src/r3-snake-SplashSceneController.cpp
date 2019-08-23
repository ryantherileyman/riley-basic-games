
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-splashscene.hpp"

namespace r3 {

	namespace snake {

		const char* SPLASH_MUSIC_PATH = "resources/music/AtTheShore.ogg";

		SplashSceneController::SplashSceneController(sf::RenderWindow& window) {
			this->window = &window;
			this->window->setView(ViewUtils::createView(window.getSize().x, window.getSize().y));

			this->renderer = new SplashSceneRenderer();

			this->music = { nullptr };
			this->musicLoaded = false;
		}

		SplashSceneController::~SplashSceneController() {
			delete this->renderer;
			if (this->music != nullptr) {
				delete this->music;
			}
		}

		void SplashSceneController::start() {
			if (!this->musicLoaded) {
				if (this->music == nullptr) {
					this->music = new sf::Music();
				}
				this->musicLoaded = this->music->openFromFile(SPLASH_MUSIC_PATH);
			}

			if (this->musicLoaded) {
				this->music->play();
				this->music->setLoop(true);
			}
		}

		void SplashSceneController::finish() {
			if (this->musicLoaded) {
				this->music->stop();
			}
		}

		SplashSceneClientRequest SplashSceneController::processEvent(sf::Event& event) {
			SplashSceneClientRequest result = SplashSceneClientRequest::NONE;

			if (event.type == sf::Event::Closed) {
				result = SplashSceneClientRequest::EXIT_GAME;
			}
			else if (event.type == sf::Event::Resized) {
				this->window->setView(ViewUtils::createView(event.size.width, event.size.height));
			}
			else if (event.type == sf::Event::KeyPressed) {
				if (
					(event.key.code == sf::Keyboard::Key::Enter) ||
					(event.key.code == sf::Keyboard::Key::Space)
				) {
					result = SplashSceneClientRequest::START_QUICK_GAME;
				}
			}

			return result;
		}

		void SplashSceneController::render() {
			this->renderer->render(*this->window);
			this->window->display();
		}

	}

}
