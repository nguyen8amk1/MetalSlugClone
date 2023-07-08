#pragma once 
#include "MetalSlug.h"
#include "Animation.h"
#include "CollisionChecker.h"
#include "../Util.cpp"

#include "Windows.h"

namespace MetalSlug {

class Player: public CameraControlledEntity {
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

	// Grenade 
	Rect grenadeRect = { 0, 0, .1f, .1f };

	enum class GrenadePhysicState {
		NONE, 
		FIRST_HOP_UP, 
		FIRST_HOP_DOWN, 
		SECOND_HOP_UP, 
		SECOND_HOP_DOWN, 
		ON_GROUND
	};

	enum class GrenadeAnimationState {
		NONE,
		FIRST_HOP, 
		SECOND_HOP, 
		EXPLODE
	};

	GrenadePhysicState currentGrenadePhysicState = GrenadePhysicState::NONE;
	GrenadeAnimationState currentGrenadeAnimationState = GrenadeAnimationState::NONE;

	AnimationMetaData grenadeSpinningAnimationMetaData;
	Animation* grenadeSpinningAnimation;

	Animation* currentGrenadeAnimation;

	int grenadeDirection = 1;  
	
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

		// TODO: need the animation in the file 
		Util::AnimationUtil::initAnimationMetaData(throwingAnimationMetaData, filename, .1f, 1, 19, {0, 200}, {43, 40});
		throwingAnimation = new Animation(throwingAnimationMetaData, platformMethods);

		//Grenade
		Util::AnimationUtil::initAnimationMetaData(grenadeSpinningAnimationMetaData, filename, .1f, 1, 1, {0, 428}, {11, 18}); //0, 428, 11, 18
		grenadeSpinningAnimation = new Animation(grenadeSpinningAnimationMetaData, platformMethods);

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

	float firstHopHeight = .251851f;
	float secondHopHeight = .15f;

	float grenadeXTimeAccumulator = 0.0f;
	float grenadeYTimeAccumulator = 0.0f;

	float howFarFirstHop = 0.4806f;
	float howFarSecondHop = 0.31097f;

	float firstHopDuration = 0.6f;
	float secondHopDuration = 0.6f;


	float xt = 0.0f;
	float yt = 0.0f;
	Point originalGrenadePos;

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
			else if (input.pressThrowGrenade) {
				grenadeInit();
				toThrowingAnimation();
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
			else if (input.pressThrowGrenade) {
				grenadeInit();
				toThrowingAnimation();
			}

		} break;

		case AnimationState::JUMPING: {
			if (physicState == PhysicState::JUMPDOWN ||
				physicState == PhysicState::FALL) {
				toFallingAnimation();
			}

			if (!die && input.pressThrowGrenade) {
				grenadeInit();
				toThrowingAnimation();
			}

		} break;

		case AnimationState::FALLING: {
			if (physicState == PhysicState::ONGROUND) {
				toIdlingAnimation();
			}

			if (!die && input.pressThrowGrenade) {
				grenadeInit();
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
			// TODO: check whatever leg animation and change to it
			if (input.pressLeft) {
				horizontalFacingDirection = -1;
				currentLegAnimation = walkingLegAnimation;
			}
			else if (input.pressRight) {
				horizontalFacingDirection = 1;
				currentLegAnimation = walkingLegAnimation;
			}

			bool isPressingMove = input.pressLeft || input.pressRight;
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

		grenadeStateMachineUpdate(dt, levelData);
		
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

		//OutputDebugStringA(Util::MessageFormater::print(grenadeRect.x, ", ", grenadeRect.y, '\n').c_str());

		if (currentGrenadeAnimation) {
			currentGrenadeAnimation->changePos(grenadeRect.x, grenadeRect.y);
			currentGrenadeAnimation->changeSize(grenadeRect.width, grenadeRect.height);
			currentGrenadeAnimation->animate(camera, dt);
		}

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

	float upCurve(float t) {
		return -powf((t - 1), 2) + 1;
	}

	float downCurve(float t) {
		return  -powf(t, 2) + 1;
	}

	void grenadeInit() {
		currentGrenadePhysicState = GrenadePhysicState::FIRST_HOP_UP;
		currentGrenadeAnimationState = GrenadeAnimationState::FIRST_HOP;
		currentGrenadeAnimation = grenadeSpinningAnimation;
		originalGrenadePos.x = colliderRect.x;
		originalGrenadePos.y = colliderRect.y;
		grenadeDirection = horizontalFacingDirection;
	}

	void grenadeStateMachineUpdate(double dt, LevelData &levelData) {
		// Grenade State Machine  
		switch (currentGrenadePhysicState) {
		case GrenadePhysicState::FIRST_HOP_UP: {
			// action: 
			grenadeXTimeAccumulator += dt;
			grenadeYTimeAccumulator += dt;

			Util::Math::clampf(grenadeXTimeAccumulator, 0, firstHopDuration/2.0f);
			Util::Math::clampf(grenadeYTimeAccumulator, 0, firstHopDuration/2.0f);

			xt = Util::Math::normalizef(grenadeXTimeAccumulator, firstHopDuration/2.0f);
			yt = Util::Math::normalizef(grenadeYTimeAccumulator, firstHopDuration/2.0f);

			// event: 
			if (yt >= 1.0f) {
				yt -= 1.0f;
				xt = 0;
				currentGrenadePhysicState = GrenadePhysicState::FIRST_HOP_DOWN;
				originalGrenadePos.x = grenadeRect.x;

				grenadeXTimeAccumulator = 0;
				grenadeYTimeAccumulator = 0;
			}
			else {
				float xd = xt * howFarFirstHop / 2.0f;
				float yd = upCurve(yt) * firstHopHeight;
				if (grenadeDirection == -1) {
					xd = -xd;
				}

				grenadeRect.x = originalGrenadePos.x + xd;
				grenadeRect.y = originalGrenadePos.y + yd;
			}
		} break;

		case GrenadePhysicState::FIRST_HOP_DOWN: {
			grenadeXTimeAccumulator += dt;
			grenadeYTimeAccumulator += dt;

			Util::Math::clampf(grenadeXTimeAccumulator, 0, firstHopDuration/2.0f);
			Util::Math::clampf(grenadeYTimeAccumulator, 0, firstHopDuration/2.0f);

			xt = Util::Math::normalizef(grenadeXTimeAccumulator, firstHopDuration/2.0f);
			yt = Util::Math::normalizef(grenadeYTimeAccumulator, firstHopDuration/2.0f);

			bool hitGround = false;
			for (RectangleCollider *rect: levelData.groundColliders) {
				hitGround = CollisionChecker::doesRectangleVsRectangleCollide(grenadeRect, rect->getRect());
				if (hitGround) break;
			}

			if (hitGround) {
				yt = 0;
				xt = 0;
				currentGrenadePhysicState = GrenadePhysicState::SECOND_HOP_UP;
				originalGrenadePos.x = grenadeRect.x;
				originalGrenadePos.y = grenadeRect.y;

				grenadeXTimeAccumulator = 0;
				grenadeYTimeAccumulator = 0;

			}
			else {
				float xd = xt * howFarFirstHop / 2.0f;
				float yd = downCurve(yt) * firstHopHeight;
				if (grenadeDirection == -1) {
					xd = -xd;
				}

				grenadeRect.x = originalGrenadePos.x + xd;
				grenadeRect.y = originalGrenadePos.y + yd;
			}

		} break;

		case GrenadePhysicState::SECOND_HOP_UP: {
			// action: 
			grenadeXTimeAccumulator += dt;
			grenadeYTimeAccumulator += dt;

			Util::Math::clampf(grenadeXTimeAccumulator, 0, secondHopDuration/2.0f);
			Util::Math::clampf(grenadeYTimeAccumulator, 0, secondHopDuration/2.0f);

			xt = Util::Math::normalizef(grenadeXTimeAccumulator, secondHopDuration/2.0f);
			yt = Util::Math::normalizef(grenadeYTimeAccumulator, secondHopDuration/2.0f);

			// event: 
			if (yt >= 1.0f) {
				yt -= 1.0f;
				xt = 0;
				currentGrenadePhysicState = GrenadePhysicState::SECOND_HOP_DOWN;
				originalGrenadePos.x = grenadeRect.x;

				grenadeXTimeAccumulator = 0;
				grenadeYTimeAccumulator = 0;
			}
			else {
				float xd = xt * howFarSecondHop / 2.0f;
				float yd = upCurve(yt) * secondHopHeight;
				if (grenadeDirection == -1) {
					xd = -xd;
				}

				grenadeRect.x = originalGrenadePos.x + xd;
				grenadeRect.y = originalGrenadePos.y + yd;
			}
		} break;

		case GrenadePhysicState::SECOND_HOP_DOWN: {
			grenadeXTimeAccumulator += dt;
			grenadeYTimeAccumulator += dt;

			Util::Math::clampf(grenadeXTimeAccumulator, 0, secondHopDuration/2.0f);
			Util::Math::clampf(grenadeYTimeAccumulator, 0, secondHopDuration/2.0f);

			xt = Util::Math::normalizef(grenadeXTimeAccumulator, secondHopDuration/2.0f);
			yt = Util::Math::normalizef(grenadeYTimeAccumulator, secondHopDuration/2.0f);

			bool hitGround = false;
			for (RectangleCollider *rect: levelData.groundColliders) {
				hitGround = CollisionChecker::doesRectangleVsRectangleCollide(grenadeRect, rect->getRect());
				if (hitGround) break;
			}

			if (hitGround) {
				yt = 0;
				xt = 0;
				currentGrenadePhysicState = GrenadePhysicState::NONE;

				originalGrenadePos.x = grenadeRect.x;
				originalGrenadePos.y = grenadeRect.y;

				grenadeXTimeAccumulator = 0;
				grenadeYTimeAccumulator = 0;
			}
			else {
				float xd = xt * howFarSecondHop / 2.0f;
				float yd = downCurve(yt) * secondHopHeight;
				if (grenadeDirection == -1) {
					xd = -xd;
				}

				grenadeRect.x = originalGrenadePos.x + xd;
				grenadeRect.y = originalGrenadePos.y + yd;
			}

		} break;

		case GrenadePhysicState::ON_GROUND: {
			// TODO: 
			// action: 
			// event: 

		} break;
		}
	}

public:
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