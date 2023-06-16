#pragma once
#include<string>


namespace MetalSlug {

struct Rect {
	float x, y, width, height;
};

struct PlatformSpecificImage {
	virtual void setRect(Rect &rect) = 0;
	virtual PlatformSpecificImage* getImagePortion(Rect &rect) = 0;
	virtual int getPixelWidth() = 0;
	virtual int getPixelHeight() = 0;
};

struct PlatformDebugInfo {
	float frameTimeMillis;
	float fps;
};

struct GameInputContext {
	bool moveLeft, moveRight, moveUp, moveDown;
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

struct Capsule {
	Point start;
	Point end;
	float r;
};

// This is interface only  
class PlatformSpecficMethodsCollection {
public: 
	virtual PlatformSpecificImage* loadImage(const std::string& filename) = 0;
	virtual void renderImage(PlatformSpecificImage *image) = 0;
	virtual void fillRectangle(Rect &rect, Color &color) = 0;
	virtual void drawRectangle(Rect &rect, Color &color) = 0;
	virtual void fillCircle(Circle &circle , Color &color) = 0;
	virtual void drawCircle(Circle &circle, Color &color) = 0;
	virtual GameText* createText(int x, int y) = 0;
	virtual void drawText(GameText *text) = 0;

	virtual void drawPoint(Point &p, Color &color) = 0;
	virtual void drawLine(Point &a, Point &b, Color &color) = 0;
	virtual void drawCapsule(Capsule &capsule, Color &color) = 0;

	virtual void debugLog(const std::string& debugString) = 0;
};

}
