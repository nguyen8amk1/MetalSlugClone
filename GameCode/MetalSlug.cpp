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
	GameText *backgroundRectText = NULL;

	// TODO: change the coord conversion a little bit: 
	// from something
	// temp
	Point planeStart = {-30, -.3f};
	Point planeEnd = {30, -.3f};

	Vec2f playerPos = {-17, 0.6f};
	Point playerStart = {-17, .4f};
	Point playerEnd = {-17, 0.0f};

	float playerHalfWidth = .1f;
	Capsule player = {playerStart, playerEnd, playerHalfWidth};

	Color collidedColor = {255, 0, 0, 255};
	Color planeColor = {255, 255, 0, 255};
	Color playerColor = {0, 0, 255, 255};

	enum PhysicState {
		ONGROUND, 
		JUMPUP, 
		JUMPDOWN, 
		FALL
	};

	PhysicState physicState = ONGROUND;

public:
	MetalSlug(PlatformSpecficMethodsCollection *platformMethods) {
		this->platformMethods = platformMethods;
	}

	PlatformDebugInfo *platformDebugInfo = NULL;
	Rect backgroundRect;
	float zoomFactor = 1.43333f; // to scale 224 as big as 320 -> scale by factor of 320/224
	//float zoomFactor = 1; 

	// TODO: SWITCH TO PLAYER WITH DIFFERENT SHAPE: SQUARE
	// IMPLEMENT THE SWEPTAABB 

	// SO THE POSITION STILL ONLY WORKS WITH A CERTAIN SCALE FACTOR, WE'LL JUST LIVE WITH THAT :)) 
	Vec2f cameraPos = {-17.5529f, -0.2472f};

	void setup() {
		// scale the camera 
		cameraPos.x *= zoomFactor;
		cameraPos.y *= zoomFactor;

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
		backgroundRectText  = platformMethods->createText(0, 60, 10);

		/*
		tempImg = platformMethods->loadImage("Assets/Imgs/sprites_cat_running_trans.png");
		Rect portionRect = {0, 0, 500, 200};
		tempImg = tempImg->getImagePortion(portionRect);
		*/

		backgroundImg = platformMethods->loadImage("Assets/Imgs/LevelsRawImage/metalslug_mission1_blank.png");
		//backgroundRect = {25.1f, 0.357f, backgroundScale*backgroundImg->getGameWidth(), backgroundScale*backgroundImg->getGameHeight()};
		backgroundRect = {0, 0, backgroundImg->getGameWidth(), backgroundImg->getGameHeight()};
		backgroundImg->setRect(backgroundRect);

		// change the position according to the scale 
		playerPos.x *= zoomFactor;
		playerPos.y *= zoomFactor;
		playerStart.x *= zoomFactor;
		playerStart.y *= zoomFactor;
		playerEnd.x *= zoomFactor;
		playerEnd.y *= zoomFactor;

		planeStart.x *= zoomFactor;
		planeStart.y *= zoomFactor;
		planeEnd.x *= zoomFactor;
		planeEnd.y *= zoomFactor;
		backgroundRect.x *= zoomFactor;
		backgroundRect.y *= zoomFactor;

		// enlarge numbers according to the scale as well 
		player.r *= zoomFactor;
		backgroundRect.width *= zoomFactor;
		backgroundRect.height *= zoomFactor;

		// apply the camera 
		playerPos.x -= cameraPos.x;
		playerPos.y -= cameraPos.y;
		playerStart.x -= cameraPos.x;
		playerStart.y -= cameraPos.y;
		playerEnd.x -= cameraPos.x;
		playerEnd.y -= cameraPos.y;

		planeStart.x -= cameraPos.x;
		planeStart.y -= cameraPos.y;
		planeEnd.x -= cameraPos.x;
		planeEnd.y -= cameraPos.y;
		backgroundRect.x -= cameraPos.x;
		backgroundRect.y -= cameraPos.y;
	}

	float tempSpeed = 1;
	float gravity = 2.2f;
	float jumpT = 0;
	float jumpProgress = 0;
	float playerOriginalGroundY = 0;
	float jumpHeight = .8f;

	// Camera 
	// NOTE: All the pos of entity in the game is in world space not camera space, 
	// the camera calculation is just for rendering   

	// NOTE: text not involve with the camera 
	// Problem: currently we don't have the WORLD SIZE and the CAMERA SIZE
	// how tf do we know how to render ?? (we already render it correctly but how ??) 
	// I know we scale the background is up to 1.43 to fit with the screen, but that's it :)) 
	// Problem: When changing the scale everything broke -> make things work independent off the scale 

	void updateAndRender(GameInputContext &input, double dt) {
		// @StartTest: Camera
		if (input.pressRightArrow) {
			float d = tempSpeed * dt;
			cameraPos.x += d;

			playerPos.x -= d;
			playerStart.x -= d;
			playerEnd.x -= d;
			planeStart.x -= d;
			planeEnd.x -= d;
			backgroundRect.x -= d;
		}
		else if (input.pressLeftArrow) {
			float d = tempSpeed * dt;
			cameraPos.x -= d;

			playerPos.x += d;
			playerStart.x += d;
			playerEnd.x += d;

			planeStart.x += d;
			planeEnd.x += d;
			backgroundRect.x += d;
		}
		if (input.pressUpArrow) {
			float d = tempSpeed * dt;
			cameraPos.y += d;

			playerPos.y -= d;
			playerStart.y -= d;
			playerEnd.y -= d;

			planeStart.y -= d;
			planeEnd.y -= d;
			backgroundRect.y -= d;
		}
		else if (input.pressDownArrow) {
			float d = tempSpeed * dt;
			cameraPos.y -= d;

			playerPos.y += d;
			playerStart.y += d;
			playerEnd.y += d;

			planeStart.y += d;
			planeEnd.y += d;
			backgroundRect.y += d;
		}

		backgroundImg->setRect(backgroundRect);
		backgroundRectText->setText(Util::MessageFormater::print("bgrect: ", backgroundRect.x, ", ", backgroundRect.y));
		platformMethods->renderImage(backgroundImg);
		platformMethods->drawText(backgroundRectText);

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
				physicState = JUMPUP;
				jumpT = 0;
				// TODO: need to figure out collision handling before doing anything else - ASAP
				playerOriginalGroundY = playerPos.y; 
			}
		}
		else if (physicState == JUMPUP) {
			jumpT += gravity*dt;
			jumpProgress = -pow((jumpT-1), 2) + 1;
			playerPos.y = playerOriginalGroundY + (jumpHeight)*jumpProgress; 
			bool suddenHitPlatform = false;
			if (jumpT >= 1) {
				jumpT -= 1;
				physicState = JUMPDOWN;
			}
			// TODO: handle if sudden hit another object on the head -> physicState = FALL
			else if (suddenHitPlatform) {
				physicState = FALL;
			}
		}
		else if (physicState == JUMPDOWN) {
			jumpT += gravity*dt;
			jumpProgress = -pow(jumpT, 2) + 1;
			playerPos.y = playerOriginalGroundY + (jumpHeight)*jumpProgress; 

			// event checking 
			if (CollisionChecker::doesCapsuleVsLineCollide(player, planeStart, planeEnd)) {
				physicState = ONGROUND;
			}
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
		else if (physicState == JUMPUP) {
			physicStateStr = "JUMPUP";
		}
		else if (physicState == JUMPDOWN) {
			physicStateStr = "JUMPDOWN";
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