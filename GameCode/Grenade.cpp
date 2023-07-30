#include "Grenade.h"
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




Grenade::Grenade(PlatformSpecficMethodsCollection *platformMethods, GrenadeAnimationContext *grenadeAnimationContext) {
	xLerp = new TimeBoundedLerp(firstHopDuration/2.0f);
	yLerp = new TimeBoundedLerp(firstHopDuration/2.0f);
	colliderRect.width = grenadeAnimationContext->grenadeWidth;
	colliderRect.height = grenadeAnimationContext->grenadeHeight;
	this->platformMethods = platformMethods;

	globalGameData = GlobalGameData::getInstance();

	// TODO: should i inject the animations of the grenade into here or something else ??   
	//PlatformSpecificImage* spriteSheet = globalGameData->getSpriteSheet("MARCO_ROSSI");
	//Util::AnimationUtil::initAnimationMetaData(spinningAnimationMetaData, spriteSheet, .1f, 1, 1, {0, 428}, {11, 18}); //0, 428, 11, 18

	spinningAnimationMetaData = grenadeAnimationContext->spinningAnimationMetaData;
	spinningAnimation = new Animation(spinningAnimationMetaData, platformMethods);
}

void Grenade::startThrow(int facingDirection, float playerX, float playerY) {
	currentPhysicState = PhysicState::FIRST_HOP_UP;
	currentAnimationState = AnimationState::FIRST_HOP;
	currentAnimation = spinningAnimation;
	originalPos.x = playerX;
	originalPos.y = playerY;
	direction = facingDirection;
}

void Grenade::update(Camera *camera, double dt) {
	// TODO: Animation State Machine 

	//  Physic State Machine  
	switch (currentPhysicState) {
	case PhysicState::FIRST_HOP_UP: {
		xLerp->update(dt, false);
		yLerp->update(dt, false);

		//OutputDebugStringA(Util::MessageFormater::print(colliderRect.x, ", ", colliderRect.y, '\n').c_str());

		// event: 
		if (yLerp->getCurrentT() >= 1.0f) {
			xLerp->resetHard();
			yLerp->resetSmooth();

			currentPhysicState = PhysicState::FIRST_HOP_DOWN;
			originalPos.x = colliderRect.x;
			OutputDebugStringA("MOVE TO FIRST HOP DOWN\n");

		}
		else {
			float xd = Util::Math::lerp(0, howFarFirstHop / 2.0f, xLerp->getCurrentT());
			float yd = Util::Math::lerp(0, firstHopHeight, Util::Math::upCurve(yLerp->getCurrentT()));

			if (direction == -1) {
				xd = -xd;
			}

			colliderRect.x = originalPos.x + xd;
			colliderRect.y = originalPos.y + yd;
		}
	} break;

	case PhysicState::FIRST_HOP_DOWN: {
		xLerp->update(dt, false);
		yLerp->update(dt, false);

		bool hitGround = false;
		for (RectangleCollider *rect: *(globalGameData->getGroundColliders())) {
			hitGround = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, rect->getRect());
			if (hitGround) break;
		}

		if (hitGround) {
			xLerp->resetHard();
			yLerp->resetSmooth();

			xLerp->setDuration(secondHopDuration/2.0f);
			yLerp->setDuration(secondHopDuration/2.0f);

			currentPhysicState = PhysicState::SECOND_HOP_UP;

			originalPos.x = colliderRect.x;
			originalPos.y = colliderRect.y;

		}
		else {
			float xd = Util::Math::lerp(0, howFarFirstHop / 2.0f, xLerp->getCurrentT());
			float yd = Util::Math::lerp(0, firstHopHeight, Util::Math::downCurve(yLerp->getCurrentT()));

			if (direction == -1) {
				xd = -xd;
			}

			colliderRect.x = originalPos.x + xd;
			colliderRect.y = originalPos.y + yd;
		}

	} break;

	case PhysicState::SECOND_HOP_UP: {
		// action: 
		xLerp->update(dt, false);
		yLerp->update(dt, false);

		// event: 
		if (yLerp->getCurrentT() >= 1.0f) {
			xLerp->resetHard();
			yLerp->resetSmooth();

			currentPhysicState = PhysicState::SECOND_HOP_DOWN;
			originalPos.x = colliderRect.x;
		}
		else {
			float xd = Util::Math::lerp(0, howFarSecondHop / 2.0f, xLerp->getCurrentT());
			float yd = Util::Math::lerp(0, secondHopHeight, Util::Math::upCurve(yLerp->getCurrentT()));

			if (direction == -1) {
				xd = -xd;
			}

			colliderRect.x = originalPos.x + xd;
			colliderRect.y = originalPos.y + yd;
		}
	} break;

	case PhysicState::SECOND_HOP_DOWN: {
		xLerp->update(dt, false);
		yLerp->update(dt, false);

		bool hitGround = false;
		for (RectangleCollider *rect: *(globalGameData->getGroundColliders())) {
			hitGround = CollisionChecker::doesRectangleVsRectangleCollide(colliderRect, rect->getRect());
			if (hitGround) break;
		}

		if (hitGround) {
			xLerp->resetHard();
			yLerp->resetHard();

			currentPhysicState = PhysicState::NONE;

			originalPos.x = colliderRect.x;
			originalPos.y = colliderRect.y;
		}
		else {
			float xd = Util::Math::lerp(0, howFarSecondHop / 2.0f, xLerp->getCurrentT());
			float yd = Util::Math::lerp(0, secondHopHeight, Util::Math::downCurve(yLerp->getCurrentT()));

			if (direction == -1) {
				xd = -xd;
			}

			colliderRect.x = originalPos.x + xd;
			colliderRect.y = originalPos.y + yd;
		}

	} break;

	case PhysicState::ON_GROUND: {
		// TODO: 
		// action:
		// event: 
	} break;
	}
	
	if (currentAnimation) {
		currentAnimation->changePos(colliderRect.x, colliderRect.y);
		currentAnimation->changeSize(colliderRect.width, colliderRect.height);
		currentAnimation->animate(camera, dt);
	}

	Color c = { 255, 0, 255 };
	Rect r = camera->convertWorldRectToScreenRect(colliderRect);
	platformMethods->drawRectangle(r, c);

}


// GRENADE FACTORY
GrenadeFactory::GrenadeFactory(PlatformSpecficMethodsCollection* platformMethods) {
	contextFactory = new GrenadeAnimationContextFactory();
	this->platformMethods = platformMethods;
}

Grenade* GrenadeFactory::createPlayerGrenade() {
	GrenadeAnimationContext grenadeAnimationContext = contextFactory->createRebelSoilderGrenadeAnimationContext();
	Grenade* grenade = new Grenade(platformMethods, &grenadeAnimationContext);
	return grenade;
}

Grenade* GrenadeFactory::createRebelSoilderGrenade() {
	GrenadeAnimationContext grenadeAnimationContext = contextFactory->createRebelSoilderGrenadeAnimationContext();
	Grenade* grenade = new Grenade(platformMethods, &grenadeAnimationContext);
	return grenade;
}

}