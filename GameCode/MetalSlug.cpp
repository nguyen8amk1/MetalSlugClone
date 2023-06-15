#pragma once

#include<iostream>
#include<vector>
#include "../SDL2Util.cpp"
#include "../SDL2PlatformMethodsCollection.cpp"
#include "Animation.cpp"

namespace MetalSlug {

class MetalSlug {
private: 
	PlatformSpecficMethodsCollection *platformMethods; 
	Rect r = {0, 0, .1f, .1f};

public:
	MetalSlug(PlatformSpecficMethodsCollection *platformMethods) {
		this->platformMethods = platformMethods;
	}

	std::vector<std::string> frameFiles;
	Animation *tempAnim;
	PlatformSpecificImage* tempImg;

	void setup() {
		// load all the frames
		for (int i = 0; i < 8; i++) {
			frameFiles.push_back(SDL2Util::MessageFormater::print("Assets/Imgs/testCat/tile00", i, ".png"));
		}

		Rect animRect = { 0, 0, .5f, .3f};
		tempAnim = new Animation(0.1f, "Assets/Imgs/sprites_cat_running_trans.png", platformMethods, animRect, 2, 4);

		/*
		tempImg = platformMethods->loadImage("Assets/Imgs/sprites_cat_running_trans.png");
		Rect portionRect = {0, 0, 500, 200};
		tempImg = tempImg->getImagePortion(portionRect);
		*/
	}

	float tempSpeed = 1;
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

		//platformMethods->renderImage(tempImg);
		//platformMethods->fillRectangle(r);

		tempAnim->changePos(r.x, r.y);
		tempAnim->animate(dt);
	}
	
	~MetalSlug() {
		delete tempAnim;
	}
	
};

}