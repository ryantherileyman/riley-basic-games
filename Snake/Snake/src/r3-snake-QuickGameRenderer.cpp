
#include <assert.h>
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-quickgamescene.hpp"

namespace r3 {

	namespace snake {

		const char* QUICK_GAME_UI_FONT_PATH = "resources/fonts/SourceSansPro-Regular.otf";
		const char* SNAKE_TILESET_TEXTURE_PATH = "resources/textures/snake-tileset.png";
		const char* GRASS_TEXTURE_PATH = "resources/textures/grass.png";
		const char* SHRUB_TEXTURE_PATH = "resources/textures/shrub.png";

		const int SNAKE_TILE_PIXEL_SIZE = 75;
		const int GRASS_PIXEL_SIZE = 256;
		const int SHRUB_PIXEL_SIZE = 256;
		const sf::Vector2f FIELD_CENTER_VIEWPORT_POSITION(960.0f, 576.0f);
		const float FIELD_HIGHEST_TOP_VIEWPORT_POSITION = 108.0f;
		const float FIELD_MINIMUM_VIEWPORT_MARGIN = 20.0f;

		const sf::Color QUICK_GAME_BACKGROUND_COLOR = sf::Color(0, 126, 3, 255);

		const wchar_t* SNAKE_LENGTH_FORMAT_STRING = L"Snake Length: %d";
		const wchar_t* LONGEST_SNAKE_FORMAT_STRING = L"Longest Snake: %d";

		const wchar_t* START_INSTRUCTIONS = L"Press ENTER to start game";
		const wchar_t* EXIT_INSTRUCTIONS = L"Press ESC to return to the main menu";
		const wchar_t* LAST_GAME_WON_STRING = L"Congratulations!  You got the longest snake!";

		namespace QuickGameRendererUtils {

			float resolveViewportTileSize(const QuickGame& game) {
				float maxFieldWidth = ViewUtils::VIEW_SIZE.x - (FIELD_MINIMUM_VIEWPORT_MARGIN * 2.0f);
				float maxFieldHeight = ViewUtils::VIEW_SIZE.y - FIELD_HIGHEST_TOP_VIEWPORT_POSITION - FIELD_MINIMUM_VIEWPORT_MARGIN;

				float unalignedTileSize = fminf(maxFieldWidth / game.getFieldSize().x, maxFieldHeight / game.getFieldSize().y);

				float result = (float)SNAKE_TILE_PIXEL_SIZE;
				if (unalignedTileSize < (float)SNAKE_TILE_PIXEL_SIZE) {
					result = (float)SNAKE_TILE_PIXEL_SIZE / ceilf((float)SNAKE_TILE_PIXEL_SIZE / unalignedTileSize);
				} else {
					result = (float)SNAKE_TILE_PIXEL_SIZE * floorf(unalignedTileSize / (float)SNAKE_TILE_PIXEL_SIZE);
				}

				return result;
			}

			sf::Vector2f resolveViewportFieldTopLeftPosition(const QuickGame& game, float viewportTileSize) {
				sf::Vector2f result;
				result.x = FIELD_CENTER_VIEWPORT_POSITION.x - (viewportTileSize * game.getFieldSize().x * 0.5f);
				result.y = FIELD_CENTER_VIEWPORT_POSITION.y - (viewportTileSize * game.getFieldSize().y * 0.5f);
				return result;
			}

			sf::Sprite createSpriteWithTexture(const sf::Texture& sourceTexture) {
				sf::Sprite result;
				result.setTexture(sourceTexture, true);
				return result;
			}

			void initSprite(sf::Sprite& sprite, const sf::Texture& sourceTexture, int pixelLeft, int pixelTop) {
				sprite.setTexture(sourceTexture);
				sprite.setTextureRect(sf::IntRect(pixelLeft, pixelTop, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
			}

			void initSnakeHeadSprite(sf::Sprite& sprite, const sf::Texture& sourceTexture, ObjectDirection direction) {
				sprite.setTexture(sourceTexture);
				switch (direction) {
				case ObjectDirection::UP:
					sprite.setTextureRect(sf::IntRect(0, 0, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
					break;
				case ObjectDirection::RIGHT:
					sprite.setTextureRect(sf::IntRect(375, 0, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
					break;
				case ObjectDirection::DOWN:
					sprite.setTextureRect(sf::IntRect(75, 300, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
					break;
				case ObjectDirection::LEFT:
					sprite.setTextureRect(sf::IntRect(75, 75, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
					break;
				}
			}

			void initSnakeTailSprite(sf::Sprite& sprite, const sf::Texture& sourceTexture, ObjectDirection direction) {
				sprite.setTexture(sourceTexture);
				switch (direction) {
				case ObjectDirection::UP:
					sprite.setTextureRect(sf::IntRect(0, 150, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
					break;
				case ObjectDirection::RIGHT:
					sprite.setTextureRect(sf::IntRect(225, 0, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
					break;
				case ObjectDirection::DOWN:
					sprite.setTextureRect(sf::IntRect(75, 150, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
					break;
				case ObjectDirection::LEFT:
					sprite.setTextureRect(sf::IntRect(225, 75, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
					break;
				}
			}

			void initSnakeBodySprite(sf::Sprite& sprite, const sf::Texture& sourceTexture, ObjectDirection enterDirection, ObjectDirection exitDirection) {
				sprite.setTexture(sourceTexture);
				switch (enterDirection) {
				case ObjectDirection::UP:
					assert(exitDirection != ObjectDirection::DOWN);
					switch (exitDirection) {
					case ObjectDirection::UP:
						sprite.setTextureRect(sf::IntRect(0, 75, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::RIGHT:
						sprite.setTextureRect(sf::IntRect(0, 300, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::LEFT:
						sprite.setTextureRect(sf::IntRect(0, 225, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
						break;
					}
					break;
				case ObjectDirection::RIGHT:
					assert(exitDirection != ObjectDirection::LEFT);
					switch (exitDirection) {
					case ObjectDirection::UP:
						sprite.setTextureRect(sf::IntRect(150, 0, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::RIGHT:
						sprite.setTextureRect(sf::IntRect(300, 0, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::DOWN:
						sprite.setTextureRect(sf::IntRect(75, 0, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
						break;
					}
					break;
				case ObjectDirection::DOWN:
					assert(exitDirection != ObjectDirection::UP);
					switch (exitDirection) {
					case ObjectDirection::RIGHT:
						sprite.setTextureRect(sf::IntRect(225, 150, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::DOWN:
						sprite.setTextureRect(sf::IntRect(75, 225, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::LEFT:
						sprite.setTextureRect(sf::IntRect(150, 150, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
						break;
					}
					break;
				case ObjectDirection::LEFT:
					assert(exitDirection != ObjectDirection::RIGHT);
					switch (exitDirection) {
					case ObjectDirection::UP:
						sprite.setTextureRect(sf::IntRect(300, 75, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::DOWN:
						sprite.setTextureRect(sf::IntRect(375, 75, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::LEFT:
						sprite.setTextureRect(sf::IntRect(150, 75, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
						break;
					}
					break;
				}
			}

		}

		QuickGameRenderer::QuickGameRenderer() {
			this->uiFont = nullptr;
			this->snakeTilesetTexture = nullptr;

			this->uiFont = new sf::Font();
			if (!this->uiFont->loadFromFile(QUICK_GAME_UI_FONT_PATH) ) {
				throw "Could not load user interface font";
			}

			this->snakeTilesetTexture = new sf::Texture();
			if (!this->snakeTilesetTexture->loadFromFile(SNAKE_TILESET_TEXTURE_PATH)) {
				throw "Could not load game texture";
			}

			this->grassTexture = new sf::Texture();
			if (!this->grassTexture->loadFromFile(GRASS_TEXTURE_PATH)) {
				throw "Could not load grass texture";
			}
			this->grassTexture->setRepeated(true);

			this->shrubTexture = new sf::Texture();
			if (!this->shrubTexture->loadFromFile(SHRUB_TEXTURE_PATH)) {
				throw "Could not load shrub texture";
			}
			this->shrubTexture->setRepeated(true);

			this->snakeLengthText.setFont(*this->uiFont);
			this->snakeLengthText.setCharacterSize(48);
			this->snakeLengthText.setOutlineColor(sf::Color::White);
			this->snakeLengthText.setPosition(24.0f, 24.0f);

			this->longestSnakeText.setFont(*this->uiFont);
			this->longestSnakeText.setCharacterSize(48);
			this->longestSnakeText.setOutlineColor(sf::Color::White);
			this->longestSnakeText.setPosition(0.0f, 24.0f);

			this->startInstructionsText.setFont(*this->uiFont);
			this->startInstructionsText.setCharacterSize(64);
			this->startInstructionsText.setOutlineColor(sf::Color::White);
			this->startInstructionsText.setString(START_INSTRUCTIONS);
			float startInstructionsWidth = FontUtils::resolveTextWidth(startInstructionsText);
			this->startInstructionsText.setPosition((ViewUtils::VIEW_SIZE.x / 2.0f) - (startInstructionsWidth / 2.0f), (ViewUtils::VIEW_SIZE.y / 2.0f) - 35.0f);

			this->exitInstructionsText.setFont(*this->uiFont);
			this->exitInstructionsText.setCharacterSize(64);
			this->exitInstructionsText.setOutlineColor(sf::Color::White);
			this->exitInstructionsText.setString(EXIT_INSTRUCTIONS);
			float exitInstructionsWidth = FontUtils::resolveTextWidth(exitInstructionsText);
			this->exitInstructionsText.setPosition((ViewUtils::VIEW_SIZE.x / 2.0f) - (exitInstructionsWidth / 2.0f), (ViewUtils::VIEW_SIZE.y / 2.0f) + 35.0f);

			this->grassSprite = QuickGameRendererUtils::createSpriteWithTexture(*this->grassTexture);
			this->shrubSprite = QuickGameRendererUtils::createSpriteWithTexture(*this->shrubTexture);
			QuickGameRendererUtils::initSprite(this->appleSprite, *this->snakeTilesetTexture, 150, 375);
		}

		QuickGameRenderer::~QuickGameRenderer() {
			if (this->uiFont != nullptr) {
				delete this->uiFont;
			}
			if (this->snakeTilesetTexture != nullptr) {
				delete this->snakeTilesetTexture;
			}
		}

		void QuickGameRenderer::renderWaitToStart(sf::RenderTarget& renderTarget, const QuickGameRenderState& gameRenderState) {
			renderTarget.clear(QUICK_GAME_BACKGROUND_COLOR);
			this->renderPlayingField(renderTarget, *gameRenderState.game);
			this->renderScoreUi(renderTarget, gameRenderState);
			renderTarget.draw(startInstructionsText);
			renderTarget.draw(exitInstructionsText);
		}

		void QuickGameRenderer::renderGameRunning(sf::RenderTarget& renderTarget, const QuickGameRenderState& gameRenderState) {
			renderTarget.clear(QUICK_GAME_BACKGROUND_COLOR);
			this->renderPlayingField(renderTarget, *gameRenderState.game);
			this->renderApple(renderTarget, *gameRenderState.game);
			this->renderSnake(renderTarget, *gameRenderState.game);
			this->renderScoreUi(renderTarget, gameRenderState);
		}

		void QuickGameRenderer::renderGameDoneSummary(sf::RenderTarget& renderTarget, const QuickGameRenderState& gameRenderState) {
			renderTarget.clear(QUICK_GAME_BACKGROUND_COLOR);
			this->renderPlayingField(renderTarget, *gameRenderState.game);
			this->renderSnake(renderTarget, *gameRenderState.game);
			this->renderScoreUi(renderTarget, gameRenderState);
			if (gameRenderState.lastGameBeatLongestSnakeLength) {
				this->renderLongestSnakeUi(renderTarget);
			}
			renderTarget.draw(startInstructionsText);
			renderTarget.draw(exitInstructionsText);
		}

		void QuickGameRenderer::renderPlayingField(sf::RenderTarget& renderTarget, const QuickGame& game) {
			float tileSize = QuickGameRendererUtils::resolveViewportTileSize(game);
			sf::Vector2f fieldPosition = QuickGameRendererUtils::resolveViewportFieldTopLeftPosition(game, tileSize);

			// Draw grass tiles under entire playing field
			this->grassSprite.setScale(tileSize / (float)GRASS_PIXEL_SIZE, tileSize / (float)GRASS_PIXEL_SIZE);
			this->grassSprite.setPosition(fieldPosition.x, fieldPosition.y);
			this->grassSprite.setTextureRect(sf::IntRect(0, 0, game.getFieldSize().x * GRASS_PIXEL_SIZE, game.getFieldSize().y * GRASS_PIXEL_SIZE));
			renderTarget.draw(this->grassSprite);

			// Set scale for shrub sprite
			this->shrubSprite.setScale(tileSize / (float)SHRUB_PIXEL_SIZE, tileSize / (float)SHRUB_PIXEL_SIZE);

			// Draw top row of shrub barriers
			this->shrubSprite.setPosition(fieldPosition.x, fieldPosition.y);
			this->shrubSprite.setTextureRect(sf::IntRect(0, 0, game.getFieldSize().x * SHRUB_PIXEL_SIZE, SHRUB_PIXEL_SIZE));
			renderTarget.draw(this->shrubSprite);

			// Draw bottom row of shrub barriers
			this->shrubSprite.setPosition(fieldPosition.x, fieldPosition.y + (game.getFieldSize().y - 1) * tileSize);
			this->shrubSprite.setTextureRect(sf::IntRect(0, 0, game.getFieldSize().x * SHRUB_PIXEL_SIZE, SHRUB_PIXEL_SIZE));
			renderTarget.draw(this->shrubSprite);

			// Draw left column of shrub barriers
			this->shrubSprite.setPosition(fieldPosition.x, fieldPosition.y + tileSize);
			this->shrubSprite.setTextureRect(sf::IntRect(0, 0, SHRUB_PIXEL_SIZE, (game.getFieldSize().y - 2) * SHRUB_PIXEL_SIZE));
			renderTarget.draw(this->shrubSprite);

			// Draw right column of shrub barriers
			this->shrubSprite.setPosition(fieldPosition.x + (game.getFieldSize().x - 1) * tileSize, fieldPosition.y + tileSize);
			this->shrubSprite.setTextureRect(sf::IntRect(0, 0, SHRUB_PIXEL_SIZE, (game.getFieldSize().y - 2) * SHRUB_PIXEL_SIZE));
			renderTarget.draw(this->shrubSprite);
		}

		void QuickGameRenderer::renderApple(sf::RenderTarget& renderTarget, const QuickGame& game) {
			float tileSize = QuickGameRendererUtils::resolveViewportTileSize(game);
			sf::Vector2f fieldPosition = QuickGameRendererUtils::resolveViewportFieldTopLeftPosition(game, tileSize);
			float spriteScale = tileSize / (float)SNAKE_TILE_PIXEL_SIZE;

			if (game.getAppleExists()) {
				sf::Vector2i applePosition = game.getApplePosition();
				this->appleSprite.setScale(spriteScale, spriteScale);
				this->appleSprite.setPosition(applePosition.x * tileSize + fieldPosition.x, applePosition.y * tileSize + fieldPosition.y);
				renderTarget.draw(this->appleSprite);
			}
		}

		void QuickGameRenderer::renderSnake(sf::RenderTarget& renderTarget, const QuickGame& game) {
			float tileSize = QuickGameRendererUtils::resolveViewportTileSize(game);
			float spriteScale = tileSize / (float)SNAKE_TILE_PIXEL_SIZE;

			sf::Sprite tailSprite = this->createSnakeTailSprite(game);
			tailSprite.setScale(spriteScale, spriteScale);
			renderTarget.draw(tailSprite);

			int bodySegmentCount = game.getSnake()->getBodyLength();
			for (int segmentIndex = bodySegmentCount - 1; segmentIndex >= 0; segmentIndex--) {
				sf::Sprite currBodySprite = this->createSnakeBodySprite(game, game.getSnake()->getBody(segmentIndex));
				currBodySprite.setScale(spriteScale, spriteScale);
				renderTarget.draw(currBodySprite);
			}

			sf::Sprite headSprite = this->createSnakeHeadSprite(game);
			headSprite.setScale(spriteScale, spriteScale);
			renderTarget.draw(headSprite);
		}

		void QuickGameRenderer::renderScoreUi(sf::RenderTarget& renderTarget, const QuickGameRenderState& gameRenderState) {
			// Draw current snake length
			int currSnakeLength = 0;
			if (gameRenderState.game != nullptr) {
				currSnakeLength = gameRenderState.game->getSnake()->getLength();
			}

			wchar_t snakeLengthString[32];
			swprintf_s(snakeLengthString, SNAKE_LENGTH_FORMAT_STRING, currSnakeLength);
			snakeLengthText.setString(snakeLengthString);
			renderTarget.draw(snakeLengthText);

			// Draw longest snake length
			wchar_t longestSnakeString[32];
			swprintf_s(longestSnakeString, LONGEST_SNAKE_FORMAT_STRING, gameRenderState.longestSnake);
			longestSnakeText.setString(longestSnakeString);
			float longestSnakeTextWidth = FontUtils::resolveTextWidth(longestSnakeText);
			longestSnakeText.setPosition(ViewUtils::VIEW_SIZE.x - longestSnakeTextWidth - 24.0f, 24.0f);
			renderTarget.draw(longestSnakeText);
		}

		void QuickGameRenderer::renderLongestSnakeUi(sf::RenderTarget& renderTarget) {
			sf::Text gameWonText;
			gameWonText.setFont(*this->uiFont);
			gameWonText.setCharacterSize(64);
			gameWonText.setOutlineColor(sf::Color::White);
			gameWonText.setString(LAST_GAME_WON_STRING);
			float gameWonWidth = FontUtils::resolveTextWidth(gameWonText);
			float gameWonLeftPos = (ViewUtils::VIEW_SIZE.x / 2.0f) - (gameWonWidth / 2.0f);
			float gameWonTopPos = 80.0f;
			gameWonText.setPosition(gameWonLeftPos, gameWonTopPos);
			renderTarget.draw(gameWonText);

			this->appleSprite.setScale(1.2f, 1.2f);

			this->appleSprite.setPosition(gameWonLeftPos - 100.0f, gameWonTopPos);
			renderTarget.draw(this->appleSprite);

			this->appleSprite.setPosition(gameWonLeftPos + gameWonWidth + 15.0f, gameWonTopPos);
			renderTarget.draw(this->appleSprite);
		}

		sf::Sprite QuickGameRenderer::createSnakeHeadSprite(const QuickGame& game) {
			SnakeSegment head = game.getSnake()->getHead();

			float tileSize = QuickGameRendererUtils::resolveViewportTileSize(game);
			sf::Vector2f fieldPosition = QuickGameRendererUtils::resolveViewportFieldTopLeftPosition(game, tileSize);

			sf::Sprite result;
			QuickGameRendererUtils::initSnakeHeadSprite(result, *this->snakeTilesetTexture, head.enterDirection);
			result.setPosition(head.position.x * tileSize + fieldPosition.x, head.position.y * tileSize + fieldPosition.y);

			return result;
		}

		sf::Sprite QuickGameRenderer::createSnakeTailSprite(const QuickGame& game) {
			SnakeSegment tail = game.getSnake()->getTail();

			float tileSize = QuickGameRendererUtils::resolveViewportTileSize(game);
			sf::Vector2f fieldPosition = QuickGameRendererUtils::resolveViewportFieldTopLeftPosition(game, tileSize);

			sf::Sprite result;
			QuickGameRendererUtils::initSnakeTailSprite(result, *this->snakeTilesetTexture, tail.exitDirection);
			result.setPosition(tail.position.x * tileSize + fieldPosition.x, tail.position.y * tileSize + fieldPosition.y);

			return result;
		}

		sf::Sprite QuickGameRenderer::createSnakeBodySprite(const QuickGame& game, const SnakeSegment& snakeSegment) {
			float tileSize = QuickGameRendererUtils::resolveViewportTileSize(game);
			sf::Vector2f fieldPosition = QuickGameRendererUtils::resolveViewportFieldTopLeftPosition(game, tileSize);

			sf::Sprite result;
			QuickGameRendererUtils::initSnakeBodySprite(result, *this->snakeTilesetTexture, snakeSegment.enterDirection, snakeSegment.exitDirection);
			result.setPosition(snakeSegment.position.x * tileSize + fieldPosition.x, snakeSegment.position.y * tileSize + fieldPosition.y);

			return result;
		}

	}

}
