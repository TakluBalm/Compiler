#ifndef PARSER_H
#define PARSER_H

#include "../include.h"

extern struct PARSE_ERR{
	char* msg;
	tok_p token;
} PARSE_ERR;

enum node_type_t{
	SYNTAX,
	RULE,
	DEF,
	LINE_END,
	LIST,
	TERM,
	LEAF
};

enum delModes{
	PUSH,
	DEL
};

typedef struct AST_node{
	enum node_type_t type;
	int numChild;
	union{
		struct AST_node** children;
		tok_p token;
	};
} ast_node;

ast_node* parser(lexer_p lex);
void delTree(ast_node* root, enum delModes mode);
bool updateErr(tok_p errTok, char* msg);
void err_print();

#endif