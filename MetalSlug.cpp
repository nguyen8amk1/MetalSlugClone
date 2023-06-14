#pragma once

#include<iostream>
#include "Util.cpp"

namespace MetalSlug {

// This is interface only  
class PlatformSpecficMethodsCollection {
public: 
	virtual void loadImage(const std::string& filename) = 0;
	virtual void debugLog(const std::string& debugString) = 0;
};

class MetalSlug {
private: 
	PlatformSpecficMethodsCollection *platformMethods; 

public:
	MetalSlug(PlatformSpecficMethodsCollection *platformMethods) {
		this->platformMethods = platformMethods;
	}

	void updateAndRender(double dt) {
		platformMethods->debugLog(print("Delta time: ", dt, '\n'));
	}
};

}