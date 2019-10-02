
#include <assert.h>
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-RenderUtils.hpp"

namespace r3 {

	namespace snake {

		namespace RenderUtils {

			const int TILE_PIXEL_SIZE = 75;

			const float FIELD_HIGHEST_TOP_VIEWPORT_POSITION = 108.0f;
			const float FIELD_MINIMUM_VIEWPORT_MARGIN = 20.0f;
			const sf::Vector2f FIELD_CENTER_VIEWPORT_POSITION(960.0f, 576.0f);

			typedef struct Snake_RenderSnakeSegmentInput {
				const sf::Texture* texture;
				float tileSize;
				sf::Vector2f fieldPosition;
				float spriteScale;
				SnakeSegment segment;
				bool snakeHurtFlag;
			} RenderSnakeSegmentInput;

			float resolveViewportTileSize(const sf::Vector2i& fieldSize) {
				float maxFieldWidth = ViewUtils::VIEW_SIZE.x - (FIELD_MINIMUM_VIEWPORT_MARGIN * 2.0f);
				float maxFieldHeight = ViewUtils::VIEW_SIZE.y - FIELD_HIGHEST_TOP_VIEWPORT_POSITION - FIELD_MINIMUM_VIEWPORT_MARGIN;

				float unalignedTileSize = fminf(maxFieldWidth / fieldSize.x, maxFieldHeight / fieldSize.y);

				float result = (float)TILE_PIXEL_SIZE;
				if (unalignedTileSize < (float)TILE_PIXEL_SIZE) {
					result = (float)TILE_PIXEL_SIZE / ceilf((float)TILE_PIXEL_SIZE / unalignedTileSize);
				}
				else {
					result = (float)TILE_PIXEL_SIZE * floorf(unalignedTileSize / (float)TILE_PIXEL_SIZE);
				}

				return result;
			}

			sf::Vector2f resolveViewportFieldTopLeftPosition(const sf::Vector2i& fieldSize, float viewportTileSize) {
				sf::Vector2f result;
				result.x = FIELD_CENTER_VIEWPORT_POSITION.x - (viewportTileSize * fieldSize.x * 0.5f);
				result.y = FIELD_CENTER_VIEWPORT_POSITION.y - (viewportTileSize * fieldSize.y * 0.5f);
				return result;
			}

			sf::Sprite createSnakeHeadSprite(const RenderSnakeSegmentInput& input) {
				assert(input.segment.segmentType == SnakeSegmentType::HEAD);

				sf::Sprite result(*input.texture);

				switch (input.segment.enterDirection) {
				case ObjectDirection::UP:
					if (input.snakeHurtFlag) {
						result.setTextureRect(sf::IntRect(300, 225, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
					}
					else {
						result.setTextureRect(sf::IntRect(0, 0, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
					}
					break;
				case ObjectDirection::RIGHT:
					if (input.snakeHurtFlag) {
						result.setTextureRect(sf::IntRect(375, 300, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
					}
					else {
						result.setTextureRect(sf::IntRect(375, 0, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
					}
					break;
				case ObjectDirection::DOWN:
					if (input.snakeHurtFlag) {
						result.setTextureRect(sf::IntRect(300, 375, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
					} else {
						result.setTextureRect(sf::IntRect(75, 300, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
					}
					break;
				case ObjectDirection::LEFT:
					if (input.snakeHurtFlag) {
						result.setTextureRect(sf::IntRect(225, 300, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
					}
					else {
						result.setTextureRect(sf::IntRect(75, 75, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
					}
					break;
				}

				result.setPosition(input.fieldPosition.x + input.segment.position.x * input.tileSize, input.fieldPosition.y + input.segment.position.y * input.tileSize);
				result.setScale(input.spriteScale, input.spriteScale);

				return result;
			}

			sf::Sprite createSnakeTailSprite(const RenderSnakeSegmentInput& input) {
				assert(input.segment.segmentType == SnakeSegmentType::TAIL);

				sf::Sprite result(*input.texture);

				switch (input.segment.exitDirection) {
				case ObjectDirection::UP:
					result.setTextureRect(sf::IntRect(0, 150, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
					break;
				case ObjectDirection::RIGHT:
					result.setTextureRect(sf::IntRect(225, 0, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
					break;
				case ObjectDirection::DOWN:
					result.setTextureRect(sf::IntRect(75, 150, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
					break;
				case ObjectDirection::LEFT:
					result.setTextureRect(sf::IntRect(225, 75, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
					break;
				}

				result.setPosition(input.fieldPosition.x + input.segment.position.x * input.tileSize, input.fieldPosition.y + input.segment.position.y * input.tileSize);
				result.setScale(input.spriteScale, input.spriteScale);

				return result;
			}

			sf::Sprite createSnakeBodySprite(const RenderSnakeSegmentInput& input) {
				assert(input.segment.segmentType == SnakeSegmentType::BODY);

				sf::Sprite result(*input.texture);
				switch (input.segment.enterDirection) {
				case ObjectDirection::UP:
					assert(input.segment.exitDirection != ObjectDirection::DOWN);
					switch (input.segment.exitDirection) {
					case ObjectDirection::UP:
						result.setTextureRect(sf::IntRect(0, 75, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::RIGHT:
						result.setTextureRect(sf::IntRect(0, 300, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::LEFT:
						result.setTextureRect(sf::IntRect(0, 225, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
						break;
					}
					break;
				case ObjectDirection::RIGHT:
					assert(input.segment.exitDirection != ObjectDirection::LEFT);
					switch (input.segment.exitDirection) {
					case ObjectDirection::UP:
						result.setTextureRect(sf::IntRect(150, 0, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::RIGHT:
						result.setTextureRect(sf::IntRect(300, 0, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::DOWN:
						result.setTextureRect(sf::IntRect(75, 0, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
						break;
					}
					break;
				case ObjectDirection::DOWN:
					assert(input.segment.exitDirection != ObjectDirection::UP);
					switch (input.segment.exitDirection) {
					case ObjectDirection::RIGHT:
						result.setTextureRect(sf::IntRect(225, 150, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::DOWN:
						result.setTextureRect(sf::IntRect(75, 225, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::LEFT:
						result.setTextureRect(sf::IntRect(150, 150, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
						break;
					}
					break;
				case ObjectDirection::LEFT:
					assert(input.segment.exitDirection != ObjectDirection::RIGHT);
					switch (input.segment.exitDirection) {
					case ObjectDirection::UP:
						result.setTextureRect(sf::IntRect(300, 75, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::DOWN:
						result.setTextureRect(sf::IntRect(375, 75, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
						break;
					case ObjectDirection::LEFT:
						result.setTextureRect(sf::IntRect(150, 75, TILE_PIXEL_SIZE, TILE_PIXEL_SIZE));
						break;
					}
					break;
				}

				result.setPosition(input.fieldPosition.x + input.segment.position.x * input.tileSize, input.fieldPosition.y + input.segment.position.y * input.tileSize);
				result.setScale(input.spriteScale, input.spriteScale);

				return result;
			}

			void renderSnake(sf::RenderTarget& renderTarget, const RenderSnakeInput& renderSnakeInput) {
				RenderSnakeSegmentInput renderSegmentInput;
				renderSegmentInput.texture = renderSnakeInput.texture;
				renderSegmentInput.tileSize = resolveViewportTileSize(renderSnakeInput.fieldSize);
				renderSegmentInput.fieldPosition = resolveViewportFieldTopLeftPosition(renderSnakeInput.fieldSize, renderSegmentInput.tileSize);
				renderSegmentInput.spriteScale = renderSegmentInput.tileSize / (float)TILE_PIXEL_SIZE;
				renderSegmentInput.snakeHurtFlag = renderSnakeInput.snakeHurtFlag;

				renderSegmentInput.segment = renderSnakeInput.snake->getTail();
				sf::Sprite tailSprite = createSnakeTailSprite(renderSegmentInput);
				renderTarget.draw(tailSprite);

				int bodySegmentCount = renderSnakeInput.snake->getBodyLength();
				for (int segmentIndex = bodySegmentCount - 1; segmentIndex >= 0; segmentIndex--) {
					renderSegmentInput.segment = renderSnakeInput.snake->getBody(segmentIndex);
					sf::Sprite bodySprite = createSnakeBodySprite(renderSegmentInput);
					renderTarget.draw(bodySprite);
				}

				renderSegmentInput.segment = renderSnakeInput.snake->getHead();
				sf::Sprite headSprite = createSnakeHeadSprite(renderSegmentInput);
				renderTarget.draw(headSprite);
			}

		}

	}

}
