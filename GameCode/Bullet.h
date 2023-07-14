#pragma once 
#include "Animation.h"
#include "MetalSlug.h"
#include "../Util.cpp"

namespace MetalSlug {

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

public: 
	Bullet(PlatformSpecficMethodsCollection *platformMethods) {
		Util::AnimationUtil::initAnimationMetaData(animMetaData, "Assets/Imgs/Characters/marco_messi.png", .1f, 1, 1, {74, 476}, {12, 11});
		anim = new Animation(animMetaData, platformMethods);
	}

	void update(double dt, LevelData &levelData, Camera *camera) {
		//  state machine 
		switch (currentState) {
		case State::FLY: {
			timeAccumulator += dt;
			t = Util::Math::normalizef(timeAccumulator, timeToGoFromAToB);
			float xd = Util::Math::lerp(0, howFarTheGoes, t);
			colliderRect.x = originalX + xd;
			//OutputDebugStringA(Util::MessageFormater::print(" x: ", colliderRect.x, '\n').c_str());

			/*
			if (hitEnemy) {
				currentState = State::HIT;
				reset(colliderRect.x, colliderRect.y);
			}
			*/
		} break;

		case State::HIT: {
			// TODO: hit means do nothing :v 
		} break;
		}

		anim->changePos(colliderRect.x, colliderRect.y);
		anim->changeSize(colliderRect.width, colliderRect.height);
		anim->animate(camera, dt);
	}

	void reset(float playerX, float playerY) {
		colliderRect.x = playerX;
		colliderRect.y = playerY;

		originalX = colliderRect.x;
		originalY = colliderRect.y;

		timeAccumulator = 0;
		t = 0;
	}

	void shoot(float playerX, float playerY) {
		currentState = State::FLY;
		reset(playerX, playerY);
	}

	Rect getColliderRect() {
		return colliderRect;
	}
};


}
