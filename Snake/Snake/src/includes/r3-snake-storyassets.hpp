
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "r3-snake-storydefn.hpp"
#include "r3-snake-storyloader.hpp"
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

		typedef struct Snake_StoryMapAssetBundle {
			StoryMapDefn mapDefn;
			LoadStoryMapValidationResult loadMapValidationResult;
			std::unordered_map<int, sf::Texture*> floorTextureRefMap;
			std::unordered_map<int, sf::Texture*> barrierTextureRefMap;
		} StoryMapAssetBundle;

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
			sf::Texture snakeTexture;
			sf::Texture foodTexture;
			sf::Texture dangerTexture;
			std::unordered_map<std::string, sf::Texture> textureMap;

		private:
			StoryMapAssetBundle mapAssetBundle;
			std::unordered_map<std::string, StoryMapAssetBundle> cutsceneMapAssetBundleMap;

		private:
			sf::Music music;
			sf::SoundBuffer foodSpawnedSoundBuffer;
			sf::SoundBuffer eatFoodSoundBuffer;
			sf::SoundBuffer hitBarrierSoundBuffer;
			sf::SoundBuffer snakeHissSoundBuffer;

		public:
			StoryLevelAssetBundle();

		public:
			~StoryLevelAssetBundle();

		public:
			void loadLevelAsync(const std::string& campaignFolderName, StoryLevelDefn& levelDefn);
			StoryLevelAssetLoadingStatus getLoadingStatus();

		public:
			LoadStoryMapValidationResult getLoadMapValidationResult() const;
			std::vector<std::string> getFailedFilenameList() const;

		public:
			const StoryMapDefn& getMapDefn() const;
			const sf::Texture& getSnakeTexture() const;
			const sf::Texture& getFoodTexture() const;
			const sf::Texture& getDangerTexture() const;
			const sf::Texture& getFloorTexture(int floorId) const;
			const sf::Texture& getBarrierTexture(int barrierId) const;

		public:
			sf::Music& getMusic();
			const sf::SoundBuffer& getFoodSpawnedSoundBuffer() const;
			const sf::SoundBuffer& getEatFoodSoundBuffer() const;
			const sf::SoundBuffer& getHitBarrierSoundBuffer() const;
			const sf::SoundBuffer& getSnakeHissSoundBuffer() const;

		private:
			void loadLevel();
			LoadStoryMapResult loadMap(const std::string& filename);

		private:
			void loadSnakeTexture();
			void loadFoodTexture();
			void loadDangerTexture();
			void loadFloorTextureMap(const StoryMapDefn& mapDefn);
			void loadBarrierTextureMap(const StoryMapDefn& mapDefn);
			void loadMapTexture(const std::string& filename);

		private:
			void loadMusic();
			void loadFoodSpawnedSoundBuffer();
			void loadEatFoodSoundBuffer();
			void loadHitBarrierSoundBuffer();
			void loadSnakeHissSoundBuffer();

		private:
			void indicateLoadingFilename(const std::string& filename);
			void updateTotalAssetCount(const StoryMapDefn& mapDefn);
			void incrementLoadedAssetCount();
			void indicateLoadingComplete();

		};

	}

}
