#pragma once 

#include "MetalSlug.h"
#include "Animation.cpp"
#include "../Util.cpp"
#include "CollisionChecker.h"

namespace MetalSlug {
class Hostage {
private: 
	Rect hostageColliderRect;

	enum class HostagePhysicState {
		ONGROUND,
		FALL 
	};

	enum class HostageAnimationState {
		TIED, 
		UNTIED 
	};

	HostagePhysicState hostageCurrentPhysicState = HostagePhysicState::ONGROUND;
	HostageAnimationState hostageCurrentAnimationState = HostageAnimationState::TIED;

	// Hostage animation 
	AnimationMetaData hostageTiedAnimationMetaData; 
	Animation *hostageTiedAnimation;

	AnimationMetaData hostageUntiedAnimationMetaData; 
	Animation *hostageUntiedAnimation;
	Animation *hostageCurrentAnimation;

	PlatformSpecficMethodsCollection* platformMethods;

	float gravity;
	float moveSpeed;

public: 
	Hostage(float gravity, float moveSpeed, Rect hostageColliderRect, PlatformSpecficMethodsCollection *platformMethods) {
		this->gravity = gravity;
		this->moveSpeed = moveSpeed;
		this->platformMethods = platformMethods;
		this->hostageColliderRect = hostageColliderRect;

		std::string filename = "Assets/Imgs/Characters/hostage.png";
		Util::AnimationUtil::initAnimationMetaData(hostageTiedAnimationMetaData, filename, .1f, 1, 1, {6, 26}, {30, 27});
		hostageTiedAnimation = new Animation(hostageTiedAnimationMetaData, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(hostageUntiedAnimationMetaData, filename, .1f, 1, 1, {6, 248}, {33, 38});
		hostageUntiedAnimation  = new Animation(hostageUntiedAnimationMetaData, platformMethods);

		hostageCurrentAnimation = hostageTiedAnimation;
	}

	void update(const GameInputContext& input, LevelData &levelData, double dt) {
		// Physic state machine 
		switch (hostageCurrentPhysicState) {
		case HostagePhysicState::ONGROUND: {
			for (Rect &groundCollider: levelData.groundColliders) {
				if (!CollisionChecker::doesRectangleVsRectangleCollide(hostageColliderRect, groundCollider)) {
					hostageCurrentPhysicState = HostagePhysicState::FALL;
					break;
				}
			}
			break;
		}
		case HostagePhysicState::FALL: {
			hostageColliderRect.y -= (float)(gravity*dt); 

			for (Rect& groundCollider : levelData.groundColliders) {
				CollisionInfo colInfo;
				CollisionChecker::doesRectangleVsRectangleCollide(hostageColliderRect, groundCollider, colInfo);
				if (colInfo.count > 0) {
					hostageCurrentPhysicState = HostagePhysicState::ONGROUND;
					hostageColliderRect.x -= colInfo.normal.x * colInfo.depths[0];
					hostageColliderRect.y -= colInfo.normal.y * colInfo.depths[0];
					break;
				}
			}
			break;
		}
		}

		// Animation state machine 
		switch (hostageCurrentAnimationState) {
		case HostageAnimationState::TIED: {
			// TODO: 
			bool touchPlayer = CollisionChecker::doesRectangleVsRectangleCollide(hostageColliderRect, levelData.playerColliderRect);
			if (touchPlayer) {
				hostageCurrentAnimationState = HostageAnimationState::UNTIED;
				hostageCurrentAnimation = hostageUntiedAnimation;
			}
			break;
		}
		case HostageAnimationState::UNTIED: {
			// TODO: 
			hostageColliderRect.x -= moveSpeed*.5f*dt;

			bool outOfScreen = hostageColliderRect.x < -1.45f;
			if (outOfScreen) {
				// TODO: handle hostage out of screen 
			}
			break;
		}
		}

		// Rendering 
		hostageCurrentAnimation->changePos(hostageColliderRect.x, hostageColliderRect.y);
		hostageCurrentAnimation->animate(dt);
		Color testCol = {255, 255, 0};
		platformMethods->drawRectangle(hostageColliderRect, testCol);
	}

	/*
	void changePos(float x, float y) {
		hostageColliderRect.x = x;
		hostageColliderRect.y = y;
	}

	Vec2f getCurrentPos() {
		return { hostageColliderRect.x, hostageColliderRect.y };
	}
	*/

	void moveXBy(float d) {
		hostageColliderRect.x += d;
	}

	void moveYBy(float d) {
		hostageColliderRect.y += d;
	}
};

}
