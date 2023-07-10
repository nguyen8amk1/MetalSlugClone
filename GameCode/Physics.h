#pragma once 
#include "MetalSlug.h"
#include "CollisionChecker.h"

namespace MetalSlug {

struct BasicPhysicStateMachineResult {
	Rect colliderRect;
};

class BasicPhysicStateMachine {
public:
	// Physic state machine  
	static enum class PhysicState {
		ONGROUND, 
		FALL 
	};

private: 
	PhysicState currentPhysicState = PhysicState::ONGROUND;
	float gravity;

public:
	BasicPhysicStateMachine(float gravity) {
		this->gravity = gravity;
	}

	BasicPhysicStateMachineResult update(double dt, Rect colliderRect, LevelData &levelData) {
		// Physic state machine 
		switch (currentPhysicState) {
		case PhysicState::ONGROUND: {
			for (RectangleCollider *groundCollider: levelData.groundColliders) {
				if (!CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, groundCollider->getRect())) {
					currentPhysicState = PhysicState::FALL;
					break;
				}
			}
			break;
		}
		case PhysicState::FALL: {
			colliderRect.y -= (float)(gravity*dt); 

			for (RectangleCollider* groundCollider : levelData.groundColliders) {
				CollisionInfo colInfo;
				CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, groundCollider->getRect(), colInfo);
				if (colInfo.count > 0) {
					currentPhysicState = PhysicState::ONGROUND;
					colliderRect.x -= colInfo.normal.x * colInfo.depths[0];
					colliderRect.y -= colInfo.normal.y * colInfo.depths[0];
					break;
				}
			}
			break;
		}
		}
		return { colliderRect };
	}

	PhysicState currentState() {
		return currentPhysicState;
	}
};

}
