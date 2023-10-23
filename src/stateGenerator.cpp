using namespace std;

#include <stateGenerator.hpp>
#include <queue>
#include <set>
#include <iostream>
#include <climits>

/******** State ************/

static string& EMPTY = *(new string(""));
#define EPSILON_STR "ε"

class StateComponent {
	friend class State;

	set<string> &_follow;
	const Rule &_rule;
	int _idx;

	public:
	StateComponent(const Rule &r) : _rule(r), _idx(0), _follow(*(new set<string>())) {}
	StateComponent(const Rule& r, set<string> &followSet) : _rule(r), _idx(0), _follow(*(new set<string>(followSet))) {}
	
	string toString() const {
		return "[" + to_string(_rule.id()) + "," + to_string(_idx) + "]";
	}
	
	const Rule &rule() const {
		return _rule;
	}
	
	set<string> &followSet() const {
		return _follow;
	}

	int idx(){
		return _idx;
	}
};

struct Cmp {
	bool operator()(const StateComponent& l, const StateComponent& r) const {
		return l.toString() < r.toString();
	}
};

class State {
	map<string, set<StateComponent, Cmp>> expandable;
	set<StateComponent, Cmp> reducible;
	queue<StateComponent> unprocessed;

	string id;

	State(){}

	void addKernelComponent(const StateComponent& sc){
		const Term* cur = sc.rule().getTerm(sc._idx);
		if(cur == nullptr){
			reducible.insert(sc);
		}else{
			expandable[cur->getName()].insert(sc);
			unprocessed.push(sc);
		}
		id += sc.toString();
	}

	public:
	static State createStartState(const StateComponent& sc){
		State s;
		s.addKernelComponent(sc);
		return s;
	}

	const string& strId() const {
		return id;
	}

	bool addUniqueComponent(const StateComponent& sc){
		const Term* cur = sc.rule().getTerm(sc._idx);
		if(cur == nullptr && reducible.find(sc) != reducible.end()){
			set<string> &f = reducible.find(sc)->followSet();
			for(auto& str: sc.followSet()){
				f.insert(str);
			}
			return false;
		}
		if(cur == nullptr){
			reducible.insert(sc);
			unprocessed.push(sc);
			return true;
		}
		auto itr1 = expandable.find(cur->getName());
		if(itr1  != expandable.end() && itr1->second.find(sc) != itr1->second.end()){
			set<string> &f = itr1->second.find(sc)->followSet();
			for(auto& str: sc.followSet()){
				f.insert(str);
			}
			return false;
		}
		expandable[cur->getName()].insert(sc);
		unprocessed.push(sc);
		return true;
	}

	const vector<pair<string, State>> shifts() const {
		vector<pair<string, State>> res;
		for(auto def: expandable){
			State s;
			res.emplace_back(def.first, s);
			for(auto sc: def.second){
				sc._idx++;
				res[res.size()-1].second.addKernelComponent(sc);
			}
		}
		return res;
	}

	const set<StateComponent, Cmp> &reduces() const {
		return reducible;
	}

	queue<StateComponent> &processingQueue(){
		return unprocessed;
	}
};


/****** StateStore ********/

int StateGenerator::StateStore::addState(State& state){
	const string& str = state.strId();
	if(!contains(state)){
		states[mp.size()] = new State(state);
		return mp[str] = mp.size();
	}
	return mp[str];
}

bool StateGenerator::StateStore::contains(State& state){
	const string& str = state.strId();
	return mp.find(str) != mp.end();
}

int StateGenerator::StateStore::getId(State& state){
	if(!contains(state))	return -1;
	return mp[state.strId()];
}

State& StateGenerator::StateStore::referenceTo(int id){
	return *(states[id]);
}

int StateGenerator::StateStore::size(){
	return states.size();
}

/******* TermStore ********/

bool StateGenerator::TermStore::contains(const Term* term) const {
	return contains(term->getName());
}

bool StateGenerator::TermStore::contains(const string& name) const {
	return ids.find(name) != ids.end();
}

int StateGenerator::TermStore::query(const Term* term) const {
	return query(term->getName());
}

int StateGenerator::TermStore::query(const string& name) const {
	if(contains(name)){
		return ids.find(name)->second;
	}
	return -1;
}

const Term& StateGenerator::TermStore::query(int id) const {
	return *(terms[id]);
}

int StateGenerator::TermStore::insert(const Term* term){
	if(!contains(term)){
		ids[term->getName()] = ids.size();
		terms.push_back(term);
		return ids.size()-1;
	}
	return ids[term->getName()];
}

bool StateGenerator::TermStore::contains(int id) const {
	return id < terms.size();
}

int StateGenerator::TermStore::size() const {
	return terms.size();
}


/******** StateGenerator *********/

State& StateGenerator::computeClosure(State& kernel){
	auto& q = kernel.processingQueue();
	while(!q.empty()){
		auto& sc = q.front();
		if(sc.rule().getTerm(sc.idx()) == nullptr){
			q.pop();
			continue;
		}

		set<string> * followSet = new set<string>();
		int offset = 1;
		while(true){
			if(sc.rule().getTerm(sc.idx()+offset) == nullptr){
				for(auto& str: sc.followSet()){
					followSet->insert(str);
				}
				break;
			} else {
				bool epsilon = false;
				for(auto& str: firstOf(termStore.query(sc.rule().getTerm(sc.idx()+offset)))){
					if(str == EPSILON_STR){
						epsilon = true;
						continue;
					}
					followSet->insert(str);
				}
				if(!epsilon)	break;
			}
			offset++;
		}

		for(auto rule: _ast.getDefinition(sc.rule().getTerm(sc.idx())->getName())){
			StateComponent scc(*rule, *followSet);
			kernel.addUniqueComponent(scc);
		}

		q.pop();
		delete followSet;
	}

	return kernel;
}

void StateGenerator::generateStateTable(){
	//	Bootstrap
	fillUpTermStore();

	for(int i = 0; i < termStore.size(); i++){
		cout << i << " " << termStore.query(i).getName() << endl;
	}

	StateComponent sc(*_ast.getDefinition(_ast.startSymbol())[0]);
	sc.followSet().insert("$");
	State s = State::createStartState(sc);

	queue<int> q;
	q.push(store.addState(s));

	int maxID = q.front();

	// Loop
	while(!q.empty()){
		_table.resize(maxID+1);

		auto from = q.front(); q.pop();
		State& state = store.referenceTo(from);
				
		computeClosure(state);

		_table[from].resize(termStore.size());
		for(int i = 0; i < termStore.size(); i++){
			_table[from][i].type = Action::REJECT;
			_table[from][i].val = -1;
		}
		
		for(auto pairs: state.shifts()){
			auto& step = pairs.first;
			auto& kernel = pairs.second;

			if(!store.contains(kernel)){
				q.push(store.addState(kernel));
			}
			int to = store.getId(kernel);
			maxID = max(to, maxID);

			int termID = termStore.query(step);
			auto& term = termStore.query(termID);
			if(term.type() == Term::NONTERMINAL){
				_table[from][termID].type = Action::GOTO;
			} else {
				_table[from][termID].type = Action::SHIFT;
			}
			_table[from][termID].val = to;
		}

		for(auto& sc : state.reduces()){
			int ruleId = sc.rule().id();
			for(auto& str: sc.followSet()){
				int on = termStore.query(str);
				_table[from][on].type = Action::REDUCE;
				_table[from][on].val = ruleId;
			}
		}
	}

}

void StateGenerator::fillUpTermStore(){
	static Term* END = new Term(Term::TERMINAL, "$");
	if(!termStoreFilled){
		termStore.insert(END);
		queue<string> q;
		q.push(_ast.startSymbol());

		while(!q.empty()){
			const vector<Rule*> rules = _ast.getDefinition(q.front());
			q.pop();
			for(auto rule: rules){
				for(auto term : rule->getTerms()){
					if(!termStore.contains(term)){
						termStore.insert(term);
						q.push(term->getName());
					}
				}
			}
		}
		termStoreFilled = true;
	}
}

#define UNDER_PROCESS 1
#define NOT_PROCESSED 2
#define PROCESSED 3

const set<string> &StateGenerator::firstOf(int id){
	static vector<set<string>> firsts;
	static vector<int> calculated;
	if(firsts.size() == 0){
		fillUpTermStore();
		firsts.resize(termStore.size(), set<string>());
		calculated.resize(termStore.size(), NOT_PROCESSED);
	}

	if(calculated[id] == PROCESSED){
		return firsts[id];
	}
	calculated[id] = UNDER_PROCESS;

	const Term &term = termStore.query(id);
	if(term.type() == Term::TERMINAL){
		firsts[id].insert(term.getName());
		calculated[id] = PROCESSED;
		return firsts[id];
	}

	bool processed = true;
	auto rules = _ast.getDefinition(term.getName());
	bool epsilon1 = false, epsilon2 = false, epsilon = false, epsilonAssumption = false;
	set<string> ifEpsilon;
	for(auto rule: rules){
		if(rule->getTerms().size() == 0){
			epsilon1 = true;
		}
		for(auto term : rule->getTerms()){
			epsilon2 = true;
			int termId = termStore.query(term);
			if(termId != id){
				if(calculated[termId] == UNDER_PROCESS){
					processed = false;
					continue;
				}
				for(auto str: firstOf(termId)){
					if(epsilonAssumption){
						ifEpsilon.insert(str);
					}else{
						firsts[id].insert(str);
					}
				}
			}else{
				epsilonAssumption = !epsilon;
				continue;
			}
			if(firsts[id].find(EPSILON_STR) == firsts[id].end()){
				epsilon2 = false;
				break;
			}
		}
		epsilonAssumption = false;
		epsilon = epsilon | epsilon1 | epsilon2 ;
	}
	if(epsilon){
		firsts[id].insert(EPSILON_STR);
		for(auto str: ifEpsilon){
			firsts[id].insert(str);
		}
	}
	if(processed){
		calculated[id] = PROCESSED;
	} else {
		calculated[id] = NOT_PROCESSED;
	}
	ifEpsilon.clear();
	return firsts[id];
}
