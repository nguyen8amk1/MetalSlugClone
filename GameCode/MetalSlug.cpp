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

	AnimationMetaData backgroundMetaData;
	Animation *background;

	// debug 
	GameText *frameMillis = NULL;
	GameText *fps = NULL;
	GameText *playerXY = NULL;
	//GameText *groundPos = NULL;
	GameText *playerPhysicStateText = NULL;
	GameText *playerAnimationStateText = NULL;
	//GameText *backgroundRectText = NULL;

	std::vector<RectangleCollider*> groundColliders;
	std::vector<CameraControlledEntity*> entities;

	Color collidedColor = {255, 0, 0, 255};
	Color groundColor = {255, 255, 0, 255};
	Color playerColor = {0, 0, 255, 255};

	// NOTE: the only feature we have with the camera is move up, down, left, right 
	// no zoom, no scale -> fixed view port is 320 by 224
	Camera *camera;
	Player *player;

	// Level code
	float cameraMovePointX = -.42f;
	Rect preventGoingBackBlock = {-1.43f - .05f, 0, .1f, 2};
	std::vector<Hostage*> hostages;

	// level1 
	AnimationMetaData waterFallAnimationMetaData;
	Animation *waterFallAnimation;
	AnimationMetaData waterFall2AnimationMetaData;
	Animation *waterFall2Animation;
	

	Rect cameraOpeningRect, cameraAfterLandingRect,
		cameraWaterFall1Rect, cameraWaterFall2Rect, cameraWaterFall3Rect,
		cameraWaterFall4Rect, cameraWaterFall5Rect;

	enum class Level1CameraState {
		OPENING, 
		AFTER_LANDING, 
		WATERFALL_STEP1, 
		WATERFALL_STEP2, 
		WATERFALL_STEP3, 
		WATERFALL_STEP4, 
		WATERFALL_STEP5
	};

	// What will the level state use to control  
	// -> THE PLAYER PHYSIC, ANIMATION STATE MACHINE ONLY STARTED WHEN IN THE RUNNING LEVEL STATE 
	// NOTE: THE LEVEL STATE MACHINE ONLY CONTROL PLAYER, CAMERA BEHAVIOR THE OTHER (BACKGROUND, COLLIDER, ...) NOT INVOLVE
	enum class Level1State {
		OPENING, 
		PLAYING, 
		END
	};

	Level1CameraState currentCameraState = Level1CameraState::OPENING;
	Level1State currentLevel1State = Level1State::OPENING;

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
		initLevel1();

		frameMillis = platformMethods->createText(0, 0, 10);
		fps  = platformMethods->createText(0, 15, 10);
		playerXY  = platformMethods->createText(0, 30, 10);

		playerPhysicStateText  = platformMethods->createText(0, 45, 10);
		playerAnimationStateText  = platformMethods->createText(0, 60, 10);

	}

	float tempSpeed = 1;
	float gravity = 2.2f;

	// Camera 
	// NOTE: All the pos of entity in the game is in world space not camera space, 
	// the camera calculation is just for rendering   
	// NOTE: text not involve with the camera 

	// Everything right here is in Level running state:  
	LevelData levelData;
	void updateAndRender(GameInputContext &input, double dt) {
		//backgroundRectText->setText(Util::MessageFormater::print("bgrect: ", backgroundRect.x, ", ", backgroundRect.y));
		//platformMethods->drawText(backgroundRectText);

		background->animate(dt);
		waterFallAnimation->animate(dt);
		waterFall2Animation->animate(dt);

		// LEVEL STATE MACHINE
		switch (currentLevel1State) {
		case Level1State::OPENING: {
			// action: the player will move slowly down, 
			GameInputContext i;
			levelData.groundColliders = groundColliders;

			player->moveYBy(-gravity*.2f*dt);
			player->update(i, levelData, dt);

			levelData.playerColliderRect = player->getRect();

			// event: once touch the ground will: 
				// switch the level state to playing mode 
			CollisionInfo colInfo;
			CollisionChecker::doesRectangleVsRectangleCollide(levelData.playerColliderRect, groundColliders[0]->getRect(), colInfo);
			if (colInfo.count > 0) {
				player->moveXBy(-colInfo.normal.x * colInfo.depths[0]);
				player->moveYBy(-colInfo.normal.y * colInfo.depths[0]);
				currentLevel1State = Level1State::PLAYING;
				levelData.levelStarted = true;
			}

			break;
		}
		case Level1State::PLAYING: {
			if (levelData.playerColliderRect.x >= cameraMovePointX) {
				float d = tempSpeed * dt;
				//cameraPos.x += d;

				camera->moveXBy(d);

				for (CameraControlledEntity *entity: entities) {
					camera->update(entity);
				}
			}

			CollisionInfo colInfo;
			CollisionChecker::doesRectangleVsRectangleCollide(levelData.playerColliderRect, preventGoingBackBlock, colInfo);
			if (colInfo.count > 0) {
				player->moveXBy(-colInfo.normal.x * colInfo.depths[0]);
				player->moveYBy(-colInfo.normal.y * colInfo.depths[0]);
			}

			levelData.groundColliders = groundColliders;

			player->update(input, levelData, dt);

			levelData.playerColliderRect = player->getRect();


			for (Hostage *hostage: hostages) {
				hostage->update(input, levelData, dt);
			}

			break;
		}
		case Level1State::END: {

			break;
		}
		}

		// TODO: CAMERA STATE MACHINE 
		switch (currentCameraState) {
		case Level1CameraState::OPENING:{
			// action: maybe the camera moving should be here  
			//...

			// event: player touch the ground -> camera y position = ... , transition to after landing
			if (levelData.levelStarted) {
				currentCameraState = Level1CameraState::AFTER_LANDING;
				float d = cameraAfterLandingRect.y - cameraOpeningRect.y;
				camera->moveYBy(d);

				for (CameraControlledEntity *entity: entities) {
					camera->update(entity);
				}

				camera->moveYBy(0);
			}
			break;
		}
		case Level1CameraState::AFTER_LANDING:{
			// action: 
			// ... 
			// event: 
			if (camera->getPos().x >= cameraWaterFall1Rect.x) {
				currentCameraState = Level1CameraState::WATERFALL_STEP1;

				float d = cameraWaterFall1Rect.y - cameraAfterLandingRect.y;
				camera->moveYBy(d);
				for (CameraControlledEntity *entity: entities) {
					camera->update(entity);
				}
				camera->moveYBy(0);
			}

			// player.x hit waterfall_transition_1_x -> 
			// camera y position = ..., transition to water fall step 1 
			break;
		}
		case Level1CameraState::WATERFALL_STEP1: {
			// action
			// event 
			if (camera->getPos().x >= cameraWaterFall2Rect.x) {
				currentCameraState = Level1CameraState::WATERFALL_STEP2;

				float d = cameraWaterFall2Rect.y - cameraWaterFall1Rect.y;
				camera->moveYBy(d);
				for (CameraControlledEntity *entity: entities) {
					camera->update(entity);
				}
				camera->moveYBy(0);
			}
			break;
		}
		case Level1CameraState::WATERFALL_STEP2: {
			// action
			// event 
			if (camera->getPos().x >= cameraWaterFall3Rect.x) {
				currentCameraState = Level1CameraState::WATERFALL_STEP3;

				float d = cameraWaterFall3Rect.y - cameraWaterFall2Rect.y;
				camera->moveYBy(d);
				for (CameraControlledEntity *entity: entities) {
					camera->update(entity);
				}
				camera->moveYBy(0);
			}
			break;
		}
		case Level1CameraState::WATERFALL_STEP3: {
			// action
			// event 
			if (camera->getPos().x >= cameraWaterFall4Rect.x) {
				currentCameraState = Level1CameraState::WATERFALL_STEP4;

				float d = cameraWaterFall4Rect.y - cameraWaterFall3Rect.y;
				camera->moveYBy(d);
				for (CameraControlledEntity *entity: entities) {
					camera->update(entity);
				}
				camera->moveYBy(0);
			}
			break;
			break;
		}
		case Level1CameraState::WATERFALL_STEP4: {
			// action
			// event 
			if (camera->getPos().x >= cameraWaterFall5Rect.x) {
				currentCameraState = Level1CameraState::WATERFALL_STEP5;

				float d = cameraWaterFall5Rect.y - cameraWaterFall4Rect.y;
				camera->moveYBy(d);
				for (CameraControlledEntity *entity: entities) {
					camera->update(entity);
				}
				camera->moveYBy(0);
			}
			break;
		}
		case Level1CameraState::WATERFALL_STEP5: {
			// action
			// event 
			break;
		}
			
		}

		displayDebug();
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

private: 
	Rect convertLevelColliderBlockPixelRectToGameRect(const Rect& pixelRect, int backgroundPixelWidth, int backgroundPixelHeight) {
		float tx = backgroundPixelWidth/320.0f; 
		float ty = backgroundPixelHeight/224.0f; 

		float w = pixelRect.width/111.888f;
		float h = pixelRect.height/112.0f;

		float x = (pixelRect.x/111.888f) - 1.43*tx + w/2.0f;
		float y = ty - (2*ty*pixelRect.y/304.0f) - h/2.0f;

		return { x, y, w, h };
	}

	void initLevel1() {

		//opening position: 0, 64, 320, 224
		int backgroundPixelWidth = 4152;
		int backgroundPixelHeight = 304;

		/*
			opening position: 0, 64, 320, 224
			after landing position: 0, 80, 320, 224
			waterfall step 1: 3235, 67, 320, 224
			waterfall step 2: 3339, 41, 320, 224
			waterfall step 3: 3441, 14, 320, 224
			waterfall step 4: 3501, 0, 320, 224
			waterfall step 5: 3501, 0, 320, 224
		*/
		cameraOpeningRect = convertLevelColliderBlockPixelRectToGameRect({ 0, 64, 320, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		cameraAfterLandingRect = convertLevelColliderBlockPixelRectToGameRect({ 0, 80, 320, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		cameraWaterFall1Rect = convertLevelColliderBlockPixelRectToGameRect({ 3235, 67, 320, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		cameraWaterFall2Rect = convertLevelColliderBlockPixelRectToGameRect({ 3339, 41, 320, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		cameraWaterFall3Rect = convertLevelColliderBlockPixelRectToGameRect({ 3441, 14, 320, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		cameraWaterFall4Rect = convertLevelColliderBlockPixelRectToGameRect({ 3051, 0, 320, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		//cameraWaterFall5 = convertLevelColliderBlockPixelRectToGameRect({ }, backgroundPixelWidth, backgroundPixelHeight);

		//Vec2f cameraPosition = {-17.12425f, -0.357f}; // 17.12425 = bggamewidth/2 - half_world_size (1.43) 
		Vec2f cameraPosition = {cameraOpeningRect.x, cameraOpeningRect.y}; // 17.12425 = bggamewidth/2 - half_world_size (1.43) 
		camera = new Camera(cameraPosition);

		Util::AnimationUtil::initAnimationMetaData(backgroundMetaData, "Assets/Imgs/LevelsRawImage/metalslug_mission1_blank.png", 0, 1, 1, {0, 0}, {(float)backgroundPixelWidth, (float)backgroundPixelHeight});
		background = new Animation(backgroundMetaData, platformMethods);
		background->changeSize(37.108f, 1.357f*2.0f);

		player = new Player(gravity, tempSpeed, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(waterFallAnimationMetaData, "Assets/Imgs/LevelsRawImage/level1_sprites.png", .1f, 1, 8, {0, 0}, {430, 272});
		waterFallAnimation = new Animation(waterFallAnimationMetaData, platformMethods);
		Rect waterFallRect = convertLevelColliderBlockPixelRectToGameRect({3338, 0, 430, 272}, backgroundPixelWidth, backgroundPixelHeight);
		waterFallAnimation->setRect(waterFallRect);

		Util::AnimationUtil::initAnimationMetaData(waterFall2AnimationMetaData, "Assets/Imgs/LevelsRawImage/level1_sprites.png", .1f, 2, 4, {0, 346}, {832, 304});
		waterFall2Animation = new Animation(waterFall2AnimationMetaData, platformMethods);
		Rect waterFall2Rect = convertLevelColliderBlockPixelRectToGameRect({3320, 0, 832, 304}, backgroundPixelWidth, backgroundPixelHeight);
		waterFall2Animation->setRect(waterFall2Rect);

		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({ 0, 252, 672, 52 }, backgroundPixelWidth, backgroundPixelHeight))
		);

		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({ 660, 282, 1156, 22 }, backgroundPixelWidth, backgroundPixelHeight))
		);

		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({1815, 254, 65, 50}, backgroundPixelWidth, backgroundPixelHeight))
		);

		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({851, 236, 71, 19}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({932, 196, 184, 21}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({932, 196, 184, 21}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({1166, 194, 149, 19}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({1350, 195, 174, 19}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({1518, 237, 79, 19}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({1886, 279, 1600, 25}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({3368, 229, 316, 36}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({3474, 205, 290, 34}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(convertLevelColliderBlockPixelRectToGameRect({3544, 144, 608, 55}, backgroundPixelWidth, backgroundPixelHeight))
		);

		Rect hostageColliderRect = convertLevelColliderBlockPixelRectToGameRect({1009, 100, 18, 38}, backgroundPixelWidth, backgroundPixelHeight);
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

		for (RectangleCollider *collider: groundColliders) {
			entities.push_back(collider);
		}
		
		for (Hostage *hostage: hostages) {
			entities.push_back(hostage);
		}

		entities.push_back(background);
		entities.push_back(player);
		entities.push_back(waterFallAnimation);
		entities.push_back(waterFall2Animation);

		/*
		backgroundRectText  = platformMethods->createText(0, 60, 10);
		groundPos  = platformMethods->createText(0, 75, 10);
		*/

		// apply the camera 
		for (CameraControlledEntity *entity: entities) {
			camera->apply(entity);
		}
	}

	void displayDebug() {
		// Debug info
		if (platformDebugInfo) {
			frameMillis->setText(Util::MessageFormater::print("Frametime Millis: ", platformDebugInfo->frameTimeMillis));
			fps->setText(Util::MessageFormater::print("FPS: ", platformDebugInfo->fps));

			platformMethods->drawText(frameMillis);
			platformMethods->drawText(fps);
		}
		platformMethods->drawRectangle(levelData.playerColliderRect, playerColor);

		for (RectangleCollider *ground : groundColliders) {
			Rect r = ground->getRect();
			platformMethods->drawRectangle(r, groundColor);
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
};

}