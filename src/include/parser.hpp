#ifndef PARSER_H
#define PARSER_H

#include "lexer.hpp"
#include <vector>
#include <stack>
#include <map>

class Term;
class Rule;
class Definition;
class TreeNode;
class AstNode;

class TreeNode {
	public:
	Token token;
	std::vector<TreeNode*> children;
};

class AstNode {
	public:
	enum Type {
		DEFINITION,
		RULE,
		TERM
	} nodeType;

	virtual const std::string getName() const = 0;
};

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

class Ast {
	std::map<std::string, std::vector<Rule*>> rules;
	std::string _startSymbol;

	public:
	void addDefinition(const std::string& nt, Rule* rule);
	const std::vector<Rule*>& getDefinition(const std::string& nt) const;
	const std::string startSymbol(void) const;
};

class Parser {
	private:
	std::stack<TreeNode*> treeStk;
	std::stack<int> stateStk;
	std::map<TreeNode*, const AstNode*> mp;
	Lexer lexer;
	Ast* ast;

	public:
	
	Parser(Lexer lexer);
	Parser(){};
	Ast* parse();

	void reduce(int rule);
	void shift(Token tkn, int state);
};

#endif