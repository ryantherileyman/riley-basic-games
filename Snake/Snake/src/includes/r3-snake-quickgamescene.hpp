
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "r3-snake-gameoptions.hpp"
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
			bool snakeAteAppleFlag;
			bool snakeGrewFlag;
		} QuickGameUpdateResult;

		typedef enum class Snake_QuickGameMode {
			WAIT_TO_START,
			GAME_RUNNING,
			GAME_DONE_SUMMARY,
		} QuickGameMode;

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
			std::default_random_engine randomizer;

		private:
			sf::Vector2i fieldSize;
			float snakeSpeedTilesPerSecond;
			Snake* snake;
			
		private:
			bool appleExistsFlag;
			sf::Vector2i applePosition;

		private:
			int framesSinceSnakeMoved;
			ObjectDirection queuedSnakeInput;
			int queuedSnakeGrowth;

		public:
			QuickGame(const QuickGameDefn* quickGameDefn);

		public:
			~QuickGame();

		public:
			sf::Vector2i getFieldSize() const;
			Snake* getSnake() const;
			bool getAppleExists() const;
			sf::Vector2i getApplePosition() const;

		public:
			QuickGameUpdateResult update(const QuickGameInputRequest* input);

		private:
			sf::Vector2i resolveNewApplePosition();
			bool snakeWouldHitBarrier(ObjectDirection direction);

		};

		class QuickGameController {

		private:
			sf::RenderWindow* window;
			QuickGameRenderer* renderer;

		private:
			QuickGameMode mode;
			QuickGameOptionsDefn gameOptions;
			QuickGame* game;

		private:
			sf::SoundBuffer* eatAppleSoundBuffer;
			sf::SoundBuffer* hitBarrierSoundBuffer;

		private:
			sf::Sound eatAppleSound;
			sf::Sound hitBarrierSound;

		private:
			sf::Music* gameRunningMusic;
			bool gameRunningMusicLoaded;

		private:
			sf::Music* gameDoneSummaryMusic;
			bool gameDoneSummaryMusicLoaded;

		private:
			int longestSnakeLength;
			bool lastGameBeatLongestSnakeLength;

		private:
			ObjectDirection nextSnakeMovementInput;

		public:
			QuickGameController(sf::RenderWindow& window);

		public:
			~QuickGameController();

		public:
			void setQuickGameOptions(const QuickGameOptionsDefn& gameOptions);

		public:
			QuickGameSceneClientRequest processEvent(sf::Event& event);
			void update();
			void render();

		private:
			QuickGameSceneClientRequest processWaitToStartKeyEvent(sf::Event& event);
			QuickGameSceneClientRequest processGameRunningKeyEvent(sf::Event& event);

		private:
			void startGame();

		private:
			void ensureGameRunningMusicLoaded();
			void freeGameRunningMusic();

		private:
			void ensureGameDoneSummaryMusicLoaded();
			void freeGameDoneSummaryMusic();

		private:
			void beginWaitToStartMusic();
			void beginGameRunningMusic();
			void beginGameDoneSummaryMusic();

		};

		typedef struct Snake_QuickGameRenderState {
			const QuickGame* game;
			int longestSnake;
			bool lastGameBeatLongestSnakeLength;
		} QuickGameRenderState;

		class QuickGameRenderer {

		private:
			sf::Font* uiFont;
			sf::Texture* snakeTilesetTexture;

		private:
			sf::Text snakeLengthText;
			sf::Text longestSnakeText;
			sf::Text startInstructionsText;
			sf::Text exitInstructionsText;

		private:
			sf::Sprite grassSprite;
			sf::Sprite shrubSprite;
			sf::Sprite appleSprite;

		public:
			QuickGameRenderer();

		public:
			~QuickGameRenderer();

		public:
			void renderWaitToStart(sf::RenderTarget& renderTarget, const QuickGameRenderState& gameRenderState);
			void renderGameRunning(sf::RenderTarget& renderTarget, const QuickGameRenderState& gameRenderState);
			void renderGameDoneSummary(sf::RenderTarget& renderTarget, const QuickGameRenderState& gameRenderState);

		private:
			void renderPlayingField(sf::RenderTarget& renderTarget, const QuickGame& game);
			void renderApple(sf::RenderTarget& renderTarget, const QuickGame& game);
			void renderSnake(sf::RenderTarget& renderTarget, const QuickGame& game);
			void renderScoreUi(sf::RenderTarget& renderTarget, const QuickGameRenderState& gameRenderState);
			void renderLongestSnakeUi(sf::RenderTarget& renderTarget);

		private:
			sf::Sprite createSnakeHeadSprite(const QuickGame& game);
			sf::Sprite createSnakeTailSprite(const QuickGame& game);
			sf::Sprite createSnakeBodySprite(const QuickGame& game, const SnakeSegment& snakeSegment);

		};

	}

}
