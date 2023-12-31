#pragma once 
#include "MetalSlug.h"
#include "Animation.h"
#include "CollisionChecker.h"
//#include "PlayerPhysic.h"
//#include "PlayerAnimation.h"
#include "GlobalGameData.h"

//#include "Windows.h"
#include "Grenade.h"
#include "Camera.h"

namespace MetalSlug {

class Grenade;
class GlobalGameData;
class PlayerAnimation;

class Player {
private: 
	// Physics 
	enum class PlayerPhysicState {
		ONGROUND,
		JUMPUP,
		JUMPDOWN,
		FALL
	};

	PlayerPhysicState physicState = PlayerPhysicState::ONGROUND;

	float jumpTotalTime = 1.0f;
	float jumpT = 0;
	float jumpTimeAccumulator = 0;

	float originalGroundY = 0;
	float jumpHeight = .5777f;


	// Animations
	enum class BodyAnimationState {
		IDLING,
		JUMPING,
		FALLING,
		WALKING,
		DYING,
		THROWING,
		HORIZONTAL_SHOOTING, 
		UP_SHOOTING, 
		SLASH, 
		STAB 
	};

	enum class LegAnimationState {
		IDLING,
		JUMPING,
		FALLING,
		WALKING,
		DYING
	};

	//Body animation
	BodyAnimationState bodyAnimationState = BodyAnimationState::IDLING;
	AnimationMetaData idlingAnimationMetaData;
	AnimationMetaData jumpingAnimationMetaData;
	AnimationMetaData fallingAnimationMetaData;
	AnimationMetaData walkingAnimationMetaData;

	Animation* idlingAnimation;
	Animation* jumpingAnimation;
	Animation* fallingAnimation;
	Animation* walkingAnimation;

	AnimationMetaData dieAnimationMetaData; 
	Animation* dieAnimation;

	AnimationMetaData throwingAnimationMetaData; 
	Animation* throwingAnimation;

	AnimationMetaData horizontalShootingAnimationMetaData; 
	Animation* horizontalShootingAnimation;

	AnimationMetaData upShootingAnimationMetaData; 
	Animation* upShootingAnimation;

	AnimationMetaData slashingAnimationMetaData; 
	Animation* slashingAnimation;

	AnimationMetaData stabbingAnimationMetaData; 
	Animation* stabbingAnimation;


	AnimationMetaData bulletMetaData; 
	Animation* bullet;

	// Leg animation
	LegAnimationState legAnimationState = LegAnimationState::IDLING;
	AnimationMetaData walkingLegAnimationMetaData;
	Animation* walkingLegAnimation;

	AnimationMetaData idlingLegAnimationMetaData;
	Animation* idlingLegAnimation;

	AnimationMetaData jumpingLegAnimationMetaData;
	Animation* jumpingLegAnimation;

	AnimationMetaData fallingLegAnimationMetaData;
	Animation* fallingLegAnimation;



	Animation* currentBodyAnimation;
	Animation* currentLegAnimation;

	bool grenadeIsThrow = false; 
	bool doneDieAnimation = false;


	// Player
	Rect colliderRect = { -17.8f, 1.0f, .2f, .4f };

	float moveSpeed, gravity;

	bool die = false;

	Rect interactionRectDisabledRect = {0, -5.0f, 0, 0};
	Rect interactionRect;

	int horizontalFacingDirection = 1;


	PlatformSpecficMethodsCollection* platformMethods;

	bool hostageDoesHitInteractionRect = false;
	bool rebelDoesHitInteractionRect = false;

	GlobalGameData* globalGameData;

public: 
	Player(float gravity, float moveSpeed, PlatformSpecficMethodsCollection* platformMethods);
	~Player();

	void update(const GameInputContext& input, Camera* camera, double dt);

	void moveXBy(float d) {
		colliderRect.x += d;
	}

	void moveYBy(float d) {
		colliderRect.y += d;
	}

	Rect getRect() { return colliderRect; }
	Rect getInteractionRect() { return interactionRect; }

	void rebelHitInteractionRect();
	void hostageHitInteractionRect();

private: 
	void interactionRectInit();

	void shootBullet();

	void throwGrenade();
	
	// Physics 
	void updatePhysics(PlayerEvent &event, double dt);
	void commonDieEventTransition();
	void commonOnGroundEventTransition();


	// Animations
	void animationInit();
	void updateAnimation(PlayerEvent& event, double dt, Camera* camera);

	// BODY ANIMATION
	void bodyAnimationStateMachineUpdate(PlayerEvent& event, double dt, Camera* camera);
	void commonBodyJumpFallEventTransition(PlayerPhysicState physicState);
	void commonBodyDieEventTransition(bool die);
	void commonBodyWalkingEventTransition(PlayerEvent& event);
	void commonShootingEventTransition(PlayerEvent& event);
	void commonSlashingEventTransition();
	void commonStabbingEventTransition();

	void toBodySlashingAnimation();
	void toBodyStabbingAnimation();
	void toBodyIdlingAnimation();
	void toBodyFallingAnimation();
	void toBodyDyingAnimation();
	void toBodyJumpingAnimation();

	void toThrowingAnimation();
	void toHorizontalShootingAnimation();
	void toUpShootingAnimation();

	// LEG ANIMATION
	void legAnimationStateMachineUpdate(PlayerEvent& event, double dt, Camera* camera);
	void commonLegJumpFallEventTransition(PlayerPhysicState physicState);
	void commonLegDieEventTransition(bool die);
	void commonLegIdlingEventTransition(PlayerEvent& event, PlayerPhysicState physicState);
	void commonThrowingBombEventTransition(PlayerEvent &event);
	void commonWalkingLegEventTransition(PlayerEvent& event);

	void toLegIdlingAnimation();
	void toLegFallingAnimation();
	void toLegDyingAnimation();
	void toLegJumpingAnimation();

};

}
