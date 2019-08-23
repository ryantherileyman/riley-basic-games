
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-splashscene.hpp"

namespace r3 {

	namespace snake {

		const char* SPLASH_UI_FONT_PATH = "resources/fonts/SourceSansPro-Regular.otf";

		const char* SPLASH_IMAGE_PATH = "resources/textures/splash-screen.png";
		const sf::Color SPLASH_BACKGROUND_COLOR = sf::Color(0, 126, 3, 255);

		const wchar_t* START_GAME_INSTRUCTIONS = L"Press ENTER to start";

		SplashSceneRenderer::SplashSceneRenderer() {
			this->uiFont = nullptr;
			this->splashTexture = new sf::Texture();
			this->splashSprite = nullptr;

			this->uiFont = new sf::Font();
			if (!this->uiFont->loadFromFile(SPLASH_UI_FONT_PATH)) {
				throw "Could not load user interface font";
			}

			if (!this->splashTexture->loadFromFile(SPLASH_IMAGE_PATH)) {
				throw "Could not load splash screen";
			}

			this->startGameInstructionsText.setFont(*this->uiFont);
			this->startGameInstructionsText.setCharacterSize(64);
			this->startGameInstructionsText.setOutlineColor(sf::Color::White);
			this->startGameInstructionsText.setString(START_GAME_INSTRUCTIONS);
			float startGameInstructionsWidth = FontUtils::resolveTextWidth(this->startGameInstructionsText);
			this->startGameInstructionsText.setPosition((ViewUtils::VIEW_SIZE.x / 2.0f) - (startGameInstructionsWidth / 2.0f), (ViewUtils::VIEW_SIZE.y / 2.0f));

			this->splashSprite = new sf::Sprite();
			this->splashSprite->setTexture(*this->splashTexture);
		}

		SplashSceneRenderer::~SplashSceneRenderer() {
			delete this->uiFont;

			if (this->splashSprite != nullptr) {
				delete this->splashSprite;
			}
			delete this->splashTexture;
		}

		void SplashSceneRenderer::render(sf::RenderTarget& renderTarget) {
			renderTarget.clear(SPLASH_BACKGROUND_COLOR);
			renderTarget.draw(*this->splashSprite);
			renderTarget.draw(this->startGameInstructionsText);
		}

	}

}
