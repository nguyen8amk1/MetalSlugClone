#include "PlayerAnimation.h"
#include "CollisionChecker.h"
#include "PlayerPhysic.h"
#include "../Util.h"
//#include "Windows.h"

namespace MetalSlug {
PlayerAnimation::PlayerAnimation(PlatformSpecficMethodsCollection *platformMethods) {
	globalGameData = GlobalGameData::getInstance();
	PlatformSpecificImage *spriteSheet = globalGameData->getSpriteSheet("MARCO_ROSSI");

	Util::AnimationUtil::initAnimationMetaData(idlingAnimationMetaData, spriteSheet, .15f, 1, 4, {0, 0}, {31, 29});
	idlingAnimation  = new Animation(idlingAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(walkingAnimationMetaData, spriteSheet, .15f, 1, 12, { 0, 29 }, {34, 29});
	walkingAnimation = new Animation(walkingAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(jumpingAnimationMetaData, spriteSheet, .1f, 1, 6, {0, 64}, {29, 26});
	jumpingAnimation = new Animation(jumpingAnimationMetaData,platformMethods);

	Util::AnimationUtil::initAnimationMetaData(fallingAnimationMetaData, spriteSheet, .1f, 1, 6, {145, 64}, {-29, 26});
	fallingAnimation = new Animation(fallingAnimationMetaData, platformMethods);

	// Player's Leg animation
	Util::AnimationUtil::initAnimationMetaData(walkingLegAnimationMetaData, spriteSheet, .1f, 1, 12, {405, 44}, {32, 20} );
	walkingLegAnimation  = new Animation(walkingLegAnimationMetaData,platformMethods);

	Util::AnimationUtil::initAnimationMetaData(idlingLegAnimationMetaData, spriteSheet, .1f, 1, 1, {124, 13}, {21, 16});
	idlingLegAnimation  = new Animation(idlingLegAnimationMetaData,platformMethods);

	Util::AnimationUtil::initAnimationMetaData(jumpingLegAnimationMetaData, spriteSheet, .1f, 1, 6, {174, 64}, {21, 25});
	jumpingLegAnimation  = new Animation(jumpingLegAnimationMetaData,platformMethods);

	Util::AnimationUtil::initAnimationMetaData(fallingLegAnimationMetaData, spriteSheet, .1f, 1, 6, {279, 64}, {-21, 25});
	fallingLegAnimation = new Animation(fallingLegAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(dieAnimationMetaData, spriteSheet, .1f, 1, 19, {0, 97}, {43, 40});
	dieAnimation = new Animation(dieAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(throwingAnimationMetaData, spriteSheet, .1f, 1, 6, {0, 138}, {36, 30});
	throwingAnimation = new Animation(throwingAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(horizontalShootingAnimationMetaData, spriteSheet, .1f, 1, 10, {0, 168}, {52, 29});
	horizontalShootingAnimation = new Animation(horizontalShootingAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(upShootingAnimationMetaData, spriteSheet, .1f, 1, 10, {0, 197}, {30, 65});
	upShootingAnimation = new Animation(upShootingAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(bulletMetaData, spriteSheet, .1f, 1, 1, {74, 476}, {12, 11});
	bullet = new Animation(bulletMetaData, platformMethods);

	currentBodyAnimation = idlingAnimation;
	currentLegAnimation = idlingLegAnimation;
}

PlayerAnimationResult PlayerAnimation::update(PlayerEvent &event, double dt, Camera *camera, LevelData &levelData, PlayerPhysicState physicState, Rect colliderRect, int horizontalFacingDirection, bool die) {
	bodyAnimationStateMachineUpdate(event, dt, camera, levelData, physicState, colliderRect, horizontalFacingDirection, die);
	legAnimationStateMachineUpdate(event, dt, camera, levelData, physicState, colliderRect, horizontalFacingDirection, die);
	
	float legX = colliderRect.x;
	float legY = colliderRect.y - .15f;

	if (currentLegAnimation) {
		currentLegAnimation->changePos(legX, legY);
		currentLegAnimation->flip(horizontalFacingDirection, 1);
		currentLegAnimation->animate(camera, dt);
	}

	currentBodyAnimation->changePos(colliderRect.x, colliderRect.y);
	currentBodyAnimation->flip(horizontalFacingDirection, 1);
	currentBodyAnimation->animate(camera, dt);

	return {colliderRect, horizontalFacingDirection, die };
}

bool PlayerAnimation::isThrowGrenade() {
	return grenadeIsThrow;

}

PlayerAnimation::~PlayerAnimation() {
	delete idlingAnimation;
	delete jumpingAnimation;
	delete fallingAnimation;
	delete walkingAnimation;

	delete idlingLegAnimation;
	delete walkingLegAnimation;
	delete jumpingLegAnimation;
	delete fallingLegAnimation;
}

void PlayerAnimation::legAnimationStateMachineUpdate(PlayerEvent &event, double dt, Camera *camera, LevelData &levelData, PlayerPhysicState physicState, Rect colliderRect, int &horizontalFacingDirection, bool &die) {
	switch (legAnimationState) {
	case LegAnimationState::IDLING: {
		commonWalkingLegEventTransition(event, horizontalFacingDirection);
		commonLegJumpFallEventTransition(physicState);
		commonLegDieEventTransition(die);
	}break;

	case LegAnimationState::WALKING: {
		bool isPressingMove = event.moveLeft || event.moveRight;
		if (!isPressingMove && 
			physicState != PlayerPhysicState::JUMPUP && 
			physicState != PlayerPhysicState::JUMPDOWN && 
			physicState != PlayerPhysicState::FALL) {

			toLegIdlingAnimation();
		}

		commonLegJumpFallEventTransition(physicState);
		commonLegDieEventTransition(die);
	} break;

	case LegAnimationState::JUMPING: {
		if (physicState == PlayerPhysicState::JUMPDOWN ||
			physicState == PlayerPhysicState::FALL) {
			toLegFallingAnimation();
		}

		commonLegDieEventTransition(die);
	} break;

	case LegAnimationState::FALLING: {
		if (physicState == PlayerPhysicState::ONGROUND) {
			toLegIdlingAnimation();
		}

		commonLegDieEventTransition(die);
	} break;

	case LegAnimationState::DYING: {
		// action: 
		// event: 
		if (doneDieAnimation) {
			toLegIdlingAnimation();
		}
	} break;
	}
}

void PlayerAnimation::bodyAnimationStateMachineUpdate (PlayerEvent &event, double dt, Camera *camera, LevelData &levelData, PlayerPhysicState physicState, Rect &colliderRect, int horizontalFacingDirection, bool &die) {
	switch (bodyAnimationState) {
	case BodyAnimationState::IDLING: {
		commonBodyWalkingEventTransition(event);
		commonThrowingBombEventTransition(event);
		commonShootingEventTransition(event);
		commonBodyJumpFallEventTransition(physicState);
		commonBodyDieEventTransition(die);
	}break;

	case BodyAnimationState::WALKING: {
		bool isPressingMove = event.moveLeft|| event.moveRight;
		if (!isPressingMove && 
			physicState != PlayerPhysicState::JUMPUP && 
			physicState != PlayerPhysicState::JUMPDOWN && 
			physicState != PlayerPhysicState::FALL) {

			toBodyIdlingAnimation();
		}

		commonBodyJumpFallEventTransition(physicState);
		commonBodyDieEventTransition(die);
		commonThrowingBombEventTransition(event);
		commonShootingEventTransition(event);
	} break;

	case BodyAnimationState::JUMPING: {
		if (physicState == PlayerPhysicState::JUMPDOWN ||
			physicState == PlayerPhysicState::FALL) {
			toBodyFallingAnimation();
		}

		commonBodyDieEventTransition(die);
		commonThrowingBombEventTransition(event);
		commonShootingEventTransition(event);
	} break;

	case BodyAnimationState::FALLING: {
		if (physicState == PlayerPhysicState::ONGROUND) {
			toBodyIdlingAnimation();
		}

		commonBodyDieEventTransition(die);
		commonThrowingBombEventTransition(event);
		commonShootingEventTransition(event);
	} break;

	case BodyAnimationState::DYING: {
		// action: 
		// event: 
		doneDieAnimation = dieAnimation->finishOneCycle();
		if (doneDieAnimation) {
			toBodyIdlingAnimation();
			colliderRect.x -= .2f;
			die = false;
		}
	} break;
	case BodyAnimationState::THROWING: {

		bool isPressingMove = event.moveLeft || event.moveRight;
		if (!isPressingMove && 
			physicState != PlayerPhysicState::JUMPUP && 
			physicState != PlayerPhysicState::JUMPDOWN && 
			physicState != PlayerPhysicState::FALL) {
			currentBodyAnimation = idlingAnimation;
		}

		// event: 
		bool doneThrowAnimation = throwingAnimation->finishOneCycle();
		if (doneThrowAnimation) {
			// TODO: some how go to the state that it should be, one of 4:  jumping, falling, walking, idling 
			toBodyIdlingAnimation();
		}

		commonBodyWalkingEventTransition(event);
		commonBodyJumpFallEventTransition(physicState);
		commonBodyDieEventTransition(die);

		commonThrowingBombEventTransition(event);
		commonShootingEventTransition(event);
	} break;

	case BodyAnimationState::HORIZONTAL_SHOOTING: {
		bool doneShootingAnimation = horizontalShootingAnimation->finishOneCycle();
		if (doneShootingAnimation) {
			toBodyIdlingAnimation();
		}
		commonBodyWalkingEventTransition(event);

		commonBodyJumpFallEventTransition(physicState);
		commonBodyDieEventTransition(die);

		commonThrowingBombEventTransition(event);
	} break;

	case BodyAnimationState::UP_SHOOTING: {
		bool doneShootingAnimation = upShootingAnimation->finishOneCycle();
		if (doneShootingAnimation) {
			toBodyIdlingAnimation();
		}
		commonBodyWalkingEventTransition(event);

		commonBodyJumpFallEventTransition(physicState);
		commonBodyDieEventTransition(die);

		commonThrowingBombEventTransition(event);
	} break;

	}


}

void PlayerAnimation::commonLegJumpFallEventTransition(PlayerPhysicState physicState) {
	if (physicState == PlayerPhysicState::JUMPUP) {
		toLegJumpingAnimation();
	}
	else if (physicState == PlayerPhysicState::FALL) {
		toLegFallingAnimation();
	}
}

void PlayerAnimation::commonBodyJumpFallEventTransition(PlayerPhysicState physicState) {
	if (physicState == PlayerPhysicState::JUMPUP) {
		toBodyJumpingAnimation();
	}
	else if (physicState == PlayerPhysicState::FALL) {
		toBodyFallingAnimation();
	}

}

void PlayerAnimation::commonLegDieEventTransition(bool die) {
	if (die) {
		toLegDyingAnimation();
	}
}

void PlayerAnimation::commonBodyDieEventTransition(bool die) {
	if (die) {
		toBodyDyingAnimation();
	}
}

void PlayerAnimation::commonThrowingBombEventTransition(PlayerEvent &event) {
	if (event.throwGrenade) {
		grenadeIsThrow = true;
		toThrowingAnimation();
	}
	else {
		grenadeIsThrow = false;
	}
}

void PlayerAnimation::commonShootingEventTransition(PlayerEvent &event) {
	bool verticaleventIsDown = event.up && event.down;
	if (!verticaleventIsDown && event.shoot) {
		toHorizontalShootingAnimation();
	}

	if (event.up && event.shoot) {
		toUpShootingAnimation();

	}
}

void PlayerAnimation::commonBodyWalkingEventTransition(PlayerEvent &event) {
	if (event.moveLeft || event.moveRight) {
		bodyAnimationState = BodyAnimationState::WALKING;
		currentBodyAnimation = walkingAnimation;
	}
}

void PlayerAnimation::commonWalkingLegEventTransition(PlayerEvent &event, int &horizontalFacingDirection) {
	if (event.moveLeft) {
		legAnimationState = LegAnimationState::WALKING;
		horizontalFacingDirection = -1;
		currentLegAnimation = walkingLegAnimation;
	}
	else if (event.moveRight) {
		legAnimationState = LegAnimationState::WALKING;
		horizontalFacingDirection = 1;
		currentLegAnimation = walkingLegAnimation;
	}
}



void PlayerAnimation::toBodyIdlingAnimation() {
	bodyAnimationState = BodyAnimationState::IDLING;
	currentBodyAnimation = idlingAnimation;
}

void PlayerAnimation::toLegIdlingAnimation() {
	legAnimationState = LegAnimationState::IDLING;
	currentLegAnimation = idlingLegAnimation;
}

void PlayerAnimation::toBodyFallingAnimation() {
	bodyAnimationState = BodyAnimationState::FALLING;
	currentBodyAnimation = fallingAnimation;
	currentLegAnimation = fallingLegAnimation;
}

void PlayerAnimation::toLegFallingAnimation() {
	legAnimationState = LegAnimationState::FALLING;
	currentLegAnimation = fallingLegAnimation;
}

void PlayerAnimation::toBodyDyingAnimation() {
	bodyAnimationState = BodyAnimationState::DYING;
	currentBodyAnimation = dieAnimation;
}

void PlayerAnimation::toLegDyingAnimation() {
	legAnimationState = LegAnimationState::DYING;
	currentLegAnimation = NULL;
}

void PlayerAnimation::toBodyJumpingAnimation() {
	bodyAnimationState = BodyAnimationState::JUMPING;
	currentBodyAnimation = jumpingAnimation;
}

void PlayerAnimation::toLegJumpingAnimation() {
	legAnimationState = LegAnimationState::JUMPING;
	currentLegAnimation = jumpingLegAnimation;
}

void PlayerAnimation::toThrowingAnimation() {
	bodyAnimationState = BodyAnimationState::THROWING;
	currentBodyAnimation = throwingAnimation;
}

void PlayerAnimation::toHorizontalShootingAnimation() {
	bodyAnimationState = BodyAnimationState::HORIZONTAL_SHOOTING;
	currentBodyAnimation = horizontalShootingAnimation;
}

void PlayerAnimation::toUpShootingAnimation() {
	bodyAnimationState = BodyAnimationState::UP_SHOOTING;
	currentBodyAnimation = upShootingAnimation;
}

}
