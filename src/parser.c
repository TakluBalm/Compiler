#include "include.h"

static ast_node* parseSyntax(lexer_p lex);
static ast_node* parseRule(lexer_p lex);
static ast_node* parseDef(lexer_p lex);
static ast_node* parseLineEnd(lexer_p lex);
static ast_node* parseList(lexer_p lex);
static ast_node* parseTerm(lexer_p lex);
static void err_print();

size_t TOK_CONSUMED = 0;

struct{
	char* msg;
	size_t consumedTok;
	tok_p err_tok;
} PARSE_ERR;

void err_print(){
	printf("ERROR while processing %lluth token:\n\t", PARSE_ERR.consumedTok);
	printf(PARSE_ERR.msg, PARSE_ERR.err_tok->value);
}

void delTree(ast_node* root, int mode){
	if(root == NULL)	return;
	if(root->type == LEAF){
		if(mode == DEL){
			free(root->token->value);
			free(root->token);
		}else{
			pushTok(root->token);
			TOK_CONSUMED--;
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
	TOK_CONSUMED++;
	if(next_tok->type != END){
		delTree(tree, PUSH);
		pushTok(next_tok);
		TOK_CONSUMED--;
		tree = NULL;
		err_print();
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
	TOK_CONSUMED++;
	nt->type = LEAF;
	ast_node* colon = malloc(sizeof(*colon));
	colon->token = lexer_next_token(lex);
	TOK_CONSUMED++;
	colon->type = LEAF;
	if(colon->token->type != COLON || nt->token->type != NON_TERMINAL){
		TOK_CONSUMED -= 2;
		if(nt->token->type != NON_TERMINAL && PARSE_ERR.consumedTok < TOK_CONSUMED){
			delTok(PARSE_ERR.err_tok);
			PARSE_ERR.err_tok = dupTok(nt->token);
			PARSE_ERR.consumedTok = TOK_CONSUMED;
			PARSE_ERR.msg = "Expected a NON_TERMINAL instead of %s\n";
		}else{
			delTok(PARSE_ERR.err_tok);
			PARSE_ERR.err_tok = dupTok(colon->token);
			PARSE_ERR.consumedTok = TOK_CONSUMED;
			PARSE_ERR.msg = "Expected a \':\' instead of %s\n";
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
	TOK_CONSUMED++;
	or->type = LEAF;
	if(or->token->type != OR){
		if(PARSE_ERR.consumedTok < --TOK_CONSUMED){
			delTok(PARSE_ERR.err_tok);
			PARSE_ERR.err_tok = dupTok(or->token);
			PARSE_ERR.consumedTok = TOK_CONSUMED;
			PARSE_ERR.msg = "Expected a | instead of %s\n";
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
	TOK_CONSUMED++;
	semi->token = lexer_next_token(lex);
	TOK_CONSUMED++;
	if(semi->token->type != SEMI_COLON){
		if(PARSE_ERR.consumedTok < --TOK_CONSUMED){
			delTok(PARSE_ERR.err_tok);
			PARSE_ERR.err_tok = dupTok(semi->token);
			PARSE_ERR.consumedTok = TOK_CONSUMED;
			PARSE_ERR.msg = "Expected \';\' instead of %s\n";
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
	TOK_CONSUMED++;
	if(next->token->type != NON_TERMINAL && next->token->type != TERMINAL){
		if(PARSE_ERR.consumedTok < --TOK_CONSUMED){
			delTok(PARSE_ERR.err_tok);
			PARSE_ERR.err_tok = dupTok(next->token);
			PARSE_ERR.consumedTok = TOK_CONSUMED;
			PARSE_ERR.msg = "Expected a Terminal or Non-Terminal instead of %s\n";
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
