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
	Rect r = {0, 0, .1f, .1f};

	std::vector<std::string> frameFiles;
	Animation *tempAnim;
	PlatformSpecificImage* tempImg;

	GameText *frameMillis = NULL;
	GameText *fps = NULL;

public:
	MetalSlug(PlatformSpecficMethodsCollection *platformMethods) {
		this->platformMethods = platformMethods;
	}

	PlatformDebugInfo *platformDebugInfo = NULL;

	void setup() {
		// load all the frames
		for (int i = 0; i < 8; i++) {
			frameFiles.push_back(Util::MessageFormater::print("Assets/Imgs/testCat/tile00", i, ".png"));
		}

		Rect animRect = { 0, 0, .5f, .3f};
		tempAnim = new Animation(0.1f, "Assets/Imgs/sprites_cat_running_trans.png", platformMethods, animRect, 2, 4);
		
		// TODO: figure out a way to bypass this 
		frameMillis = platformMethods->createText(0, 0);
		fps  = platformMethods->createText(0, 25);

		/*
		tempImg = platformMethods->loadImage("Assets/Imgs/sprites_cat_running_trans.png");
		Rect portionRect = {0, 0, 500, 200};
		tempImg = tempImg->getImagePortion(portionRect);
		*/
	}

	float tempSpeed = 1;
	void updateAndRender(GameInputContext &input, double dt) {
		if (platformDebugInfo) {
			frameMillis->setText(Util::MessageFormater::print("Frametime Millis: ", platformDebugInfo->frameTimeMillis));
			fps->setText(Util::MessageFormater::print("FPS: ", platformDebugInfo->fps));

			platformMethods->drawText(frameMillis);
			platformMethods->drawText(fps);
		}

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

		/*
		frameMillis->clean();
		fps->clean();
		*/

		delete frameMillis;
		delete fps;
	}
	
};

}