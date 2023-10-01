#pragma once 
#include<vector>
#include "MetalSlug.h"
#include "CollisionChecker.h"

namespace MetalSlug {
// NOTE: the physic state machine is currently reusable but not extenable 
// since the states of the new extended system not interaction correctly with the basic state machine 

// Physic state machine  
enum class BasicPhysicState {
	ONGROUND, 
	FALL 
};

struct BasicPhysicStateMachineResult {
	Rect colliderRect;
	BasicPhysicState basicPhysicState;
};

class BasicPhysicStateMachine {
private: 
	BasicPhysicState currentPhysicState = BasicPhysicState::ONGROUND;
	float gravity;
public:
	BasicPhysicStateMachine(float gravity);
	BasicPhysicStateMachineResult update(double dt, Rect colliderRect, std::vector<RectangleCollider*> *groundColliders);
};

}
