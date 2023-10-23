#include <parser.hpp>
#include <vector>
#include <stack>
#include <algorithm>
#include <queue>

using namespace std;

enum Action{
	S,	// Shift
	R,	// Reduce
	G,	// Goto
	A,	// Accept
	N
};

/****** RULE *******/

int Rule::_cnt = 0;

void Rule::addTerm(const Term* term){
	terms.push_back(term);
}

int Rule::id() const {
	return _id;
}

Rule::Rule(){
	_id = ++_cnt;
}

const string Rule::getName() const {
	string name;
	for(int i = 0; i < terms.size(); i++){
		if(i != 0)	name += " ";
		name += terms[i]->getName();
	}
	return name;
}

const vector<const Term*>& Rule::getTerms() const {
	return terms;
}

const Term* Rule::getTerm(int idx) const {
	if(terms.size() > idx && idx >= 0){
		return terms[idx];
	}
	return nullptr;
}

/***** DEFINITION ******/

class Definition : public AstNode {
	Ast* ast;
	std::vector<Rule*> rules;
	std::string lhs;
	
	public:
	void addRule(Rule* rule){
		rules.push_back(rule);
		if(lhs != ""){
			ast->addDefinition(lhs, rule);
		}
	}
	void setLHS(const std::string& nt){
		lhs = nt;
		for(auto rule: rules){
			ast->addDefinition(lhs, rule);
		}
	}
	const string getLHS() const {
		return lhs;
	}
	const std::string getName() const {
		string name = (lhs != "")?(lhs + ": "):lhs;
		for(int i = 0; i < rules.size(); i++){
			if(i != 0)	name += " | ";
			name += rules[i]->getName();
		}
		return name;
	}

	Definition(Ast* ast){
		this->ast = ast;
	}
};

/****** TERM ******/

const string Term::getName() const {
	return name;
}

Term::Term(){}

Term::Term(enum Type type, const string& name){
	this->_type = type;
	this->name = name;
}

/****** AST *******/

void Ast::addDefinition(const string& nt, Rule* rule){
	rules[nt].push_back(rule);
}

const vector<Rule*>& Ast::getDefinition(const string& nt) const {
	auto it = rules.find(nt);
	if(it == rules.end())	return *(new vector<Rule*>());
	else	return it->second;
}

const string Ast::startSymbol() const {
	return "S'";
}

/****** TABLES ********/

static const char table[24][17] = {
	{3 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1 , 2 , -1, -1, -1},
	{3 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 24, -1, 4 , -1, -1, -1},
	{2 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2 , -1, -1, -1, -1, -1},
	{-1, 5 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1 , -1, -1, -1, -1, -1},
	{10, -1, -1, -1, 9 , 11, -1, 12, -1, 13, -1, -1, -1, -1, 6 , 7 , 8 },
	{-1, -1, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, -1, 5 , 5 , 9 , 11, 5 , 12, 5 , 13, 5 , -1, -1, -1, -1, -1, 16},
	{7 , -1, 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , -1, -1, -1, -1, -1, -1},
	{8 , -1, 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , 8 , -1, -1, -1, -1, -1, -1},
	{9 , -1, 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , -1, -1, -1, -1, -1, -1},
	{10, -1, -1, -1, 9 , 11, -1, 12, -1, 13, -1, -1, -1, -1, 17, 7 , 8 },
	{10, -1, -1, -1, 9 , 11, -1, 12, -1, 13, -1, -1, -1, -1, 18, 7 , 8 },
	{10, -1, -1, -1, 9 , 11, -1, 12, -1, 13, -1, -1, -1, -1, 19, 7 , 8 },
	{3 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3 , -1, -1, -1, -1, -1},
	{10, -1, -1, -1, 9 , 11, -1, 12, -1, 13, -1, -1, -1, -1, -1, 20, 8 },
	{6 , -1, 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, 15, -1, -1, 21, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, 15, -1, -1, -1, -1, 22, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, 15, -1, -1, -1, -1, -1, -1, 23, -1, -1, -1, -1, -1, -1},
	{10, -1, 4 , 4 , 9 , 11, 4 , 12, 4 , 13, 4 , -1, -1, -1, -1, -1, 16},
	{10, -1, 10, 10, 10, 10, 10, 10, 10, 10, 10, -1, -1, -1, -1, -1, -1},
	{11, -1, 11, 11, 11, 11, 11, 11, 11, 11, 11, -1, -1, -1, -1, -1, -1},
	{12, -1, 12, 12, 12, 12, 12, 12, 12, 12, 12, -1, -1, -1, -1, -1, -1}
};

static const enum Action actionTable[24][17] = {
	{S, N, N, N, N, N, N, N, N, N, N, N, G, G, N, N, N},
	{S, N, N, N, N, N, N, N, N, N, N, A, N, G, N, N, N},
	{R, N, N, N, N, N, N, N, N, N, N, R, N, N, N, N, N},
	{N, S, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N},
	{R, N, N, N, N, N, N, N, N, N, N, R, N, N, N, N, N},
	{S, N, N, N, S, S, N, S, N, S, N, N, N, N, G, G, G},
	{N, N, S, S, N, N, N, N, N, N, N, N, N, N, N, N, N},
	{S, N, R, R, S, S, R, S, R, S, R, N, N, N, N, N, G},
	{R, N, R, R, R, R, R, R, R, R, R, N, N, N, N, N, N},
	{R, N, R, R, R, R, R, R, R, R, R, N, N, N, N, N, N},
	{R, N, R, R, R, R, R, R, R, R, R, N, N, N, N, N, N},
	{S, N, N, N, S, S, N, S, N, S, N, N, N, N, G, G, G},
	{S, N, N, N, S, S, N, S, N, S, N, N, N, N, G, G, G},
	{S, N, N, N, S, S, N, S, N, S, N, N, N, N, G, G, G},
	{R, N, N, N, N, N, N, N, N, N, N, R, N, N, N, N, N},
	{S, N, N, N, S, S, N, S, N, S, N, N, N, N, N, G, G},
	{R, N, R, R, R, R, R, R, R, R, R, N, N, N, N, N, N},
	{N, N, N, S, N, N, S, N, N, N, N, N, N, N, N, N, N},
	{N, N, N, S, N, N, N, N, S, N, N, N, N, N, N, N, N},
	{N, N, N, S, N, N, N, N, N, N, S, N, N, N, N, N, N},
	{S, N, R, R, S, S, R, S, R, S, R, N, N, N, N, N, G},
	{R, N, R, R, R, R, R, R, R, R, R, N, N, N, N, N, N},
	{R, N, R, R, R, R, R, R, R, R, R, N, N, N, N, N, N},
	{R, N, R, R, R, R, R, R, R, R, R, N, N, N, N, N, N}
};

/******* PARSER *********/

void Parser::reduce(int rule){
	int popNum;
	enum Type type;
	switch(rule){
		case 1:
		case 2:{
			type = SYNTAX;
			break;
		}
		case 3:{
			type = DEF;
			break;
		}
		case 4:
		case 5:{
			type = RULE;
			break;
		}
		case 6:
		case 7:{
			type = DER;
			break;
		}
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:{
			type = TERM;
			break;
		}
	}
	switch(rule){
		case 2:
		case 5:
		case 7:
		case 8:
		case 9:{
			popNum = 1;
			break;
		}
		case 1:
		case 6:{
			popNum = 2;
			break;
		}
		case 4:
		case 10:
		case 11:
		case 12:{
			popNum = 3;
			break;
		}
		case 3:{
			popNum = 4;
			break;
		}
	}

	TreeNode* node = new TreeNode();
	node->token = Token(type);

	while(popNum--){
		node->children.push_back(treeStk.top());
		stateStk.pop();
		treeStk.pop();
	}

	reverse(node->children.begin(), (*node).children.end());

	switch(rule){
		case 8:
		case 9:{
			Term::Type t = (node->children[0]->token.type == TERMINAL)?Term::Type::TERMINAL:Term::Type::NONTERMINAL;
			string& name = *node->children[0]->token.val;
			const Term* term = new Term(t, name);

			mp[node] = term;
			break;
		}

		case 7:{
			Term* term = (Term*)(mp[node->children[0]]);
			Rule* rule = new Rule();
			rule->addTerm(term);

			mp[node] = rule;
			break;
		}
		case 6:{
			Term* term = (Term*)(mp[node->children[1]]);
			Rule* rule = (Rule*)(mp[node->children[0]]);
			rule->addTerm(term);

			mp[node] = rule;
			break;
		}

		case 5:{
			Definition* def = new Definition(ast);
			Rule* rule = (Rule*)(mp[node->children[0]]);
			def->addRule(rule);

			mp[node] = def;
			break;
		}
		case 4:{
			Definition* def = (Definition*)(mp[node->children[0]]);
			Rule* rule = (Rule*)(mp[node->children[2]]);
			def->addRule(rule);

			mp[node] = def;
			break;
		}

		case 3:{
			Definition& def = *(Definition*)(mp[node->children[2]]);
			def.setLHS(*(node->children[0]->token.val));

			mp[node] = &def;
			break;
		}

		case 10:{
			Definition& def = *(Definition*)(mp[node->children[1]]);
			const string& name = "(" + def.getName() + ")";
			const Term* term = new Term(Term::NONTERMINAL, name);
			
			def.setLHS(term->getName());

			mp[node] = term;
			break;
		}
		case 11:{
			Definition& def = *(Definition*)(mp[node->children[1]]);
			const string& name = "[" + def.getName() + "]";
			const Term* term = new Term(Term::NONTERMINAL, name);

			def.addRule(new Rule());
			def.setLHS(term->getName());

			mp[node] = term;
			break;
		}
		case 12:{
			Definition& def = *(Definition*)(mp[node->children[1]]);
			const string& name = "{" + def.getName() + "}";
			const Term* term = new Term(Term::NONTERMINAL, name);

			def.addRule(new Rule());
			def.setLHS(term->getName());

			mp[node] = term;
			break;
		}

		case 1:
			break;
		case 2:{
			Definition* def = (Definition*)(mp[node->children[0]]);
			Rule* rule = new Rule();
			
			Term* term = new Term(Term::NONTERMINAL, def->getLHS());
			rule->addTerm(term);

			ast->addDefinition(ast->startSymbol(), rule);
			break;
		}

	}

	int state = table[stateStk.top()][type];

	treeStk.push(node);
	stateStk.push(state);
}

void Parser::shift(Token tkn, int state){
	TreeNode* n = new TreeNode();
	n->token = tkn;
	treeStk.push(n);
	stateStk.push(state);
}

Parser::Parser(Lexer l){
	lexer = l;
	ast = new Ast();
}

static void deleteTree(TreeNode* root){
	if(root->token.val != nullptr){
		delete root->token.val;
	}
	for(auto child: root->children){
		deleteTree(child);
	}
	delete root;
}

Ast* Parser::parse(){
	stateStk.push(0);
	while(true){
		Token tkn = lexer.peek();
		int state = stateStk.top();
		enum Action action = actionTable[state][tkn.type];
		int val = table[state][tkn.type];
		if(action == R){
			reduce(val);
			continue;
		}
		if(action == A){
			TreeNode* root = treeStk.top();
			treeStk.pop();
			stateStk.pop();

			deleteTree(root);
			mp.clear();

			return ast;
		}
		if(action == N){
			return {};
		}
		shift(tkn, val);
		lexer.consume();
	}
}
