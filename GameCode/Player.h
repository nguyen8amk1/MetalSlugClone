#pragma once 
#include "MetalSlug.h"
#include "Animation.h"
#include "CollisionChecker.h"
#include "PlayerPhysic.h"
#include "PlayerAnimation.h"

//#include "Windows.h"
#include "Grenade.h"
#include "Bullet.h"
#include "Camera.h"

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

	std::vector<Bullet*> bullets;

public: 
	Player(float gravity, float moveSpeed, PlatformSpecficMethodsCollection* platformMethods);
	~Player();

	void update(const GameInputContext& input, LevelData& levelData, Camera* camera, double dt);

private: 
	void interactionRectInit();

	void shootBullet();

	void throwGrenade();


public:
	void moveXBy(float d) {
		colliderRect.x += d;
	}

	void moveYBy(float d) {
		colliderRect.y += d;
	}

	// FIXME: this is too slow 
	// FIXME: this is too slow 
	// FIXME: this is too slow 
	// FIXME: this is too slow 
	// FIXME: this is too slow 
	// FIXME: this is too slow 
	std::vector<Rect> getBulletRects();

	Rect getRect() { return colliderRect; }

	Rect getInteractionRect() { return interactionRect; }
};

}
