#pragma once 
#include<string>
#include<vector>
#include<assert.h>
#include<unordered_map>
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
	// bullets [X]
	// enemies [X]
	// and have coresponse methods to release them as well  

private: 
	std::vector<RebelSoilder*> rebelSoilders;
	std::vector<Bullet*> bullets;

	// TODO: let's have a output of a hash map of (string, PlatformSpecificImage) in here 
	// with the input of a hash map (string, string) means file path and name 
	std::unordered_map<std::string, PlatformSpecificImage*> spriteSheets;

	/*
	PlatformSpecificImage* marcoRessiSpriteSheet;
	PlatformSpecificImage* rebelSoilderSpriteSheet;
	*/

	Player* player;

	GlobalGameData() {
	}

public: 
	static GlobalGameData* getInstance(); 

	std::vector<RebelSoilder*>* getRebelSoilders();

	std::vector<Bullet*>* getBullets();
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