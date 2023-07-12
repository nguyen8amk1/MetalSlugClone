#pragma once
#include "MetalSlug.h"
#include "Animation.h"
#include "CollisionChecker.h"
#include "PlayerPhysic.h"
#include "../Util.cpp"

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
		SHOOTING 
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

	Animation* currentBodyAnimation;
	Animation* currentLegAnimation;

	bool grenadeIsThrow = false;

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

		Util::AnimationUtil::initAnimationMetaData(throwingAnimationMetaData, filename, .2f, 1, 6, {0, 138}, {36, 30});
		throwingAnimation = new Animation(throwingAnimationMetaData, platformMethods);

		currentBodyAnimation = idlingAnimation;
		currentLegAnimation = idlingLegAnimation;
	}

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

	PlayerAnimationResult update(const GameInputContext &input, double dt, Camera *camera, LevelData &levelData, PlayerPhysicState physicState, Rect colliderRect, int horizontalFacingDirection, bool die) {
		/*
		animationResult = animationStateMachine->update();
		currentState = animationResult.nextState;
		currentState->update();
		*/

		legAnimationStateMachineUpdate(input, dt, camera, levelData, physicState, colliderRect, horizontalFacingDirection, die);
		bodyAnimationStateMachineUpdate(input, dt, camera, levelData, physicState, colliderRect, horizontalFacingDirection, die);
		
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

	void commonLegTransition(const GameInputContext &input, bool die, PlayerPhysicState physicState) {
		//bool onGround = physicState == PlayerPhysicState::ONGROUND;
		if (physicState == PlayerPhysicState::JUMPUP) {
			toLegJumpingAnimation();
		}
		else if (physicState == PlayerPhysicState::FALL) {
			toLegFallingAnimation();
		}

		commonLegDieTransition(die);
	}

	void commonBodyTransition(const GameInputContext &input, bool die, PlayerPhysicState physicState) {
		if (physicState == PlayerPhysicState::JUMPUP) {
			toBodyJumpingAnimation();
		}
		else if (physicState == PlayerPhysicState::FALL) {
			toBodyFallingAnimation();
		}

		commonBodyDieTransition(die);
	}

	void commonLegDieTransition(bool die) {
		if (die) {
			toLegDyingAnimation();
		}
	}

	void commonBodyDieTransition(bool die) {
		if (die) {
			toBodyDyingAnimation();
		}
	}

	void commonThrowingBombTransition(const GameInputContext &input) {
		if (input.throwGrenade.isDown) {
			grenadeIsThrow = true;
			toThrowingAnimation();
		}
		else {
			grenadeIsThrow = false;
		}
	}

	void commonShootingTransition() {
		// TODO: do shooting transition

	}

	void commonBodyWalkingTransition(const GameInputContext &input) {
		if (input.left.isDown || input.right.isDown) {
			bodyAnimationState = BodyAnimationState::WALKING;
			currentBodyAnimation = walkingAnimation;
		}
	}

	void commonWalkingLegTransition(const GameInputContext &input, int &horizontalFacingDirection) {
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

	void legAnimationStateMachineUpdate(const GameInputContext &input, double dt, Camera *camera, LevelData &levelData, PlayerPhysicState physicState, Rect colliderRect, int &horizontalFacingDirection, bool &die) {
		switch (legAnimationState) {
		case LegAnimationState::IDLING: {
			commonWalkingLegTransition(input, horizontalFacingDirection);
			commonLegTransition(input, die, physicState);
		}break;

		case LegAnimationState::WALKING: {
			bool isPressingMove = input.left.isDown || input.right.isDown;
			if (!isPressingMove && 
				physicState != PlayerPhysicState::JUMPUP && 
				physicState != PlayerPhysicState::JUMPDOWN && 
				physicState != PlayerPhysicState::FALL) {

				toLegIdlingAnimation();
			}

			commonLegTransition(input, die, physicState);
		} break;

		case LegAnimationState::JUMPING: {
			if (physicState == PlayerPhysicState::JUMPDOWN ||
				physicState == PlayerPhysicState::FALL) {
				toLegFallingAnimation();
			}

			commonLegDieTransition(die);
		} break;

		case LegAnimationState::FALLING: {
			if (physicState == PlayerPhysicState::ONGROUND) {
				toLegIdlingAnimation();
			}

			commonLegDieTransition(die);
		} break;

		case LegAnimationState::DYING: {
			// action: 
			// event: 
			bool doneDieAnimation = dieAnimation->finishOneCycle();
			if (doneDieAnimation) {
				toLegIdlingAnimation();
				/*
				colliderRect.x -= .2f;
				die = false;
				*/
			}
		} break;
		}
	}

	void bodyAnimationStateMachineUpdate (const GameInputContext &input, double dt, Camera *camera, LevelData &levelData, PlayerPhysicState physicState, Rect &colliderRect, int horizontalFacingDirection, bool &die) {
		switch (bodyAnimationState) {
		case BodyAnimationState::IDLING: {
			commonBodyWalkingTransition(input);
			commonThrowingBombTransition(input);
			commonBodyTransition(input, die, physicState);
		}break;

		case BodyAnimationState::WALKING: {
			bool isPressingMove = input.left.isDown || input.right.isDown;
			if (!isPressingMove && 
				physicState != PlayerPhysicState::JUMPUP && 
				physicState != PlayerPhysicState::JUMPDOWN && 
				physicState != PlayerPhysicState::FALL) {

				toBodyIdlingAnimation();
			}

			commonBodyTransition(input, die, physicState);
			commonThrowingBombTransition(input);
		} break;

		case BodyAnimationState::JUMPING: {
			if (physicState == PlayerPhysicState::JUMPDOWN ||
				physicState == PlayerPhysicState::FALL) {
				toBodyFallingAnimation();
			}

			commonBodyDieTransition(die);
			commonThrowingBombTransition(input);
		} break;

		case BodyAnimationState::FALLING: {
			if (physicState == PlayerPhysicState::ONGROUND) {
				toBodyIdlingAnimation();
			}

			commonBodyDieTransition(die);
			commonThrowingBombTransition(input);
		} break;

		case BodyAnimationState::DYING: {
			// action: 
			// event: 
			bool doneDieAnimation = dieAnimation->finishOneCycle();
			if (doneDieAnimation) {
				toBodyIdlingAnimation();
				colliderRect.x -= .2f;
				die = false;
			}
		} break;
		case BodyAnimationState::THROWING: {
			// FIXME: 
			// it's still be able to die but can't shift back anymore
			// can't throw 

			commonBodyWalkingTransition(input);

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

			commonThrowingBombTransition(input);
			commonBodyTransition(input, die, physicState);
		} break;
		}


	}
};

}
