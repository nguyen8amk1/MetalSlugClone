#pragma once 
#include<vector>
#include<string>
#include "MetalSlug.h"
#include "../SDL2PlatformMethodsCollection.cpp"

namespace MetalSlug {

class Animation {
private:
	Rect rect; 
	PlatformSpecificImage frames[8]; // -> TODO: dynamically allocate this thing 
	
	int currentFrameIndex = 0;
	float timeAccumulator = 0.0f;
	float animDelay = 0.0f;

	PlatformSpecficMethodsCollection* platformMethods;
	
public: 
	Animation(float animDelay, std::vector<std::string> &frameFiles, PlatformSpecficMethodsCollection *platformMethods) {
		this->platformMethods = platformMethods;
		this->animDelay = animDelay;

		// loading the frames;
		for (int i = 0; i < 8; i++) {
			frames[i] = this->platformMethods->loadImage(frameFiles[i]);
		}
	}

	void animate(double dt) {

		timeAccumulator += dt;
		if (timeAccumulator >= animDelay) {
			++currentFrameIndex %= 8;

			frames[currentFrameIndex].textureRect.x = rect.x;
			frames[currentFrameIndex].textureRect.y = rect.y;

			timeAccumulator -= animDelay;
		}
		platformMethods->renderImage(frames[currentFrameIndex]);
	}

	void changePos(int x, int y) {
		rect.x = x;
		rect.y = y;
	}
};

}