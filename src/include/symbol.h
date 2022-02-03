#ifndef SYMBOL_H
#define SYMBOL_H

#include "../include.h"

typedef struct SYMBOL* sym_p;
typedef htable_p symtab_p;
typedef HashTable SymbolTable;


struct SYMBOL{
	char* name;
	enum{
		TERMINAL,
		NON_TERMINAL
	} type;
	unsigned int vertex;
};

sym_p GetSym();
void SymTabInit(symtab_p st);
void SymInsert(symtab_p st, sym_p s);
sym_p SymSearch(symtab_p st, const char* name);

#endif