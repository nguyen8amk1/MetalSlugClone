#pragma once 
#include<vector>
#include<string>
#include "MetalSlug.h"
#include "../SDL2PlatformMethodsCollection.cpp"

namespace MetalSlug {

class Animation {
private:
	Rect rect; 
	std::vector<PlatformSpecificImage*> frames; 
	
	int currentFrameIndex = 0;
	float timeAccumulator = 0.0f;
	float animDelay = 0.0f;

	PlatformSpecficMethodsCollection* platformMethods;
	
public: 
	Animation(float animDelay, std::vector<std::string> &frameFiles, PlatformSpecficMethodsCollection *platformMethods, Rect &rect) {
		this->platformMethods = platformMethods;
		this->animDelay = animDelay;
		this->rect = rect;

		// loading the frames;
		for (std::string filename: frameFiles) {
			PlatformSpecificImage *img = this->platformMethods->loadImage(filename);

			img->setRect(rect);

			frames.push_back(img);
		}
	}

	Animation(float animDelay, const std::string &tiledSheetFileName, PlatformSpecficMethodsCollection *platformMethods, Rect &normRect, int rows, int columns) {
		this->platformMethods = platformMethods;
		this->animDelay = animDelay;
		this->rect = normRect;

		PlatformSpecificImage *img = this->platformMethods->loadImage(tiledSheetFileName);
		Rect pixelRect;

		int frameWidth = img->getPixelWidth()/columns;
		int frameHeight = img->getPixelHeight()/rows;

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < columns; j++) {
				pixelRect.width = (float)frameWidth;
				pixelRect.height = (float)frameHeight;
				pixelRect.x = (float)(j * pixelRect.width);
				pixelRect.y = (float)(i * pixelRect.height);

				PlatformSpecificImage* portion = img->getImagePortion(pixelRect);
				portion->setRect(rect);

				frames.push_back(portion);
			}
		}

	}

	void animate(double dt) {

		timeAccumulator += dt;
		if (timeAccumulator >= animDelay) {
			++currentFrameIndex %= frames.size();

			frames[currentFrameIndex]->setRect(rect);

			timeAccumulator -= animDelay;
		}

		platformMethods->renderImage(frames[currentFrameIndex]);
	}

	void changePos(float x, float y) {
		rect.x = x;
		rect.y = y;
	}

	void changeSize(float width, float height) {
		rect.width = width;
		rect.height = height;
	}

	~Animation() {
		for (PlatformSpecificImage *img: frames) {
			delete img;
		}
	}
};

}