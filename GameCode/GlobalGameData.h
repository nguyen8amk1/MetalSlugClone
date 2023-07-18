#pragma once 
#include<string>
#include<vector>
#include<assert.h>
#include<unordered_map>
#include "RebelSoilder.h"
#include "Hostage.h"
#include "Bullet.h"
#include "Player.h"

namespace MetalSlug {
class RebelSoilder;
class Bullet;
class Player;
class Hostage;

class GlobalGameData { 
	// TODO: this suppose to be a singleton, will contains everything that should be access globally 
	// and this suppose to replace the LevelData struct as well 
	// for now, it's gonna contain: 
	// bullets [X]
	// enemies [X]
	// hostages [X] 
	// and have coresponse methods to release them as well  

private: 
	std::vector<RebelSoilder*> rebelSoilders;
	std::vector<Bullet*> bullets;
	std::vector<Hostage*> hostages;
	Player* player;

	std::unordered_map<std::string, PlatformSpecificImage*> spriteSheets;

	GlobalGameData() {
	}

public: 
	static GlobalGameData* getInstance(); 

	std::vector<RebelSoilder*>* getRebelSoilders();

	std::vector<Bullet*>* getBullets();

	std::vector<Hostage*>* getHostages();

	void removeBulletAt(int index);

	void setPlayer(Player* player);

	Player* getPlayer();

	void addNewSpriteSheet(std::pair<std::string, std::string> spriteSheetInfo, PlatformSpecficMethodsCollection *platformMethods) {
		PlatformSpecificImage* spriteSheet = platformMethods->loadImage(spriteSheetInfo.second);
		assert(spriteSheet);
		spriteSheets[spriteSheetInfo.first] = spriteSheet;
	}

	PlatformSpecificImage* getSpriteSheet(const std::string &name) {
		return spriteSheets[name];
	}
};

}