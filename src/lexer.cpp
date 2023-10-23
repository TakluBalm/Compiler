#include <lexer.hpp>
#include <stdio.h>
#include <string>

using namespace std;

#define OCT	8
#define DEC	10
#define HEX	16
	
void Lexer::advance(){
	current = fgetc(file);
}

void Lexer::skipWhiteSpace(){
	while(true){
		switch(current){
			case '\n':
			case ' ':
			case '\t':
			case '\r':{
				advance();
				break;
			}
			default:{
				return;
			}
		}
	}
}

string Lexer::parseNonTerminal(){
	string value;

	while((current >= 'A' && current <= 'Z') || current == '_'){
		value += current;
		advance();
	}

	return value;
}

string Lexer::parseTerminal(){
	string value;

	while((current >= 'a' && current <= 'z') || current == '_'){
		value += current;
		advance();
	}

	return value;
}

Lexer::Lexer(FILE* file){
	this->file = file;
	current = fgetc(file);
	processed = 0;
}

Token Lexer::peek(){
	if(!consumed)	return prev;
	consumed = false;
	skipWhiteSpace();
	switch(current){
		case ':':{
			enum Type type = COLON;
			Token t(type);
			advance();
			return prev = t;
		}
		case '|':{
			enum Type type = OR;
			Token t(type);
			advance();
			return prev = t;
		}
		case '[':{
			enum Type type = OBRACKET;
			Token t(type);
			advance();
			return prev = t;
		}
		case ']':{
			enum Type type = CBRACKET;
			Token t(type);
			advance();
			return prev = t;
		}
		case '{':{
			enum Type type = OBRACE;
			Token t(type);
			advance();
			return prev = t;
		}
		case '}':{
			enum Type type = CBRACE;
			Token t(type);
			advance();
			return prev = t;
		}
		case '(':{
			enum Type type = OPAREN;
			Token t(type);
			advance();
			return prev = t;
		}
		case ')':{
			enum Type type = CPAREN;
			Token t(type);
			advance();
			return prev = t;
		}
		case ';':{
			enum Type type = SEMI_COLON;
			Token t(type);
			advance();
			return prev = t;
		}
		case EOF:{
			return prev = Token(END);
		}
		default: {
			string* value = new string();
			if(current >= 'a' && current <= 'z'){
				*value = parseTerminal();
				return prev = Token(TERMINAL, value);
			}else if(current >= 'A' && current <= 'Z'){
				*value = parseNonTerminal();
				return prev = Token(NON_TERMINAL, value);
			}else{
				return prev = Token(UNIDENTIFIED_TOK);
			}
		}
	}
	consumed = false;
	return Token(UNIDENTIFIED_TOK);
}

void Lexer::consume(){
	consumed = true;
}