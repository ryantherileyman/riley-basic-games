
#pragma once

namespace r3 {
	namespace graphics2d {

		typedef struct r3_Size2D {
			float width;
			float height;
		} Size2D;

		typedef struct r3_Vector2D {
			float x;
			float y;
		} Vector2D, Position2D;

		typedef struct r3_LineSegment2D {
			Position2D point1;
			Position2D point2;
		} LineSegment2D;

		Vector2D createVectorFromLineSegment(const Position2D* point1, const Position2D* point2);
		Vector2D createVectorFromLineSegment(const LineSegment2D* lineSegment);

		float vectorLength(const Vector2D* vector);
		void normalizeVector(Vector2D* vector);
		float dotProduct(const Vector2D* vector1, const Vector2D* vector2);
		float crossProduct(const Vector2D* vector1, const Vector2D* vector2);

		float lineSegmentLength(const LineSegment2D* lineSegment);

		typedef enum class r3_LineSegment2DIntersectionType {
			NO_INTERSECTION,
			SINGLE_POINT,
			COLLINEAR_OVERLAP,
		} LineSegmentIntersectionType;

		typedef struct r3_LineSegment2DIntersectionResult {
			LineSegmentIntersectionType intersectionType;
			Position2D intersectionPoint1;
			float percentPoint1;
			Position2D intersectionPoint2;
			float percentPoint2;
		} LineSegmentIntersectionResult;

		LineSegmentIntersectionResult checkLineSegmentsIntersect(const LineSegment2D* lineSegment1, const LineSegment2D* lineSegment2);

	}
}
