
#include <time.h>
#include "pong-lib.h"

namespace pong {

	using namespace r3::graphics2d;

	SnookerProPaddleAi::SnookerProPaddleAi() {
		this->generator.seed(time(NULL));

		this->collisionSet.topWallLineSegment = { 0.0f, 0.0f, 0.0f, 0.0f };
		this->collisionSet.bottomWallLineSegment = { 0.0f, 0.0f, 0.0f, 0.0f };
		this->collisionSet.leftPaddleLineSegment = { 0.0f, 0.0f, 0.0f, 0.0f };
		this->collisionSet.rightPaddleLineSegment = { 0.0f, 0.0f, 0.0f, 0.0f };
	}

	PaddleInputType SnookerProPaddleAi::resolvePaddleInputType(PaddleAiInput input) {
		PaddleInputType result = PaddleInputType::NONE;

		Position2D currBallPosition = input.match->getBallState().position;
		Vector2D currBallDirection = input.match->getBallState().direction;

		Position2D currPaddlePosition = input.paddle->getPosition();

		// Court collision set only needs to be initialized once; we are assuming the court size will never change
		if (!this->courtCollisionSetInitializedFlag) {
			this->initializeCollisionSet(input);
			this->courtCollisionSetInitializedFlag = true;
		}

		// We only want to determine the desired position of the paddle when the ball has changed its horizontal direction
		if (this->prevBallDirectionX != currBallDirection.x) {
			if (this->ballHeadedTowardPaddle(input)) {
				this->desiredPaddlePosition = this->calculateYPositionBallWillCrossPlaneOfPaddle(input);
				this->performedDeflectionAdjustmentFlag = false;
			}
			else {
				this->desiredPaddlePosition = 0.0f;
			}
		}

		// As the ball gets close to the paddle, attempt to deflect the ball in some direction to "fake out" the other player
		if (
			!this->performedDeflectionAdjustmentFlag &&
			(this->prevBallDirectionX == currBallDirection.x) &&
			this->ballNearPaddle(input)
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

	void SnookerProPaddleAi::initializeCollisionSet(PaddleAiInput input) {
		this->collisionSet.topWallLineSegment = input.match->getTopWallLineSegment();
		this->collisionSet.bottomWallLineSegment = input.match->getBottomWallLineSegment();

		this->collisionSet.leftPaddleLineSegment.point1.x = input.match->getLeftPaddle()->getPosition().x;
		this->collisionSet.leftPaddleLineSegment.point1.y = -input.match->getCourtSize().width / 2.0f;
		this->collisionSet.leftPaddleLineSegment.point2.x = input.match->getLeftPaddle()->getPosition().x;
		this->collisionSet.leftPaddleLineSegment.point2.y = input.match->getCourtSize().width / 2.0f;

		this->collisionSet.rightPaddleLineSegment.point1.x = input.match->getRightPaddle()->getPosition().x;
		this->collisionSet.rightPaddleLineSegment.point1.y = -input.match->getCourtSize().width / 2.0f;
		this->collisionSet.rightPaddleLineSegment.point2.x = input.match->getRightPaddle()->getPosition().x;
		this->collisionSet.rightPaddleLineSegment.point2.y = input.match->getCourtSize().width / 2.0f;
	}

	bool SnookerProPaddleAi::ballHeadedTowardPaddle(PaddleAiInput input) {
		Vector2D currBallDirection = input.match->getBallState().direction;

		bool result =
			(
				(input.paddle->getSide() == PaddleSide::LEFT) &&
				(currBallDirection.x < 0)
			) ||
			(
				(input.paddle->getSide() == PaddleSide::RIGHT) &&
				(currBallDirection.x > 0)
			);
		return result;
	}

	float SnookerProPaddleAi::calculateYPositionBallWillCrossPlaneOfPaddle(PaddleAiInput input) {
		Position2D currBallPosition = input.match->getBallState().position;
		Vector2D currBallDirection = input.match->getBallState().direction;

		// Pretend the ball is travelling twice the distance of the court width
		// This ensures it will always hit the plane of the paddle
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

		// Look for collisions until the ball would hit the paddle
		CourtCollisionCheckUtil* collisionCheckUtil = new CourtCollisionCheckUtil(&this->collisionSet);

		BallCollisionResult collisionResult = collisionCheckUtil->detectNextCollision(&ballPathLineSegment);
		while (collisionResult.collisionTarget != paddleCollisionTarget) {
			ballPathLineSegment = collisionCheckUtil->adjustBallPath(&ballPathLineSegment, &collisionResult);
			collisionResult = collisionCheckUtil->detectNextCollision(&ballPathLineSegment);
		}

		float result = collisionResult.collisionPoint.y;
		return result;
	}

	bool SnookerProPaddleAi::ballNearPaddle(PaddleAiInput input) {
		Position2D currBallPosition = input.match->getBallState().position;

		bool result =
			(
				(input.paddle->getSide() == PaddleSide::LEFT) &&
				(currBallPosition.x < -input.match->getCourtSize().width * 0.25f)
			) ||
			(
				(input.paddle->getSide() == PaddleSide::RIGHT) &&
				(currBallPosition.x > input.match->getCourtSize().width * 0.25f)
			);
		return result;
	}

}
