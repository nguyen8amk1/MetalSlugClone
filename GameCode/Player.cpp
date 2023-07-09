#pragma once 
#include "MetalSlug.h"
#include "Animation.h"
#include "CollisionChecker.h"
#include "../Util.cpp"

//#include "Windows.h"
#include "Grenade.h"

namespace MetalSlug {

class Player {
private: 
	Rect colliderRect = { -17.8f, 1.0f, .2f, .4f };

	enum class PhysicState {
		ONGROUND,
		JUMPUP,
		JUMPDOWN,
		FALL
	};

	enum class AnimationState {
		IDLING,
		JUMPING,
		FALLING,
		WALKING, 
		DYING, 
		THROWING 
	}Vec2f ;

	PhysicState physicState = PhysicState::ONGROUND;
	AnimationState animationState = AnimationState::IDLING;
	float moveSpeed, gravity;

	float jumpT = 0;
	float jumpProgress = 0;
	float originalGroundY = 0;
	float jumpHeight = .5777f;
	float horizontalFacingDirection = 1;

	AnimationMetaData idlingAnimationMetaData;
	AnimationMetaData jumpingAnimationMetaData;
	AnimationMetaData fallingAnimationMetaData;
	AnimationMetaData walkingAnimationMetaData;

	Animation* idlingAnimation;
	Animation* jumpingAnimation;
	Animation* fallingAnimation;
	Animation* walkingAnimation;

	AnimationMetaData walkingLegAnimationMetaData;
	Animation* walkingLegAnimation;

	AnimationMetaData idlingLegAnimationMetaData;
	Animation* idlingLegAnimation;

	AnimationMetaData jumpingLegAnimationMetaData;
	Animation* jumpingLegAnimation;

	AnimationMetaData fallingLegAnimationMetaData;
	Animation* fallingLegAnimation;

	AnimationMetaData dieAnimationMetaData; 
	Animation* dieAnimation;

	AnimationMetaData throwingAnimationMetaData; 
	Animation* throwingAnimation;

	Animation* currentAnimation;
	Animation* currentLegAnimation;

	bool die = false;

	Rect interactionRectDisabledRect = {0, -5.0f, 0, 0};
	Rect interactionRect;

	std::vector<Grenade*> grenades;
	Rect grenadeRect = { 0, 0, .1f, .1f };

	PlatformSpecficMethodsCollection* platformMethods;

public: 
	void changeGravity(float gravity) {
		this->gravity = gravity;
	}

	Player(float gravity, float moveSpeed, PlatformSpecficMethodsCollection *platformMethods) {
		// test 
		this->moveSpeed = moveSpeed;
		this->gravity = gravity;

		this->platformMethods = platformMethods;

		std::string filename = "Assets/Imgs/Characters/marco_messi.png";
		Util::AnimationUtil::initAnimationMetaData(idlingAnimationMetaData, filename, .15f, 1, 4, {0, 0}, {31, 29});
		idlingAnimation  = new Animation(idlingAnimationMetaData, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(walkingAnimationMetaData, filename, .15f, 1, 12, { 0, 29 }, {34, 29});
		walkingAnimation = new Animation(walkingAnimationMetaData, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(jumpingAnimationMetaData, filename, .1f, 1, 6, {0, 64}, {29, 26});
		jumpingAnimation = new Animation(jumpingAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(fallingAnimationMetaData, filename, .1f, 1, 6, {145, 64}, {-29, 26});
		fallingAnimation = new Animation(fallingAnimationMetaData, platformMethods);

		// Player's Leg animation
		Util::AnimationUtil::initAnimationMetaData(walkingLegAnimationMetaData, filename, .1f, 1, 12, {405, 44}, {32, 20} );
		walkingLegAnimation  = new Animation(walkingLegAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(idlingLegAnimationMetaData, filename, .1f, 1, 1, {124, 13}, {21, 16});
		idlingLegAnimation  = new Animation(idlingLegAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(jumpingLegAnimationMetaData, filename, .1f, 1, 6, {174, 64}, {21, 25});
		jumpingLegAnimation  = new Animation(jumpingLegAnimationMetaData,platformMethods);

		Util::AnimationUtil::initAnimationMetaData(fallingLegAnimationMetaData, filename, .1f, 1, 6, {279, 64}, {-21, 25});
		fallingLegAnimation = new Animation(fallingLegAnimationMetaData, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(dieAnimationMetaData, filename, .1f, 1, 19, {0, 97}, {43, 40});
		dieAnimation = new Animation(dieAnimationMetaData, platformMethods);

		// TODO: need the animation in the file 
		Util::AnimationUtil::initAnimationMetaData(throwingAnimationMetaData, filename, .1f, 1, 19, {0, 200}, {43, 40});
		throwingAnimation = new Animation(throwingAnimationMetaData, platformMethods);

		currentAnimation = idlingAnimation;
		currentLegAnimation = idlingLegAnimation;
	}

	~Player() {
		delete idlingAnimation;
		delete jumpingAnimation;
		delete fallingAnimation;
		delete walkingAnimation;

		delete idlingLegAnimation;
		delete walkingLegAnimation;
		delete jumpingLegAnimation;
		delete fallingLegAnimation;
	}

	void update(const GameInputContext &input, LevelData &levelData, Camera *camera, double dt) {
		// @StartTest: 
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
			// Physics state machine
			if (physicState == PhysicState::FALL) {
				colliderRect.y -= (float)(gravity*dt); 

				CollisionInfo colInfo;
				for (RectangleCollider* ground : levelData.groundColliders) {
					CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, ground->getRect(), colInfo);
					if (colInfo.count > 0) {
						break;
					}
				}

				if (colInfo.count > 0) {
					physicState = PhysicState::ONGROUND;
					colliderRect.x -= colInfo.normal.x * colInfo.depths[0];
					colliderRect.y -= colInfo.normal.y * colInfo.depths[0];
				}
			}
			else if (physicState == PhysicState::ONGROUND) {
				bool collided = false;
				for (RectangleCollider* ground : levelData.groundColliders) {
					collided = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, ground->getRect());
					if (collided) {
						break;
					}
				}

				if (!collided) {
					physicState = PhysicState::FALL;
				}
				else if(!die && input.jump.isDown) {
					physicState = PhysicState::JUMPUP;
					jumpT = 0;
					originalGroundY = colliderRect.y; 
				}

				bool hitDangerRect = false;
				for (Rect dangerRect: levelData.dangerRects) {
					hitDangerRect = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, dangerRect);
					if (hitDangerRect) break;
				}

				if (hitDangerRect) {
					die = true;
				}
			}
			else if (physicState == PhysicState::JUMPUP) {
				jumpT += gravity*dt;
				jumpProgress = -pow((jumpT-1), 2) + 1;
				colliderRect.y = originalGroundY + (jumpHeight)*jumpProgress; 

				if (jumpT >= 1) {
					jumpT -= 1;
					physicState = PhysicState::JUMPDOWN;
				}
			}
			else if (physicState == PhysicState::JUMPDOWN) {
				jumpT += gravity*dt;
				jumpProgress = -pow(jumpT, 2) + 1;
				colliderRect.y = originalGroundY + (jumpHeight)*jumpProgress; 

				CollisionInfo colInfo;
				for (RectangleCollider *ground: levelData.groundColliders) {
					CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, ground->getRect(), colInfo);
					if (colInfo.count > 0) break;
				}

				if (colInfo.count > 0) {
					physicState = PhysicState::ONGROUND;

					colliderRect.x -= colInfo.normal.x * colInfo.depths[0];
					colliderRect.y -= colInfo.normal.y * colInfo.depths[0];
				}
			}
		}

		// ANIMATION STATE MACHINE (sometimes the animation state based on the physics state)
		switch (animationState) {
		case AnimationState::IDLING: {
			bool onGround = physicState == PhysicState::ONGROUND;
			if (input.left.isDown) {
				animationState = AnimationState::WALKING;
				horizontalFacingDirection = -1;
				currentAnimation = walkingAnimation;
				currentLegAnimation = walkingLegAnimation;
			}
			else if (input.right.isDown) {
				animationState = AnimationState::WALKING;
				horizontalFacingDirection = 1;
				currentAnimation = walkingAnimation;
				currentLegAnimation = walkingLegAnimation;
			}

			if (input.jump.isDown) {
				toJumpingAnimation();
			}
			else if (!onGround) {
				toFallingAnimation();
			}

			if (die) {
				toDyingAnimation();
			}
			else if (input.throwGrenade.isDown) {
				throwGrenade();
				toThrowingAnimation();
			}

		}break;

		case AnimationState::WALKING: {

			bool isPressingMove = input.left.isDown || input.right.isDown;
			if (!isPressingMove && 
				physicState != PhysicState::JUMPUP && 
				physicState != PhysicState::JUMPDOWN && 
				physicState != PhysicState::FALL) {

				toIdlingAnimation();
			}
			else if (physicState == PhysicState::JUMPUP) {
				toJumpingAnimation();
			}
			else if (physicState == PhysicState::FALL) {
				toFallingAnimation();
			}

			if (die) {
				toDyingAnimation();
			}
			else if (input.throwGrenade.isDown) {
				throwGrenade();
				toThrowingAnimation();
			}

		} break;

		case AnimationState::JUMPING: {
			if (physicState == PhysicState::JUMPDOWN ||
				physicState == PhysicState::FALL) {
				toFallingAnimation();
			}

			if (!die && input.throwGrenade.isDown) {
				throwGrenade();
				toThrowingAnimation();
			}

		} break;

		case AnimationState::FALLING: {
			if (physicState == PhysicState::ONGROUND) {
				toIdlingAnimation();
			}

			if (!die && input.throwGrenade.isDown) {
				throwGrenade();
				toThrowingAnimation();
			}
		} break;

		case AnimationState::DYING: {
			// action: 
			// event: 
			bool doneDieAnimation = dieAnimation->finishOneCycle();
			if (doneDieAnimation) {
				toIdlingAnimation();
				colliderRect.x -= .2f;
				die = false;
			}
		} break;

		case AnimationState::THROWING: {
			// action: 
			// FIXME: This is not how it's suppose to work, need huge refactor later since the leg transition here is wrong 
			if (input.left.isDown) {
				horizontalFacingDirection = -1;
				currentLegAnimation = walkingLegAnimation;
			}
			else if (input.right.isDown) {
				horizontalFacingDirection = 1;
				currentLegAnimation = walkingLegAnimation;
			}

			bool isPressingMove = input.left.isDown || input.right.isDown;
			if (!isPressingMove && 
				physicState != PhysicState::JUMPUP && 
				physicState != PhysicState::JUMPDOWN && 
				physicState != PhysicState::FALL) {
				currentLegAnimation = idlingLegAnimation;
			}
			else if (physicState == PhysicState::JUMPUP) {
				currentLegAnimation = jumpingLegAnimation;
			}
			else if (physicState == PhysicState::FALL) {
				currentLegAnimation = fallingLegAnimation;
			}

			if (!die && input.throwGrenade.isDown) {
				throwGrenade();
				// TODO: reset the throwing animation
			}

			// event: 
			bool doneThrowAnimation = throwingAnimation->finishOneCycle();
			if (doneThrowAnimation) {
				// TODO: some how go to the state that it should be, one of 4:  jumping, falling, walking, idling 
				toIdlingAnimation();
			}
		} break;

		default: 
			// TODO: handle error "Animation state not recoginize"
			break;
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
		// @EndTest
		//OutputDebugStringA(Util::MessageFormater::print(grenadeRect.x, ", ", grenadeRect.y, '\n').c_str());

		float legX = colliderRect.x;
		float legY = colliderRect.y - .15f;

		if (currentLegAnimation) {
			currentLegAnimation->changePos(legX, legY);
			currentLegAnimation->flip(horizontalFacingDirection, 1);
			currentLegAnimation->animate(camera, dt);
		}

		currentAnimation->changePos(colliderRect.x, colliderRect.y);
		currentAnimation->flip(horizontalFacingDirection, 1);
		currentAnimation->animate(camera, dt);
	}

private: 
	void toIdlingAnimation() {
		animationState = AnimationState::IDLING;
		currentAnimation = idlingAnimation;
		currentLegAnimation = idlingLegAnimation;
	}

	void toFallingAnimation() {
		animationState = AnimationState::FALLING;
		currentAnimation = fallingAnimation;
		currentLegAnimation = fallingLegAnimation;
	}

	void toDyingAnimation() {
		animationState = AnimationState::DYING;
		currentAnimation = dieAnimation;
		currentLegAnimation = NULL;
	}

	void toJumpingAnimation() {
		animationState = AnimationState::JUMPING;
		currentAnimation = jumpingAnimation;
		currentLegAnimation = jumpingLegAnimation;
	}

	void toThrowingAnimation() {
		animationState = AnimationState::THROWING;
		currentAnimation = throwingAnimation;
	}

	void throwGrenade()
	{
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