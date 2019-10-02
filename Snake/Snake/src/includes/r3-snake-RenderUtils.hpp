
#include <SFML/Graphics.hpp>
#include "r3-snake-gamestate.hpp"

namespace r3 {

	namespace snake {

		namespace RenderUtils {

			typedef struct Snake_RenderSnakeInput {
				sf::Vector2i fieldSize;
				const sf::Texture* texture;
				const Snake* snake;
				bool snakeHurtFlag;
			} RenderSnakeInput;

			float resolveViewportTileSize(const sf::Vector2i& fieldSize);

			sf::Vector2f resolveViewportFieldTopLeftPosition(const sf::Vector2i& fieldSize, float viewportTileSize);

			void renderSnake(sf::RenderTarget& renderTarget, const RenderSnakeInput& renderSnakeInput);

		}

	}

}
