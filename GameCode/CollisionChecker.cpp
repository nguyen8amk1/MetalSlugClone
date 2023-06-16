#define NOMINMAX
#include "MetalSlug.h"
#include<iostream>
#include<math.h>

namespace MetalSlug {

const class CollisionChecker {
public:
	static bool doesRectVsRectCollide(Rect& r1, Rect& r2) {
		// Calculate the boundaries of the rectangles
		float r1Left = r1.x;
		float r1Right = r1.x + r1.width;
		float r1Top = r1.y;
		float r1Bottom = r1.y + r1.height;

		float r2Left = r2.x;
		float r2Right = r2.x + r2.width;
		float r2Top = r2.y;
		float r2Bottom = r2.y + r2.height;

		// Check for collision
		bool collisionX = r1Left < r2Right && r1Right > r2Left;
		bool collisionY = r1Top < r2Bottom && r1Bottom > r2Top;

		return collisionX && collisionY;
	}

	static bool doesCircleVsCircleCollide(Circle &c1, Circle &c2) {
		float distance = std::sqrt(std::pow(c2.x - c1.x, 2) + std::pow(c2.y - c1.y, 2));

		// Check for collision by comparing the distance with the sum of the radii
		return distance <= (c1.r + c2.r);
	}

	// FIXME: this is not working very well though :v 
	static bool doesRectvsCircleCollide(Rect &r, Circle &c) {
		// Find the closest point on the rectangle to the center of the circle
		float closestX = fmaxf(r.x, fminf(c.x, r.x + r.width));
		float closestY = fmaxf(r.y, fminf(c.y, r.y + r.height));

		// Calculate the distance between the closest point and the center of the circle
		float distanceX = closestX - c.x;
		float distanceY = closestY - c.y;
		float distance = sqrtf(distanceX * distanceX + distanceY * distanceY);

		// Check if the distance is less than or equal to the radius of the circle
		return distance <= c.r;
	}

	static bool doesCircleVsLineCollide(struct Circle c, struct Point a, struct Point b) {
		// Calculate the distance between the center of the circle and the line segment
		float distance = calculateDistanceToLineSegment(c.x, c.y, a.x, a.y, b.x, b.y);
		// Check if the distance is less than or equal to the radius of the circle
		return distance <= c.r;
	}

private:
	static float calculateDistanceToLineSegment(float cx, float cy, float ax, float ay, float bx, float by) {
		float lineLength = calculateDistance(ax, ay, bx, by);
		if (lineLength == 0.0f) {
			// If the line segment has zero length, return the distance to one of the endpoints
			return calculateDistance(cx, cy, ax, ay);
		}

		// Calculate the normalized direction vector of the line segment
		float directionX = (bx - ax) / lineLength;
		float directionY = (by - ay) / lineLength;

		// Calculate the vector from one endpoint of the line segment to the center of the circle
		float vecX = cx - ax;
		float vecY = cy - ay;

		// Calculate the dot product between the vector and the direction vector of the line segment
		float dotProduct = vecX * directionX + vecY * directionY;

		// Clamp the dot product between 0 and the length of the line segment
		dotProduct = fmaxf(0.0f, fminf(dotProduct, lineLength));

		// Calculate the closest point on the line segment
		float closestX = ax + dotProduct * directionX;
		float closestY = ay + dotProduct * directionY;

		// Calculate the distance between the closest point and the center of the circle
		return calculateDistance(cx, cy, closestX, closestY);
	}

	static float calculateDistance(float x1, float y1, float x2, float y2) {
		float dx = x2 - x1;
		float dy = y2 - y1;
		return sqrtf(dx * dx + dy * dy);
	}

};

}
