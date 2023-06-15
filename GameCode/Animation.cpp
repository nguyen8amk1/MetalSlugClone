#pragma once 
#include<vector>
#include<string>
#include "MetalSlug.h"
#include "../SDL2PlatformMethodsCollection.cpp"

namespace MetalSlug {

class Animation {
private:
	Rect rect; 
	std::vector<PlatformSpecificImage> frames; 
	
	int currentFrameIndex = 0;
	float timeAccumulator = 0.0f;
	float animDelay = 0.0f;

	PlatformSpecficMethodsCollection* platformMethods;
	
public: 
	Animation(float animDelay, std::vector<std::string> &frameFiles, PlatformSpecficMethodsCollection *platformMethods, Rect &rect) {
		this->platformMethods = platformMethods;
		this->animDelay = animDelay;

		this->rect = rect;

		// loading the frames;
		for (std::string filename: frameFiles) {
			PlatformSpecificImage img = this->platformMethods->loadImage(filename);

			// FIXME: this is a huge dependencies on the SDL2 system -> give it a 

			// @StartFix: textureRect is not an actual member of PlatformSpecificImage
			img.textureRect.x = rect.x;
			img.textureRect.y = rect.y;
			img.textureRect.w = rect.width;
			img.textureRect.h = rect.height;
			// @EndFix

			frames.push_back(img);
		}
	}

	void animate(double dt) {

		timeAccumulator += dt;
		if (timeAccumulator >= animDelay) {
			++currentFrameIndex %= frames.size();

			// FIXME: this is a huge dependencies on the SDL2 system -> give it a 
			// @StartFix: textureRect is not an actual member of PlatformSpecificImage
			frames[currentFrameIndex].textureRect.x = rect.x;
			frames[currentFrameIndex].textureRect.y = rect.y;
			frames[currentFrameIndex].textureRect.w = rect.width;
			frames[currentFrameIndex].textureRect.h = rect.height;
			// @EndFix

			timeAccumulator -= animDelay;
		}
		platformMethods->renderImage(frames[currentFrameIndex]);
	}

	void changePos(int x, int y) {
		rect.x = x;
		rect.y = y;
	}

	void changeSize(int width, int height) {
		rect.width = width;
		rect.height = height;
	}
};

}