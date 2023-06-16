#pragma once

#include<string>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<Windows.h>
#include "SDL2Util.cpp"
#include "SDL2Components.cpp"


namespace SDL2Platform {

struct SDL2PlatformSpecificImage : MetalSlug::PlatformSpecificImage {
    SDL_Texture* texture;
    SDL_Rect textureRect;
    SDL_Renderer *renderer; 

    void setRect(MetalSlug::Rect &rect) override {
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(rect.x, rect.y, textureRect.x, textureRect.y);
        SDL2Util::NormalizeCoordConverter::normalizedSizeToPixelSize(rect.width, rect.height, textureRect.w, textureRect.h);
        SDL2Util::NormalizeCoordConverter::toMiddleOrigin(textureRect.x, textureRect.y, textureRect.w, textureRect.h);
    }

    int getPixelWidth() override {
        return textureRect.w;
    }

    int getPixelHeight() override {
        return textureRect.h;
    }


    MetalSlug::PlatformSpecificImage* getImagePortion(MetalSlug::Rect& pixelRect) {
        SDL_Rect imgPartRect;
        imgPartRect.x = pixelRect.x;
        imgPartRect.y = pixelRect.y;
        imgPartRect.w = pixelRect.width;
        imgPartRect.h = pixelRect.height;

        SDL2PlatformSpecificImage *image = new SDL2PlatformSpecificImage();
        SDL_Texture* myTexturePart = getAreaTexture(imgPartRect, renderer, texture);

        image->texture = myTexturePart;
        image->textureRect = imgPartRect;

        return image;
    }

private:
    SDL_Texture* getAreaTexture(SDL_Rect rect, SDL_Renderer* renderer, SDL_Texture* source)
    {
        SDL_Texture* result = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rect.w, rect.h);
        SDL_SetRenderTarget(renderer, result);
        SDL_SetTextureBlendMode(source, SDL_BLENDMODE_NONE); // @Fixme: not working, now the background turns white :))

        SDL_RenderCopy(renderer, source, &rect, NULL);

        // the folowing line should reset the target to default(the screen)
        //SDL_SetRenderTarget(renderer, NULL);
        // I also removed the RenderPresent funcion as it is not needed here      
        return result;
    }
};

class SDL2PlatformMethodsCollection: public MetalSlug::PlatformSpecficMethodsCollection {
public: 
    SDL_Renderer* renderer;
    SDL_Texture* frameTexture;

public: 

    SDL2PlatformMethodsCollection() {
		if (TTF_Init() == -1)
		{
			std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
			OutputDebugStringA(Util::MessageFormater::print("SDL_ttf could not initialize! TTF_Error: ", TTF_GetError(),  '\n').c_str());
			return;
		}

    }

    MetalSlug::PlatformSpecificImage* loadImage(const std::string& filename) override {
        // TODO: load image using SDL2 methods
        SDL2PlatformSpecificImage *image = new SDL2PlatformSpecificImage();

		SDL_Texture* img = IMG_LoadTexture(renderer, filename.c_str());
		int w, h;
		SDL_QueryTexture(img, NULL, NULL, &w, &h); 
		SDL_Rect texr; 
		texr.x = 0; 
		texr.y = 0;
		texr.w = w;
		texr.h = h;

        image->texture = img;
        image->textureRect = texr;
        image->renderer = renderer; 

        return image;
    }

    void debugLog(const std::string& debugString) override {
        OutputDebugStringA(debugString.c_str());
    }

    void renderImage(MetalSlug::PlatformSpecificImage *image) override {
        SDL2PlatformSpecificImage *img = (SDL2PlatformSpecificImage*) image;
        SDL_RenderCopy(renderer, img->texture, NULL, &img->textureRect);
    }

    void fillRectangle(MetalSlug::Rect &normRect) override {
        SDL_Rect r;
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(normRect.x, normRect.y, r.x, r.y);
        SDL2Util::NormalizeCoordConverter::normalizedSizeToPixelSize(normRect.width, normRect.height, r.w, r.h);
        SDL2Util::NormalizeCoordConverter::toMiddleOrigin(r.x, r.y, r.w, r.h);

        // Set render color to blue ( rect will be rendered in this color )
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

        // Render rect
        SDL_RenderFillRect(renderer, &r);
    }

    void drawRectangle(MetalSlug::Rect &normRect) override {
        SDL_Rect r;
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(normRect.x, normRect.y, r.x, r.y);
        SDL2Util::NormalizeCoordConverter::normalizedSizeToPixelSize(normRect.width, normRect.height, r.w, r.h);
        SDL2Util::NormalizeCoordConverter::toMiddleOrigin(r.x, r.y, r.w, r.h);

        // Set render color to blue ( rect will be rendered in this color )
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

        SDL_RenderDrawRect(renderer, &r);
    }

    MetalSlug::GameText* createText(int x, int y) override {
        SDL2GameText* text = new SDL2GameText(renderer, x, y);
        return text;
    }

    void drawText(MetalSlug::GameText *text) {
        SDL2GameText* txt = (SDL2GameText*) text;
		SDL_RenderCopy(renderer, txt->message, NULL, &txt->pixelRect);
    }
};

}