#pragma once 
#include "MetalSlug.h"
#include "Animation.h"
#include "CollisionChecker.h"
#include "Physics.h"
#include "GlobalGameData.h"
#include <Windows.h>

namespace MetalSlug {
class GlobalGameData;

class RebelSoilder {
private:
	// Collider 
	Rect colliderRect;
	Rect interactionRectDisabledRect = { 0, -5.0f, 0, 0 };
	Rect interactionRect;


	// Animation
	AnimationMetaData idleAnimationMetaData;
	Animation* idleAnimation;

	AnimationMetaData slashingAnimationMetaData;
	Animation* slashingAnimation;

	AnimationMetaData throwingBombAnimationMetaData;
	Animation* throwingBombAnimation;

	// Animation state machine 
	enum class AnimationState {
		IDLING,			// 0, 4, 108, 38
		SLASHING,		// 0, 42, 735, 37
		THROWING_BOMB,	// 0, 79, 782, 42 
		DIE
	};

	AnimationState currentAnimationState = AnimationState::IDLING;
	Animation* currentAnimation;

	PlatformSpecficMethodsCollection* platformMethods;
	float gravity;
	float moveSpeed;
	BasicPhysicStateMachine* physicStateMachine;

	double timeAccumulator = 0.0;

	BasicPhysicStateMachineResult physicResult = {colliderRect};

	GlobalGameData *globalGameData;

public: 
	RebelSoilder(float gravity, float moveSpeed, Rect colliderRect, PlatformSpecficMethodsCollection* platformMethods); 
	void update(LevelData& levelData, Camera* camera, double dt);

	void moveXBy(float d) {
		colliderRect.x += d;
	}

	void moveYBy(float d) {
		colliderRect.y += d;
	}

	Rect getInteractionRect() { return interactionRect; }

private: 
	void toThrowingBombAnimation();

	void toSlashingAnimation();

	void toDieAnimation();

	void dieEventTransition();

};

}
