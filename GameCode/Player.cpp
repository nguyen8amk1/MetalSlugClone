#pragma once 
#include "MetalSlug.h"
#include "Animation.h"
#include "CollisionChecker.h"
#include "../Util.cpp"

// Only for test 
#include "Windows.h"

namespace MetalSlug {

class Player: public CameraControlledEntity {
private: 
	Rect colliderRect = { -19.0f, .3f, .2f, .4f };

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
		DYING
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

	AnimationMetaData dieAnimationMetaData; // row: 1, col: 19, 0, 97, 817, 40
	Animation* dieAnimation;

	Animation* currentAnimation;
	Animation* currentLegAnimation;

	bool die = false;

	Rect interactionRectDisabledRect = {0, -5.0f, 0, 0};
	Rect interactionRect;

public: 
	void changeGravity(float gravity) {
		this->gravity = gravity;
	}

	Player(float gravity, float moveSpeed, PlatformSpecficMethodsCollection *platformMethods) {
		this->moveSpeed = moveSpeed;
		this->gravity = gravity;

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
			if (input.pressLeft) {
				colliderRect.x -= (float)(moveSpeed*dt); 
			}
			else if (input.pressRight) {
				colliderRect.x += (float)(moveSpeed*dt); 
			}

			if (input.pressShoot) {

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
				else if(!die && input.pressJump) {
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

				// TODO: handle if sudden hit another object on the head -> physicState = FALL
				bool suddenHitPlatform = false;

				if (jumpT >= 1) {
					jumpT -= 1;
					physicState = PhysicState::JUMPDOWN;
				}
				else if (suddenHitPlatform) {
					physicState = PhysicState::FALL;
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

		// animation state machine (sometimes the animation state based on the physics state)
		switch (animationState) {
		case AnimationState::IDLING: {
			bool onGround = physicState == PhysicState::ONGROUND;
			if (input.pressLeft) {
				animationState = AnimationState::WALKING;
				horizontalFacingDirection = -1;
				currentAnimation = walkingAnimation;
				currentLegAnimation = walkingLegAnimation;
			}
			else if (input.pressRight) {
				animationState = AnimationState::WALKING;
				horizontalFacingDirection = 1;
				currentAnimation = walkingAnimation;
				currentLegAnimation = walkingLegAnimation;
			}

			if (input.pressJump) {
				toJumpingAnimation();
			}
			else if (!onGround) {
				toFallingAnimation();
			}

			if (die) {
				toDyingAnimation();
			}
		}break;

		case AnimationState::WALKING: {

			bool isPressingMove = input.pressLeft || input.pressRight;
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
		} break;

		case AnimationState::JUMPING: {
			if (physicState == PhysicState::JUMPDOWN ||
				physicState == PhysicState::FALL) {
				toFallingAnimation();
			}

		} break;

		case AnimationState::FALLING: {
			if (physicState == PhysicState::ONGROUND) {
				toIdlingAnimation();
			}
		} break;

		case AnimationState::DYING: {
			// TODO: 
			// action: 
			// event: 
			bool doneDieAnimation = dieAnimation->finishOneCycle();
			if (doneDieAnimation) {
				toIdlingAnimation();
				colliderRect.x -= .2f;
				die = false;
			}
		} break;

		default: 
			// TODO: handle error "Animation state not recoginize"
			break;
		}

		bool collided = false;
		for (RectangleCollider *ground: levelData.groundColliders) {
			collided = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, ground->getRect());
			if (collided) break;
		}

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

	void moveXBy(float d) override {
		colliderRect.x += d;
	}

	void moveYBy(float d) override {
		colliderRect.y += d;
	}

	Rect getRect() { return colliderRect; }

	Rect getInteractionRect() { return interactionRect; }
};

}