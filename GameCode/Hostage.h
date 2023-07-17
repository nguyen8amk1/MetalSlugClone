#pragma once 

#include "MetalSlug.h"
#include "Animation.h"
#include "../Util.h"
#include "CollisionChecker.h"
#include "Physics.h"
#include "Camera.h"
#include "GlobalGameData.h"

namespace MetalSlug {
class GlobalGameData;

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

	GlobalGameData *globalGameData;

public: 
	Hostage(float gravity, float moveSpeed, Rect hostageColliderRect, PlatformSpecficMethodsCollection* platformMethods);
	void update(LevelData& levelData, Camera* camera, double dt);

	void moveXBy(float d);
	void moveYBy(float d);
};

}
