# Language_Recognizer
Parse BNF grammar and form AST

***
## Structure of the repository
---
1. ### **lib** folder consists of reusable data structure code.
	* *fio.c* : Contains wrapper for File I/O system calls. It has been used for buffered file I/O in our project.
	* *vector.c* : Contains the code for C++ like vector implementation in C. It helps managing a variable sized array.
	* *hash_table.c* : Contains code for the adding, removing and searching data in a hash table.

2. ### **src** folder consists of implementation specific code.
	* *lexer.c* : Contains code for implementing a lexer.\
		A lexer needs to be given a file handle (from *fio.c*). It reads characters from files which it uses to generate and return tokens. The tokens have the following structure.
		```C
		struct TOKEN{
			char* value;
			enum {
				TERMINAL,
				NON_TERMINAL,
				SEMI_COLON,
				COLON,
				OR,
				END,
				UNIDENTIFIED_TOK
			} type;
			size_t lineNum;
			size_t characterNum;
		};
		```
	* *parser.c* : Contains code for implementing a parser\
		A parser is initialised by passing a lexer which it uses to obtain tokens. It organises these tokens to create a Syntax/Parse Tree. A node of the tree has the following structure.
		```C
		struct AST_node{
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
		}
		```
	* *semantic.c* : It takes the parse tree as input and does some error checking on it. These checks include multiple definition of non-terminals OR a non-terminal declared but not defined. It will contain functions to manipulate and use the Syntax Tree to achieve our final goal of making a state machine to recognize the grammar.
	* *main.c* : Contains the main program of our project. It brings together the code from other files and integrates them

3. ### **test.txt** consists of a test bnf grammar I have been testing against my code.

4. ### **Makefile** for the automated build.

***
## Build the code
---
Make sure you have GNU make installed before running the command
```
make lib
make main
```
This will create the shared library libMyLib.so and the main program. Use `make run` to test **main** against *test.txt*.
