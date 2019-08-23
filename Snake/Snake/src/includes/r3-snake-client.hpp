
#include <SFML/Graphics.hpp>
#pragma once

namespace r3 {

	namespace snake {
		
		typedef enum Snake_ClientMode {
			SPLASH_SCREEN,
			QUICK_GAME,
		} ClientMode;

		class SplashSceneController;
		class QuickGameController;
		class GameClient;

		class GameClient {

		private:
			ClientMode mode;
			sf::RenderWindow window;

		private:
			SplashSceneController* splashSceneController;
			QuickGameController* quickGameController;

		public:
			GameClient();

		public:
			~GameClient();

		public:
			void run();

		private:
			void processSplashScreenEvent(sf::Event& event);
			void processQuickGameEvent(sf::Event& event);

		};

	}

}

