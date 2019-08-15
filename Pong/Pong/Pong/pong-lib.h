
#include <vector>
#include "riley-graphics-2d.h"
#pragma once

namespace pong {

	namespace PaddleSizeOptions {
		extern const float TINY;
		extern const float SMALL;
		extern const float MEDIUM;
		extern const float LARGE;
		extern const float ENORMOUS;
	}

	namespace BallSpeedOptions {
		extern const float SLOW;
		extern const float NORMAL;
		extern const float FAST;
		extern const float BLAZING;
		extern const float LUDICROUS;
	}

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
		MATCH_OPTIONS,
	} ClientMode;

	typedef struct Pong_MatchWonState {
		PaddleSide sideWon;
		int leftMatchWonCount;
		int rightMatchWonCount;
	} MatchWonState;

	typedef enum class Pong_MatchOptionsInputType {
		PREV_OPTION,
		NEXT_OPTION,
		PREV_VALUE,
		NEXT_VALUE,
	} MatchOptionsInputType;

	class Paddle;
	class CourtCollisionCheckUtil;
	class Match;
	class MatchRenderer;
	class MatchOptionsController;
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
		void renderMatchPaused();
		void renderMatchWon(MatchWonState matchWonState);
		void renderMatchOptions(const MatchOptionsController* matchOptionsController);

	private:
		void clearScene();
		void prepareTransform();
		void renderCourt();
		void renderMatchScore();
		void renderMatchObjects();

	};



	class MatchOptionsController {

	public:
		static const int OPTION_PADDLE_SIZE = 0;
		static const int OPTION_BALL_SPEED = 1;

		static const int PADDLE_SIZE_TINY = 0;
		static const int PADDLE_SIZE_SMALL = 1;
		static const int PADDLE_SIZE_MEDIUM = 2;
		static const int PADDLE_SIZE_LARGE = 3;
		static const int PADDLE_SIZE_ENORMOUS = 4;

		static const int BALL_SPEED_SLOW = 0;
		static const int BALL_SPEED_NORMAL = 1;
		static const int BALL_SPEED_FAST = 2;
		static const int BALL_SPEED_BLAZING = 3;
		static const int BALL_SPEED_LUDICROUS = 4;

	private:
		static int resolvePaddleSizeOptionValue(r3::graphics2d::Size2D paddleSize);
		static int resolveBallSpeedOptionValue(float ballSpeed);

	private:
		int currMatchOption;
		int currOptionValueArray[2];
		int maxOptionValueArray[2];

	public:
		MatchOptionsController(const MatchDefn* matchDefn);

	public:
		int getCurrOption() const;
		int getPaddleSizeOptionValue() const;
		int getBallSpeedOptionValue() const;

	public:
		r3::graphics2d::Size2D getPaddleSize();
		float getBallSpeed();

	public:
		void update(MatchOptionsInputType input);

	};

	class GameClient {

	private:
		MatchDefn matchDefn;
		ClientMode mode;

		Match* match;
		MatchRenderer* matchRenderer;
		MatchOptionsController* matchOptionsController;

		int matchWinThreshold;
		MatchWonState matchWonState;

	public:
		GameClient();

	public:
		~GameClient();

	public:
		void update();
		void draw();
		void processKeystroke(unsigned char key);
		void processSpecialKeystroke(int key);

	private:
		bool processWaitToStartKeystroke(unsigned char key);
		void processMatchRunningKeystroke(unsigned char key);
		void processMatchPausedKeystroke(unsigned char key);
		MatchInputRequest pollMatchRunningInputs();
		void processMatchOptionsKeystroke(unsigned char key);

	private:
		void startNewMatch();

	};

}
