#pragma once 
#include<vector>
#include<cassert>
#include<string>
#include "MetalSlug.h"
#include "../SDL2PlatformMethodsCollection.cpp"

namespace MetalSlug {

struct AnimationMetaData {
	float animDelay;
	std::string tiledSheetFileName;
	Rect rect; 
	Vec2f relativeCorner;
	Vec2f framePixelSize;
	int rows, columns;
};

class Animation {
private:
	Rect rect; 
	int currentFrameIndex = 0;
	float timeAccumulator = 0.0f;
	float animDelay = 0.0f;
	bool flipX = false;
	bool flipY = false;

	PlatformSpecficMethodsCollection* platformMethods;
	PlatformSpecificImage *spriteSheet = NULL;
	std::vector<Rect> pixelRects;
	//std::vector<Rect> fittedNormalizedRects;

	
public: 
	Animation(AnimationMetaData &metaData, PlatformSpecficMethodsCollection *platformMethods) {
		this->platformMethods = platformMethods;
		this->animDelay = metaData.animDelay;
		this->rect = metaData.rect;

		spriteSheet = this->platformMethods->loadImage(metaData.tiledSheetFileName);

		for (int i = 0; i < metaData.rows; i++) {
			for (int j = 0; j < metaData.columns; j++) {
				Rect pixelRect;
				pixelRect.width = (float)(metaData.framePixelSize.x);
				pixelRect.height = (float)(metaData.framePixelSize.y);
				pixelRect.x = metaData.relativeCorner.x + (float)(j * pixelRect.width);
				pixelRect.y = metaData.relativeCorner.y + (float)(i * pixelRect.height);
				pixelRect.width = (float)fabs(metaData.framePixelSize.x);
				pixelRect.height = (float)fabs(metaData.framePixelSize.y);

				pixelRects.push_back(pixelRect);

				/*
				Rect fittedRect = {}; 
				fittingRect(pixelRect, fittedRect, rect);
				fittedNormalizedRects.push_back(fittedRect);
				*/
			}
		}
	}

	void animate(double dt) {

		timeAccumulator += dt;
		if (timeAccumulator >= animDelay) {
			++currentFrameIndex;
			currentFrameIndex %= pixelRects.size();

			timeAccumulator -= animDelay;
		}
		platformMethods->renderImagePortionAt(spriteSheet, pixelRects[currentFrameIndex], rect, flipX, flipY);
	}

	void changePos(float x, float y) {
		rect.x = x;
		rect.y = y;
	}

	void changeSize(float width, float height) {
		rect.width = width;
		rect.height = height;
	}

	void flip(int hDir, int vDir) {
		if (hDir == -1) {
			flipX = true;
		}
		else if (hDir == 1) {
			flipX = false;
		}

		if (vDir == -1) {
			flipY = true;
		}
		else if (vDir == 1) {
			flipY = false;
		}
	}
private: 
	/*
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
	*/

	~Animation() {
	}
};

}