#pragma once 
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<cassert>
#include<stdint.h>
#include <intrin.h>
#include "GameCode/MetalSlug.cpp"
#include "SDL2PlatformMethodsCollection.cpp"

#define SCREEN_WIDTH 304
#define SCREEN_HEIGHT 224
#define SCALE_FACTOR 3 
#define SCALED_SCREEN_WIDTH SCALE_FACTOR*SCREEN_WIDTH
#define SCALED_SCREEN_HEIGHT SCALE_FACTOR*SCREEN_HEIGHT

namespace SDL2Platform {

typedef int64_t uint64;
typedef double real64;
typedef float real32;

class Game {
public: 
	void toggleFullScreen(SDL_Window *window, bool fullScreen = false) {
		if (fullScreen) {
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		}
		else {
			SDL_SetWindowFullscreen(window, 0);
		}
	}

	int run() {
		bool fullScreen = false;

		// Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
			return -1;
		}

		if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL Image could not initialize! SDL_Error: %s\n", SDL_GetError());
			return -1;
		}


		// Create a window
		SDL_Window* window = SDL_CreateWindow("Game Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCALED_SCREEN_WIDTH, SCALED_SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (!window) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
			SDL_Quit();
			return -1;
		}
		toggleFullScreen(window, fullScreen);


		// Create a renderer
		SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (!renderer) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
			SDL_DestroyWindow(window);
			SDL_Quit();
			return -1;
		}

		SDL_Texture* frameTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
		if (!frameTexture) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Texture could not be created! SDL_Error: %s\n", SDL_GetError());
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
			return -1;
		}

		SDL2PlatformMethodsCollection *methodsCollection = new SDL2PlatformMethodsCollection();
		methodsCollection->renderer = renderer;

		MetalSlug::GameInputContext gameInput = {};
		MetalSlug::MetalSlug game(methodsCollection);

		// Game loop
		bool isRunning = true;

		const Uint8* keyboardState;

		uint64 PerfCountFrequency = SDL_GetPerformanceFrequency();
		uint64 LastCounter = SDL_GetPerformanceCounter();
		uint64 LastCycleCount = __rdtsc();

		int GameUpdateHz = 60;
		real32 TargetSecondsPerFrame = 1.0f / (real32)GameUpdateHz;
		MetalSlug::PlatformDebugInfo debugInfo = {};

		game.setup();
		game.platformDebugInfo = &debugInfo;

		SDL_Rect stretchedScreenRect;
		stretchedScreenRect.x = 0;
		stretchedScreenRect.y = 0;
		stretchedScreenRect.w = SCALED_SCREEN_WIDTH;  // Double the width of the frame texture
		stretchedScreenRect.h = SCALED_SCREEN_HEIGHT;  // Double the height of the frame texture

		while (isRunning) {
			// Handle events
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					isRunning = false;
				}
				else if (event.type == SDL_KEYDOWN) {
					if ((event.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) && event.key.keysym.sym == SDLK_RETURN) {
						fullScreen = !fullScreen;
						toggleFullScreen(window, fullScreen);
					}

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
				/*
				gameInput.up = updateButton(gameInput.up, keyboardState[SDL_SCANCODE_W]);
				gameInput.down = updateButton(gameInput.down, keyboardState[SDL_SCANCODE_S]);
				gameInput.left = updateButton(gameInput.left, keyboardState[SDL_SCANCODE_A]);
				gameInput.right = updateButton(gameInput.right, keyboardState[SDL_SCANCODE_D]);
				gameInput.jump = updateButton(gameInput.jump, keyboardState[SDL_SCANCODE_K]);
				gameInput.shoot = updateButton(gameInput.shoot, keyboardState[SDL_SCANCODE_J]);
				gameInput.throwGrenade = updateButton(gameInput.throwGrenade,  keyboardState[SDL_SCANCODE_U]);

				gameInput.upArrow = updateButton(gameInput.upArrow, keyboardState[SDL_SCANCODE_UP]);
				gameInput.downArrow = updateButton(gameInput.downArrow,  keyboardState[SDL_SCANCODE_DOWN]);
				gameInput.leftArrow = updateButton(gameInput.leftArrow,  keyboardState[SDL_SCANCODE_LEFT]);
				gameInput.rightArrow = updateButton(gameInput.rightArrow,  keyboardState[SDL_SCANCODE_RIGHT]);
				gameInput.enter = updateButton(gameInput.enter,  keyboardState[SDL_SCANCODE_RETURN]);

				gameInput.debugZoomInBackground = updateButton(gameInput.debugZoomInBackground,  keyboardState[SDL_SCANCODE_M]);
				gameInput.debugZoomOutBackground = updateButton(gameInput.debugZoomOutBackground,  keyboardState[SDL_SCANCODE_N]);
				*/

				gameInput.up.isDown = keyboardState[SDL_SCANCODE_W];
				gameInput.down.isDown = keyboardState[SDL_SCANCODE_S];
				gameInput.left.isDown = keyboardState[SDL_SCANCODE_A];
				gameInput.right.isDown = keyboardState[SDL_SCANCODE_D];
				gameInput.jump.isDown = keyboardState[SDL_SCANCODE_K];
				gameInput.shoot.isDown = keyboardState[SDL_SCANCODE_J];
				gameInput.throwGrenade.isDown = keyboardState[SDL_SCANCODE_U];
				gameInput.upArrow.isDown = keyboardState[SDL_SCANCODE_UP];
				gameInput.downArrow.isDown = keyboardState[SDL_SCANCODE_DOWN];
				gameInput.leftArrow.isDown = keyboardState[SDL_SCANCODE_LEFT];
				gameInput.rightArrow.isDown = keyboardState[SDL_SCANCODE_RIGHT];
				gameInput.enter.isDown = keyboardState[SDL_SCANCODE_RETURN];
				gameInput.debugZoomInBackground.isDown = keyboardState[SDL_SCANCODE_M];
				gameInput.debugZoomOutBackground.isDown = keyboardState[SDL_SCANCODE_N];

				// TODO: Handle Joystick
			}


			if (SDL2Util::TimeUtil::SDLGetSecondsElapsed(LastCounter, SDL_GetPerformanceCounter()) < TargetSecondsPerFrame)
			{
				int TimeToSleep = (int)((TargetSecondsPerFrame - SDL2Util::TimeUtil::SDLGetSecondsElapsed(LastCounter, SDL_GetPerformanceCounter())) * 1000) - 1;
				if (TimeToSleep > 0)
				{
					SDL_Delay(TimeToSleep);
				}

				//assert(SDLGetSecondsElapsed(LastCounter, SDL_GetPerformanceCounter()) < TargetSecondsPerFrame);
				while (SDL2Util::TimeUtil::SDLGetSecondsElapsed(LastCounter, SDL_GetPerformanceCounter()) < TargetSecondsPerFrame)
				{
					// Waiting...
				}
			}


			// Get this before SDLUpdateWindow() so that we don't keep missing VBlanks.
			int64_t EndCounter = SDL_GetPerformanceCounter();

			// Render
			uint64 EndCycleCount = __rdtsc();
			uint64 CounterElapsed = EndCounter - LastCounter;
			uint64 CyclesElapsed = EndCycleCount - LastCycleCount;

			real64 MSPerFrame = (((1000.0f * (real64)CounterElapsed) / (real64)PerfCountFrequency));
			real64 FPS = (real64)PerfCountFrequency / (real64)CounterElapsed;
			real64 MCPF = ((real64)CyclesElapsed / (1000.0f * 1000.0f));

			debugInfo.fps = FPS;
			debugInfo.frameTimeMillis = MSPerFrame;

			float secondsPerframe = MSPerFrame / 1000.0f;
			//OutputDebugStringA(Util::MessageFormater::print("Seconds perframe: ", secondsPerframe,  ", Millis per frame: ", MSPerFrame, ", FPS: ", FPS, "MCPF: ", MCPF, '\n').c_str());

			// Set the rendering target to the frame texture
			SDL_SetRenderTarget(renderer, frameTexture);


			// Clear the frame texture
			SDL_SetRenderDrawColor(renderer, 255, 0, 255, 0);
			SDL_RenderClear(renderer);


			game.updateAndRender(gameInput, secondsPerframe);

			SDL_SetRenderTarget(renderer, NULL);

			// Render the frame texture to the back buffer with stretching
			SDL_RenderCopy(renderer, frameTexture, NULL, &stretchedScreenRect);

			// Render the frame texture to the back buffer
			SDL_RenderPresent(renderer);


			LastCycleCount = EndCycleCount;
			LastCounter = EndCounter;
		}

		// Clean up
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		// Don't forget to free your surface and texture

		TTF_Quit();
		SDL_Quit();

		delete methodsCollection;

		return 0;
	}

	// Function to update the button state
	/*
	MetalSlug::Button updateButton(MetalSlug::Button& button, bool isKeyDown) {
		MetalSlug::Button newButton;
		newButton.wasDown = button.isDown;
		newButton.isDown = isKeyDown;

		// Check if the newButton was just pressed
		newButton.isPressed = newButton.isDown && !newButton.wasDown;
		return newButton;
	}
	*/

};

}


