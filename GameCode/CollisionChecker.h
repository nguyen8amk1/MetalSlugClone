#pragma once
#include "MetalSlug.h"

namespace MetalSlug {

	struct CollisionInfo {
		int count;
		float depths[2];
		Vec2f contact_points[2];

		// always points from shape A to shape B (first and second shapes passed into
		// any of the c2***to***Manifold functions)
		Vec2f normal;
	};

    class CollisionChecker {
    public:
        static bool doesRectangleVsRectangleCollide(const Rect& r1, const Rect& r2);
        static void doesRectangleVsRectangleCollide(const Rect& r1, const Rect& r2, CollisionInfo& collisionInfo);
    };

} 
