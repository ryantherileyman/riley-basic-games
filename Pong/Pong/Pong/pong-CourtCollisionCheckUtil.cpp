
#include <cassert>
#include "pong-lib.h"

namespace pong {

	using namespace r3::graphics2d;

	CourtCollisionCheckUtil::CourtCollisionCheckUtil(const CourtCollisionSet* collisionSet) {
		this->collisionSet = collisionSet;
	}

	Vector2D CourtCollisionCheckUtil::resolveNewDirectionForWallCollision(
		const LineSegment2D* originalPathLineSegment
	) {
		Vector2D result = createVectorFromLineSegment(originalPathLineSegment);
		result.y = -result.y;

		normalizeVector(&result);

		return result;
	}

	Vector2D CourtCollisionCheckUtil::resolveNewDirectionForPaddleCollision(
		const LineSegment2D* originalPathLineSegment,
		const Position2D* collisionPoint,
		const LineSegment2D* paddleLineSegment
	) {
		Vector2D result;

		result.x = (originalPathLineSegment->point1.x - originalPathLineSegment->point2.x) / fabsf(originalPathLineSegment->point1.x - originalPathLineSegment->point2.x);

		float paddle_y = (paddleLineSegment->point1.y + paddleLineSegment->point2.y) / 2;
		float paddle_height = fabsf(paddleLineSegment->point1.y - paddleLineSegment->point2.y);
		result.y = (collisionPoint->y - paddle_y) / paddle_height;

		normalizeVector(&result);

		return result;
	}

	LineSegment2D CourtCollisionCheckUtil::adjustBallPathForWallCollision(
		const LineSegment2D* originalPathLineSegment,
		const BallCollisionResult* collisionResult
	) {
		LineSegment2D result;
		result.point1 = collisionResult->collisionPoint;
		result.point2.x = originalPathLineSegment->point2.x;
		result.point2.y = collisionResult->collisionPoint.y + (collisionResult->collisionPoint.y - originalPathLineSegment->point2.y);
		return result;
	}

	r3::graphics2d::LineSegment2D CourtCollisionCheckUtil::adjustBallPathForPaddleCollision(
		const LineSegment2D* originalPathLineSegment,
		const BallCollisionResult* collisionResult,
		const LineSegment2D* paddleLineSegment
	) {
		Vector2D newDirection = resolveNewDirectionForPaddleCollision(originalPathLineSegment, &collisionResult->collisionPoint, paddleLineSegment);

		float originalLength = lineSegmentLength(originalPathLineSegment);
		float bounceFactor = originalLength * (1 - collisionResult->percent);

		LineSegment2D result;
		result.point1 = collisionResult->collisionPoint;
		result.point2.x = collisionResult->collisionPoint.x + (newDirection.x * bounceFactor);
		result.point2.y = collisionResult->collisionPoint.y + (newDirection.y * bounceFactor);

		return result;
	}

	BallCollisionResult CourtCollisionCheckUtil::detectNextCollision(const LineSegment2D* ballPathLineSegment) {
		BallCollisionResult result{ BallCollisionTarget::NONE, 0, {0, 0} };

		// Check if the ball passes the plane of the left paddle
		if (
			(ballPathLineSegment->point1.x > this->collisionSet->leftPaddleLineSegment.point1.x) &&
			(ballPathLineSegment->point2.x <= this->collisionSet->leftPaddleLineSegment.point1.x)
		) {
			float percent = (this->collisionSet->leftPaddleLineSegment.point1.x - ballPathLineSegment->point1.x) / (ballPathLineSegment->point2.x - ballPathLineSegment->point1.x);
			float crossPlaneY = ballPathLineSegment->point1.y + ((ballPathLineSegment->point2.y - ballPathLineSegment->point1.y) * percent);

			// Check if the ball collided with the left paddle
			if (
				(crossPlaneY > this->collisionSet->leftPaddleLineSegment.point1.y) &&
				(crossPlaneY < this->collisionSet->leftPaddleLineSegment.point2.y)
			) {
				result.collisionTarget = BallCollisionTarget::LEFT_PADDLE;
				result.percent = percent;
				result.collisionPoint.x = this->collisionSet->leftPaddleLineSegment.point1.x;
				result.collisionPoint.y = crossPlaneY;
				result.newDirection = resolveNewDirectionForPaddleCollision(ballPathLineSegment, &result.collisionPoint, &this->collisionSet->leftPaddleLineSegment);
			}
		}

		// Check if the ball passes the plane of the right paddle
		if (
			(ballPathLineSegment->point1.x < this->collisionSet->rightPaddleLineSegment.point1.x) &&
			(ballPathLineSegment->point2.x >= this->collisionSet->rightPaddleLineSegment.point1.x)
		) {
			float percent = (this->collisionSet->rightPaddleLineSegment.point1.x - ballPathLineSegment->point1.x) / (ballPathLineSegment->point2.x - ballPathLineSegment->point1.x);
			float crossPlaneY = ballPathLineSegment->point1.y + ((ballPathLineSegment->point2.y - ballPathLineSegment->point1.y) * percent);

			// Check if the ball collided with the right paddle
			if (
				(crossPlaneY > this->collisionSet->rightPaddleLineSegment.point1.y) &&
				(crossPlaneY < this->collisionSet->rightPaddleLineSegment.point2.y)
			) {
				result.collisionTarget = BallCollisionTarget::RIGHT_PADDLE;
				result.percent = percent;
				result.collisionPoint.x = this->collisionSet->rightPaddleLineSegment.point1.x;
				result.collisionPoint.y = crossPlaneY;
				result.newDirection = resolveNewDirectionForPaddleCollision(ballPathLineSegment, &result.collisionPoint, &this->collisionSet->rightPaddleLineSegment);
			}
		}

		// Check if the ball passes the plane of the top wall
		if (
			(ballPathLineSegment->point1.y < this->collisionSet->topWallLineSegment.point1.y) &&
			(ballPathLineSegment->point2.y >= this->collisionSet->topWallLineSegment.point1.y)
		) {
			float percent = (this->collisionSet->topWallLineSegment.point1.y - ballPathLineSegment->point1.y) / (ballPathLineSegment->point2.y - ballPathLineSegment->point1.y);
			float crossPlaneX = ballPathLineSegment->point1.x + ((ballPathLineSegment->point2.x - ballPathLineSegment->point1.x) * percent);

			// Check if the ball collided with the top wall
			if (
				(crossPlaneX > this->collisionSet->topWallLineSegment.point1.x) &&
				(crossPlaneX < this->collisionSet->topWallLineSegment.point2.x)
			) {
				result.collisionTarget = BallCollisionTarget::TOP_WALL;
				result.percent = percent;
				result.collisionPoint.x = crossPlaneX;
				result.collisionPoint.y = this->collisionSet->topWallLineSegment.point1.y;
				result.newDirection = resolveNewDirectionForWallCollision(ballPathLineSegment);
			}
		}

		// Check if the ball passes the plane of the bottom wall
		if (
			(ballPathLineSegment->point1.y > this->collisionSet->bottomWallLineSegment.point1.y) &&
			(ballPathLineSegment->point2.y <= this->collisionSet->bottomWallLineSegment.point1.y)
		) {
			float percent = (this->collisionSet->bottomWallLineSegment.point1.y - ballPathLineSegment->point1.y) / (ballPathLineSegment->point2.y - ballPathLineSegment->point1.y);
			float crossPlaneX = ballPathLineSegment->point1.x + ((ballPathLineSegment->point2.x - ballPathLineSegment->point1.x) * percent);

			// Check if the ball collided with the bottom wall
			if (
				(crossPlaneX > this->collisionSet->bottomWallLineSegment.point1.x) &&
				(crossPlaneX < this->collisionSet->bottomWallLineSegment.point2.x)
			) {
				result.collisionTarget = BallCollisionTarget::BOTTOM_WALL;
				result.percent = percent;
				result.collisionPoint.x = crossPlaneX;
				result.collisionPoint.y = this->collisionSet->bottomWallLineSegment.point1.y;
				result.newDirection = resolveNewDirectionForWallCollision(ballPathLineSegment);
			}
		}

		return result;
	}

	LineSegment2D CourtCollisionCheckUtil::adjustBallPath(const LineSegment2D* originalPathLineSegment, const BallCollisionResult* collisionResult) {
		assert(collisionResult->collisionTarget != BallCollisionTarget::NONE);

		LineSegment2D result{ {0, 0}, {0, 0} };

		switch (collisionResult->collisionTarget) {
		case BallCollisionTarget::NONE:
			break;
		case BallCollisionTarget::TOP_WALL:
		case BallCollisionTarget::BOTTOM_WALL:
			result = adjustBallPathForWallCollision(originalPathLineSegment, collisionResult);
			break;
		case BallCollisionTarget::LEFT_PADDLE:
			result = adjustBallPathForPaddleCollision(originalPathLineSegment, collisionResult, &this->collisionSet->leftPaddleLineSegment);
			break;
		case BallCollisionTarget::RIGHT_PADDLE:
			result = adjustBallPathForPaddleCollision(originalPathLineSegment, collisionResult, &this->collisionSet->rightPaddleLineSegment);
			break;
		}

		return result;
	}

}
