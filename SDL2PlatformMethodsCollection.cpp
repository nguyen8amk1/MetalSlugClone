#pragma once

#include<string>
#include<SDL.h>
#include<SDL_image.h>
#include<Windows.h>
#include "GameCode/MetalSlug.h"

struct PlatformSpecificImage {
    SDL_Texture* texture;
    SDL_Rect textureRect;
};

namespace SDL2Platform {

class SDL2PlatformMethodsCollection: public MetalSlug::PlatformSpecficMethodsCollection {
public: 
    SDL_Renderer* renderer;
    SDL_Texture* frameTexture;

public: 

    PlatformSpecificImage loadImage(const std::string& filename) override {
        // TODO: load image using SDL2 methods
        PlatformSpecificImage image;
		SDL_Texture* img = IMG_LoadTexture(renderer, filename.c_str());
		int w, h;
		SDL_QueryTexture(img, NULL, NULL, &w, &h); 
		SDL_Rect texr; 
		texr.x = 0; 
		texr.y = 0;
		texr.w = w;
		texr.h = h;
        image.texture = img;
        image.textureRect = texr;
        return image;
    }

    void debugLog(const std::string& debugString) override {
        OutputDebugStringA(debugString.c_str());
    }

    void renderImage(PlatformSpecificImage image) override {
        SDL_RenderCopy(renderer, image.texture, NULL, &image.textureRect);
    }

    void fillRectangle(MetalSlug::Rect &rect) override {
        SDL_Rect r;
        r.x = rect.x;
        r.y = rect.y;
        r.w = rect.width;
        r.h = rect.height;

        // Set render color to blue ( rect will be rendered in this color )
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

        // Render rect
        SDL_RenderFillRect(renderer, &r);
    }

    void drawRectangle(MetalSlug::Rect &rect) override {
        SDL_Rect r;
        r.x = rect.x;
        r.y = rect.y;
        r.w = rect.width;
        r.h = rect.height;

        // Set render color to blue ( rect will be rendered in this color )
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

        SDL_RenderDrawRect(renderer, &r);
    }
};
}