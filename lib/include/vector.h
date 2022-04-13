#ifndef VECTOR_H
#define VECTOR_H

#include "../include.h"

struct vector{
	size_t size;
	size_t capacity;
	void** arr;
	struct vector (*add)(void* data, struct vector);
	struct vector (*remove)(size_t index, struct vector);
	size_t (*find)(void* data, bool (*isEqual)(void* d1, void*d2),struct vector);
};

struct vector VecInit();
struct vector addVec(struct vector v1, struct vector v2);
struct vector mergeVec(struct vector v1, struct vector v2, bool (*isEqual)(void* d1, void* d2), bool destroy);

#endif