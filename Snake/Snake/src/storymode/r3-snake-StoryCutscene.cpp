
#include "../includes/r3-snake-storymodescene.hpp"

namespace r3 {

	namespace snake {

		StoryCutscene::StoryCutscene(const StoryCutsceneDefn& cutsceneDefn) {
			this->cutsceneDefn = &cutsceneDefn;
			this->currFrame = 0;
			this->framesSinceLastEvent = 0;
			this->nextEventIndex = 0;
			this->snake = nullptr;
		}

		StoryCutscene::~StoryCutscene() {
			if (this->snake != nullptr) {
				delete this->snake;
			}
		}

		const std::vector<StoryCutsceneScreenView>& StoryCutscene::getActiveScreenViewList() const {
			return this->activeScreenViewList;
		}

		const Snake* StoryCutscene::getSnake() const {
			return this->snake;
		}

		const std::unordered_map<int, StoryFoodInstance>& StoryCutscene::getFoodInstanceMap() const {
			return this->foodInstanceMap;
		}

		bool StoryCutscene::update() {
			this->updateActiveScreenViews();

			this->processEvents();

			this->currFrame++;
			this->framesSinceLastEvent++;

			printf("Now at frame %d\n", this->currFrame);

			bool result = (this->nextEventIndex >= this->cutsceneDefn->eventDefnList.size());
			return result;
		}

		void StoryCutscene::updateActiveScreenViews() {
			int viewsToErase = 0;
			for (size_t index = 0; index < this->activeScreenViewList.size(); index++) {
				if (this->activeScreenViewList[index].fadeFramesRemaining > 0) {
					this->activeScreenViewList[index].fadeFramesRemaining--;
				}
				else {
					viewsToErase = index;
				}
			}

			if (viewsToErase > 0) {
				this->activeScreenViewList.erase(this->activeScreenViewList.begin(), this->activeScreenViewList.begin() + viewsToErase);
			}
		}

		void StoryCutscene::processEvents() {
			bool doneProcessingEvents = (this->nextEventIndex >= this->cutsceneDefn->eventDefnList.size());

			while (!doneProcessingEvents) {
				const StoryCutsceneEventDefn& nextEventDefn = this->cutsceneDefn->eventDefnList.at(this->nextEventIndex);
				if (this->framesSinceLastEvent >= nextEventDefn.framesSinceLastEvent) {
					this->processEvent(nextEventDefn);

					this->nextEventIndex++;
					doneProcessingEvents = (this->nextEventIndex >= this->cutsceneDefn->eventDefnList.size());

					this->framesSinceLastEvent = 0;
				}
				else {
					doneProcessingEvents = true;
				}
			}
		}

		void StoryCutscene::processEvent(const StoryCutsceneEventDefn& eventDefn) {
			switch (eventDefn.eventType) {
			case StoryCutsceneEventType::COLOR:
				this->processColorEvent(eventDefn.colorEvent);
				break;
			case StoryCutsceneEventType::TEXTURE:
				this->processTextureEvent(eventDefn.textureEvent);
				break;
			case StoryCutsceneEventType::SHOW_MAP:
				this->processShowMapEvent(eventDefn.mapEvent);
				break;
			case StoryCutsceneEventType::SHOW_SNAKE:
				this->processShowSnakeEvent(eventDefn.snakeEvent);
				break;
			case StoryCutsceneEventType::MOVE_SNAKE:
				this->processMoveSnakeEvent(eventDefn.snakeEvent);
				break;
			case StoryCutsceneEventType::HIDE_SNAKE:
				this->processHideSnakeEvent(eventDefn.snakeEvent);
				break;
			case StoryCutsceneEventType::SHOW_FOOD:
				this->processShowFoodEvent(eventDefn.foodEvent);
				break;
			case StoryCutsceneEventType::HIDE_FOOD:
				this->processHideFoodEvent(eventDefn.foodEvent);
				break;
			}
		}

		void StoryCutscene::processColorEvent(const StoryCutsceneColorEventDefn& colorEventDefn) {
			StoryCutsceneScreenView newScreenView;
			newScreenView.screenEventType = StoryCutsceneScreenViewType::COLOR;
			newScreenView.fadeFrames = colorEventDefn.fadeFrames;
			newScreenView.fadeFramesRemaining = colorEventDefn.fadeFrames;
			newScreenView.color = colorEventDefn.color;

			this->addScreenView(newScreenView);
		}

		void StoryCutscene::processTextureEvent(const StoryCutsceneTextureEventDefn& textureEventDefn) {
			StoryCutsceneScreenView newScreenView;
			newScreenView.screenEventType = StoryCutsceneScreenViewType::TEXTURE;
			newScreenView.fadeFrames = textureEventDefn.fadeFrames;
			newScreenView.fadeFramesRemaining = textureEventDefn.fadeFrames;
			newScreenView.textureFilename = textureEventDefn.textureFilename;

			this->addScreenView(newScreenView);
		}

		void StoryCutscene::processShowMapEvent(const StoryCutsceneMapEventDefn& showMapEventDefn) {
			StoryCutsceneScreenView newScreenView;
			newScreenView.screenEventType = StoryCutsceneScreenViewType::MAP;
			newScreenView.fadeFrames = showMapEventDefn.fadeFrames;
			newScreenView.fadeFramesRemaining = showMapEventDefn.fadeFrames;
			newScreenView.mapFilename = showMapEventDefn.mapFilename;

			this->addScreenView(newScreenView);
		}

		void StoryCutscene::addScreenView(const StoryCutsceneScreenView& screenView) {
			if (screenView.fadeFramesRemaining <= 0) {
				this->activeScreenViewList.clear();
			}

			this->activeScreenViewList.push_back(screenView);
		}

		void StoryCutscene::processShowSnakeEvent(const StoryCutsceneSnakeEventDefn& snakeEventDefn) {
			if (this->snake != nullptr) {
				delete this->snake;
				this->snake = nullptr;

				printf("Cut-scene:  Teleporting the snake to a new position %d, %d\n", snakeEventDefn.snakeStart.headPosition.x, snakeEventDefn.snakeStart.headPosition.y);
			}

			if (
				!this->activeScreenViewList.empty() &&
				(this->activeScreenViewList.back().screenEventType == StoryCutsceneScreenViewType::MAP)
			) {
				this->snake = new Snake(snakeEventDefn.snakeStart);
			}
			else {
				printf("Cut-scene:  The \"showSnake\" event will be ignored, as no map is currently visible\n");
			}
		}

		void StoryCutscene::processMoveSnakeEvent(const StoryCutsceneSnakeEventDefn& snakeEventDefn) {
			if (this->snake == nullptr) {
				printf("Cut-scene:  The \"moveSnake\" event will be ignored, as the snake is not currently visible\n");
			}
			else {
				if (snakeEventDefn.snakeGrowFlag) {
					this->snake->growForward(snakeEventDefn.snakeDirection);
				}
				else {
					this->snake->moveForward(snakeEventDefn.snakeDirection);
				}
			}
		}

		void StoryCutscene::processHideSnakeEvent(const StoryCutsceneSnakeEventDefn& snakeEventDefn) {
			if (this->snake == nullptr) {
				printf("Cut-scene:  The \"hideSnake\" event will be ignored, as the snake is not currently visible\n");
			}
			else {
				delete this->snake;
				this->snake = nullptr;
			}
		}

		void StoryCutscene::processShowFoodEvent(const StoryCutsceneFoodEventDefn& foodEventDefn) {
			if (this->foodInstanceMap.count(foodEventDefn.instanceId) > 0) {
				printf("Cut-scene:  Food instance %d already exists\n", foodEventDefn.instanceId);
			}

			StoryFoodInstance newFoodInstance;
			newFoodInstance.foodInstanceId = foodEventDefn.instanceId;
			newFoodInstance.foodType = foodEventDefn.foodType;
			newFoodInstance.position = foodEventDefn.position;

			this->foodInstanceMap[foodEventDefn.instanceId] = newFoodInstance;
		}

		void StoryCutscene::processHideFoodEvent(const StoryCutsceneFoodEventDefn& foodEventDefn) {
			if (this->foodInstanceMap.count(foodEventDefn.instanceId) == 0) {
				printf("Cut-scene:  The \"hideFood\" event will be ignored, as the instance ID is not currently visible.\n");
			}
			else {
				this->foodInstanceMap.erase(foodEventDefn.instanceId);
			}
		}

	}

}
