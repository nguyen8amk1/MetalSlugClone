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
	GameText *groundPos = NULL;
	GameText *playerPhysicState = NULL;
	GameText *backgroundRectText = NULL;

	// temp
	/*
	Point groundStart = {-30, -.6f};
	Point groundEnd = {30, -.6f};
	*/

	Rect player = {-17, .3f, .2f, .2f};
	Rect ground = {0, -1.12f, 60, .488f};

	Color collidedColor = {255, 0, 0, 255};
	Color groundColor = {255, 255, 0, 255};
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

	// NOTE: the only feature we have with the camera is move up, down, left, right 
	// no zoom, no scale -> fixed view port is 320 by 224

	Vec2f cameraPos = {-17.12425f, -0.357f}; // 17.12425 = bggamewidth/2 - half_world_size (1.43) 
	//Vec2f cameraPos = {0, 0};

	void setup() {
		Rect animRect = { 0, 0, .5f, .8f};
		AnimationMetaData animMetaData;
		animMetaData.animDelay = .1f;
		animMetaData.animRect = animRect;
		animMetaData.spriteSheetFileName = "Assets/Imgs/Characters/Marco_Rossi_cut.png";

		/*
		Rect pixelRect;
		int frameWidth = 512;
		int frameHeight = 256;

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 4; j++) {
				pixelRect.width = (float)frameWidth;
				pixelRect.height = (float)frameHeight;
				pixelRect.x = (float)(j * pixelRect.width);
				pixelRect.y = (float)(i * pixelRect.height);

				animMetaData.framePixelRects.push_back(pixelRect);
			}
		}
		*/

		animMetaData.framePixelRects.push_back({10, 297, 29, 40});
		animMetaData.framePixelRects.push_back({44, 297, 29, 40});
		animMetaData.framePixelRects.push_back({78, 297, 29, 40});
		animMetaData.framePixelRects.push_back({112, 297, 32, 40});
		animMetaData.framePixelRects.push_back({149, 297, 32, 40});
		animMetaData.framePixelRects.push_back({186, 297, 35, 40});
		animMetaData.framePixelRects.push_back({226, 297, 34, 40});
		animMetaData.framePixelRects.push_back({261, 297, 34, 40});
		animMetaData.framePixelRects.push_back({300, 297, 29, 40});
		animMetaData.framePixelRects.push_back({334, 297, 29, 40});

		tempAnim = new Animation(animMetaData, platformMethods);
		
		frameMillis = platformMethods->createText(0, 0, 10);
		fps  = platformMethods->createText(0, 15, 10);
		playerXY  = platformMethods->createText(0, 30, 10);

		playerPhysicState  = platformMethods->createText(0, 45, 10);
		backgroundRectText  = platformMethods->createText(0, 60, 10);
		groundPos  = platformMethods->createText(0, 75, 10);

		/*
		tempImg = platformMethods->loadImage("Assets/Imgs/sprites_cat_running_trans.png");
		Rect portionRect = {0, 0, 500, 200};
		tempImg = tempImg->getImagePortion(portionRect);
		*/

		backgroundImg = platformMethods->loadImage("Assets/Imgs/LevelsRawImage/metalslug_mission1_blank.png");
		//backgroundRect = {25.1f, 0.357f, backgroundScale*backgroundImg->getGameWidth(), backgroundScale*backgroundImg->getGameHeight()};
		backgroundRect = {0, 0, backgroundImg->getGameWidth(), backgroundImg->getGameHeight()};

		// apply the camera 
		player.x -= cameraPos.x;
		player.y -= cameraPos.y;

		ground.x -= cameraPos.x;
		ground.y -= cameraPos.y;

		backgroundRect.x -= cameraPos.x;
		backgroundRect.y -= cameraPos.y;

		backgroundImg->setRect(backgroundRect);
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

	void updateAndRender(GameInputContext &input, double dt) {
		// @StartTest: Camera
		if (input.pressRightArrow) {
			float d = tempSpeed * dt;
			cameraPos.x += d;

			player.x -= d;
			ground.x -= d;
			backgroundRect.x -= d;
		}
		else if (input.pressLeftArrow) {
			float d = tempSpeed * dt;
			cameraPos.x -= d;

			player.x += d;

			ground.x += d;
			backgroundRect.x += d;
		}
		if (input.pressUpArrow) {
			float d = tempSpeed * dt;
			cameraPos.y += d;

			player.y -= d;

			ground.y -= d;
			backgroundRect.y -= d;
		}
		else if (input.pressDownArrow) {
			float d = tempSpeed * dt;
			cameraPos.y -= d;

			player.y += d;

			ground.y += d;
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
			player.x -= (float)(tempSpeed*dt); 
		}
		else if (input.pressRight) {
			player.x += (float)(tempSpeed*dt); 
		}

		// Physics state machine
		if (physicState == FALL) {
			player.y -= (float)(gravity*dt); 

			CollisionInfo colInfo;
			CollisionChecker::doesRectangleVsRectangleCollide(player, ground, colInfo);
			if (colInfo.count > 0) {
				physicState = ONGROUND;
				player.x -= colInfo.normal.x * colInfo.depths[0];
				player.y -= colInfo.normal.y * colInfo.depths[0];
			}
		}
		else if (physicState == ONGROUND) {
			if (!CollisionChecker::doesRectangleVsRectangleCollide(player, ground)) {
				physicState = FALL;
			}
			else if(CollisionChecker::doesRectangleVsRectangleCollide(player, ground) && 
					input.pressJump) {
				physicState = JUMPUP;
				jumpT = 0;
				playerOriginalGroundY = player.y; 
			}
		}
		else if (physicState == JUMPUP) {
			jumpT += gravity*dt;
			jumpProgress = -pow((jumpT-1), 2) + 1;
			player.y = playerOriginalGroundY + (jumpHeight)*jumpProgress; 

			// TODO: handle if sudden hit another object on the head -> physicState = FALL
			bool suddenHitPlatform = false;

			if (jumpT >= 1) {
				jumpT -= 1;
				physicState = JUMPDOWN;
			}
			else if (suddenHitPlatform) {
				physicState = FALL;
			}
		}
		else if (physicState == JUMPDOWN) {
			jumpT += gravity*dt;
			jumpProgress = -pow(jumpT, 2) + 1;
			player.y = playerOriginalGroundY + (jumpHeight)*jumpProgress; 

			CollisionInfo colInfo;
			CollisionChecker::doesRectangleVsRectangleCollide(player, ground, colInfo);
			if (colInfo.count > 0) {
				physicState = ONGROUND;

				player.x -= colInfo.normal.x * colInfo.depths[0];
				player.y -= colInfo.normal.y * colInfo.depths[0];
			}
		}

		if (CollisionChecker::doesRectangleVsRectangleCollide(player, ground)) {
			groundColor = collidedColor;
			playerColor = collidedColor;
		}
		else {
			groundColor = {255, 255, 0, 255};
			playerColor = {0, 0, 255, 255};
		}


		// Debug collision
		// @EndTest
		platformMethods->drawRectangle(player, playerColor);
		platformMethods->drawRectangle(ground, groundColor);

		tempAnim->animate(dt);

		// Debug info
		playerXY->setText(Util::MessageFormater::print("Player pos: ", player.x, ", ", player.y));
		platformMethods->drawText(playerXY);

		groundPos->setText(Util::MessageFormater::print("Ground pos: ", ground.x, ", ", ground.y));
		platformMethods->drawText(groundPos);

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