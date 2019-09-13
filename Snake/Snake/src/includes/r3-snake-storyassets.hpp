
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "r3-snake-storydefn.hpp"
#pragma once

namespace r3 {

	namespace snake {

		typedef enum class Snake_StoryLevelAssetLoadingCompletionStatus {
			PENDING,
			LOADING,
			COMPLETE,
			FAILED,
		} StoryLevelAssetLoadingCompletionStatus;

		typedef struct Snake_StoryLevelAssetLoadingStatus {
			StoryLevelAssetLoadingCompletionStatus completionStatus;
			float loadedPct;
			std::string currFilename;
		} StoryLevelAssetLoadingStatus;

		class StoryLevelAssetBundle {

		private:
			sf::Mutex loadingStatusMutex;
			sf::Thread* loadingThread;
			std::string campaignFolderName;
			StoryLevelDefn* levelDefn;

		private:
			StoryLevelAssetLoadingCompletionStatus loadingCompletionStatus;
			int totalAssetCount;
			int loadedAssetCount;
			std::string currFilenameBeingLoaded;

		private:
			std::vector<std::string> failedFilenameList;

		private:
			StoryMapDefn mapDefn;
			sf::Texture snakeTexture;
			sf::Texture foodTexture;
			std::unordered_map<int, sf::Texture> floorTextureMap;
			std::unordered_map<int, sf::Texture> barrierTextureMap;

		private:
			sf::Music music;
			sf::SoundBuffer eatFoodSoundBuffer;
			sf::SoundBuffer hitBarrierSoundBuffer;

		public:
			StoryLevelAssetBundle();

		public:
			~StoryLevelAssetBundle();

		public:
			void loadLevelAsync(const std::string& campaignFolderName, StoryLevelDefn& levelDefn);
			StoryLevelAssetLoadingStatus getLoadingStatus();

		public:
			std::vector<std::string> getFailedFilenameList();

		public:
			const StoryMapDefn& getMapDefn();
			const sf::Texture& getSnakeTexture();
			const sf::Texture& getFoodTexture();
			const sf::Texture& getFloorTexture(int floorId);
			const sf::Texture& getBarrierTexture(int barrierId);

		public:
			const sf::Music& getMusic();
			const sf::SoundBuffer& getEatFoodSoundBuffer();
			const sf::SoundBuffer& getHitBarrierSoundBuffer();

		private:
			void loadLevel();

		private:
			void loadSnakeTexture();
			void loadFoodTexture();
			void loadFloorTextureMap(const StoryMapDefn& mapDefn);
			void loadBarrierTextureMap(const StoryMapDefn& mapDefn);

		private:
			void loadMusic();
			void loadEatFoodSoundBuffer();
			void loadHitBarrierSoundBuffer();

		private:
			void indicateLoadingFilename(const std::string& filename);
			void updateTotalAssetCount(const StoryMapDefn& mapDefn);
			void incrementLoadedAssetCount();
			void indicateLoadingComplete();

		};

	}

}
