#pragma once 
#include "Player.h"


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


void Player::update(const GameInputContext &input, LevelData &levelData, Camera *camera, double dt) {

	PlayerEvent event;
	event.jump = input.jump.isDown;
	event.moveLeft = input.left.isDown;
	event.moveRight = input.right.isDown;
	event.up = input.up.isDown;
	event.down = input.down.isDown;
	event.throwGrenade = input.throwGrenade.isPressed;
	event.shoot = input.shoot.isDown;

	if (levelData.levelStarted) {
		if (!die) {
			if (event.shoot) {
				interactionRectInit();
				shootBullet();
			}
			else {
				interactionRect = interactionRectDisabledRect;
			}
		}

		updatePhysics(event, levelData, dt);
	}

	updateAnimation(event, dt, camera, levelData);

	if (grenadeIsThrow) {
		throwGrenade();
	}

	for (Grenade *grenade: grenades) {
		grenade->update(camera, dt, levelData);
	}
	
	for (Bullet* bullet : *(globalGameData->getBullets())) {
		bullet->update(dt, levelData, camera);
	}

	/*
	bool collided = false;
	for (RectangleCollider *ground: levelData.groundColliders) {
		collided = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, ground->getRect());
		if (collided) break;
	}
	*/

	/*
	if (collided) {
		groundColor = collidedColor;
		playerColor = collidedColor;
	}
	else {
		groundColor = {255, 255, 0, 255};
		playerColor = {0, 0, 255, 255};
	}
	*/
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
	Bullet *bullet = new Bullet(platformMethods);
	bullet->shoot(colliderRect.x, colliderRect.y);

	globalGameData->getBullets()->push_back(bullet);
}

void Player::throwGrenade() {
	Grenade *grenade = new Grenade(grenadeRect, platformMethods);
	grenade->startThrow(horizontalFacingDirection, colliderRect.x, colliderRect.y);
	grenades.push_back(grenade);
}

// Physics 
void Player::updatePhysics(PlayerEvent &event, LevelData &levelData, double dt) {
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

		commonOnGroundEventTransition(levelData);
		commonDieEventTransition(levelData);
	}
	else if (physicState == PlayerPhysicState::ONGROUND) {
		bool collided = false;
		for (RectangleCollider* ground : levelData.groundColliders) {
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

		commonDieEventTransition(levelData);
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
		commonDieEventTransition(levelData);
	}
	else if (physicState == PlayerPhysicState::JUMPDOWN) {
		jumpTimeAccumulator += dt;
		Util::Math::clampf(jumpTimeAccumulator, 0, jumpTotalTime/2.0f);
		jumpT = Util::Math::normalizef(jumpTimeAccumulator, jumpTotalTime/2.0f);
		float yd = Util::Math::downCurve(jumpT) * jumpHeight;
		colliderRect.y = originalGroundY + yd;

		commonOnGroundEventTransition(levelData);
		commonDieEventTransition(levelData);
	}
}

void Player::commonOnGroundEventTransition(LevelData &levelData) {
	CollisionInfo colInfo;
	for (RectangleCollider* ground : levelData.groundColliders) {
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

void Player::commonDieEventTransition(LevelData &levelData) {
	bool hitDangerRect = false;
	for (Rect dangerRect: levelData.dangerRects) {
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

	currentBodyAnimation = idlingAnimation;
	currentLegAnimation = idlingLegAnimation;
}

void Player::updateAnimation(PlayerEvent &event, double dt, Camera *camera, LevelData &levelData) {
	bodyAnimationStateMachineUpdate(event, dt, camera, levelData);
	legAnimationStateMachineUpdate(event, dt, camera, levelData);
	
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


void Player::legAnimationStateMachineUpdate(PlayerEvent &event, double dt, Camera *camera, LevelData &levelData) {
	switch (legAnimationState) {
	case LegAnimationState::IDLING: {
		commonWalkingLegEventTransition(event);
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

void Player::bodyAnimationStateMachineUpdate (PlayerEvent &event, double dt, Camera *camera, LevelData &levelData) {
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
		grenadeIsThrow = true;
		toThrowingAnimation();
	}
	else {
		grenadeIsThrow = false;
	}
}

void Player::commonShootingEventTransition(PlayerEvent &event) {
	bool verticaleventIsDown = event.up && event.down;
	if (!verticaleventIsDown && event.shoot) {
		toHorizontalShootingAnimation();
	}

	if (event.up && event.shoot) {
		toUpShootingAnimation();

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