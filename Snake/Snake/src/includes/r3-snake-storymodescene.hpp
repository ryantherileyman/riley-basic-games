
#include <SFML/Graphics.hpp>
#include "r3-snake-gameoptions.hpp"
#pragma once

namespace r3 {

	namespace snake {

		typedef enum class Snake_StoryGameMode {
			WAIT_TO_START,
		} StoryGameMode;

		typedef enum class Snake_StoryGameSceneClientRequest {
			NONE,
			EXIT_GAME,
			RETURN_TO_SPLASH_SCREEN,
		} StoryGameSceneClientRequest;

		class StoryGameController;
		class StoryGameRenderer;

		class StoryGameController {

		private:
			sf::RenderWindow* window;
			StoryGameRenderer* renderer;
			SystemOptionsDefn systemOptions;

		private:
			StoryGameMode mode;

		public:
			StoryGameController(sf::RenderWindow& window);

		public:
			~StoryGameController();

		public:
			void setSystemOptions(const SystemOptionsDefn& systemOptions);

		public:
			StoryGameSceneClientRequest processEvent(sf::Event& event);
			void update();
			void render();

		private:
			StoryGameSceneClientRequest processWaitToStartKeyEvent(sf::Event& event);

		};

		class StoryGameRenderer {

		private:
			sf::Font* uiFont;
			sf::Texture* healthBarTexture;

		public:
			StoryGameRenderer();

		public:
			~StoryGameRenderer();

		public:
			void renderWaitToStart(sf::RenderTarget& renderTarget);

		private:
			void renderWaitToStartInstructions(sf::RenderTarget& renderTarget);
			void renderGameRunningUi(sf::RenderTarget& renderTarget);

		};

	}

}
