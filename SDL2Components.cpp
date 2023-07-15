#pragma once 
#include<SDL.h>
#include<SDL_ttf.h>
#include<string>
#include<Windows.h>
#include"Util.h"
#include"SDL2Util.cpp"
#include"GameCode/MetalSlug.h"

namespace SDL2Platform {

struct SDL2GameText : public MetalSlug::GameText {
    SDL_Surface* surface;
    SDL_Texture* message;
    SDL_Rect pixelRect;

    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color textColor = {0, 255, 0};

    SDL2GameText(SDL_Renderer *renderer, int x, int y, int fontSize = 10, const std::string &text = "ditmesaigon") {
        // set the font: 
		std::string fontPath = "D:/Programming/Workspace/MetalSlugClone_DirectX/Assets/Fonts/Consolas.ttf";
		font = TTF_OpenFont(fontPath.c_str(), fontSize);

		if (!font) {
			OutputDebugStringA(Util::MessageFormater::print("TTF_OpenFont: ", TTF_GetError(), '\n').c_str());
			// handle error
            return;
		}
        this->renderer = renderer; 



		surface = TTF_RenderText_Blended(font, text.c_str(), textColor);
		// now you can convert it into a texture
		message = SDL_CreateTextureFromSurface(renderer, surface);

        pixelRect.x = x;
        pixelRect.y = y;
        pixelRect.w = surface->w;
        pixelRect.h = surface->h;
	}

    void setText(const std::string& text) override {
        SDL_Surface* updatedTextSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);

        // Free previous texture
        SDL_DestroyTexture(message);

        // Create a new texture from the updated surface
        message = SDL_CreateTextureFromSurface(renderer, updatedTextSurface);

        // Free previous surface
        SDL_FreeSurface(surface);

        // Set the updated surface
        surface = updatedTextSurface;

        pixelRect.w = surface->w;
        pixelRect.h = surface->h;
    }

    void setPixelPos(int x, int y) {
        pixelRect.x = x;
        pixelRect.y = y;
    }

    void clean() override {
		SDL_DestroyTexture(message);
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
    }
    
};

struct SDL2PlatformSpecificImage : MetalSlug::PlatformSpecificImage {
    SDL_Texture* texture;
    SDL_Rect textureRect;
    SDL_Renderer *renderer; 

    void setRect(MetalSlug::Rect &rect) override {
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(rect.x, rect.y, textureRect.x, textureRect.y);
        SDL2Util::NormalizeCoordConverter::normalizedSizeToPixelSize(rect.width, rect.height, textureRect.w, textureRect.h);
        SDL2Util::NormalizeCoordConverter::toMiddleOrigin(textureRect.x, textureRect.y, textureRect.w, textureRect.h);
    }

    void fitRect(MetalSlug::Rect &rect) override {
        float minWidthOrHeight = fmin(rect.width, rect.height);
        SDL2Util::NormalizeCoordConverter::normalizedCoordToPixelCoord(rect.x, rect.y, textureRect.x, textureRect.y);
        SDL2Util::NormalizeCoordConverter::normalizedSizeToPixelSize(minWidthOrHeight, minWidthOrHeight, textureRect.w, textureRect.h);
        SDL2Util::NormalizeCoordConverter::toMiddleOrigin(textureRect.x, textureRect.y, textureRect.w, textureRect.h);
    }

    int getPixelWidth() override {
        return textureRect.w;
    }

    int getPixelHeight() override {
        return textureRect.h;
    }

    float getGameWidth() override {
        return ((float)textureRect.w/304)*2.714;
    }

    float getGameHeight() override {
        return ((float)textureRect.h/224)*2;
    }


    MetalSlug::PlatformSpecificImage* getImagePortion(MetalSlug::Rect& pixelRect) {
        SDL_Rect imgPartRect;
        imgPartRect.x = (int)pixelRect.x;
        imgPartRect.y = (int)pixelRect.y;
        imgPartRect.w = (int)pixelRect.width;
        imgPartRect.h = (int)pixelRect.height;

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

}
