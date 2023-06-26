#pragma once
#include "MetalSlug.h"

namespace MetalSlug {

struct AnimationMetaData {
	float animDelay;
	std::string tiledSheetFileName;
	Rect rect; 
	Vec2f relativeCorner;
	Vec2f framePixelSize;
	int rows, columns;
};
}