
#include <vector>
#include "riley-graphics-2d.h"
#pragma once

namespace pong {

	typedef struct Pong_BallState {
		float size;
		r3::graphics2d::Position2D position;
		r3::graphics2d::Vector2D direction;
	} BallState;

	typedef enum class Pong_PaddleSide {
		LEFT,
		RIGHT,
	} PaddleSide;

	typedef struct Pong_PaddleDefn {
		r3::graphics2d::Size2D courtSize;
		r3::graphics2d::Size2D paddleSize;
		PaddleSide side;
	} PaddleDefn;

	typedef struct Pong_MatchDefn {
		r3::graphics2d::Size2D courtSize;
		r3::graphics2d::Size2D paddleSize;
		float paddleSpeed;
		float ballSize;
		float ballSpeed;
	} MatchDefn;

	typedef enum class Pong_PaddleInputType {
		NONE,
		MOVE_UP,
		MOVE_DOWN,
	} PaddleInputType;

	typedef struct Pong_MatchInputRequest {
		PaddleInputType leftPaddleInput;
		PaddleInputType rightPaddleInput;
	} MatchInputRequest;

	typedef struct Pong_CourtCollisionSet {
		r3::graphics2d::LineSegment2D topWallLineSegment;
		r3::graphics2d::LineSegment2D bottomWallLineSegment;
		r3::graphics2d::LineSegment2D leftPaddleLineSegment;
		r3::graphics2d::LineSegment2D rightPaddleLineSegment;
	} CourtCollisionSet;

	typedef enum class Pong_BallCollisionTarget {
		NONE,
		TOP_WALL,
		BOTTOM_WALL,
		LEFT_PADDLE,
		RIGHT_PADDLE,
	} BallCollisionTarget;

	typedef struct Pong_BallCollisionResult {
		BallCollisionTarget collisionTarget;
		float percent;
		r3::graphics2d::Position2D collisionPoint;
		r3::graphics2d::Vector2D newDirection;
	} BallCollisionResult;

	typedef struct Pong_BallPathResult {
		std::vector<BallCollisionResult> collisionResultList;
		r3::graphics2d::Position2D newPosition;
		r3::graphics2d::Vector2D newDirection;
	} BallPathResult;

	typedef struct Pong_MatchUpdateResult {
		BallPathResult ballPath;
		bool leftScoredFlag;
		bool rightScoredFlag;
	} MatchUpdateResult;

	typedef enum class Pong_ClientMode {
		WAIT_TO_START,
		MATCH_RUNNING,
		MATCH_PAUSED,
		MATCH_WON,
	} ClientMode;

	class Paddle;
	class CourtCollisionCheckUtil;
	class Match;
	class GameClient;

	class Paddle {

	private:
		r3::graphics2d::Size2D courtSize;

		r3::graphics2d::Size2D size;
		PaddleSide side;
		r3::graphics2d::Position2D position;

	public:
		Paddle(const PaddleDefn* paddleDefn);

	public:
		r3::graphics2d::Size2D getSize() const;
		PaddleSide getSide() const;
		r3::graphics2d::Position2D getPosition() const;

	public:
		r3::graphics2d::LineSegment2D createCollisionLineSegment();

	public:
		float moveUp(float distance);
		float moveDown(float distance);

	};

	class CourtCollisionCheckUtil {

	private:
		const CourtCollisionSet* collisionSet;

	public:
		CourtCollisionCheckUtil(const CourtCollisionSet* collisionSet);

	public:
		static r3::graphics2d::Vector2D resolveNewDirectionForWallCollision(
			const r3::graphics2d::LineSegment2D* originalPathLineSegment
		);

		static r3::graphics2d::Vector2D resolveNewDirectionForPaddleCollision(
			const r3::graphics2d::LineSegment2D* originalPathLineSegment,
			const r3::graphics2d::Position2D* collisionPoint,
			const r3::graphics2d::LineSegment2D* paddleLineSegment
		);

		static r3::graphics2d::LineSegment2D adjustBallPathForWallCollision(
			const r3::graphics2d::LineSegment2D* originalPathLineSegment,
			const BallCollisionResult* collisionResult
		);

		static r3::graphics2d::LineSegment2D adjustBallPathForPaddleCollision(
			const r3::graphics2d::LineSegment2D* originalPathLineSegment,
			const BallCollisionResult* collisionResult,
			const r3::graphics2d::LineSegment2D* paddleLineSegment
		);

	public:
		BallCollisionResult detectNextCollision(const r3::graphics2d::LineSegment2D* ballPathLineSegment);
		r3::graphics2d::LineSegment2D adjustBallPath(const r3::graphics2d::LineSegment2D* originalPathLineSegment, const BallCollisionResult* collisionResult);

	};

	class Match {

	private:
		r3::graphics2d::Size2D courtSize;
		float paddleSpeed;
		float ballSpeed;

		Paddle* leftPaddle;
		Paddle* rightPaddle;

		int leftScore;
		int rightScore;

		BallState ballState;

		CourtCollisionSet collisionSet;

	public:
		Match(const MatchDefn* matchDefn);

	public:
		~Match();

	public:
		r3::graphics2d::Size2D getCourtSize() const;
		Paddle* getLeftPaddle() const;
		Paddle* getRightPaddle() const;
		BallState getBallState() const;
		int getLeftScore() const;
		int getRightScore() const;

	public:
		void startPoint(PaddleSide side);
		MatchUpdateResult update(const MatchInputRequest* input);

	private:
		void updatePaddles(const MatchInputRequest* input);
		BallPathResult resolveBallPath();
		void updateBall(const BallPathResult* ballPath);
		void updateScore(const MatchUpdateResult* matchUpdate);

	};

	class MatchRenderer {

	private:
		const Match* match;

	public:
		MatchRenderer(const Match* match);

	public:
		void renderWaitToStart();
		void renderMatchRunning();

	private:
		void clearScene();
		void prepareTransform();
		void renderCourt();
		void renderMatchScore();
		void renderMatchObjects();

	};

	class GameClient {

	private:
		ClientMode mode;

		Match* match;
		MatchRenderer* matchRenderer;

		int leftWinCount;
		int rightWinCount;

	public:
		GameClient();

	public:
		~GameClient();

	public:
		void update();
		void draw();

	private:
		bool pollWaitToStartInputs();
		MatchInputRequest pollMatchRunningInputs();

	};

}
