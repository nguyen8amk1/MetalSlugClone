#pragma once
#include "MetalSlug.h"

namespace MetalSlug {

struct AnimationMetaData {
	float animDelay;
	std::string tiledSheetFileName;
	Rect rect; 
	Vec2f relativeCorner;
	Vec2f framePixelSize;
	int rows, columns;
};

class Animation: public CameraControlledEntity {
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

public:
    Animation(AnimationMetaData& metaData, PlatformSpecficMethodsCollection* platformMethods);
    void animate(double dt);
    void changePos(float x, float y);
    void changeSize(float width, float height);
    void flip(int hDir, int vDir);
    void moveXBy(float d) override;
    void moveYBy(float d) override;
    void setRect(const Rect& r);

    ~Animation();
};

}