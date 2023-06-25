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
	/*
	Animation(AnimationMetaData &metaData, PlatformSpecficMethodsCollection *platformMethods) {
		this->platformMethods = platformMethods;
		this->animDelay = metaData.animDelay;
		this->rect = metaData.animRect;
		spriteSheet = this->platformMethods->loadImage(metaData.spriteSheetFileName);
		this->pixelRects = metaData.framePixelRects;
		

		for (Rect &pixelRect: pixelRects) {
			// TODO: fitting pixelRects into the normalized rects 
			//.how to do it ?? 

			// NOTE: the conversion from pixel to normal shouldn't be done here though
			// this should be done else where, since the game shouldn't have any knowledge about the game resolution  
			// or it should ??. 
			// I don't know but currently we just fixed resolution 320x224 so that is the number i'm currently using   

			Rect fittedRect = {}; 
			fittingRect(pixelRect, fittedRect, rect);

			// NOTE: this is still not the fitted rect, in terms of fit in the smaller rect 
			// this is just normalized it but still not fitted

			fittedNormalizedRects.push_back(fittedRect);
		}
	}
	*/

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

		//frames[currentFrameIndex]->fitRect(rect); 
		//adjustedFrameRects[currentFrameIndex]-> // change the pos 
		//platformMethods->renderImage(frames[currentFrameIndex], flipX, flipY);

		// TODO: Steps: 
		// get the pixel rect of the portion that need to be render 
		// get the fitted size for the portion using the index -> already calculated in the constructor 
		//		-> fitRect(rect); -> return the game rect for the frame 
		// shove it into the renderImagePortionAt(); -> it suppose to take the porition of the big image with the normrect of the frame and render it right where it suppose to

		// TODO: the name should realy be renderImagePortionAtRect

		/*
		fittedNormalizedRects[currentFrameIndex].x = rect.x;
		fittedNormalizedRects[currentFrameIndex].y = rect.y;
		platformMethods->renderImagePortionAt(spriteSheet, pixelRects[currentFrameIndex], fittedNormalizedRects[currentFrameIndex], flipX, flipY);
		*/

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