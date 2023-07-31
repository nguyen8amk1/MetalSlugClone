#pragma once 
#include "Animation.h"
#include "MetalSlug.h"
#include "GlobalGameData.h"
#include "../Util.h"

namespace MetalSlug {
class GlobalGameData;
enum class PlayerPhysicState;

class Bullet {
private: 
	enum class State {
		FLY,
		HIT
	};

	State currentState = State::HIT;
	AnimationMetaData animMetaData;
	Animation* anim;
	Rect colliderRect = {0, 0, .05f, .05f};
	float t = 0;
	float originalX, originalY;
	float howFarTheGoes = .5f;
	float timeAccumulator = 0;
	float timeToGoFromAToB = .5f;
	bool hitEnemy = false;

	GlobalGameData* globalGameData;

public: 
	Bullet(PlatformSpecficMethodsCollection* platformMethods);

	void update(double dt, Camera* camera);

	void reset();
	void reset(float playerX, float playerY);

	void shoot(float playerX, float playerY);

	Rect getColliderRect() {
		return colliderRect;
	}
};


}
