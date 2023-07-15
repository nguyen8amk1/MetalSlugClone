#pragma once
#include "MetalSlug.h"
#include "Animation.h"
#include "CollisionChecker.h"
#include "PlayerPhysic.h"
#include "../Util.h"
#include "Windows.h"

/*
class State;

struct StateResult{
	State* nextState;
};

class State {
public: 
	virtual StateResult update(const GameInputContext& input) = 0;
};

class Transition {
public:
	State* nextState(const GameInputContext& input) {
		if (input.left.isDown) {
			// TODO: 
		}
	}
};

class IdlingState: public State {
private:
	State *nextState = NULL;
	Transition transition;
public:
	IdlingState(Transition transition) {
	}	

	StateResult update(const GameInputContext &input) override {
		// DO STUFF 
		if() {
			return ;
		}	
		return { transition.nextState };
	}
};

struct AnimationStateMachineResult {
	State *nextState;
};

class AnimationStateMachine {
private: 
	std::vector<State*> states;
	State* currentState;
public:
	AnimationStateMachineResult update(const GameInputContext &input) {
		// DO stuff:
		StateResult result = currentState->update();
		return result.nextState;
	}
};
*/

namespace MetalSlug {
struct PlayerAnimationResult {
	Rect colliderRect;
	int horizontalFacingDirection;
	bool die;
};

class PlayerAnimation {
private: 
	enum class BodyAnimationState {
		IDLING,
		JUMPING,
		FALLING,
		WALKING,
		DYING,
		THROWING,
		HORIZONTAL_SHOOTING, 
		UP_SHOOTING
	};

	enum class LegAnimationState {
		IDLING,
		JUMPING,
		FALLING,
		WALKING,
		DYING
	};

	BodyAnimationState bodyAnimationState = BodyAnimationState::IDLING;
	LegAnimationState legAnimationState = LegAnimationState::IDLING;

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

	AnimationMetaData horizontalShootingAnimationMetaData; 
	Animation* horizontalShootingAnimation;

	AnimationMetaData upShootingAnimationMetaData; 
	Animation* upShootingAnimation;

	AnimationMetaData bulletMetaData; 
	Animation* bullet;

	Animation* currentBodyAnimation;
	Animation* currentLegAnimation;

	bool grenadeIsThrow = false; 
	bool doneDieAnimation = false;

public: 
	PlayerAnimation(PlatformSpecficMethodsCollection *platformMethods) {
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

		Util::AnimationUtil::initAnimationMetaData(throwingAnimationMetaData, filename, .1f, 1, 6, {0, 138}, {36, 30});
		throwingAnimation = new Animation(throwingAnimationMetaData, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(horizontalShootingAnimationMetaData, filename, .1f, 1, 10, {0, 168}, {52, 29});
		horizontalShootingAnimation = new Animation(horizontalShootingAnimationMetaData, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(upShootingAnimationMetaData, filename, .1f, 1, 10, {0, 197}, {30, 65});
		upShootingAnimation = new Animation(upShootingAnimationMetaData, platformMethods);

		Util::AnimationUtil::initAnimationMetaData(bulletMetaData, filename, .1f, 1, 1, {74, 476}, {12, 11});
		bullet = new Animation(bulletMetaData, platformMethods);

		currentBodyAnimation = idlingAnimation;
		currentLegAnimation = idlingLegAnimation;
	}

	PlayerAnimationResult update(const GameInputContext &input, double dt, Camera *camera, LevelData &levelData, PlayerPhysicState physicState, Rect colliderRect, int horizontalFacingDirection, bool die) {
		bodyAnimationStateMachineUpdate(input, dt, camera, levelData, physicState, colliderRect, horizontalFacingDirection, die);
		legAnimationStateMachineUpdate(input, dt, camera, levelData, physicState, colliderRect, horizontalFacingDirection, die);
		
		float legX = colliderRect.x;
		float legY = colliderRect.y - .15f;

		if (currentLegAnimation) {
			currentLegAnimation->changePos(legX, legY);
			currentLegAnimation->flip(horizontalFacingDirection, 1);
			currentLegAnimation->animate(camera, dt);
		}

		currentBodyAnimation->changePos(colliderRect.x, colliderRect.y);
		currentBodyAnimation->flip(horizontalFacingDirection, 1);
		currentBodyAnimation->animate(camera, dt);

		return {colliderRect, horizontalFacingDirection, die };
	}

	bool isThrowGrenade() {
		return grenadeIsThrow;

	}

	~PlayerAnimation() {
		delete idlingAnimation;
		delete jumpingAnimation;
		delete fallingAnimation;
		delete walkingAnimation;

		delete idlingLegAnimation;
		delete walkingLegAnimation;
		delete jumpingLegAnimation;
		delete fallingLegAnimation;
	}

private: 
	void legAnimationStateMachineUpdate(const GameInputContext &input, double dt, Camera *camera, LevelData &levelData, PlayerPhysicState physicState, Rect colliderRect, int &horizontalFacingDirection, bool &die) {
		switch (legAnimationState) {
		case LegAnimationState::IDLING: {
			commonWalkingLegEventTransition(input, horizontalFacingDirection);
			commonLegEventTransition(input, die, physicState);
		}break;

		case LegAnimationState::WALKING: {
			bool isPressingMove = input.left.isDown || input.right.isDown;
			if (!isPressingMove && 
				physicState != PlayerPhysicState::JUMPUP && 
				physicState != PlayerPhysicState::JUMPDOWN && 
				physicState != PlayerPhysicState::FALL) {

				toLegIdlingAnimation();
			}

			commonLegEventTransition(input, die, physicState);
		} break;

		case LegAnimationState::JUMPING: {
			if (physicState == PlayerPhysicState::JUMPDOWN ||
				physicState == PlayerPhysicState::FALL) {
				toLegFallingAnimation();
			}

			commonLegDieEventTransition(die);
		} break;

		case LegAnimationState::FALLING: {
			if (physicState == PlayerPhysicState::ONGROUND) {
				toLegIdlingAnimation();
			}

			commonLegDieEventTransition(die);
		} break;

		case LegAnimationState::DYING: {
			// action: 
			// event: 
			if (doneDieAnimation) {
				toLegIdlingAnimation();
			}
		} break;
		}
	}

	void bodyAnimationStateMachineUpdate (const GameInputContext &input, double dt, Camera *camera, LevelData &levelData, PlayerPhysicState physicState, Rect &colliderRect, int horizontalFacingDirection, bool &die) {
		switch (bodyAnimationState) {
		case BodyAnimationState::IDLING: {
			commonBodyWalkingEventTransition(input);
			commonThrowingBombEventTransition(input);
			commonShootingEventTransition(input);
			commonBodyEventTransition(input, die, physicState);
		}break;

		case BodyAnimationState::WALKING: {
			bool isPressingMove = input.left.isDown || input.right.isDown;
			if (!isPressingMove && 
				physicState != PlayerPhysicState::JUMPUP && 
				physicState != PlayerPhysicState::JUMPDOWN && 
				physicState != PlayerPhysicState::FALL) {

				toBodyIdlingAnimation();
			}

			commonBodyEventTransition(input, die, physicState);
			commonThrowingBombEventTransition(input);
			commonShootingEventTransition(input);
		} break;

		case BodyAnimationState::JUMPING: {
			if (physicState == PlayerPhysicState::JUMPDOWN ||
				physicState == PlayerPhysicState::FALL) {
				toBodyFallingAnimation();
			}

			commonBodyDieEventTransition(die);
			commonThrowingBombEventTransition(input);
			commonShootingEventTransition(input);
		} break;

		case BodyAnimationState::FALLING: {
			if (physicState == PlayerPhysicState::ONGROUND) {
				toBodyIdlingAnimation();
			}

			commonBodyDieEventTransition(die);
			commonThrowingBombEventTransition(input);
			commonShootingEventTransition(input);
		} break;

		case BodyAnimationState::DYING: {
			// action: 
			// event: 
			doneDieAnimation = dieAnimation->finishOneCycle();
			if (doneDieAnimation) {
				toBodyIdlingAnimation();
				colliderRect.x -= .2f;
				die = false;
			}
		} break;
		case BodyAnimationState::THROWING: {

			bool isPressingMove = input.left.isDown || input.right.isDown;
			if (!isPressingMove && 
				physicState != PlayerPhysicState::JUMPUP && 
				physicState != PlayerPhysicState::JUMPDOWN && 
				physicState != PlayerPhysicState::FALL) {
				currentBodyAnimation = idlingAnimation;
			}

			// event: 
			bool doneThrowAnimation = throwingAnimation->finishOneCycle();
			if (doneThrowAnimation) {
				// TODO: some how go to the state that it should be, one of 4:  jumping, falling, walking, idling 
				toBodyIdlingAnimation();
			}

			commonBodyWalkingEventTransition(input);
			commonBodyEventTransition(input, die, physicState);
			commonThrowingBombEventTransition(input);
			commonShootingEventTransition(input);
		} break;

		case BodyAnimationState::HORIZONTAL_SHOOTING: {
			bool doneShootingAnimation = horizontalShootingAnimation->finishOneCycle();
			if (doneShootingAnimation) {
				toBodyIdlingAnimation();
			}
			commonBodyWalkingEventTransition(input);
			commonBodyEventTransition(input, die, physicState);
			commonThrowingBombEventTransition(input);
		} break;

		case BodyAnimationState::UP_SHOOTING: {
			bool doneShootingAnimation = upShootingAnimation->finishOneCycle();
			if (doneShootingAnimation) {
				toBodyIdlingAnimation();
			}
			commonBodyWalkingEventTransition(input);
			commonBodyEventTransition(input, die, physicState);
			commonThrowingBombEventTransition(input);
		} break;

		}


	}

	void commonLegEventTransition(const GameInputContext &input, bool die, PlayerPhysicState physicState) {
		//bool onGround = physicState == PlayerPhysicState::ONGROUND;
		if (physicState == PlayerPhysicState::JUMPUP) {
			toLegJumpingAnimation();
		}
		else if (physicState == PlayerPhysicState::FALL) {
			toLegFallingAnimation();
		}

		commonLegDieEventTransition(die);
	}

	void commonBodyEventTransition(const GameInputContext &input, bool die, PlayerPhysicState physicState) {
		if (physicState == PlayerPhysicState::JUMPUP) {
			toBodyJumpingAnimation();
		}
		else if (physicState == PlayerPhysicState::FALL) {
			toBodyFallingAnimation();
		}

		commonBodyDieEventTransition(die);
	}

	void commonLegDieEventTransition(bool die) {
		if (die) {
			toLegDyingAnimation();
		}
	}

	void commonBodyDieEventTransition(bool die) {
		if (die) {
			toBodyDyingAnimation();
		}
	}

	void commonThrowingBombEventTransition(const GameInputContext &input) {
		if (input.throwGrenade.isPressed) {
			grenadeIsThrow = true;
			toThrowingAnimation();
		}
		else {
			grenadeIsThrow = false;
		}
	}

	void commonShootingEventTransition(const GameInputContext &input) {
		bool verticalInputIsDown = input.up.isDown && input.down.isDown;
		if (!verticalInputIsDown && input.shoot.isDown) {
			toHorizontalShootingAnimation();
		}

		if (input.up.isDown && input.shoot.isDown) {
			toUpShootingAnimation();

		}
	}

	void commonBodyWalkingEventTransition(const GameInputContext &input) {
		if (input.left.isDown || input.right.isDown) {
			bodyAnimationState = BodyAnimationState::WALKING;
			currentBodyAnimation = walkingAnimation;
		}
	}

	void commonWalkingLegEventTransition(const GameInputContext &input, int &horizontalFacingDirection) {
		if (input.left.isDown) {
			legAnimationState = LegAnimationState::WALKING;
			horizontalFacingDirection = -1;
			currentLegAnimation = walkingLegAnimation;
		}
		else if (input.right.isDown) {
			legAnimationState = LegAnimationState::WALKING;
			horizontalFacingDirection = 1;
			currentLegAnimation = walkingLegAnimation;
		}
	}



	void toBodyIdlingAnimation() {
		bodyAnimationState = BodyAnimationState::IDLING;
		currentBodyAnimation = idlingAnimation;
	}

	void toLegIdlingAnimation() {
		legAnimationState = LegAnimationState::IDLING;
		currentLegAnimation = idlingLegAnimation;
	}

	void toBodyFallingAnimation() {
		bodyAnimationState = BodyAnimationState::FALLING;
		currentBodyAnimation = fallingAnimation;
		currentLegAnimation = fallingLegAnimation;
	}

	void toLegFallingAnimation() {
		legAnimationState = LegAnimationState::FALLING;
		currentLegAnimation = fallingLegAnimation;
	}

	void toBodyDyingAnimation() {
		bodyAnimationState = BodyAnimationState::DYING;
		currentBodyAnimation = dieAnimation;
	}

	void toLegDyingAnimation() {
		legAnimationState = LegAnimationState::DYING;
		currentLegAnimation = NULL;
	}

	void toBodyJumpingAnimation() {
		bodyAnimationState = BodyAnimationState::JUMPING;
		currentBodyAnimation = jumpingAnimation;
	}

	void toLegJumpingAnimation() {
		legAnimationState = LegAnimationState::JUMPING;
		currentLegAnimation = jumpingLegAnimation;
	}

	void toThrowingAnimation() {
		bodyAnimationState = BodyAnimationState::THROWING;
		currentBodyAnimation = throwingAnimation;
	}

	void toHorizontalShootingAnimation() {
		bodyAnimationState = BodyAnimationState::HORIZONTAL_SHOOTING;
		currentBodyAnimation = horizontalShootingAnimation;
	}

	void toUpShootingAnimation() {
		bodyAnimationState = BodyAnimationState::UP_SHOOTING;
		currentBodyAnimation = upShootingAnimation;
	}
};

}
