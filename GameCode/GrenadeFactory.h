#pragma once 
#include "Animation.h"
#include "GlobalGameData.h"

namespace MetalSlug {
class Grenade;
class GlobalGameData;
struct GrenadeAnimationContext {
	float grenadeWidth, grenadeHeight;
	AnimationMetaData spinningAnimationMetaData;
};
	
class GrenadeAnimationContextFactory {
private:
	GlobalGameData* globalGameData;
public:
	GrenadeAnimationContextFactory();
	GrenadeAnimationContext createPlayerGrenadeAnimationContext();
	GrenadeAnimationContext createRebelSoilderGrenadeAnimationContext();
};

class GrenadeFactory {
private: 
	GrenadeAnimationContextFactory* contextFactory;
	PlatformSpecficMethodsCollection* platformMethods;
public:
	GrenadeFactory(PlatformSpecficMethodsCollection* platformMethods);
	Grenade* createPlayerGrenade();
	Grenade* createRebelSoilderGrenade();
};

}