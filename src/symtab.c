#ifndef DEBUG
#define DEBUG 0
#endif

#include "include.h"

sym_p GetSym(){
	sym_p s = calloc(1, sizeof(*s));
	return s;
}

void SymTabInit(symtab_p st){
	return HashTableInit(st);
}

void SymInsert(symtab_p st, sym_p s){
	st->insert(s->name, s, st);
}

sym_p SymSearch(symtab_p st, const char* name){
	return st->search(name, st);
}