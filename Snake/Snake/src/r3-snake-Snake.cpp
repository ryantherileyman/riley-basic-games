
#include <assert.h>
#include "includes/r3-snake-gamestate.hpp"

namespace r3 {

	namespace snake {

		namespace SnakeUtils {
			
			sf::Vector2i directionToVector(const ObjectDirection& direction) {
				sf::Vector2i result(0, 0);

				switch (direction) {
				case ObjectDirection::UP:
					result.y--;
					break;
				case ObjectDirection::RIGHT:
					result.x++;
					break;
				case ObjectDirection::DOWN:
					result.y++;
					break;
				case ObjectDirection::LEFT:
					result.x--;
					break;
				}

				return result;
			}

		}

		Snake::Snake(SnakeStartDefn& startDefn) {
			assert(startDefn.length >= 2);

			sf::Vector2i nextSegmentPosition = startDefn.headPosition;
			sf::Vector2i adjustVector = sf::Vector2i(0, 0) - SnakeUtils::directionToVector(startDefn.facingDirection);

			this->head.segmentType = SnakeSegmentType::HEAD;
			this->head.position = nextSegmentPosition;
			this->head.enterDirection = startDefn.facingDirection;
			this->head.exitDirection = ObjectDirection::NONE;

			this->bodyList.reserve(500);
			int bodyCount = startDefn.length - 2;
			for (int currBodyPos = 0; currBodyPos < bodyCount; currBodyPos++) {
				nextSegmentPosition += adjustVector;

				SnakeSegment currBodySegment;
				currBodySegment.segmentType = SnakeSegmentType::BODY;
				currBodySegment.position = nextSegmentPosition;
				currBodySegment.enterDirection = startDefn.facingDirection;
				currBodySegment.exitDirection = startDefn.facingDirection;

				this->bodyList.push_back(currBodySegment);
			}

			nextSegmentPosition += adjustVector;

			this->tail.segmentType = SnakeSegmentType::TAIL;
			this->tail.position = nextSegmentPosition;
			this->tail.enterDirection = ObjectDirection::NONE;
			this->tail.exitDirection = startDefn.facingDirection;
		}

		SnakeSegment Snake::getHead() const {
			return this->head;
		}

		int Snake::getBodyLength() const {
			return this->bodyList.size();
		}

		SnakeSegment Snake::getBody(int segmentIndex) const {
			SnakeSegment result = this->bodyList.at(segmentIndex);
			return result;
		}

		SnakeSegment Snake::getTail() const {
			return this->tail;
		}

		void Snake::moveForward(ObjectDirection direction) {
			this->moveHeadForward(direction);
			this->moveBodyForward();
			this->moveTailForward();
		}

		void Snake::growForward(ObjectDirection direction) {
			this->moveHeadForward(direction);
			this->moveBodyForward();

			SnakeSegment lastBodySegment = this->bodyList.back();

			SnakeSegment extraBodySegment;
			extraBodySegment.segmentType = SnakeSegmentType::BODY;
			extraBodySegment.position = lastBodySegment.position - SnakeUtils::directionToVector(lastBodySegment.exitDirection);
			extraBodySegment.exitDirection = lastBodySegment.enterDirection;
			extraBodySegment.enterDirection = tail.exitDirection;

			if (this->bodyList.size() == this->bodyList.capacity()) {
				this->bodyList.reserve(this->bodyList.size() + 500);
			}

			this->bodyList.push_back(extraBodySegment);
		}

		void Snake::moveHeadForward(ObjectDirection direction) {
			this->head.position += SnakeUtils::directionToVector(direction);
			this->head.enterDirection = direction;
		}

		void Snake::moveBodyForward() {
			int bodyLength = this->getBodyLength();

			ObjectDirection previousSegmentEnterDirection = this->head.enterDirection;

			for (int currSegmentIndex = 0; currSegmentIndex < bodyLength; currSegmentIndex++) {
				this->bodyList[currSegmentIndex].position += SnakeUtils::directionToVector(this->bodyList[currSegmentIndex].exitDirection);
				this->bodyList[currSegmentIndex].enterDirection = this->bodyList[currSegmentIndex].exitDirection;
				this->bodyList[currSegmentIndex].exitDirection = previousSegmentEnterDirection;

				previousSegmentEnterDirection = this->bodyList[currSegmentIndex].enterDirection;
			}
		}

		void Snake::moveTailForward() {
			ObjectDirection newExitDirection = this->bodyList.back().enterDirection;

			this->tail.position += SnakeUtils::directionToVector(this->tail.exitDirection);
			this->tail.exitDirection = newExitDirection;
		}

	}

}
