#pragma once
#include "MetalSlug.h"
#include "CollisionChecker.h"
#include "../Util.h"

namespace MetalSlug {
enum class PlayerPhysicState {
	ONGROUND,
	JUMPUP,
	JUMPDOWN,
	FALL
};

struct PlayerPhysicResult {
	Rect colliderRect; 
	PlayerPhysicState physicState;
	bool die;
};

class PlayerPhysic {
private:
	PlayerPhysicState physicState = PlayerPhysicState::ONGROUND;

	float jumpTotalTime = 1.0f;
	float jumpT = 0;
	float jumpTimeAccumulator = 0;

	float originalGroundY = 0;
	float jumpHeight = .5777f;
	float gravity;
	float moveSpeed;

public:
	PlayerPhysic(float moveSpeed, float gravity) {
		this->gravity = gravity;
		this->moveSpeed = moveSpeed;
	}

	PlayerPhysicResult update(PlayerEvent &event, double dt, Rect colliderRect, bool die, LevelData &levelData) {
		if (!die) {
			if (event.moveLeft) {
				colliderRect.x -= (float)(moveSpeed*dt); 
			}
			else if (event.moveRight) {
				colliderRect.x += (float)(moveSpeed*dt); 
			}
		}

		// Physics state machine
		if (physicState == PlayerPhysicState::FALL) {
			colliderRect.y -= (float)(gravity*dt); 

			commonOnGroundEventTransition(colliderRect, levelData);
			commonDieEventTransition(colliderRect, levelData, die);
		}
		else if (physicState == PlayerPhysicState::ONGROUND) {
			bool collided = false;
			for (RectangleCollider* ground : levelData.groundColliders) {
				collided = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, ground->getRect());
				if (collided) {
					break;
				}
			}

			if (!collided) {
				physicState = PlayerPhysicState::FALL;
			}

			else if(!die && event.jump) {
				physicState = PlayerPhysicState::JUMPUP;
				jumpT = 0;
				jumpTimeAccumulator = 0;
				originalGroundY = colliderRect.y; 
			}

			commonDieEventTransition(colliderRect, levelData, die);
		}
		else if (physicState == PlayerPhysicState::JUMPUP) {

			jumpTimeAccumulator += dt;
			Util::Math::clampf(jumpTimeAccumulator, 0, jumpTotalTime/2.0f);
			jumpT = Util::Math::normalizef(jumpTimeAccumulator, jumpTotalTime/2.0f);

			if (jumpT >= 1.0f) {
				jumpT -= 1.0f;
				jumpTimeAccumulator = 0;
				physicState = PlayerPhysicState::JUMPDOWN;
			}
			else {
				float yd = Util::Math::upCurve(jumpT)* jumpHeight;
				colliderRect.y = originalGroundY + yd;
			}
			commonDieEventTransition(colliderRect, levelData, die);
		}
		else if (physicState == PlayerPhysicState::JUMPDOWN) {
			jumpTimeAccumulator += dt;
			Util::Math::clampf(jumpTimeAccumulator, 0, jumpTotalTime/2.0f);
			jumpT = Util::Math::normalizef(jumpTimeAccumulator, jumpTotalTime/2.0f);
			float yd = Util::Math::downCurve(jumpT) * jumpHeight;
			colliderRect.y = originalGroundY + yd;

			commonOnGroundEventTransition(colliderRect, levelData);
			commonDieEventTransition(colliderRect, levelData, die);
		}

		return {colliderRect, physicState, die};
	}

private: 
	void commonOnGroundEventTransition(Rect &colliderRect, LevelData &levelData) {
		CollisionInfo colInfo;
		for (RectangleCollider* ground : levelData.groundColliders) {
			CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, ground->getRect(), colInfo);
			if (colInfo.count > 0) {
				break;
			}
		}

		if (colInfo.count > 0) {
			physicState = PlayerPhysicState::ONGROUND;
			colliderRect.x -= colInfo.normal.x * colInfo.depths[0];
			colliderRect.y -= colInfo.normal.y * colInfo.depths[0];
		}
	}

	void commonDieEventTransition(Rect &colliderRect, LevelData &levelData, bool &die) {
		bool hitDangerRect = false;
		for (Rect dangerRect: levelData.dangerRects) {
			hitDangerRect = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, dangerRect);
			if (hitDangerRect) break;
		}

		if (hitDangerRect) {
			die = true;
		}
	}
};

}
