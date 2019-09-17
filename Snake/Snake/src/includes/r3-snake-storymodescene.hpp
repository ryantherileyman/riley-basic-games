
#include <queue>
#include <SFML/Graphics.hpp>
#include "r3-snake-storydefn.hpp"
#include "r3-snake-storyassets.hpp"
#include "r3-snake-gameoptions.hpp"
#pragma once

namespace r3 {

	namespace snake {

		typedef enum class Snake_StoryGameMode {
			LOAD_CAMPAIGN,
			LOAD_CAMPAIGN_ERROR,
			LOAD_LEVEL,
			LOAD_LEVEL_ERROR,
			WAIT_TO_START,
			GAME_RUNNING,
		} StoryGameMode;

		typedef enum class Snake_StoryGameSceneClientRequest {
			NONE,
			EXIT_GAME,
			RETURN_TO_SPLASH_SCREEN,
		} StoryGameSceneClientRequest;

		typedef struct Snake_StoryGameInputRequest {
			std::vector<ObjectDirection> snakeMovementList;
		} StoryGameInputRequest;

		typedef struct Snake_StoryGameUpdateResult {
			ObjectDirection snakeMovementResult;
			bool snakeHitBarrierFlag;
		} StoryGameUpdateResult;

		class StoryGame;
		class StoryGameController;
		class StoryGameRenderer;

		class StoryGame {

		private:
			StoryMap* map;
			Snake* snake;
			float snakeSpeedTilesPerSecond;

		private:
			int framesSinceSnakeMoved;
			std::queue<ObjectDirection> snakeMovementQueue;

		public:
			StoryGame();

		public:
			~StoryGame();

		public:
			void startNewLevel(const StoryMapDefn& mapDefn, const StoryLevelDefn& levelDefn);

		public:
			StoryMap* getMap() const;
			Snake* getSnake() const;

		public:
			StoryGameUpdateResult update(const StoryGameInputRequest& input);

		private:
			void freeLevel();

		private:
			void acceptSnakeMovementList(const std::vector<ObjectDirection>& snakeMovementList);
			void consumeAllUnusableSnakeInputs();
			ObjectDirection resolveDirectionToMoveSnake();
			bool snakeWouldHitBarrier(ObjectDirection direction);

		};

		class StoryGameController {

		private:
			sf::RenderWindow* window;
			StoryGameRenderer* renderer;
			SystemOptionsDefn systemOptions;

		private:
			StoryGameMode mode;
			std::string campaignFolder;
			int currLevelIndex;
			std::vector<StoryLevelDefn> levelDefnList;
			StoryLevelAssetBundle* levelAssetBundle;
			StoryGame* storyGame;

		private:
			std::vector<ObjectDirection> snakeMovementInputQueue;

		public:
			StoryGameController(sf::RenderWindow& window);

		public:
			~StoryGameController();

		public:
			void setCampaignFolder(const std::string& campaignFolder);
			void setSystemOptions(const SystemOptionsDefn& systemOptions);

		public:
			StoryGameSceneClientRequest processEvent(sf::Event& event);
			void update();
			void render();

		private:
			void loadCampaign();
			void initiateLoadLevel();
			void updateBasedOnLoadLevelStatus();

		private:
			StoryGameSceneClientRequest processLoadErrorKeyEvent(sf::Event& event);
			StoryGameSceneClientRequest processWaitToStartKeyEvent(sf::Event& event);
			StoryGameSceneClientRequest processGameRunningKeyEvent(sf::Event& event);

		private:
			void updateGameRunning();

		};

		typedef struct Snake_StoryGameRenderState {
			const StoryLevelAssetBundle* levelAssetBundle;
			const StoryGame* storyGame;
		} StoryGameRenderState;

		class StoryGameRenderer {

		private:
			sf::Font* uiFont;
			sf::Texture* healthBarTexture;

		public:
			StoryGameRenderer();

		public:
			~StoryGameRenderer();

		public:
			void renderLoadCampaignError(sf::RenderTarget& renderTarget);
			void renderLoadLevelStatus(sf::RenderTarget& renderTarget, const StoryLevelAssetLoadingStatus& assetLoadingStatus);
			void renderLoadLevelError(sf::RenderTarget& renderTarget);
			void renderWaitToStart(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderGameRunning(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);

		private:
			void renderGameRunningUi(sf::RenderTarget& renderTarget);
			void renderPlayingField(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderSnake(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderWaitToStartInstructions(sf::RenderTarget& renderTarget);
			void renderExitInstructions(sf::RenderTarget& renderTarget);

		private:
			sf::RectangleShape createTextBackgroundShape(float width, float height);
			sf::Text createInstructionsText(const wchar_t* textString, float y);

		};

	}

}
