
#include "../includes/r3-snake-storydefn.hpp"

namespace r3 {

	namespace snake {

		StoryLevel::StoryLevel(const StoryMapDefn& mapDefn, const StoryLevelDefn& levelDefn) {
			this->map = new StoryMap(mapDefn);
			this->snake = new Snake(levelDefn.snakeStart);
		}

		StoryLevel::~StoryLevel() {
			delete this->map;
			delete this->snake;
		}

		StoryMap* StoryLevel::getMap() const {
			return this->map;
		}

		Snake* StoryLevel::getSnake() const {
			return this->snake;
		}

	}

}
