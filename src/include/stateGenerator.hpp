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

	private:
	StateStore store;
	const Ast& _ast;
	std::vector<std::vector<Action>> _table;
	bool termStoreFilled = false;
	const std::set<std::string> &firstOf(int id);
	State& computeClosure(State& kernel);

	public:

	StateGenerator(Ast& ast) : _ast(ast) {}
	bool generateStateTable();
	const std::vector<std::vector<Action>>& table() {return _table;}
};

#endif