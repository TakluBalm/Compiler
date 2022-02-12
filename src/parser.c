#include "include.h"

ast_node* parseSyntax(lexer_p lex);
ast_node* parseRule(lexer_p lex);
ast_node* parseDef(lexer_p lex);
ast_node* parseLineEnd(lexer_p lex);
ast_node* parseList(lexer_p lex);
ast_node* parseTerm(lexer_p lex);

unsigned int DEPTH_COUNT = 0;

void delTree(ast_node* root){
	if(root == NULL)	return;
	if(root->type == LEAF){
		free(root->token->value);
		free(root->token);
	}else{
		for(int i = 0; i < root->numChild; i++){
			delTree(root->children[i]);
		}
		free(root->children);
	}
	free(root);
	return;
}

ast_node* parser(lexer_p lex){
	ast_node* tree;
	DEPTH_COUNT++;
	tree = parseSyntax(lex);
	return tree;
}

ast_node* parseSyntax(lexer_p lex){
	DEPTH_COUNT++;
	ast_node* tree = malloc(sizeof(*tree));
	tree->type = SYNTAX;

	ast_node* rl = parseRule(lex);
	if(rl == NULL){
		free(tree);
		DEPTH_COUNT--;
		return NULL;
	}

	tok_p next_tok = lexer_next_token(lex);
	if(next_tok->type == END){
		tree->numChild = 1;
		tree->children = malloc((tree->numChild)*sizeof(*(tree->children)));
		tree->children[0] = rl;
	}
	pushTok(next_tok);

	ast_node* s = parseSyntax(lex);
	if(s == NULL){
		tree->numChild = 1;
		tree->children = malloc((tree->numChild)*sizeof(tree));
		tree->children[0] = rl;
		DEPTH_COUNT--;
		return tree;
	}

	tree->numChild = 2;
	tree->children = malloc((tree->numChild)*sizeof(*(tree->children)));
	tree->children[0] = rl;
	tree->children[1] = s;
	DEPTH_COUNT--;
	return tree;
}

ast_node* parseRule(lexer_p lex){
	DEPTH_COUNT++;
	ast_node* tree = malloc(sizeof(*tree));
	tree->type = RULE; 

	ast_node* nt = malloc(sizeof(*nt));
	nt->token = lexer_next_token(lex);
	nt->type = LEAF;
	ast_node* colon = malloc(sizeof(*colon));
	colon->token = lexer_next_token(lex);
	colon->type = LEAF;
	if(colon->token->type != COLON || nt->token->type != NON_TERMINAL){
		pushTok(nt->token);
		pushTok(colon->token);
		free(colon);
		free(nt);
		free(tree);
		DEPTH_COUNT--;
		return NULL;
	}

	ast_node* def = parseDef(lex);
	if(def == NULL){
		free(tree);
		DEPTH_COUNT--;
		return NULL;
	}
	ast_node* le = parseLineEnd(lex);
	if(le == NULL){
		free(tree);
		delTree(def);
		DEPTH_COUNT--;
		return NULL;		
	}

	tree->numChild = 4;
	tree->children = malloc(4*sizeof(*tree));
	tree->children[0] = nt;
	tree->children[1] = colon;
	tree->children[2] = def;
	tree->children[3] = le;
	DEPTH_COUNT--;
	return tree;
}

ast_node* parseDef(lexer_p lex){
	DEPTH_COUNT++;
	ast_node* tree = malloc(sizeof(*tree));
	tree->type = DEF;

	ast_node* lt = parseList(lex);
	if(lt == NULL){
		free(tree);
		DEPTH_COUNT--;
		return NULL;
	}

	ast_node* or = malloc(sizeof(*or));
	or->token = lexer_next_token(lex);
	or->type = LEAF;
	if(or->token->type != OR){
		pushTok(or->token);
		tree->numChild = 1;
		tree->children = malloc(sizeof(ast_node*));
		tree->children[0] = lt;
		DEPTH_COUNT--;;
		return tree;
	}

	ast_node* def = parseDef(lex);
	if(def == NULL){
		tree->numChild = 1;
		tree->children = malloc(sizeof(ast_node*));
		tree->children[0] = lt;
		DEPTH_COUNT--;
		return tree;
	}

	tree->numChild = 3;
	tree->children = malloc(sizeof(ast_node*)*3);
	tree->children[0] = lt;
	tree->children[1] = or;
	tree->children[2] = def;
	DEPTH_COUNT--;
	return tree;
}

ast_node* parseLineEnd(lexer_p lex){
	DEPTH_COUNT++;
	ast_node* tree = malloc(sizeof(*tree));
	tree->type = LINE_END;

	ast_node* semi = malloc(sizeof(*semi));
	semi->type = LEAF;
	semi->token = lexer_next_token(lex);
	if(semi->token->type != SEMI_COLON){
		pushTok(semi->token);
		free(tree);
		free(semi);
		DEPTH_COUNT--;
		return NULL;
	}

	ast_node* le = parseLineEnd(lex);
	if(le == NULL){
		tree->numChild = 1;
		tree->children = malloc(tree->numChild*sizeof(tree));
		tree->children[0] = semi;
		DEPTH_COUNT--;
		return tree;
	}


	tree->numChild = 2;
	tree->children = malloc(tree->numChild*sizeof(tree));
	tree->children[0] = semi;
	tree->children[1] = le;
	DEPTH_COUNT--;
	return tree;
}

ast_node* parseList(lexer_p lex){
	DEPTH_COUNT++;
	ast_node* tree = malloc(sizeof(*tree));
	tree->type = LIST;

	ast_node* t = parseTerm(lex);
	if(t == NULL){
		free(tree);
		DEPTH_COUNT--;
		return NULL;
	}

	ast_node* lt = parseList(lex);
	if(lt == NULL){
		tree->numChild = 1;
		tree->children = malloc((tree->numChild)*sizeof(tree));
		tree->children[0] = t;
		DEPTH_COUNT--;
		return tree;
	}


	tree->numChild = 2;
	tree->children = malloc((tree->numChild)*sizeof(tree));
	tree->children[0] = t;
	tree->children[1] = lt;
	DEPTH_COUNT--;
	return tree;
}

ast_node* parseTerm(lexer_p lex){
	DEPTH_COUNT++;
	ast_node* tree = malloc(sizeof(*tree));
	tree->type = TERM;

	ast_node* next = malloc(sizeof(*next));
	next->type = LEAF;
	next->token = lexer_next_token(lex);
	if(next->token->type != NON_TERMINAL && next->token->type != TERMINAL){
		pushTok(next->token);
		free(next);
		free(tree);
		DEPTH_COUNT--;
		return NULL;
	}

	tree->numChild = 1;
	tree->children = malloc((tree->numChild)*sizeof(tree));
	tree->children[0] = next;
	DEPTH_COUNT--;
	return tree;
}
