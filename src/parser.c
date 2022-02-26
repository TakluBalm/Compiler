#include "include.h"

static ast_node* parseSyntax(lexer_p lex);
static ast_node* parseRule(lexer_p lex);
static ast_node* parseDef(lexer_p lex);
static ast_node* parseLineEnd(lexer_p lex);
static ast_node* parseList(lexer_p lex);
static ast_node* parseTerm(lexer_p lex);

struct PARSE_ERR PARSE_ERR;

void err_print(){
	printf("ERROR on %llu:%llu\n\t", PARSE_ERR.token->lineNum, PARSE_ERR.token->characterNum);
	printf(PARSE_ERR.msg);
}

bool isAhead(tok_p t1, tok_p t2){
	if(t2 == NULL)	return true;
	if(t1 == NULL)	return false;
	if(t1->lineNum < t2->lineNum)	return false;
	if(t1->lineNum > t2->lineNum)	return true;
	if(t1->characterNum > t2->characterNum)	return true;
	return false;
}

void delTree(ast_node* root, int mode){
	if(root == NULL)	return;
	if(root->type == LEAF){
		if(mode == DEL){
			free(root->token->value);
			free(root->token);
		}else{
			pushTok(root->token);
		}
	}else{
		for(int i = 0; i < root->numChild; i++){
			delTree(root->children[i], mode);
		}
		free(root->children);
	}
	free(root);
	return;
}

ast_node* parser(lexer_p lex){
	ast_node* tree;
	tree = parseSyntax(lex);
	tok_p next_tok = lexer_next_token(lex);
	if(next_tok->type != END){
		delTree(tree, PUSH);
		pushTok(next_tok);
		tree = NULL;
	}
	return tree;
}

ast_node* parseSyntax(lexer_p lex){
	ast_node* tree = malloc(sizeof(*tree));
	tree->type = SYNTAX;

	ast_node* rl = parseRule(lex);
	if(rl == NULL){
		free(tree);
		return NULL;
	}

	ast_node* s = parseSyntax(lex);
	if(s == NULL){
		tree->numChild = 1;
		tree->children = malloc((tree->numChild)*sizeof(tree));
		tree->children[0] = rl;
		return tree;
	}

	tree->numChild = 2;
	tree->children = malloc((tree->numChild)*sizeof(*(tree->children)));
	tree->children[0] = rl;
	tree->children[1] = s;
	return tree;
}

ast_node* parseRule(lexer_p lex){
	ast_node* tree = malloc(sizeof(*tree));
	tree->type = RULE; 

	ast_node* nt = malloc(sizeof(*nt));
	nt->token = lexer_next_token(lex);
	nt->type = LEAF;
	ast_node* colon = malloc(sizeof(*colon));
	colon->token = lexer_next_token(lex);
	colon->type = LEAF;
	if(colon->token->type != COLON || nt->token->type != NON_TERMINAL){
		if(nt->token->type != NON_TERMINAL && isAhead(nt->token, PARSE_ERR.token)){
			delTok(PARSE_ERR.token);
			PARSE_ERR.token = dupTok(nt->token);
			if(PARSE_ERR.msg != NULL)	free(PARSE_ERR.msg);
			PARSE_ERR.msg = calloc(100, sizeof(char));
			sprintf(PARSE_ERR.msg,"Expected a NON_TERMINAL instead of %s\n", nt->token->value);
		}else if(isAhead(colon->token, PARSE_ERR.token)){
			delTok(PARSE_ERR.token);
			PARSE_ERR.token = dupTok(colon->token);
			if(PARSE_ERR.msg != NULL)	free(PARSE_ERR.msg);
			PARSE_ERR.msg = calloc(100, sizeof(char));
			sprintf(PARSE_ERR.msg,"Expected a \':\' instead of %s\n", colon->token->value);
		}
		pushTok(nt->token);
		pushTok(colon->token);
		free(colon);
		free(nt);
		free(tree);
		return NULL;
	}

	ast_node* def = parseDef(lex);
	if(def == NULL){
		free(tree);
		return NULL;
	}
	ast_node* le = parseLineEnd(lex);
	if(le == NULL){
		free(tree);
		delTree(def, PUSH);
		return NULL;		
	}

	tree->numChild = 4;
	tree->children = malloc(4*sizeof(*tree));
	tree->children[0] = nt;
	tree->children[1] = colon;
	tree->children[2] = def;
	tree->children[3] = le;
	return tree;
}

ast_node* parseDef(lexer_p lex){
	ast_node* tree = malloc(sizeof(*tree));
	tree->type = DEF;

	ast_node* lt = parseList(lex);
	if(lt == NULL){
		free(tree);
		return NULL;
	}

	ast_node* or = malloc(sizeof(*or));
	or->token = lexer_next_token(lex);
	or->type = LEAF;
	if(or->token->type != OR){
		if(isAhead(or->token, PARSE_ERR.token)){
			delTok(PARSE_ERR.token);
			PARSE_ERR.token = dupTok(or->token);
			if(PARSE_ERR.msg != NULL)	free(PARSE_ERR.msg);
			PARSE_ERR.msg = calloc(100, sizeof(char));
			sprintf(PARSE_ERR.msg, "Expected a | instead of %s\n", or->token->value);
		}
		pushTok(or->token);
		tree->numChild = 1;
		tree->children = malloc(sizeof(ast_node*));
		tree->children[0] = lt;
		return tree;
	}

	ast_node* def = parseDef(lex);
	if(def == NULL){
		tree->numChild = 1;
		tree->children = malloc(sizeof(ast_node*));
		tree->children[0] = lt;
		return tree;
	}

	tree->numChild = 3;
	tree->children = malloc(sizeof(ast_node*)*3);
	tree->children[0] = lt;
	tree->children[1] = or;
	tree->children[2] = def;
	return tree;
}

ast_node* parseLineEnd(lexer_p lex){
	ast_node* tree = malloc(sizeof(*tree));
	tree->type = LINE_END;

	ast_node* semi = malloc(sizeof(*semi));
	semi->type = LEAF;
	semi->token = lexer_next_token(lex);
	if(semi->token->type != SEMI_COLON){
		if(isAhead(semi->token, PARSE_ERR.token)){
			delTok(PARSE_ERR.token);
			PARSE_ERR.token = dupTok(semi->token);
			if(PARSE_ERR.msg != NULL)	free(PARSE_ERR.msg);
			PARSE_ERR.msg = calloc(100, sizeof(char));
			sprintf(PARSE_ERR.msg, "Expected \';\' instead of %s\n");
		}
		pushTok(semi->token);
		free(tree);
		free(semi);
		return NULL;
	}

	ast_node* le = parseLineEnd(lex);
	if(le == NULL){
		tree->numChild = 1;
		tree->children = malloc(tree->numChild*sizeof(tree));
		tree->children[0] = semi;
		return tree;
	}


	tree->numChild = 2;
	tree->children = malloc(tree->numChild*sizeof(tree));
	tree->children[0] = semi;
	tree->children[1] = le;
	return tree;
}

ast_node* parseList(lexer_p lex){
	ast_node* tree = malloc(sizeof(*tree));
	tree->type = LIST;

	ast_node* t = parseTerm(lex);
	if(t == NULL){
		free(tree);
		return NULL;
	}

	ast_node* lt = parseList(lex);
	if(lt == NULL){
		tree->numChild = 1;
		tree->children = malloc((tree->numChild)*sizeof(tree));
		tree->children[0] = t;
		return tree;
	}


	tree->numChild = 2;
	tree->children = malloc((tree->numChild)*sizeof(tree));
	tree->children[0] = t;
	tree->children[1] = lt;
	return tree;
}

ast_node* parseTerm(lexer_p lex){
	ast_node* tree = malloc(sizeof(*tree));
	tree->type = TERM;

	ast_node* next = malloc(sizeof(*next));
	next->type = LEAF;
	next->token = lexer_next_token(lex);
	if(next->token->type != NON_TERMINAL && next->token->type != TERMINAL){
		if(isAhead(next->token, PARSE_ERR.token)){
			delTok(PARSE_ERR.token);
			PARSE_ERR.token = dupTok(next->token);
			if(PARSE_ERR.msg != NULL)	free(PARSE_ERR.msg);
			PARSE_ERR.msg = calloc(100, sizeof(char));
			sprintf(PARSE_ERR.msg, "Expected a Terminal or Non-Terminal instead of %s\n", next->token->value);
		}
		pushTok(next->token);
		free(next);
		free(tree);
		return NULL;
	}

	tree->numChild = 1;
	tree->children = malloc((tree->numChild)*sizeof(tree));
	tree->children[0] = next;
	return tree;
}
