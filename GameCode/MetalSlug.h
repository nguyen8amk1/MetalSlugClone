#pragma once


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

// This is interface only  
class PlatformSpecficMethodsCollection {
public: 
	virtual PlatformSpecificImage* loadImage(const std::string& filename) = 0;
	virtual void renderImage(PlatformSpecificImage *image) = 0;
	virtual void fillRectangle(Rect &rect) = 0;
	virtual void drawRectangle(Rect &rect) = 0;
	virtual GameText* createText(int x, int y) = 0;
	virtual void drawText(GameText *text) = 0;

	/*
	virtual void drawPoint(Rect &rect) = 0;
	virtual void fillEllipse(Rect &rect) = 0;
	virtual void drawEllipse(Rect &rect) = 0;
	*/
	virtual void debugLog(const std::string& debugString) = 0;
};

}
