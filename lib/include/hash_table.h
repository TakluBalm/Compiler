#ifndef HASHTAB_H
#define HASHTAB_H

#include <stddef.h>
#define TB_INIT_SIZE 50

typedef struct HashTable* htable_p;
typedef struct HashTable HashTable;
typedef struct hash_node* hash_node_p;

struct HashTable{
	size_t size;
	size_t stored;
	hash_node_p* tb;
	void (*insert)(char*, void*, htable_p);
	void* (*search)(char*, htable_p);
	void* (*remove)(char*, htable_p);
};

struct HashTable HashTableInit();
struct HashTable DelHTable(bool freeData, struct HashTable ht);

#endif