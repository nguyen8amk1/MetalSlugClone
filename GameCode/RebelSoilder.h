#pragma once 
#include "MetalSlug.h"
#include "Animation.h"
#include "CollisionChecker.h"
#include "Physics.h"
#include "GlobalGameData.h"
#include <Windows.h>

namespace MetalSlug {
class GrenadeFactory;
class GlobalGameData;

class RebelSoilder {
public:
	// Animation state machine 
	enum class AnimationState {
		IDLING,			// 0, 4, 108, 38
		SLASHING,		// 0, 42, 735, 37
		THROWING_BOMB,	// 0, 79, 782, 42 
		SLASHED_DIE,
		NONE
	};

private:
	// Collider 
	Rect colliderRect;
	Rect interactionRectDisabledRect = { 0, -5.0f, 0, 0 };
	Rect interactionRect;
	int horizontalFacingDirection = -1;


	// Animation
	AnimationMetaData idleAnimationMetaData;
	Animation* idleAnimation;

	AnimationMetaData slashingAnimationMetaData;
	Animation* slashingAnimation;

	AnimationMetaData throwingBombAnimationMetaData;
	Animation* throwingBombAnimation;

	AnimationMetaData slashedDieAnimationMetaData;
	Animation* slashedDieAnimation;


	AnimationState currentAnimationState;
	Animation* currentAnimation;

	PlatformSpecficMethodsCollection* platformMethods;
	float gravity;
	float moveSpeed;
	BasicPhysicStateMachine* physicStateMachine;

	double timeAccumulator = 0.0;

	BasicPhysicStateMachineResult physicResult = { colliderRect };

	bool touchPlayer = false;
	bool playerInThrowingRange = false;
	bool hitByBullet = false;

	GlobalGameData* globalGameData;
	GrenadeFactory* grenadeFactory;

public:
	RebelSoilder(float gravity, float moveSpeed, Rect colliderRect, PlatformSpecficMethodsCollection* platformMethods, AnimationState animationState = AnimationState::IDLING);
	void update(Camera* camera, double dt);

	void moveXBy(float d) {
		colliderRect.x += d;
	}

	void moveYBy(float d) {
		colliderRect.y += d;
	}

	Rect getRect() { return colliderRect; }
	Rect getInteractionRect() { return interactionRect; }

	void bulletHit() {
		hitByBullet = true;
	}

private:
	void toThrowingBombAnimation();

	void toSlashingAnimation();

	void toDieAnimation();

	void throwGrenade(); 

	// event transitions
	void bulletDieEventTransition();
	void slashedDieEventTransition();
	void slashingEventTransition();
	void throwingBombEventTransition();
};
}

