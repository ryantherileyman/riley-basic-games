
#include <assert.h>
#include <string>
#include "../includes/r3-snake-storyassets.hpp"
#include "../includes/r3-snake-storyloader.hpp"

namespace r3 {

	namespace snake {

		namespace StoryLevelAssetBundleConstants {

			const char* DEFAULT_SNAKE_TEXTURE_PATH = "resources/textures/snake-tileset.png";
			const char* DEFAULT_FOOD_TEXTURE_PATH = "resources/textures/food-tileset.png";
			const char* DEFAULT_DANGER_TEXTURE_PATH = "resources/textures/danger-tileset.png";

			const char* DEFAULT_FOOD_SPAWNED_SOUND_PATH = "resources/sounds/food-spawned.wav";
			const char* DEFAULT_EAT_FOOD_SOUND_PATH = "resources/sounds/eat-apple.wav";
			const char* DEFAULT_HIT_BARRIER_SOUND_PATH = "resources/sounds/hit-barrier.wav";
			const char* DEFAULT_SNAKE_HISS_SOUND_PATH = "resources/sounds/snake-hiss.wav";

		}

		namespace StoryLevelAssetBundleUtils {

			int resolveCutsceneAssetCount(const StoryCutsceneDefn& cutsceneDefn) {
				int result = 0;

				if (!cutsceneDefn.soundTrackFilename.empty()) {
					result++;
				}

				for (auto const& currEventDefn : cutsceneDefn.eventDefnList) {
					if (
						(currEventDefn.eventType == StoryCutsceneEventType::TEXTURE) ||
						(currEventDefn.eventType == StoryCutsceneEventType::SHOW_MAP)
					) {
						result++;
					}
				}

				return result;
			}

			int resolveLevelAssetCount(const StoryLevelDefn& levelDefn) {
				int result = 8;
				if (!levelDefn.musicFilename.empty()) {
					result++;
				}

				if (levelDefn.openingCutsceneDefn.existsFlag) {
					result += resolveCutsceneAssetCount(levelDefn.openingCutsceneDefn);
				}

				if (levelDefn.winCutsceneDefn.existsFlag) {
					result += resolveCutsceneAssetCount(levelDefn.winCutsceneDefn);
				}

				if (levelDefn.lossCutsceneDefn.existsFlag) {
					result += resolveCutsceneAssetCount(levelDefn.lossCutsceneDefn);
				}

				return result;
			}

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
			this->totalAssetCount = StoryLevelAssetBundleUtils::resolveLevelAssetCount(levelDefn);

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

		LoadStoryMapValidationResult StoryLevelAssetBundle::getLoadMapValidationResult() const {
			return this->mapAssetBundle.loadMapValidationResult;
		}

		std::vector<std::string> StoryLevelAssetBundle::getFailedFilenameList() const {
			std::vector<std::string> result = this->failedFilenameList;
			return result;
		}

		const StoryMapDefn& StoryLevelAssetBundle::getMapDefn() const {
			return this->mapAssetBundle.mapDefn;
		}

		const sf::Texture& StoryLevelAssetBundle::getTexture(const std::string& filename) const {
			return this->textureMap.at(filename);
		}

		const sf::Texture& StoryLevelAssetBundle::getSnakeTexture() const {
			return this->snakeTexture;
		}

		const sf::Texture& StoryLevelAssetBundle::getFoodTexture() const {
			return this->foodTexture;
		}

		const sf::Texture& StoryLevelAssetBundle::getDangerTexture() const {
			return this->dangerTexture;
		}

		const sf::Texture& StoryLevelAssetBundle::getFloorTexture(int floorId) const {
			assert( this->mapAssetBundle.floorTextureRefMap.count(floorId) == 1 );

			const sf::Texture& result = *this->mapAssetBundle.floorTextureRefMap.at(floorId);
			return result;
		}

		const sf::Texture& StoryLevelAssetBundle::getBarrierTexture(int barrierId) const {
			assert( this->mapAssetBundle.barrierTextureRefMap.count(barrierId) == 1 );

			const sf::Texture& result = *this->mapAssetBundle.barrierTextureRefMap.at(barrierId);
			return result;
		}

		const StoryMapDefn& StoryLevelAssetBundle::getCutsceneMapDefn(const std::string& mapFilename) const {
			const StoryMapDefn& result = this->cutsceneMapAssetBundleMap.at(mapFilename).mapDefn;
			return result;
		}

		const sf::Texture& StoryLevelAssetBundle::getCutsceneMapFloorTexture(const std::string& mapFilename, int floorId) const {
			const sf::Texture& result = *this->cutsceneMapAssetBundleMap.at(mapFilename).floorTextureRefMap.at(floorId);
			return result;
		}

		const sf::Texture& StoryLevelAssetBundle::getCutsceneMapBarrierTexture(const std::string& mapFilename, int barrierId) const {
			const sf::Texture& result = *this->cutsceneMapAssetBundleMap.at(mapFilename).barrierTextureRefMap.at(barrierId);
			return result;
		}

		sf::Music& StoryLevelAssetBundle::getMusic() {
			return this->music;
		}

		sf::Music& StoryLevelAssetBundle::getOpeningCutsceneMusic() {
			return this->openingCutsceneMusic;
		}

		sf::Music& StoryLevelAssetBundle::getWinCutsceneMusic() {
			return this->winCutsceneMusic;
		}

		sf::Music& StoryLevelAssetBundle::getLossCutsceneMusic() {
			return this->lossCutsceneMusic;
		}

		const sf::SoundBuffer& StoryLevelAssetBundle::getFoodSpawnedSoundBuffer() const {
			return this->foodSpawnedSoundBuffer;
		}

		const sf::SoundBuffer& StoryLevelAssetBundle::getEatFoodSoundBuffer() const {
			return this->eatFoodSoundBuffer;
		}

		const sf::SoundBuffer& StoryLevelAssetBundle::getHitBarrierSoundBuffer() const {
			return this->hitBarrierSoundBuffer;
		}

		const sf::SoundBuffer& StoryLevelAssetBundle::getSnakeHissSoundBuffer() const {
			return this->snakeHissSoundBuffer;
		}

		void StoryLevelAssetBundle::loadLevel() {
			LoadStoryMapResult loadMainMapResult = this->loadMap(this->levelDefn->mapFilename);
			this->mapAssetBundle.mapDefn = loadMainMapResult.mapDefn;
			this->mapAssetBundle.loadMapValidationResult = loadMainMapResult.validationResult;

			if (this->levelDefn->openingCutsceneDefn.existsFlag) {
				this->loadCutsceneSoundTrack(this->openingCutsceneMusic, this->levelDefn->openingCutsceneDefn);
				this->loadCutsceneTextures(this->levelDefn->openingCutsceneDefn);
				this->loadCutsceneMaps(this->levelDefn->openingCutsceneDefn);
			}

			if (this->levelDefn->winCutsceneDefn.existsFlag) {
				this->loadCutsceneSoundTrack(this->winCutsceneMusic, this->levelDefn->winCutsceneDefn);
				this->loadCutsceneTextures(this->levelDefn->winCutsceneDefn);
				this->loadCutsceneMaps(this->levelDefn->winCutsceneDefn);
			}

			if (this->levelDefn->lossCutsceneDefn.existsFlag) {
				this->loadCutsceneSoundTrack(this->lossCutsceneMusic, this->levelDefn->lossCutsceneDefn);
				this->loadCutsceneTextures(this->levelDefn->lossCutsceneDefn);
				this->loadCutsceneMaps(this->levelDefn->lossCutsceneDefn);
			}

			if (loadMainMapResult.validationResult.valid()) {
				this->loadSnakeTexture();
				this->loadFoodTexture();
				this->loadDangerTexture();
				this->loadFloorTextureMap(this->mapAssetBundle);
				this->loadBarrierTextureMap(this->mapAssetBundle);

				this->loadMusic();
				this->loadFoodSpawnedSoundBuffer();
				this->loadEatFoodSoundBuffer();
				this->loadHitBarrierSoundBuffer();
				this->loadSnakeHissSoundBuffer();
			}

			this->indicateLoadingComplete();
		}

		void StoryLevelAssetBundle::loadCutsceneTextures(const StoryCutsceneDefn& cutsceneDefn) {
			for (auto const& currEventDefn : cutsceneDefn.eventDefnList) {
				if (currEventDefn.eventType == StoryCutsceneEventType::TEXTURE) {
					this->loadTexture(currEventDefn.textureEvent.textureFilename, false);
				}
			}
		}

		void StoryLevelAssetBundle::loadCutsceneMaps(const StoryCutsceneDefn& cutsceneDefn) {
			for (auto const& currEventDefn : cutsceneDefn.eventDefnList) {
				if (currEventDefn.eventType == StoryCutsceneEventType::SHOW_MAP) {
					if (this->cutsceneMapAssetBundleMap.count(currEventDefn.mapEvent.mapFilename) == 0) {
						LoadStoryMapResult currLoadStoryMapResult = this->loadMap(currEventDefn.mapEvent.mapFilename);

						this->cutsceneMapAssetBundleMap[currEventDefn.mapEvent.mapFilename] = StoryMapAssetBundle();
						this->cutsceneMapAssetBundleMap[currEventDefn.mapEvent.mapFilename].mapDefn = currLoadStoryMapResult.mapDefn;
						this->cutsceneMapAssetBundleMap[currEventDefn.mapEvent.mapFilename].loadMapValidationResult = currLoadStoryMapResult.validationResult;

						if (currLoadStoryMapResult.validationResult.valid()) {
							this->loadFloorTextureMap(this->cutsceneMapAssetBundleMap[currEventDefn.mapEvent.mapFilename]);
							this->loadBarrierTextureMap(this->cutsceneMapAssetBundleMap[currEventDefn.mapEvent.mapFilename]);
						}
					}
				}
			}
		}

		LoadStoryMapResult StoryLevelAssetBundle::loadMap(const std::string& filename) {
			this->indicateLoadingFilename(filename);

			LoadStoryMapResult result = StoryLoaderUtils::loadStoryMap(this->campaignFolderName, filename);
			if (result.validationResult.valid()) {
				this->updateTotalAssetCount(result.mapDefn);
				this->incrementLoadedAssetCount();
			}
			else {
				this->failedFilenameList.push_back(filename);
			}

			return result;
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

		void StoryLevelAssetBundle::loadDangerTexture() {
			this->indicateLoadingFilename(StoryLevelAssetBundleConstants::DEFAULT_DANGER_TEXTURE_PATH);

			if (this->dangerTexture.loadFromFile(StoryLevelAssetBundleConstants::DEFAULT_DANGER_TEXTURE_PATH)) {
				this->incrementLoadedAssetCount();
			}
			else {
				this->failedFilenameList.push_back(StoryLevelAssetBundleConstants::DEFAULT_DANGER_TEXTURE_PATH);
			}
		}

		void StoryLevelAssetBundle::loadFloorTextureMap(StoryMapAssetBundle& mapAssetBundle) {
			for (auto const& currFloorDefnPair : mapAssetBundle.mapDefn.floorDefnMap) {
				this->loadTexture(currFloorDefnPair.second.filename, true);
				mapAssetBundle.floorTextureRefMap[currFloorDefnPair.first] = &this->textureMap[currFloorDefnPair.second.filename];
			}
		}

		void StoryLevelAssetBundle::loadBarrierTextureMap(StoryMapAssetBundle& mapAssetBundle) {
			for (auto const& currBarrierDefnPair : mapAssetBundle.mapDefn.barrierDefnMap) {
				this->loadTexture(currBarrierDefnPair.second.filename, true);
				mapAssetBundle.barrierTextureRefMap[currBarrierDefnPair.first] = &this->textureMap[currBarrierDefnPair.second.filename];
			}
		}

		void StoryLevelAssetBundle::loadTexture(const std::string& filename, bool repeatFlag) {
			this->indicateLoadingFilename(filename);

			if (this->textureMap.count(filename) == 0) {
				std::string fullFilePath = r3::snake::StoryLoaderUtils::resolveImageFilePath(this->campaignFolderName, filename);

				this->textureMap[filename] = sf::Texture();
				if (this->textureMap[filename].loadFromFile(fullFilePath)) {
					this->textureMap[filename].setRepeated(repeatFlag);
					this->incrementLoadedAssetCount();
				}
				else {
					this->failedFilenameList.push_back(filename);
				}
			}
			else {
				this->incrementLoadedAssetCount();
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

		void StoryLevelAssetBundle::loadCutsceneSoundTrack(sf::Music& targetMusic, const StoryCutsceneDefn& cutsceneDefn) {
			if (!cutsceneDefn.soundTrackFilename.empty()) {
				if (targetMusic.openFromFile(r3::snake::StoryLoaderUtils::resolveSoundFilePath(this->campaignFolderName, cutsceneDefn.soundTrackFilename))) {
					this->incrementLoadedAssetCount();
				}
				else {
					this->failedFilenameList.push_back(cutsceneDefn.soundTrackFilename);
				}
			}
		}

		void StoryLevelAssetBundle::loadFoodSpawnedSoundBuffer() {
			this->indicateLoadingFilename(StoryLevelAssetBundleConstants::DEFAULT_FOOD_SPAWNED_SOUND_PATH);

			if (this->foodSpawnedSoundBuffer.loadFromFile(StoryLevelAssetBundleConstants::DEFAULT_FOOD_SPAWNED_SOUND_PATH)) {
				this->incrementLoadedAssetCount();
			}
			else {
				this->failedFilenameList.push_back(StoryLevelAssetBundleConstants::DEFAULT_FOOD_SPAWNED_SOUND_PATH);
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

		void StoryLevelAssetBundle::loadSnakeHissSoundBuffer() {
			this->indicateLoadingFilename(StoryLevelAssetBundleConstants::DEFAULT_SNAKE_HISS_SOUND_PATH);

			if (this->snakeHissSoundBuffer.loadFromFile(StoryLevelAssetBundleConstants::DEFAULT_SNAKE_HISS_SOUND_PATH)) {
				this->incrementLoadedAssetCount();
			}
			else {
				this->failedFilenameList.push_back(StoryLevelAssetBundleConstants::DEFAULT_SNAKE_HISS_SOUND_PATH);
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
