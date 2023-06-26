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

struct LevelData {
	std::vector<Rect> groundColliders;
	Rect playerColliderRect;
};

class MetalSlug;
}
