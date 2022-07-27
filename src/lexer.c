#include "include.h"

#define OCT	8
#define DEC	10
#define HEX	16

tok_p TOK_STACK[512];
unsigned int TOK_STACK_POINTER = 0;
size_t LINE = 1;
size_t CHAR = 1;

void pushTok(tok_p tok){
	TOK_STACK[TOK_STACK_POINTER++] = tok;
}

tok_p popTok(){
	if(TOK_STACK_POINTER > 0){
		return TOK_STACK[--TOK_STACK_POINTER];
	}
	return NULL;
}

void delLexer(lexer_p lex){
	if(TOK_STACK_POINTER > 0){
		while(TOK_STACK_POINTER){
			tok_p tok = popTok();
			free(tok->value);
			free(tok);
		}
	}
	free(lex);
}

static bool isLetter(char c){
	if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
		return true;
	}
	return false;
}

static bool isDigit(char c, int base){
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

static bool isSymbol(char c){
	if(c == '_' || c == '-'){
		return true;
	}
	return false;
}

static bool isCharacter(char c){
	if(isLetter(c) || isDigit(c, DEC) || isSymbol(c)){
		return true;
	}
	return false;
}

static void lexer_advance(lexer_p lex){
	lex->current = getcf(lex->file);
	CHAR++;
}

static void lexer_skip_whitespace(lexer_p lex){
	while(true){
		switch(lex->current){
			case '\n':{
				LINE++;
				CHAR = 0;
			}
			case ' ':
			case '\t':
			case '\r':{
				lexer_advance(lex);
				break;
			}
			default:{
				return;
			}
		}
	}
}

lexer_p init_lexer(file_p f){
	lexer_p lex = malloc(sizeof(*lex));
	lex->file = f;
	lex->current = getcf(f);
	lex->processed = 0;
	return lex;
}

tok_p getTok(char* value, enum tok_type_t type){
	tok_p t = malloc(sizeof(*t));
	t->type = type;
	t->value = value;
	return t;
}

tok_p dupTok(tok_p token){
	tok_p t = malloc(sizeof(*t));
	t->type = token->type;
	char* str = token->value;
	int len = 0;
	while(str[len])	len++;
	t->value = malloc((len+1)*sizeof(char));
	while(len+1){
		t->value[len] = str[len];
		len--;
	}
	t->characterNum = token->characterNum;
	t->lineNum = token->lineNum;
	return t;
}

void delTok(tok_p token){
	if(token == NULL)	return;
	free(token->value);
	free(token);
}

static char* lexer_parse_non_terminal(lexer_p lex){
	char* value = NULL;
	int len = 0;
	lex->processed = false;

	while(isLetter(lex->current) || isSymbol(lex->current)){
		if(len%20 == 0)	value = realloc(value, (len+20)*sizeof(char));
		value[len++] = lex->current;
		lexer_advance(lex);
		lex->processed = true;
	}

	if(lex->current == '>'){
		if(value != NULL)	value[len] = '\0';
		lexer_advance(lex);
		return value;
	}
	else{
		lex->processed = false;
		if(value != NULL)	free(value);
		return NULL;
	}
}

static int lexer_parse_digit(lexer_p lex, int base){
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

static long long int lexer_parse_number(lexer_p lex, int base){
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

static char lexer_parse_char(lexer_p lex){
	char value = lex->current;
	lex->processed = true;

// For escape sequences \ followed by ASCII value in decimal
	if(value == '\\'){
		value = 0;
		lexer_advance(lex);
		int count = 0;
		while(isDigit(lex->current, OCT) && count < 3){
			value = 8*value + lexer_parse_digit(lex, OCT);
			count++;
		}
		return value;
	}

	if(value != EOF)	lexer_advance(lex);
	return value;
}

static char* lexer_parse_terminal(lexer_p lex){
	char* value = NULL;
	int len = 0;
	char next_char = lex->current;

	while(lex->current != '\"' && lex->current != EOF){
		if(len%20 == 0)	value = realloc(value, (len+20)*sizeof(char));
		next_char = lexer_parse_char(lex);
		value[len++] = next_char;
	}
	value[len] = '\0';
	lexer_advance(lex);
	return value;
}

tok_p lexer_next_token(lexer_p lex){
	tok_p tok = popTok();
	if(tok != NULL){
		return tok;
	}

	lexer_skip_whitespace(lex);

	size_t line = LINE;
	size_t charNum = CHAR;

	switch(lex->current){
		case '<':{
			lexer_advance(lex);
			char* value = lexer_parse_non_terminal(lex);
			tok_p t;
			if(lex->processed)	t =  getTok(value, NON_TERMINAL);
			else				t =  getTok(NULL, UNIDENTIFIED_TOK);
			t->lineNum = line;
			t->characterNum = charNum;
			return t;
		}
		case ':':
		case '|':
		case ';':{
			char ch = lex->current;
			char* value = calloc(2, sizeof(char));
			value[0] = ch;
			enum tok_type_t type = (ch == '|')?(OR):((ch == ';')?(SEMI_COLON):(COLON));
			tok_p t = getTok(value, type);
			t->characterNum = charNum;
			t->lineNum = line;
			lexer_advance(lex);
			return t;
		}
		case '\"':{
			lexer_advance(lex);
			char* value = lexer_parse_terminal(lex);
			tok_p t;
			if(lex->processed)	t = getTok(value , TERMINAL);
			else				t = getTok(value, UNIDENTIFIED_TOK);
			t->characterNum = charNum;
			t->lineNum = line;
			return t;
		}
		case EOF:{
			char* value = calloc(2, sizeof(char));
			value[0] = '$';
			tok_p t = getTok(value, END);
			t->characterNum = charNum;
			t->lineNum = line;
			return t;
		}
	}
	char* value = calloc(2, sizeof(char));
	value[0] = lex->current;
	tok = getTok(value, UNIDENTIFIED_TOK);
	tok->lineNum = line;
	tok->characterNum = charNum;
}