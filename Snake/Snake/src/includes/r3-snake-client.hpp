
#include <SFML/Graphics.hpp>
#pragma once

namespace r3 {

	namespace snake {
		
		typedef enum class Snake_ClientMode {
			SPLASH_SCREEN,
			QUICK_GAME,
			STORY_GAME,
		} ClientMode;

		class SplashSceneController;
		class QuickGameController;
		class StoryGameController;
		class GameClient;

		class GameClient {

		private:
			ClientMode mode;
			sf::RenderWindow window;

		private:
			SplashSceneController* splashSceneController;
			QuickGameController* quickGameController;
			StoryGameController* storyGameController;

		public:
			GameClient();

		public:
			~GameClient();

		public:
			void run();

		private:
			void processSplashScreenEvent(sf::Event& event);
			void processQuickGameEvent(sf::Event& event);
			void processStoryGameEvent(sf::Event& event);

		};

	}

}

