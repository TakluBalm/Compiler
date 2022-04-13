#include "include.h"

struct HashTable SYMBOL_BUFF;
struct HashTable SYMBOL_TABLE;
size_t MAX_TERMINAL_LEN = 0;
bool INITIALISED = false;

size_t NUM_TERM = 0;
size_t NUM_NTERM = 0;


static inline bool eq(struct symbol* s1, struct symbol* s2){
	return s1 == s2;
}

void addSymbol(ast_node* node){

	if(!INITIALISED){
		SYMBOL_BUFF = SYMBOL_TABLE = HashTableInit();
		INITIALISED = true;
	}

	if(node->type == LEAF){
		struct symbol* sym = malloc(sizeof(*sym));
		tok_p token = node->token;
		sym->token = token;
		sym->first = VecInit();

		switch(token->type){
			case TERMINAL:{
				if(SYMBOL_TABLE.search(token->value, &SYMBOL_TABLE) == NULL){

					sym->length = 0;
					for(sym->length = 0; token->value[sym->length] != '\0'; sym->length++);
					if(sym->length > MAX_TERMINAL_LEN){
						MAX_TERMINAL_LEN = sym->length;
					}

					sym->state = NUM_TERM++;

					SYMBOL_TABLE.insert(token->value, sym, &SYMBOL_TABLE);
					return;
				}
				free(sym);
				return;
			}
			case NON_TERMINAL:{
				if(SYMBOL_TABLE.search(token->value, &SYMBOL_TABLE) == NULL){
					if(SYMBOL_BUFF.search(token->value, &SYMBOL_BUFF) == NULL){
						sym->state = NUM_NTERM;
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
		SYMBOL_BUFF = SYMBOL_TABLE = HashTableInit();
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

struct symbol* searchSymbol(tok_p keyTok){

	if(!INITIALISED){
		SYMBOL_BUFF = SYMBOL_TABLE = HashTableInit();
		INITIALISED = true;
	}

	return SYMBOL_TABLE.search(keyTok->value, &SYMBOL_TABLE);
}

struct vector getElementsOfType(int type, bool returnFirstOccurence, ast_node* root){
	struct vector vec = VecInit();
	if(root->type == type){
		vec = vec.add(root, vec);
		if(returnFirstOccurence)	return vec;
	}
	if(root->type == LEAF)	return vec;
	for(int i = 0; i < root->numChild; i++){
		vec = addVec(vec, getElementsOfType(type, returnFirstOccurence, root->children[i]));
	}
	return vec;
}

bool checkSymbols(ast_node* root){
	
	if(!INITIALISED){
		SYMBOL_BUFF = SYMBOL_TABLE = HashTableInit();
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
			PARSE_ERR.msg = calloc(100,sizeof(char));
			sprintf(PARSE_ERR.msg, "The NON_TERMINAL <%s> has been declared but not defined\n", sym->token->value);
			SYMBOL_BUFF = DelHTable(true, SYMBOL_BUFF);
			return false;
		}
	}
	SYMBOL_BUFF = DelHTable(true, SYMBOL_BUFF);
	return true;
}

struct vector FIRST(tok_p token){
	struct symbol* sym = searchSymbol(token);
	if(sym->first.arr == (void**)sym)	return VecInit();
	else if(sym->first.arr != NULL)		return sym->first;
	sym->first.arr = (void*)sym;

	struct vector term_lists = getElementsOfType(LIST, true, sym->rule->children[2]);

	struct vector first = VecInit();
	for(int i = 0; i < term_lists.size; i++){
		ast_node* list = term_lists.arr[i];
		tok_p start = list->children[0]->children[0]->token;
		switch(start->type){
			case TERMINAL:{
				struct symbol* start_sym = searchSymbol(start);
				size_t index = first.find(start_sym, eq, first);
				if(index >= first.size) first = first.add(start_sym, first);
				break;
			}
			case NON_TERMINAL:{
				first = mergeVec(first, FIRST(start), eq, false);
				break;
			}
		}
	}
	sym->first = first;
	return first;
}