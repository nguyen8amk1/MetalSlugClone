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
		Rect animRect = { 100, 100, 100, 100 };
		tempAnim = new Animation(0.1f, frameFiles, platformMethods, animRect);
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

		/*
		tempAnim->changePos(r.x, r.y);
		tempAnim->changeSize(100, 100);
		*/

		platformMethods->fillRectangle(r);
		tempAnim->animate(dt);
	}
	
	~MetalSlug() {
		delete tempAnim;
	}
	
};

}