
#include <vector>
#include <SFML/Graphics.hpp>
#pragma once

namespace r3 {

	namespace snake {

		typedef enum class Snake_ObjectDirection {
			NONE,
			UP,
			RIGHT,
			DOWN,
			LEFT,
		} ObjectDirection;

		typedef enum class Snake_SnakeSegmentType {
			HEAD,
			BODY,
			TAIL,
		} SnakeSegmentType;

		typedef struct Snake_SnakeSegment {
			SnakeSegmentType segmentType;
			sf::Vector2i position;
			ObjectDirection enterDirection;
			ObjectDirection exitDirection;
		} SnakeSegment;

		typedef struct Snake_SnakeStartDefn {
			sf::Vector2i headPosition;
			ObjectDirection facingDirection;
			int length;
		} SnakeStartDefn;

		namespace SnakeUtils {

			sf::Vector2i directionToVector(const ObjectDirection& direction);

		}

		class Snake;

		class Snake {

		private:
			SnakeSegment head;
			std::vector<SnakeSegment> bodyList;
			SnakeSegment tail;

		public:
			Snake(const SnakeStartDefn& startDefn);

		public:
			SnakeSegment getHead() const;
			int getBodyLength() const;
			SnakeSegment getBody(int segmentIndex) const;
			SnakeSegment getTail() const;
			int getLength() const;

		public:
			bool isValidMovementDirection(ObjectDirection direction);
			bool occupiesPosition(sf::Vector2i position);

		public:
			void moveForward(ObjectDirection direction);
			void growForward(ObjectDirection direction);

		private:
			void moveHeadForward(ObjectDirection direction);
			void moveBodyForward();
			void moveTailForward();

		};

	}

}
