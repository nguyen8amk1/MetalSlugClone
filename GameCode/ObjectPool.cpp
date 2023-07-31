#include "ObjectPool.h"

namespace MetalSlug {
template<typename T> 
T* ObjectPool::getObject() {
	T *object = objects.pop_back();
	return object;
}

template<typename T> 
void ObjectPool::returnObject(T* object) {
	object->reset();
	objects.push_back(object);
}

}
