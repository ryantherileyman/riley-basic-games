
#include <random>
#include <queue>
#include <unordered_set>
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
			PLAY_WIN_CUTSCENE,
			PLAY_LOSS_CUTSCENE,
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
			const Snake* snake;
			float snakeHealth;
			const std::unordered_map<StoryFoodType, int>* foodEatenCountMap;
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
			const Snake* snake;
		} StoryDangerSpawnCheckInput;

		typedef struct Snake_StoryDangerPositionCheckResult {
			bool dangerExistsFlag;
			int dangerInstanceId;
		} StoryDangerPositionCheckResult;

		typedef struct Snake_StoryDangerDespawnCheckInput {
			int dangerInstanceId;
			sf::Time timeSinceLevelStarted;
		} StoryDangerDespawnCheckInput;

		typedef struct Snake_StorySnakeMovementModifier {
			float movementMultiplier;
			float secondsToModify;
			sf::Time timeModifierBegan;
		} StorySnakeMovementModifier;

		typedef struct Snake_StoryGameUpdateResult {
			ObjectDirection snakeMovementResult;
			bool snakeHitBarrierFlag;
			bool snakeDiedFlag;
			bool snakeGrewFlag;
			bool snakeShrunkFlag;
			bool snakeShrunkTooSmallFlag;
			bool completedLevelFlag;
			std::vector<StoryFoodInstance> spawnedFoodInstanceList;
			std::vector<StoryDangerInstance> spawnedDangerInstanceList;
			std::vector<StoryFoodEatenResult> foodEatenResultList;
			std::vector<StoryDangerInstance> dangerInstanceStruckSnakeList;
			std::vector<std::string> soundFileTriggeredList;

			bool snakeDamaged() const {
				bool result =
					snakeHitBarrierFlag ||
					snakeShrunkTooSmallFlag ||
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
			bool areDespawnConditionsMet(const StoryFoodSpawnCheckInput& input);
			bool areTimerConditionsMet(const StoryFoodSpawnCheckInput& input);
			bool areLengthReachedConditionsMet(const StoryFoodSpawnCheckInput& input);
			bool areHealthFellConditionsMet(const StoryFoodSpawnCheckInput& input);
			bool areSnakePositionConditionsMet(const StoryFoodSpawnCheckInput& input);
			bool areFoodEatenConditionsMet(const StoryFoodSpawnCheckInput& input);

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
			bool areTimerConditionsMet(const StoryDangerSpawnCheckInput& input);
			bool areLengthReachedConditionsMet(const StoryDangerSpawnCheckInput& input);
			bool areSnakePositionConditionsMet(const StoryDangerSpawnCheckInput& input);

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
			float snakeHealth;

		private:
			int framesSinceSnakeMoved;
			std::queue<ObjectDirection> snakeMovementQueue;
			int queuedSnakeGrowth;

		private:
			int nextSnakeMovementModifierId;
			std::unordered_map<int, StorySnakeMovementModifier> snakeMovementModifierMap;

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
			std::unordered_set<const StorySoundFxDefn*> remainingSoundFxDefnSet;

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
			void moveSnakeForward(ObjectDirection directionToMoveSnake);

		private:
			void updateSnakeMovementModifierMap();
			float resolveSnakeSpeedTilesPerSecond();
			void addNewSnakeMovementModifier(float movementMultiplier, float secondsToApply);

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
			std::vector<std::string> buildSoundFileTriggeredList(const StoryGameUpdateResult& updateResult);

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
			sf::Clock clock;
			int currFrame;
			int lastEventFrame;
			int framesSinceLastEvent;
			size_t nextEventIndex;

		private:
			std::vector<StoryCutsceneScreenView> activeScreenViewList;
			Snake* snake;
			std::unordered_map<int, StoryFoodInstance> foodInstanceMap;
			std::unordered_map<int, StoryDangerInstance> dangerInstanceMap;

		public:
			StoryCutscene(const StoryCutsceneDefn& cutsceneDefn);

		public:
			~StoryCutscene();

		public:
			int getCurrFrame() const;
			float getSecondsElapsed() const;
			const std::vector<StoryCutsceneScreenView>& getActiveScreenViewList() const;
			const Snake* getSnake() const;
			const std::unordered_map<int, StoryFoodInstance>& getFoodInstanceMap() const;
			const std::unordered_map<int, StoryDangerInstance>& getDangerInstanceMap() const;

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

		private:
			void processShowSnakeEvent(const StoryCutsceneSnakeEventDefn& snakeEventDefn);
			void processMoveSnakeEvent(const StoryCutsceneSnakeEventDefn& snakeEventDefn);
			void processHideSnakeEvent(const StoryCutsceneSnakeEventDefn& snakeEventDefn);

		private:
			void processShowFoodEvent(const StoryCutsceneFoodEventDefn& foodEventDefn);
			void processHideFoodEvent(const StoryCutsceneFoodEventDefn& foodEventDefn);

		private:
			void processShowDangerEvent(const StoryCutsceneDangerEventDefn& dangerEventDefn);
			void processHideDangerEvent(const StoryCutsceneDangerEventDefn& dangerEventDefn);

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
			StoryGameSceneClientRequest processPlayOpeningCutsceneKeyEvent(sf::Event& event);
			StoryGameSceneClientRequest processWaitToStartKeyEvent(sf::Event& event);
			StoryGameSceneClientRequest processGameRunningKeyEvent(sf::Event& event);
			StoryGameSceneClientRequest processPlayWinCutsceneKeyEvent(sf::Event& event);
			StoryGameSceneClientRequest processPlayLossCutsceneKeyEvent(sf::Event& event);
			StoryGameSceneClientRequest processLevelLostKeyEvent(sf::Event& event);
			StoryGameSceneClientRequest processCampaignWonKeyEvent(sf::Event& event);

		private:
			void updateOpeningCutscene();
			void updateWinCutscene();
			void updateLossCutscene();
			void updateGameRunning();
		
		private:
			void stopRunningLevel();
			void moveToNextLevel();
			
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
			void renderCutscenePlayingField(sf::RenderTarget& renderTarget, const StoryLevelAssetBundle& levelAssetBundle, const StoryCutsceneScreenView& screenView);
			void renderCutsceneSnake(sf::RenderTarget& renderTarget, const StoryCutsceneRenderState& renderState);
			void renderCutsceneFood(sf::RenderTarget& renderTarget, const StoryCutsceneRenderState& renderState);
			void renderCutsceneDangers(sf::RenderTarget& renderTarget, const StoryCutsceneRenderState& renderState);
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
