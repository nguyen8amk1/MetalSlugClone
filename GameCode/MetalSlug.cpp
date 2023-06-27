#pragma once

#include<iostream>
#include<vector>
#include "../Util.cpp"
#include "../SDL2PlatformMethodsCollection.cpp"
#include "Animation.h"
#include "CollisionChecker.h"

#include "Hostage.cpp"
#include "Player.cpp"

namespace MetalSlug {

class MetalSlug {
private: 
	PlatformSpecficMethodsCollection *platformMethods; 
	Rect backgroundRect;

	std::vector<std::string> frameFiles;
	PlatformSpecificImage* backgroundImg;

	// debug 
	GameText *frameMillis = NULL;
	GameText *fps = NULL;
	GameText *playerXY = NULL;
	//GameText *groundPos = NULL;
	GameText *playerPhysicStateText = NULL;
	GameText *playerAnimationStateText = NULL;
	//GameText *backgroundRectText = NULL;

	std::vector<Rect> groundColliders;

	Color collidedColor = {255, 0, 0, 255};
	Color groundColor = {255, 255, 0, 255};
	Color playerColor = {0, 0, 255, 255};

	// NOTE: the only feature we have with the camera is move up, down, left, right 
	// no zoom, no scale -> fixed view port is 320 by 224
	Vec2f cameraPos = {-17.12425f, -0.357f}; // 17.12425 = bggamewidth/2 - half_world_size (1.43) 

	Player *player;


	// Level code
	float cameraMovePointX = -.42f;
	Rect preventGoingBackBlock = {-1.43f - .05f, 0, .1f, 2};

	// TODO: place 1 hostage into the scene  
	Rect hostageColliderRect;
	
	/*
	Hostage *hostage;
	Hostage *hostage1;
	Hostage *hostage2;
	*/

	std::vector<Hostage*> hostages;

	Rect convertLevelColliderBlockPixelRectToGameRect(const Rect& pixelRect, int backgroundPixelWidth, int backgroundPixelHeight) {
		float tx = backgroundPixelWidth/320.0f; 
		float ty = backgroundPixelHeight/224.0f; 

		float w = pixelRect.width/111.888f;
		float h = pixelRect.height/112.0f;

		float x = (pixelRect.x/111.888f) - 1.43*tx + w/2.0f;
		float y = ty - (2*ty*pixelRect.y/304.0f) - h/2.0f;

		return { x, y, w, h };
	}

public:
	MetalSlug(PlatformSpecficMethodsCollection *platformMethods) {
		this->platformMethods = platformMethods;
	}

	PlatformDebugInfo *platformDebugInfo = NULL;


	// NOTE: Player State Machine
	// Player or dynamic entity in the game have 2 state machine: 
	// Physic state machine
	// Animation state machine 
	// The animation state machine will sometimes based on the state of the physics state machine 
	void setup() {
		player = new Player(gravity, tempSpeed, platformMethods);

		int backgroundPixelWidth = 4152;
		int backgroundPixelHeight = 304;
		groundColliders.push_back(convertLevelColliderBlockPixelRectToGameRect({ 0, 252, 672, 52 }, backgroundPixelWidth, backgroundPixelHeight));
		groundColliders.push_back(convertLevelColliderBlockPixelRectToGameRect({ 660, 282, 1156, 22}, backgroundPixelWidth, backgroundPixelHeight));
		groundColliders.push_back(convertLevelColliderBlockPixelRectToGameRect({1815, 254, 65, 50}, backgroundPixelWidth, backgroundPixelHeight));
		groundColliders.push_back(convertLevelColliderBlockPixelRectToGameRect({851, 236, 71, 19}, backgroundPixelWidth, backgroundPixelHeight));
		groundColliders.push_back(convertLevelColliderBlockPixelRectToGameRect({932, 196, 184, 21}, backgroundPixelWidth, backgroundPixelHeight));
		groundColliders.push_back(convertLevelColliderBlockPixelRectToGameRect({932, 196, 184, 21}, backgroundPixelWidth, backgroundPixelHeight));
		groundColliders.push_back(convertLevelColliderBlockPixelRectToGameRect({1166, 194, 149, 19}, backgroundPixelWidth, backgroundPixelHeight));
		groundColliders.push_back(convertLevelColliderBlockPixelRectToGameRect({1350, 195, 174, 19}, backgroundPixelWidth, backgroundPixelHeight));
		groundColliders.push_back(convertLevelColliderBlockPixelRectToGameRect({1518, 237, 79, 19}, backgroundPixelWidth, backgroundPixelHeight));

		hostageColliderRect = convertLevelColliderBlockPixelRectToGameRect({1009, 100, 18, 38}, backgroundPixelWidth, backgroundPixelHeight);
		hostageColliderRect.width = .2f;
		hostageColliderRect.height = .4f;
		hostages.push_back(new Hostage(gravity, tempSpeed, hostageColliderRect, platformMethods));

		hostageColliderRect = convertLevelColliderBlockPixelRectToGameRect({1230, 100, 18, 38}, backgroundPixelWidth, backgroundPixelHeight);
		hostageColliderRect.width = .2f;
		hostageColliderRect.height = .4f;
		hostages.push_back(new Hostage(gravity, tempSpeed, hostageColliderRect, platformMethods));

		hostageColliderRect = convertLevelColliderBlockPixelRectToGameRect({1423, 100, 18, 38}, backgroundPixelWidth, backgroundPixelHeight);
		hostageColliderRect.width = .2f;
		hostageColliderRect.height = .4f;
		hostages.push_back(new Hostage(gravity, tempSpeed, hostageColliderRect, platformMethods));

		frameMillis = platformMethods->createText(0, 0, 10);
		fps  = platformMethods->createText(0, 15, 10);
		playerXY  = platformMethods->createText(0, 30, 10);

		playerPhysicStateText  = platformMethods->createText(0, 45, 10);
		playerAnimationStateText  = platformMethods->createText(0, 60, 10);

		/*
		backgroundRectText  = platformMethods->createText(0, 60, 10);
		groundPos  = platformMethods->createText(0, 75, 10);
		*/

		backgroundImg = platformMethods->loadImage("Assets/Imgs/LevelsRawImage/metalslug_mission1_blank.png");
		backgroundRect = {0, 0, backgroundImg->getGameWidth(), backgroundImg->getGameHeight()};

		// apply the camera 
		player->applyCamera(cameraPos);

		for (Rect &ground : groundColliders) {
			ground.x -= cameraPos.x;
			ground.y -= cameraPos.y;
		}

		for (Hostage *hostage: hostages) {
			hostage->moveXBy(-cameraPos.x);
			hostage->moveYBy(-cameraPos.y);
		}

		backgroundRect.x -= cameraPos.x;
		backgroundRect.y -= cameraPos.y;

		backgroundImg->setRect(backgroundRect);
	}

	float tempSpeed = 1;
	float gravity = 2.2f;

	// Camera 
	// NOTE: All the pos of entity in the game is in world space not camera space, 
	// the camera calculation is just for rendering   
	// NOTE: text not involve with the camera 

	LevelData levelData;
	void updateAndRender(GameInputContext &input, double dt) {
		backgroundImg->setRect(backgroundRect);
		platformMethods->renderImage(backgroundImg);
		//backgroundRectText->setText(Util::MessageFormater::print("bgrect: ", backgroundRect.x, ", ", backgroundRect.y));
		//platformMethods->drawText(backgroundRectText);

		// @EndTest

		if (platformDebugInfo) {
			frameMillis->setText(Util::MessageFormater::print("Frametime Millis: ", platformDebugInfo->frameTimeMillis));
			fps->setText(Util::MessageFormater::print("FPS: ", platformDebugInfo->fps));

			platformMethods->drawText(frameMillis);
			platformMethods->drawText(fps);
		}


		// @StartTest: Level 
		if (levelData.levelStarted) {
			if (levelData.playerColliderRect.x >= cameraMovePointX) {
				float d = tempSpeed * dt;
				cameraPos.x += d;

				player->moveXBy(-d);

				for (Rect &ground : groundColliders) {
					ground.x -= d;
				}

				backgroundRect.x -= d;

				for (Hostage *hostage: hostages) {
					hostage->moveXBy(-d);
				}
			}

			CollisionInfo colInfo;
			CollisionChecker::doesRectangleVsRectangleCollide(levelData.playerColliderRect, preventGoingBackBlock, colInfo);
			if (colInfo.count > 0) {
				player->moveXBy(-colInfo.normal.x * colInfo.depths[0]);
				player->moveYBy(-colInfo.normal.y * colInfo.depths[0]);
			}
		}

		levelData.groundColliders = groundColliders;
		player->update(input, levelData, dt);

		for (Hostage *hostage: hostages) {
			hostage->update(input, levelData, dt);
		}

		// @EndTest 

		// Debug info
		platformMethods->drawRectangle(levelData.playerColliderRect, playerColor);

		for (Rect &ground : groundColliders) {
			platformMethods->drawRectangle(ground, groundColor);
		}

		/*
		platformMethods->drawRectangle(preventGoingBackBlock, groundColor);
		Point cameraMoveLineStart = {cameraMovePointX, -1};
		Point cameraMoveLineEnd = {cameraMovePointX, 1};
		platformMethods->drawLine(cameraMoveLineStart, cameraMoveLineEnd, groundColor);
		*/

		playerXY->setText(Util::MessageFormater::print("Player pos: ", levelData.playerColliderRect.x, ", ", levelData.playerColliderRect.y));
		platformMethods->drawText(playerXY);

		//groundPos->setText(Util::MessageFormater::print("Ground pos: ", ground.x, ", ", ground.y));
		//platformMethods->drawText(groundPos);

		/*
		std::string physicStateStr;
		if (playerPhysicState == PlayerPhysicState::FALL) {
			physicStateStr = "FALL";
		}
		else if (playerPhysicState == PlayerPhysicState::ONGROUND) {
			physicStateStr = "ONGROUND";
		}
		else if (playerPhysicState == PlayerPhysicState::JUMPUP) {
			physicStateStr = "JUMPUP";
		}
		else if (playerPhysicState == PlayerPhysicState::JUMPDOWN) {
			physicStateStr = "JUMPDOWN";
		}

		std::string animationStateStr;
		if (playerAnimationState == PlayerAnimationState::FALLING) {
			animationStateStr = "FALL";
		}
		else if (playerAnimationState == PlayerAnimationState::IDLING) {
			animationStateStr = "IDLING";
		}
		else if (playerAnimationState == PlayerAnimationState::JUMPING) {
			animationStateStr = "JUMP";
		}
		else if (playerAnimationState == PlayerAnimationState::WALKING) {
			animationStateStr = "WALKING";
		}

		playerPhysicStateText->setText(Util::MessageFormater::print("Physic state: ", physicStateStr));
		platformMethods->drawText(playerPhysicStateText);

		playerAnimationStateText->setText(Util::MessageFormater::print("Animation state: ", animationStateStr));
		platformMethods->drawText(playerAnimationStateText);
		*/
	}
	
	~MetalSlug() {
		//delete playerHalfAnim;

		/*
		frameMillis->clean();
		fps->clean();
		*/

		delete frameMillis;
		delete fps;
	}
};

}