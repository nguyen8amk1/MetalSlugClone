#pragma once 
#include "MetalSlug.h"
#include "Animation.h"
#include "CollisionChecker.h"
#include "PlayerPhysic.h"
#include "PlayerAnimation.h"
#include "../Util.cpp"

//#include "Windows.h"
#include "Grenade.h"

namespace MetalSlug {

class Player {
private: 
	Rect colliderRect = { -17.8f, 1.0f, .2f, .4f };

	float moveSpeed, gravity;

	bool die = false;

	Rect interactionRectDisabledRect = {0, -5.0f, 0, 0};
	Rect interactionRect;

	float horizontalFacingDirection = 1;

	std::vector<Grenade*> grenades;
	Rect grenadeRect = { 0, 0, .1f, .1f };

	PlatformSpecficMethodsCollection* platformMethods;
	PlayerPhysic* physic;
	PlayerPhysicResult physicResult;
	PlayerPhysicState physicState = PlayerPhysicState::ONGROUND;

	PlayerAnimationResult animationResult;
	PlayerAnimation *animation;

public: 
	Player(float gravity, float moveSpeed, PlatformSpecficMethodsCollection *platformMethods) {
		this->moveSpeed = moveSpeed;
		this->gravity = gravity;
		this->platformMethods = platformMethods;

		physic = new PlayerPhysic(gravity);
		animation = new PlayerAnimation(platformMethods);
	}

	~Player() {
	}

	void update(const GameInputContext &input, LevelData &levelData, Camera *camera, double dt) {
		if (!die) {
			if (input.left.isDown) {
				colliderRect.x -= (float)(moveSpeed*dt); 
			}
			else if (input.right.isDown) {
				colliderRect.x += (float)(moveSpeed*dt); 
			}

			if (input.shoot.isDown) {

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

private: 

	void throwGrenade() {
		//OutputDebugStringA("A GRENADE IS CREATED \n");
		Grenade *grenade = new Grenade(grenadeRect, platformMethods);
		grenade->startThrow(horizontalFacingDirection, colliderRect.x, colliderRect.y);
		grenades.push_back(grenade);
	}


public:
	void moveXBy(float d) {
		colliderRect.x += d;
	}

	void moveYBy(float d) {
		colliderRect.y += d;
	}

	Rect getRect() { return colliderRect; }

	Rect getInteractionRect() { return interactionRect; }
};

}