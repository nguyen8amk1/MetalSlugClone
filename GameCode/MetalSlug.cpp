#pragma once

#include<iostream>
#include<vector>
#include "../Util.cpp"
#include "../SDL2PlatformMethodsCollection.cpp"
#include "Animation.cpp"

namespace MetalSlug {

class MetalSlug {
private: 
	PlatformSpecficMethodsCollection *platformMethods; 
	Rect r = {50, 50, 50, 50};

public:
	MetalSlug(PlatformSpecficMethodsCollection *platformMethods) {
		this->platformMethods = platformMethods;
	}

	std::vector<std::string> frameFiles;
	Animation *tempAnim;

	void setup() {
		// load all the frames
		for (int i = 0; i < 8; i++) {
			frameFiles.push_back(Util::MessageFormater::print("Assets/Imgs/testCat/tile00", i, ".png"));
		}
		tempAnim = new Animation(.1f, frameFiles, platformMethods);
	}

	float tempSpeed = 500;
	void updateAndRender(GameInputContext &input, double dt) {

		// @StartTest: 
		if (input.moveLeft) {
			r.x -= tempSpeed*dt; 
		}
		else if (input.moveRight) {
			r.x += tempSpeed*dt; 
		}
		if (input.moveUp) {
			r.y -= tempSpeed*dt; 
		}
		else if (input.moveDown) {
			r.y += tempSpeed*dt; 
		}
		// @EndTest

		platformMethods->fillRectangle(r);
		tempAnim->animate(dt);
		tempAnim->changePos(r.x, r.y);
	}
	
	~MetalSlug() {
		delete tempAnim;
	}
	
};

}