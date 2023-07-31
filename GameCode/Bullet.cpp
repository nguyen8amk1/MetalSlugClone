#include "Bullet.h"

namespace MetalSlug {

Bullet::Bullet(PlatformSpecficMethodsCollection *platformMethods) {
	globalGameData = GlobalGameData::getInstance();

	Util::AnimationUtil::initAnimationMetaData(animMetaData, globalGameData->getSpriteSheet("MARCO_ROSSI"), .1f, 1, 1, {74, 476}, {12, 11});
	anim = new Animation(animMetaData, platformMethods);
}

void Bullet::update(double dt, Camera *camera) {
	//  state machine 
	switch (currentState) {
	case State::FLY: {
		timeAccumulator += dt;
		t = Util::Math::normalizef(timeAccumulator, timeToGoFromAToB);
		float xd = Util::Math::lerp(0, howFarTheGoes, t);
		colliderRect.x = originalX + xd;

		anim->changePos(colliderRect.x, colliderRect.y);
		anim->changeSize(colliderRect.width, colliderRect.height);
		anim->animate(camera, dt);
	} break;

	case State::HIT: {
		// TODO: hit means do nothing :v 
	} break;
	}
}

void Bullet::reset() {
	currentState = State::HIT;
}

void Bullet::reset(float playerX, float playerY) {
	colliderRect.x = playerX;
	colliderRect.y = playerY;

	originalX = colliderRect.x;
	originalY = colliderRect.y;

	timeAccumulator = 0;
	t = 0;
}

void Bullet::shoot(float playerX, float playerY) {
	currentState = State::FLY;
	reset(playerX, playerY);
}

}