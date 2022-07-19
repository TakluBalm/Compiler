#ifndef PARSER_H
#define PARSER_H

#include "../include.h"

extern struct PARSE_ERR{
	char* msg;
	tok_p token;
} PARSE_ERR;

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

enum delModes{
	PUSH,
	DEL
};

ast_node* parser(lexer_p lex);
void delTree(ast_node* root, enum delModes mode);
bool updateErr(tok_p errTok, char* msg);
void err_print();

#endif