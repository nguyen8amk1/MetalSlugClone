#pragma once

#include<SDL.h>
#include<Windows.h>
#include "GameCode/MetalSlug.cpp"

struct PlatformSpecificWindowContext {
    SDL_Renderer* renderer;
};

namespace SDL2Platform {

class SDL2PlatformMethodsCollection: public MetalSlug::PlatformSpecficMethodsCollection {
public: 
    void loadImage(const std::string& filename) override {
        // TODO: load image using SDL2 methods
    }

    void debugLog(const std::string& debugString) override {
        OutputDebugStringA(debugString.c_str());
    }

    void renderImage(PlatformSpecificWindowContext* windowContext) override {
		// TODO: 
    }

    void fillRectangle(PlatformSpecificWindowContext* windowContext, MetalSlug::Rect &rect) override {
        SDL_Rect r;
        r.x = rect.x;
        r.y = rect.y;
        r.w = rect.width;
        r.h = rect.height;

        // Set render color to blue ( rect will be rendered in this color )
        SDL_SetRenderDrawColor(windowContext->renderer, 0, 0, 255, 255);

        // Render rect
        SDL_RenderFillRect(windowContext->renderer, &r);
    }

    void drawRectangle(PlatformSpecificWindowContext* windowContext, MetalSlug::Rect &rect) override {
        SDL_Rect r;
        r.x = rect.x;
        r.y = rect.y;
        r.w = rect.width;
        r.h = rect.height;

        // Set render color to blue ( rect will be rendered in this color )
        SDL_SetRenderDrawColor(windowContext->renderer, 0, 0, 255, 255);

        SDL_RenderDrawRect(windowContext->renderer, &r);
    }
};
}