
#include <fstream>
#include <codecvt>
#include "../includes/r3-snake-utils.hpp"
#include "../includes/r3-snake-RenderUtils.hpp"
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

			const wchar_t* CAMPAIGN_ERROR_MESSAGE = L"An error occurred attempting to load the campaign.";
			const wchar_t* LEVEL_ERROR_MESSAGE = L"An error occurred attempting to load the assets for the next level.";
			const wchar_t* SEE_LOG_MESSAGE = L"See log.txt in the game folder for more information.";

			const wchar_t* LOAD_LEVEL_PCT_FORMAT_STRING = L"Loading assets...  (%.0f%%)";

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

		void StoryGameRenderer::renderLoadCampaignError(sf::RenderTarget& renderTarget) {
			renderTarget.clear(StoryGameRenderConstants::BACKGROUND_COLOR);

			sf::RectangleShape instructionsBackgroundShape = this->createTextBackgroundShape(1920.0f, 400.0f);
			renderTarget.draw(instructionsBackgroundShape);

			sf::Text errorText = this->createInstructionsText(StoryGameRenderConstants::CAMPAIGN_ERROR_MESSAGE, (ViewUtils::VIEW_SIZE.y / 2.0f) - 150.0f);
			renderTarget.draw(errorText);

			sf::Text seeLogText = this->createInstructionsText(StoryGameRenderConstants::SEE_LOG_MESSAGE, (ViewUtils::VIEW_SIZE.y / 2.0f) - 50.0f);
			renderTarget.draw(seeLogText);
		}

		void StoryGameRenderer::renderLoadLevelStatus(sf::RenderTarget& renderTarget, const StoryLevelAssetLoadingStatus& assetLoadingStatus) {
			renderTarget.clear(StoryGameRenderConstants::BACKGROUND_COLOR);

			sf::RectangleShape instructionsBackgroundShape = this->createTextBackgroundShape(1260.0f, 400.0f);
			renderTarget.draw(instructionsBackgroundShape);

			wchar_t loadLevelPctString[40];
			swprintf_s(loadLevelPctString, StoryGameRenderConstants::LOAD_LEVEL_PCT_FORMAT_STRING, (double)100.0f * assetLoadingStatus.loadedPct);
			sf::Text loadLevelPctText = this->createInstructionsText(loadLevelPctString, (ViewUtils::VIEW_SIZE.y / 2.0f) - 50.0f);
			renderTarget.draw(loadLevelPctText);

			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> stringConverter;
			std::wstring loadFilenameString = stringConverter.from_bytes(assetLoadingStatus.currFilename);
			sf::Text loadFilenameText = this->createInstructionsText(loadFilenameString.c_str(), (ViewUtils::VIEW_SIZE.y / 2.0f) + 50.0f);
			renderTarget.draw(loadFilenameText);
		}

		void StoryGameRenderer::renderLoadLevelError(sf::RenderTarget& renderTarget) {
			renderTarget.clear(StoryGameRenderConstants::BACKGROUND_COLOR);

			sf::RectangleShape instructionsBackgroundShape = this->createTextBackgroundShape(1920.0f, 400.0f);
			renderTarget.draw(instructionsBackgroundShape);

			sf::Text errorText = this->createInstructionsText(StoryGameRenderConstants::LEVEL_ERROR_MESSAGE, (ViewUtils::VIEW_SIZE.y / 2.0f) - 150.0f);
			renderTarget.draw(errorText);

			sf::Text seeLogText = this->createInstructionsText(StoryGameRenderConstants::SEE_LOG_MESSAGE, (ViewUtils::VIEW_SIZE.y / 2.0f) - 50.0f);
			renderTarget.draw(seeLogText);
		}

		void StoryGameRenderer::renderWaitToStart(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState) {
			renderTarget.clear(StoryGameRenderConstants::BACKGROUND_COLOR);
			this->renderGameRunningUi(renderTarget);
			this->renderPlayingField(renderTarget, renderState);
			this->renderWaitToStartInstructions(renderTarget);
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

		void StoryGameRenderer::renderPlayingField(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState) {
			sf::Vector2i fieldSize = renderState.level->getMap()->getFieldSize();
			float tileSize = RenderUtils::resolveViewportTileSize(fieldSize);
			sf::Vector2f fieldPosition = RenderUtils::resolveViewportFieldTopLeftPosition(fieldSize, tileSize);

			const sf::Texture& primaryFloorTexture = renderState.levelAssetBundle->getFloorTexture(0);

			// Draw primary grass tile (floorId = 0) under entire playing field
			sf::Sprite grassSprite;
			grassSprite.setTexture(renderState.levelAssetBundle->getFloorTexture(0));
			grassSprite.setScale(tileSize / (float)primaryFloorTexture.getSize().x, tileSize / (float)primaryFloorTexture.getSize().y);
			grassSprite.setPosition(fieldPosition.x, fieldPosition.y);
			grassSprite.setTextureRect(sf::IntRect(0, 0, fieldSize.x * primaryFloorTexture.getSize().x, fieldSize.y * primaryFloorTexture.getSize().y));
			renderTarget.draw(grassSprite);

			// Create a generic sprite for drawing a single tile at a time
			sf::Sprite tileSprite;

			for (int y = 0; y < fieldSize.y; y++) {
				for (int x = 0; x < fieldSize.x; x++) {
					int floorId = renderState.level->getMap()->getFloorId(x, y);
					int barrierId = renderState.level->getMap()->getBarrierId(x, y);

					// Draw the floor tile if it is not the primary
					if (floorId > 0) {
						const sf::Texture& floorTexture = renderState.levelAssetBundle->getFloorTexture(floorId);
						tileSprite.setTexture(floorTexture);
						tileSprite.setScale(tileSize / (float)floorTexture.getSize().x, tileSize / (float)floorTexture.getSize().y);
						tileSprite.setPosition(fieldPosition.x + (x * tileSize), fieldPosition.y + (y * tileSize));
						renderTarget.draw(tileSprite);
					}

					// Draw the barrier tile if it does not indicate no barrier exists in this location
					if (barrierId > 0) {
						const sf::Texture& barrierTexture = renderState.levelAssetBundle->getBarrierTexture(barrierId);
						tileSprite.setTexture(barrierTexture);
						tileSprite.setScale(tileSize / (float)barrierTexture.getSize().x, tileSize / (float)barrierTexture.getSize().y);
						tileSprite.setPosition(fieldPosition.x + (x * tileSize), fieldPosition.y + (y * tileSize));
						renderTarget.draw(tileSprite);
					}
				}
			}
		}

		void StoryGameRenderer::renderWaitToStartInstructions(sf::RenderTarget& renderTarget) {
			sf::RectangleShape instructionsBackgroundShape = this->createTextBackgroundShape(1260.0f, 400.0f);
			renderTarget.draw(instructionsBackgroundShape);

			sf::Text startInstructionsText = this->createInstructionsText(StoryGameRenderConstants::START_INSTRUCTIONS, (ViewUtils::VIEW_SIZE.y / 2.0f) - 50.0f);
			renderTarget.draw(startInstructionsText);

			sf::Text exitInstructionsText = this->createInstructionsText(StoryGameRenderConstants::EXIT_INSTRUCTIONS, (ViewUtils::VIEW_SIZE.y / 2.0f) + 50.0f);
			renderTarget.draw(exitInstructionsText);
		}

		sf::RectangleShape StoryGameRenderer::createTextBackgroundShape(float width, float height) {
			sf::RectangleShape result(sf::Vector2f(width, height));
			result.setPosition((ViewUtils::VIEW_SIZE.x / 2.0f) - (width / 2.0f), (ViewUtils::VIEW_SIZE.y / 2.0f) - (height / 2.0f) + 50.0f);
			result.setFillColor(sf::Color(0, 0, 0, 64));
			
			return result;
		}

		sf::Text StoryGameRenderer::createInstructionsText(const wchar_t* textString, float y) {
			sf::Text result;
			result.setFont(*this->uiFont);
			result.setCharacterSize(72);
			result.setOutlineThickness(2.0f);
			result.setString(textString);
			FontUtils::setCenteredPosition(result, y);

			return result;
		}

	}

}
