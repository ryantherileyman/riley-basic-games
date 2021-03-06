
#include <vector>
#include <random>
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

	typedef enum class Pong_PaddleControlSource {
		PLAYER,
		AI_GUESSER,
		AI_LATE_FOLLOWER,
		AI_FOLLOWER,
		AI_CLOSE_FOLLOWER,
		AI_SNOOKER_PRO,
	} PaddleControlSource;

	typedef struct Pong_PaddleDefn {
		r3::graphics2d::Size2D courtSize;
		r3::graphics2d::Size2D paddleSize;
		PaddleSide side;
		PaddleControlSource controlSource;
	} PaddleDefn;

	typedef struct Pong_MatchDefn {
		r3::graphics2d::Size2D courtSize;
		r3::graphics2d::Size2D paddleSize;
		float paddleSpeed;
		PaddleControlSource leftPaddleControlSource;
		PaddleControlSource rightPaddleControlSource;
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

	class PaddleAi;
	class GuesserPaddleAi;
	class FollowerPaddleAi;
	class SnookerProPaddleAi;
	class Paddle;
	class CourtCollisionCheckUtil;
	class Match;
	class MatchRenderer;
	class MatchOptionsController;
	class GameClient;

	typedef struct Pong_PaddleAiInput {
		const Paddle* paddle;
		const Match* match;
	} PaddleAiInput;

	class PaddleAi {
	public:
		virtual PaddleInputType resolvePaddleInputType(PaddleAiInput input) = 0;
	};

	class GuesserPaddleAi : public PaddleAi {

	private:
		std::default_random_engine generator;
		std::uniform_int_distribution<int> changeDirectionChanceDistribution = std::uniform_int_distribution<int>(1, 100);
		std::uniform_int_distribution<int> newDirectionDistribution = std::uniform_int_distribution<int>(1, 3);

		PaddleInputType currInput = PaddleInputType::NONE;

	public:
		GuesserPaddleAi();

	public:
		PaddleInputType resolvePaddleInputType(PaddleAiInput input);

	};

	typedef struct Pong_FollowerPaddleAiDefn {
		float paddleHeightMultiplier;
		bool onlyFollowIfBallIsApproaching;
	} FollowerPaddleAiDefn;

	class FollowerPaddleAi : public PaddleAi {

	private:
		FollowerPaddleAiDefn aiDefn;

	public:
		FollowerPaddleAi(const FollowerPaddleAiDefn* aiDefn);

	public:
		PaddleInputType resolvePaddleInputType(PaddleAiInput input);

	};

	class SnookerProPaddleAi : public PaddleAi {

	private:
		bool courtCollisionSetInitializedFlag = false;
		CourtCollisionSet collisionSet;

		float prevBallDirectionX = 0.0f;
		float desiredPaddlePosition = 0.0f;
		bool performedDeflectionAdjustmentFlag = false;

		std::default_random_engine generator;
		std::uniform_real_distribution<float> paddleDeflectionDistribution = std::uniform_real_distribution<float>(-0.5f, 0.5f);

	public:
		SnookerProPaddleAi();

	public:
		PaddleInputType resolvePaddleInputType(PaddleAiInput input);

	private:
		void initializeCollisionSet(PaddleAiInput input);

	private:
		bool ballHeadedTowardPaddle(PaddleAiInput input);
		float calculateYPositionBallWillCrossPlaneOfPaddle(PaddleAiInput input);

	private:
		bool ballNearPaddle(PaddleAiInput input);

	};

	class Paddle {

	private:
		r3::graphics2d::Size2D courtSize;

		r3::graphics2d::Size2D size;
		PaddleSide side;
		r3::graphics2d::Position2D position;

		PaddleControlSource controlSource;

		PaddleAi* ai;

	public:
		Paddle(const PaddleDefn* paddleDefn);

	public:
		~Paddle();

	public:
		r3::graphics2d::Size2D getSize() const;
		PaddleSide getSide() const;
		r3::graphics2d::Position2D getPosition() const;
		PaddleControlSource getControlSource() const;

	public:
		r3::graphics2d::LineSegment2D createCollisionLineSegment();

	public:
		float moveUp(float distance);
		float moveDown(float distance);

	public:
		PaddleInputType resolvePaddleInputType(const Match* match);

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
		r3::graphics2d::LineSegment2D getTopWallLineSegment() const;
		r3::graphics2d::LineSegment2D getBottomWallLineSegment() const;

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
		static const int OPTION_LEFT_PADDLE_CONTROL_SOURCE = 2;
		static const int OPTION_RIGHT_PADDLE_CONTROL_SOURCE = 3;

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

		static const int PADDLE_CONTROL_SOURCE_PLAYER = 0;
		static const int PADDLE_CONTROL_SOURCE_AI_GUESSER = 1;
		static const int PADDLE_CONTROL_SOURCE_AI_LATE_FOLLOWER = 2;
		static const int PADDLE_CONTROL_SOURCE_AI_FOLLOWER = 3;
		static const int PADDLE_CONTROL_SOURCE_AI_CLOSE_FOLLOWER = 4;
		static const int PADDLE_CONTROL_SOURCE_AI_SNOOKER_PRO = 5;

	private:
		static int resolvePaddleSizeOptionValue(r3::graphics2d::Size2D paddleSize);
		static int resolveBallSpeedOptionValue(float ballSpeed);
		static int resolvePaddleControlSourceValue(PaddleControlSource paddleControlSource);

	private:
		int currMatchOption;
		int currOptionValueArray[4];
		int maxOptionValueArray[4];

	public:
		MatchOptionsController(const MatchDefn* matchDefn);

	public:
		int getCurrOption() const;
		int getPaddleSizeOptionValue() const;
		int getBallSpeedOptionValue() const;
		int getLeftPaddleControlSourceValue() const;
		int getRightPaddleControlSourceValue() const;

	public:
		r3::graphics2d::Size2D getPaddleSize();
		float getBallSpeed();
		PaddleControlSource getLeftPaddleControlSource();
		PaddleControlSource getRightPaddleControlSource();

	private:
		static PaddleControlSource getPaddleControlSource(int paddleControlSourceValue);

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
