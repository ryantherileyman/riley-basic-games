
#include <cassert>
#include "pong-lib.h"

namespace pong {

	CourtCollisionCheckUtil::CourtCollisionCheckUtil(const CourtCollisionSet* collisionSet) {
		this->collisionSet = collisionSet;
	}

	BallCollisionResult CourtCollisionCheckUtil::detectNextCollision(const LineSegment2D* ballPathLineSegment) {
		BallCollisionResult result{ BallCollisionTarget::NONE, 0, {0, 0} };

		LineSegmentIntersectionResult topWallIntersectionResult = checkLineSegmentsIntersect(ballPathLineSegment, &this->collisionSet->topWallLineSegment);
		if (topWallIntersectionResult.intersectionType == LineSegmentIntersectionType::SINGLE_POINT) {
			result.collisionTarget = BallCollisionTarget::TOP_WALL;
			result.percent = topWallIntersectionResult.percentPoint1;
			result.collisionPoint = topWallIntersectionResult.intersectionPoint1;
		}

		LineSegmentIntersectionResult bottomWallIntersectionResult = checkLineSegmentsIntersect(ballPathLineSegment, &this->collisionSet->bottomWallLineSegment);
		if (bottomWallIntersectionResult.intersectionType == LineSegmentIntersectionType::SINGLE_POINT) {
			assert(result.collisionTarget == BallCollisionTarget::NONE);
			result.collisionTarget = BallCollisionTarget::BOTTOM_WALL;
			result.percent = bottomWallIntersectionResult.percentPoint1;
			result.collisionPoint = bottomWallIntersectionResult.intersectionPoint1;
		}

		LineSegmentIntersectionResult leftPaddleIntersectionResult = checkLineSegmentsIntersect(ballPathLineSegment, &this->collisionSet->leftPaddleLineSegment);
		if (leftPaddleIntersectionResult.intersectionType == LineSegmentIntersectionType::SINGLE_POINT) {
			assert(result.collisionTarget == BallCollisionTarget::NONE);
			result.collisionTarget = BallCollisionTarget::LEFT_PADDLE;
			result.percent = leftPaddleIntersectionResult.percentPoint1;
			result.collisionPoint = leftPaddleIntersectionResult.intersectionPoint1;
		}

		LineSegmentIntersectionResult rightPaddleIntersectionResult = checkLineSegmentsIntersect(ballPathLineSegment, &this->collisionSet->rightPaddleLineSegment);
		if (rightPaddleIntersectionResult.intersectionType == LineSegmentIntersectionType::SINGLE_POINT) {
			assert(result.collisionTarget == BallCollisionTarget::NONE);
			result.collisionTarget = BallCollisionTarget::RIGHT_PADDLE;
			result.percent = rightPaddleIntersectionResult.percentPoint1;
			result.collisionPoint = rightPaddleIntersectionResult.intersectionPoint1;
		}

		return result;
	}

	LineSegment2D CourtCollisionCheckUtil::adjustBallPath(const LineSegment2D* originalPathLineSegment, const BallCollisionResult* collisionResult) {
		assert(collisionResult->collisionTarget != BallCollisionTarget::NONE);

		LineSegment2D result;
		result.point1 = collisionResult->collisionPoint;

		if (collisionResult->collisionTarget == BallCollisionTarget::TOP_WALL) {
			result.point2.x = originalPathLineSegment->point2.x;
			result.point2.y = collisionResult->collisionPoint.y + (collisionResult->collisionPoint.y - originalPathLineSegment->point2.y);
		}

		if (collisionResult->collisionTarget == BallCollisionTarget::BOTTOM_WALL) {
			result.point2.x = originalPathLineSegment->point2.x;
			result.point2.y = collisionResult->collisionPoint.y - (collisionResult->collisionPoint.y - originalPathLineSegment->point1.y);
		}

		if (collisionResult->collisionTarget == BallCollisionTarget::LEFT_PADDLE) {
			float new_x_dir = originalPathLineSegment->point1.x - originalPathLineSegment->point2.x;

			float paddle_y = (this->collisionSet->leftPaddleLineSegment.point1.y + this->collisionSet->leftPaddleLineSegment.point2.y) / 2;
			float paddle_height = fabsf(this->collisionSet->leftPaddleLineSegment.point1.y - this->collisionSet->leftPaddleLineSegment.point2.y);
			float new_y_dir = (collisionResult->collisionPoint.y - paddle_y) / paddle_height;

			result.point2.x = collisionResult->collisionPoint.x + (collisionResult->collisionPoint.x - originalPathLineSegment->point2.x);

			// TODO... continue...
		}

	}

}
