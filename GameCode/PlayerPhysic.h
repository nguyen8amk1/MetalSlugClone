#pragma once
#include "MetalSlug.h"
#include "CollisionChecker.h"
#include "../Util.cpp"

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

public:
	PlayerPhysic(float gravity) {
		this->gravity = gravity;
	}

	PlayerPhysicResult update(const GameInputContext &input, double dt, Rect colliderRect, bool die, LevelData &levelData) {
		// Physics state machine
		if (physicState == PlayerPhysicState::FALL) {
			colliderRect.y -= (float)(gravity*dt); 

			commonOnGroundTransition(colliderRect, levelData);
			commonDieTransition(colliderRect, levelData, die);
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
			else if(!die && input.jump.isDown) {
				physicState = PlayerPhysicState::JUMPUP;
				jumpT = 0;
				jumpTimeAccumulator = 0;
				originalGroundY = colliderRect.y; 
			}

			commonDieTransition(colliderRect, levelData, die);
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
			commonDieTransition(colliderRect, levelData, die);
		}
		else if (physicState == PlayerPhysicState::JUMPDOWN) {
			jumpTimeAccumulator += dt;
			Util::Math::clampf(jumpTimeAccumulator, 0, jumpTotalTime/2.0f);
			jumpT = Util::Math::normalizef(jumpTimeAccumulator, jumpTotalTime/2.0f);
			float yd = Util::Math::downCurve(jumpT) * jumpHeight;
			colliderRect.y = originalGroundY + yd;

			commonOnGroundTransition(colliderRect, levelData);
			commonDieTransition(colliderRect, levelData, die);
		}

		return {colliderRect, physicState, die};
	}

private: 
	void commonOnGroundTransition(Rect &colliderRect, LevelData &levelData) {
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

	void commonDieTransition(Rect &colliderRect, LevelData &levelData, bool &die) {
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
