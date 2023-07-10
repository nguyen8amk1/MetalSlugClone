#pragma once 
#include "MetalSlug.h"
#include "Animation.h"
#include "../Util.cpp"
#include "CollisionChecker.h"
#include "Physics.h"

namespace MetalSlug {

class RebelSoilder {
private:
	// Collider 
	Rect colliderRect;
	Rect interactionRectDisabledRect = { 0, -5.0f, 0, 0 };
	Rect interactionRect;


	// Animation
	AnimationMetaData idleAnimationMetaData;
	Animation* idleAnimation;

	AnimationMetaData slashingAnimationMetaData;
	Animation* slashingAnimation;

	AnimationMetaData throwingBombAnimationMetaData;
	Animation* throwingBombAnimation;

	// Animation state machine 
	enum class AnimationState {
		IDLING,			// 0, 4, 108, 38
		SLASHING,		// 0, 42, 735, 37
		THROWING_BOMB	// 0, 79, 782, 42 
	};

	AnimationState currentAnimationState = AnimationState::IDLING;
	Animation* currentAnimation;

	PlatformSpecficMethodsCollection* platformMethods;
	float gravity;
	float moveSpeed;
	BasicPhysicStateMachine* physicStateMachine;

	double timeAccumulator = 0.0;

	BasicPhysicStateMachineResult physicResult = {colliderRect};

public: 
	RebelSoilder(float gravity, float moveSpeed, Rect colliderRect, PlatformSpecficMethodsCollection *platformMethods) {
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

	}

	void update(LevelData &levelData, Camera *camera, double dt) {
		physicResult = physicStateMachine->update(dt, colliderRect, levelData);
		colliderRect = physicResult.colliderRect;

		// Animation state machine 
		switch (currentAnimationState) {
		case AnimationState::IDLING: {
			// TODO: 
			bool touchPlayer = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, levelData.playerColliderRect);
			bool playerInThrowingRange = false;
			if (touchPlayer) {
				currentAnimationState = AnimationState::SLASHING;
				currentAnimation = slashingAnimation;
			}
			else if (playerInThrowingRange) {
				currentAnimationState = AnimationState::THROWING_BOMB;
				currentAnimation = throwingBombAnimation;
			}
			break;
		}

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
			break;
		}

		case AnimationState::THROWING_BOMB: {
			// TODO: 
			// action:...
			// transition: to idling or slashing  
			// event: ...
			break;
		}
		}

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

	void moveXBy(float d) {
		colliderRect.x += d;
	}

	void moveYBy(float d) {
		colliderRect.y += d;
	}

	Rect getInteractionRect() { return interactionRect; }
};

}
