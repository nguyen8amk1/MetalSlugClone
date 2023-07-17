#include "GlobalGameData.h"

namespace MetalSlug {

GlobalGameData* GlobalGameData::getInstance() {
	static GlobalGameData instance;
	return &instance;
}

std::vector<RebelSoilder*>* GlobalGameData::getRebelSoilders() {
	return &rebelSoilders;
}

std::vector<Bullet*>* GlobalGameData::getBullets() {
	return &bullets;
}

std::vector<Hostage*>* GlobalGameData::getHostages() {
	return &hostages;
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
	bullets.erase(bullets.begin() + index); // Deleting the fourth element
}

}
