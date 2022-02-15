#include "include.h"

void printTree(ast_node*, int);

int main(int argc, char** argv){
	if(argc < 2){
		printf("The correct commandline syntax is: %s filename\n", argv[0]);
		return 1;
	}
	char* filename = argv[1];

	file_p f = openf(filename, NCREAT|READ);
	if(f == NULL){
		printf("Could not open file\n");
	}
	lexer_p lex = init_lexer(f);
	ast_node* tree = parser(lex);
	if(tree == NULL)	printf("Invalid Syntax\n");
	else{
		printf("Syntax Valid:\n");
		printTree(tree, 0);
	}
	delTree(tree, DEL);
	delLexer(lex);
	closef(f);
}

void printTree(ast_node* tree, int depth){
	if(tree == NULL)	return;
	for(int i = 0; i < depth; i++){
		printf("|");
	}
	printf("%d", tree->type);
	if(tree->type != LEAF){
		printf("\n");
		for(int i = 0; i < tree->numChild; i++){
			printTree(tree->children[i], depth+1);
		}
		return;
	}
	printf(" \"%s\"\n",tree->token->value);
}
