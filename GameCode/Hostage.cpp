#include "Hostage.h"

namespace MetalSlug {

Hostage::Hostage(float gravity, float moveSpeed, Rect hostageColliderRect, PlatformSpecficMethodsCollection *platformMethods) {
	this->gravity = gravity;
	this->moveSpeed = moveSpeed;
	this->platformMethods = platformMethods;
	this->hostageColliderRect = hostageColliderRect;

	globalGameData = GlobalGameData::getInstance();

	basicPhysicStateMachine = new BasicPhysicStateMachine(gravity);

	PlatformSpecificImage *spriteSheet = globalGameData->getSpriteSheet("HOSTAGE");
	Util::AnimationUtil::initAnimationMetaData(hostageTiedAnimationMetaData, spriteSheet, .1f, 1, 1, {6, 26}, {30, 27});
	hostageTiedAnimation = new Animation(hostageTiedAnimationMetaData, platformMethods);

	Util::AnimationUtil::initAnimationMetaData(hostageUntiedAnimationMetaData, spriteSheet, .1f, 1, 1, {6, 248}, {33, 38});
	hostageUntiedAnimation  = new Animation(hostageUntiedAnimationMetaData, platformMethods);

	hostageCurrentAnimation = hostageTiedAnimation;
}

void Hostage::update(Camera *camera, double dt) {
	basicPhysicResult = basicPhysicStateMachine->update(dt, hostageColliderRect, globalGameData->getGroundColliders());
	hostageColliderRect = basicPhysicResult.colliderRect;

	// Animation state machine 
	switch (hostageCurrentAnimationState) {
	case HostageAnimationState::TIED: {
		bool playerUntied = CollisionChecker::doesRectangleVsRectangleCollide(hostageColliderRect, globalGameData->getPlayer()->getInteractionRect());
		if (playerUntied) {
			hostageCurrentAnimationState = HostageAnimationState::UNTIED;
			hostageCurrentAnimation = hostageUntiedAnimation;
		}
		break;
	}
	case HostageAnimationState::UNTIED: {
		hostageColliderRect.x -= moveSpeed *.5f*dt;

		bool outOfScreen = hostageColliderRect.x < -1.45f;
		if (outOfScreen) {
			// TODO: handle hostage out of screen -> put them back into the pool 
		}
		break;
	}
	}

	// Rendering 
	hostageCurrentAnimation->changePos(hostageColliderRect.x, hostageColliderRect.y);
	hostageCurrentAnimation->animate(camera, dt);
	Color testCol = {255, 255, 0};
	
	Rect r = hostageColliderRect;
	Vec2f t = camera->convertWorldPosToScreenPos({r.x, r.y});
	r.x = t.x;
	r.y = t.y;
	platformMethods->drawRectangle(r, testCol);
}

void Hostage::moveXBy(float d) {
	hostageColliderRect.x += d;
}

void Hostage::moveYBy(float d) {
	hostageColliderRect.y += d;
}

}
