#define NOMINMAX
#include "MetalSlug.h"
#include<iostream>
#include<math.h>
#include "Tinyc2CollisionChecker.cpp"

namespace MetalSlug {

const class CollisionChecker {
public:
	static bool doesRectangleVsRectangleCollide(const Rect& r1, const Rect& r2) {
		float r1halfwidth = r1.width / 2.0f;
		float r1halfheight = r1.height / 2.0f;
		float r1minx = r1.x - r1halfwidth;
		float r1miny = r1.y - r1halfheight;
		float r1maxx = r1.x + r1halfwidth;
		float r1maxy = r1.y + r1halfheight;

		float r2halfwidth = r2.width / 2.0f;
		float r2halfheight = r2.height / 2.0f;
		float r2minx = r2.x - r2halfwidth;
		float r2miny = r2.y - r2halfheight;
		float r2maxx = r2.x + r2halfwidth;
		float r2maxy = r2.y + r2halfheight;

		Tinyc2::c2AABB a;
		a.min.x = r1minx;
		a.min.y = r1miny;
		a.max.x = r1maxx;
		a.max.y = r1maxy;

		Tinyc2::c2AABB b;
		b.min.x = r2minx;
		b.min.y = r2miny;
		b.max.x = r2maxx;
		b.max.y = r2maxy;

		return Tinyc2::CollisionChecker::c2AABBtoAABB(a, b);
	}

	static void doesRectangleVsRectangleCollide(const Rect& r1, const Rect& r2, CollisionInfo& collisionInfo) {
		float r1halfwidth = r1.width / 2.0f;
		float r1halfheight = r1.height / 2.0f;
		float r1minx = r1.x - r1halfwidth;
		float r1miny = r1.y - r1halfheight;
		float r1maxx = r1.x + r1halfwidth;
		float r1maxy = r1.y + r1halfheight;

		float r2halfwidth = r2.width / 2.0f;
		float r2halfheight = r2.height / 2.0f;
		float r2minx = r2.x - r2halfwidth;
		float r2miny = r2.y - r2halfheight;
		float r2maxx = r2.x + r2halfwidth;
		float r2maxy = r2.y + r2halfheight;

		Tinyc2::c2AABB a;
		a.min.x = r1minx;
		a.min.y = r1miny;
		a.max.x = r1maxx;
		a.max.y = r1maxy;

		Tinyc2::c2AABB b;
		b.min.x = r2minx;
		b.min.y = r2miny;
		b.max.x = r2maxx;
		b.max.y = r2maxy;

		Tinyc2::c2Manifold manifold;
		Tinyc2::CollisionChecker::c2AABBtoAABBManifold(a, b, &manifold);

		collisionInfo.contact_points[0].x = manifold.contact_points[0].x;
		collisionInfo.contact_points[0].y = manifold.contact_points[0].y;
		collisionInfo.contact_points[1].x = manifold.contact_points[1].x;
		collisionInfo.contact_points[1].y = manifold.contact_points[1].y;

		collisionInfo.count = manifold.count;

		collisionInfo.depths[0] = manifold.depths[0];
		collisionInfo.depths[1] = manifold.depths[1];

		collisionInfo.normal.x = manifold.normal.x;
		collisionInfo.normal.y = manifold.normal.y;
	}
};

}
