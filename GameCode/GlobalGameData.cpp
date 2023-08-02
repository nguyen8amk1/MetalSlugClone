#include "GlobalGameData.h"

namespace MetalSlug {

GlobalGameData* GlobalGameData::getInstance() {
	static GlobalGameData instance;
	return &instance;
}

void GlobalGameData::init(PlatformSpecficMethodsCollection* platformMethods) {
	/************
	NOTE: Init need to be called in order to initialize first before global game data can be use 
	************/

	this->platformMethods = platformMethods;
	grenadeFactory = new GrenadeFactory(platformMethods);
}

std::vector<RebelSoilder*>* GlobalGameData::getRebelSoilders() {
	return &rebelSoilders;
}

void GlobalGameData::spawnRebelSoilder(Rect rebelColliderRect, float gravity, float tempSpeed, std::string initialState) {
	// TODO: this is just temporary, this never gonna be in my final game  
	if (initialState.compare("IDLING") == 0) {
		rebelSoilders.push_back(new RebelSoilder(gravity, tempSpeed, rebelColliderRect, platformMethods, RebelSoilder::AnimationState::IDLING));
	}
	else if (initialState.compare("THROWING_BOMB") == 0) {
		rebelSoilders.push_back(new RebelSoilder(gravity, tempSpeed, rebelColliderRect, platformMethods, RebelSoilder::AnimationState::THROWING_BOMB));
	}
}

void GlobalGameData::removeRebelSoilderAt(int index) {
	OutputDebugStringA(Util::MessageFormater::print("TODO: remove rebel soilder at index ", index, '\n').c_str());
}

std::vector<Bullet*>* GlobalGameData::getBullets() {
	return &bullets;
}

void GlobalGameData::spawnHostage(float gravity, float tempSpeed, Rect hostageColliderRect) {
	hostages.push_back(new Hostage(gravity, tempSpeed, hostageColliderRect, platformMethods));
}

void GlobalGameData::removeHostageAt(int index) {
	OutputDebugStringA(Util::MessageFormater::print("TODO: remove hostage at index ", index, '\n').c_str());
}

std::vector<Hostage*>* GlobalGameData::getHostages() {
	return &hostages;
}

std::vector<RectangleCollider*>* GlobalGameData::getGroundColliders() {
	return &groundColliders;
}

std::vector<Rect>* GlobalGameData::getDangerRects() {
	return &dangerRects;
}

std::vector<Grenade*>* GlobalGameData::getGrenades() {
	return &grenades;
}

void GlobalGameData::spawnGrenade(std::string role, int horizontalFacingDirection, float x, float y) {
	Grenade *grenade;
	if (role.compare("PLAYER") == 0) {
		grenade = grenadeFactory->createPlayerGrenade();
	}
	else if (role.compare("REBEL") == 0) {
		grenade = grenadeFactory->createRebelSoilderGrenade();
	}
	else {
		grenade = grenadeFactory->createPlayerGrenade();
		OutputDebugStringA("ERROR: grenade role not regcognize");
	}
	grenade->startThrow(horizontalFacingDirection, x, y);
	grenades.push_back(grenade);
}

void GlobalGameData::removeGrenadeAt(int index) {
	OutputDebugStringA(Util::MessageFormater::print("TODO: remove grenade at index ", index, '\n').c_str());
}

void GlobalGameData::setPlayer(Player *player) {
	this->player = player;
}

Player* GlobalGameData::getPlayer() {
	return player;
}

void GlobalGameData::removeBulletAt(int index) {
	/*
	Bullet *address = bullets[index];
	delete address;
	*/
	//bullets.erase(bullets.begin() + index); // Deleting the fourth element
	bullets.at(index)->reset();
}

void GlobalGameData::spawnBullet(float x, float y) {
	Bullet *bullet = new Bullet(platformMethods);
	bullet->shoot(x, y);
	bullets.push_back(bullet);
}

bool GlobalGameData::doesLevelStarted() { return levelStarted; }
void GlobalGameData::stopLevel() { levelStarted = false; }
void GlobalGameData::startLevel() { levelStarted = true; }
}
