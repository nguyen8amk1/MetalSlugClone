#pragma once 
#include<string>
#include<vector>
#include "RebelSoilder.h"
#include "Bullet.h"
#include "Player.h"

namespace MetalSlug {
class RebelSoilder;
class Bullet;
class Player;

class GlobalGameData { 
	// TODO: this suppose to be a singleton, will contains everything that should be access globally 
	// and this suppose to replace the LevelData struct as well 
	// for now, it's gonna contain: 
	// bullets []
	// enemies []
	// and have coresponse methods to release them as well  
private: 
	std::vector<RebelSoilder*> rebelSoilders;
	std::vector<Bullet*> bullets;
	Player* player;

	GlobalGameData() {
	}

public: 
	static GlobalGameData* getInstance(); 

	std::vector<RebelSoilder*>* getRebelSoilders();

	std::vector<Bullet*>* getBullets();

	void setPlayer(Player* player);

	Player* getPlayer();
};

}