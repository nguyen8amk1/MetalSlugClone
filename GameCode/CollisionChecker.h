#pragma once

#include "MetalSlug.h"

namespace MetalSlug {

    class CollisionChecker {
    public:
        static bool doesRectangleVsRectangleCollide(const Rect& r1, const Rect& r2);

        static void doesRectangleVsRectangleCollide(const Rect& r1, const Rect& r2, CollisionInfo& collisionInfo);
    };

} 
