#pragma once 

#include<iostream>
#include<sstream>
#include<string>
#include<stdint.h>
#include<SDL.h>


namespace SDL2Util {

const class NormalizeCoordConverter {
public:
	// -> formula:	for height: (x + 1)*300 with x in range [-1, 1]
	// ->			for width: (x + 4/3)*300 with x in range [-4/3, 4/3]

	// NOTE: this only works for 800 and 600 -> will find better formula later 
	static void normalizedCoordToPixelCoord(float normX, float normY, int &pixelX, int &pixelY) {
		pixelX = ((3*normX + 4) * 100.0f);
		pixelY = ((normY + 1)/2.0f)*600;
	}

	static void normalizedSizeToPixelSize(float normWidth, float normHeight, int &pixelWidth, int &pixelHeight) {
		pixelWidth = 300*normWidth;
		pixelHeight = 300*normHeight;
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