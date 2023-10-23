# YAY (Yet-Another-Yacc)
Parse EBNF grammar and generate its corresponding LALR state table

## Structure of the repository
1. **src** : consists all the code.
	* *`lexer.cpp`* : Contains code for implementing a lexer.\
		A lexer needs to be given a file handle `FILE*`. It reads characters from files which it uses to generate and return tokens. The tokens have the following structure.
		```C++
		class Token{
			public:
			enum Type type;
			std::string* val = NULL;

			Token(Type t, std::string* val){
				type = t;
				this->val = val;
			}

			Token(Type t){
				type = t;
			}

			Token(){};
		};
		```
		It exposed a Lexer class with following type
		```C++
		class Lexer{
			private:

			FILE* file;
			char current;
			bool processed;
			Token prev;
			bool consumed = true;

			public:
			
			Lexer(FILE* f);
			Lexer();

			Token peek();
			void consume();

			void advance();
			void skipWhiteSpace();
			std::string parseNonTerminal();
			int parseDigit(int base);
			long long int parseNumber(int base);
			char parseChar();
			std::string parseTerminal();
		};
		```
	* *`parser.cpp`* : Contains code for implementing a parser\
		A parser is initialised by passing a lexer which it uses to obtain tokens. It creates an AST (Abstract Syntax Tree) for the EBNF. The 3 consists of 2 types of nodes
		- Term (base)
		```C++
		class Term : public AstNode {
			public:
			enum Type {
				TERMINAL,
				NONTERMINAL
			};
			const std::string getName() const;
			Term(enum Type type, const std::string& term);
			Term();
			enum Type type() const { return _type; };

			protected:
			std::string name;
			enum Type _type;
		};
		```
		- Rule (made up of terms)
		```C++
		class Rule : public AstNode {
			static int _cnt;
			std::vector<const Term*> terms;
			int _id;
			
			public:
			Rule();
			void addTerm(const Term* term);
			const std::vector<const Term*>& getTerms() const;
			int id() const;
			const Term* getTerm(int idx) const;
			const std::string getName() const;
		};
		```
		The AST looks as follow:
		```C++
		class Ast {
			std::map<std::string, std::vector<Rule*>> rules;
			std::string _startSymbol;

			public:
			void addDefinition(const std::string& nt, Rule* rule);
			const std::vector<Rule*>& getDefinition(const std::string& nt) const;
			const std::string startSymbol(void) const;
		};
		```
	* *`main.cpp`* : Contains the main program of our project. It brings together the code from other files and integrates them
	* *`include/`* : Contains relevant header files

3. ### **test.txt** consists of a test bnf grammar I have been testing against my code.

4. ### **CMakeLists.txt** for CMake build.

## Build the code
Make sure you have GNU make installed before running the command
```Bash
mkdir build
cd build
cmake -S ../
make
./src/main
```

## Run the code
The program will ask for path to EBNF file. Once correct path is provided, it will print the generate BNF rules, map of terminals to corresponding ID and then finally the state transition table

## EBNF Description

- `Terminals` : Any lowercase word is considered a terminal
- `Non-Terminals` : Any uppercase word is considered a non-terminal
- `Rule` : A rule is a non-terminal followed by a colon, followed by a description and terminated with a semi-colon. The description is a list of terminals / non-terminals with a modification optinally attached to them.
- `Modifications`: These are of 3 types:
	- *Optional* : A list terminals / non-terminals surrounded by `[` `]` are treated as optinal, i.e. they can be matchded or not depending on scenario
	- *Repeating* : A list terminals / non-terminals surrounded by `{` `}` are treated as repeatable, i.e. they can be matchded zero or more times
	- *Grouping* : A list terminals / non-terminals surrounded by `(` `)` are treated as grouped, i.e. they are considered as a subrule.

EBNF decription written in this format:
```
SYNTAX : [SYNTAX] semicolon DEF ;
DEF : nonterminal colon RULES ;
RULES : [RULES or] RULE ;
RULE : [RULE] TERM ;
TERM : obracket RULES cbracket | obrace RULES cbrace | oparen RULES cparen | nonterminal | terminal ; 
```
