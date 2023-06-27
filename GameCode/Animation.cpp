#pragma once 
#include<vector>
#include<cassert>
#include<string>
#include "../SDL2PlatformMethodsCollection.cpp"
#include "Animation.h"

namespace MetalSlug {

Animation::Animation(AnimationMetaData &metaData, PlatformSpecficMethodsCollection *platformMethods) {
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

void Animation::animate(double dt) {

	timeAccumulator += dt;
	if (timeAccumulator >= animDelay) {
		++currentFrameIndex;
		currentFrameIndex %= pixelRects.size();

		timeAccumulator -= animDelay;
	}
	platformMethods->renderImagePortionAt(spriteSheet, pixelRects[currentFrameIndex], rect, flipX, flipY);
}

void Animation::changePos(float x, float y) {
	rect.x = x;
	rect.y = y;
}

void Animation::changeSize(float width, float height) {
	rect.width = width;
	rect.height = height;
}

void Animation::flip(int hDir, int vDir) {
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

Animation::~Animation() {
}

}