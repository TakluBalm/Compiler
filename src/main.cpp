#include <lexer.hpp>
#include <parser.hpp>
#include <stateGenerator.hpp>
#include <iostream>
#include <set>
#include <map>
#include <queue>

using namespace std;

int main(int argc, char** argv){
	cout << "File: ";
	string filename; cin >> filename;
	FILE* file = fopen(filename.c_str(), "r");
	if(file == NULL){
		cout << "File could not be opened\n";
		return 1;
	}
	Lexer lex(file);
	Parser parser(lex);
	Ast* ast = parser.parse();
	if(ast != nullptr){
		cout << "Successfully parsed\n";
		map<string, bool> mp;
		string start = ast->startSymbol();
		queue<string> q;
		set<const Term*> allTerms;
		q.push(start);
		while(!q.empty()){
			auto top = q.front(); q.pop();
			mp[top] = true;
			const vector<Rule*>& rules = ast->getDefinition(top);
			cout << top << ":\n";
			for(auto rule: rules){
				vector<const Term*> terms = rule->getTerms();
				for(auto term: terms){
					allTerms.insert(term);
					string sym = term->getName();
					if(term->type() == term->NONTERMINAL && !mp[sym]){
						q.push(sym);
					}
				}
				if(terms.empty()){
					cout << rule->id() << "\t\u03B5\n";
				}else{
					cout << rule->id() <<  "\t" << rule->getName() << endl;
				}
			}
		}

		cout << endl << endl;

		StateGenerator sg(*ast);

		sg.generateStateTable();
		auto& table = sg.table();

		int maxVal = 0;
		for(auto row: table){
			for(auto act: row){
				maxVal = max(maxVal, act.val);
			}
		}
		maxVal = max(maxVal, (int)table[0].size());
		
		int maxLen = to_string(maxVal).size()+1;

		for(int i = 0; i < maxLen; i++){
			cout << " ";
		}
		cout << "|";
		for(int i = 0; i < table[0].size(); i++){
			auto str = to_string(i);
			for(int j = 0; j + str.size() < maxLen; j++)	cout << " ";
			cout << str << "|";
		}
		cout << endl;

		int idx = 0;
		for(auto row: table){
			auto str = to_string(idx++);
			for(int i = 0; i + str.size() < maxLen; i++){
				cout << " ";
			}
			cout << str << "|";

			for(auto act: row){
				if(act.type == Action::REJECT){
					for(int i = 0; i < maxLen; i++){
						cout << " ";
					}
					cout << "|";
					continue;
				}
				auto str = to_string(act.val);
				for(int i = 0; i + str.size() + 1 < maxLen; i++){
					cout << " ";
				}
				if(act.type == Action::SHIFT)	cout << "S";
				if(act.type == Action::REDUCE)	cout << "R";
				if(act.type == Action::GOTO)	cout << "G";

				cout << str << "|";
			}
			cout << endl;
		}

	}
}