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

void Rule::addTerm(const Term* term){
	if(terms.size() != 0){
		name += " ";
	}
	name += term->getName();
	terms.push_back(term);
}

void Rule::addTerm(const Term* term, int idx){
	terms.insert(terms.begin()+idx, term);
	name.clear();
	for(auto term : terms){
		name += term->getName() + " ";
	}
	if(name.size() != 0){
		name.pop_back();
	}
}

int Rule::id() const {
	return _id;
}

Rule::Rule(){
	static int cnt = 0;
	_id = ++cnt;
}

const string &Rule::getName() const {
	return name;
}

const Term *Rule::lhs() const{
	return _lhs;
}

void Rule::lhs(const Term *_lhs){
	this->_lhs = _lhs;
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
	const Term* lhs;
	string name;
	
	public:
	void addRule(Rule* rule){
		if(lhs != nullptr){
			rule->lhs(lhs);
		}
		if(rules.size() != 0){
			name += " | ";
		}
		name += rule->getName();
		rules.push_back(rule);
	}

	void setLHS(const Term *nt){
		lhs = nt;
		for(auto rule: rules){
			rule->lhs(lhs);
		}
	}

	const Term &getLHS() const {
		return *lhs;
	}

	const vector<Rule*>& getRules() const {
		return rules;
	}

	const std::string &getName() const {
		return name;
	}

	Definition(Ast* ast){
		this->ast = ast;
	}
};

/****** TERM ******/

const string &Term::getName() const {
	return name;
}

Term::Term(){}

Term::Term(enum Type type, const string& name){
	this->_type = type;
	this->name = name;
}

/******* TermStore ********/

bool Ast::TermStore::contains(const Term* term) const {
	return contains(term->getName());
}

bool Ast::TermStore::contains(const string& name) const {
	return ids.find(name) != ids.end();
}

int Ast::TermStore::query(const Term* term) const {
	return query(term->getName());
}

int Ast::TermStore::query(const string& name) const {
	if(contains(name)){
		return ids.find(name)->second;
	}
	return -1;
}

const Term& Ast::TermStore::query(int id) const {
	return *(terms[id]);
}

int Ast::TermStore::insert(const Term* term){
	static Term* END = new Term(Term::TERMINAL, "$");
	static Term* START = new Term(Term::NONTERMINAL, "S'");
	static bool loaded = false;
	if(!loaded){
		loaded = true;
		insert(END);
		insert(START);
	}
	if(!contains(term)){
		ids[term->getName()] = ids.size();
		terms.push_back(term);
		return ids.size()-1;
	}
	return ids[term->getName()];
}

bool Ast::TermStore::contains(int id) const {
	return id < terms.size();
}

int Ast::TermStore::size() const {
	return terms.size();
}

/****** AST *******/

bool Ast::addRule(const Rule* rule){
	if(rule == nullptr || rule->lhs() == nullptr){
		return false;
	}
	ruleStore.insert(rule);
	return true;
}

bool Ast::addRules(const vector<Rule*> &rules){
	for(auto rule: rules){
		if(rule == nullptr || rule->lhs() == nullptr || rule->lhs() != rules[0]->lhs()){
			return false;
		}
	}
	ruleStore.insert(rules);
	return true;
}

const vector<const Rule*>& Ast::getDefinition(const string& nt) const {
	return ruleStore.query(nt);
}

const vector<const Rule*>& Ast::getDefinition(const Term *nt) const {
	return ruleStore.query(nt);
}

const string &Ast::startSymbol() const {
	static string str = "S'";
	return str;
}

/****** RULESTORE *********/

void Ast::RuleStore::insert(const Rule* rule){
	const Term *t = rule->lhs();
	def[t->getName()].push_back(rule);
	int id = rule->id();
	if(v.size() < id+1){
		v.resize(id+1);
	}
	v[id] = rule;
}

void Ast::RuleStore::insert(const vector<Rule*> &rules){
	if(rules.size() == 0)	return;

	const Term *t = rules[0]->lhs();
	auto& vec = def[t->getName()];
	
	for(auto rule: rules){
		vec.push_back(rule);
		int id = rule->id();
		if(v.size() < id+1){
			v.resize(id+1);
		}
		v[id] = rule;
	}
}

inline const vector<const Rule*> &Ast::RuleStore::query(const Term *term) const {
	auto name = term->getName();
	return query(name);
}

const vector<const Rule*> &Ast::RuleStore::query(const string &name) const {
	static vector<const Rule*> empty;
	auto it = def.find(name);
	if(it == def.end()){
		return empty;
	}
	return it->second;
}

const Rule &Ast::RuleStore::query(int id) const {
	return *(v[id]);
}

int Ast::RuleStore::size() const {
	return def.size();
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
			const Term* term;
			if(ast->termStore.query(name) == -1){
				term = new Term(t, name);
				ast->termStore.insert(term);
			} else {
				term = &ast->termStore.query(ast->termStore.query(name));
			}

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
			string &name = *(node->children[0]->token.val);
			int id = ast->termStore.query(name);
			const Term* term = (id == -1)?(new Term(Term::NONTERMINAL, name)):&(ast->termStore.query(id));

			def.setLHS(term);

			for(auto rule: def.getRules()){
				rule->userProvided = true;
			}
			ast->addRules(def.getRules());

			mp[node] = &def;
			break;
		}

		case 10:{
			Definition& def = *(Definition*)(mp[node->children[1]]);
			const string& name = "(" + def.getName() + ")";
			const Term* term;
			if(ast->termStore.query(name) == -1){
				term = new Term(Term::NONTERMINAL, name);
				ast->termStore.insert(term);
			} else {
				term = &ast->termStore.query(ast->termStore.query(name));
			}

			def.setLHS(term);
			ast->addRules(def.getRules());

			mp[node] = term;
			break;
		}
		case 11:{
			Definition& def = *(Definition*)(mp[node->children[1]]);
			const string& name = "[" + def.getName() + "]";
			const Term* term;
			if(ast->termStore.query(name) == -1){
				term = new Term(Term::NONTERMINAL, name);
				ast->termStore.insert(term);
			} else {
				term = &ast->termStore.query(ast->termStore.query(name));
			}

			def.addRule(new Rule());
			def.setLHS(term);
			ast->addRules(def.getRules());

			mp[node] = term;
			break;
		}
		case 12:{
			Definition& def = *(Definition*)(mp[node->children[1]]);
			const string& name = "{" + def.getName() + "}";
			const Term* term;
			if(ast->termStore.query(name) == -1){
				term = new Term(Term::NONTERMINAL, name);
				ast->termStore.insert(term);
			} else {
				term = &ast->termStore.query(ast->termStore.query(name));
			}

			for(auto rule: def.getRules()){
				rule->addTerm(term, 0);
			}

			def.addRule(new Rule());
			def.setLHS(term);
			ast->addRules(def.getRules());

			mp[node] = term;
			break;
		}

		case 1:{
			break;
		}
		case 2:{
			Definition* def = (Definition*)(mp[node->children[0]]);
			Rule* rule = new Rule();
			int id = ast->termStore.query(&def->getLHS());
			
			const Term* term;
			if(id == -1){
				term = new Term(Term::NONTERMINAL, def->getLHS().getName());
				ast->termStore.insert(term);
			} else {
				term = &ast->termStore.query(id);
			}
			rule->addTerm(term);
			rule->lhs(&ast->termStore.query(ast->termStore.query(ast->startSymbol())));

			ast->addRule(rule);
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
			return nullptr;
		}
		shift(tkn, val);
		lexer.consume();
	}
}
