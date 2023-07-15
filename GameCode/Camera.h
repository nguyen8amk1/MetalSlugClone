#pragma once 
#include "MetalSlug.h"

namespace MetalSlug {
class Camera {
private: 
	Vec2f currentPosition;
	Vec2f oldPosition;

public: 
	Camera(const Vec2f position) {
		this->currentPosition = position;
		this->oldPosition = position;
	}

	Vec2f convertWorldPosToScreenPos(const Vec2f &pos) {
		float x = pos.x - currentPosition.x;
		float y = pos.y - currentPosition.y;
		float dx = currentPosition.x - oldPosition.x;
		float dy = currentPosition.y - oldPosition.y;

		return {x - dx, y - dy};
	}

	 Rect convertWorldRectToScreenRect(const Rect rect) {
		Rect r = rect;
		Vec2f t = convertWorldPosToScreenPos({ r.x, r.y });
		r.x = t.x;
		r.y = t.y;
		return r;
	 }

	void moveXBy(float d) {
		oldPosition.x = currentPosition.x;
		currentPosition.x += d;
	}

	void moveYBy(float d) {
		oldPosition.y = currentPosition.y;
		currentPosition.y += d;
	}

	Vec2f getPos() {
		return currentPosition;
	}
};

}