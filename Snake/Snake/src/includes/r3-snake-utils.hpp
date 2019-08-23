
#include <SFML/Graphics.hpp>
#pragma once

namespace r3 {

	namespace snake {

		namespace ViewUtils {

			extern const float VIEW_ASPECT_RATIO;
			extern const sf::Vector2f VIEW_SIZE;

			sf::View createView(unsigned int windowWidth, unsigned int windowHeight);

		}

		namespace FontUtils {

			float resolveTextWidth(sf::Text& text);

		}

	}

}
