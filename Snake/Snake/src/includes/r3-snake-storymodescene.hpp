
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
		} StoryGameMode;

		typedef enum class Snake_StoryGameSceneClientRequest {
			NONE,
			EXIT_GAME,
			RETURN_TO_SPLASH_SCREEN,
		} StoryGameSceneClientRequest;

		typedef struct Snake_StoryGameRenderState {
			const StoryLevelAssetBundle* levelAssetBundle;
			const StoryLevel* level;
		} StoryGameRenderState;

		class StoryGameController;
		class StoryGameRenderer;

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
			StoryLevel* level;

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
			void renderLoadCampaignError(sf::RenderTarget& renderTarget);
			void renderLoadLevelStatus(sf::RenderTarget& renderTarget, const StoryLevelAssetLoadingStatus& assetLoadingStatus);
			void renderLoadLevelError(sf::RenderTarget& renderTarget);
			void renderWaitToStart(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);

		private:
			void renderGameRunningUi(sf::RenderTarget& renderTarget);
			void renderPlayingField(sf::RenderTarget& renderTarget, const StoryGameRenderState& renderState);
			void renderWaitToStartInstructions(sf::RenderTarget& renderTarget);

		private:
			sf::RectangleShape createTextBackgroundShape(float width, float height);
			sf::Text createInstructionsText(const wchar_t* textString, float y);

		};

	}

}
