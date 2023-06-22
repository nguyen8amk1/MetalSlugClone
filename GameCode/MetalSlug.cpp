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
	Animation *playerTopHalfAnim;
	Animation *playerBottomHalfAnim;
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

	Rect topHalfAnimRect;
	Rect bottomHalfAnimRect;
	float topHalfAnimOffsetX = 0;
	float topHalfAnimOffsetY = .1f;
	float bottomHalfAnimOffsetX = -.04f;
	float bottomHalfAnimOffsetY = -.05f;

	// Player Animations 
	AnimationMetaData idlingTopAnimationMetaData;
	AnimationMetaData jumpingTopAnimationMetaData;
	AnimationMetaData fallingTopAnimationMetaData;
	AnimationMetaData walkingTopAnimationMetaData;

	Animation *playerTopIdlingAnimation;
	Animation *playerTopJumpingAnimation;
	Animation *playerTopFallingAnimation;
	Animation *playerTopWalkingAnimation;

	AnimationMetaData idlingBottomAnimationMetaData;
	AnimationMetaData jumpingBottomAnimationMetaData;
	AnimationMetaData fallingBottomAnimationMetaData;
	AnimationMetaData walkingBottomAnimationMetaData;

	Animation *playerBottomIdlingAnimation;
	Animation *playerBottomJumpingAnimation;
	Animation *playerBottomFallingAnimation;
	Animation *playerBottomWalkingAnimation;


	void setup() {
		// NOTE: all of this will be loaded from a file, so no worries  
		topHalfAnimRect = { topHalfAnimOffsetX, topHalfAnimOffsetY, .237f, .3f};
		bottomHalfAnimRect = {bottomHalfAnimOffsetX, bottomHalfAnimOffsetY, .2f, .3f};

		// Top half  

		int idlingWidth = 35;
		int idlingHeight = 40;
		idlingTopAnimationMetaData;
		idlingTopAnimationMetaData.animDelay = .1f;
		idlingTopAnimationMetaData.animRect = topHalfAnimRect;
		idlingTopAnimationMetaData.spriteSheetFileName = "Assets/Imgs/Characters/Marco_Rossi_1.png";
		idlingTopAnimationMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(348, 317, idlingWidth, idlingHeight));
		playerTopIdlingAnimation = new Animation(idlingTopAnimationMetaData, platformMethods);

		int walkingWidth = 31;
		int walkingHeight = 27;
		walkingTopAnimationMetaData;
		walkingTopAnimationMetaData.animDelay = .1f;
		walkingTopAnimationMetaData.animRect = topHalfAnimRect;
		walkingTopAnimationMetaData.spriteSheetFileName = "Assets/Imgs/Characters/Marco_Rossi_1.png";
		walkingTopAnimationMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(343, 490,  walkingWidth, walkingHeight));
		playerTopWalkingAnimation = new Animation(walkingTopAnimationMetaData, platformMethods);

		// @Continue: Keep find the center and size of jumping and falling
		int jumpingWidth = 29;
		int jumpingHeight = 28;
		jumpingTopAnimationMetaData;
		jumpingTopAnimationMetaData.animDelay = .1f;
		jumpingTopAnimationMetaData.animRect = topHalfAnimRect;
		jumpingTopAnimationMetaData.spriteSheetFileName = "Assets/Imgs/Characters/Marco_Rossi_1.png";
		jumpingTopAnimationMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(257, 396, jumpingWidth, jumpingHeight));

		int fallingWidth = 26;
		int fallingHeight = 34;
		fallingTopAnimationMetaData;
		fallingTopAnimationMetaData.animDelay = .1f;
		fallingTopAnimationMetaData.animRect = topHalfAnimRect;
		fallingTopAnimationMetaData.spriteSheetFileName = "Assets/Imgs/Characters/Marco_Rossi_1.png";
		fallingTopAnimationMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(120, 360, fallingWidth, fallingHeight));

		playerTopFallingAnimation = new Animation(fallingTopAnimationMetaData, platformMethods);
		playerTopJumpingAnimation = new Animation(jumpingTopAnimationMetaData, platformMethods);

		// Bottom half 
		idlingWidth = 21;
		idlingHeight = 16;
		idlingBottomAnimationMetaData.animDelay = .1f;
		idlingBottomAnimationMetaData.animRect = bottomHalfAnimRect;
		idlingBottomAnimationMetaData.spriteSheetFileName = "Assets/Imgs/Characters/Marco_Rossi_1.png";
		idlingBottomAnimationMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(20, 463, idlingWidth, idlingHeight));
		playerBottomIdlingAnimation = new Animation(idlingBottomAnimationMetaData, platformMethods);

		walkingWidth = 31;
		walkingHeight = 20;
		walkingBottomAnimationMetaData;
		walkingBottomAnimationMetaData.animDelay = .1f;
		walkingBottomAnimationMetaData.animRect = bottomHalfAnimRect;
		walkingBottomAnimationMetaData.spriteSheetFileName = "Assets/Imgs/Characters/Marco_Rossi_1.png";
		walkingBottomAnimationMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(114, 546, walkingWidth, walkingHeight));
		playerBottomWalkingAnimation = new Animation(walkingBottomAnimationMetaData, platformMethods);

		jumpingWidth = 31;
		jumpingHeight = 20;
		jumpingBottomAnimationMetaData;
		jumpingBottomAnimationMetaData.animDelay = .1f;
		jumpingBottomAnimationMetaData.animRect = bottomHalfAnimRect;
		jumpingBottomAnimationMetaData.spriteSheetFileName = "Assets/Imgs/Characters/Marco_Rossi_1.png";
		jumpingBottomAnimationMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(242, 545, jumpingWidth, jumpingHeight));

		fallingWidth = 28;
		fallingHeight = 20;
		fallingBottomAnimationMetaData;
		fallingBottomAnimationMetaData.animDelay = .1f;
		fallingBottomAnimationMetaData.animRect = bottomHalfAnimRect;
		fallingBottomAnimationMetaData.spriteSheetFileName = "Assets/Imgs/Characters/Marco_Rossi_1.png";
		fallingBottomAnimationMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(49, 545, fallingWidth, fallingHeight));

		playerBottomFallingAnimation = new Animation(fallingBottomAnimationMetaData, platformMethods);
		playerBottomJumpingAnimation = new Animation(jumpingBottomAnimationMetaData, platformMethods);

		playerTopHalfAnim = playerTopIdlingAnimation; 
		playerBottomHalfAnim = playerBottomIdlingAnimation;

#if 0 
		AnimationMetaData topHalfAnimMetaData;
		AnimationMetaData bottomHalfAnimMetaData;
		/*
		topHalfAnimMetaData.animDelay = .1f;
		topHalfAnimMetaData.animRect = topHalfAnimRect;
		topHalfAnimMetaData.spriteSheetFileName = "Assets/Imgs/Characters/Marco_Rossi_1.png";
		*/

		/*
		bottomHalfAnimMetaData.animDelay = .1f;
		bottomHalfAnimMetaData.animRect = bottomHalfAnimRect;
		bottomHalfAnimMetaData.spriteSheetFileName = "Assets/Imgs/Characters/Marco_Rossi_1.png";
		*/

		// TODO: load all the sample frame for each state 
		// Animation init 
		int topHalfWidth = 35;
		int topHalfHeight = 40;
		topHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(24, 317, topHalfWidth, topHalfHeight));
		/*
		topHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(58, 317, topHalfWidth, topHalfHeight));
		topHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(92, 317, topHalfWidth, topHalfHeight));
		topHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(127, 317, topHalfWidth, topHalfHeight));
		topHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(164, 317, topHalfWidth, topHalfHeight));
		topHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(203, 317, topHalfWidth, topHalfHeight));
		topHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(243, 317, topHalfWidth, topHalfHeight));
		topHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(279, 317, topHalfWidth, topHalfHeight));
		topHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(314, 317, topHalfWidth, topHalfHeight));
		topHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(348, 317, topHalfWidth, topHalfHeight));
		*/
		playerTopHalfAnim = new Animation(topHalfAnimMetaData, platformMethods);

		int bottomHalfWidth = 21;
		int bottomHalfHeight = 16;
		bottomHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(20, 463, bottomHalfWidth, bottomHalfHeight));
		/*
		bottomHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(46, 463, bottomHalfWidth, bottomHalfHeight));
		bottomHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(72, 463, bottomHalfWidth, bottomHalfHeight));
		bottomHalfAnimMetaData.framePixelRects.push_back(Util::Generator::generatePixelRectFromCenter(98, 463, bottomHalfWidth, bottomHalfHeight));
		*/
		playerBottomHalfAnim = new Animation(bottomHalfAnimMetaData, platformMethods);
#endif 

		
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
				playerTopHalfAnim = playerTopWalkingAnimation;
				playerBottomHalfAnim = playerBottomWalkingAnimation;
			}
			else if (input.pressRight) {
				playerAnimationState = WALKING;
				playerTopHalfAnim = playerTopWalkingAnimation;
				playerBottomHalfAnim = playerBottomWalkingAnimation;
			}

			if (input.pressJump) {
				playerAnimationState = JUMPING;
				playerTopHalfAnim = playerTopJumpingAnimation;
				playerBottomHalfAnim = playerBottomJumpingAnimation;
			}
			else if (!onGround) {
				playerAnimationState = PlayerAnimationState::FALLING;
				playerTopHalfAnim = playerTopFallingAnimation;
				playerBottomHalfAnim = playerBottomFallingAnimation;
			}
		}break;

		case WALKING: {

			bool isPressingMove = input.pressLeft || input.pressRight;
			if (!isPressingMove && 
				playerPhysicState != JUMPUP && 
				playerPhysicState != JUMPDOWN && 
				playerPhysicState != PlayerPhysicState::FALL) {

				playerAnimationState = IDLING;
				playerTopHalfAnim = playerTopIdlingAnimation;
				playerBottomHalfAnim = playerBottomIdlingAnimation;
			}
			else if (playerPhysicState == JUMPUP) {
				playerAnimationState = JUMPING;
				playerTopHalfAnim = playerTopJumpingAnimation;
				playerBottomHalfAnim = playerBottomJumpingAnimation;
			}
			else if (playerPhysicState == PlayerPhysicState::FALL) {
				playerAnimationState = PlayerAnimationState::FALLING;
				playerTopHalfAnim = playerTopFallingAnimation;
				playerBottomHalfAnim = playerBottomFallingAnimation;
			}
		} break;

		case JUMPING: {
			if (playerPhysicState == JUMPDOWN ||
				playerPhysicState == PlayerPhysicState::FALL) {

				playerAnimationState = PlayerAnimationState::FALLING;
				playerTopHalfAnim = playerTopFallingAnimation;
				playerBottomHalfAnim = playerBottomFallingAnimation;
			}

		} break;

		case PlayerAnimationState::FALLING: {
			if (playerPhysicState == ONGROUND) {
				playerAnimationState = IDLING;
				playerTopHalfAnim = playerTopIdlingAnimation;
				playerBottomHalfAnim = playerBottomIdlingAnimation;
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

		// Debug collision


		// @EndTest
		platformMethods->drawRectangle(player, playerColor);
		platformMethods->drawRectangle(ground, groundColor);
		
		topHalfAnimRect.x = player.x + topHalfAnimOffsetX;
		topHalfAnimRect.y = player.y + topHalfAnimOffsetY;
		bottomHalfAnimRect.x = player.x + bottomHalfAnimOffsetX;
		bottomHalfAnimRect.y = player.y + bottomHalfAnimOffsetY;

		platformMethods->drawRectangle(topHalfAnimRect, groundColor);
		platformMethods->drawRectangle(bottomHalfAnimRect, groundColor);

		playerTopHalfAnim->changePos(topHalfAnimRect.x, topHalfAnimRect.y);
		playerBottomHalfAnim->changePos(bottomHalfAnimRect.x, bottomHalfAnimRect.y);

		playerTopHalfAnim->flip(playerHorizontalFacingDirection, 1);
		playerBottomHalfAnim->flip(playerHorizontalFacingDirection, 1); 

		playerTopHalfAnim->animate(dt);
		playerBottomHalfAnim->animate(dt);

		// Debug info
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
		delete playerTopHalfAnim;

		/*
		frameMillis->clean();
		fps->clean();
		*/

		delete frameMillis;
		delete fps;
	}
	
};

}