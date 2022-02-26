#include "include.h"

struct HashTable SYMBOL_BUFF;
struct HashTable SYMBOL_TABLE;
size_t MAX_TERMINAL_LEN = 0;
bool INITIALISED = false;

struct vector{
	size_t size;
	size_t capacity;
	ast_node** arr;
};

struct vector addNode(ast_node* node, struct vector vec){
	if(vec.capacity <= vec.size){
		if(vec.capacity == 0){
			vec.arr = malloc(sizeof(ast_node*));
			vec.capacity = 1;
		}else{
			vec.arr = realloc(vec.arr, 2*vec.capacity*sizeof(ast_node*));
			vec.capacity = 2*vec.capacity;
		}
	}
	vec.arr[vec.size++] = node;
	return vec;
}

void removeNodeAtIndex(size_t index, struct vector vec){
	if(index == vec.size - 1){
		vec.size--;
		return;
	}
	for(int i = index+1; i < vec.size; i++){
		vec.arr[i - 1] = vec.arr[i];
	}
	vec.size--;
}

size_t findNode(ast_node* node, struct vector vec){
	size_t i;
	for(i = 0; i < vec.size; i++){
		if(vec.arr[i] == node)	break;
	}
	return i;
}

void findAndRemove(ast_node* node, struct vector vec){
	size_t index = findNode(node, vec);
	if(index >= vec.size)	return;
	removeNodeAtIndex(index, vec);
}

struct vector mergeVec(struct vector v1, struct vector v2){
	if(v1.arr == NULL || v2.arr == NULL){
		return (v1.arr == NULL)?v2:v1;
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
		v.arr = calloc(v.capacity, sizeof(ast_node*));
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

static bool isEqual(char* a, char* b){
	while(*a != '\0' && *b != '\0'){
		if(*(a++) != *(b++))	return false;
	}
	if(*a == *b)	return true;
	return false;
}

static bool eq(tok_p tok1, tok_p tok2){
	if(tok1->type != tok2->type)	return false;
	isEqual(tok1->value, tok2->value);
}

void addSymbol(ast_node* node){

	if(!INITIALISED){
		HashTableInit(&SYMBOL_BUFF);
		HashTableInit(&SYMBOL_TABLE);
		INITIALISED = true;
	}

	if(node->type == LEAF){
		struct symbol* sym = malloc(sizeof(*sym));
		tok_p token = node->token;
		sym->token = token;

		switch(token->type){
			case TERMINAL:{
				if(SYMBOL_TABLE.search(token->value, &SYMBOL_TABLE) == NULL){

					sym->length = 0;
					for(sym->length = 0; token->value[sym->length] != '\0'; sym->length++);
					if(sym->length > MAX_TERMINAL_LEN){
						MAX_TERMINAL_LEN = sym->length;
					}

					SYMBOL_TABLE.insert(token->value, sym, &SYMBOL_TABLE);
					return;
				}
				free(sym);
				return;
			}
			case NON_TERMINAL:{
				if(SYMBOL_TABLE.search(token->value, &SYMBOL_TABLE) == NULL){
					if(SYMBOL_BUFF.search(token->value, &SYMBOL_BUFF) == NULL){
						SYMBOL_BUFF.insert(token->value, sym, &SYMBOL_BUFF);
						return;
					}
				}
				free(sym);
				return;
			}
			default:{
				free(sym);
				return;
			}
		}
	}
}

bool defineNT(ast_node* rule){

	if(!INITIALISED){
		HashTableInit(&SYMBOL_BUFF);
		HashTableInit(&SYMBOL_TABLE);
		INITIALISED = true;
	}

	tok_p token = rule->children[0]->token;
	char* key = token->value;	
	struct symbol* sym = SYMBOL_BUFF.remove(key, &SYMBOL_BUFF);

	if(sym != NULL){
		sym->rule = rule;
		sym->token = token;
		SYMBOL_TABLE.insert(key, sym, &SYMBOL_TABLE);
		return true;
	}

	sym = SYMBOL_TABLE.search(key, &SYMBOL_TABLE);
	if(sym != NULL){
		PARSE_ERR.token = token;
		if(PARSE_ERR.msg != NULL)	free(PARSE_ERR.msg);
		PARSE_ERR.msg = calloc(100, sizeof(char));
		sprintf(PARSE_ERR.msg, "Double definition of Non-Terminal <%s>. Defined earlier at %llu:%llu\n", token->value, sym->token->lineNum, sym->token->characterNum);
	}
}

struct symbol* searchSymbol(char* key){

	if(!INITIALISED){
		HashTableInit(&SYMBOL_BUFF);
		HashTableInit(&SYMBOL_TABLE);
		INITIALISED = true;
	}

	return SYMBOL_TABLE.search(key, &SYMBOL_TABLE);
}

struct vector getElementsOfType(int type, bool returnFirstOccurence, ast_node* root){
	struct vector vec;
	vec.size = 0; vec.capacity = 0; vec.arr = NULL;
	if(root->type == type){
		vec = addNode(root, vec);
		if(returnFirstOccurence)	return vec;
	}
	for(int i = 0; i < root->numChild; i++){
	vec = mergeVec(vec, getElementsOfType(type, returnFirstOccurence, root->children[i]));
	}
	return vec;
}

bool checkSymbols(ast_node* root){
	
	if(!INITIALISED){
		HashTableInit(&SYMBOL_BUFF);
		HashTableInit(&SYMBOL_TABLE);
		INITIALISED = true;
	}
	
	
	struct vector rules = getElementsOfType(RULE, true, root);
	for(int i = 0; i < rules.size; i++){
		ast_node* rule = rules.arr[i];
		struct vector leafs = getElementsOfType(LEAF, true, rule);
		for(int j = 0; j < leafs.size; j++){
			addSymbol(leafs.arr[j]);
		}
		free(leafs.arr);
		if(!defineNT(rule)){
			return false;
		}
	}
	
	for(int i = 0; i < SYMBOL_BUFF.size; i++){
		if(SYMBOL_BUFF.tb[i] != NULL){
			struct symbol* sym = *((void**)SYMBOL_BUFF.tb[i]);
			PARSE_ERR.token = sym->token;
			if(PARSE_ERR.msg != NULL)	free(PARSE_ERR.msg);
			calloc(100,sizeof(char));
			sprintf(PARSE_ERR.msg, "The NON_TERMINAL <%s> has been declared but not defined\n", sym->token->value);
			DelHTable(true, &SYMBOL_BUFF);
			return false;
		}
	}
	DelHTable(true, &SYMBOL_BUFF);
	return true;
}
