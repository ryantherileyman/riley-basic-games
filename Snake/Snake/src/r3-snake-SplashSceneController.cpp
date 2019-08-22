
#include "r3-snake-lib.hpp"

namespace r3 {

	namespace snake {

		const char* SPLASH_MUSIC_PATH = "resources/music/AtTheShore.ogg";

		namespace SplashSceneViewUtils {

			const float VIEW_ASPECT_RATIO = 1.77777778f;
			const sf::Vector2f VIEW_SIZE(1920.0f, 1080.0f);

			sf::View createView(unsigned int windowWidth, unsigned int windowHeight) {
				float aspectRatio = (float)windowWidth / (float)windowHeight;
				
				sf::FloatRect viewport(0.0f, 0.0f, VIEW_SIZE.x, VIEW_SIZE.y);

				if (aspectRatio > VIEW_ASPECT_RATIO) {
					viewport.width = VIEW_SIZE.x * (aspectRatio / VIEW_ASPECT_RATIO);
					viewport.left = 0.0f - ((viewport.width - VIEW_SIZE.x) / 2.0f);
				}
				else {
					viewport.height = VIEW_SIZE.y * (VIEW_ASPECT_RATIO / aspectRatio);
					viewport.top = 0.0f - ((viewport.height - VIEW_SIZE.y) / 2.0f);
				}

				sf::View result(viewport);
				return result;
			}

		}

		SplashSceneController::SplashSceneController(sf::RenderWindow& window) {
			this->window = &window;
			this->window->setView(SplashSceneViewUtils::createView(window.getSize().x, window.getSize().y));

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
				this->window->setView(SplashSceneViewUtils::createView(event.size.width, event.size.height));
			}

			return result;
		}

		void SplashSceneController::render() {
			this->renderer->render(*this->window);
			this->window->display();
		}

	}

}
