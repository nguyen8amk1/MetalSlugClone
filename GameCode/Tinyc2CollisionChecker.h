#pragma once 

#include<math.h>

namespace Tinyc2 {
#define c2Sin( radians ) sinf( radians )
#define c2Cos( radians ) cosf( radians )
#define c2Sqrt( a ) sqrtf( a )
#define c2Min( a, b ) ((a) < (b) ? (a) : (b))
#define c2Max( a, b ) ((a) > (b) ? (a) : (b))
#define c2Abs( a ) ((a) < 0 ? -(a) : (a))
#define c2Clamp( a, lo, hi ) c2Max( lo, c2Min( a, hi ) )

	typedef struct
	{
		float x;
		float y;
	} c2v;

	typedef struct
	{
		c2v min;
		c2v max;
	} c2AABB;

	typedef struct
	{
		int count;
		float depths[2];
		c2v contact_points[2];

		// always points from shape A to shape B (first and second shapes passed into
		// any of the c2***to***Manifold functions)
		c2v normal;
	} c2Manifold;

	const class CollisionChecker {
	public: 
		static int c2AABBtoAABB(c2AABB A, c2AABB B)
		{
			bool d0 = B.max.x < A.min.x;
			bool d1 = A.max.x < B.min.x;
			bool d2 = B.max.y < A.min.y;
			bool d3 = A.max.y < B.min.y;
			return !(d0 || d1 || d2 || d3);
		}

		static void c2AABBtoAABBManifold(c2AABB A, c2AABB B, c2Manifold* m)
		{
			m->count = 0;
			c2v mid_a = c2Mulvs(c2Add(A.min, A.max), 0.5f);
			c2v mid_b = c2Mulvs(c2Add(B.min, B.max), 0.5f);
			c2v eA = c2Absv(c2Mulvs(c2Sub(A.max, A.min), 0.5f));
			c2v eB = c2Absv(c2Mulvs(c2Sub(B.max, B.min), 0.5f));
			c2v d = c2Sub(mid_b, mid_a);

			// calc overlap on x and y axes
			float dx = eA.x + eB.x - c2Abs(d.x);
			if (dx < 0) return;
			float dy = eA.y + eB.y - c2Abs(d.y);
			if (dy < 0) return;

			c2v n;
			float depth;
			c2v p;

			// x axis overlap is smaller
			if (dx < dy)
			{
				depth = dx;
				if (d.x < 0)
				{
					n = c2V(-1.0f, 0);
					p = c2Sub(mid_a, c2V(eA.x, 0));
				}
				else
				{
					n = c2V(1.0f, 0);
					p = c2Add(mid_a, c2V(eA.x, 0));
				}
			}

			// y axis overlap is smaller
			else
			{
				depth = dy;
				if (d.y < 0)
				{
					n = c2V(0, -1.0f);
					p = c2Sub(mid_a, c2V(0, eA.y));
				}
				else
				{
					n = c2V(0, 1.0f);
					p = c2Add(mid_a, c2V(0, eA.y));
				}
			}

			m->count = 1;
			m->contact_points[0] = p;
			m->depths[0] = depth;
			m->normal = n;
		}

	private: 
		static c2v c2Mulvs(c2v a, float b) { a.x *= b; a.y *= b; return a; }
		static c2v c2Add(c2v a, c2v b) { a.x += b.x; a.y += b.y; return a; }
		static c2v c2V(float x, float y) { c2v a; a.x = x; a.y = y; return a; }
		static c2v c2Sub(c2v a, c2v b) { a.x -= b.x; a.y -= b.y; return a; }
		static c2v c2Absv(c2v a) { return c2V(c2Abs(a.x), c2Abs(a.y)); }
			
	};

}
