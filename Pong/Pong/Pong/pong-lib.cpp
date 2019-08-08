
#include "pong-lib.h"

namespace pong {

	Vector2D createVectorFromLineSegment(const Position2D* point1, const Position2D* point2) {
		Vector2D result;
		result.x = point2->x - point1->x;
		result.y = point2->y - point1->y;
		return result;
	}

	Vector2D createVectorFromLineSegment(const LineSegment2D* lineSegment) {
		Vector2D result = createVectorFromLineSegment(&lineSegment->point1, &lineSegment->point2);
		return result;
	}

	float dotProduct(const Vector2D* vector1, const Vector2D* vector2) {
		float result = ((vector1->x * vector2->x) + (vector1->y * vector2->y));
		return result;
	}

	float crossProduct(const Vector2D* vector1, const Vector2D* vector2) {
		float result = ((vector1->x * vector2->y) - (vector1->y * vector2->x));
		return result;
	}

	LineSegmentIntersectionResult checkLineSegmentsIntersect(const LineSegment2D* lineSegment1, const LineSegment2D* lineSegment2) {
		const float EFFECTIVELY_ZERO = 1e-10;

		LineSegmentIntersectionResult result;
		result.intersectionType = LineSegmentIntersectionType::NO_INTERSECTION;
		result.intersectionPoint1.x = 0;
		result.intersectionPoint1.y = 0;
		result.percentPoint1 = 0;
		result.intersectionPoint2.x = 0;
		result.intersectionPoint2.y = 0;
		result.percentPoint2 = 0;

		Vector2D vector1 = createVectorFromLineSegment(lineSegment1);
		Vector2D vector2 = createVectorFromLineSegment(lineSegment2);

		float vector1CrossVector2 = crossProduct(&vector1, &vector2);

		Vector2D point1Vector = createVectorFromLineSegment(&lineSegment1->point1, &lineSegment2->point1);
		float point1VectorCrossVector1 = crossProduct(&point1Vector, &vector1);
		float point1VectorCrossVector2 = crossProduct(&point1Vector, &vector2);

		if (
			(fabs(vector1CrossVector2) < EFFECTIVELY_ZERO) &&
			(fabs(point1VectorCrossVector1) < EFFECTIVELY_ZERO)
		) {
			float vector1DotVector1 = dotProduct(&vector1, &vector1);

			float startPercent = dotProduct(&point1Vector, &vector1) / vector1DotVector1;
			float endPercent = startPercent + (dotProduct(&vector2, &vector1) / vector1DotVector1);

			if (
				(
					(startPercent >= 0) &&
					(startPercent <= 1)
				) ||
				(
					(endPercent >= 0) &&
					(endPercent <= 1)
				)
			) {
				result.intersectionType = LineSegmentIntersectionType::COLLINEAR_OVERLAP;

				if (startPercent < 0) startPercent = 0;
				if (startPercent > 1) startPercent = 1;
				result.intersectionPoint1.x = lineSegment1->point1.x + (vector1.x * startPercent);
				result.intersectionPoint1.y = lineSegment1->point1.y + (vector1.y * startPercent);
				result.percentPoint1 = startPercent;

				if (endPercent < 0) endPercent = 0;
				if (endPercent > 1) endPercent = 1;
				result.intersectionPoint2.x = lineSegment1->point1.x + (vector1.x * endPercent);
				result.percentPoint2 = endPercent;
				result.intersectionPoint2.y = lineSegment1->point1.y + (vector1.y * endPercent);

				if (fabsf(startPercent - endPercent) < EFFECTIVELY_ZERO) {
					result.intersectionType = LineSegmentIntersectionType::SINGLE_POINT;
				}
			}

			return result;
		}

		if (
			(fabs(vector1CrossVector2) < EFFECTIVELY_ZERO) &&
			!(fabs(point1VectorCrossVector1) < EFFECTIVELY_ZERO)
		) {
			return result;
		}

		float percentAlongLineSegment1 = point1VectorCrossVector2 / vector1CrossVector2;
		float percentAlongLineSegment2 = point1VectorCrossVector1 / vector1CrossVector2;

		if (
			!(fabs(vector1CrossVector2) < EFFECTIVELY_ZERO) &&
			(percentAlongLineSegment1 >= 0) &&
			(percentAlongLineSegment1 <= 1) &&
			(percentAlongLineSegment2 >= 0) &&
			(percentAlongLineSegment2 <= 1)
		) {
			result.intersectionType = LineSegmentIntersectionType::SINGLE_POINT;
			result.intersectionPoint1.x = lineSegment1->point1.x + (percentAlongLineSegment1 * vector1.x);
			result.intersectionPoint1.y = lineSegment1->point1.y + (percentAlongLineSegment1 * vector1.y);
			result.percentPoint1 = percentAlongLineSegment1;
		}

		return result;
	}

}
