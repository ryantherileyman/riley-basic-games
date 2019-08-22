
#include <SFML/Graphics.hpp>
#pragma once

namespace r3 {

	namespace snake {
		
		typedef enum Snake_ClientMode {
			SPLASH_SCREEN,
		} ClientMode;

		class SplashSceneController;
		class GameClient;

		class GameClient {

		private:
			ClientMode mode;
			sf::RenderWindow window;

		private:
			SplashSceneController* splashSceneController;

		public:
			GameClient();

		public:
			~GameClient();

		public:
			void run();

		private:
			void processSplashScreenEvent(sf::Event& event);

		};

	}

}

