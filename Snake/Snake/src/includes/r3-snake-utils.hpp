
#include <SFML/Graphics.hpp>
#pragma once

namespace r3 {

	namespace snake {

		namespace GameLoopUtils {

			extern const sf::Int64 MICROSECONDS_PER_FRAME;

		}

		namespace ViewUtils {

			extern const float VIEW_ASPECT_RATIO;
			extern const sf::Vector2f VIEW_SIZE;

			sf::View createView(unsigned int windowWidth, unsigned int windowHeight);

		}

		namespace FontUtils {

			extern float resolveTextWidth(sf::Text& text);
			extern void setCenteredPosition(sf::Text& text, float top);

		}

	}

}
