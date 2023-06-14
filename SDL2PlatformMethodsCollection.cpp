#pragma once

#include<Windows.h>
#include "MetalSlug.cpp"

namespace SDL2Platform {

class SDL2PlatformMethodsCollection: public MetalSlug::PlatformSpecficMethodsCollection {
public: 
    void loadImage(const std::string& filename) override {
        // TODO: load image using SDL2 methods
    }

    void debugLog(const std::string& debugString) override {
        OutputDebugStringA(debugString.c_str());
    }
};
}