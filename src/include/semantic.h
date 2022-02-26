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
};

void addSymbol(ast_node* node);
struct symbol* searchSymbol(char* token);
bool checkSymbols(ast_node* root);

#endif