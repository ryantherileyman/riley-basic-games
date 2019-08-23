
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-quickgamescene.hpp"

namespace r3 {

	namespace snake {

		QuickGameController::QuickGameController(sf::RenderWindow& window) {
			this->window = &window;

			this->renderer = new QuickGameRenderer();
		}

		QuickGameController::~QuickGameController() {
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
				if (event.key.code == sf::Keyboard::Key::Escape) {
					result = QuickGameSceneClientRequest::RETURN_TO_SPLASH_SCREEN;
				}
			}

			return result;
		}

		void QuickGameController::render() {
			this->renderer->render(*this->window);
			this->window->display();
		}

	}

}
