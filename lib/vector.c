#include "include.h"


static struct vector VECTOR_Add(void* data, struct vector vec){
	if(vec.capacity <= vec.size){
		if(vec.capacity == 0){
			vec.arr = malloc(sizeof(void*));
			vec.capacity = 1;
		}else{
			vec.arr = realloc(vec.arr, 2*vec.capacity*sizeof(void*));
			vec.capacity = 2*vec.capacity;
		}
	}
	vec.arr[vec.size++] = data;
	return vec;
}

static struct vector VECTOR_Remove(size_t index, struct vector vec){
	if(index == vec.size - 1){
		vec.size--;
		return vec;
	}
	for(int i = index+1; i < vec.size; i++){
		vec.arr[i - 1] = vec.arr[i];
	}
	vec.size--;
	return vec;
}

static size_t VECTOR_Find(void* data, bool (*isEqual)(void* d1, void*d2), struct vector vec){
	size_t i;
	for(i = 0; i < vec.size; i++){
		if(isEqual(vec.arr[i], data))	break;
	}
	return i;
}

struct vector VecInit(){
	struct vector v;
	v.size = v.capacity = 0;
	v.arr = NULL;
	v.add = VECTOR_Add;
	v.find = VECTOR_Find;
	v.remove = VECTOR_Remove;
	return v;
}

struct vector addVec(struct vector v1, struct vector v2){
	if(v1.arr == NULL || v2.arr == NULL){
		return (v1.arr == NULL)?v2:v1;
	}
	if(v1.arr == v2.arr){
		printf("Trying to add same vectors\n");
		return v1;
	}
	struct vector v;
	if(v1.capacity >= v1.size + v2.size || v2.capacity >= v1.size + v2.size){
		struct vector filler;
		if(v1.capacity >= v1.size + v2.size){
			v = v1;
			filler = v2;
		}else{
			v = v2;
			filler = v1;
		}
		for(int i = 0; i < filler.size; i++){
			v.arr[i + v.size] = filler.arr[i];
		}
		free(filler.arr);
		return v;
	}else{
		v.size = v1.size + v2.size;
		v.capacity = v1.capacity + v2.capacity;
		v.arr = calloc(v.capacity, sizeof(void*));
		struct vector smallV = (v1.size < v2.size)?v1:v2;
		struct vector bigV = (smallV.arr == v1.arr)?v2:v1;
		for(int i = 0; i < bigV.size; i++){
			if(i < smallV.size)	v.arr[i] = smallV.arr[i];
			v.arr[i + smallV.size] = bigV.arr[i];
		}
		free(v1.arr);
		free(v2.arr);
		return v;
	}
}

struct vector mergeVec(struct vector v1, struct vector v2, bool (*isEqual)(void* d1, void* d2), bool destroy){
	if(v1.arr == NULL || v2.arr == NULL){	//	One is a NULL vector
		return (v1.arr == NULL)?v2:v1;
	}
	if(v1.arr == v2.arr){	// Both vectors are same
		return v1;
	}
	struct vector v = VecInit();
	if(v1.capacity >= v1.size + v2.size || v2.capacity >= v1.size + v2.size){	// Merge one vector into the other one
		struct vector filler;
		if(v1.capacity >= v1.size + v2.size){
			v = v1;
			filler = v2;
		}else{
			v = v2;
			filler = v1;
		}
		for(int i = 0; i < filler.size; i++){
			if(v.find(filler.arr[i], isEqual, v) >= v.size){
				v = v.add(filler.arr[i], v);
			}
		}
		if(destroy)	free(filler.arr);
		return v;
	}else{		// Free both the vectors and return a new one
		v.capacity = v1.capacity + v2.capacity;
		v.arr = calloc(v.capacity, sizeof(void*));
		for(int i = 0; i < v1.size; i++){
			if(v.find(v1.arr[i], isEqual, v) >= v.size){
				v = v.add(v1.arr[i], v);
			}
		}
		for(int i = 0; i < v2.size; i++){
			if(v.find(v2.arr[i], isEqual, v) >= v.size){
				v = v.add(v2.arr[i], v);
			}
		}
		if(destroy){
			free(v1.arr);
			free(v2.arr);
		}
		return v;
	}
}
