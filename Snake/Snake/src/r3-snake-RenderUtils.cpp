
#include "includes/r3-snake-utils.hpp"
#include "includes/r3-snake-RenderUtils.hpp"

namespace r3 {

	namespace snake {

		namespace RenderUtils {

			const int TILE_PIXEL_SIZE = 75;

			const float FIELD_HIGHEST_TOP_VIEWPORT_POSITION = 108.0f;
			const float FIELD_MINIMUM_VIEWPORT_MARGIN = 20.0f;
			const sf::Vector2f FIELD_CENTER_VIEWPORT_POSITION(960.0f, 576.0f);

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

		}

	}

}
