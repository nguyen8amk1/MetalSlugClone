#include "Physics.h"
namespace MetalSlug {

BasicPhysicStateMachine::BasicPhysicStateMachine(float gravity) {
	this->gravity = gravity;
}

BasicPhysicStateMachineResult BasicPhysicStateMachine::update(double dt, Rect colliderRect, std::vector<RectangleCollider*> *groundColliders) {
	// Physic state machine 
	switch (currentPhysicState) {
	case BasicPhysicState::ONGROUND: {
		for (RectangleCollider *groundCollider: *groundColliders) {
			if (!CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, groundCollider->getRect())) {
				currentPhysicState = BasicPhysicState::FALL;
				break;
			}
		}
		break;
	}
	case BasicPhysicState::FALL: {
		colliderRect.y -= (float)(gravity*dt); 

		for (RectangleCollider *groundCollider: *groundColliders) {
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

}