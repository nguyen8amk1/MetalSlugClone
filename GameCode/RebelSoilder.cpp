#include "RebelSoilder.h"

namespace MetalSlug {

RebelSoilder::RebelSoilder(float gravity, float moveSpeed, Rect colliderRect, PlatformSpecficMethodsCollection *platformMethods, AnimationState animationState) {
	this->gravity = gravity;
	this->moveSpeed = moveSpeed;
	this->platformMethods = platformMethods;
	this->colliderRect = colliderRect;

	physicStateMachine = new BasicPhysicStateMachine(gravity);
	interactionRect = interactionRectDisabledRect;
	globalGameData = GlobalGameData::getInstance();
	grenadeFactory = new GrenadeFactory(platformMethods);

	PlatformSpecificImage *spriteSheet = globalGameData->getSpriteSheet("REBEL_SOILDER");
	Util::AnimationUtil::initAnimationMetaData(idleAnimationMetaData, spriteSheet, .1f, 1, 4, {0, 4}, {108/4, 38});
	idleAnimation = new Animation(idleAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(slashingAnimationMetaData, spriteSheet, .1f, 1, 15, {0, 42}, {735/15, 37});
	slashingAnimation = new Animation(slashingAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(throwingBombAnimationMetaData, spriteSheet, .1f, 1, 17, {0, 79}, {46, 42});
	throwingBombAnimation = new Animation(throwingBombAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(slashedDieAnimationMetaData, spriteSheet, .1f, 1, 12, {0, 409}, {53, 49});
	slashedDieAnimation = new Animation(slashedDieAnimationMetaData, platformMethods);

	currentAnimationState = animationState;

	if (currentAnimationState == AnimationState::IDLING) {
		currentAnimation = idleAnimation;
	}
	else if (currentAnimationState == AnimationState::SLASHING) {
		currentAnimation = slashingAnimation;
	}
	else if (currentAnimationState == AnimationState::THROWING_BOMB) {
		currentAnimation = throwingBombAnimation;
	}
}

void RebelSoilder::update(Camera *camera, double dt) {
	physicResult = physicStateMachine->update(dt, colliderRect, globalGameData->getGroundColliders());
	colliderRect = physicResult.colliderRect;

	// Physics checking right here 
	touchPlayer = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, globalGameData->getPlayer()->getRect());
	playerInThrowingRange = false;

	// Animation state machine 
	switch (currentAnimationState) {
	case AnimationState::IDLING: {

		// transition
		slashingEventTransition();
		throwingBombEventTransition();
		slashedDieEventTransition();
		bulletDieEventTransition();
	} break;

	case AnimationState::SLASHING: {
		timeAccumulator += dt;
		if (timeAccumulator >= .7f) {
			interactionRect.x = colliderRect.x - colliderRect.width/2;
			interactionRect.y = colliderRect.y;
			interactionRect.width = colliderRect.width/2;
			interactionRect.height = colliderRect.height/2;
			timeAccumulator = 0;
		}
		else {
			interactionRect = interactionRectDisabledRect;
		}
		// transition: to idling or throwing bomb 
		// event: ...
		slashedDieEventTransition();
		bulletDieEventTransition();
	} break;

	case AnimationState::THROWING_BOMB: {
		// action:...
		bool finish1Cycle = currentAnimation->finishOneCycle();
		if (finish1Cycle) {
			throwGrenade();
		}
		// transition: to idling or slashing  
		slashingEventTransition();
		slashedDieEventTransition();
		bulletDieEventTransition();
	} break;

	case AnimationState::SLASHED_DIE: {
		// TODO: 
		// action: when die animation finish reseting the state of the object and go back into the object pool 
		// event: ...
		bool finish1Cycle = currentAnimation->finishOneCycle();
		if (finish1Cycle) {
			currentAnimation = NULL;
			currentAnimationState = AnimationState::NONE;
		}
	} break;
	}

	// NOTE: The common event check outside (hit bullet): 
	// Rendering 
	if (currentAnimation) {
		currentAnimation->changePos(colliderRect.x, colliderRect.y);
		currentAnimation->animate(camera, dt);

		Color testCol = { 255, 255, 0 };
		Rect r = colliderRect;
		Vec2f t = camera->convertWorldPosToScreenPos({ r.x, r.y });
		r.x = t.x;
		r.y = t.y;
		platformMethods->drawRectangle(r, testCol);
	}

	hitByBullet = false;
}

void RebelSoilder::toThrowingBombAnimation() {
	currentAnimationState = AnimationState::THROWING_BOMB;
	currentAnimation = throwingBombAnimation;
}

void RebelSoilder::toSlashingAnimation() {
	currentAnimationState = AnimationState::SLASHING;
	currentAnimation = slashingAnimation;
}

void RebelSoilder::toDieAnimation() {
	//OutputDebugStringA("ENEMY DIEEEEEEE\n");
	currentAnimationState = AnimationState::SLASHED_DIE;
	currentAnimation = slashedDieAnimation;
}

void RebelSoilder::slashingEventTransition() {
	if (touchPlayer) {
		toSlashingAnimation();
	}
}

void RebelSoilder::throwGrenade() {
	Grenade* grenade = grenadeFactory->createRebelSoilderGrenade();;
	grenade->startThrow(horizontalFacingDirection, colliderRect.x, colliderRect.y);
	globalGameData->getGrenades()->push_back(grenade);
}

void RebelSoilder::throwingBombEventTransition() {
	if (!touchPlayer && playerInThrowingRange) {
		toThrowingBombAnimation();
	}
}

void RebelSoilder::bulletDieEventTransition() {
	if (hitByBullet) {
		toDieAnimation();
	}
}

void RebelSoilder::slashedDieEventTransition() {
	// TODO: 
}

}