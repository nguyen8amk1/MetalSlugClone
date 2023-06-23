#pragma once 
#include<SDL.h>
#include<SDL_image.h>
#include<string> 

namespace SDL2Testing {
class SandBox {
public:
	int run() {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL initialization failed: %s", SDL_GetError());
            return -1;
        }

        // Create a window
        SDL_Window* window = SDL_CreateWindow("Prototype Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
        if (!window) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s", SDL_GetError());
            return -1;
        }

        // Create a renderer
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create renderer: %s", SDL_GetError());
            return -1;
        }

        // Load the image texture
        SDL_Texture* imageTexture = nullptr;
        std::string imagePath = "D:/Programming/Workspace/MetalSlugClone_DirectX/Assets/Imgs/sprites_cat_running_trans.png";  // Replace with the actual path to your image file
        SDL_Surface* imageSurface = IMG_Load(imagePath.c_str());
        if (!imageSurface) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load image: %s", IMG_GetError());
            return -1;
        }

        imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
        SDL_FreeSurface(imageSurface);
        if (!imageTexture) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture from image: %s", SDL_GetError());
            return -1;
        }

        // Rectangle position and size
        SDL_Rect rect;
        rect.x = 400;
        rect.y = 300;
        rect.w = 50;
        rect.h = 50;

        // Velocity variables
        float xVel = 0.0f;
        float yVel = 0.0f;

        // Image portion variables
        SDL_Rect srcRect;
        srcRect.x = 0;  // The top-left corner of the image portion
        srcRect.y = 0;
        srcRect.w = 512;  // The width and height of the image portion
        srcRect.h = 256;

        // Game loop
        bool quit = false;
        SDL_Event event;
        Uint32 currentTime = SDL_GetTicks();
        Uint32 previousTime = currentTime;
        float deltaTime = 0.0f;
        const float desiredFPS = 60.0f;
        const float frameDelay = 1000.0f / desiredFPS;

        while (!quit) {
            previousTime = currentTime;
            currentTime = SDL_GetTicks();
            deltaTime = (currentTime - previousTime) / 1000.0f;
            // Event handling
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
                else if (event.type == SDL_KEYDOWN) {
                    // Move the rectangle based on arrow key inputs
                    switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        yVel = -500.0f;
                        break;
                    case SDLK_DOWN:
                        yVel = 500.0f;
                        break;
                    case SDLK_LEFT:
                        xVel = -500.0f;
                        break;
                    case SDLK_RIGHT:
                        xVel = 500.0f;
                        break;
                    }
                }
                else if (event.type == SDL_KEYUP) {
                    // Stop the rectangle movement when a key is released
                    switch (event.key.keysym.sym) {
                    case SDLK_UP:
                    case SDLK_DOWN:
                        yVel = 0.0f;
                        break;
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        xVel = 0.0f;
                        break;
                    }
                }
            }

            // Update the rectangle position
            rect.x += static_cast<int>(xVel * deltaTime);
            rect.y += static_cast<int>(yVel * deltaTime);


            // Clear the screen
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderClear(renderer);

            SDL_Rect imageRect;
            imageRect.x = rect.x + (rect.w - srcRect.w) / 2;  // Center the image horizontally within the rectangle
            imageRect.y = rect.y + (rect.h - srcRect.h) / 2;  // Center the image vertically within the rectangle
            imageRect.w = srcRect.w;
            imageRect.h = srcRect.h;
            SDL_RenderCopy(renderer, imageTexture, &srcRect, &imageRect);

            // Draw the rectangle
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);

            // Update the screen
            SDL_RenderPresent(renderer);

            // Delay to cap the frame rate
            Uint32 frameEndTime = SDL_GetTicks();
            Uint32 frameTime = frameEndTime - currentTime;
            if (frameDelay > frameTime) {
                SDL_Delay(static_cast<Uint32>(frameDelay - frameTime));
            }
        }

        // Cleanup
        SDL_DestroyTexture(imageTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
	}
};
}