#pragma once 
#include "Animation.h"
#include "MetalSlug.h"
#include "../Util.cpp"
#include "TimeBoundedLerp.h"
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
	float firstHopDuration = 0.6f;
	float howFarFirstHop = 0.4806f;

	float secondHopHeight = .15f;
	float secondHopDuration = 0.6f;
	float howFarSecondHop = 0.31097f;

	TimeBoundedLerp *xLerp;
	TimeBoundedLerp *yLerp;


	Point originalPos;

public:

	Grenade(const Rect colliderRect, PlatformSpecficMethodsCollection *platformMethods) {
		this->colliderRect = colliderRect;
		xLerp = new TimeBoundedLerp(firstHopDuration/2.0f);
		yLerp = new TimeBoundedLerp(firstHopDuration/2.0f);

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
			xLerp->update(dt, false);
			yLerp->update(dt, false);

			//OutputDebugStringA(Util::MessageFormater::print(colliderRect.x, ", ", colliderRect.y, '\n').c_str());

			// event: 
			if (yLerp->getCurrentT() >= 1.0f) {
				xLerp->resetHard();
				yLerp->resetSmooth();

				/*
				yt -= 1.0f;
				xt = 0;
				xTimeAccumulator = 0;
				yTimeAccumulator = 0;
				*/

				currentPhysicState = PhysicState::FIRST_HOP_DOWN;
				originalPos.x = colliderRect.x;
				OutputDebugStringA("MOVE TO FIRST HOP DOWN\n");

			}
			else {
				float xd = Util::Math::lerp(0, howFarFirstHop / 2.0f, xLerp->getCurrentT());
				float yd = Util::Math::lerp(0, firstHopHeight, Util::Math::upCurve(yLerp->getCurrentT()));

				/*
				float xd = //xLerp->getCurrentValue(0, howFarFirstHop/2.0f);
				float yd = Util::Math::lerp(0, firstHopHeight, Util::Math::upCurve(yt));
				*/

				if (direction == -1) {
					xd = -xd;
				}

				colliderRect.x = originalPos.x + xd;
				colliderRect.y = originalPos.y + yd;
			}
		} break;

		case PhysicState::FIRST_HOP_DOWN: {
			xLerp->update(dt, false);
			yLerp->update(dt, false);

			bool hitGround = false;
			for (RectangleCollider *rect: levelData.groundColliders) {
				hitGround = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, rect->getRect());
				if (hitGround) break;
			}

			if (hitGround) {
				xLerp->resetHard();
				yLerp->resetSmooth();

				xLerp->setDuration(secondHopDuration/2.0f);
				yLerp->setDuration(secondHopDuration/2.0f);

				currentPhysicState = PhysicState::SECOND_HOP_UP;

				originalPos.x = colliderRect.x;
				originalPos.y = colliderRect.y;

			}
			else {
				float xd = Util::Math::lerp(0, howFarFirstHop / 2.0f, xLerp->getCurrentT());
				float yd = Util::Math::lerp(0, firstHopHeight, Util::Math::downCurve(yLerp->getCurrentT()));

				if (direction == -1) {
					xd = -xd;
				}

				colliderRect.x = originalPos.x + xd;
				colliderRect.y = originalPos.y + yd;
			}

		} break;

		case PhysicState::SECOND_HOP_UP: {
			// action: 
			xLerp->update(dt, false);
			yLerp->update(dt, false);

			// event: 
			if (yLerp->getCurrentT() >= 1.0f) {
				xLerp->resetHard();
				yLerp->resetSmooth();

				currentPhysicState = PhysicState::SECOND_HOP_DOWN;
				originalPos.x = colliderRect.x;
			}
			else {
				float xd = Util::Math::lerp(0, howFarSecondHop / 2.0f, xLerp->getCurrentT());
				float yd = Util::Math::lerp(0, secondHopHeight, Util::Math::upCurve(yLerp->getCurrentT()));

				if (direction == -1) {
					xd = -xd;
				}

				colliderRect.x = originalPos.x + xd;
				colliderRect.y = originalPos.y + yd;
			}
		} break;

		case PhysicState::SECOND_HOP_DOWN: {
			xLerp->update(dt, false);
			yLerp->update(dt, false);

			bool hitGround = false;
			for (RectangleCollider *rect: levelData.groundColliders) {
				hitGround = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, rect->getRect());
				if (hitGround) break;
			}

			if (hitGround) {
				xLerp->resetHard();
				yLerp->resetHard();

				currentPhysicState = PhysicState::NONE;

				originalPos.x = colliderRect.x;
				originalPos.y = colliderRect.y;
			}
			else {
				float xd = Util::Math::lerp(0, howFarSecondHop / 2.0f, xLerp->getCurrentT());
				float yd = Util::Math::lerp(0, secondHopHeight, Util::Math::downCurve(yLerp->getCurrentT()));

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
