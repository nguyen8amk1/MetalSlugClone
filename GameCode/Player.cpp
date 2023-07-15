#pragma once 
#include "Player.h"


namespace MetalSlug { 

Player::Player(float gravity, float moveSpeed, PlatformSpecficMethodsCollection *platformMethods) {
	this->moveSpeed = moveSpeed;
	this->gravity = gravity;
	this->platformMethods = platformMethods;
	globalGameData = GlobalGameData::getInstance();

	physic = new PlayerPhysic(gravity);
	animation = new PlayerAnimation(platformMethods);
}

Player::~Player() {
}


void Player::update(const GameInputContext &input, LevelData &levelData, Camera *camera, double dt) {
	if (!die) {
		if (input.left.isDown) {
			colliderRect.x -= (float)(moveSpeed*dt); 
		}
		else if (input.right.isDown) {
			colliderRect.x += (float)(moveSpeed*dt); 
		}

		if (input.shoot.isDown) {
			interactionRectInit();
			shootBullet();
		}
		else {
			interactionRect = interactionRectDisabledRect;
		}
	}


	if (levelData.levelStarted) {
		physicResult = physic->update(input, dt, colliderRect, die, levelData);
		colliderRect = physicResult.colliderRect;
		physicState = physicResult.physicState;
		die = physicResult.die;
	}

	animationResult = animation->update(input, dt, camera, levelData, physicState, colliderRect, horizontalFacingDirection, die);
	colliderRect = animationResult.colliderRect;
	horizontalFacingDirection = animationResult.horizontalFacingDirection;
	die = animationResult.die;

	if (animation->isThrowGrenade()) {
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

}