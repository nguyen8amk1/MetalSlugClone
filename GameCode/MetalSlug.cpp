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

	std::vector<std::string> frameFiles;
	Animation *tempAnim;
	PlatformSpecificImage* tempImg;

	// debug 
	GameText *frameMillis = NULL;
	GameText *fps = NULL;

	// TODO: change the coord conversion a little bit: 
	// from something
	// temp
	Point planeStart = {-1.0f, -.25f};
	Point planeEnd = {1.0f, -.25f};
	Vec2f playerPos = {-.5f, 0.0f};
	Point playerStart = {-.4f, .4f};
	Point playerEnd = {-.4f, 0.0f};
	float playerHalfWidth = .1f;
	Capsule player = {playerStart, playerEnd, playerHalfWidth};

	Color collidedColor = {255, 0, 0, 255};
	Color planeColor = {255, 255, 0, 255};
	Color playerColor = {0, 0, 255, 255};

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
		
		frameMillis = platformMethods->createText(0, 0);
		fps  = platformMethods->createText(0, 25);

		/*
		tempImg = platformMethods->loadImage("Assets/Imgs/sprites_cat_running_trans.png");
		Rect portionRect = {0, 0, 500, 200};
		tempImg = tempImg->getImagePortion(portionRect);
		*/
	}

	float tempSpeed = 1;
	// TODO: apply 
	// 
	void updateAndRender(GameInputContext &input, double dt) {
		if (platformDebugInfo) {
			frameMillis->setText(Util::MessageFormater::print("Frametime Millis: ", platformDebugInfo->frameTimeMillis));
			fps->setText(Util::MessageFormater::print("FPS: ", platformDebugInfo->fps));

			platformMethods->drawText(frameMillis);
			platformMethods->drawText(fps);
		}

		// @StartTest: 
		if (input.moveLeft) {
			playerPos.x -= (float)(tempSpeed*dt); 
		}
		else if (input.moveRight) {
			playerPos.x += (float)(tempSpeed*dt); 
		}
		if (input.moveUp) {
			playerPos.y += (float)(tempSpeed*dt); 
		}
		else if (input.moveDown) {
			playerPos.y -= (float)(tempSpeed*dt); 
		}

		// Debug collision
		if (CollisionChecker::doesCapsuleVsLineCollide(player, planeStart, planeEnd)) {
			planeColor = collidedColor;
			playerColor = collidedColor;
		}
		else {
			planeColor = {255, 255, 0, 255};
			playerColor = {0, 0, 255, 255};
		}

		float d = fabs(player.start.y - player.end.y)/2;

		player.start.x = playerPos.x;
		player.start.y = playerPos.y + d;

		player.end.x = playerPos.x;
		player.end.y = playerPos.y - d;

		// @EndTest
		platformMethods->drawCapsule(player, playerColor);
		platformMethods->drawLine(planeStart, planeEnd, planeColor);

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