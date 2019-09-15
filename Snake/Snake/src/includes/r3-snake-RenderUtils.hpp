
#include <SFML/Graphics.hpp>

namespace r3 {

	namespace snake {

		namespace RenderUtils {

			float resolveViewportTileSize(const sf::Vector2i& fieldSize);

			sf::Vector2f resolveViewportFieldTopLeftPosition(const sf::Vector2i& fieldSize, float viewportTileSize);

		}

	}

}
