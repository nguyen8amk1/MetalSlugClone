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
	PlatformSpecificImage* backgroundImg;

	// debug 
	GameText *frameMillis = NULL;
	GameText *fps = NULL;
	GameText *playerXY = NULL;
	GameText *playerPhysicState = NULL;

	// TODO: change the coord conversion a little bit: 
	// from something
	// temp
	Point planeStart = {-10, -.25f};
	Point planeEnd = {10, -.25f};
	Vec2f playerPos = {-.5f, 0.6f};
	Point playerStart = {-.4f, .4f};
	Point playerEnd = {-.4f, 0.0f};
	float playerHalfWidth = .1f;
	Capsule player = {playerStart, playerEnd, playerHalfWidth};

	Color collidedColor = {255, 0, 0, 255};
	Color planeColor = {255, 255, 0, 255};
	Color playerColor = {0, 0, 255, 255};

	// TODO: write a physics state machine
	// let's draw a state machine transition map 
		// onground -> jump -> fall -> onground 

	// DO FALL AND ONGROUND FIRST @Current
	// FALL - pull down 
	// ONGROUND - y stick to the plane 

	enum PhysicState {
		ONGROUND, 
		JUMP, 
		FALL
	};

	PhysicState physicState = ONGROUND;

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
		
		frameMillis = platformMethods->createText(0, 0, 10);
		fps  = platformMethods->createText(0, 15, 10);
		playerXY  = platformMethods->createText(0, 30, 10);
		playerPhysicState  = platformMethods->createText(0, 45, 10);

		/*
		tempImg = platformMethods->loadImage("Assets/Imgs/sprites_cat_running_trans.png");
		Rect portionRect = {0, 0, 500, 200};
		tempImg = tempImg->getImagePortion(portionRect);
		*/

		backgroundImg = platformMethods->loadImage("Assets/Imgs/LevelsRawImage/metalslug_mission1_blank.png");
		float backgroundScaleX = 2;
		float backgroundScaleY = 2;
		Rect backgroundRect = {0, 0, backgroundScaleX*backgroundImg->getGameWidth(), backgroundScaleY*backgroundImg->getGameHeight()};
		backgroundImg->setRect(backgroundRect);
	}

	float tempSpeed = 1;
	float gravity = 1;
	float jumpT = 0;
	float jumpProgress = 0;
	float playerOriginalGroundY = 0;
	float jumpHeight = .5;

	void updateAndRender(GameInputContext &input, double dt) {
		// @StartTest: Camera
		// TODO: how to make it fill the screen in the right size 
		platformMethods->renderImage(backgroundImg);
		// @EndTest

		if (platformDebugInfo) {
			frameMillis->setText(Util::MessageFormater::print("Frametime Millis: ", platformDebugInfo->frameTimeMillis));
			fps->setText(Util::MessageFormater::print("FPS: ", platformDebugInfo->fps));

			platformMethods->drawText(frameMillis);
			platformMethods->drawText(fps);
		}

		// @StartTest: 
		if (input.pressLeft) {
			playerPos.x -= (float)(tempSpeed*dt); 
		}
		else if (input.pressRight) {
			playerPos.x += (float)(tempSpeed*dt); 
		}

		// Physics state machine
		if (physicState == FALL) {
			playerPos.y -= (float)(gravity*dt); 

			// event checking 
			if (CollisionChecker::doesCapsuleVsLineCollide(player, planeStart, planeEnd)) {
				physicState = ONGROUND;
			}
		}
		else if (physicState == ONGROUND) {
			if (!CollisionChecker::doesCapsuleVsLineCollide(player, planeStart, planeEnd)) {
				physicState = FALL;
			}
			else if(CollisionChecker::doesCapsuleVsLineCollide(player, planeStart, planeEnd) && 
					input.pressJump) {
				physicState = JUMP;
				jumpT = 0;
				// TODO: need to figure out collision handling before doing anything else - ASAP
				playerOriginalGroundY = playerPos.y; 
			}
		}
		else if (physicState == JUMP) {
			jumpT += gravity*dt;
			jumpProgress = -pow((jumpT*2-1), 2) + 1;
			playerPos.y = playerOriginalGroundY + (jumpHeight)*jumpProgress; 

			// event checking 
			/*
			if (CollisionChecker::doesCapsuleVsLineCollide(player, planeStart, planeEnd)) {
				physicState = ONGROUND;
			}
			*/
		}

		if (CollisionChecker::doesCapsuleVsLineCollide(player, planeStart, planeEnd)) {
			planeColor = collidedColor;
			playerColor = collidedColor;
		}
		else {
			planeColor = {255, 255, 0, 255};
			playerColor = {0, 0, 255, 255};
		}

		Point tempPoint = {playerPos.x, playerOriginalGroundY};
		platformMethods->drawPoint(playerPos, planeColor);
		platformMethods->drawPoint(tempPoint, planeColor);

		// Debug collision
		float d = fabs(player.start.y - player.end.y)/2;
		
		player.start.x = playerPos.x;
		player.start.y = playerPos.y + d;

		player.end.x = playerPos.x;
		player.end.y = playerPos.y - d;

		// @EndTest
		platformMethods->drawCapsule(player, playerColor);
		platformMethods->drawLine(planeStart, planeEnd, planeColor);

		// Debug info
		playerXY->setText(Util::MessageFormater::print("Player pos: ", playerPos.x, ", ", playerPos.y));
		platformMethods->drawText(playerXY);

		std::string physicStateStr;
		if (physicState == FALL) {
			physicStateStr = "FALL";
		}
		else if (physicState == ONGROUND) {
			physicStateStr = "ONGROUND";
		}
		else if (physicState == JUMP) {
			physicStateStr = "JUMP";
		}
		playerPhysicState->setText(Util::MessageFormater::print("Physic state: ", physicStateStr));
		platformMethods->drawText(playerPhysicState);
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