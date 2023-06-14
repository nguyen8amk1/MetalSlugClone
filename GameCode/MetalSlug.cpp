#pragma once

#include<iostream>
#include "../Util.cpp"

struct PlatformSpecificWindowContext;

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
	virtual void renderImage(PlatformSpecificWindowContext *windowContext) = 0;
	virtual void fillRectangle(PlatformSpecificWindowContext *windowContext, Rect &rect) = 0;
	virtual void drawRectangle(PlatformSpecificWindowContext *windowContext, Rect &rect) = 0;
	virtual void debugLog(const std::string& debugString) = 0;
};

class MetalSlug {
private: 
	PlatformSpecficMethodsCollection *platformMethods; 
	PlatformSpecificWindowContext *windowContext; 
	Rect r = {50, 50, 50, 50};

public:
	MetalSlug(PlatformSpecificWindowContext *windowContext, PlatformSpecficMethodsCollection *platformMethods) {
		this->platformMethods = platformMethods;
		this->windowContext = windowContext;
	}

	int speed = 2;
	void updateAndRender(GameInputContext &input, double dt) {
		platformMethods->debugLog(Util::print("Delta time: ", dt, '\n'));
		platformMethods->fillRectangle(windowContext, r);

		// @StartTest: 
		if (input.moveLeft) {
			r.x -= speed * dt; 
		}
		else if (input.moveRight) {
			r.x += speed * dt; 
		}
		if (input.moveUp) {
			r.y -= speed * dt; 
		}
		else if (input.moveDown) {
			r.y += speed * dt; 
		}
		// @EndTest
	}
};

}