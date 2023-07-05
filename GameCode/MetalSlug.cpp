#pragma once

#include<iostream>
#include<vector>
#include "../Util.cpp"
#include "../SDL2PlatformMethodsCollection.cpp"
#include "Animation.h"
#include "CollisionChecker.h"

#include "Hostage.cpp"
#include "Player.cpp"
#include "RebelSoilder.h"

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
	GameText *playerPhysicStateText = NULL;
	GameText *playerAnimationStateText = NULL;

	std::vector<RectangleCollider*> groundColliders;
	std::vector<CameraControlledEntity*> entities;

	Color collidedColor = {255, 0, 0, 255};
	Color groundColor = {255, 255, 0, 255};
	Color playerColor = {0, 0, 255, 255};

	// NOTE: the only feature we have with the camera is move up, down, left, right 
	// no zoom, no scale -> fixed view port is 320 by 224

	// Camera 
	// NOTE: text not involve with the camera 
	Camera *camera;
	Player *player;

	// Level code
	float cameraMovePointX = -.42f;
	Rect preventGoingBackBlock = {-1.357f - .05f, 0, .1f, 2};
	std::vector<Hostage*> hostages;
	float endOfMapX = 0;
	bool cameraGoesOutsideOfMap = false; 

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

	float tempSpeed = 1;
	float gravity = 2.2f;

	LevelData levelData;

	std::vector<RebelSoilder*> rebelSoilders;
	//RebelSoilder *rebelSoilder;

	// TODO: I need a template class that takes  
	// Collider 
	// PhysicStateMachine 
	// AnimationStateMachine 


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

	void updateAndRender(GameInputContext &input, double dt) {
		//backgroundRectText->setText(Util::MessageFormater::print("bgrect: ", backgroundRect.x, ", ", backgroundRect.y));
		//platformMethods->drawText(backgroundRectText);

		// LEVEL STATE MACHINE
		switch (currentLevel1State) {
		case Level1State::OPENING: {
			background->animate(camera, dt);
			doLevelOpeningState(input, dt);
			break;
		}
		case Level1State::PLAYING: {
			doLevelPlayingState(input, dt);
			break;
		}
		case Level1State::END: {
			// TODO: 
			break;
		}
		}

		// CAMERA STATE MACHINE 
		switch (currentCameraState) {
		case Level1CameraState::OPENING:{
			doCameraOpeningState();
			break;
		}
		case Level1CameraState::AFTER_LANDING:{
			doCameraAfterLandingState();
			break;
		}
		case Level1CameraState::WATERFALL_STEP1: {
			doWaterFall1State();
			break;
		}
		case Level1CameraState::WATERFALL_STEP2: {
			doWaterFall2State();
			break;
		}
		case Level1CameraState::WATERFALL_STEP3: {
			doWaterFall3State();
			break;
		}
		case Level1CameraState::WATERFALL_STEP4: {
			doWaterFall4State();
			break;
		}
		case Level1CameraState::WATERFALL_STEP5: {
			doWaterFall5State();
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
	void initLevel1() {
		int backgroundPixelWidth = 4152;
		int backgroundPixelHeight = 304;

		cameraOpeningRect		= Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({ 0, 64, 304, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		cameraAfterLandingRect	= Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({ 0, 80, 304, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		cameraWaterFall1Rect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({ 3176, 80, 304, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		cameraWaterFall2Rect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({ 3251, 67, 304, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		cameraWaterFall3Rect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({ 3355, 41, 304, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		cameraWaterFall4Rect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({ 3457, 14, 304, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		cameraWaterFall5Rect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({ 3517, 0, 304, 224 }, backgroundPixelWidth, backgroundPixelHeight);
		endOfMapX = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({3848, 0, 304, 224}, backgroundPixelWidth, backgroundPixelHeight).x;

		//Vec2f cameraPosition = {-17.12425f, -0.357f}; // 17.12425 = bggamewidth/2 - half_world_size (1.43) 
		Vec2f cameraPosition = {cameraOpeningRect.x, cameraOpeningRect.y}; // 17.12425 = bggamewidth/2 - half_world_size (1.43) 
		camera = new Camera(cameraPosition);

		Util::AnimationUtil::initAnimationMetaData(backgroundMetaData, "Assets/Imgs/LevelsRawImage/metalslug_mission1_blank.png", 0, 1, 1, {0, 0}, {(float)backgroundPixelWidth, (float)backgroundPixelHeight});
		background = new Animation(backgroundMetaData, platformMethods);
		Rect backgroundRect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({0, 0, (float)backgroundPixelWidth, (float)backgroundPixelHeight}, backgroundPixelWidth, backgroundPixelHeight);
		background->setRect(backgroundRect);

		player = new Player(gravity, tempSpeed, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(waterFallAnimationMetaData, "Assets/Imgs/LevelsRawImage/level1_sprites.png", .1f, 1, 8, {0, 0}, {430, 272});
		waterFallAnimation = new Animation(waterFallAnimationMetaData, platformMethods);
		Rect waterFallRect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({3338, 0, 430, 272}, backgroundPixelWidth, backgroundPixelHeight);
		waterFallAnimation->setRect(waterFallRect);

		Util::AnimationUtil::initAnimationMetaData(waterFall2AnimationMetaData, "Assets/Imgs/LevelsRawImage/level1_sprites.png", .1f, 2, 4, {0, 346}, {832, 304});
		waterFall2Animation = new Animation(waterFall2AnimationMetaData, platformMethods);
		Rect waterFall2Rect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({3320, 0, 832, 304}, backgroundPixelWidth, backgroundPixelHeight);
		waterFall2Animation->setRect(waterFall2Rect);

		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({ 0, 252, 672, 52 }, backgroundPixelWidth, backgroundPixelHeight))
		);

		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({ 660, 282, 1156, 22 }, backgroundPixelWidth, backgroundPixelHeight))
		);

		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({1815, 254, 65, 50}, backgroundPixelWidth, backgroundPixelHeight))
		);

		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({851, 236, 71, 19}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({932, 196, 184, 21}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({932, 196, 184, 21}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({1166, 194, 149, 19}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({1350, 195, 174, 19}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({1518, 237, 79, 19}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({1886, 279, 1600, 25}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({3368, 229, 316, 36}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({3474, 205, 290, 34}, backgroundPixelWidth, backgroundPixelHeight))
		);
		groundColliders.push_back(
			new RectangleCollider(Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({3544, 144, 608, 55}, backgroundPixelWidth, backgroundPixelHeight))
		);

		Rect hostageColliderRect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({1009, 100, 18, 38}, backgroundPixelWidth, backgroundPixelHeight);
		hostageColliderRect.width = .2f;
		hostageColliderRect.height = .4f;
		hostages.push_back(new Hostage(gravity, tempSpeed, hostageColliderRect, platformMethods));

		hostageColliderRect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({1230, 100, 18, 38}, backgroundPixelWidth, backgroundPixelHeight);
		hostageColliderRect.width = .2f;
		hostageColliderRect.height = .4f;
		hostages.push_back(new Hostage(gravity, tempSpeed, hostageColliderRect, platformMethods));

		hostageColliderRect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({1423, 100, 18, 38}, backgroundPixelWidth, backgroundPixelHeight);
		hostageColliderRect.width = .2f;
		hostageColliderRect.height = .4f;
		hostages.push_back(new Hostage(gravity, tempSpeed, hostageColliderRect, platformMethods));

		// Rebels 
		Rect rebelColliderRect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({500, 100, 18, 38}, backgroundPixelWidth, backgroundPixelHeight);
		rebelColliderRect.width = .2f;
		rebelColliderRect.height = .4f;
		rebelSoilders.push_back(new RebelSoilder(gravity, tempSpeed, rebelColliderRect, platformMethods));

		rebelColliderRect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({630, 100 ,18, 38}, backgroundPixelWidth, backgroundPixelHeight);
		rebelColliderRect.width = .2f;
		rebelColliderRect.height = .4f;
		rebelSoilders.push_back(new RebelSoilder(gravity, tempSpeed, rebelColliderRect, platformMethods));

		rebelColliderRect = Util::LevelUtil::convertLevelColliderBlockPixelRectToGameRect({870, 100, 18, 38}, backgroundPixelWidth, backgroundPixelHeight);
		rebelColliderRect.width = .2f;
		rebelColliderRect.height = .4f;
		rebelSoilders.push_back(new RebelSoilder(gravity, tempSpeed, rebelColliderRect, platformMethods));

		for (RebelSoilder *rebelSoilder: rebelSoilders) {
			levelData.dangerRects.push_back(rebelSoilder->getInteractionRect());
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

		Rect r = levelData.playerColliderRect;
		Vec2f t = camera->convertWorldPosToScreenPos({r.x, r.y});
		r.x = t.x;
		r.y = t.y;
		platformMethods->drawRectangle(r, playerColor);

		r = levelData.playerInteractionRect;
		t = camera->convertWorldPosToScreenPos({r.x, r.y});
		r.x = t.x;
		r.y = t.y;
		platformMethods->drawRectangle(r, playerColor);

		for (RebelSoilder *rebelSoilder: rebelSoilders) {
			r = rebelSoilder->getInteractionRect();
			t = camera->convertWorldPosToScreenPos({r.x, r.y});
			r.x = t.x;
			r.y = t.y;
			platformMethods->drawRectangle(r, playerColor);
		}
		//OutputDebugStringA(Util::MessageFormater::print("Interaction rect: ", levelData.playerInteractionRect.x, ", ", levelData.playerInteractionRect.y, ", ", levelData.playerInteractionRect.width, ", ", levelData.playerInteractionRect.height, '\n').c_str());

		for (RectangleCollider *ground : groundColliders) {
			r = ground->getRect();
			Vec2f t = camera->convertWorldPosToScreenPos({r.x, r.y});
			r.x = t.x;
			r.y = t.y;
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

	void doLevelOpeningState(GameInputContext &input, double dt) {
		// action: the player will move slowly down, 
		GameInputContext i;
		levelData.groundColliders = groundColliders;

		player->moveYBy(-gravity*.2f*dt);
		player->update(i, levelData, camera, dt);

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
	}

	void doLevelPlayingState(GameInputContext &input, double dt) {
		Vec2f t = camera->convertWorldPosToScreenPos({ levelData.playerColliderRect.x, levelData.playerColliderRect.y });
		Rect r = levelData.playerColliderRect;
		r.x = t.x;
		r.y = t.y;

		if (r.x >= cameraMovePointX) {
			// TODO: Need to take a course on easing :))
			// Coding math link: https://www.youtube.com/watch?v=zLh0K1PdUbc&ab_channel=CodingMath
			float d = tempSpeed * dt;
			cameraGoesOutsideOfMap = camera->getPos().x + d >= endOfMapX;

			if(!cameraGoesOutsideOfMap) 
				camera->moveXBy(d);
		}

		CollisionInfo colInfo;
		CollisionChecker::doesRectangleVsRectangleCollide(r, preventGoingBackBlock, colInfo);
		if (colInfo.count > 0) {
			player->moveXBy(-colInfo.normal.x * colInfo.depths[0]);
			player->moveYBy(-colInfo.normal.y * colInfo.depths[0]);
		}

		levelData.groundColliders = groundColliders;

		background->animate(camera, dt);
		waterFallAnimation->animate(camera, dt);
		waterFall2Animation->animate(camera, dt);

		for (Hostage *hostage: hostages) {
			hostage->update(levelData, camera, dt);
		}

		for (int i = 0; i < rebelSoilders.size(); i++) {
			RebelSoilder* rebelSoilder = rebelSoilders[i];
			rebelSoilder->update(levelData, camera, dt);
			levelData.dangerRects[i] = rebelSoilder->getInteractionRect();
		}


		player->update(input, levelData, camera, dt);

		levelData.playerColliderRect = player->getRect();
		levelData.playerInteractionRect = player->getInteractionRect();

	}

	void doCameraOpeningState() {
		// action: maybe the camera moving should be here  
		//...
		// event: player touch the ground -> camera y position = ... , transition to after landing
		if (levelData.levelStarted) {
			currentCameraState = Level1CameraState::AFTER_LANDING;
			float d = cameraAfterLandingRect.y - cameraOpeningRect.y;
			camera->moveYBy(d);
			camera->moveYBy(0);
		}
	}

	void doCameraAfterLandingState() {
		// action: 
		// ... 

		// event: 
		if (camera->getPos().x >= cameraWaterFall1Rect.x) {
			currentCameraState = Level1CameraState::WATERFALL_STEP1;

			float d = cameraWaterFall1Rect.y - cameraAfterLandingRect.y;
			camera->moveYBy(d);
			camera->moveYBy(0);
		}

		// player.x hit waterfall_transition_1_x -> 
		// camera y position = ..., transition to water fall step 1 
	}

	void doWaterFall1State() {
		// action
		// event 
		if (camera->getPos().x >= cameraWaterFall2Rect.x) {
			currentCameraState = Level1CameraState::WATERFALL_STEP2;

			float d = cameraWaterFall2Rect.y - cameraWaterFall1Rect.y;
			camera->moveYBy(d);
			camera->moveYBy(0);
		}
	}

	void doWaterFall2State() {
		// action
		// event 
		if (camera->getPos().x >= cameraWaterFall3Rect.x) {
			currentCameraState = Level1CameraState::WATERFALL_STEP3;

			float d = cameraWaterFall3Rect.y - cameraWaterFall2Rect.y;
			camera->moveYBy(d);
			camera->moveYBy(0);
		}
	}

	void doWaterFall3State() {
		// action
		// event 
		if (camera->getPos().x >= cameraWaterFall4Rect.x) {
			currentCameraState = Level1CameraState::WATERFALL_STEP4;

			float d = cameraWaterFall4Rect.y - cameraWaterFall3Rect.y;
			camera->moveYBy(d);
			camera->moveYBy(0);
		}
	}

	void doWaterFall4State() {
		// action
		// event 
		if (camera->getPos().x >= cameraWaterFall5Rect.x) {
			currentCameraState = Level1CameraState::WATERFALL_STEP5;

			float d = cameraWaterFall5Rect.y - cameraWaterFall4Rect.y;
			camera->moveYBy(d);
			camera->moveYBy(0);
		}
	}

	void doWaterFall5State() {
			// action
			//...
			// event 
	}
};

}