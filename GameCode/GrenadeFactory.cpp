#include "GrenadeFactory.h"

namespace MetalSlug {
GrenadeAnimationContextFactory::GrenadeAnimationContextFactory() {
	 globalGameData = GlobalGameData::getInstance();
}

GrenadeAnimationContext GrenadeAnimationContextFactory::createPlayerGrenadeAnimationContext() {
	GrenadeAnimationContext context;
	context.grenadeWidth = .1f;
	context.grenadeHeight = .1f;
	PlatformSpecificImage* spriteSheet = globalGameData->getSpriteSheet("MARCO_ROSSI");
	Util::AnimationUtil::initAnimationMetaData(context.spinningAnimationMetaData, spriteSheet, .1f, 1, 1, { 0, 428 }, { 11, 18 }); 
	return context;
}

GrenadeAnimationContext GrenadeAnimationContextFactory::createRebelSoilderGrenadeAnimationContext() {
	GrenadeAnimationContext context;
	context.grenadeWidth = .2f;
	context.grenadeHeight = .2f;
	PlatformSpecificImage* spriteSheet = globalGameData->getSpriteSheet("REBEL_SOILDER");
	Util::AnimationUtil::initAnimationMetaData(context.spinningAnimationMetaData, spriteSheet, .1f, 1, 7, { 0, 385 }, {21, 24}); 
	return context;
}

// GRENADE FACTORY
GrenadeFactory::GrenadeFactory(PlatformSpecficMethodsCollection* platformMethods) {
	contextFactory = new GrenadeAnimationContextFactory();
	this->platformMethods = platformMethods;
}

Grenade* GrenadeFactory::createPlayerGrenade() {
	GrenadeAnimationContext grenadeAnimationContext = contextFactory->createPlayerGrenadeAnimationContext();
	Grenade* grenade = new Grenade(platformMethods, &grenadeAnimationContext);
	return grenade;
}

Grenade* GrenadeFactory::createRebelSoilderGrenade() {
	GrenadeAnimationContext grenadeAnimationContext = contextFactory->createRebelSoilderGrenadeAnimationContext();
	Grenade* grenade = new Grenade(platformMethods, &grenadeAnimationContext);
	return grenade;
}

}