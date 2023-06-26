#pragma once

#include<iostream>
#include<vector>
#include "../Util.cpp"
#include "../SDL2PlatformMethodsCollection.cpp"
#include "Animation.cpp"
#include "CollisionChecker.h"

#include "Hostage.cpp"

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

	Rect playerColliderRect = {-17.5f, .3f, .2f, .4f};
	Rect ground = {0, -1.12f, 60, .488f};

	Color collidedColor = {255, 0, 0, 255};
	Color groundColor = {255, 255, 0, 255};
	Color playerColor = {0, 0, 255, 255};

	enum class PlayerPhysicState {
		ONGROUND, 
		JUMPUP, 
		JUMPDOWN, 
		FALL
	};

	enum class PlayerAnimationState {
		IDLING, 
		JUMPING, 
		FALLING, 
		WALKING
	};

	PlayerPhysicState playerPhysicState = PlayerPhysicState::ONGROUND;
	PlayerAnimationState playerAnimationState = PlayerAnimationState::IDLING;

	// NOTE: the only feature we have with the camera is move up, down, left, right 
	// no zoom, no scale -> fixed view port is 320 by 224
	Vec2f cameraPos = {-17.12425f, -0.357f}; // 17.12425 = bggamewidth/2 - half_world_size (1.43) 

	// Player Animations 
	AnimationMetaData playerIdlingAnimationMetaData;
	AnimationMetaData playerJumpingAnimationMetaData;
	AnimationMetaData playerFallingAnimationMetaData;
	AnimationMetaData playerWalkingAnimationMetaData;

	Animation *playerIdlingAnimation;
	Animation *playerJumpingAnimation;
	Animation *playerFallingAnimation;
	Animation *playerWalkingAnimation;

	AnimationMetaData playerWalkingLegAnimationMetaData; 
	Animation *playerWalkingLegAnimation;

	AnimationMetaData playerIdlingLegAnimationMetaData; 
	Animation *playerIdlingLegAnimation;

	AnimationMetaData playerJumpingLegAnimationMetaData; 
	Animation *playerJumpingLegAnimation;

	AnimationMetaData playerFallingLegAnimationMetaData; 
	Animation *playerFallingLegAnimation;

	Animation *playerCurrentAnimation;
	Animation *playerCurrentLegAnimation;


	// Level code
	bool levelStarted = false;
	float cameraMovePointX = -.42f;
	Rect preventGoingBackBlock = {-1.43f - .05f, 0, .1f, 2};

	// TODO: place 1 hostage into the scene  
	Rect hostageColliderRect;
	Hostage *hostage;
	Hostage *hostage1;
	Hostage *hostage2;

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


	void playerInit() {
		std::string filename = "Assets/Imgs/Characters/marco_messi.png";
		Util::AnimationUtil::initAnimationMetaData(playerIdlingAnimationMetaData, filename, .15f, 1, 4, {0, 0}, {31, 29});
		playerIdlingAnimation  = new Animation(playerIdlingAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(playerWalkingAnimationMetaData, filename, .15f, 1, 12, { 0, 29 }, {34, 29});
		playerWalkingAnimation = new Animation(playerWalkingAnimationMetaData, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(playerJumpingAnimationMetaData, filename, .1f, 1, 6, {0, 64}, {29, 26});
		playerJumpingAnimation = new Animation(playerJumpingAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(playerFallingAnimationMetaData, filename, .1f, 1, 6, {145, 64}, {-29, 26});
		playerFallingAnimation = new Animation(playerFallingAnimationMetaData, platformMethods);

		// Player's Leg animation
		Util::AnimationUtil::initAnimationMetaData(playerWalkingLegAnimationMetaData, filename, .1f, 1, 12, {405, 44}, {32, 20} );
		playerWalkingLegAnimation  = new Animation(playerWalkingLegAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(playerIdlingLegAnimationMetaData, filename, .1f, 1, 1, {124, 13}, {21, 16});
		playerIdlingLegAnimation  = new Animation(playerIdlingLegAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(playerJumpingLegAnimationMetaData, filename, .1f, 1, 6, {174, 64}, {21, 25});
		playerJumpingLegAnimation  = new Animation(playerJumpingLegAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(playerFallingLegAnimationMetaData, filename, .1f, 1, 6, {279, 64}, {-21, 25});
		playerFallingLegAnimation = new Animation(playerFallingLegAnimationMetaData, platformMethods);

	}

	void playerUpdate(const GameInputContext &input, double dt) {
		// @StartTest: 
		if (input.pressLeft) {
			playerColliderRect.x -= (float)(tempSpeed*dt); 
		}
		else if (input.pressRight) {
			playerColliderRect.x += (float)(tempSpeed*dt); 
		}

		// Physics state machine
		if (playerPhysicState == PlayerPhysicState::FALL) {
			playerColliderRect.y -= (float)(gravity*dt); 

			CollisionInfo colInfo;
			CollisionChecker::doesRectangleVsRectangleCollide(playerColliderRect, ground, colInfo);
			if (colInfo.count > 0) {
				playerPhysicState = PlayerPhysicState::ONGROUND;
				playerColliderRect.x -= colInfo.normal.x * colInfo.depths[0];
				playerColliderRect.y -= colInfo.normal.y * colInfo.depths[0];
			}
		}
		else if (playerPhysicState == PlayerPhysicState::ONGROUND) {
			levelStarted = true;
			if (!CollisionChecker::doesRectangleVsRectangleCollide(playerColliderRect, ground)) {
				playerPhysicState = PlayerPhysicState::FALL;
			}
			else if(CollisionChecker::doesRectangleVsRectangleCollide(playerColliderRect, ground) && 
					input.pressJump) {
				playerPhysicState = PlayerPhysicState::JUMPUP;
				jumpT = 0;
				playerOriginalGroundY = playerColliderRect.y; 
			}
		}
		else if (playerPhysicState == PlayerPhysicState::JUMPUP) {
			jumpT += gravity*dt;
			jumpProgress = -pow((jumpT-1), 2) + 1;
			playerColliderRect.y = playerOriginalGroundY + (jumpHeight)*jumpProgress; 

			// TODO: handle if sudden hit another object on the head -> physicState = FALL
			bool suddenHitPlatform = false;

			if (jumpT >= 1) {
				jumpT -= 1;
				playerPhysicState = PlayerPhysicState::JUMPDOWN;
			}
			else if (suddenHitPlatform) {
				playerPhysicState = PlayerPhysicState::FALL;
			}
		}
		else if (playerPhysicState == PlayerPhysicState::JUMPDOWN) {
			jumpT += gravity*dt;
			jumpProgress = -pow(jumpT, 2) + 1;
			playerColliderRect.y = playerOriginalGroundY + (jumpHeight)*jumpProgress; 

			CollisionInfo colInfo;
			CollisionChecker::doesRectangleVsRectangleCollide(playerColliderRect, ground, colInfo);
			if (colInfo.count > 0) {
				playerPhysicState = PlayerPhysicState::ONGROUND;

				playerColliderRect.x -= colInfo.normal.x * colInfo.depths[0];
				playerColliderRect.y -= colInfo.normal.y * colInfo.depths[0];
			}
		}

		// animation state machine (sometimes the animation state based on the physics state)
		switch (playerAnimationState) {
		case PlayerAnimationState::IDLING: {
			bool onGround = playerPhysicState == PlayerPhysicState::ONGROUND;
			if (input.pressLeft) {
				playerAnimationState = PlayerAnimationState::WALKING;
				playerHorizontalFacingDirection = -1;
				playerCurrentAnimation = playerWalkingAnimation;
				playerCurrentLegAnimation = playerWalkingLegAnimation;
			}
			else if (input.pressRight) {
				playerAnimationState = PlayerAnimationState::WALKING;
				playerHorizontalFacingDirection = 1;
				playerCurrentAnimation = playerWalkingAnimation;
				playerCurrentLegAnimation = playerWalkingLegAnimation;
			}

			if (input.pressJump) {
				playerAnimationState = PlayerAnimationState::JUMPING;
				playerCurrentAnimation = playerJumpingAnimation;
				playerCurrentLegAnimation = playerJumpingLegAnimation;
			}
			else if (!onGround) {
				playerAnimationState = PlayerAnimationState::FALLING;
				playerCurrentAnimation = playerFallingAnimation;
				playerCurrentLegAnimation = playerFallingLegAnimation;
			}
		}break;

		case PlayerAnimationState::WALKING: {

			bool isPressingMove = input.pressLeft || input.pressRight;
			if (!isPressingMove && 
				playerPhysicState != PlayerPhysicState::JUMPUP && 
				playerPhysicState != PlayerPhysicState::JUMPDOWN && 
				playerPhysicState != PlayerPhysicState::FALL) {

				playerAnimationState = PlayerAnimationState::IDLING;
				playerCurrentAnimation = playerIdlingAnimation;
				playerCurrentLegAnimation = playerIdlingLegAnimation;
			}
			else if (playerPhysicState == PlayerPhysicState::JUMPUP) {
				playerAnimationState = PlayerAnimationState::JUMPING;
				playerCurrentAnimation = playerJumpingAnimation;
				playerCurrentLegAnimation = playerJumpingLegAnimation;
			}
			else if (playerPhysicState == PlayerPhysicState::FALL) {
				playerAnimationState = PlayerAnimationState::FALLING;
				playerCurrentAnimation = playerFallingAnimation;
				playerCurrentLegAnimation = playerFallingLegAnimation;
			}
		} break;

		case PlayerAnimationState::JUMPING: {
			if (playerPhysicState == PlayerPhysicState::JUMPDOWN ||
				playerPhysicState == PlayerPhysicState::FALL) {

				playerAnimationState = PlayerAnimationState::FALLING;
				playerCurrentAnimation = playerFallingAnimation;
				playerCurrentLegAnimation = playerFallingLegAnimation;
			}

		} break;

		case PlayerAnimationState::FALLING: {
			if (playerPhysicState == PlayerPhysicState::ONGROUND) {
				playerAnimationState = PlayerAnimationState::IDLING;
				playerCurrentAnimation = playerIdlingAnimation;
				playerCurrentLegAnimation = playerIdlingLegAnimation;
			}
		} break;

		default: 
			// TODO: handle error "Animation state not recoginize"
			break;
		}


		if (CollisionChecker::doesRectangleVsRectangleCollide(playerColliderRect, ground)) {
			groundColor = collidedColor;
			playerColor = collidedColor;
		}
		else {
			groundColor = {255, 255, 0, 255};
			playerColor = {0, 0, 255, 255};
		}

		// @EndTest
		float legX = playerColliderRect.x;
		float legY = playerColliderRect.y - .15f;
		playerCurrentLegAnimation->changePos(legX, legY);
		playerCurrentLegAnimation->flip(playerHorizontalFacingDirection, 1);
		playerCurrentLegAnimation->animate(dt);

		playerCurrentAnimation->changePos(playerColliderRect.x, playerColliderRect.y);
		playerCurrentAnimation->flip(playerHorizontalFacingDirection, 1);
		playerCurrentAnimation->animate(dt);

	}


	void setup() {
		hostageColliderRect = { .4f, 0, .2f, .4f };
		hostage = new Hostage(gravity, tempSpeed, hostageColliderRect, platformMethods);

		hostageColliderRect = { 1, 0, .2f, .4f };
		hostage1 = new Hostage(gravity, tempSpeed, hostageColliderRect, platformMethods);

		hostageColliderRect = { 2, 0, .2f, .4f };
		hostage2 = new Hostage(gravity, tempSpeed, hostageColliderRect, platformMethods);

		playerInit();

		playerCurrentAnimation = playerIdlingAnimation;
		playerCurrentLegAnimation = playerIdlingLegAnimation;



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
		playerColliderRect.x -= cameraPos.x;
		playerColliderRect.y -= cameraPos.y;

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
	float jumpHeight = .5777f;
	float playerHorizontalFacingDirection = 1;

	// Camera 
	// NOTE: All the pos of entity in the game is in world space not camera space, 
	// the camera calculation is just for rendering   
	// NOTE: text not involve with the camera 

	LevelData levelData;
	void updateAndRender(GameInputContext &input, double dt) {
		/*
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
		*/

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

		playerUpdate(input, dt);

		levelData.groundCollider = ground;
		levelData.playerColliderRect = playerColliderRect;

		hostage->update(input, levelData, dt);
		hostage1->update(input, levelData, dt);
		hostage2->update(input, levelData, dt);

		// @StartTest: Level 
		if (levelStarted) {
			if (playerColliderRect.x >= cameraMovePointX) {
				float d = tempSpeed * dt;
				cameraPos.x += d;

				playerColliderRect.x -= d;
				ground.x -= d;
				backgroundRect.x -= d;

				hostage->moveXBy(-d);
				hostage1->moveXBy(-d);
				hostage2->moveXBy(-d);
			}

			CollisionInfo colInfo;
			CollisionChecker::doesRectangleVsRectangleCollide(playerColliderRect, preventGoingBackBlock, colInfo);
			if (colInfo.count > 0) {
				playerColliderRect.x -= colInfo.normal.x * colInfo.depths[0];
				playerColliderRect.y -= colInfo.normal.y * colInfo.depths[0];
			}
		}
		// @EndTest 


		// Debug info
		platformMethods->drawRectangle(playerColliderRect, playerColor);
		platformMethods->drawRectangle(ground, groundColor);

		/*
		platformMethods->drawRectangle(preventGoingBackBlock, groundColor);
		Point cameraMoveLineStart = {cameraMovePointX, -1};
		Point cameraMoveLineEnd = {cameraMovePointX, 1};
		platformMethods->drawLine(cameraMoveLineStart, cameraMoveLineEnd, groundColor);
		*/

		playerXY->setText(Util::MessageFormater::print("Player pos: ", playerColliderRect.x, ", ", playerColliderRect.y));
		platformMethods->drawText(playerXY);

		//groundPos->setText(Util::MessageFormater::print("Ground pos: ", ground.x, ", ", ground.y));
		//platformMethods->drawText(groundPos);

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