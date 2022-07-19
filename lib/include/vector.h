#ifndef VECTOR_H
#define VECTOR_H

#include "../include.h"

struct vector{
	size_t size;
	size_t capacity;
	void** arr;
	struct vector (*add)(void* data, struct vector);
	struct vector (*remove)(size_t index, struct vector);
	bool (*_comparator)(void*, void*);
	size_t (*find)(void* data ,struct vector);
};

struct vector VecInit();
void setComparator(struct vector* vec, bool (*_comparator)(void*, void*));
struct vector addVec(struct vector v1, struct vector v2);
struct vector mergeVec(struct vector v1, struct vector v2, bool destroy);

#endif