#pragma once
#include<string>
#include<vector>


namespace MetalSlug {


struct Rect {
	float x, y, width, height;
};

struct PlatformSpecificImage {
	virtual void setRect(Rect &rect) = 0;
	virtual void fitRect(Rect& rect) = 0;
	virtual PlatformSpecificImage* getImagePortion(Rect &rect) = 0;
	virtual int getPixelWidth() = 0;
	virtual int getPixelHeight() = 0;
	virtual float getGameWidth() = 0;
	virtual float getGameHeight() = 0;
};

struct PlatformDebugInfo {
	float frameTimeMillis;
	float fps;
};

struct GameInputContext {
	bool pressLeft,
		pressRight,
		pressUp,
		pressDown,
		pressJump,
		pressShoot,
		pressUpArrow,
		pressDownArrow,
		pressLeftArrow,
		pressRightArrow,
		pressDebugZoomInBackground,
		pressDebugZoomOutBackground,
		pressEnter;

	bool mouseRightClick, 
		mouseLeftClick;
};

struct GameText {
	virtual void setText(const std::string &text) = 0;
	virtual void setPixelPos(int x, int y) = 0;
	virtual void clean() = 0;
};

struct Circle {
	float x, y, r;
};

struct Color {
	int r, g, b, a;
};

struct Point {
	float x, y;
};

typedef Point Vec2f;

struct Capsule {
	Point start;
	Point end;
	float r;
};

struct CollisionInfo {
	int count;
	float depths[2];
	Vec2f contact_points[2];

	// always points from shape A to shape B (first and second shapes passed into
	// any of the c2***to***Manifold functions)
	Vec2f normal;
};

// This is interface only  
class PlatformSpecficMethodsCollection {
public: 
	virtual PlatformSpecificImage* loadImage(const std::string& filename) = 0;
	virtual void renderImage(PlatformSpecificImage *image, bool flipX=false, bool flipY=false) = 0;
	virtual void renderImagePortionAt(PlatformSpecificImage* image, Rect &pixelRects, Rect &fittedNormalizedRects, bool flipX = false, bool flipY = false) = 0;
	virtual void fillRectangle(Rect &rect, Color &color) = 0;
	virtual void drawRectangle(Rect &rect, Color &color) = 0;
	virtual void fillCircle(Circle &circle , Color &color) = 0;
	virtual void drawCircle(Circle &circle, Color &color) = 0;
	virtual GameText* createText(int x, int y, int fontSize) = 0;
	virtual void drawText(GameText *text) = 0;

	virtual void drawPoint(Point &p, Color &color) = 0;
	virtual void drawLine(Point &a, Point &b, Color &color) = 0;
	virtual void drawCapsule(Capsule &capsule, Color &color) = 0;

	virtual void debugLog(const std::string& debugString) = 0;
};


class CameraControlledEntity {
public: 
	virtual void moveXBy(float d) = 0;
	virtual void moveYBy(float d) = 0;
};

class RectangleCollider : public CameraControlledEntity {
private: 
	Rect rect;
public: 
	RectangleCollider(Rect rect) {
		this->rect = rect;
	}

	void moveXBy(float d) override {
		rect.x += d;
	}

	void moveYBy(float d) override {
		rect.y += d;
	}

	Rect getRect() { return this->rect; }
};

class Camera {
private: 
	Vec2f currentPosition;
	Vec2f oldPosition;

public: 
	Camera(const Vec2f position) {
		this->currentPosition = position;
		this->oldPosition = position;
	}

	void apply(CameraControlledEntity *entity) {
		entity->moveXBy(-currentPosition.x);
		entity->moveYBy(-currentPosition.y);
	}

	void update(CameraControlledEntity *entity) {
		float dx = currentPosition.x - oldPosition.x;
		float dy = currentPosition.y - oldPosition.y;

		entity->moveXBy(-dx);
		entity->moveYBy(-dy);
	}

	void moveXBy(float d) {
		oldPosition.x = currentPosition.x;
		currentPosition.x += d;
	}

	void moveYBy(float d) {
		oldPosition.y = currentPosition.y;
		currentPosition.y += d;
	}
};

struct LevelData {
	std::vector<RectangleCollider*> groundColliders;
	Rect playerColliderRect;
	bool levelStarted = false;
};
}
