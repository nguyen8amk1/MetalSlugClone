#pragma once
#include<queue>

namespace MetalSlug {
class PooledObject {
public: 
	virtual void reset() = 0;
};

class ObjectPool {
private: 
	std::queue<PooledObject*> objects;
public: 
	template<typename T> 
	T *getObject();
	template<typename T> 
	void returnObject(T *object);
};

}
