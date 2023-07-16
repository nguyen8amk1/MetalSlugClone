#include "RebelSoilder.h"

namespace MetalSlug {

RebelSoilder::RebelSoilder(float gravity, float moveSpeed, Rect colliderRect, PlatformSpecficMethodsCollection *platformMethods) {
	this->gravity = gravity;
	this->moveSpeed = moveSpeed;
	this->platformMethods = platformMethods;
	this->colliderRect = colliderRect;

	physicStateMachine = new BasicPhysicStateMachine(gravity);
	interactionRect = interactionRectDisabledRect;
	globalGameData = GlobalGameData::getInstance();

	PlatformSpecificImage *spriteSheet = globalGameData->getSpriteSheet("REBEL_SOILDER");
	Util::AnimationUtil::initAnimationMetaData(idleAnimationMetaData, spriteSheet, .1f, 1, 4, {0, 4}, {108/4, 38});
	idleAnimation = new Animation(idleAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(slashingAnimationMetaData, spriteSheet, .1f, 1, 15, {0, 42}, {735/15, 37});
	slashingAnimation = new Animation(slashingAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(throwingBombAnimationMetaData, spriteSheet, .1f, 1, 17, {0, 79}, {782/17, 42});
	throwingBombAnimation = new Animation(throwingBombAnimationMetaData, platformMethods);

	currentAnimation = idleAnimation;

	globalGameData = GlobalGameData::getInstance();
}

void RebelSoilder::update(LevelData &levelData, Camera *camera, double dt) {
	physicResult = physicStateMachine->update(dt, colliderRect, levelData);
	colliderRect = physicResult.colliderRect;

	// Animation state machine 
	switch (currentAnimationState) {
	case AnimationState::IDLING: {
		bool touchPlayer = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, globalGameData->getPlayer()->getRect());
		bool playerInThrowingRange = false;
		if (touchPlayer) {
			toSlashingAnimation();
		}
		else if (playerInThrowingRange) {
			toThrowingBombAnimation();
		}

		dieEventTransition();
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
		dieEventTransition();
	} break;

	case AnimationState::THROWING_BOMB: {
		// TODO: 
		// action:...
		// transition: to idling or slashing  
		// event: ...
		dieEventTransition();
	} break;

	case AnimationState::DIE: {
		// TODO: 
		// action:...
		// transition: to idling or slashing  
		// event: ...
	} break;
	}

	// NOTE: The common event check outside (hit bullet): 
	// Rendering 
	currentAnimation->changePos(colliderRect.x, colliderRect.y);
	currentAnimation->animate(camera, dt);
	Color testCol = { 255, 255, 0 };

	Rect r = colliderRect;
	Vec2f t = camera->convertWorldPosToScreenPos({ r.x, r.y });
	r.x = t.x;
	r.y = t.y;
	platformMethods->drawRectangle(r, testCol);

}

void RebelSoilder::dieEventTransition() {
	for (int i = 0; i < globalGameData->getBullets()->size(); i++) {
		Bullet* bullet = (*globalGameData->getBullets())[i];
		bool bulletHit = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, bullet->getColliderRect());
		if (bulletHit) {
			globalGameData->removeBulletAt(i);
			OutputDebugStringA(Util::MessageFormater::print("TODO: Remove bullet at index: ", i, '\n').c_str());
			toDieAnimation();
			break;
		}
	}
}

}