
#include <time.h>
#include "pong-lib.h"

namespace pong {

	using namespace r3::graphics2d;

	SnookerProPaddleAi::SnookerProPaddleAi() {
		this->generator.seed(time(NULL));
	}

	PaddleInputType SnookerProPaddleAi::resolvePaddleInputType(PaddleAiInput input) {
		PaddleInputType result = PaddleInputType::NONE;

		float halfCourtWidth = input.match->getCourtSize().width / 2.0f;

		Position2D currBallPosition = input.match->getBallState().position;
		Vector2D currBallDirection = input.match->getBallState().direction;

		Position2D currPaddlePosition = input.paddle->getPosition();

		if (
			(this->prevBallDirectionX != currBallDirection.x) &&
			(
				(
					(input.paddle->getSide() == PaddleSide::LEFT) &&
					(currBallDirection.x < 0)
				) ||
				(
					(input.paddle->getSide() == PaddleSide::RIGHT) &&
					(currBallDirection.x > 0)
				)
			)
		) {
			CourtCollisionSet courtCollisionSet;

			courtCollisionSet.topWallLineSegment = input.match->getTopWallLineSegment();
			courtCollisionSet.bottomWallLineSegment = input.match->getBottomWallLineSegment();

			courtCollisionSet.leftPaddleLineSegment.point1.x = input.match->getLeftPaddle()->getPosition().x;
			courtCollisionSet.leftPaddleLineSegment.point1.y = -halfCourtWidth;
			courtCollisionSet.leftPaddleLineSegment.point2.x = input.match->getLeftPaddle()->getPosition().x;
			courtCollisionSet.leftPaddleLineSegment.point2.y = halfCourtWidth;

			courtCollisionSet.rightPaddleLineSegment.point1.x = input.match->getRightPaddle()->getPosition().x;
			courtCollisionSet.rightPaddleLineSegment.point1.y = -halfCourtWidth;
			courtCollisionSet.rightPaddleLineSegment.point2.x = input.match->getRightPaddle()->getPosition().x;
			courtCollisionSet.rightPaddleLineSegment.point2.y = halfCourtWidth;

			LineSegment2D ballPathLineSegment;
			ballPathLineSegment.point1.x = currBallPosition.x;
			ballPathLineSegment.point1.y = currBallPosition.y;
			ballPathLineSegment.point2.x = currBallPosition.x + (currBallDirection.x * input.match->getCourtSize().width * 2);
			ballPathLineSegment.point2.y = currBallPosition.y + (currBallDirection.y * input.match->getCourtSize().width * 2);

			BallCollisionTarget paddleCollisionTarget = BallCollisionTarget::NONE;
			if (input.paddle->getSide() == PaddleSide::LEFT) {
				paddleCollisionTarget = BallCollisionTarget::LEFT_PADDLE;
			}
			else if (input.paddle->getSide() == PaddleSide::RIGHT) {
				paddleCollisionTarget = BallCollisionTarget::RIGHT_PADDLE;
			}

			CourtCollisionCheckUtil* collisionCheckUtil = new CourtCollisionCheckUtil(&courtCollisionSet);

			BallCollisionResult collisionResult = collisionCheckUtil->detectNextCollision(&ballPathLineSegment);
			while (collisionResult.collisionTarget != paddleCollisionTarget) {
				ballPathLineSegment = collisionCheckUtil->adjustBallPath(&ballPathLineSegment, &collisionResult);
				collisionResult = collisionCheckUtil->detectNextCollision(&ballPathLineSegment);
			}

			this->desiredPaddlePosition = collisionResult.collisionPoint.y;
			this->performedDeflectionAdjustmentFlag = false;
		}

		if (
			!this->performedDeflectionAdjustmentFlag &&
			(this->prevBallDirectionX == currBallDirection.x) &&
			(
				(
					(input.paddle->getSide() == PaddleSide::LEFT) &&
					(currBallPosition.x < -halfCourtWidth * 0.5f)
				) ||
				(
					(input.paddle->getSide() == PaddleSide::RIGHT) &&
					(currBallPosition.x > halfCourtWidth * 0.5f)
				)
			)
		) {
			float deflectionPos = this->paddleDeflectionDistribution(this->generator);
			this->desiredPaddlePosition = this->desiredPaddlePosition + input.paddle->getSize().height * deflectionPos;
			this->performedDeflectionAdjustmentFlag = true;
		}

		if (currPaddlePosition.y < this->desiredPaddlePosition) {
			result = PaddleInputType::MOVE_UP;
		}
		else if (currPaddlePosition.y > this->desiredPaddlePosition) {
			result = PaddleInputType::MOVE_DOWN;
		}

		this->prevBallDirectionX = input.match->getBallState().direction.x;

		return result;
	}

}
