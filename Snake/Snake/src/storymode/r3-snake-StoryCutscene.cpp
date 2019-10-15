
#include "../includes/r3-snake-storymodescene.hpp"

namespace r3 {

	namespace snake {

		StoryCutscene::StoryCutscene(const StoryCutsceneDefn& cutsceneDefn) {
			this->cutsceneDefn = &cutsceneDefn;
			this->currFrame = 0;
			this->framesSinceLastEvent = 0;
			this->nextEventIndex = 0;
		}

		const std::vector<StoryCutsceneScreenView>& StoryCutscene::getActiveScreenViewList() const {
			return this->activeScreenViewList;
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

	}

}
