
#include <assert.h>
#include "../includes/r3-snake-storyassets.hpp"
#include "../includes/r3-snake-storyloader.hpp"

namespace r3 {

	namespace snake {

		namespace StoryLevelAssetBundleConstants {

			const char* DEFAULT_SNAKE_TEXTURE_PATH = "resources/textures/snake-tileset.png";
			const char* DEFAULT_FOOD_TEXTURE_PATH = "resources/textures/food-tileset.png";

			const char* DEFAULT_EAT_FOOD_SOUND_PATH = "resources/sounds/eat-apple.wav";
			const char* DEFAULT_HIT_BARRIER_SOUND_PATH = "resources/sounds/hit-barrier.wav";

		}

		StoryLevelAssetBundle::StoryLevelAssetBundle() {
			this->levelDefn = { nullptr };
			this->loadingThread = { nullptr };
			this->loadingCompletionStatus = StoryLevelAssetLoadingCompletionStatus::PENDING;
			this->totalAssetCount = 0;
			this->loadedAssetCount = 0;
		}

		StoryLevelAssetBundle::~StoryLevelAssetBundle() {
			if (this->loadingThread != nullptr) {
				delete this->loadingThread;
			}
		}

		void StoryLevelAssetBundle::loadLevelAsync(const std::string& campaignFolderName, StoryLevelDefn& levelDefn) {
			assert( this->loadingCompletionStatus == StoryLevelAssetLoadingCompletionStatus::PENDING );

			this->campaignFolderName = campaignFolderName;
			this->levelDefn = &levelDefn;
			this->loadingCompletionStatus = StoryLevelAssetLoadingCompletionStatus::LOADING;
			this->totalAssetCount = 5;
			if (!levelDefn.musicFilename.empty()) {
				this->totalAssetCount++;
			}

			this->loadingThread = new sf::Thread(&StoryLevelAssetBundle::loadLevel, this);
			this->loadingThread->launch();
		}

		StoryLevelAssetLoadingStatus StoryLevelAssetBundle::getLoadingStatus() {
			StoryLevelAssetLoadingStatus result;

			this->loadingStatusMutex.lock();

			result.completionStatus = this->loadingCompletionStatus;
			result.loadedPct = (float)this->loadedAssetCount / (float)this->totalAssetCount;
			result.currFilename = this->currFilenameBeingLoaded;

			this->loadingStatusMutex.unlock();

			return result;
		}

		std::vector<std::string> StoryLevelAssetBundle::getFailedFilenameList() const {
			std::vector<std::string> result = this->failedFilenameList;
			return result;
		}

		const StoryMapDefn& StoryLevelAssetBundle::getMapDefn() const {
			return this->mapDefn;
		}

		const sf::Texture& StoryLevelAssetBundle::getSnakeTexture() const {
			return this->snakeTexture;
		}

		const sf::Texture& StoryLevelAssetBundle::getFoodTexture() const {
			return this->foodTexture;
		}

		const sf::Texture& StoryLevelAssetBundle::getFloorTexture(int floorId) const {
			assert( this->floorTextureMap.count(floorId) == 1 );

			const sf::Texture& result = this->floorTextureMap.at(floorId);
			return result;
		}

		const sf::Texture& StoryLevelAssetBundle::getBarrierTexture(int barrierId) const {
			assert( this->barrierTextureMap.count(barrierId) == 1 );

			const sf::Texture& result = this->barrierTextureMap.at(barrierId);
			return result;
		}

		const sf::Music& StoryLevelAssetBundle::getMusic() const {
			return this->music;
		}

		const sf::SoundBuffer& StoryLevelAssetBundle::getEatFoodSoundBuffer() const {
			return this->eatFoodSoundBuffer;
		}

		const sf::SoundBuffer& StoryLevelAssetBundle::getHitBarrierSoundBuffer() const {
			return this->hitBarrierSoundBuffer;
		}

		void StoryLevelAssetBundle::loadLevel() {
			this->indicateLoadingFilename(this->levelDefn->mapFilename);
			LoadStoryMapResult loadStoryMapResult = StoryLoaderUtils::loadStoryMap(this->campaignFolderName, this->levelDefn->mapFilename);
			if (loadStoryMapResult.validationResult.valid()) {
				this->updateTotalAssetCount(loadStoryMapResult.mapDefn);
				this->incrementLoadedAssetCount();

				this->mapDefn = loadStoryMapResult.mapDefn;

				this->loadSnakeTexture();
				this->loadFoodTexture();
				this->loadFloorTextureMap(loadStoryMapResult.mapDefn);
				this->loadBarrierTextureMap(loadStoryMapResult.mapDefn);

				this->loadMusic();
				this->loadEatFoodSoundBuffer();
				this->loadHitBarrierSoundBuffer();
			}
			else {
				this->failedFilenameList.push_back(this->levelDefn->mapFilename);
			}

			this->indicateLoadingComplete();
		}

		void StoryLevelAssetBundle::loadSnakeTexture() {
			this->indicateLoadingFilename(StoryLevelAssetBundleConstants::DEFAULT_SNAKE_TEXTURE_PATH);

			if (this->snakeTexture.loadFromFile(StoryLevelAssetBundleConstants::DEFAULT_SNAKE_TEXTURE_PATH)) {
				this->incrementLoadedAssetCount();
			}
			else {
				this->failedFilenameList.push_back(StoryLevelAssetBundleConstants::DEFAULT_SNAKE_TEXTURE_PATH);
			}
		}

		void StoryLevelAssetBundle::loadFoodTexture() {
			this->indicateLoadingFilename(StoryLevelAssetBundleConstants::DEFAULT_FOOD_TEXTURE_PATH);

			if (this->foodTexture.loadFromFile(StoryLevelAssetBundleConstants::DEFAULT_FOOD_TEXTURE_PATH)) {
				this->incrementLoadedAssetCount();
			}
			else {
				this->failedFilenameList.push_back(StoryLevelAssetBundleConstants::DEFAULT_FOOD_TEXTURE_PATH);
			}
		}

		void StoryLevelAssetBundle::loadFloorTextureMap(const StoryMapDefn& mapDefn) {
			for (auto const& currFloorDefnPair : mapDefn.floorDefnMap) {
				this->indicateLoadingFilename(currFloorDefnPair.second.filename);

				this->floorTextureMap[currFloorDefnPair.first] = sf::Texture();
				if (this->floorTextureMap[currFloorDefnPair.first].loadFromFile(r3::snake::StoryLoaderUtils::resolveImageFilePath(this->campaignFolderName, currFloorDefnPair.second.filename))) {
					this->floorTextureMap[currFloorDefnPair.first].setRepeated(true);
					this->incrementLoadedAssetCount();
				}
				else {
					this->failedFilenameList.push_back(currFloorDefnPair.second.filename);
				}
			}
		}

		void StoryLevelAssetBundle::loadBarrierTextureMap(const StoryMapDefn& mapDefn) {
			for (auto const& currBarrierDefnPair : mapDefn.barrierDefnMap) {
				this->indicateLoadingFilename(currBarrierDefnPair.second.filename);

				this->barrierTextureMap[currBarrierDefnPair.first] = sf::Texture();
				if (this->barrierTextureMap[currBarrierDefnPair.first].loadFromFile(r3::snake::StoryLoaderUtils::resolveImageFilePath(this->campaignFolderName, currBarrierDefnPair.second.filename))) {
					this->barrierTextureMap[currBarrierDefnPair.first].setRepeated(true);
					this->incrementLoadedAssetCount();
				}
				else {
					this->failedFilenameList.push_back(currBarrierDefnPair.second.filename);
				}
			}
		}

		void StoryLevelAssetBundle::loadMusic() {
			if (this->music.openFromFile(r3::snake::StoryLoaderUtils::resolveMusicFilePath(this->campaignFolderName, this->levelDefn->musicFilename))) {
				this->incrementLoadedAssetCount();
			}
			else {
				this->failedFilenameList.push_back(this->levelDefn->musicFilename);
			}
		}

		void StoryLevelAssetBundle::loadEatFoodSoundBuffer() {
			this->indicateLoadingFilename(StoryLevelAssetBundleConstants::DEFAULT_EAT_FOOD_SOUND_PATH);

			if (this->eatFoodSoundBuffer.loadFromFile(StoryLevelAssetBundleConstants::DEFAULT_EAT_FOOD_SOUND_PATH)) {
				this->incrementLoadedAssetCount();
			}
			else {
				this->failedFilenameList.push_back(StoryLevelAssetBundleConstants::DEFAULT_EAT_FOOD_SOUND_PATH);
			}
		}

		void StoryLevelAssetBundle::loadHitBarrierSoundBuffer() {
			this->indicateLoadingFilename(StoryLevelAssetBundleConstants::DEFAULT_HIT_BARRIER_SOUND_PATH);

			if (this->hitBarrierSoundBuffer.loadFromFile(StoryLevelAssetBundleConstants::DEFAULT_HIT_BARRIER_SOUND_PATH)) {
				this->incrementLoadedAssetCount();
			}
			else {
				this->failedFilenameList.push_back(StoryLevelAssetBundleConstants::DEFAULT_HIT_BARRIER_SOUND_PATH);
			}
		}

		void StoryLevelAssetBundle::indicateLoadingFilename(const std::string& filename) {
			this->loadingStatusMutex.lock();

			this->currFilenameBeingLoaded = filename;

			this->loadingStatusMutex.unlock();
		}

		void StoryLevelAssetBundle::incrementLoadedAssetCount() {
			this->loadingStatusMutex.lock();

			this->loadedAssetCount++;

			this->loadingStatusMutex.unlock();
		}

		void StoryLevelAssetBundle::updateTotalAssetCount(const StoryMapDefn& mapDefn) {
			this->loadingStatusMutex.lock();

			this->totalAssetCount += ( mapDefn.floorDefnMap.size() + mapDefn.barrierDefnMap.size() );

			this->loadingStatusMutex.unlock();
		}

		void StoryLevelAssetBundle::indicateLoadingComplete() {
			this->loadingStatusMutex.lock();

			if (this->failedFilenameList.size() == 0) {
				this->loadingCompletionStatus = StoryLevelAssetLoadingCompletionStatus::COMPLETE;
			} else {
				this->loadingCompletionStatus = StoryLevelAssetLoadingCompletionStatus::FAILED;
			}

			this->loadingStatusMutex.unlock();
		}

	}

}
