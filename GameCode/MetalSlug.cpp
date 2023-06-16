#pragma once

#include<iostream>
#include<vector>
#include "../Util.cpp"
#include "../SDL2PlatformMethodsCollection.cpp"
#include "Animation.cpp"
#include "CollisionChecker.cpp"

namespace MetalSlug {

class MetalSlug {
private: 
	PlatformSpecficMethodsCollection *platformMethods; 

	Rect r     = {-0.5f, -0.5f, .2f, .2f};
	Rect rect2 = {0, 0, .2f, .2f};
	Circle circle1 = { 0, 0, .2 };
	Circle circle2 = { .5, .5, .2 };

	std::vector<std::string> frameFiles;
	Animation *tempAnim;
	PlatformSpecificImage* tempImg;

	// debug 
	GameText *frameMillis = NULL;
	GameText *fps = NULL;

	// temp
	Color collidedColor = {255, 0, 0, 255};
	Color rect1Color = {0, 255, 0, 255};
	Color rect2Color = {0, 0, 255, 255};
	Color circle1Color = {255, 255, 255, 255};
	Color circle2Color = {255, 255, 255, 255};

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
	int r1 = 255;
	int g1 = 255;
	int b1 = 255;
	
	int r2 = 255;
	int g2 = 255;
	int b2 = 0;
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
		circle1.x = r.x;
		circle1.y = r.y;

		//platformMethods->renderImage(tempImg);
		//platformMethods->fillRectangle(r);
		Point a = {0, 0};
		Point b = {-1, -1};


		if (CollisionChecker::doesRectVsRectCollide(r, rect2)) {
			rect1Color = collidedColor;
			rect2Color = collidedColor;
		}
		else if (CollisionChecker::doesCircleVsCircleCollide(circle1, circle2)) {
			circle2Color = collidedColor;
			circle1Color = collidedColor;
		}
		else if (CollisionChecker::doesCircleVsLineCollide(circle1, a, b)) {
			circle1Color = collidedColor;
		}
		else {
			rect1Color = {255, 255, 255, 255};
			rect2Color = {255, 255, 0, 255};
			circle1Color = {255, 255, 255, 255};
			circle2Color = {255, 255, 255, 255};
		}

		platformMethods->drawRectangle(r, rect1Color);
		platformMethods->drawRectangle(rect2, rect2Color);

		platformMethods->drawCircle(circle1, circle1Color);
		platformMethods->drawCircle(circle2, circle2Color);

		platformMethods->drawLine(a, b, rect1Color);

		/*
		tempAnim->changePos(r.x, r.y);
		tempAnim->animate(dt);
		*/
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