#include "r3-snake-lib.hpp"

namespace r3 {

	namespace snake {

		const char* SPLASH_IMAGE_PATH = "resources/textures/splash-screen.png";
		const sf::Color SPLASH_BACKGROUND_COLOR = sf::Color(0, 126, 3, 255);

		SplashSceneRenderer::SplashSceneRenderer() {
			this->splashTexture = new sf::Texture();
			this->splashSprite = nullptr;

			if (!this->splashTexture->loadFromFile(SPLASH_IMAGE_PATH)) {
				throw "Could not load splash screen";
			}

			this->splashSprite = new sf::Sprite();
			this->splashSprite->setTexture(*this->splashTexture);
		}

		SplashSceneRenderer::~SplashSceneRenderer() {
			if (this->splashSprite != nullptr) {
				delete this->splashSprite;
			}
			delete this->splashTexture;
		}

		void SplashSceneRenderer::render(sf::RenderTarget& renderTarget) {
			renderTarget.clear(SPLASH_BACKGROUND_COLOR);
			renderTarget.draw(*this->splashSprite);
		}

	}

}
