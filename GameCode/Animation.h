#pragma once
#include "MetalSlug.h"
#include "Camera.h"

namespace MetalSlug {

struct AnimationMetaData {
	float animDelay;
	PlatformSpecificImage *spriteSheet;
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
    PlatformSpecificImage* spriteSheet = nullptr;
    std::vector<Rect> pixelRects;

	bool oneCycleFinish = false;

public:
    Animation(AnimationMetaData& metaData, PlatformSpecficMethodsCollection* platformMethods);
    void animate(Camera *camera, double dt);
    void changePos(float x, float y);
    void changeSize(float width, float height);
    void flip(int hDir, int vDir);
    void moveXBy(float d);
    void moveYBy(float d);
    void setRect(const Rect& r);
    bool finishOneCycle();
    Rect getRect();
    ~Animation();
};

}