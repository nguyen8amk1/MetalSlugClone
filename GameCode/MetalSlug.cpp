#pragma once

#include<iostream>
#include "../Util.cpp"
#include "MetalSlug.h"

namespace MetalSlug {

class MetalSlug {
private: 
	PlatformSpecficMethodsCollection *platformMethods; 
	Rect r = {50, 50, 50, 50};

public:
	MetalSlug(PlatformSpecficMethodsCollection *platformMethods) {
		this->platformMethods = platformMethods;
	}

	int tempSpeed = 500;
	void updateAndRender(GameInputContext &input, double dt) {
		/*
		std::string debugString = Util::MessageFormater::print("Delta time: ", dt, '\n');
		platformMethods->debugLog(debugString);
		*/

		platformMethods->fillRectangle(r);

		// @StartTest: 
		if (input.moveLeft) {
			r.x -= tempSpeed*dt; 
		}
		else if (input.moveRight) {
			r.x += tempSpeed*dt; 
		}
		if (input.moveUp) {
			r.y -= tempSpeed*dt; 
		}
		else if (input.moveDown) {
			r.y += tempSpeed*dt; 
		}
		// @EndTest
	}
};

}