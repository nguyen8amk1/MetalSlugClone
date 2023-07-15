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

void GlobalGameData::setPlayer(Player *player) {
	this->player = player;
}

Player* GlobalGameData::getPlayer() {
	return player;
}

}
