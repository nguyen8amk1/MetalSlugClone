#pragma once 
#include<SDL.h>
#include<SDL_ttf.h>
#include<string>
#include<Windows.h>
#include"Util.cpp"
#include"GameCode/MetalSlug.h"

namespace SDL2Platform {

struct SDL2GameText : public MetalSlug::GameText {
    SDL_Surface* surface;
    SDL_Texture* message;
    SDL_Rect pixelRect;

    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color textColor = { 255, 255, 255 };

    SDL2GameText(SDL_Renderer *renderer, int x, int y, int fontSize = 20, const std::string &text = "ditmesaigon") {
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
        // TODO: set the text right here 
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

}
