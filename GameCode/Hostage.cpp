#pragma once 

#include "MetalSlug.h"
#include "Animation.h"
#include "../Util.cpp"
#include "CollisionChecker.h"
#include "Physics.h"

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

	BasicPhysicStateMachine *basicPhysicStateMachine;
	BasicPhysicStateMachineResult basicPhysicResult;

public: 
	Hostage(float gravity, float moveSpeed, Rect hostageColliderRect, PlatformSpecficMethodsCollection *platformMethods) {
		this->gravity = gravity;
		this->moveSpeed = moveSpeed;
		this->platformMethods = platformMethods;
		this->hostageColliderRect = hostageColliderRect;

		basicPhysicStateMachine = new BasicPhysicStateMachine(gravity);

		std::string filename = "Assets/Imgs/Characters/hostage.png";
		Util::AnimationUtil::initAnimationMetaData(hostageTiedAnimationMetaData, filename, .1f, 1, 1, {6, 26}, {30, 27});
		hostageTiedAnimation = new Animation(hostageTiedAnimationMetaData, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(hostageUntiedAnimationMetaData, filename, .1f, 1, 1, {6, 248}, {33, 38});
		hostageUntiedAnimation  = new Animation(hostageUntiedAnimationMetaData, platformMethods);

		hostageCurrentAnimation = hostageTiedAnimation;
	}

	void update(LevelData &levelData, Camera *camera, double dt) {
		basicPhysicResult = basicPhysicStateMachine->update(dt, hostageColliderRect, levelData);
		hostageColliderRect = basicPhysicResult.colliderRect;

		// Animation state machine 
		switch (hostageCurrentAnimationState) {
		case HostageAnimationState::TIED: {
			// TODO: 
			bool playerUntied = CollisionChecker::doesRectangleVsRectangleCollide(hostageColliderRect, levelData.playerInteractionRect);
			if (playerUntied) {
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
		hostageCurrentAnimation->animate(camera, dt);
		Color testCol = {255, 255, 0};
		
		Rect r = hostageColliderRect;
		Vec2f t = camera->convertWorldPosToScreenPos({r.x, r.y});
		r.x = t.x;
		r.y = t.y;
		platformMethods->drawRectangle(r, testCol);
	}

	void moveXBy(float d) {
		hostageColliderRect.x += d;
	}

	void moveYBy(float d) {
		hostageColliderRect.y += d;
	}
};

}
