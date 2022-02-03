#include "include.h"

#define TB_SIZE 50

struct hash_node{
	void* data;
	char* key;
	hash_node_p next;
};

static bool isEqual(char* a, char* b){
	while(*a != '\0' && *b != '\0'){
		if(*(a++) != *(b++))	return false;
	}
	if(*a == *b)	return true;
	return false;
}

static int hash_function(char* str){
	unsigned long long int hash = 0;
	int c;
	while(c = *str++){
		hash = ((hash << 5) + hash)^c;
	}
	return hash;
}

static void hash_expand(htable_p ht){
	size_t new_size = (ht->size << 1);
	hash_node_p* new_tb = calloc(new_size,  sizeof(*new_tb)), n;
	for(int i = 0; i < ht->size; i++){
		n = ht->tb[i];
		while(n != NULL){
			int new_i = hash_function(n->key) % new_size;
			hash_node_p temp = n->next;
			n->next = new_tb[i];
			new_tb[i] = n;
			n = temp;
		}
	}
	free(ht->tb);
	ht->tb = new_tb;
	ht->size = new_size;
}

static void HASH_PRINT(htable_p ht){
	int count = 0;
	for(int i = 0; i < ht->size; i++){
		printf("%d\n", i);
		hash_node_p n = ht->tb[i];
		while(n != NULL){
			count++;
			printf("(%d)\tKey: \"%s\"\tPointer to data: %p\n", count, n->key, n->data);
			n = n->next;
		}
	}
}

static void* HASH_SEARCH(char* key, htable_p ht){
	if(ht->size == 0)	return NULL;
	int i = hash_function(key) % ht->size;
	hash_node_p n = ht->tb[i];
	while(!isEqual(key, n->key)){
		n = n->next;
		if(n == NULL)	return NULL;
	}
	return n->key;
}

static void HASH_INSERT(char* key, void* data, htable_p ht){
	if(ht->size == 0){
		ht->size = TB_SIZE;
		ht->tb = calloc(TB_SIZE, sizeof(*(ht->tb)));
	}
	double load_factor = (double) (ht->stored)/(ht->size);
	
	if(load_factor >= 0.8){
		hash_expand(ht);
	}

	int i = hash_function(key) % ht->size;
	hash_node_p n = malloc(sizeof(*n));
	n->data = data;
	n->key = key;
	n->next = ht->tb[i];
	ht->tb[i] = n;
	ht->stored++;
}

void HashTableInit(htable_p ht){
	ht->size = 0;
	ht->search = HASH_SEARCH;
	ht->insert = HASH_INSERT;
	ht->print = HASH_PRINT;
	ht->stored = 0;
	ht->tb = NULL;
}

void DelHTable(htable_p ht){
	for(int i = 0; i < ht->size; i++){
		hash_node_p n = ht->tb[i];
		while(n != NULL){
			hash_node_p temp = n->next;
			if(n->data != NULL)	free(n->data);
			free(n->key);
			free(n);
			n = temp;
		}
	}
	free(ht->tb);
	free(ht);
}