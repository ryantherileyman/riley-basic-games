
#include "../includes/r3-snake-utils.hpp"
#include "../includes/r3-snake-storymodescene.hpp"

namespace r3 {

	namespace snake {

		namespace StoryGameRenderConstants {

			const char* UI_FONT_PATH = "resources/fonts/SourceSansPro-Regular.otf";
			const char* HEALTH_BAR_TEXTURE_PATH = "resources/textures/health-bar.png";

			const wchar_t* HUD_SCORE_FORMAT_STRING = L"Score: %d";
			const wchar_t* HUD_HEALTH = L"Health";
			const wchar_t* HUD_FOOD_LEFT = L"%s Left: %d";
			const wchar_t* HUD_SNAKE_LENGTH = L"Snake Length: %d / %d";
			const wchar_t* HUD_TIME_REMAINING = L"Time Left: %02d:%02d";

			const wchar_t* START_INSTRUCTIONS = L"Press ENTER to start game";
			const wchar_t* EXIT_INSTRUCTIONS = L"Press ESC to return to the main menu";

			const sf::Color BACKGROUND_COLOR = sf::Color(0, 126, 3, 255);

		}

		StoryGameRenderer::StoryGameRenderer() {
			this->uiFont = new sf::Font();
			this->healthBarTexture = new sf::Texture();

			if (!this->uiFont->loadFromFile(StoryGameRenderConstants::UI_FONT_PATH)) {
				throw "Could not load user interface font";
			}

			if (!this->healthBarTexture->loadFromFile(StoryGameRenderConstants::HEALTH_BAR_TEXTURE_PATH)) {
				throw "Could not load health bar texture";
			}
		}

		StoryGameRenderer::~StoryGameRenderer() {
			delete this->uiFont;
			delete this->healthBarTexture;
		}

		void StoryGameRenderer::renderWaitToStart(sf::RenderTarget& renderTarget) {
			renderTarget.clear(StoryGameRenderConstants::BACKGROUND_COLOR);
			this->renderGameRunningUi(renderTarget);
			this->renderWaitToStartInstructions(renderTarget);
		}

		void StoryGameRenderer::renderWaitToStartInstructions(sf::RenderTarget& renderTarget) {
			sf::RectangleShape instructionsBackgroundShape(sf::Vector2f(1260.0f, 400.0f));
			instructionsBackgroundShape.setPosition(330.0f, 390.0f);
			instructionsBackgroundShape.setFillColor(sf::Color(0, 0, 0, 64));
			renderTarget.draw(instructionsBackgroundShape);

			sf::Text startInstructionsText;
			startInstructionsText.setFont(*this->uiFont);
			startInstructionsText.setCharacterSize(72);
			startInstructionsText.setOutlineThickness(2.0f);
			startInstructionsText.setString(StoryGameRenderConstants::START_INSTRUCTIONS);
			FontUtils::setCenteredPosition(startInstructionsText, (ViewUtils::VIEW_SIZE.y / 2.0f) - 50.0f);
			renderTarget.draw(startInstructionsText);

			sf::Text exitInstructionsText;
			exitInstructionsText.setFont(*this->uiFont);
			exitInstructionsText.setCharacterSize(72);
			exitInstructionsText.setOutlineThickness(2.0f);
			exitInstructionsText.setString(StoryGameRenderConstants::EXIT_INSTRUCTIONS);
			FontUtils::setCenteredPosition(exitInstructionsText, (ViewUtils::VIEW_SIZE.y / 2.0f) + 50.0f);
			renderTarget.draw(exitInstructionsText);
		}

		void StoryGameRenderer::renderGameRunningUi(sf::RenderTarget& renderTarget) {
			wchar_t scoreStr[32];
			swprintf_s(scoreStr, StoryGameRenderConstants::HUD_SCORE_FORMAT_STRING, 0); // TODO: score should come from game state

			sf::Text scoreText;
			scoreText.setFont(*this->uiFont);
			scoreText.setCharacterSize(48);
			scoreText.setString(scoreStr);
			scoreText.setPosition(24.0f, 24.0f);
			renderTarget.draw(scoreText);

			wchar_t winConditionStr[48];
			swprintf_s(winConditionStr, StoryGameRenderConstants::HUD_FOOD_LEFT, L"Apples", 10); // TODO: food info should come from game state

			sf::Text winConditionText;
			winConditionText.setFont(*this->uiFont);
			winConditionText.setCharacterSize(48);
			winConditionText.setString(winConditionStr);
			winConditionText.setPosition(ViewUtils::VIEW_SIZE.x - FontUtils::resolveTextWidth(winConditionText) - 24.0f, 24.0f);
			renderTarget.draw(winConditionText);

			sf::Text healthText;
			healthText.setFont(*this->uiFont);
			healthText.setCharacterSize(42);
			healthText.setString(StoryGameRenderConstants::HUD_HEALTH);
			FontUtils::setCenteredPosition(healthText, 16.0f);
			renderTarget.draw(healthText);

			sf::RectangleShape healthBarBackgroundShape(sf::Vector2f(596.0f , 17.0f));
			healthBarBackgroundShape.setPosition(662.0f, 82.0f);
			healthBarBackgroundShape.setFillColor(sf::Color::Black);
			renderTarget.draw(healthBarBackgroundShape);

			sf::RectangleShape healthBarForegroundShape(sf::Vector2f(596.0f * ( 2.0f / 3.0f ), 17.0f)); // TODO: current health / max health should come from game state
			healthBarForegroundShape.setPosition(662.0f, 82.0f);
			healthBarForegroundShape.setFillColor(sf::Color::Red);
			renderTarget.draw(healthBarForegroundShape);

			sf::Sprite healthBarSprite;
			healthBarSprite.setTexture(*this->healthBarTexture);
			healthBarSprite.setPosition(620.0f, 70.0f);
			renderTarget.draw(healthBarSprite);
		}

	}

}
