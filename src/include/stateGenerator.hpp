#ifndef STATEGENERATOR_H
#define STATEGENERATOR_H

#include <map>
#include <set>
#include <string>
#include "parser.hpp"

class State ;

struct Action{
	enum Type {
		SHIFT,
		REDUCE,
		GOTO,
		REJECT,
		ACCEPT
	} type;
	int val;
};

class StateGenerator {

	public:

	class StateStore {
		std::map<std::string, int> mp;
		std::map<int, State*> states;

		public:
		int addState(State& state);
		bool contains(State& state);
		int getId(State& state);
		int size();
		State& referenceTo(int id);
	};

	class TermStore {
		std::map<std::string, int> ids;
		std::vector<const Term*> terms;

		public:
		int insert(const Term* term);
		bool contains(const Term* term) const ;
		bool contains(const std::string& name) const;
		bool contains(int id) const ;
		int query(const Term* term) const;
		int query(const std::string& name) const;
		const Term& query(int id) const;
		int size() const;
	};

	private:
	StateStore store;
	TermStore termStore;
	Ast& _ast;
	std::vector<std::vector<Action>> _table;
	bool termStoreFilled = false;
	void fillUpTermStore();
	const std::set<std::string> &firstOf(int id);
	State& computeClosure(State& kernel);

	public:

	StateGenerator(Ast& ast) : _ast(ast) {}
	void generateStateTable();
	std::vector<std::vector<Action>>& table() {return _table;}
};

#endif