#pragma once
#include "MetalSlug.h"
#include "Animation.h"
#include "GlobalGameData.h"

namespace MetalSlug {
class GlobalGameData;

struct PlayerAnimationResult {
	Rect colliderRect;
	int horizontalFacingDirection;
	bool die;
};

class PlayerAnimation {
private: 
	enum class BodyAnimationState {
		IDLING,
		JUMPING,
		FALLING,
		WALKING,
		DYING,
		THROWING,
		HORIZONTAL_SHOOTING, 
		UP_SHOOTING
	};

	enum class LegAnimationState {
		IDLING,
		JUMPING,
		FALLING,
		WALKING,
		DYING
	};

	BodyAnimationState bodyAnimationState = BodyAnimationState::IDLING;
	LegAnimationState legAnimationState = LegAnimationState::IDLING;

	AnimationMetaData idlingAnimationMetaData;
	AnimationMetaData jumpingAnimationMetaData;
	AnimationMetaData fallingAnimationMetaData;
	AnimationMetaData walkingAnimationMetaData;

	Animation* idlingAnimation;
	Animation* jumpingAnimation;
	Animation* fallingAnimation;
	Animation* walkingAnimation;

	AnimationMetaData walkingLegAnimationMetaData;
	Animation* walkingLegAnimation;

	AnimationMetaData idlingLegAnimationMetaData;
	Animation* idlingLegAnimation;

	AnimationMetaData jumpingLegAnimationMetaData;
	Animation* jumpingLegAnimation;

	AnimationMetaData fallingLegAnimationMetaData;
	Animation* fallingLegAnimation;

	AnimationMetaData dieAnimationMetaData; 
	Animation* dieAnimation;

	AnimationMetaData throwingAnimationMetaData; 
	Animation* throwingAnimation;

	AnimationMetaData horizontalShootingAnimationMetaData; 
	Animation* horizontalShootingAnimation;

	AnimationMetaData upShootingAnimationMetaData; 
	Animation* upShootingAnimation;

	AnimationMetaData bulletMetaData; 
	Animation* bullet;

	Animation* currentBodyAnimation;
	Animation* currentLegAnimation;

	bool grenadeIsThrow = false; 
	bool doneDieAnimation = false;
	GlobalGameData* globalGameData;

public: 
	PlayerAnimation(PlatformSpecficMethodsCollection* platformMethods);

	PlayerAnimationResult update(PlayerEvent& event, double dt, Camera* camera, LevelData& levelData, PlayerPhysicState physicState, Rect colliderRect, int horizontalFacingDirection, bool die);

	bool isThrowGrenade();

	~PlayerAnimation();

private: 
	void legAnimationStateMachineUpdate(PlayerEvent& event, double dt, Camera* camera, LevelData& levelData, PlayerPhysicState physicState, Rect colliderRect, int& horizontalFacingDirection, bool& die);

	void bodyAnimationStateMachineUpdate(PlayerEvent& event, double dt, Camera* camera, LevelData& levelData, PlayerPhysicState physicState, Rect& colliderRect, int horizontalFacingDirection, bool& die);

	void commonLegJumpFallEventTransition(PlayerPhysicState physicState);

	void commonBodyJumpFallEventTransition(PlayerPhysicState physicState);

	void commonLegDieEventTransition(bool die);

	void commonBodyDieEventTransition(bool die);

	void commonThrowingBombEventTransition(PlayerEvent &event);

	void commonShootingEventTransition(PlayerEvent& event);

	void commonBodyWalkingEventTransition(PlayerEvent& event);

	void commonWalkingLegEventTransition(PlayerEvent& event, int& horizontalFacingDirection);

	void toBodyIdlingAnimation();

	void toLegIdlingAnimation();

	void toBodyFallingAnimation();

	void toLegFallingAnimation();

	void toBodyDyingAnimation();

	void toLegDyingAnimation();

	void toBodyJumpingAnimation();

	void toLegJumpingAnimation();

	void toThrowingAnimation();

	void toHorizontalShootingAnimation();

	void toUpShootingAnimation();
};

}
