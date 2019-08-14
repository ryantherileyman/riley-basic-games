
#include "pong-lib.h"

namespace pong {

	using namespace r3::graphics2d;

	Paddle::Paddle(const PaddleDefn* paddleDefn) {
		this->courtSize = paddleDefn->courtSize;
		this->size = paddleDefn->paddleSize;
		this->side = paddleDefn->side;

		switch (paddleDefn->side) {
		case PaddleSide::LEFT:
			this->position.x = -paddleDefn->courtSize.width / 2 + (paddleDefn->paddleSize.width * 2);
			break;
		case PaddleSide::RIGHT:
			this->position.x = paddleDefn->courtSize.width / 2 - (paddleDefn->paddleSize.width * 2);
			break;
		}
		this->position.y = 0;
	}

	Size2D Paddle::getSize() const {
		return this->size;
	}

	PaddleSide Paddle::getSide() const {
		return this->side;
	}

	Position2D Paddle::getPosition() const {
		return this->position;
	}

	LineSegment2D Paddle::createCollisionLineSegment() {
		LineSegment2D result;
		result.point1.x = this->position.x;
		result.point1.y = this->position.y - (this->size.height / 2);
		result.point2.x = this->position.x;
		result.point2.y = this->position.y + (this->size.height / 2);
		return result;
	}

	float Paddle::moveUp(float distance) {
		float result = this->position.y += distance;
		if (result > (this->courtSize.height / 2)) {
			result = this->courtSize.height / 2;
		}

		this->position.y = result;

		return result;
	}

	float Paddle::moveDown(float distance) {
		float result = this->position.y -= distance;
		if (result < (-this->courtSize.height / 2)) {
			result = -this->courtSize.height / 2;
		}

		this->position.y = result;

		return result;
	}

}