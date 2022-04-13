#include "include.h"

void printTree(ast_node*, int);
void printVector(struct vector v);

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
	if(tree == NULL){
		printf("Invalid Syntax\n");
		err_print();
	}else if(!checkSymbols(tree)){
		printf("Invalid Semantics\n");
		err_print();
	}
	else{
		struct vector rules = getElementsOfType(RULE, false, tree);
		for(size_t i = 0; i < rules.size; i++){
			struct vector v = FIRST(((ast_node*)rules.arr[i])->children[0]->token);
			for(size_t j = 0; j < v.size; j++){
				printf("%s ", ((struct symbol*)(v.arr[j]))->token->value);
			}
			printf("\n");
		}
	}
	SYMBOL_TABLE = DelHTable(true, SYMBOL_TABLE);
	delTree(tree, DEL);
	delLexer(lex);
	closef(f);
}

void printVector(struct vector v){
	for(int i = 0; i < v.size; i++){
		printf("\"%s\" ", ((struct symbol*)v.arr[i])->token->value);
	}
	printf("\n");
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
