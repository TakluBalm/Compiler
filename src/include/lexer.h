#ifndef LEXER_H
#define LEXER_H

#include "../../lib/include/fio.h"
#include <stdbool.h>

struct LEXER{
	file_p file;
	char current;
	bool processed;
};

typedef struct LEXER* lexer_p;


lexer_p init_lexer(file_p f);

char* lexer_parse_name(lexer_p lex);
int lexer_parse_digit(lexer_p lex, int base);
long long int lexer_parse_number(lexer_p lex, int base);
char* lexer_parse_const(lexer_p lex);
void lexer_skip_whitespace(lexer_p lex);
void lexer_advance(lexer_p lex);

#endif