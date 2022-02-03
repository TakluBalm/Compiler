#include "include.h"

#define _SZ(type)	sizeof(type)

bool isLetter(char c){
	if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
		return true;
	}
	return false;
}

bool isDigit(char c, int base){
	switch(base){
		case DEC:{
			if(c >= '0' && c <= '9')	return true;
			return false;
		}
		case OCT:{
			if(c >= '0' && c <= '7')	return true;
			return false;
		}
		case HEX:{
			if(isDigit(c, DEC) || (c >= 'a' && c <= 'f'))	return true;
			return false;
		}
	}
}

bool isSymbol(char c){
	if(c == '_' || c == '-'){
		return true;
	}
	return false;
}

bool isCharacter(char c){
	if(isLetter(c) || isDigit(c, DEC) || isSymbol(c)){
		return true;
	}
	return false;
}

void lexer_advance(lexer_p lex){
	lex->current = getcf(lex->file);
}

void lexer_skip_whitespace(lexer_p lex){
	while(lex->current == ' ' || lex->current == '\n' || lex->current == '\t' || lex->current == '\r'){
		lexer_advance(lex);
	}
}

lexer_p init_lexer(file_p f){
	lexer_p lex = malloc(_SZ(*lex));
	lex->file = f;
	lex->current = getcf(f);
	lex->processed = 0;
	return lex;
}

char* lexer_parse_name(lexer_p lex){
	char* value = NULL;
	int len = 0;
	lex->processed = false;

	while(isLetter(lex->current) || isSymbol(lex->current)){
		if(len%20 == 0)	value = realloc(value, (len+20)*_SZ(char));
		value[len++] = lex->current;
		lexer_advance(lex);
		lex->processed = true;
	}

	if(value != NULL)	value[len] = '\0';
	return value;
}

int lexer_parse_digit(lexer_p lex, int base){
	int dig;
	lex->processed = false;

	if(isDigit(lex->current, base)){
		switch(base){
			case OCT:
			case DEC:{
				dig =  lex->current - '0';
				lex->processed = true;
				break;
			}

			case HEX:{
				if(lex->current >= 'a')	dig = 10 + lex->current- 'a';
				else dig = lex->current - '0';
				lex->processed = true;
				break;
			}
		}
		lexer_advance(lex);
	}
	return dig;
}

long long int lexer_parse_number(lexer_p lex, int base){
	long long int num = 0;

	if(!isDigit(lex->current, base)){
		lex->processed = false;
		return -1;
	}

	while(isDigit(lex->current, base)){
		int i = lexer_parse_digit(lex, base);
		if(!lex->processed){
			lex->processed = true;
			break;
		}
		num = base*num + i;
	}

	return num;
}

/*
	In case of processing failure the lexer_parse_char will give
	the unprocessed character as return value and keep the lexer
	pointing to the initial value. In case a character has been
	processed the lexer after returning points the the next value.
*/


char lexer_parse_char(lexer_p lex){
	char value = lex->current;
	long long int before = lex->processed;


	if(value != '\"' && value != '\'' && value != EOF){
		lexer_advance(lex);
		lex->processed++;
	}

// For escape sequences \ followed by ASCII value in decimal
	if(value == '\\'){
		value = 0;
		lexer_advance(lex);
		int count = 0;
		while(isDigit(lex->current, OCT) && count < 3){
			value = 8*value + lexer_parse_digit(lex, OCT);
			count++;
		}
	}

	return value;
}

char* lexer_parse_const(lexer_p lex){
	char* value = calloc(1, _SZ(char));
	int len = 0;
	char cur;

	while(lex->current != '\"' && lex->current != EOF){
		cur = lexer_parse_char(lex);
		value[len++] = cur;
		value = realloc(value, (len+1)*_SZ(char));
	}
	value[len] = '\0';
	lexer_advance(lex);
	return value;
}

// token_p lexer_next_token(lexer_p lex){
// 	lexer_skip_whitespace(lex);
// 	switch(lex->current){
// 		case '<':{
// 			lexer_advance(lex);
// 			char* name = lexer_parse_name(lex);

// 			if(lex->current != '>' || name == NULL)	return init_tok(name, TOKEN_ERR);
// 			lexer_advance(lex);
// 			return init_tok(name, TOKEN_RULE_NAME);
// 		}
// 		case '\"':{
// 			lexer_advance(lex);
// 			char* str = lexer_parse_const(lex);
// 			return init_tok(str, TOKEN_CONST);
// 		}
// 		case '|':
// 		case ';':
// 		case ':':{
// 			char* value = calloc(1, _SZ(char));
// 			*value = lex->current;
// 			lexer_advance(lex);
// 			return init_tok(value, TOKEN_PUNC);
// 		}
// 		case EOF:	return init_tok(NULL, TOKEN_EOF);
// 		default:	return init_tok(NULL, TOKEN_ERR);
// 	}
// }