
#include "pong-lib.h"

namespace pong {

	Match::Match(const MatchDefn* matchDefn) {
		PaddleDefn leftPaddleDefn;
		leftPaddleDefn.courtSize = matchDefn->courtSize;
		leftPaddleDefn.paddleSize = matchDefn->paddleSize;
		leftPaddleDefn.side = PaddleSide::LEFT;

		PaddleDefn rightPaddleDefn;
		rightPaddleDefn.courtSize = matchDefn->courtSize;
		rightPaddleDefn.paddleSize = matchDefn->paddleSize;
		rightPaddleDefn.side = PaddleSide::RIGHT;

		this->courtSize = matchDefn->courtSize;
		this->paddleSpeed = 3;
		this->ballSpeed = 2;

		this->leftPaddle = new Paddle(&leftPaddleDefn);
		this->rightPaddle = new Paddle(&rightPaddleDefn);

		this->leftScore = 0;
		this->rightScore = 0;

		this->ballState.size = matchDefn->ballSize;
		this->ballState.position.x = 0;
		this->ballState.position.y = 0;
		this->ballState.direction.x = 1;
		this->ballState.direction.y = 0;

		this->topWallLineSegment.point1.x = -matchDefn->courtSize.width / 2;
		this->topWallLineSegment.point1.y = matchDefn->courtSize.height / 2;
		this->topWallLineSegment.point2.x = matchDefn->courtSize.width / 2;
		this->topWallLineSegment.point2.y = matchDefn->courtSize.height / 2;

		this->bottomWallLineSegment.point1.x = -matchDefn->courtSize.width / 2;
		this->bottomWallLineSegment.point1.y = -matchDefn->courtSize.height / 2;
		this->bottomWallLineSegment.point2.x = matchDefn->courtSize.width / 2;
		this->bottomWallLineSegment.point2.y = -matchDefn->courtSize.height / 2;
	}

	Match::~Match() {
		delete this->leftPaddle;
		delete this->rightPaddle;
	}

	MatchUpdateResult Match::update(const MatchInputRequest* input) {
		this->updatePaddles(input);

		LineSegment2D ballPathLineSegment;
		ballPathLineSegment.point1 = this->ballState.position;
		ballPathLineSegment.point2.x = this->ballState.position.x + (this->ballState.direction.x * this->ballSpeed);
		ballPathLineSegment.point2.y = this->ballState.position.y + (this->ballState.direction.y * this->ballSpeed);

		LineSegment2D leftPaddleLineSegment = this->leftPaddle->createCollisionLineSegment();
		LineSegment2D rightPaddleLineSegment = this->rightPaddle->createCollisionLineSegment();

		LineSegmentIntersectionResult topWallIntersectionResult = checkLineSegmentsIntersect(&ballPathLineSegment, &topWallLineSegment);
		if (topWallIntersectionResult.intersectionType == LineSegmentIntersectionType::SINGLE_POINT) {
			// TODO: check if remaining flight path will hit a paddle
		}

		LineSegmentIntersectionResult bottomWallIntersectionResult = checkLineSegmentsIntersect(&ballPathLineSegment, &bottomWallLineSegment);
		if (bottomWallIntersectionResult.intersectionType == LineSegmentIntersectionType::SINGLE_POINT) {
			// TODO: check if remaining flight path will hit a paddle
		}

		LineSegmentIntersectionResult leftPaddleIntersectionResult = checkLineSegmentsIntersect(&ballPathLineSegment, &leftPaddleLineSegment);

		LineSegmentIntersectionResult rightPaddleIntersectionResult = checkLineSegmentsIntersect(&ballPathLineSegment, &rightPaddleLineSegment);


		// TODO...
		MatchUpdateResult result;
		result.newBallPosition.x = 0;
		result.newBallPosition.y = 0;
		result.newBallDirection.x = 0;
		result.newBallDirection.y = 0;
		result.ballCollisionResultList = std::vector<BallCollisionResult>();
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

}
