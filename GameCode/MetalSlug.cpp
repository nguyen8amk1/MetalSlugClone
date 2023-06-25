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

	Rect player = {-17, .3f, .2f, .4f};
	Rect ground = {0, -1.12f, 60, .488f};

	Color collidedColor = {255, 0, 0, 255};
	Color groundColor = {255, 255, 0, 255};
	Color playerColor = {0, 0, 255, 255};

	enum PlayerPhysicState {
		ONGROUND, 
		JUMPUP, 
		JUMPDOWN, 
		FALL
	};

	enum PlayerAnimationState {
		IDLING, 
		JUMPING, 
		FALLING, 
		WALKING
	};

	PlayerPhysicState playerPhysicState = ONGROUND;
	PlayerAnimationState playerAnimationState = IDLING;

public:

	MetalSlug(PlatformSpecficMethodsCollection *platformMethods) {
		this->platformMethods = platformMethods;
	}

	PlatformDebugInfo *platformDebugInfo = NULL;

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

	Rect animRect;


	// NOTE: Player State Machine
	// Player or dynamic entity in the game have 2 state machine: 
	// Physic state machine
	// Animation state machine 
	// The animation state machine will sometimes based on the state of the physics state machine 

	// TODO: let's just clean the player code a little bit 
	void initAnimationMetaData(AnimationMetaData &metaData, const std::string &spriteSheetFilename, float animDelay, int rows, int columns, const Vec2f &relativeCorner, const Vec2f &framePixelSize) {
		metaData.animDelay = animDelay;
		metaData.tiledSheetFileName = spriteSheetFilename;
		metaData.rows = rows;
		metaData.columns = columns;
		metaData.relativeCorner = relativeCorner;
		metaData.framePixelSize = framePixelSize;

		float h = (metaData.framePixelSize.y/224.0f)*2;
		float w = h*(metaData.framePixelSize.y/metaData.framePixelSize.y); 
		animRect = {0, 0, fabs(w), fabs(h)};
		metaData.rect = animRect;
	}

	void playerInit() {
		std::string filename = "Assets/Imgs/Characters/marco_messi.png";
		initAnimationMetaData(playerIdlingAnimationMetaData, filename, .15f, 1, 4, {0, 0}, {31, 29});
		playerIdlingAnimation  = new Animation(playerIdlingAnimationMetaData,platformMethods);

		initAnimationMetaData(playerWalkingAnimationMetaData, filename, .15f, 1, 12, { 0, 29 }, {34, 29});
		playerWalkingAnimation = new Animation(playerWalkingAnimationMetaData, platformMethods);

		initAnimationMetaData(playerJumpingAnimationMetaData, filename, .1f, 1, 6, {0, 64}, {29, 26});
		playerJumpingAnimation = new Animation(playerJumpingAnimationMetaData,platformMethods);

		initAnimationMetaData(playerFallingAnimationMetaData, filename, .1f, 1, 6, {145, 64}, {-29, 26});
		playerFallingAnimation = new Animation(playerFallingAnimationMetaData, platformMethods);

		// Player's Leg animation
		initAnimationMetaData(playerWalkingLegAnimationMetaData, filename, .1f, 1, 12, {405, 44}, {32, 20} );
		playerWalkingLegAnimation  = new Animation(playerWalkingLegAnimationMetaData,platformMethods);

		initAnimationMetaData(playerIdlingLegAnimationMetaData, filename, .1f, 1, 1, {124, 13}, {21, 16});
		playerIdlingLegAnimation  = new Animation(playerIdlingLegAnimationMetaData,platformMethods);

		initAnimationMetaData(playerJumpingLegAnimationMetaData, filename, .1f, 1, 6, {174, 64}, {21, 25});
		playerJumpingLegAnimation  = new Animation(playerJumpingLegAnimationMetaData,platformMethods);

		initAnimationMetaData(playerFallingLegAnimationMetaData, filename, .1f, 1, 6, {279, 64}, {-21, 25});
		playerFallingLegAnimation = new Animation(playerFallingLegAnimationMetaData, platformMethods);

	}

	void setup() {
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
	float jumpHeight = .5777f;
	float playerHorizontalFacingDirection = 1;

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
		//backgroundRectText->setText(Util::MessageFormater::print("bgrect: ", backgroundRect.x, ", ", backgroundRect.y));
		platformMethods->renderImage(backgroundImg);
		//platformMethods->drawText(backgroundRectText);

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
		if (playerPhysicState == FALL) {
			player.y -= (float)(gravity*dt); 

			CollisionInfo colInfo;
			CollisionChecker::doesRectangleVsRectangleCollide(player, ground, colInfo);
			if (colInfo.count > 0) {
				playerPhysicState = ONGROUND;
				player.x -= colInfo.normal.x * colInfo.depths[0];
				player.y -= colInfo.normal.y * colInfo.depths[0];
			}
		}
		else if (playerPhysicState == ONGROUND) {
			if (!CollisionChecker::doesRectangleVsRectangleCollide(player, ground)) {
				playerPhysicState = FALL;
			}
			else if(CollisionChecker::doesRectangleVsRectangleCollide(player, ground) && 
					input.pressJump) {
				playerPhysicState = JUMPUP;
				jumpT = 0;
				playerOriginalGroundY = player.y; 
			}
		}
		else if (playerPhysicState == JUMPUP) {
			jumpT += gravity*dt;
			jumpProgress = -pow((jumpT-1), 2) + 1;
			player.y = playerOriginalGroundY + (jumpHeight)*jumpProgress; 

			// TODO: handle if sudden hit another object on the head -> physicState = FALL
			bool suddenHitPlatform = false;

			if (jumpT >= 1) {
				jumpT -= 1;
				playerPhysicState = JUMPDOWN;
			}
			else if (suddenHitPlatform) {
				playerPhysicState = FALL;
			}
		}
		else if (playerPhysicState == JUMPDOWN) {
			jumpT += gravity*dt;
			jumpProgress = -pow(jumpT, 2) + 1;
			player.y = playerOriginalGroundY + (jumpHeight)*jumpProgress; 

			CollisionInfo colInfo;
			CollisionChecker::doesRectangleVsRectangleCollide(player, ground, colInfo);
			if (colInfo.count > 0) {
				playerPhysicState = ONGROUND;

				player.x -= colInfo.normal.x * colInfo.depths[0];
				player.y -= colInfo.normal.y * colInfo.depths[0];
			}
		}

		// animation state machine (sometimes the animation state based on the physics state)
		switch (playerAnimationState) {
		case IDLING: {
			bool onGround = playerPhysicState == ONGROUND;
			if (input.pressLeft) {
				playerAnimationState = WALKING;
				playerHorizontalFacingDirection = -1;
				playerCurrentAnimation = playerWalkingAnimation;
				playerCurrentLegAnimation = playerWalkingLegAnimation;
			}
			else if (input.pressRight) {
				playerAnimationState = WALKING;
				playerHorizontalFacingDirection = 1;
				playerCurrentAnimation = playerWalkingAnimation;
				playerCurrentLegAnimation = playerWalkingLegAnimation;
			}

			if (input.pressJump) {
				playerAnimationState = JUMPING;
				playerCurrentAnimation = playerJumpingAnimation;
				playerCurrentLegAnimation = playerJumpingLegAnimation;
			}
			else if (!onGround) {
				playerAnimationState = PlayerAnimationState::FALLING;
				playerCurrentAnimation = playerFallingAnimation;
				playerCurrentLegAnimation = playerFallingLegAnimation;
			}
		}break;

		case WALKING: {

			bool isPressingMove = input.pressLeft || input.pressRight;
			if (!isPressingMove && 
				playerPhysicState != JUMPUP && 
				playerPhysicState != JUMPDOWN && 
				playerPhysicState != PlayerPhysicState::FALL) {

				playerAnimationState = IDLING;
				playerCurrentAnimation = playerIdlingAnimation;
				playerCurrentLegAnimation = playerIdlingLegAnimation;
			}
			else if (playerPhysicState == JUMPUP) {
				playerAnimationState = JUMPING;
				playerCurrentAnimation = playerJumpingAnimation;
				playerCurrentLegAnimation = playerJumpingLegAnimation;
			}
			else if (playerPhysicState == PlayerPhysicState::FALL) {
				playerAnimationState = PlayerAnimationState::FALLING;
				playerCurrentAnimation = playerFallingAnimation;
				playerCurrentLegAnimation = playerFallingLegAnimation;
			}
		} break;

		case JUMPING: {
			if (playerPhysicState == JUMPDOWN ||
				playerPhysicState == PlayerPhysicState::FALL) {

				playerAnimationState = PlayerAnimationState::FALLING;
				playerCurrentAnimation = playerFallingAnimation;
				playerCurrentLegAnimation = playerFallingLegAnimation;
			}

		} break;

		case PlayerAnimationState::FALLING: {
			if (playerPhysicState == ONGROUND) {
				playerAnimationState = IDLING;
				playerCurrentAnimation = playerIdlingAnimation;
				playerCurrentLegAnimation = playerIdlingLegAnimation;
			}
		} break;

		default: 
			// TODO: handle error "Animation state not recoginize"
			break;
		}


		if (CollisionChecker::doesRectangleVsRectangleCollide(player, ground)) {
			groundColor = collidedColor;
			playerColor = collidedColor;
		}
		else {
			groundColor = {255, 255, 0, 255};
			playerColor = {0, 0, 255, 255};
		}

		// @EndTest
		float legX = player.x;
		float legY = player.y - .15f;
		playerCurrentLegAnimation->changePos(legX, legY);
		playerCurrentLegAnimation->flip(playerHorizontalFacingDirection, 1);
		playerCurrentLegAnimation->animate(dt);

		playerCurrentAnimation->changePos(player.x, player.y);
		playerCurrentAnimation->flip(playerHorizontalFacingDirection, 1);
		playerCurrentAnimation->animate(dt);


		// Debug info
		platformMethods->drawRectangle(player, playerColor);
		platformMethods->drawRectangle(ground, groundColor);

		playerXY->setText(Util::MessageFormater::print("Player pos: ", player.x, ", ", player.y));
		platformMethods->drawText(playerXY);

		//groundPos->setText(Util::MessageFormater::print("Ground pos: ", ground.x, ", ", ground.y));
		//platformMethods->drawText(groundPos);

		std::string physicStateStr;
		if (playerPhysicState == FALL) {
			physicStateStr = "FALL";
		}
		else if (playerPhysicState == ONGROUND) {
			physicStateStr = "ONGROUND";
		}
		else if (playerPhysicState == JUMPUP) {
			physicStateStr = "JUMPUP";
		}
		else if (playerPhysicState == JUMPDOWN) {
			physicStateStr = "JUMPDOWN";
		}

		std::string animationStateStr;
		if (playerAnimationState == PlayerAnimationState::FALLING) {
			animationStateStr = "FALL";
		}
		else if (playerAnimationState == IDLING) {
			animationStateStr = "IDLING";
		}
		else if (playerAnimationState == JUMPING) {
			animationStateStr = "JUMP";
		}
		else if (playerAnimationState == WALKING) {
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