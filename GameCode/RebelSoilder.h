#pragma once 
#include "MetalSlug.h"
#include "Animation.h"

namespace MetalSlug {
// TODO: edit the animation in PTS then load into the game 

class RebelSoilder: CameraControlledEntity {
private: 
	// Collider 
	Rect colliderRect;

	// Physic state machine  
	enum class PhysicState {
		ONGROUND, 
		FALL 
	};

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
		THROWING_BOMB	// 0, 79, 782, 42 
	};

	PhysicState currentPhysicState = PhysicState::ONGROUND;
	AnimationState currentAnimationState = AnimationState::IDLING;

public: 
	RebelSoilder(float gravity, float moveSpeed, Rect colliderRect, PlatformSpecficMethodsCollection *platformMethods) {
		// TODO: 
		this->colliderRect = colliderRect;

	}

	void update(const GameInputContext& input, LevelData &levelData, Camera *camera, double dt) {
		// TODO: 
	}
};

}
