
#pragma once

namespace r3 {

	namespace snake {

		typedef struct Snake_QuickGameOptionsDefn {
			sf::Vector2i fieldSize = sf::Vector2i(50, 25);
			int snakeSpeedTilesPerSecond = 10;
			int snakeGrowthPerApple = 3;
		} QuickGameOptionsDefn;

		typedef struct Snake_SystemOptionsDefn {
			int musicVolume = 100;
			int soundEffectsVolume = 100;
		} SystemOptionsDefn;

	}

}
