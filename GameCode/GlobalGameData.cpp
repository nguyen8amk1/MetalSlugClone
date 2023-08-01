#include "GlobalGameData.h"

namespace MetalSlug {

GlobalGameData* GlobalGameData::getInstance() {
	static GlobalGameData instance;
	return &instance;
}

std::vector<RebelSoilder*>* GlobalGameData::getRebelSoilders() {
	return &rebelSoilders;
}

void GlobalGameData::spawnRebelSoilder(Rect rebelColliderRect, float gravity, float tempSpeed) {
	rebelSoilders.push_back(new RebelSoilder(gravity, tempSpeed, rebelColliderRect, platformMethods));
}

void GlobalGameData::removeRebelSoilderAt(int index) {
	OutputDebugStringA(Util::MessageFormater::print("TODO: remove rebel soilder at index ", index, '\n').c_str());
}

std::vector<Bullet*>* GlobalGameData::getBullets() {
	return &bullets;
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
