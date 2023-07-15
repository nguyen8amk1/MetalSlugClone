#include "RebelSoilder.h"

namespace MetalSlug {

RebelSoilder::RebelSoilder(float gravity, float moveSpeed, Rect colliderRect, PlatformSpecficMethodsCollection *platformMethods) {
	// TODO: 
	this->gravity = gravity;
	this->moveSpeed = moveSpeed;
	this->platformMethods = platformMethods;
	this->colliderRect = colliderRect;

	physicStateMachine = new BasicPhysicStateMachine(gravity);

	interactionRect = interactionRectDisabledRect;

	std::string filename = "Assets/Imgs/Characters/rebel_soilder.png";

	Util::AnimationUtil::initAnimationMetaData(idleAnimationMetaData, filename, .1f, 1, 4, {0, 4}, {108/4, 38});
	idleAnimation = new Animation(idleAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(slashingAnimationMetaData, filename, .1f, 1, 15, {0, 42}, {735/15, 37});
	slashingAnimation = new Animation(slashingAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(throwingBombAnimationMetaData, filename, .1f, 1, 17, {0, 79}, {782/17, 42});
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

		dieEventTransition(levelData.bulletRects);
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
		dieEventTransition(levelData.bulletRects);
	} break;

	case AnimationState::THROWING_BOMB: {
		// TODO: 
		// action:...
		// transition: to idling or slashing  
		// event: ...
		dieEventTransition(levelData.bulletRects);
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
}