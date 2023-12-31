#pragma once 
#include "Player.h"
#include<Windows.h>


namespace MetalSlug { 

Player::Player(float gravity, float moveSpeed, PlatformSpecficMethodsCollection *platformMethods) {
	this->moveSpeed = moveSpeed;
	this->gravity = gravity;
	this->platformMethods = platformMethods;
	globalGameData = GlobalGameData::getInstance();
	animationInit();
}

Player::~Player() {
	delete idlingAnimation;
	delete jumpingAnimation;
	delete fallingAnimation;
	delete walkingAnimation;

	delete idlingLegAnimation;
	delete walkingLegAnimation;
	delete jumpingLegAnimation;
	delete fallingLegAnimation;
}


void Player::update(const GameInputContext &input, Camera *camera, double dt) {

	PlayerEvent event;
	event.jump = input.jump.isDown;
	event.moveLeft = input.left.isDown;
	event.moveRight = input.right.isDown;
	event.up = input.up.isDown;
	event.down = input.down.isDown;
	event.throwGrenade = input.throwGrenade.isPressed;
	event.shoot = input.shoot.isDown;

	// @Debug
	/*
	if (hostageDoesHitInteractionRect) {
		OutputDebugStringA("HOSTAGE HIT INTERACTION RECT \n");
	}
	*/

	if (globalGameData->doesLevelStarted()) {
		if (!die) {
			if (event.shoot) {
				interactionRectInit();
			}
			else {
				interactionRect = interactionRectDisabledRect;
			}
		}

		updatePhysics(event, dt);
	}

	updateAnimation(event, dt, camera);

	rebelDoesHitInteractionRect = false;
	hostageDoesHitInteractionRect = false;
}

void Player::interactionRectInit() {
	bool facingRight = horizontalFacingDirection == 1;
	bool facingLeft = horizontalFacingDirection == -1;
	if (facingRight) {
		interactionRect.x = colliderRect.x + colliderRect.width/2;
	}
	else if(facingLeft){
		interactionRect.x = colliderRect.x - colliderRect.width/2;
	}

	interactionRect.y = colliderRect.y;
	interactionRect.width = colliderRect.width/2;
	interactionRect.height = colliderRect.height/2;
}

void Player::shootBullet() {
	globalGameData->spawnBullet(colliderRect.x, colliderRect.y);
}

void Player::throwGrenade() {
	globalGameData->spawnGrenade("PLAYER", horizontalFacingDirection, colliderRect.x, colliderRect.y);
}

// Physics 
void Player::updatePhysics(PlayerEvent &event, double dt) {
	if (!die) {
		if (event.moveLeft) {
			colliderRect.x -= (float)(moveSpeed*dt); 
		}
		else if (event.moveRight) {
			colliderRect.x += (float)(moveSpeed*dt); 
		}
	}

	// Physics state machine
	if (physicState == PlayerPhysicState::FALL) {
		colliderRect.y -= (float)(gravity*dt); 

		commonOnGroundEventTransition();
		commonDieEventTransition();
	}
	else if (physicState == PlayerPhysicState::ONGROUND) {
		bool collided = false;
		for (RectangleCollider* ground : *(globalGameData->getGroundColliders())) {
			collided = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, ground->getRect());
			if (collided) {
				break;
			}
		}

		if (!collided) {
			physicState = PlayerPhysicState::FALL;
		}

		else if(!die && event.jump) {
			physicState = PlayerPhysicState::JUMPUP;
			jumpT = 0;
			jumpTimeAccumulator = 0;
			originalGroundY = colliderRect.y; 
		}

		commonDieEventTransition();
	}
	else if (physicState == PlayerPhysicState::JUMPUP) {

		jumpTimeAccumulator += dt;
		Util::Math::clampf(jumpTimeAccumulator, 0, jumpTotalTime/2.0f);
		jumpT = Util::Math::normalizef(jumpTimeAccumulator, jumpTotalTime/2.0f);

		if (jumpT >= 1.0f) {
			jumpT -= 1.0f;
			jumpTimeAccumulator = 0;
			physicState = PlayerPhysicState::JUMPDOWN;
		}
		else {
			float yd = Util::Math::upCurve(jumpT)* jumpHeight;
			colliderRect.y = originalGroundY + yd;
		}
		commonDieEventTransition();
	}
	else if (physicState == PlayerPhysicState::JUMPDOWN) {
		jumpTimeAccumulator += dt;
		Util::Math::clampf(jumpTimeAccumulator, 0, jumpTotalTime/2.0f);
		jumpT = Util::Math::normalizef(jumpTimeAccumulator, jumpTotalTime/2.0f);
		float yd = Util::Math::downCurve(jumpT) * jumpHeight;
		colliderRect.y = originalGroundY + yd;

		commonOnGroundEventTransition();
		commonDieEventTransition();
	}
}

void Player::commonOnGroundEventTransition() {
	CollisionInfo colInfo;
	for (RectangleCollider* ground : *(globalGameData->getGroundColliders())) {
		CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, ground->getRect(), colInfo);
		if (colInfo.count > 0) {
			break;
		}
	}

	if (colInfo.count > 0) {
		physicState = PlayerPhysicState::ONGROUND;
		colliderRect.x -= colInfo.normal.x * colInfo.depths[0];
		colliderRect.y -= colInfo.normal.y * colInfo.depths[0];
	}
}

void Player::commonDieEventTransition() {
	bool hitDangerRect = false;
	for (Rect dangerRect: *(globalGameData->getDangerRects())) {
		hitDangerRect = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, dangerRect);
		if (hitDangerRect) break;
	}

	if (hitDangerRect) {
		die = true;
	}
}

// Animations
void Player::animationInit() {
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

	Util::AnimationUtil::initAnimationMetaData(slashingAnimationMetaData, spriteSheet, .1f, 1, 6, {0, 265}, {45, 48});
	slashingAnimation = new Animation(slashingAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(stabbingAnimationMetaData, spriteSheet, .1f, 1, 6, {0,310}, {48,33});
	stabbingAnimation = new Animation(stabbingAnimationMetaData, platformMethods);

	currentBodyAnimation = idlingAnimation;
	currentLegAnimation = idlingLegAnimation;
}

void Player::updateAnimation(PlayerEvent &event, double dt, Camera *camera) {
	bodyAnimationStateMachineUpdate(event, dt, camera);
	legAnimationStateMachineUpdate(event, dt, camera);
	
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
}


void Player::legAnimationStateMachineUpdate(PlayerEvent &event, double dt, Camera *camera) {
	switch (legAnimationState) {
	case LegAnimationState::IDLING: {
		commonWalkingLegEventTransition(event);
		commonLegJumpFallEventTransition(physicState);
		commonLegDieEventTransition(die);
	}break;

	case LegAnimationState::WALKING: {
		commonLegIdlingEventTransition(event, physicState);

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

void Player::bodyAnimationStateMachineUpdate (PlayerEvent &event, double dt, Camera *camera ) {
	switch (bodyAnimationState) {
	case BodyAnimationState::IDLING: {
		commonBodyWalkingEventTransition(event);
		commonThrowingBombEventTransition(event);
		commonBodyJumpFallEventTransition(physicState);
		commonBodyDieEventTransition(die);
		commonSlashingEventTransition();
		commonStabbingEventTransition();
		commonShootingEventTransition(event);
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
		commonSlashingEventTransition();
		commonStabbingEventTransition();
	} break;

	case BodyAnimationState::JUMPING: {
		if (physicState == PlayerPhysicState::JUMPDOWN ||
			physicState == PlayerPhysicState::FALL) {
			toBodyFallingAnimation();
		}

		commonBodyDieEventTransition(die);
		commonThrowingBombEventTransition(event);
		commonShootingEventTransition(event);
		commonSlashingEventTransition();
		commonStabbingEventTransition();
	} break;

	case BodyAnimationState::FALLING: {
		if (physicState == PlayerPhysicState::ONGROUND) {
			toBodyIdlingAnimation();
		}

		commonBodyDieEventTransition(die);
		commonThrowingBombEventTransition(event);
		commonShootingEventTransition(event);
		commonSlashingEventTransition();
		commonStabbingEventTransition();
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
		// event: 
		bool doneThrowAnimation = throwingAnimation->finishOneCycle();
		if (doneThrowAnimation) {
			// TODO: some how go to the state that it should be, one of 4:  jumping, falling, walking, idling 
			//OutputDebugStringA("DONE THROWING ANIMATION\n");
			toBodyIdlingAnimation();
			commonBodyWalkingEventTransition(event);
			commonBodyJumpFallEventTransition(physicState);
			commonThrowingBombEventTransition(event);
			commonShootingEventTransition(event);
			commonSlashingEventTransition();
			commonStabbingEventTransition();
		}
		commonBodyDieEventTransition(die);
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
		commonSlashingEventTransition();
		commonStabbingEventTransition();
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
		commonSlashingEventTransition();
		commonStabbingEventTransition();
	} break;

	case BodyAnimationState::SLASH: {
		// TODO: 
		// action 
		// transition
		bool finish1Cycle = currentBodyAnimation->finishOneCycle();
		if (finish1Cycle) {
			commonBodyJumpFallEventTransition(physicState);
			commonBodyWalkingEventTransition(event);
		}
	} break;

	case BodyAnimationState::STAB: {
		// TODO: 
		// action 
		// transition
		bool finish1Cycle = currentBodyAnimation->finishOneCycle();
		if (finish1Cycle) {
			commonBodyJumpFallEventTransition(physicState);
			commonBodyWalkingEventTransition(event);
		}
	} break;

	}
}


void Player::rebelHitInteractionRect() {
	rebelDoesHitInteractionRect = true;
}

void Player::hostageHitInteractionRect() {
	hostageDoesHitInteractionRect = true;
}



void Player::commonLegIdlingEventTransition(PlayerEvent &event, PlayerPhysicState physicState) {
	bool isPressingMove = event.moveLeft || event.moveRight;
	if (!isPressingMove && 
		physicState != PlayerPhysicState::JUMPUP && 
		physicState != PlayerPhysicState::JUMPDOWN && 
		physicState != PlayerPhysicState::FALL) {

		toLegIdlingAnimation();
	}
}

void Player::commonLegJumpFallEventTransition(PlayerPhysicState physicState) {
	if (physicState == PlayerPhysicState::JUMPUP) {
		toLegJumpingAnimation();
	}
	else if (physicState == PlayerPhysicState::FALL) {
		toLegFallingAnimation();
	}
}

void Player::commonBodyJumpFallEventTransition(PlayerPhysicState physicState) {
	if (physicState == PlayerPhysicState::JUMPUP) {
		toBodyJumpingAnimation();
	}
	else if (physicState == PlayerPhysicState::FALL) {
		toBodyFallingAnimation();
	}

}

void Player::commonLegDieEventTransition(bool die) {
	if (die) {
		toLegDyingAnimation();
	}
}

void Player::commonBodyDieEventTransition(bool die) {
	if (die) {
		toBodyDyingAnimation();
	}
}

void Player::commonThrowingBombEventTransition(PlayerEvent &event) {
	if (event.throwGrenade) {
		throwGrenade();
		toThrowingAnimation();
	}
}

void Player::commonShootingEventTransition(PlayerEvent &event) {
	if (!rebelDoesHitInteractionRect && !hostageDoesHitInteractionRect) {
		bool verticaleventIsDown = event.up || event.down;
		if (!verticaleventIsDown && event.shoot) {
			toHorizontalShootingAnimation();
			shootBullet();
		}

		if (verticaleventIsDown) {
			toUpShootingAnimation();
			shootBullet();
		}
	}
}

void Player::commonBodyWalkingEventTransition(PlayerEvent &event) {
	if (event.moveLeft || event.moveRight) {
		bodyAnimationState = BodyAnimationState::WALKING;
		currentBodyAnimation = walkingAnimation;
	}
}

void Player::commonWalkingLegEventTransition(PlayerEvent &event) {
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


void Player::commonSlashingEventTransition() {
	if (hostageDoesHitInteractionRect) {
		toBodySlashingAnimation();
	}
}

void Player::commonStabbingEventTransition() {
	if (rebelDoesHitInteractionRect) {
		toBodyStabbingAnimation();
	}
}

void Player::toBodySlashingAnimation() {
	bodyAnimationState = BodyAnimationState::SLASH;
	currentBodyAnimation = slashingAnimation;
}

void Player::toBodyStabbingAnimation() {
	bodyAnimationState = BodyAnimationState::STAB;
	currentBodyAnimation = stabbingAnimation;
}

void Player::toBodyIdlingAnimation() {
	bodyAnimationState = BodyAnimationState::IDLING;
	currentBodyAnimation = idlingAnimation;
}

void Player::toLegIdlingAnimation() {
	legAnimationState = LegAnimationState::IDLING;
	currentLegAnimation = idlingLegAnimation;
}

void Player::toBodyFallingAnimation() {
	bodyAnimationState = BodyAnimationState::FALLING;
	currentBodyAnimation = fallingAnimation;
	currentLegAnimation = fallingLegAnimation;
}

void Player::toLegFallingAnimation() {
	legAnimationState = LegAnimationState::FALLING;
	currentLegAnimation = fallingLegAnimation;
}

void Player::toBodyDyingAnimation() {
	bodyAnimationState = BodyAnimationState::DYING;
	currentBodyAnimation = dieAnimation;
}

void Player::toLegDyingAnimation() {
	legAnimationState = LegAnimationState::DYING;
	currentLegAnimation = NULL;
}

void Player::toBodyJumpingAnimation() {
	bodyAnimationState = BodyAnimationState::JUMPING;
	currentBodyAnimation = jumpingAnimation;
}

void Player::toLegJumpingAnimation() {
	legAnimationState = LegAnimationState::JUMPING;
	currentLegAnimation = jumpingLegAnimation;
}

void Player::toThrowingAnimation() {
	bodyAnimationState = BodyAnimationState::THROWING;
	currentBodyAnimation = throwingAnimation;
}

void Player::toHorizontalShootingAnimation() {
	bodyAnimationState = BodyAnimationState::HORIZONTAL_SHOOTING;
	currentBodyAnimation = horizontalShootingAnimation;
}

void Player::toUpShootingAnimation() {
	bodyAnimationState = BodyAnimationState::UP_SHOOTING;
	currentBodyAnimation = upShootingAnimation;
}

}