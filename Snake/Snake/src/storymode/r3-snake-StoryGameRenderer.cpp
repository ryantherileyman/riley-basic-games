
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

			const wchar_t* HUD_FOOD_APPLES = L"Apples";
			const wchar_t* HUD_FOOD_CARROTS = L"Carrots";
			const wchar_t* HUD_FOOD_GREEN_APPLES = L"Green Apples";

			const wchar_t* SCORE_FOOD_EATEN_NO_BONUS = L"+%d";
			const wchar_t* SCORE_FOOD_EATEN_WITH_BONUS = L"+%d Bonus Path!";
			const wchar_t* SCORE_FOOD_EATEN_PERFECT_PATH = L"+%d Perfect Path!";

			const wchar_t* CAMPAIGN_ERROR_MESSAGE = L"An error occurred attempting to load the campaign.";
			const wchar_t* LEVEL_ERROR_MESSAGE = L"An error occurred attempting to load the next level's assets.";
			const wchar_t* SEE_LOG_MESSAGE = L"See log.txt in the game folder for more information.";

			const wchar_t* LOAD_LEVEL_PCT_FORMAT_STRING = L"Loading assets...  (%.0f%%)";

			const wchar_t* CAMPAIGN_WON_MESSAGE = L"Congratulations!  You have won the campaign!";

			const wchar_t* START_INSTRUCTIONS = L"Press ENTER to start game";
			const wchar_t* EXIT_INSTRUCTIONS = L"Press ESC to return to the main menu";

			const sf::Color BACKGROUND_COLOR = sf::Color(0, 126, 3, 255);

			const int FOOD_PIXEL_SIZE = 75;
			const int DANGER_PIXEL_SIZE = 75;

		}

		namespace StoryGameRenderUtils {

			const wchar_t* resolveFoodTypeLabel(StoryFoodType foodType) {
				const wchar_t* result = nullptr;

				switch (foodType) {
				case StoryFoodType::APPLE:
					result = StoryGameRenderConstants::HUD_FOOD_APPLES;
					break;
				case StoryFoodType::CARROT:
					result = StoryGameRenderConstants::HUD_FOOD_CARROTS;
					break;
				case StoryFoodType::GREEN_APPLE:
					result = StoryGameRenderConstants::HUD_FOOD_GREEN_APPLES;
					break;
				}

				return result;
			}

			const wchar_t* resolveFoodEatenFormatString(const StoryFoodEatenBySnakeScoreResult& scoreResult) {
				const wchar_t* result = StoryGameRenderConstants::SCORE_FOOD_EATEN_NO_BONUS;

				if (scoreResult.perfectPathScore > 0) {
					result = StoryGameRenderConstants::SCORE_FOOD_EATEN_PERFECT_PATH;
				}
				else if (scoreResult.bonusPathScore > 0) {
					result = StoryGameRenderConstants::SCORE_FOOD_EATEN_WITH_BONUS;
				}

				return result;
			}

			void setFoodSpriteTextureRect(sf::Sprite& sprite, StoryFoodType foodType) {
				switch (foodType) {
				case StoryFoodType::APPLE:
					sprite.setTextureRect(sf::IntRect(0, 0, StoryGameRenderConstants::FOOD_PIXEL_SIZE, StoryGameRenderConstants::FOOD_PIXEL_SIZE));
					break;
				case StoryFoodType::CARROT:
					sprite.setTextureRect(sf::IntRect(75, 0, StoryGameRenderConstants::FOOD_PIXEL_SIZE, StoryGameRenderConstants::FOOD_PIXEL_SIZE));
					break;
				case StoryFoodType::GREEN_APPLE:
					sprite.setTextureRect(sf::IntRect(150, 0, StoryGameRenderConstants::FOOD_PIXEL_SIZE, StoryGameRenderConstants::FOOD_PIXEL_SIZE));
					break;
				}
			}

			void setDangerSpriteTextureRect(sf::Sprite& sprite, StoryDangerType dangerType, bool snakeOccupiesPosition) {
				switch (dangerType) {
				case StoryDangerType::SPIKE_TRAP:
					int textureRectXPos = (snakeOccupiesPosition ? 75 : 0);
					sprite.setTextureRect(sf::IntRect(textureRectXPos, 0, StoryGameRenderConstants::DANGER_PIXEL_SIZE, StoryGameRenderConstants::DANGER_PIXEL_SIZE));
					break;
				}
			}

		}

		namespace StoryCutsceneRenderUtils {

			int resolveScreenViewAlpha(const StoryCutsceneScreenView& screenView) {
				float percent = 1.0f;
				if (screenView.fadeFrames > 0) {
					percent = (float)(screenView.fadeFrames - screenView.fadeFramesRemaining) / (float)screenView.fadeFrames;
				}

				int result = (int)(255 * percent);
				return result;
			}

			sf::RectangleShape createScreenViewColorShape(const StoryCutsceneScreenView& screenView) {
				int alpha = resolveScreenViewAlpha(screenView);

				sf::Color color = screenView.color;
				color.a = alpha;

				sf::RectangleShape result = sf::RectangleShape(sf::Vector2f(ViewUtils::VIEW_SIZE.x, ViewUtils::VIEW_SIZE.y));
				result.setFillColor(color);
				return result;
			}

			sf::Sprite createScreenViewTextureSprite(const StoryCutsceneScreenView& screenView, const sf::Texture& texture) {
				int alpha = resolveScreenViewAlpha(screenView);

				sf::Sprite result;
				result.setTexture(texture);
				result.setColor(sf::Color(255, 255, 255, alpha));
				result.setScale(ViewUtils::VIEW_SIZE.x / texture.getSize().x, ViewUtils::VIEW_SIZE.y / texture.getSize().y);
				return result;
			}

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

		void StoryGameRenderer::clearAnimations() {
			this->foodEatenAnimationList.clear();
		}

		void StoryGameRenderer::queueFoodEatenAnimations(const std::vector<StoryFoodEatenResult>& foodEatenResultList, const StoryGame* storyGame) {
			for (auto const& currFoodEaten : foodEatenResultList) {
				StoryFoodEatenAnimation currAnimation;
				currAnimation.tilePosition = currFoodEaten.foodInstance.position;
				currAnimation.eatenTime = storyGame->getTimeElapsed();
				currAnimation.scoreResult = currFoodEaten.scoreResult;

				printf("Adding food eaten animation for score of %d\n", currAnimation.scoreResult.totalScore);

				this->foodEatenAnimationList.push_back(currAnimation);
			}
		}

		void StoryGameRenderer::renderLoadCampaignError(sf::RenderTarget& renderTarget) {
			renderTarget.clear(StoryGameRenderConstants::BACKGROUND_COLOR);

			sf::RectangleShape instructionsBackgroundShape = this->createTextBackgroundShape(1920.0f, 400.0f);
			renderTarget.draw(instructionsBackgroundShape);

			sf::Text errorText = this->createInstructionsText(StoryGameRenderConstants::CAMPAIGN_ERROR_MESSAGE, (ViewUtils::VIEW_SIZE.y / 2.0f) - 150.0f);
			renderTarget.draw(errorText);

			sf::Text seeLogText = this->createInstructionsText(StoryGameRenderConstants::SEE_LOG_MESSAGE, (ViewUtils::VIEW_SIZE.y / 2.0f) - 50.0f);
			renderTarget.draw(seeLogText);

			this->renderExitInstructions(renderTarget);
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

			this->renderExitInstructions(renderTarget);
		}

		void StoryGameRenderer::renderCutscene(sf::RenderTarget& renderTarget, const StoryCutsceneRenderState& renderState) {
			renderTarget.clear(StoryGameRenderConstants::BACKGROUND_COLOR);

			for (auto const& currScreenView : renderState.storyCutscene->getActiveScreenViewList()) {
				if (currScreenView.screenEventType == StoryCutsceneScreenViewType::COLOR) {
					printf("Screen view color %d,%d,%d,%d\n", currScreenView.color.r, currScreenView.color.g, currScreenView.color.b, currScreenView.color.a);

					sf::RectangleShape colorShape = StoryCutsceneRenderUtils::createScreenViewColorShape(currScreenView);
					renderTarget.draw(colorShape);
				}
				else if (currScreenView.screenEventType == StoryCutsceneScreenViewType::TEXTURE) {
					printf("Screen view texture <%s> at %d alpha\n", currScreenView.textureFilename.c_str(), StoryCutsceneRenderUtils::resolveScreenViewAlpha(currScreenView));

					const sf::Texture& texture = renderState.levelAssetBundle->getTexture(currScreenView.textureFilename);
					sf::Sprite textureSprite = StoryCutsceneRenderUtils::createScreenViewTextureSprite(currScreenView, texture);
					renderTarget.draw(textureSprite);
				}
				else if ( currScreenView.screenEventType == StoryCutsceneScreenViewType::MAP) {
					printf("Screen view map <%s> at %d alpha\n", currScreenView.mapFilename.c_str(), StoryCutsceneRenderUtils::resolveScreenViewAlpha(currScreenView));

					sf::RenderTexture mapTexture;
					mapTexture.create((unsigned int)ViewUtils::VIEW_SIZE.x, (unsigned int)ViewUtils::VIEW_SIZE.y);
					mapTexture.clear(StoryGameRenderConstants::BACKGROUND_COLOR);
					this->renderCutscenePlayingField(mapTexture, *renderState.levelAssetBundle, currScreenView);

					sf::Sprite mapSprite = StoryCutsceneRenderUtils::createScreenViewTextureSprite(currScreenView, mapTexture.getTexture());
					mapSprite.setPosition(0.0f, ViewUtils::VIEW_SIZE.y);
					mapSprite.setScale(1.0f, -1.0f);
					renderTarget.draw(mapSprite);
				}
			}

			if (!renderState.storyCutscene->getActiveScreenViewList().empty()) {
				if (renderState.storyCutscene->getSnake() != nullptr) {
					this->renderCutsceneSnake(renderTarget, renderState);
				}

				this->renderCutsceneFood(renderTarget, renderState);
				this->renderCutsceneDangers(renderTarget, renderState);
			}
		}

		void StoryGameRenderer::renderCutscenePlayingField(sf::RenderTarget& renderTarget, const StoryLevelAssetBundle& levelAssetBundle, const StoryCutsceneScreenView& screenView) {
			const StoryMapDefn& mapDefn = levelAssetBundle.getCutsceneMapDefn(screenView.mapFilename);
			StoryMap map(mapDefn);

			sf::Vector2i fieldSize = map.getFieldSize();
			float tileSize = RenderUtils::resolveViewportTileSize(fieldSize);
			sf::Vector2f fieldPosition = RenderUtils::resolveViewportFieldTopLeftPosition(fieldSize, tileSize);

			const sf::Texture& primaryFloorTexture = levelAssetBundle.getCutsceneMapFloorTexture(screenView.mapFilename, 0);

			// Draw primary grass tile (floorId = 0) under entire playing field
			sf::Sprite grassSprite;
			grassSprite.setTexture(primaryFloorTexture);
			grassSprite.setScale(tileSize / (float)primaryFloorTexture.getSize().x, tileSize / (float)primaryFloorTexture.getSize().y);
			grassSprite.setPosition(fieldPosition.x, fieldPosition.y);
			grassSprite.setTextureRect(sf::IntRect(0, 0, fieldSize.x * primaryFloorTexture.getSize().x, fieldSize.y * primaryFloorTexture.getSize().y));
			renderTarget.draw(grassSprite);

			// Create a generic sprite for drawing a single tile at a time
			sf::Sprite tileSprite;

			for (int y = 0; y < fieldSize.y; y++) {
				for (int x = 0; x < fieldSize.x; x++) {
					int floorId = map.getFloorId(x, y);
					int barrierId = map.getBarrierId(x, y);

					// Draw the floor tile if it is not the primary
					if (floorId > 0) {
						const sf::Texture& floorTexture = levelAssetBundle.getCutsceneMapFloorTexture(screenView.mapFilename, floorId);
						tileSprite.setTexture(floorTexture);
						tileSprite.setScale(tileSize / (float)floorTexture.getSize().x, tileSize / (float)floorTexture.getSize().y);
						tileSprite.setPosition(fieldPosition.x + (x * tileSize), fieldPosition.y + (y * tileSize));
						renderTarget.draw(tileSprite);
					}

					// Draw the barrier tile if it does not indicate no barrier exists in this location
					if (barrierId > 0) {
						const sf::Texture& barrierTexture = levelAssetBundle.getCutsceneMapBarrierTexture(screenView.mapFilename, barrierId);
						tileSprite.setTexture(barrierTexture);
						tileSprite.setScale(tileSize / (float)barrierTexture.getSize().x, tileSize / (float)barrierTexture.getSize().y);
						tileSprite.setPosition(fieldPosition.x + (x * tileSize), fieldPosition.y + (y * tileSize));
						renderTarget.draw(tileSprite);
					}
				}
			}
		}

		void StoryGameRenderer::renderCutsceneSnake(sf::RenderTarget& renderTarget, const StoryCutsceneRenderState& renderState) {
			const StoryCutsceneScreenView& lastScreenView = renderState.storyCutscene->getActiveScreenViewList().back();
			if (lastScreenView.screenEventType == StoryCutsceneScreenViewType::MAP) {
				const StoryMapDefn& mapDefn = renderState.levelAssetBundle->getCutsceneMapDefn(lastScreenView.mapFilename);

				RenderUtils::RenderSnakeInput renderSnakeInput;
				renderSnakeInput.fieldSize = mapDefn.fieldSize;
				renderSnakeInput.texture = &renderState.levelAssetBundle->getSnakeTexture();
				renderSnakeInput.snake = renderState.storyCutscene->getSnake();
				renderSnakeInput.snakeHurtFlag = false;

				RenderUtils::renderSnake(renderTarget, renderSnakeInput);
			}
		}

		void StoryGameRenderer::renderCutsceneFood(sf::RenderTarget& renderTarget, const StoryCutsceneRenderState& renderState) {
			const StoryCutsceneScreenView& lastScreenView = renderState.storyCutscene->getActiveScreenViewList().back();
			if (lastScreenView.screenEventType == StoryCutsceneScreenViewType::MAP) {
				const StoryMapDefn& mapDefn = renderState.levelAssetBundle->getCutsceneMapDefn(lastScreenView.mapFilename);

				sf::Vector2i fieldSize = mapDefn.fieldSize;
				float tileSize = RenderUtils::resolveViewportTileSize(fieldSize);
				sf::Vector2f fieldPosition = RenderUtils::resolveViewportFieldTopLeftPosition(fieldSize, tileSize);

				sf::Sprite foodSprite;
				foodSprite.setTexture(renderState.levelAssetBundle->getFoodTexture());
				foodSprite.setScale(tileSize / (float)StoryGameRenderConstants::FOOD_PIXEL_SIZE, tileSize / (float)StoryGameRenderConstants::FOOD_PIXEL_SIZE);

				for (auto const& currFoodInstancePair : renderState.storyCutscene->getFoodInstanceMap()) {
					StoryGameRenderUtils::setFoodSpriteTextureRect(foodSprite, currFoodInstancePair.second.foodType);
					foodSprite.setPosition(fieldPosition.x + currFoodInstancePair.second.position.x * tileSize, fieldPosition.y + currFoodInstancePair.second.position.y * tileSize);

					renderTarget.draw(foodSprite);
				}
			}
		}

		void StoryGameRenderer::renderCutsceneDangers(sf::RenderTarget& renderTarget, const StoryCutsceneRenderState& renderState) {
			const StoryCutsceneScreenView& lastScreenView = renderState.storyCutscene->getActiveScreenViewList().back();
			if (lastScreenView.screenEventType == StoryCutsceneScreenViewType::MAP) {
				const StoryMapDefn& mapDefn = renderState.levelAssetBundle->getCutsceneMapDefn(lastScreenView.mapFilename);

				sf::Vector2i fieldSize = mapDefn.fieldSize;
				float tileSize = RenderUtils::resolveViewportTileSize(fieldSize);
				sf::Vector2f fieldPosition = RenderUtils::resolveViewportFieldTopLeftPosition(fieldSize, tileSize);

				sf::Sprite dangerSprite;
				dangerSprite.setTexture(renderState.levelAssetBundle->getDangerTexture());
				dangerSprite.setScale(tileSize / (float)StoryGameRenderConstants::DANGER_PIXEL_SIZE, tileSize / (float)StoryGameRenderConstants::DANGER_PIXEL_SIZE);

				for (auto const& currDangerInstancePair : renderState.storyCutscene->getDangerInstanceMap()) {
					bool snakeOccupiesPosition = false;
					if (renderState.storyCutscene->getSnake() != nullptr) {
						snakeOccupiesPosition = renderState.storyCutscene->getSnake()->occupiesPosition(currDangerInstancePair.second.position);
					}
					StoryGameRenderUtils::setDangerSpriteTextureRect(dangerSprite, currDangerInstancePair.second.dangerType, snakeOccupiesPosition);

					dangerSprite.setPosition(fieldPosition.x + currDangerInstancePair.second.position.x * tileSize, fieldPosition.y + currDangerInstancePair.second.position.y * tileSize);

					renderTarget.draw(dangerSprite);
				}
			}
		}

		void StoryGameRenderer::renderWaitToStart(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState) {
			renderTarget.clear(StoryGameRenderConstants::BACKGROUND_COLOR);
			this->renderGameRunningUi(renderTarget, renderState);
			this->renderPlayingField(renderTarget, renderState);
			this->renderSnake(renderTarget, renderState);
			this->renderWaitToStartInstructions(renderTarget);
		}

		void StoryGameRenderer::renderGameRunning(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState) {
			renderTarget.clear(StoryGameRenderConstants::BACKGROUND_COLOR);
			this->renderGameRunningUi(renderTarget, renderState);
			this->renderPlayingField(renderTarget, renderState);
			this->renderSnake(renderTarget, renderState);
			this->renderFoodSpawns(renderTarget, renderState);
			this->renderDangerSpawns(renderTarget, renderState);
			this->renderFoodEatenAnimations(renderTarget, renderState);
		}

		void StoryGameRenderer::renderLevelLost(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState) {
			renderTarget.clear(StoryGameRenderConstants::BACKGROUND_COLOR);
			this->renderGameRunningUi(renderTarget, renderState);
			this->renderPlayingField(renderTarget, renderState);
		}

		void StoryGameRenderer::renderCampaignWon(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState) {
			renderTarget.clear(StoryGameRenderConstants::BACKGROUND_COLOR);
			this->renderGameRunningUi(renderTarget, renderState);
			this->renderPlayingField(renderTarget, renderState);

			sf::Text wonText = this->createInstructionsText(StoryGameRenderConstants::CAMPAIGN_WON_MESSAGE, (ViewUtils::VIEW_SIZE.y / 2.0f) - 150.0f);
			renderTarget.draw(wonText);

			this->renderExitInstructions(renderTarget);
		}

		void StoryGameRenderer::renderGameRunningUi(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState) {
			wchar_t scoreStr[32];
			swprintf_s(scoreStr, StoryGameRenderConstants::HUD_SCORE_FORMAT_STRING, renderState.storyGame->getScore());

			sf::Text scoreText;
			scoreText.setFont(*this->uiFont);
			scoreText.setCharacterSize(48);
			scoreText.setString(scoreStr);
			scoreText.setPosition(24.0f, 24.0f);
			renderTarget.draw(scoreText);

			wchar_t winConditionStr[48];
			switch (renderState.storyGame->getWinCondition().conditionType) {
			case StoryWinConditionType::ON_FOOD_EATEN:
			{
				const wchar_t* foodTypeLabel = StoryGameRenderUtils::resolveFoodTypeLabel(renderState.storyGame->getWinCondition().foodType);
				int foodWinCount = renderState.storyGame->getWinCondition().foodCount;
				int foodEatenCount = renderState.storyGame->getFoodEaten(renderState.storyGame->getWinCondition().foodType);
				swprintf_s(winConditionStr, StoryGameRenderConstants::HUD_FOOD_LEFT, foodTypeLabel, foodWinCount - foodEatenCount);
			}
				break;
			case StoryWinConditionType::ON_LENGTH_REACHED:
			{
				int currSnakeLength = renderState.storyGame->getSnake()->getLength();
				int lengthToWin = renderState.storyGame->getWinCondition().snakeLength;
				swprintf_s(winConditionStr, StoryGameRenderConstants::HUD_SNAKE_LENGTH, currSnakeLength, lengthToWin);
			}
				break;
			case StoryWinConditionType::ON_TIME_SURVIVED:
			{
				int secondsPassed = (int)renderState.storyGame->getTimeElapsed().asSeconds();
				int secondsToWin = renderState.storyGame->getWinCondition().timePassed;

				int minutesRemaining = (secondsToWin - secondsPassed) / 60;
				int secondsRemaining = secondsToWin - secondsPassed - (minutesRemaining * 60);
				swprintf_s(winConditionStr, StoryGameRenderConstants::HUD_TIME_REMAINING, minutesRemaining, secondsRemaining);
			}
				break;
			}

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

			float healthPct = renderState.storyGame->getCurrSnakeHealth() / renderState.storyGame->getMaxSnakeHealth();
			sf::RectangleShape healthBarForegroundShape(sf::Vector2f(596.0f * healthPct, 17.0f));
			healthBarForegroundShape.setPosition(662.0f, 82.0f);
			healthBarForegroundShape.setFillColor(sf::Color::Red);
			renderTarget.draw(healthBarForegroundShape);

			sf::Sprite healthBarSprite;
			healthBarSprite.setTexture(*this->healthBarTexture);
			healthBarSprite.setPosition(620.0f, 70.0f);
			renderTarget.draw(healthBarSprite);
		}

		void StoryGameRenderer::renderPlayingField(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState) {
			sf::Vector2i fieldSize = renderState.storyGame->getMap()->getFieldSize();
			float tileSize = RenderUtils::resolveViewportTileSize(fieldSize);
			sf::Vector2f fieldPosition = RenderUtils::resolveViewportFieldTopLeftPosition(fieldSize, tileSize);

			const sf::Texture& primaryFloorTexture = renderState.levelAssetBundle->getFloorTexture(0);

			// Draw primary grass tile (floorId = 0) under entire playing field
			sf::Sprite grassSprite;
			grassSprite.setTexture(primaryFloorTexture);
			grassSprite.setScale(tileSize / (float)primaryFloorTexture.getSize().x, tileSize / (float)primaryFloorTexture.getSize().y);
			grassSprite.setPosition(fieldPosition.x, fieldPosition.y);
			grassSprite.setTextureRect(sf::IntRect(0, 0, fieldSize.x * primaryFloorTexture.getSize().x, fieldSize.y * primaryFloorTexture.getSize().y));
			renderTarget.draw(grassSprite);

			// Create a generic sprite for drawing a single tile at a time
			sf::Sprite tileSprite;

			for (int y = 0; y < fieldSize.y; y++) {
				for (int x = 0; x < fieldSize.x; x++) {
					int floorId = renderState.storyGame->getMap()->getFloorId(x, y);
					int barrierId = renderState.storyGame->getMap()->getBarrierId(x, y);

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

		void StoryGameRenderer::renderSnake(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState) {
			RenderUtils::RenderSnakeInput renderSnakeInput;
			renderSnakeInput.fieldSize = renderState.storyGame->getMap()->getFieldSize();
			renderSnakeInput.texture = &renderState.levelAssetBundle->getSnakeTexture();
			renderSnakeInput.snake = renderState.storyGame->getSnake();
			renderSnakeInput.snakeHurtFlag = renderState.snakeDamagedFlag;

			RenderUtils::renderSnake(renderTarget, renderSnakeInput);
		}

		void StoryGameRenderer::renderFoodSpawns(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState) {
			sf::Vector2i fieldSize = renderState.storyGame->getMap()->getFieldSize();
			float tileSize = RenderUtils::resolveViewportTileSize(fieldSize);
			sf::Vector2f fieldPosition = RenderUtils::resolveViewportFieldTopLeftPosition(fieldSize, tileSize);

			sf::Sprite foodSprite;
			foodSprite.setTexture(renderState.levelAssetBundle->getFoodTexture());
			foodSprite.setScale(tileSize / (float)StoryGameRenderConstants::FOOD_PIXEL_SIZE, tileSize / (float)StoryGameRenderConstants::FOOD_PIXEL_SIZE);

			for (auto const& currFoodSpawnTracker : renderState.storyGame->getFoodSpawnTrackerList()) {
				const StoryFoodDefn& foodDefn = currFoodSpawnTracker.getFoodDefn();

				StoryGameRenderUtils::setFoodSpriteTextureRect(foodSprite, foodDefn.foodType);

				for (auto const& currFoodInstance : currFoodSpawnTracker.getFoodInstanceList()) {
					foodSprite.setPosition(fieldPosition.x + currFoodInstance.position.x * tileSize, fieldPosition.y + currFoodInstance.position.y * tileSize);

					renderTarget.draw(foodSprite);
				}
			}
		}

		void StoryGameRenderer::renderDangerSpawns(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState) {
			sf::Vector2i fieldSize = renderState.storyGame->getMap()->getFieldSize();
			float tileSize = RenderUtils::resolveViewportTileSize(fieldSize);
			sf::Vector2f fieldPosition = RenderUtils::resolveViewportFieldTopLeftPosition(fieldSize, tileSize);

			sf::Sprite dangerSprite;
			dangerSprite.setTexture(renderState.levelAssetBundle->getDangerTexture());
			dangerSprite.setScale(tileSize / (float)StoryGameRenderConstants::DANGER_PIXEL_SIZE, tileSize / (float)StoryGameRenderConstants::DANGER_PIXEL_SIZE);

			for (auto const& currDangerSpawnTracker : renderState.storyGame->getDangerSpawnTrackerList()) {
				const StoryDangerDefn& dangerDefn = currDangerSpawnTracker.getDangerDefn();

				for (auto const& currDangerInstance : currDangerSpawnTracker.getDangerInstanceList()) {
					bool snakeOccupiesPosition = renderState.storyGame->getSnake()->occupiesPosition(currDangerInstance.position);
					int textureRectXPos = (snakeOccupiesPosition ? 75 : 0);

					dangerSprite.setTextureRect(sf::IntRect(textureRectXPos, 0, StoryGameRenderConstants::DANGER_PIXEL_SIZE, StoryGameRenderConstants::DANGER_PIXEL_SIZE));
					dangerSprite.setPosition(fieldPosition.x + currDangerInstance.position.x * tileSize, fieldPosition.y + currDangerInstance.position.y * tileSize);

					renderTarget.draw(dangerSprite);
				}
			}
		}

		void StoryGameRenderer::renderFoodEatenAnimations(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState) {
			sf::Vector2i fieldSize = renderState.storyGame->getMap()->getFieldSize();
			float tileSize = RenderUtils::resolveViewportTileSize(fieldSize);
			sf::Vector2f fieldPosition = RenderUtils::resolveViewportFieldTopLeftPosition(fieldSize, tileSize);

			wchar_t foodEatenStr[32];

			sf::Text foodEatenText;
			foodEatenText.setFont(*this->uiFont);
			foodEatenText.setCharacterSize(32);

			for (int animationIndex = this->foodEatenAnimationList.size() - 1; animationIndex >= 0; animationIndex--) {
				StoryFoodEatenAnimation currAnimation = this->foodEatenAnimationList[animationIndex];

				float secondsSinceFoodEaten = renderState.storyGame->getTimeElapsed().asSeconds() - currAnimation.eatenTime.asSeconds();
				if (secondsSinceFoodEaten >= 1.0f) {
					this->foodEatenAnimationList.erase(this->foodEatenAnimationList.begin() + animationIndex);

					printf("Removing food eaten animation at index %d, size is now %d\n", animationIndex, this->foodEatenAnimationList.size());
				}
				else {
					swprintf_s(foodEatenStr, StoryGameRenderUtils::resolveFoodEatenFormatString(currAnimation.scoreResult), currAnimation.scoreResult.totalScore);
					foodEatenText.setString(foodEatenStr);

					float basePosX = fieldPosition.x + currAnimation.tilePosition.x * tileSize + tileSize / 2.0f;
					float animatedPosX = basePosX - FontUtils::resolveTextWidth(foodEatenText) / 2.0f;

					float basePosY = fieldPosition.y + currAnimation.tilePosition.y * tileSize;
					float animatedPosY = basePosY - secondsSinceFoodEaten * 30.0f;
					foodEatenText.setPosition(animatedPosX, animatedPosY);

					sf::Uint8 alpha = 255 - (sf::Uint8)(secondsSinceFoodEaten * 255);
					foodEatenText.setFillColor(sf::Color(255, 255, 255, alpha));

					renderTarget.draw(foodEatenText);
				}
			}
		}

		void StoryGameRenderer::renderWaitToStartInstructions(sf::RenderTarget& renderTarget) {
			sf::RectangleShape instructionsBackgroundShape = this->createTextBackgroundShape(1260.0f, 400.0f);
			renderTarget.draw(instructionsBackgroundShape);

			sf::Text startInstructionsText = this->createInstructionsText(StoryGameRenderConstants::START_INSTRUCTIONS, (ViewUtils::VIEW_SIZE.y / 2.0f) - 50.0f);
			renderTarget.draw(startInstructionsText);

			this->renderExitInstructions(renderTarget);
		}

		void StoryGameRenderer::renderExitInstructions(sf::RenderTarget& renderTarget) {
			sf::Text exitInstructionsText = this->createInstructionsText(StoryGameRenderConstants::EXIT_INSTRUCTIONS, (ViewUtils::VIEW_SIZE.y / 2.0f) + 50.0f);
			renderTarget.draw(exitInstructionsText);
		}

		sf::RectangleShape StoryGameRenderer::createTextBackgroundShape(float width, float height) {
			sf::RectangleShape result(sf::Vector2f(width, height));
			result.setPosition((ViewUtils::VIEW_SIZE.x / 2.0f) - (width / 2.0f), (ViewUtils::VIEW_SIZE.y / 2.0f) - (height / 2.0f) + 50.0f);
			result.setFillColor(sf::Color(0, 0, 0, 96));
			
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
