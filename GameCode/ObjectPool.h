#pragma once
#include<vector>

namespace MetalSlug {
class PooledObject {
public: 
	virtual void reset() = 0;
};

class ObjectPool {
private: 
	std::vector<PooledObject*> objects;
public: 
	template<typename T> 
	T *getObject();
	template<typename T> 
	void returnObject(T *object);
};

}
