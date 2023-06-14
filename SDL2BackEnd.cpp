#pragma once 
#include<SDL.h>
#include "GameCode/MetalSlug.cpp"
#include "SDL2PlatformMethodsCollection.cpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

namespace SDL2Platform {

int run() {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Game Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Timing variables
    /*
    Uint32 previousFrameTime = SDL_GetTicks();
    float deltaTime = 0.0f;
    */

    SDL2PlatformMethodsCollection *methodsCollection = new SDL2PlatformMethodsCollection();
    methodsCollection->renderer = renderer;

    MetalSlug::GameInputContext gameInput = {};

    MetalSlug::MetalSlug game(methodsCollection);

    // Game loop
    bool isRunning = true;

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;
    const int FPS = 60;
    const double targetFrameTime = 1000.0 / FPS; // 60 FPS

    const Uint8* keyboardState;
    while (isRunning) {
        // TODO: make the game input working
        // ... 

        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                // Handle key press
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    // Handle up key press
                    break;
                }
            }
            else if (event.type == SDL_KEYUP) {
                // Handle key release
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    // Handle up key release
                    isRunning = false;
                    break;
                }
            }

            // Get keyboard state
            keyboardState = SDL_GetKeyboardState(NULL);
            gameInput.moveUp = keyboardState[SDL_SCANCODE_UP];
            gameInput.moveDown = keyboardState[SDL_SCANCODE_DOWN];
            gameInput.moveLeft = keyboardState[SDL_SCANCODE_LEFT];
            gameInput.moveRight = keyboardState[SDL_SCANCODE_RIGHT];
        }

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = ((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());


        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        game.updateAndRender(gameInput, deltaTime);

        SDL_RenderPresent(renderer);

        // Cap the frame rate
        if (deltaTime < targetFrameTime) {
            double delayTime = targetFrameTime - deltaTime;
            SDL_Delay((Uint32)delayTime);
        }
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    delete methodsCollection;

    return 0;
}

}





int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	return SDL2Platform::run();
}
