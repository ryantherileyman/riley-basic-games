
#include <vector>
#pragma once

namespace pong {

	typedef struct Pong_Size2D {
		float width;
		float height;
	} Size2D;

	typedef struct Pong_Vector2D {
		float x;
		float y;
	} Vector2D, Position2D;

	typedef struct Pong_LineSegment2D {
		Position2D point1;
		Position2D point2;
	} LineSegment2D;

	Vector2D createVectorFromLineSegment(const Position2D* point1, const Position2D* point2);
	Vector2D createVectorFromLineSegment(const LineSegment2D* lineSegment);
	float dotProduct(const Vector2D* vector1, const Vector2D* vector2);
	float crossProduct(const Vector2D* vector1, const Vector2D* vector2);

	typedef enum class Pong_LineSegment2DIntersectionType {
		NO_INTERSECTION,
		SINGLE_POINT,
		COLLINEAR_OVERLAP,
	} LineSegmentIntersectionType;

	typedef struct Pong_LineSegment2DIntersectionResult {
		LineSegmentIntersectionType intersectionType;
		Position2D intersectionPoint1;
		float percentPoint1;
		Position2D intersectionPoint2;
		float percentPoint2;
	} LineSegmentIntersectionResult;

	LineSegmentIntersectionResult checkLineSegmentsIntersect(const LineSegment2D* lineSegment1, const LineSegment2D* lineSegment2);

	typedef struct Pong_BallState {
		float size;
		Position2D position;
		Vector2D direction;
	} BallState;

	typedef enum class Pong_PaddleSide {
		LEFT,
		RIGHT,
	} PaddleSide;

	typedef struct Pong_PaddleDefn {
		Size2D courtSize;
		Size2D paddleSize;
		PaddleSide side;
	} PaddleDefn;

	typedef struct Pong_MatchDefn {
		Size2D courtSize;
		Size2D paddleSize;
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
		LineSegment2D topWallLineSegment;
		LineSegment2D bottomWallLineSegment;
		LineSegment2D leftPaddleLineSegment;
		LineSegment2D rightPaddleLineSegment;
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
		Position2D collisionPoint;
	} BallCollisionResult;

	typedef struct Pong_MatchUpdateResult {
		std::vector<BallCollisionResult> ballCollisionResultList;
		Position2D newBallPosition;
		Vector2D newBallDirection;
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
		Size2D courtSize;

		Size2D size;
		PaddleSide side;
		Position2D position;

	public:
		Paddle(const PaddleDefn* paddleDefn);

	public:
		LineSegment2D createCollisionLineSegment();

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
		BallCollisionResult detectNextCollision(const LineSegment2D* ballPathLineSegment);
		LineSegment2D adjustBallPath(const LineSegment2D* originalPathLineSegment, const BallCollisionResult* collisionResult);

	};

	class Match {

	private:
		Size2D courtSize;
		float paddleSpeed;
		float ballSpeed;

		Paddle* leftPaddle;
		Paddle* rightPaddle;

		int leftScore;
		int rightScore;

		BallState ballState;

		LineSegment2D topWallLineSegment;
		LineSegment2D bottomWallLineSegment;

	public:
		Match(const MatchDefn* matchDefn);

	public:
		~Match();

	public:


	public:
		MatchUpdateResult update(const MatchInputRequest* input);

	private:
		void updatePaddles(const MatchInputRequest* input);

	};

	class GameClient {

	private:
		ClientMode mode;

		Match* match;

		int leftWinCount;
		int rightWinCount;

	public:
		GameClient();

	};

}
