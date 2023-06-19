#pragma once 

#include<iostream>
#include<sstream>
#include<string>
#include<stdint.h>
#include<SDL.h>

namespace SDL2Util {
const class NormalizeCoordConverter {
public:
	// NOTE: this one is still hard coded size :)) 
	static void normalizedCoordToPixelCoord(float normX, float normY, int &pixelX, int &pixelY) {
		//int pixelHeight = MetalSlug::MetalSlug::pixelScreenHeight;
		//int pixelWidth = MetalSlug::MetalSlug::pixelScreenWidth;
		//pixelY = (-pixelHeight / 2)* (normY + 1) + pixelHeight;
		pixelX = 111.888 * (normX + 1.43);
		pixelY = -112*(normY + 1) + 224;
	}

	static void normalizedSizeToPixelSize(float normWidth, float normHeight, int &pixelWidth, int &pixelHeight) {
		pixelWidth = (int)(111.888*normWidth);  // FIXME: still have something wrong with the size conversion 
		pixelHeight = (int)(112*normHeight); 
	}

	static void toMiddleOrigin(int &x, int &y, int width, int height) {
		x -= (width/2);
		y -= (height/2);
	}
};

const class TimeUtil {
public:
	static float SDLGetSecondsElapsed(int64_t OldCounter, int64_t CurrentCounter)
	{
		return ((float)(CurrentCounter - OldCounter) / (float)(SDL_GetPerformanceFrequency()));
	}
};

}