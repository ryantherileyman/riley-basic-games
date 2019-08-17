
#include "pong-lib.h"

namespace pong {

	using namespace r3::graphics2d;

	Match::Match(const MatchDefn* matchDefn) {
		PaddleDefn leftPaddleDefn;
		leftPaddleDefn.courtSize = matchDefn->courtSize;
		leftPaddleDefn.paddleSize = matchDefn->paddleSize;
		leftPaddleDefn.side = PaddleSide::LEFT;
		leftPaddleDefn.controlSource = matchDefn->leftPaddleControlSource;

		PaddleDefn rightPaddleDefn;
		rightPaddleDefn.courtSize = matchDefn->courtSize;
		rightPaddleDefn.paddleSize = matchDefn->paddleSize;
		rightPaddleDefn.side = PaddleSide::RIGHT;
		rightPaddleDefn.controlSource = matchDefn->rightPaddleControlSource;

		this->courtSize = matchDefn->courtSize;
		this->paddleSpeed = matchDefn->paddleSpeed;
		this->ballSpeed = matchDefn->ballSpeed;

		this->leftPaddle = new Paddle(&leftPaddleDefn);
		this->rightPaddle = new Paddle(&rightPaddleDefn);

		this->leftScore = 0;
		this->rightScore = 0;

		this->ballState.size = matchDefn->ballSize;
		this->ballState.position.x = 0;
		this->ballState.position.y = 0;
		this->ballState.direction.x = 1;
		this->ballState.direction.y = 0;

		this->collisionSet.topWallLineSegment.point1.x = -matchDefn->courtSize.width / 2 + (matchDefn->paddleSize.width * 2);
		this->collisionSet.topWallLineSegment.point1.y = matchDefn->courtSize.height / 2;
		this->collisionSet.topWallLineSegment.point2.x = matchDefn->courtSize.width / 2 - (matchDefn->paddleSize.width * 2);
		this->collisionSet.topWallLineSegment.point2.y = matchDefn->courtSize.height / 2;

		this->collisionSet.bottomWallLineSegment.point1.x = -matchDefn->courtSize.width / 2 + (matchDefn->paddleSize.width * 2);
		this->collisionSet.bottomWallLineSegment.point1.y = -matchDefn->courtSize.height / 2;
		this->collisionSet.bottomWallLineSegment.point2.x = matchDefn->courtSize.width / 2 - (matchDefn->paddleSize.width * 2);
		this->collisionSet.bottomWallLineSegment.point2.y = -matchDefn->courtSize.height / 2;
	}

	Match::~Match() {
		delete this->leftPaddle;
		delete this->rightPaddle;
	}

	Size2D Match::getCourtSize() const {
		return this->courtSize;
	}

	Paddle* Match::getLeftPaddle() const {
		return this->leftPaddle;
	}

	Paddle* Match::getRightPaddle() const {
		return this->rightPaddle;
	}

	BallState Match::getBallState() const {
		return this->ballState;
	}

	int Match::getLeftScore() const {
		return this->leftScore;
	}

	int Match::getRightScore() const {
		return this->rightScore;
	}

	LineSegment2D Match::getTopWallLineSegment() const {
		return this->collisionSet.topWallLineSegment;
	}

	LineSegment2D Match::getBottomWallLineSegment() const {
		return this->collisionSet.bottomWallLineSegment;
	}

	void Match::startPoint(PaddleSide side) {
		this->ballState.position.x = 0.0f;
		this->ballState.position.y = 0.0f;

		switch (side) {
		case PaddleSide::LEFT:
			this->ballState.direction.x = -1.0f;
			break;
		case PaddleSide::RIGHT:
			this->ballState.direction.x = 1.0f;
			break;
		}
		this->ballState.direction.y = 0.0f;
	}

	MatchUpdateResult Match::update(const MatchInputRequest* input) {
		this->updatePaddles(input);

		BallPathResult ballPath = this->resolveBallPath();
		this->updateBall(&ballPath);

		MatchUpdateResult result;
		result.ballPath = ballPath;
		result.leftScoredFlag = ballPath.newPosition.x > (this->courtSize.width / 2);
		result.rightScoredFlag = ballPath.newPosition.x < (-this->courtSize.width / 2);

		this->updateScore(&result);

		return result;
	}

	void Match::updatePaddles(const MatchInputRequest* input) {
		if (input->leftPaddleInput == PaddleInputType::MOVE_UP) {
			this->leftPaddle->moveUp(this->paddleSpeed);
		}
		if (input->leftPaddleInput == PaddleInputType::MOVE_DOWN) {
			this->leftPaddle->moveDown(this->paddleSpeed);
		}

		if (input->rightPaddleInput == PaddleInputType::MOVE_UP) {
			this->rightPaddle->moveUp(this->paddleSpeed);
		}
		if (input->rightPaddleInput == PaddleInputType::MOVE_DOWN) {
			this->rightPaddle->moveDown(this->paddleSpeed);
		}
	}

	BallPathResult Match::resolveBallPath() {
		LineSegment2D ballPathLineSegment;
		ballPathLineSegment.point1 = this->ballState.position;
		ballPathLineSegment.point2.x = this->ballState.position.x + (this->ballState.direction.x * this->ballSpeed);
		ballPathLineSegment.point2.y = this->ballState.position.y + (this->ballState.direction.y * this->ballSpeed);

		this->collisionSet.leftPaddleLineSegment = this->leftPaddle->createCollisionLineSegment();
		this->collisionSet.rightPaddleLineSegment = this->rightPaddle->createCollisionLineSegment();

		CourtCollisionCheckUtil* collisionCheckUtil = new CourtCollisionCheckUtil(&this->collisionSet);

		BallPathResult result;
		result.collisionResultList = std::vector<BallCollisionResult>();

		BallCollisionResult collisionResult = collisionCheckUtil->detectNextCollision(&ballPathLineSegment);
		while (collisionResult.collisionTarget != BallCollisionTarget::NONE) {
			result.collisionResultList.push_back(collisionResult);

			ballPathLineSegment = collisionCheckUtil->adjustBallPath(&ballPathLineSegment, &collisionResult);
			collisionResult = collisionCheckUtil->detectNextCollision(&ballPathLineSegment);
		}

		result.newPosition = ballPathLineSegment.point2;

		result.newDirection = this->ballState.direction;

		if (result.collisionResultList.size() > 0) {
			if (result.collisionResultList.back().collisionTarget != BallCollisionTarget::NONE) {
				result.newDirection = result.collisionResultList.back().newDirection;
			}
		}

		return result;
	}

	void Match::updateBall(const BallPathResult* ballPath) {
		this->ballState.position = ballPath->newPosition;
		this->ballState.direction = ballPath->newDirection;
	}

	void Match::updateScore(const MatchUpdateResult* matchUpdate) {
		if (matchUpdate->leftScoredFlag) {
			this->leftScore++;
		}
		if (matchUpdate->rightScoredFlag) {
			this->rightScore++;
		}
	}

}
