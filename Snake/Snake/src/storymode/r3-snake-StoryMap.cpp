
#include "../includes/r3-snake-storydefn.hpp"

namespace r3 {

	namespace snake {

		StoryMap::StoryMap(const StoryMapDefn& mapDefn) {
			this->fieldSize = mapDefn.fieldSize;

			this->allocatedFloorArray = new int[mapDefn.fieldSize.x * mapDefn.fieldSize.y];
			this->floorGrid = new int* [mapDefn.fieldSize.y];
			for (int rowIndex = 0; rowIndex < mapDefn.fieldSize.y; rowIndex++) {
				this->floorGrid[rowIndex] = &this->allocatedFloorArray[rowIndex * mapDefn.fieldSize.x];
				for (int colIndex = 0; colIndex < mapDefn.fieldSize.x; colIndex++) {
					this->floorGrid[rowIndex][colIndex] = mapDefn.floorGrid[rowIndex][colIndex];
				}
			}

			this->allocatedBarrierArray = new int[mapDefn.fieldSize.x * mapDefn.fieldSize.y];
			this->barrierGrid = new int* [mapDefn.fieldSize.y];
			for (int rowIndex = 0; rowIndex < mapDefn.fieldSize.y; rowIndex++) {
				this->barrierGrid[rowIndex] = &this->allocatedBarrierArray[rowIndex * mapDefn.fieldSize.x];
				for (int colIndex = 0; colIndex < mapDefn.fieldSize.x; colIndex++) {
					this->barrierGrid[rowIndex][colIndex] = mapDefn.barrierGrid[rowIndex][colIndex];
				}
			}
		}

		StoryMap::~StoryMap() {
			delete[] this->floorGrid;
			delete[] this->allocatedFloorArray;

			delete[] this->barrierGrid;
			delete[] this->allocatedBarrierArray;
		}

		sf::Vector2i StoryMap::getFieldSize() const {
			return this->fieldSize;
		}

		int StoryMap::getFloorId(int x, int y) const {
			int result = this->floorGrid[y][x];
			return result;
		}

		int StoryMap::getBarrierId(int x, int y) const {
			int result = this->barrierGrid[y][x];
			return result;
		}

		bool StoryMap::barrierAt(int x, int y) const {
			int barrierId = this->barrierGrid[y][x];
			bool result = barrierId > 0;
			return result;
		}

	}

}
