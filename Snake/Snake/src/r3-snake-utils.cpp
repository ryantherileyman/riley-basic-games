
#include "includes/r3-snake-utils.hpp"

namespace r3 {

	namespace snake {

		namespace GameLoopUtils {

			const sf::Int64 MICROSECONDS_PER_FRAME = 1000000 / 60;

		}

		namespace ViewUtils {

			const float VIEW_ASPECT_RATIO = 1.77777778f;
			const sf::Vector2f VIEW_SIZE(1920.0f, 1080.0f);

			sf::View createView(unsigned int windowWidth, unsigned int windowHeight) {
				float aspectRatio = (float)windowWidth / (float)windowHeight;

				sf::FloatRect viewport(0.0f, 0.0f, VIEW_SIZE.x, VIEW_SIZE.y);

				if (aspectRatio > VIEW_ASPECT_RATIO) {
					viewport.width = VIEW_SIZE.x * (aspectRatio / VIEW_ASPECT_RATIO);
					viewport.left = 0.0f - ((viewport.width - VIEW_SIZE.x) / 2.0f);
				}
				else {
					viewport.height = VIEW_SIZE.y * (VIEW_ASPECT_RATIO / aspectRatio);
					viewport.top = 0.0f - ((viewport.height - VIEW_SIZE.y) / 2.0f);
				}

				sf::View result(viewport);
				return result;
			}

		}

		namespace FontUtils {

			float resolveTextWidth(sf::Text& text) {
				float result = text.findCharacterPos(text.getString().getSize()).x - text.findCharacterPos(0).x;
				return result;
			}

			void setCenteredPosition(sf::Text& text, float top) {
				float width = resolveTextWidth(text);
				text.setPosition((ViewUtils::VIEW_SIZE.x / 2.0f) - (width / 2.0f), top);
			}

		}

	}

}
