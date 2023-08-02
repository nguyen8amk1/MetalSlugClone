#include "ObjectPool.h"

namespace MetalSlug {
template<typename T> 
T* ObjectPool::getObject() {
	T *object;
	if (objects.empty()) {
		object = new T();
	}
	else {
		object = objects.pop();
	}
	return object;
}

template<typename T> 
void ObjectPool::returnObject(T* object) {
	object->reset();
	objects.push(object);
}

}
