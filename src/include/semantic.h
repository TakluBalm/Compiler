#ifndef SYMBOL_H
#define SYMBOL_H

#include "../include.h"

extern struct HashTable SYMBOL_TABLE;
extern size_t MAX_TERMINAL_LEN;

struct symbol{
	union{
		ast_node* rule;
		size_t length;
	};
	tok_p token;
	struct vector first;
	size_t state;
};

void addSymbol(ast_node* node);
struct symbol* searchSymbol(tok_p token);
bool checkSymbols(ast_node* root);
struct vector FIRST(tok_p token);
struct vector getElementsOfType(int type, bool returnFirstOccurence, ast_node* root);

#endif