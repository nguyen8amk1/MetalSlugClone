#pragma once 
#include "MetalSlug.h"
#include "CollisionChecker.h"

namespace MetalSlug {
// NOTE: the physic state machine is currently reusable but not extenable 
// since the states of the new extended system not interaction correctly with the basic state machine 

struct BasicPhysicStateMachineResult {
	Rect colliderRect;
};

// Physic state machine  
enum class BasicPhysicState {
	ONGROUND, 
	FALL 
};

class BasicPhysicStateMachine {
public:

private: 
	BasicPhysicState currentPhysicState = BasicPhysicState::ONGROUND;
	float gravity;

public:
	BasicPhysicStateMachine(float gravity) {
		this->gravity = gravity;
	}

	BasicPhysicStateMachineResult update(double dt, Rect colliderRect, LevelData &levelData) {
		// Physic state machine 
		switch (currentPhysicState) {
		case BasicPhysicState::ONGROUND: {
			for (RectangleCollider *groundCollider: levelData.groundColliders) {
				if (!CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, groundCollider->getRect())) {
					currentPhysicState = BasicPhysicState::FALL;
					break;
				}
			}
			break;
		}
		case BasicPhysicState::FALL: {
			colliderRect.y -= (float)(gravity*dt); 

			for (RectangleCollider* groundCollider : levelData.groundColliders) {
				CollisionInfo colInfo;
				CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, groundCollider->getRect(), colInfo);
				if (colInfo.count > 0) {
					currentPhysicState = BasicPhysicState::ONGROUND;
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
};

}
