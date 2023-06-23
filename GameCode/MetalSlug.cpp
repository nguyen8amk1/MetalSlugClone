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

	Rect player = {-17, .3f, .2f, .2f};
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

	Animation *playerCurrentAnimation;

	Rect animRect;
	void setup() {
		// TODO: init all the animation here  
		// first need to have the editied sprite sheet first  @Current 
		playerIdlingAnimationMetaData.animDelay = .1f;
		playerIdlingAnimationMetaData.tiledSheetFileName = "C:/Users/cutch/Downloads/marco_messi_small.png";
		playerIdlingAnimationMetaData.rows = 1;
		playerIdlingAnimationMetaData.columns = 4;
		playerIdlingAnimationMetaData.framePixelSize = {31,29};

		float h = (playerIdlingAnimationMetaData.framePixelSize.y/224.0f)*2;
		float w = h*(31.0f/29.0f); 

		animRect = {0, 0, w, h};
		playerIdlingAnimationMetaData.rect = animRect;
		playerIdlingAnimationMetaData.relativeCorner = {0,0};
		playerIdlingAnimation = new Animation(playerIdlingAnimationMetaData,platformMethods);

		/*
		playerWalkingAnimationMetaData.animDelay = .1f;
		playerWalkingAnimationMetaData.tiledSheetFileName = "";
		playerWalkingAnimationMetaData.rows = 1;
		playerWalkingAnimationMetaData.columns = 4;
		playerWalkingAnimationMetaData.rect = animRect;
		playerWalkingAnimationMetaData.relativeCorner = {,};
		playerWalkingAnimationMetaData.framePixelSize = {,};

		playerJumpingAnimationMetaData.animDelay = .1f;
		playerJumpingAnimationMetaData.tiledSheetFileName = "";
		playerJumpingAnimationMetaData.rows = 1;
		playerJumpingAnimationMetaData.columns = 4;
		playerJumpingAnimationMetaData.rect = animRect;
		playerJumpingAnimationMetaData.relativeCorner = {,};
		playerJumpingAnimationMetaData.framePixelSize = {,};

		playerFallingAnimationMetaData.animDelay = .1f;
		playerFallingAnimationMetaData.tiledSheetFileName = "";
		playerFallingAnimationMetaData.rows = 1;
		playerFallingAnimationMetaData.columns = 4;
		playerFallingAnimationMetaData.rect = animRect;
		playerFallingAnimationMetaData.relativeCorner = {,};
		playerFallingAnimationMetaData.framePixelSize = {,};
		*/

		/*
		playerWalkingAnimation = new Animation(playerWalkingAnimationMetaData, platformMethods);
		playerJumpingAnimation = new Animation(playerJumpingAnimationMetaData,platformMethods);
		playerFallingAnimation = new Animation(playerFallingAnimationMetaData, platformMethods);
		*/

		playerCurrentAnimation = playerIdlingAnimation;


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
	float jumpHeight = .8f;
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
			playerHorizontalFacingDirection = -1;
		}
		else if (input.pressRight) {
			player.x += (float)(tempSpeed*dt); 
			playerHorizontalFacingDirection = 1;
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
				//playerCurrentAnimation = playerWalkingAnimation;
			}
			else if (input.pressRight) {
				playerAnimationState = WALKING;
				//playerCurrentAnimation = playerWalkingAnimation;
			}

			if (input.pressJump) {
				playerAnimationState = JUMPING;
				//playerCurrentAnimation = playerJumpingAnimation;
			}
			else if (!onGround) {
				playerAnimationState = PlayerAnimationState::FALLING;
				//playerCurrentAnimation = playerFallingAnimation;
			}
		}break;

		case WALKING: {

			bool isPressingMove = input.pressLeft || input.pressRight;
			if (!isPressingMove && 
				playerPhysicState != JUMPUP && 
				playerPhysicState != JUMPDOWN && 
				playerPhysicState != PlayerPhysicState::FALL) {

				playerAnimationState = IDLING;
				//playerCurrentAnimation = playerIdlingAnimation;
			}
			else if (playerPhysicState == JUMPUP) {
				playerAnimationState = JUMPING;
				//playerCurrentAnimation = playerJumpingAnimation;
			}
			else if (playerPhysicState == PlayerPhysicState::FALL) {
				playerAnimationState = PlayerAnimationState::FALLING;
				//playerCurrentAnimation = playerFallingAnimation;
			}
		} break;

		case JUMPING: {
			if (playerPhysicState == JUMPDOWN ||
				playerPhysicState == PlayerPhysicState::FALL) {

				playerAnimationState = PlayerAnimationState::FALLING;
				//playerCurrentAnimation = playerFallingAnimation;
			}

		} break;

		case PlayerAnimationState::FALLING: {
			if (playerPhysicState == ONGROUND) {
				playerAnimationState = IDLING;
				//playerCurrentAnimation = playerIdlingAnimation;
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

		playerCurrentAnimation->changePos(player.x, player.y);
		//playerBottomHalfAnim->changePos(bottomHalfAnimRect.x, bottomHalfAnimRect.y);

		playerCurrentAnimation->flip(playerHorizontalFacingDirection, 1);

		playerCurrentAnimation->animate(dt);

		// Debug info
		platformMethods->drawRectangle(player, playerColor);
		platformMethods->drawRectangle(ground, groundColor);
		
		animRect.x = player.x;
		animRect.y = player.y;
		platformMethods->drawRectangle(animRect, groundColor);
		//platformMethods->drawRectangle(bottomHalfAnimRect, groundColor);

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