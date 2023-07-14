#pragma once 
#include "Animation.h"
#include "MetalSlug.h"
#include "../Util.cpp"
//#include "Windows.h"

namespace MetalSlug {

class Grenade {
private: 
	Rect colliderRect = { 0, 0, .1f, .1f };

	enum class PhysicState {
		NONE, 
		FIRST_HOP_UP, 
		FIRST_HOP_DOWN, 
		SECOND_HOP_UP, 
		SECOND_HOP_DOWN, 
		ON_GROUND
	};

	enum class AnimationState {
		NONE,
		FIRST_HOP, 
		SECOND_HOP, 
		EXPLODE
	};

	PhysicState currentPhysicState = PhysicState::NONE;
	AnimationState currentAnimationState = AnimationState::NONE;

	AnimationMetaData spinningAnimationMetaData;
	Animation* spinningAnimation;

	Animation* currentAnimation;

	int direction = 1;  

	float firstHopHeight = .251851f;
	float secondHopHeight = .15f;

	float xTimeAccumulator = 0.0f;
	float yTimeAccumulator = 0.0f;

	float howFarFirstHop = 0.4806f;
	float howFarSecondHop = 0.31097f;

	float firstHopDuration = 0.6f;
	float secondHopDuration = 0.6f;


	float xt = 0.0f;
	float yt = 0.0f;
	Point originalPos;

public:

	Grenade(const Rect colliderRect, PlatformSpecficMethodsCollection *platformMethods) {
		this->colliderRect = colliderRect;

		std::string filename = "Assets/Imgs/Characters/marco_messi.png";
		Util::AnimationUtil::initAnimationMetaData(spinningAnimationMetaData, filename, .1f, 1, 1, {0, 428}, {11, 18}); //0, 428, 11, 18
		spinningAnimation = new Animation(spinningAnimationMetaData, platformMethods);
	}

	void startThrow(int facingDirection, float playerX, float playerY) {
		currentPhysicState = PhysicState::FIRST_HOP_UP;
		currentAnimationState = AnimationState::FIRST_HOP;
		currentAnimation = spinningAnimation;
		originalPos.x = playerX;
		originalPos.y = playerY;
		direction = facingDirection;
	}

	void update(Camera *camera, double dt, LevelData &levelData) {
		//  State Machine  
		switch (currentPhysicState) {
		case PhysicState::FIRST_HOP_UP: {
			xTimeAccumulator += dt;
			yTimeAccumulator += dt;

			/*
			xTimeAccumulator = Util::Math::clampf(xTimeAccumulator, 0, firstHopDuration/2.0f);
			yTimeAccumulator = Util::Math::clampf(yTimeAccumulator, 0, firstHopDuration/2.0f);
			*/

			xt = Util::Math::normalizef(xTimeAccumulator, firstHopDuration/2.0f);
			yt = Util::Math::normalizef(yTimeAccumulator, firstHopDuration/2.0f);

			//OutputDebugStringA(Util::MessageFormater::print(colliderRect.x, ", ", colliderRect.y, '\n').c_str());

			// event: 
			if (yt >= 1.0f) {
				yt -= 1.0f;
				xt = 0;
				currentPhysicState = PhysicState::FIRST_HOP_DOWN;
				originalPos.x = colliderRect.x;

				xTimeAccumulator = 0;
				yTimeAccumulator = 0;
			}
			else {
				float xd = Util::Math::lerp(0, howFarFirstHop / 2.0f, xt);
				float yd = Util::Math::lerp(0, firstHopHeight, Util::Math::upCurve(yt));

				if (direction == -1) {
					xd = -xd;
				}

				colliderRect.x = originalPos.x + xd;
				colliderRect.y = originalPos.y + yd;
			}
		} break;

		case PhysicState::FIRST_HOP_DOWN: {
			xTimeAccumulator += dt;
			yTimeAccumulator += dt;

			/*
			xTimeAccumulator = Util::Math::clampf(xTimeAccumulator, 0, firstHopDuration/2.0f);
			yTimeAccumulator = Util::Math::clampf(yTimeAccumulator, 0, firstHopDuration/2.0f);
			*/

			xt = Util::Math::normalizef(xTimeAccumulator, firstHopDuration/2.0f);
			yt = Util::Math::normalizef(yTimeAccumulator, firstHopDuration/2.0f);

			bool hitGround = false;
			for (RectangleCollider *rect: levelData.groundColliders) {
				hitGround = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, rect->getRect());
				if (hitGround) break;
			}

			if (hitGround) {
				yt = 0;
				xt = 0;
				currentPhysicState = PhysicState::SECOND_HOP_UP;
				originalPos.x = colliderRect.x;
				originalPos.y = colliderRect.y;

				xTimeAccumulator = 0;
				yTimeAccumulator = 0;

			}
			else {
				float xd = Util::Math::lerp(0, howFarFirstHop / 2.0f, xt);
				float yd = Util::Math::lerp(0, firstHopHeight, Util::Math::downCurve(yt));

				if (direction == -1) {
					xd = -xd;
				}

				colliderRect.x = originalPos.x + xd;
				colliderRect.y = originalPos.y + yd;
			}

		} break;

		case PhysicState::SECOND_HOP_UP: {
			// action: 
			xTimeAccumulator += dt;
			yTimeAccumulator += dt;

			/*
			xTimeAccumulator = Util::Math::clampf(xTimeAccumulator, 0, secondHopDuration/2.0f);
			yTimeAccumulator = Util::Math::clampf(yTimeAccumulator, 0, secondHopDuration/2.0f);
			*/

			xt = Util::Math::normalizef(xTimeAccumulator, secondHopDuration/2.0f);
			yt = Util::Math::normalizef(yTimeAccumulator, secondHopDuration/2.0f);

			// event: 
			if (yt >= 1.0f) {
				yt -= 1.0f;
				xt = 0;
				currentPhysicState = PhysicState::SECOND_HOP_DOWN;
				originalPos.x = colliderRect.x;

				xTimeAccumulator = 0;
				yTimeAccumulator = 0;
			}
			else {
				float xd = Util::Math::lerp(0, howFarSecondHop / 2.0f, xt);
				float yd = Util::Math::lerp(0, secondHopHeight, Util::Math::upCurve(yt));

				if (direction == -1) {
					xd = -xd;
				}

				colliderRect.x = originalPos.x + xd;
				colliderRect.y = originalPos.y + yd;
			}
		} break;

		case PhysicState::SECOND_HOP_DOWN: {
			xTimeAccumulator += dt;
			yTimeAccumulator += dt;

			/*
			xTimeAccumulator = Util::Math::clampf(xTimeAccumulator, 0, secondHopDuration/2.0f);
			yTimeAccumulator = Util::Math::clampf(yTimeAccumulator, 0, secondHopDuration/2.0f);
			*/

			xt = Util::Math::normalizef(xTimeAccumulator, secondHopDuration/2.0f);
			yt = Util::Math::normalizef(yTimeAccumulator, secondHopDuration/2.0f);

			bool hitGround = false;
			for (RectangleCollider *rect: levelData.groundColliders) {
				hitGround = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, rect->getRect());
				if (hitGround) break;
			}

			if (hitGround) {
				yt = 0;
				xt = 0;
				currentPhysicState = PhysicState::NONE;

				originalPos.x = colliderRect.x;
				originalPos.y = colliderRect.y;

				xTimeAccumulator = 0;
				yTimeAccumulator = 0;
			}
			else {
				float xd = Util::Math::lerp(0, howFarSecondHop / 2.0f, xt);
				float yd = Util::Math::lerp(0, secondHopHeight, Util::Math::downCurve(yt));

				if (direction == -1) {
					xd = -xd;
				}

				colliderRect.x = originalPos.x + xd;
				colliderRect.y = originalPos.y + yd;
			}

		} break;

		case PhysicState::ON_GROUND: {
			// TODO: 
			// action: 
			// event: 

		} break;
		}
		
		if (currentAnimation) {
			currentAnimation->changePos(colliderRect.x, colliderRect.y);
			currentAnimation->changeSize(colliderRect.width, colliderRect.height);
			currentAnimation->animate(camera, dt);
		}

	}
};


}
