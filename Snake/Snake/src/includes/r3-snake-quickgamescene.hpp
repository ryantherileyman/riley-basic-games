
#include <SFML/Graphics.hpp>
#include "r3-snake-gamestate.hpp"
#pragma once

namespace r3 {

	namespace snake {

		typedef struct Snake_QuickGameDefn {
			sf::Vector2i fieldSize;
			float snakeSpeedTilesPerSecond;
			SnakeStartDefn snakeStartDefn;
		} QuickGameDefn;

		typedef struct Snake_QuickGameInputRequest {
			ObjectDirection snakeMovementInput;
		} QuickGameInputRequest;

		typedef struct Snake_QuickGameUpdateResult {
			ObjectDirection snakeMovementResult;
			bool snakeHitBarrierFlag;
		} QuickGameUpdateResult;

		typedef enum class Snake_QuickGameSceneClientRequest {
			NONE,
			EXIT_GAME,
			RETURN_TO_SPLASH_SCREEN,
		} QuickGameSceneClientRequest;

		class QuickGame;
		class QuickGameController;
		class QuickGameRenderer;

		class QuickGame {

		private:
			sf::Vector2i fieldSize;
			float snakeSpeedTilesPerSecond;
			Snake* snake;

		private:
			int framesSinceSnakeMoved;
			ObjectDirection queuedSnakeInput;

		public:
			QuickGame(const QuickGameDefn* quickGameDefn);

		public:
			~QuickGame();

		public:
			QuickGameUpdateResult update(const QuickGameInputRequest* input);

		private:
			bool snakeWouldHitBarrier(ObjectDirection direction);

		};

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
