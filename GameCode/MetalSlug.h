#pragma once

namespace MetalSlug {

struct Rect {
	int x, y, width, height;
};

struct GameInputContext {
	bool moveLeft, moveRight, moveUp, moveDown;
};

// This is interface only  
class PlatformSpecficMethodsCollection {
public: 
	virtual void loadImage(const std::string& filename) = 0;
	virtual void renderImage() = 0;
	virtual void fillRectangle(Rect &rect) = 0;
	virtual void drawRectangle(Rect &rect) = 0;
	virtual void debugLog(const std::string& debugString) = 0;
};

}
