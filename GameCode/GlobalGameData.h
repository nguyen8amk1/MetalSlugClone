#pragma once 
#include<string>
#include<vector>
#include<assert.h>
#include<unordered_map>

#include "MetalSlug.h"
#include "RebelSoilder.h"
#include "Hostage.h"
#include "Bullet.h"
#include "Player.h"
#include "Grenade.h"
#include "GrenadeFactory.h"

namespace MetalSlug {
class RebelSoilder;
class Bullet;
class Player;
class Hostage;
class GrenadeFactory;
class Grenade;
class RectangleCollider;

class GlobalGameData { 
	// for now, it's gonna contain: 
	// bullets [X]
	// enemies [X]
	// hostages [X] 
	// grenades [X]
	// ground colliders [X]
	// danger rects [X]
	// level started [X]
	// and have coresponse methods to release them as well  
 private: 
	// TODO: these vectors need to be an object pool
		// Test this on the rebel soilders first 
	PlatformSpecficMethodsCollection* platformMethods;
	std::vector<RebelSoilder*> rebelSoilders;
	std::vector<Bullet*> bullets;
	std::vector<Hostage*> hostages;
	std::vector<RectangleCollider*> groundColliders;
	std::vector<Rect> dangerRects;
	std::vector<Grenade*> grenades;

	bool levelStarted = false;
	Player* player;

	GrenadeFactory *grenadeFactory;
	std::unordered_map<std::string, PlatformSpecificImage*> spriteSheets;

	GlobalGameData() {
	}

public: 
	static GlobalGameData* getInstance(); 

	void init(PlatformSpecficMethodsCollection* platformMethods);
	void spawnRebelSoilder(Rect rebelColliderRect, float gravity, float tempSpeed); 
	void removeRebelSoilderAt(int index);
	std::vector<RebelSoilder*>* getRebelSoilders();

	void spawnHostage(float x, float y);
	void removeHostageAt(int index);
	std::vector<Hostage*>* getHostages();

	void spawnGrenade(std::string role, int facingDirection, float x, float y);
	void removeGrenadeAt(int index);
	std::vector<Grenade*> *getGrenades();

	std::vector<RectangleCollider*> *getGroundColliders();

	bool doesLevelStarted(); 
	void stopLevel(); 
	void startLevel();

	std::vector<Rect> *getDangerRects();

	std::vector<Bullet*>* getBullets();
	void spawnBullet(float x, float y);
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