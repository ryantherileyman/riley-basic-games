
#include <SFML/Graphics.hpp>
#pragma once

namespace r3 {

	namespace snake {

		typedef enum class Snake_QuickGameSceneClientRequest {
			NONE,
			EXIT_GAME,
			RETURN_TO_SPLASH_SCREEN,
		} QuickGameSceneClientRequest;

		class QuickGameController;
		class QuickGameRenderer;

		class QuickGameController {

		private:
			sf::RenderWindow* window;
			QuickGameRenderer* renderer;

		public:
			QuickGameController(sf::RenderWindow& window);

		public:
			~QuickGameController();

		public:
			QuickGameSceneClientRequest processEvent(sf::Event& event);
			void render();

		};

		class QuickGameRenderer {

		private:
			sf::Font* uiFont;
			sf::Texture* snakeTilesetTexture;

		private:
			sf::Text snakeLengthText;
			sf::Text longestSnakeText;
			sf::Text exitInstructionsText;

		private:
			sf::Sprite grassSprite;
			sf::Sprite shrubSprite;

		public:
			QuickGameRenderer();

		public:
			~QuickGameRenderer();

		public:
			void render(sf::RenderTarget& renderTarget);

		private:
			void renderPlayingField(sf::RenderTarget& renderTarget);
			void renderScoreUi(sf::RenderTarget& renderTarget);

		};

	}

}
