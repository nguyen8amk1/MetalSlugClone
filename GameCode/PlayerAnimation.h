#pragma once
#include "MetalSlug.h"
#include "Animation.h"
#include "CollisionChecker.h"
#include "PlayerPhysic.h"
#include "../Util.cpp"

namespace MetalSlug {
struct PlayerAnimationResult {
	Rect colliderRect;
	int horizontalFacingDirection;
	bool die;
};

class PlayerAnimation {
private: 
	enum class AnimationState {
		IDLING,
		JUMPING,
		FALLING,
		WALKING,
		DYING,
		THROWING,
		SHOOTING // is another state that is similar to throwing, since it's not dependent on leg animation
	};

	AnimationState animationState = AnimationState::IDLING;

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

	Animation* currentAnimation;
	Animation* currentLegAnimation;

	bool grenadeIsThrow = false;

public: 
	PlayerAnimation(PlatformSpecficMethodsCollection *platformMethods) {
		std::string filename = "Assets/Imgs/Characters/marco_messi.png";
		Util::AnimationUtil::initAnimationMetaData(idlingAnimationMetaData, filename, .15f, 1, 4, {0, 0}, {31, 29});
		idlingAnimation  = new Animation(idlingAnimationMetaData, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(walkingAnimationMetaData, filename, .15f, 1, 12, { 0, 29 }, {34, 29});
		walkingAnimation = new Animation(walkingAnimationMetaData, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(jumpingAnimationMetaData, filename, .1f, 1, 6, {0, 64}, {29, 26});
		jumpingAnimation = new Animation(jumpingAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(fallingAnimationMetaData, filename, .1f, 1, 6, {145, 64}, {-29, 26});
		fallingAnimation = new Animation(fallingAnimationMetaData, platformMethods);

		// Player's Leg animation
		Util::AnimationUtil::initAnimationMetaData(walkingLegAnimationMetaData, filename, .1f, 1, 12, {405, 44}, {32, 20} );
		walkingLegAnimation  = new Animation(walkingLegAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(idlingLegAnimationMetaData, filename, .1f, 1, 1, {124, 13}, {21, 16});
		idlingLegAnimation  = new Animation(idlingLegAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(jumpingLegAnimationMetaData, filename, .1f, 1, 6, {174, 64}, {21, 25});
		jumpingLegAnimation  = new Animation(jumpingLegAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(fallingLegAnimationMetaData, filename, .1f, 1, 6, {279, 64}, {-21, 25});
		fallingLegAnimation = new Animation(fallingLegAnimationMetaData, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(dieAnimationMetaData, filename, .1f, 1, 19, {0, 97}, {43, 40});
		dieAnimation = new Animation(dieAnimationMetaData, platformMethods);

		// TODO: need the animation in the file 
		Util::AnimationUtil::initAnimationMetaData(throwingAnimationMetaData, filename, .1f, 1, 19, {0, 200}, {43, 40});
		throwingAnimation = new Animation(throwingAnimationMetaData, platformMethods);

		currentAnimation = idlingAnimation;
		currentLegAnimation = idlingLegAnimation;
	}

	PlayerAnimationResult update(const GameInputContext &input, double dt, Camera *camera, LevelData &levelData, PlayerPhysicState physicState, Rect colliderRect, int horizontalFacingDirection, bool die) {
		switch (animationState) {
		case AnimationState::IDLING: {
			bool onGround = physicState == PlayerPhysicState::ONGROUND;
			if (input.left.isDown) {
				animationState = AnimationState::WALKING;
				horizontalFacingDirection = -1;
				currentAnimation = walkingAnimation;
				currentLegAnimation = walkingLegAnimation;
			}
			else if (input.right.isDown) {
				animationState = AnimationState::WALKING;
				horizontalFacingDirection = 1;
				currentAnimation = walkingAnimation;
				currentLegAnimation = walkingLegAnimation;
			}

			if (input.jump.isDown) {
				toJumpingAnimation();
			}
			else if (!onGround) {
				toFallingAnimation();
			}

			if (die) {
				toDyingAnimation();
			}
			else if (input.throwGrenade.isDown) {
				grenadeIsThrow = true;
				toThrowingAnimation();
			}
			else {
				grenadeIsThrow = false;
			}

		}break;

		case AnimationState::WALKING: {

			bool isPressingMove = input.left.isDown || input.right.isDown;
			if (!isPressingMove && 
				physicState != PlayerPhysicState::JUMPUP && 
				physicState != PlayerPhysicState::JUMPDOWN && 
				physicState != PlayerPhysicState::FALL) {

				toIdlingAnimation();
			}
			else if (physicState == PlayerPhysicState::JUMPUP) {
				toJumpingAnimation();
			}
			else if (physicState == PlayerPhysicState::FALL) {
				toFallingAnimation();
			}

			if (die) {
				toDyingAnimation();
			}
			else if (input.throwGrenade.isDown) {
				grenadeIsThrow = true;
				toThrowingAnimation();
			}
			else {
				grenadeIsThrow = false;
			}

		} break;

		case AnimationState::JUMPING: {
			if (physicState == PlayerPhysicState::JUMPDOWN ||
				physicState == PlayerPhysicState::FALL) {
				toFallingAnimation();
			}

			if (!die && input.throwGrenade.isDown) {
				grenadeIsThrow = true;
				toThrowingAnimation();
			}
			else {
				grenadeIsThrow = false;
			}

		} break;

		case AnimationState::FALLING: {
			if (physicState == PlayerPhysicState::ONGROUND) {
				toIdlingAnimation();
			}

			if (!die && input.throwGrenade.isDown) {
				grenadeIsThrow = true;
				toThrowingAnimation();
			}
			else {
				grenadeIsThrow = false;
			}
		} break;

		case AnimationState::DYING: {
			// action: 
			// event: 
			bool doneDieAnimation = dieAnimation->finishOneCycle();
			if (doneDieAnimation) {
				toIdlingAnimation();
				colliderRect.x -= .2f;
				die = false;
			}
		} break;

		case AnimationState::THROWING: {
			// action: 
			// FIXME: This is not how it's suppose to work, need huge refactor later since the leg transition here is wrong 
			if (input.left.isDown) {
				horizontalFacingDirection = -1;
				currentLegAnimation = walkingLegAnimation;
			}
			else if (input.right.isDown) {
				horizontalFacingDirection = 1;
				currentLegAnimation = walkingLegAnimation;
			}

			bool isPressingMove = input.left.isDown || input.right.isDown;
			if (!isPressingMove && 
				physicState != PlayerPhysicState::JUMPUP && 
				physicState != PlayerPhysicState::JUMPDOWN && 
				physicState != PlayerPhysicState::FALL) {
				currentLegAnimation = idlingLegAnimation;
			}
			else if (physicState == PlayerPhysicState::JUMPUP) {
				currentLegAnimation = jumpingLegAnimation;
			}
			else if (physicState == PlayerPhysicState::FALL) {
				currentLegAnimation = fallingLegAnimation;
			}

			if (!die && input.throwGrenade.isDown) {
				grenadeIsThrow = true;
				// TODO: reset the throwing animation
			}
			else {
				grenadeIsThrow = false;
			}

			// event: 
			bool doneThrowAnimation = throwingAnimation->finishOneCycle();
			if (doneThrowAnimation) {
				// TODO: some how go to the state that it should be, one of 4:  jumping, falling, walking, idling 
				toIdlingAnimation();
			}
		} break;

		default: 
			// TODO: handle error "Animation state not recoginize"
			break;
		}

		float legX = colliderRect.x;
		float legY = colliderRect.y - .15f;

		if (currentLegAnimation) {
			currentLegAnimation->changePos(legX, legY);
			currentLegAnimation->flip(horizontalFacingDirection, 1);
			currentLegAnimation->animate(camera, dt);
		}

		currentAnimation->changePos(colliderRect.x, colliderRect.y);
		currentAnimation->flip(horizontalFacingDirection, 1);
		currentAnimation->animate(camera, dt);

		return {colliderRect, horizontalFacingDirection, die };
	}

	bool isThrowGrenade() {
		return grenadeIsThrow;

	}

	~PlayerAnimation() {
		delete idlingAnimation;
		delete jumpingAnimation;
		delete fallingAnimation;
		delete walkingAnimation;

		delete idlingLegAnimation;
		delete walkingLegAnimation;
		delete jumpingLegAnimation;
		delete fallingLegAnimation;
	}

private: 
	void toIdlingAnimation() {
		animationState = AnimationState::IDLING;
		currentAnimation = idlingAnimation;
		currentLegAnimation = idlingLegAnimation;
	}

	void toFallingAnimation() {
		animationState = AnimationState::FALLING;
		currentAnimation = fallingAnimation;
		currentLegAnimation = fallingLegAnimation;
	}

	void toDyingAnimation() {
		animationState = AnimationState::DYING;
		currentAnimation = dieAnimation;
		currentLegAnimation = NULL;
	}

	void toJumpingAnimation() {
		animationState = AnimationState::JUMPING;
		currentAnimation = jumpingAnimation;
		currentLegAnimation = jumpingLegAnimation;
	}

	void toThrowingAnimation() {
		animationState = AnimationState::THROWING;
		currentAnimation = throwingAnimation;
	}
};

}
