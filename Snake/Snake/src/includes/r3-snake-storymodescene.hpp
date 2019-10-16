
#include <random>
#include <queue>
#include <SFML/Graphics.hpp>
#include "r3-snake-storydefn.hpp"
#include "r3-snake-storyassets.hpp"
#include "r3-snake-gameoptions.hpp"
#include "r3-sound-SimpleSoundManager.hpp"
#pragma once

namespace r3 {

	namespace snake {

		typedef enum class Snake_StoryGameMode {
			LOAD_CAMPAIGN,
			LOAD_CAMPAIGN_ERROR,
			LOAD_LEVEL,
			LOAD_LEVEL_ERROR,
			PLAY_OPENING_CUTSCENE,
			WAIT_TO_START,
			GAME_RUNNING,
			LEVEL_LOST,
			CAMPAIGN_WON,
		} StoryGameMode;

		typedef enum class Snake_StoryGameSceneClientRequest {
			NONE,
			EXIT_GAME,
			RETURN_TO_SPLASH_SCREEN,
		} StoryGameSceneClientRequest;

		typedef struct Snake_StoryGameInputRequest {
			std::vector<ObjectDirection> snakeMovementList;
		} StoryGameInputRequest;

		typedef struct Snake_StoryFoodInstance {
			int foodInstanceId;
			StoryFoodType foodType;
			sf::Vector2i position;
		} StoryFoodInstance;

		typedef struct Snake_StoryFoodSpawnCheckInput {
			sf::Time timeSinceLevelStarted;
			std::default_random_engine* randomizer;
			int snakeLength;
		} StoryFoodSpawnCheckInput;

		typedef struct Snake_StoryFoodPositionCheckResult {
			bool foodExistsFlag;
			int foodInstanceId;
		} StoryFoodPositionCheckResult;

		typedef struct Snake_StoryCheckForFoodEatenBySnakeResult {
			std::vector<StoryFoodInstance> eatenBySnakeFoodInstanceList;
			int snakeGrowth;
		} StoryCheckForFoodEatenBySnakeResult;

		typedef struct Snake_StoryFoodTileDistanceTracking {
			int origTileDistanceFromSnake;
			int tileDistanceSnakeTravelled;
		} StoryFoodTileDistanceTracking;

		typedef struct Snake_StoryFoodEatenBySnakeScoreResult {
			int baseScore;
			int bonusPathScore;
			int perfectPathScore;
			int totalScore;
		} StoryFoodEatenBySnakeScoreResult;

		typedef struct Snake_StoryFoodEatenResult {
			StoryFoodInstance foodInstance;
			StoryFoodEatenBySnakeScoreResult scoreResult;
		} StoryFoodEatenResult;

		typedef struct Snake_StoryDangerInstance {
			int dangerInstanceId;
			StoryDangerType dangerType;
			sf::Vector2i position;
		} StoryDangerInstance;

		typedef struct Snake_StoryDangerSpawnCheckInput {
			sf::Time timeSinceLevelStarted;
			std::default_random_engine* randomizer;
			int snakeLength;
		} StoryDangerSpawnCheckInput;

		typedef struct Snake_StoryDangerPositionCheckResult {
			bool dangerExistsFlag;
			int dangerInstanceId;
		} StoryDangerPositionCheckResult;

		typedef struct Snake_StoryDangerDespawnCheckInput {
			int dangerInstanceId;
			sf::Time timeSinceLevelStarted;
		} StoryDangerDespawnCheckInput;

		typedef struct Snake_StoryGameUpdateResult {
			ObjectDirection snakeMovementResult;
			bool snakeHitBarrierFlag;
			bool snakeDiedFlag;
			bool snakeGrewFlag;
			bool completedLevelFlag;
			std::vector<StoryFoodInstance> spawnedFoodInstanceList;
			std::vector<StoryDangerInstance> spawnedDangerInstanceList;
			std::vector<StoryFoodEatenResult> foodEatenResultList;
			std::vector<StoryDangerInstance> dangerInstanceStruckSnakeList;

			bool snakeDamaged() {
				bool result =
					snakeHitBarrierFlag ||
					!dangerInstanceStruckSnakeList.empty();
				return result;
			}
		} StoryGameUpdateResult;

		class StoryFoodSpawnTracker;
		class StoryDangerSpawnTracker;
		class StoryGame;
		class StoryCutscene;
		class StoryGameController;
		class StoryGameRenderer;

		class StoryFoodSpawnTracker {

		private:
			const StoryFoodDefn* foodDefn;

		private:
			std::uniform_real_distribution<float> chancePctDistribution;

		private:
			std::vector<StoryFoodInstance> foodInstanceList;

		private:
			int spawnCount;
			sf::Time timeOfLastChanceCheck;
			sf::Time timeOfLastSpawn;

		public:
			StoryFoodSpawnTracker(const StoryFoodDefn& foodDefn);

		public:
			const StoryFoodDefn& getFoodDefn() const;
			const std::vector<StoryFoodInstance>& getFoodInstanceList() const;
			StoryFoodInstance getFoodInstance(int foodInstanceId) const;

		public:
			bool shouldFoodSpawn(const StoryFoodSpawnCheckInput& input);
			StoryFoodPositionCheckResult occupiesPosition(const sf::Vector2i& position) const;

		public:
			void spawnFood(const StoryFoodInstance& foodInstance);
			void despawnFood(int foodInstanceId);

		private:
			int findFoodInstanceIndex(int foodInstanceId) const;

		};

		class StoryDangerSpawnTracker {

		private:
			const StoryDangerDefn* dangerDefn;

		private:
			std::uniform_real_distribution<float> chancePctDistribution;

		private:
			std::vector<StoryDangerInstance> dangerInstanceList;

		private:
			int spawnCount;
			sf::Time timeOfLastChanceCheck;
			sf::Time timeOfLastSpawn;

		public:
			StoryDangerSpawnTracker(const StoryDangerDefn& dangerDefn);

		public:
			const StoryDangerDefn& getDangerDefn() const;
			const std::vector<StoryDangerInstance>& getDangerInstanceList() const;
			StoryDangerInstance getDangerInstance(int dangerInstanceId) const;

		public:
			bool shouldDangerSpawn(const StoryDangerSpawnCheckInput& input);
			StoryDangerPositionCheckResult occupiesPosition(const sf::Vector2i& position) const;
			bool shouldDangerDespawn(const StoryDangerDespawnCheckInput& input) const;

		public:
			void spawnDanger(const StoryDangerInstance& dangerInstance);
			void despawnDanger(int dangerInstanceId);

		private:
			int findDangerInstanceIndex(int dangerInstanceId) const;

		};

		typedef enum class Snake_StoryGameStatus {
			NOT_STARTED,
			RUNNING,
			ENDED,
		} StoryGameStatus;

		class StoryGame {

		private:
			std::default_random_engine randomizer;
			sf::Clock clock;
			StoryGameStatus status;
			sf::Time timeElapsedWhenEnded;

		private:
			const StoryLevelDefn* levelDefn;
			StoryMap* map;
			Snake* snake;
			float snakeSpeedTilesPerSecond;
			float snakeHealth;

		private:
			int framesSinceSnakeMoved;
			std::queue<ObjectDirection> snakeMovementQueue;
			int queuedSnakeGrowth;

		private:
			int nextFoodInstanceId;
			std::vector<StoryFoodSpawnTracker> foodSpawnTrackerList;
			std::unordered_map<int, StoryFoodTileDistanceTracking> foodTileDistanceTrackingMap;
			std::unordered_map<StoryFoodType, int> foodEatenCountMap;

		private:
			int nextDangerInstanceId;
			std::vector<StoryDangerSpawnTracker> dangerSpawnTrackerList;
			std::unordered_map<int, bool> dangerStruckSnakeMap;

		private:
			int score;

		public:
			StoryGame();

		public:
			~StoryGame();

		public:
			void startNewCampaign();
			void startNewLevel(const StoryMapDefn& mapDefn, const StoryLevelDefn& levelDefn);
			void startRunningLevel();
			void stopRunningLevel();

		public:
			StoryMap* getMap() const;
			Snake* getSnake() const;
			float getCurrSnakeHealth() const;
			float getMaxSnakeHealth() const;
			const std::vector<StoryFoodSpawnTracker>& getFoodSpawnTrackerList() const;
			const std::vector<StoryDangerSpawnTracker>& getDangerSpawnTrackerList() const;
			int getFoodEaten(StoryFoodType foodType) const;
			sf::Time getTimeElapsed() const;
			const StoryWinCondition& getWinCondition() const;
			int getScore() const;

		public:
			StoryGameUpdateResult update(const StoryGameInputRequest& input);

		private:
			void freeLevel();

		private:
			void acceptSnakeMovementList(const std::vector<ObjectDirection>& snakeMovementList);
			void consumeAllUnusableSnakeInputs();
			ObjectDirection resolveDirectionToMoveSnake();
			bool snakeWouldHitBarrier(ObjectDirection direction);
			bool moveSnakeForward(ObjectDirection directionToMoveSnake);

		private:
			std::vector<StoryFoodInstance> checkForFoodSpawns();
			StoryCheckForFoodEatenBySnakeResult checkForFoodEatenBySnake();
			std::vector<sf::Vector2i> buildAvailableFoodSpawnPositionList(const StoryFoodDefn& foodDefn);
			bool foodOccupiesPosition(const sf::Vector2i& position);
			StoryFoodInstance createFoodInstance(StoryFoodType foodType, const std::vector<sf::Vector2i>& availablePositionList);

		private:
			void addNewFoodSpawnsToFoodTileDistanceTrackingMap(const std::vector<StoryFoodInstance>& spawnedFoodInstanceList);
			void updateFoodTileDistanceTrackingMapAfterSnakeMoved();
			std::unordered_map<int, StoryFoodEatenBySnakeScoreResult> buildFoodEatenBySnakeScoreResultMap(const std::vector<StoryFoodInstance>& eatenBySnakeFoodInstanceList);

		private:
			std::vector<StoryDangerInstance> checkForDangerSpawns();
			std::vector<StoryDangerInstance> checkForDangersStrikingSnake();
			void checkForDangerDespawns();
			std::vector<sf::Vector2i> buildAvailableDangerSpawnPositionList(const StoryDangerDefn& dangerDefn);
			bool dangerOccupiesPosition(const sf::Vector2i& position);
			StoryDangerInstance createDangerInstance(StoryDangerType dangerType, const std::vector<sf::Vector2i>& availablePositionList);

		private:
			void addNewDangerSpawnsToDangerStruckSnakeMap(const std::vector<StoryDangerInstance>& spawnedDangerInstanceList);

		private:
			void updateHealthBy(float amount);

		private:
			bool checkLevelCompleted();

		};

		typedef enum class Snake_StoryCutsceneScreenViewType {
			COLOR,
			TEXTURE,
			MAP,
		} StoryCutsceneScreenViewType;

		typedef struct Snake_StoryCutsceneScreenView {
			StoryCutsceneScreenViewType screenEventType;
			int fadeFrames;
			int fadeFramesRemaining;
			sf::Color color;
			std::string textureFilename;
			std::string mapFilename;
		} StoryCutsceneScreenView;

		class StoryCutscene {

		private:
			const StoryCutsceneDefn* cutsceneDefn;

		private:
			int currFrame;
			int framesSinceLastEvent;
			size_t nextEventIndex;

		private:
			std::vector<StoryCutsceneScreenView> activeScreenViewList;

		public:
			StoryCutscene(const StoryCutsceneDefn& cutsceneDefn);

		public:
			const std::vector<StoryCutsceneScreenView>& getActiveScreenViewList() const;

		public:
			bool update();

		private:
			void updateActiveScreenViews();

		private:
			void processEvents();
			void processEvent(const StoryCutsceneEventDefn& eventDefn);

		private:
			void processColorEvent(const StoryCutsceneColorEventDefn& colorEventDefn);
			void processTextureEvent(const StoryCutsceneTextureEventDefn& textureEventDefn);
			void processShowMapEvent(const StoryCutsceneMapEventDefn& showMapEventDefn);
			void addScreenView(const StoryCutsceneScreenView& screenView);

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
			r3::sound::SimpleSoundManager soundManager;
			StoryGame* storyGame;
			StoryCutscene* storyCutscene;

		private:
			std::vector<ObjectDirection> snakeMovementInputQueue;

		private:
			sf::Time timeSnakeLastDamaged;

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
			StoryGameSceneClientRequest processLevelLostKeyEvent(sf::Event& event);
			StoryGameSceneClientRequest processCampaignWonKeyEvent(sf::Event& event);

		private:
			void updateCutscene();
			void updateGameRunning();
			
		};

		typedef struct Snake_StoryGameRenderState {
			const StoryLevelAssetBundle* levelAssetBundle;
			const StoryGame* storyGame;
			bool snakeDamagedFlag;
		} StoryGameRenderState;

		typedef struct Snake_StoryCutsceneRenderState {
			const StoryLevelAssetBundle* levelAssetBundle;
			const StoryCutscene* storyCutscene;
		} StoryCutsceneRenderState;

		typedef struct Snake_StoryFoodEatenAnimation {
			sf::Vector2i tilePosition;
			sf::Time eatenTime;
			StoryFoodEatenBySnakeScoreResult scoreResult;
		} StoryFoodEatenAnimation;

		class StoryGameRenderer {

		private:
			sf::Font* uiFont;
			sf::Texture* healthBarTexture;

		private:
			std::vector<StoryFoodEatenAnimation> foodEatenAnimationList;

		public:
			StoryGameRenderer();

		public:
			~StoryGameRenderer();

		public:
			void clearAnimations();
			void queueFoodEatenAnimations(const std::vector<StoryFoodEatenResult>& foodEatenResultList, const StoryGame* storyGame);

		public:
			void renderLoadCampaignError(sf::RenderTarget& renderTarget);
			void renderLoadLevelStatus(sf::RenderTarget& renderTarget, const StoryLevelAssetLoadingStatus& assetLoadingStatus);
			void renderLoadLevelError(sf::RenderTarget& renderTarget);
			void renderCutscene(sf::RenderTarget& renderTarget, const StoryCutsceneRenderState& renderState);
			void renderWaitToStart(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderGameRunning(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderLevelLost(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderCampaignWon(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);

		private:
			void renderGameRunningUi(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderPlayingField(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderSnake(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderFoodSpawns(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderDangerSpawns(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderFoodEatenAnimations(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderWaitToStartInstructions(sf::RenderTarget& renderTarget);
			void renderExitInstructions(sf::RenderTarget& renderTarget);

		private:
			sf::RectangleShape createTextBackgroundShape(float width, float height);
			sf::Text createInstructionsText(const wchar_t* textString, float y);

		};

	}

}
