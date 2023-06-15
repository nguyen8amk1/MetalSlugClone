#pragma once


namespace MetalSlug {

struct Rect {
	int x, y, width, height;
};

struct PlatformSpecificImage {
	virtual void setRect(Rect &rect) = 0;
};


struct GameInputContext {
	bool moveLeft, moveRight, moveUp, moveDown;
};

// This is interface only  
class PlatformSpecficMethodsCollection {
public: 
	virtual PlatformSpecificImage* loadImage(const std::string& filename) = 0;
	virtual void renderImage(PlatformSpecificImage *image) = 0;
	virtual void fillRectangle(Rect &rect) = 0;
	virtual void drawRectangle(Rect &rect) = 0;
	virtual void debugLog(const std::string& debugString) = 0;
};

}
