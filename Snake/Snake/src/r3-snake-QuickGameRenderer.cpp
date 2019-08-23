
#include <assert.h>
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-quickgamescene.hpp"

namespace r3 {

	namespace snake {

		const char* QUICK_GAME_UI_FONT_PATH = "resources/fonts/SourceSansPro-Regular.otf";
		const char* SNAKE_TILESET_TEXTURE_PATH = "resources/textures/snake-tileset.png";

		const int SNAKE_TILE_PIXEL_SIZE = 75;
		const float SNAKE_TILE_VIEWPORT_SIZE = 37.5f;
		const sf::Vector2f FIELD_VIEWPORT_POSITION(22.0f, 108.0f); // TODO: configurable based on field size...  this is the furthest top-left position
		const sf::Vector2i FIELD_SIZE(50, 25); // TODO: field size is part of the game state...

		const sf::Color QUICK_GAME_BACKGROUND_COLOR = sf::Color(0, 126, 3, 255);

		const wchar_t* SNAKE_LENGTH_FORMAT_STRING = L"Snake Length: %d";
		const wchar_t* LONGEST_SNAKE_FORMAT_STRING = L"Longest Snake: %d";

		const wchar_t* EXIT_INSTRUCTIONS = L"Press ESC to return to the main menu";

		namespace QuickGameRendererUtils {

			void initSprite(sf::Sprite& sprite, const sf::Texture& sourceTexture, int pixelLeft, int pixelTop) {
				sprite.setTexture(sourceTexture);
				sprite.setTextureRect(sf::IntRect(pixelLeft, pixelTop, SNAKE_TILE_PIXEL_SIZE, SNAKE_TILE_PIXEL_SIZE));
				sprite.setScale(0.5f, 0.5f);
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
				sprite.setScale(0.5f, 0.5f);
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
				sprite.setScale(0.5f, 0.5f);
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
				sprite.setScale(0.5f, 0.5f);
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

			this->snakeLengthText.setFont(*this->uiFont);
			this->snakeLengthText.setCharacterSize(48);
			this->snakeLengthText.setOutlineColor(sf::Color::White);
			this->snakeLengthText.setPosition(24.0f, 24.0f);

			this->longestSnakeText.setFont(*this->uiFont);
			this->longestSnakeText.setCharacterSize(48);
			this->longestSnakeText.setOutlineColor(sf::Color::White);
			this->longestSnakeText.setPosition(0.0f, 24.0f);

			this->exitInstructionsText.setFont(*this->uiFont);
			this->exitInstructionsText.setCharacterSize(64);
			this->exitInstructionsText.setOutlineColor(sf::Color::White);
			this->exitInstructionsText.setString(EXIT_INSTRUCTIONS);
			float exitInstructionsWidth = FontUtils::resolveTextWidth(exitInstructionsText);
			this->exitInstructionsText.setPosition((ViewUtils::VIEW_SIZE.x / 2.0f) - (exitInstructionsWidth / 2.0f), (ViewUtils::VIEW_SIZE.y / 2.0f));

			QuickGameRendererUtils::initSprite(this->grassSprite, *this->snakeTilesetTexture, 0, 375);
			QuickGameRendererUtils::initSprite(this->shrubSprite, *this->snakeTilesetTexture, 75, 375);
		}

		QuickGameRenderer::~QuickGameRenderer() {
			if (this->uiFont != nullptr) {
				delete this->uiFont;
			}
			if (this->snakeTilesetTexture != nullptr) {
				delete this->snakeTilesetTexture;
			}
		}

		void QuickGameRenderer::renderWaitToStart(sf::RenderTarget& renderTarget) {
			renderTarget.clear(QUICK_GAME_BACKGROUND_COLOR);
			this->renderPlayingField(renderTarget);
			this->renderScoreUi(renderTarget);
			renderTarget.draw(exitInstructionsText);
		}

		void QuickGameRenderer::renderGameRunning(sf::RenderTarget& renderTarget, const QuickGame& game) {
			renderTarget.clear(QUICK_GAME_BACKGROUND_COLOR);
			this->renderPlayingField(renderTarget);
			this->renderSnake(renderTarget, game);
			this->renderScoreUi(renderTarget);
		}

		void QuickGameRenderer::renderPlayingField(sf::RenderTarget& renderTarget) {
			// Draw grass tiles under entire playing field
			for (int x = 0; x < FIELD_SIZE.x; x++) {
				for (int y = 0; y < FIELD_SIZE.y; y++) {
					this->grassSprite.setPosition(x * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.x, y * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.y);
					renderTarget.draw(this->grassSprite);
				}
			}

			// Draw top and bottom rows of shrub barriers
			for (int x = 0; x < FIELD_SIZE.x; x++) {
				this->shrubSprite.setPosition(x * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.x, FIELD_VIEWPORT_POSITION.y);
				renderTarget.draw(this->shrubSprite);

				this->shrubSprite.setPosition(x * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.x, (FIELD_SIZE.y - 1) * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.y);
				renderTarget.draw(this->shrubSprite);
			}

			// Draw left and right columns of shrub barriers
			for (int y = 1; y < FIELD_SIZE.y - 1; y++) {
				this->shrubSprite.setPosition(FIELD_VIEWPORT_POSITION.x, y * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.y);
				renderTarget.draw(this->shrubSprite);

				this->shrubSprite.setPosition((FIELD_SIZE.x - 1) * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.x, y * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.y);
				renderTarget.draw(this->shrubSprite);
			}
		}

		void QuickGameRenderer::renderSnake(sf::RenderTarget& renderTarget, const QuickGame& game) {
			sf::Sprite tailSprite = this->createSnakeTailSprite(game);
			renderTarget.draw(tailSprite);

			int bodySegmentCount = game.getSnake()->getBodyLength();
			for (int segmentIndex = bodySegmentCount - 1; segmentIndex >= 0; segmentIndex--) {
				sf::Sprite currBodySprite = this->createSnakeBodySprite(game.getSnake()->getBody(segmentIndex));
				renderTarget.draw(currBodySprite);
			}

			sf::Sprite headSprite = this->createSnakeHeadSprite(game);
			renderTarget.draw(headSprite);
		}

		void QuickGameRenderer::renderScoreUi(sf::RenderTarget& renderTarget) {
			// Draw current snake length
			wchar_t snakeLengthString[32];
			swprintf_s(snakeLengthString, SNAKE_LENGTH_FORMAT_STRING, 0);
			snakeLengthText.setString(snakeLengthString);
			renderTarget.draw(snakeLengthText);

			// Draw longest snake length
			wchar_t longestSnakeString[32];
			swprintf_s(longestSnakeString, LONGEST_SNAKE_FORMAT_STRING, 0);
			longestSnakeText.setString(longestSnakeString);
			float longestSnakeTextWidth = FontUtils::resolveTextWidth(longestSnakeText);
			longestSnakeText.setPosition(ViewUtils::VIEW_SIZE.x - longestSnakeTextWidth - 24.0f, 24.0f);
			renderTarget.draw(longestSnakeText);
		}

		sf::Sprite QuickGameRenderer::createSnakeHeadSprite(const QuickGame& game) {
			sf::Sprite result;

			SnakeSegment head = game.getSnake()->getHead();

			QuickGameRendererUtils::initSnakeHeadSprite(result, *this->snakeTilesetTexture, head.enterDirection);
			result.setPosition(head.position.x * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.x, head.position.y * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.y);

			return result;
		}

		sf::Sprite QuickGameRenderer::createSnakeTailSprite(const QuickGame& game) {
			sf::Sprite result;

			SnakeSegment tail = game.getSnake()->getTail();

			QuickGameRendererUtils::initSnakeTailSprite(result, *this->snakeTilesetTexture, tail.exitDirection);
			result.setPosition(tail.position.x * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.x, tail.position.y * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.y);

			return result;
		}

		sf::Sprite QuickGameRenderer::createSnakeBodySprite(const SnakeSegment& snakeSegment) {
			sf::Sprite result;

			QuickGameRendererUtils::initSnakeBodySprite(result, *this->snakeTilesetTexture, snakeSegment.enterDirection, snakeSegment.exitDirection);
			result.setPosition(snakeSegment.position.x * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.x, snakeSegment.position.y * SNAKE_TILE_VIEWPORT_SIZE + FIELD_VIEWPORT_POSITION.y);

			return result;
		}

	}

}
