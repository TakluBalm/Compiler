#ifndef SM_H
#define SM_H

#include "lexer.h"

typedef struct AST_node{
	enum types{
		SYNTAX,
		RULE,
		DEF,
		LINE_END,
		LIST,
		TERM,
		LEAF
	} type;
	int numChild;
	union{
		struct AST_node** children;
		tok_p token;
	};
} ast_node;

ast_node* parser(lexer_p lex);
void delTree(ast_node* root);

#endif