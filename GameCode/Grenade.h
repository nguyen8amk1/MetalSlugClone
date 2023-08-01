#pragma once 
#include "Animation.h"
#include "MetalSlug.h"
#include "../Util.h"
#include "TimeBoundedLerp.h"
#include "Camera.h"
#include "GlobalGameData.h"
#include "GrenadeFactory.h"
//#include "Windows.h"

namespace MetalSlug {
struct GrenadeAnimationContext;
class GrenadeAnimationContextFactory;
class GrenadeFactory;
class GlobalGameData;

class Grenade {
private: 
	Rect colliderRect = { 0, 0, .1f, .1f };

	enum class PhysicState {
		NONE, 
		FIRST_HOP_UP, 
		FIRST_HOP_DOWN, 
		SECOND_HOP_UP, 
		SECOND_HOP_DOWN, 
		ON_GROUND
	};

	enum class AnimationState {
		NONE,
		FIRST_HOP, 
		SECOND_HOP, 
		EXPLODE,
	};

	PhysicState currentPhysicState = PhysicState::NONE;
	AnimationState currentAnimationState = AnimationState::NONE;

	AnimationMetaData spinningAnimationMetaData;
	Animation* spinningAnimation;

	Animation* currentAnimation;

	int direction = 1;  

	float firstHopHeight = .251851f;
	float firstHopDuration = 0.6f;
	float howFarFirstHop = 0.4806f;

	float secondHopHeight = .15f;
	float secondHopDuration = 0.6f;
	float howFarSecondHop = 0.31097f;

	TimeBoundedLerp *xLerp;
	TimeBoundedLerp *yLerp;


	GlobalGameData* globalGameData;
	Point originalPos;
	PlatformSpecficMethodsCollection* platformMethods;

public:
	Grenade(PlatformSpecficMethodsCollection* platformMethods, GrenadeAnimationContext *grenadeAnimationContext);

	void startThrow(int facingDirection, float playerX, float playerY);

	void update(Camera* camera, double dt);
};

}
