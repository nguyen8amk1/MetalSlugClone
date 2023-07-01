#pragma once 

#include "GameCode/Animation.h"
#include "GameCode/MetalSlug.h"

#include<iostream>
#include<sstream>
#include<string>

namespace Util {
const class MessageFormater {
	private: 
	template<typename T>
	static void appendToStream(std::ostringstream& oss, const T& arg) {
		oss << arg;
	}

	template<typename T, typename... Args>
	static void appendToStream(std::ostringstream& oss, const T& arg, const Args&... args) {
		oss << arg;
		appendToStream(oss, args...);
	}

	public: 
	template<typename... Args>
	static std::string print(const Args&... args) {
		std::ostringstream oss;
		appendToStream(oss, args...);
		return oss.str();
	}

};

const class Generator {
public: 
	static MetalSlug::Rect generatePixelRectFromCenter(float centerX, float centerY, float width, float height) {
		int halfWidth = width / 2;
		int halfHeight = height / 2;
		MetalSlug::Rect rect = {centerX - halfWidth, centerY - halfHeight, width, height};
		return rect;
	}
};

const class AnimationUtil {
public:
	static void initAnimationMetaData(MetalSlug::AnimationMetaData &metaData, const std::string &spriteSheetFilename, float animDelay, int rows, int columns, const MetalSlug::Vec2f &relativeCorner, const MetalSlug::Vec2f &framePixelSize) {
		metaData.animDelay = animDelay;
		metaData.tiledSheetFileName = spriteSheetFilename;
		metaData.rows = rows;
		metaData.columns = columns;
		metaData.relativeCorner = relativeCorner;
		metaData.framePixelSize = framePixelSize;

		float h = (metaData.framePixelSize.y/224.0f)*2;
		float w = h*(metaData.framePixelSize.y/metaData.framePixelSize.y); 
		MetalSlug::Rect animRect = {0, 0, fabs(w), fabs(h)};
		metaData.rect = animRect;
	}
};
const class LevelUtil {
public: 
	static MetalSlug::Rect convertLevelColliderBlockPixelRectToGameRect(const MetalSlug::Rect& pixelRect, int backgroundPixelWidth, int backgroundPixelHeight) {
		float tx = backgroundPixelWidth/304.0f; 
		float ty = backgroundPixelHeight/224.0f; 

		float w = pixelRect.width/106.2937f;
		float h = pixelRect.height/112.0f;

		float x = (pixelRect.x/106.2937f) - 1.43*tx + w/2.0f;
		float y = ty - (2*ty*pixelRect.y/backgroundPixelHeight) - h/2.0f;

		return { x, y, w, h };
	}
};

}