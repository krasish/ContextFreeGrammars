#include "Grammar.h"

const unsigned ENGLISH_ALPHABET_COUNT = 26;
const unsigned MAX_INDEX_FOR_NT = 1024;

const string BLACK_TEXT = "\x1B[30m";
const string RED_TEXT = "\x1B[31m";
const string GREEN_TEXT = "\x1B[32m";
const string YELLOW_TEXT = "\x1B[33m";
const string BLUE_TEXT = "\x1B[34m";
const string MAGENTA_TEXT = "\x1B[35m";
const string CYAN_TEXT = "\x1B[36m";
const string WHITE_TEXT = "\x1B[37m";
const string BRIGHT_BLACK_TEXT = "\x1B[90m";
const string BRIGHT_RED_TEXT = "\x1B[91m";
const string BRIGHT_GREEN_TEXT = "\x1B[92m";
const string BRIGHT_YELLOW_TEXT = "\x1B[93m";
const string BRIGHT_BLUE_TEXT = "\x1B[94m";
const string BRIGHT_MAGENTA_TEXT = "\x1B[95m";
const string BRIGHT_CYAN_TEXT = "\x1B[96m";
const string BRIGHT_WHITE_TEXT = "\x1B[97m";
const string RESET_COLORING = "\033[0m";

bool stringOccursInVector(const vector<string>& vec,const string& str) {
	bool isFound = false;
	for (auto vit = vec.cbegin();!isFound && vit != vec.cend(); vit++) {
		isFound = (*vit) == str;
	}
	return isFound;
}

bool Grammar::isTerminal(const char c) const {
	return isSmallLetter(c) || isDigit(c) ||  (c == '@');
}

//Valid nonTerminals look like 'A' or 'A_1_' or 'A_12_' etc.
bool Grammar::isNonTerminal(const string& s) const {
	if (s.length() == 1) {
		return s[0] >= 'A' && s[0] <= 'Z';
	}
	///No possible correct terminals of length 2 or 3 exist
	else if (s.length() == 2 || s.length() == 3) {
		return false;
	}
	else {
		bool correctNumber = true;
		for (unsigned i = 2; correctNumber && i < s.length() - 1; i++) {
			if (i == 2)
				correctNumber = s[i] >= '1' && s[i] <= '9';		//A_0_ is considered incorrect
			else
				correctNumber = isDigit(s[i]);
		}
		return isCapitalLetter(s[0]) && (s[1] == '_') && correctNumber && (s[s.length() - 1] == '_');
	}
}

bool Grammar::isTerminalSet(const terminalSet& a) const {
	if (a.size() == 0) {
		cerr << RED_TEXT << "Attempt to create grammar with empty terminal set!" << RESET_COLORING << endl;
		assert(false);
	}
	bool isValid = true;
	for (terminalSet::const_iterator it = a.begin();isValid && it != a.end(); it++) {
		isValid = isSmallLetter((*it)) || isDigit((*it)) ;
	}
	return isValid;
}

bool Grammar::isNonTerminalSet(const nonTerminalSet& a) const {
	if (a.size() == 0) {
		cerr << RED_TEXT << "Attempt to create grammar with empty non-terminal set!" << RESET_COLORING << endl;
		assert(false);
	}
	bool isValid = true;
	for (nonTerminalSet::const_iterator it = a.begin(); isValid && it != a.end(); it++) {
		isValid = isNonTerminal(*it);
	}
	return isValid;
}

//Existing terminal = terminal from current set of terminals or @
bool Grammar::isExistingTerminal(const char &t) const {
	if (t == '@')
		return true;
	bool isTerminal = false; 
	terminalSet::const_iterator it= terminals.begin();
	while (!isTerminal && (it != terminals.end())) {
		isTerminal = (t == *it);
		it++;
	}
	return isTerminal;
}

//Existing not-terminal = terminal from current set of terminals
bool Grammar::isExistingNonTerminal(const string& nt) const {
	return stringOccursInVector(nonTerminals, nt);
}

//Cheks whether NT is of current set and product consists of NT/Ts of current set. Rules which look like A->A are useless and considered invalid.
bool Grammar::isValidProductionRule(const ProductionRule &rule) const {
	bool validNonTerminal = isExistingNonTerminal(rule.nonTerminal);
	bool validProduct = rule.product.size() > 0;
	if (rule.product.size() == 1 && (rule.nonTerminal == rule.product[0]))
		return false;
	for (const auto& it : rule.product) {
			validProduct = isExistingTerminal(it[0]) || isExistingNonTerminal(it);
		
	}
	return validNonTerminal && validProduct;
}

//Returns number of rules which start with given nonterminal
unsigned Grammar::rulesOfNonTerminalCount(const string& s) const {
	unsigned count = 0;
	for (productions::const_iterator it = rules.begin(); it != rules.end(); it++) {
		if ((*it).nonTerminal == s)
			count++;
	}
	return count;
}

string Grammar::createIndexedNonTerminal(char letter, unsigned index) const {
	string result;
	result += letter;
	result += '_';
	result += std::to_string(index);
	result += '_';
	return result;
}

string Grammar::generateNonTerminalName(char prefLetter) const {
	string newNonTerminal;
	if (prefLetter && !isDigit(prefLetter)) {
		if (isSmallLetter(prefLetter)) {
			prefLetter -= 32;
		}
		newNonTerminal = prefLetter;
		if (!isExistingNonTerminal(newNonTerminal)) {
			return newNonTerminal;
		}
		else {
			for (unsigned i = 1; i <= MAX_INDEX_FOR_NT * 4; i++) { //since function has been called with preference index cap becomes 4096
				newNonTerminal = createIndexedNonTerminal(prefLetter, i);
				if (!isExistingNonTerminal(newNonTerminal))
					return newNonTerminal;
			}
		}
	}
	/*
	If no preference argument was passed function will return single capital letter
	if there is available one. Otherwise it wil return capital letter with smallest index
	i.e. B_1_ (in case all capital letters are taken and A_1_ is taken)
	*/
	for (unsigned i = 0; i < ENGLISH_ALPHABET_COUNT; i++) {
		newNonTerminal = 'A';
		if (!isExistingNonTerminal(newNonTerminal)) {
			return newNonTerminal;
		}
		newNonTerminal[0]++;
	}
	for (unsigned i = 1; i <= MAX_INDEX_FOR_NT; i++) {
		for (int j = 0; j < ENGLISH_ALPHABET_COUNT; j++) {
			newNonTerminal = createIndexedNonTerminal(('A' + j), i);
			if (!isExistingNonTerminal(newNonTerminal)) {
				return newNonTerminal;
			}
		}
	}
}

//Adds non-terminals and returns true or returns false if nonterminal has illegal name/already exists.
bool Grammar::addNonTerminal(string nt) {
	if (isNonTerminal(nt) && !isExistingNonTerminal(nt)) {
		nonTerminals.push_back(nt);
		return true;
	}
	else {
		return false;
	}
}

bool Grammar::RuleAlreadyExists(const ProductionRule& pr) const{
	bool alreadyExists = false;
	for (auto it = rules.cbegin() ; !alreadyExists && it != rules.cend() ; it++) {
		bool sameProduct = pr.product.size() == (*it).product.size();
		for (unsigned i = 0; sameProduct && i < pr.product.size(); i++)
			sameProduct = (pr.product[i] == (*it).product[i]);
		alreadyExists = sameProduct && (pr.nonTerminal == (*it).nonTerminal);
	}
	return alreadyExists;
}

//Sorts rules by NT in same order NTs are sorted in NT vector
void Grammar::sortRules() {
	int counter = 0;
	for (auto& nt : nonTerminals) {
		for (int i = counter; i < rules.size(); i++) {
			if (rules[i].nonTerminal == nt) {
				std::swap(rules[i], rules[counter]);
				counter++;
			}
		}
	}
}

Grammar::Grammar(): nonTerminals(), terminals(), rules(), startSymbol(), Identidied() {}

Grammar::Grammar(nonTerminalSet nonTerminals, terminalSet terminals, productions rules, string startSymbol, unsigned id) :
	nonTerminals(nonTerminals), terminals(terminals), rules(rules), startSymbol(startSymbol), Identidied(id) {
	if (!isNonTerminalSet(nonTerminals)) {
		cerr << RED_TEXT << "Attempt to create grammar with invalid non-terminal set!" << RESET_COLORING << endl;
		assert(false);
	}
	if (!isTerminalSet(terminals)) {
		cerr << RED_TEXT << "Attempt to create grammar with ivalid terminal set!" << RESET_COLORING << endl;
		assert(false);
	}
	unsigned rulesCount = rules.size();
	for (unsigned i = 0; i < rulesCount; i++) {
		if (!isValidProductionRule(rules[i])) {
			cerr << RED_TEXT << "Attempt to create grammar with invalid production rule '" <<rules[i].nonTerminal << "->" << "\033[0m";
			for (auto& rp : rules[i].product) { //print product which is vector of string
				cerr << RED_TEXT << rp << RESET_COLORING;
			}
			cerr << RED_TEXT << "'!" << RESET_COLORING << endl;
			assert(false);
		}
	}
	if (!isExistingNonTerminal(startSymbol)) {
		cerr << RED_TEXT << "Attempt to create grammar with invalid starting symbol!" << RESET_COLORING << endl;
		assert(false);
	}
}

Grammar::Grammar(const Grammar& other) : nonTerminals(), terminals(), rules(), startSymbol(), Identidied() {
		this->nonTerminals = other.nonTerminals;
		this->terminals = other.terminals;
		this->rules = other.rules;
		this->startSymbol = other.startSymbol;
		this->id = other.id;
}

Grammar& Grammar::operator=(const Grammar& other) {
	if (this != &other) {
		this->nonTerminals = other.nonTerminals;
		this->terminals = other.terminals;
		this->rules = other.rules;
		this->startSymbol = other.startSymbol;
		this->id = other.id;
	}
	return *this;
}

Grammar& Grammar::Union(const Grammar& other) const {

	//Check if grammars have same terminal sets. If not, operation cannot be performed. 
	//Assertion is made by checking whether number of Ts are same and if every T of the first grammar is found in other grammar.

	bool correctTerminals = other.terminals.size() == this->terminals.size();
	for (auto it = this->terminals.cbegin() ;correctTerminals && it != this->terminals.cend(); it++) {
		for (auto jt = other.terminals.cbegin();!correctTerminals && jt != other.terminals.cend(); jt++) {
			correctTerminals = (*it == *jt);
		}
	}
	
	Grammar* unitedGrammar = new Grammar(*this); //Resulting grammar is same as this grammar with added rules of other grammar and additional starting symbol
	
	if (!correctTerminals) { //will return empty grammar
		cerr << RED_TEXT << "Cannot perform 'union' operation on grammars with different terminal sets!" << RESET_COLORING << endl;
		return *unitedGrammar;
	}
	unitedGrammar->id = 0;

	map<string, string> renamedArr; //key: NT, value: NTs new name

	//	The sets of nonterminals of the 2 grammars need to be disjoined. Non-terminals with repeted names will be renamed in new grammar.
	//	In order to make sure production rules will stay correct we create an array which contains pairs of non-terminals and their new names.
	
	for (auto it = other.nonTerminals.cbegin(); it != other.nonTerminals.cend(); it++) { //add second grammar's NTs and rename them if neccessary
		if (!(*unitedGrammar).addNonTerminal(*it)) { //hover addNonTerminal
			string second = (*unitedGrammar).generateNonTerminalName((*it)[0]);
			(*unitedGrammar).addNonTerminal(second);
			renamedArr.emplace(*it, second);
		}
	}
	for (auto it = other.rules.cbegin(); it != other.rules.cend(); it++) { //iterate over second grammar's rules and add rules of second grammar
																		   //to resulting grammar with new names of renamed NTs id any renaming was done.
		ProductionRule temp = (*it);									   //Every rule is copied to temp and renamed there if neccesary.
		for (auto jt = renamedArr.begin(); jt != renamedArr.end(); jt++) {
			if(temp.nonTerminal == (*jt).first)
				temp.nonTerminal = (*jt).second;
			for (auto kt = temp.product.begin(); kt != temp.product.end(); kt++) {
				if ((*kt) == (*jt).first) {
					*kt = (*jt).second;
				}
			}
		}
		(*unitedGrammar).rules.push_back(temp);
	}

	//Add new starting symbol and its rules as in algorithm
	string startingNonTerminal = (*unitedGrammar).generateNonTerminalName('S');
	(*unitedGrammar).addNonTerminal(startingNonTerminal);
	(*unitedGrammar).startSymbol = startingNonTerminal;
	ProductionRule temp1 = { startingNonTerminal, {this->startSymbol} };
	ProductionRule temp2 = { startingNonTerminal, {other.startSymbol} };
	
	//Check if second grammar's starting symbol was renamed. 
	//Only second grammar's symbols are renamed so this verification is not needed for first grammar's starting symbol.
	
	for (auto jt = renamedArr.begin(); jt != renamedArr.end(); jt++) {
		if (other.startSymbol == (*jt).first) {
			temp2.product[0] = (*jt).second;
		}
	}
	//Print renamed symbols for easier understanding of product grammar
	for (auto jt = renamedArr.begin(); jt != renamedArr.end(); jt++) {
		cout << CYAN_TEXT << "\tInfo 'Union': Non-terminal " << (*jt).first << " of Grammar<" << other.id << "> was renamed to " << (*jt).second << RESET_COLORING << endl;
	}
	(*unitedGrammar).rules.push_back(temp1);
	(*unitedGrammar).rules.push_back(temp2);
	return *unitedGrammar;
}

Grammar& Grammar::Concat(const Grammar& other) const {
	bool correctTerminals = other.terminals.size() == this->terminals.size();
	for (auto it = this->terminals.cbegin(); correctTerminals && it != this->terminals.cend(); it++) {
		for (auto jt = other.terminals.cbegin(); !correctTerminals && jt != other.terminals.cend(); jt++) {
			correctTerminals = (*it == *jt);
		}
	}
	Grammar* concatenatedGrammar = new Grammar;
	if (!correctTerminals) { //will return empty grammar
		cerr << RED_TEXT << "Cannot perform 'concat' operation on grammars with different terminal sets!" << RESET_COLORING << endl;
		return *concatenatedGrammar;
	}
	*concatenatedGrammar = *this;
	concatenatedGrammar->id = 0;
	map<string, string> renamedArr;
	//	The sets of nonterminals of the 2 grammars need to be disjoined. Non-terminals with repeted names will be renamed in new grammar.
	//	In order to make sure production rules will stay correct we create and array wich contains pairs of non-terminals and their new names.

	for (auto it = other.nonTerminals.cbegin(); it != other.nonTerminals.cend(); it++) {
		if (!(*concatenatedGrammar).addNonTerminal(*it)) { //hover addNonTerminal
			string second = (*concatenatedGrammar).generateNonTerminalName((*it)[0]);
			(*concatenatedGrammar).addNonTerminal(second);
			renamedArr.emplace(*it, second);
		}
	}
	for (auto it = other.rules.cbegin(); it != other.rules.cend(); it++) {
		ProductionRule temp = (*it);
		for (auto jt = renamedArr.begin(); jt != renamedArr.end(); jt++) {
			if (temp.nonTerminal == (*jt).first)
				temp.nonTerminal = (*jt).second;
			for (auto kt = temp.product.begin(); kt != temp.product.end(); kt++) {
				if ((*kt) == (*jt).first) {
					*kt = (*jt).second;
				}
			}
		}
		(*concatenatedGrammar).rules.push_back(temp);
	}

	//So far logic is identical to Union function

	string startingNonTerminal = (*concatenatedGrammar).generateNonTerminalName('S');
	(*concatenatedGrammar).addNonTerminal(startingNonTerminal);
	(*concatenatedGrammar).startSymbol = startingNonTerminal;
	ProductionRule temp1 = { startingNonTerminal, {this->startSymbol, other.startSymbol} };
	
	//Check if second grammar's starting symbol was renamed. 
	//Only second grammar's symbols are renamed so this verification is not needed for first grammar's starting symbol.

	for (auto jt = renamedArr.begin(); jt != renamedArr.end(); jt++) {
		if (other.startSymbol == (*jt).first) {
			temp1.product[1] = (*jt).second;
		}
	}
	(*concatenatedGrammar).rules.push_back(temp1);

	//Print renamed symbols for easier understanding of resulting grammar
	for (auto jt = renamedArr.begin(); jt != renamedArr.end(); jt++) {
		cout << CYAN_TEXT << "\tInfo 'Concat': Non-terminal " << (*jt).first << " of Grammar<" << other.id << "> was renamed to " << (*jt).second << RESET_COLORING << endl;
	}
	return *concatenatedGrammar;
}

Grammar& Grammar::Iter() const {

	Grammar* iteratedGrammar = new Grammar(*this); //Iteration basically returns same grammar with several added rules and new starting symbol
	
	//Add new NT and rules as in algorithm
	(*iteratedGrammar).id = 0;
	string nr = (*iteratedGrammar).generateNonTerminalName('S');
	(*iteratedGrammar).addNonTerminal(nr);
	(*iteratedGrammar).startSymbol = nr;
	ProductionRule p1 = { nr, { "@" } };
	ProductionRule p2 = { nr, { this->startSymbol, nr } };
	iteratedGrammar->rules.push_back(p1);
	iteratedGrammar->rules.push_back(p2);

	return (*iteratedGrammar);
}

bool Grammar::Chomsky() const {
	bool isChomskyfied = true;
	for (auto it = rules.cbegin();isChomskyfied && it != rules.cend(); it++) { //In CNF rules' products' lenghts are 1 or 2
		if ((*it).product.size() == 1) { 
			if ((*it).product[0] == "@") { //if epsilon rule exists it needs to be form the starting symbol
				isChomskyfied = this->startSymbol == (*it).nonTerminal;
			}
			isChomskyfied = isExistingTerminal((*it).product[0][0]); //If product has lenght 1 it needs to be a terminal
		}
		else if ((*it).product.size() == 2) { //If product has lenght 1 it needs to be a terminal
			isChomskyfied = isExistingNonTerminal((*it).product[0]);
			isChomskyfied = isExistingNonTerminal((*it).product[1]);
		}
		else
			isChomskyfied = false;
	}
	return isChomskyfied;
}

void Grammar::Chomskify() {
	std::map<char, string> generatedNonTerminals;					// key: terminal , value: generated nonterminal

	//Remove rules with productions of terminals and nonterminals. 
	for (unsigned i = 0; i < rules.size(); i++) {
		for (auto& jt : rules[i].product) {
			if (rules[i].product.size() > 1) {
				if (isExistingTerminal(jt[0])) {	// current product has size >=2 -> every terminal needs to be replaced with newly generated non-terminal
					string newNt;
					std::map<char, string>::iterator mapIterator;		//check if such nonTerminal has already been generated to avoid creating useless NTs
					mapIterator = generatedNonTerminals.find(jt[0]);
					if (mapIterator == generatedNonTerminals.end()) {   //if not create NT
						newNt = generateNonTerminalName(jt[0]);
						addNonTerminal(newNt);
						ProductionRule newRule = { newNt, {jt} };	// create new NT->T rule (e.g. D->d)
						if (!RuleAlreadyExists(newRule) && isValidProductionRule(newRule))
							rules.push_back(newRule);

						generatedNonTerminals.emplace(jt[0], newNt);	//replace T with new NT
					}
					else {
						newNt = (*mapIterator).second;
					}
					jt.assign(newNt);
				}
			}
		}
	}

	//Remove long rules		
	for (unsigned i = 0; i < rules.size(); i++) {
		if (rules[i].product.size() > 2) {
			//Save odd part in new vector
			vector<string> oddNonTermials;
			vector<string>::const_iterator start = rules[i].product.begin() + 1;
			vector<string>::const_iterator end = rules[i].product.end();
			oddNonTermials.assign(start, end);

			//Remove odd part from current product
			rules[i].product.erase(start, end);

			//Generate new NT for odd part and add rule with it
			string newNt = generateNonTerminalName();
			addNonTerminal(newNt);
			rules[i].product.push_back(newNt);

			ProductionRule tempRule = { newNt, oddNonTermials };
			if (!RuleAlreadyExists(tempRule) && isValidProductionRule(tempRule))
				rules.push_back(tempRule);
		}
	}

	//Remove epsilon rules
	vector<string> epsilonNT;	//Contains every nonterinal which can be directly or indirectly be repaced with @

	bool epsilonFromStartNeeded = false; //If any epsilon rule is not deleted logic in next algorithm breaks. 
										 //Since CNF allows epsilon rule from starting symbol it will be deleted and then added at the end of the funciton if neccesary.
	///Check for direct @ productions and add to array (e.g A->@)
	for (unsigned i = 0; i < rules.size(); i++) {
		if ((rules[i].product.size() == 1) && (rules[i].product[0] == "@")) {
			epsilonNT.push_back(rules[i].nonTerminal);
			epsilonFromStartNeeded = rules[i].nonTerminal == this->startSymbol;		//If S->@ existed then add it at the end
			std::vector<ProductionRule>::iterator tempIt = rules.begin() + i;
			rules.erase(tempIt);
			i--; //one rule is erased
			
		}
	}

	///Check for indirect @ productions and add to array (e.g. A->B B->@)
	bool addedTerminal = false;
	do {
		addedTerminal = false; //if this flag becomes true epsilonNT now has new elements and iteration needs to be executed again
		for (auto it = rules.cbegin(); it != rules.cend(); it++) {
			bool isEpsilonNt = true;
			for (auto jt : (*it).product) { //check if product consists of only epsilonNTs. If so then current rule's NT is epsilon NT.
				bool currentIsEpsilonNt = stringOccursInVector(epsilonNT, jt);
				if (!currentIsEpsilonNt) {
					isEpsilonNt = false;
				}
			}
			if (isEpsilonNt) {
				bool alreadyExists = stringOccursInVector(epsilonNT, (*it).nonTerminal);
				if (!alreadyExists) {
					if ((*it).nonTerminal == this->startSymbol)
						epsilonFromStartNeeded = true;
					epsilonNT.push_back((*it).nonTerminal);
					addedTerminal = true;
				}
			}
		}
	} while (addedTerminal);

	bool addedRule;

	do {
		addedRule = false;
		for (const auto& rule : rules) { //Iterate through all the rules to add new ones as in algorithm
			for (unsigned j = 0; j < rule.product.size(); j++) {
				bool isEpsilonNt = false;
				for (auto kt = epsilonNT.cbegin(); !isEpsilonNt && kt != epsilonNT.cend(); kt++) {
					isEpsilonNt = rule.product[j] == (*kt); //If any of product parts is epsilon NT then new rule should be added.
				}
				if (isEpsilonNt) {  //Add new rule with same product omitting current epsilon NT
					vector<string> tempProduct = rule.product;
					vector<string>::iterator tempIt = tempProduct.begin();
					tempIt += j;
					tempProduct.erase(tempIt);
					ProductionRule tempRule = { rule.nonTerminal, tempProduct };
					if (!RuleAlreadyExists(tempRule) && isValidProductionRule(tempRule)) {
						rules.push_back(tempRule);
						addedRule = true;
					}
				}
			}
		}
	} while (addedRule); //New rules could also have epsilon NTs in them if initial product had rule where NT goes to 2 or more epsilonNTs

	//Remove Non-terminal -> Non-terminal rules
	//Algorithm: Remove every NT1 -> NT2 rule and add rules NT1 -> P where P is product of NT2 rule

	vector<pair<string, string> > tilda; //Set of <NT,NT> pairs. Each pair represents NT->NT rule.
	vector<ProductionRule> newRules;
	vector<int> indeciesOfRulesToDelete; 

		///Add to tilda
	for (int i = 0; i < rules.size(); i++) {
		if (rules[i].product.size() == 1 && isExistingNonTerminal(rules[i].product[0])) {
			pair<string, string> temp = { rules[i].nonTerminal, rules[i].product[0] };
			indeciesOfRulesToDelete.push_back(i); //Current rule is NT->NT and needs to be deleted
			bool already_exists = false;
			for (auto& te : tilda) {
				if (te == temp) {
					already_exists = true;
					break;
				}
			}
			if (!already_exists)
				tilda.push_back(temp);
		}
	}
	
	bool addedToTilda;
	do { //Iteration is now over tilda, not over rules
		vector<pair<string, string> > pairsToAdd; //needed bc cannot iterate over vector and modify it at the same time
		addedToTilda = false;
		for (auto& te : tilda) {
			for (auto& te2 : tilda) {
				if (te.second == te2.first && te.first != te2.second) { // if (A,B) (B,C) are elements from tilda add (A,C) omitting rules like (A,A)
					pair<string, string> temp = { te.first, te2.second };
					bool already_exists = false;
					for (auto& te3 : tilda) {
						if (te3 == temp) {
							already_exists = true;
							break;
						}
					}
					if (!already_exists)
						pairsToAdd.push_back(temp);
				}
			}
		}
		for (auto& pta : pairsToAdd) { //add new elements from temp arr pairsToAdd to tilda
			tilda.push_back(pta);
			addedToTilda = true; //tilda now has new NTs and iteration needs to be repeated
		}
	} while (addedToTilda);

		///Add new rules to temp vector of rules -- also needed bc cannot iterate and modify vector simultaneously
	for (const auto& tildaElement : tilda) {
		for (const auto& currentRule : rules) {
			if (currentRule.nonTerminal == tildaElement.second) {
				ProductionRule temp;
				temp.nonTerminal = tildaElement.first;
				temp.product = currentRule.product;
				if (isValidProductionRule(temp) && !RuleAlreadyExists(temp) && !(temp.product.size() == 1 && isExistingNonTerminal(temp.product[0]))) {
					//Last check is whether rule is not NT -> NT	
					newRules.push_back(temp);
				}
			}
		}
	}
		///Push back rules from temp vector to vetor of strings of grammar
	for (const auto& vectorElement : newRules) {
		if (!RuleAlreadyExists(vectorElement) && isValidProductionRule(vectorElement)) {
			rules.push_back(vectorElement);
		}

	}
		///Delete NT -> NT rules
	for (std::vector<int>::reverse_iterator rit = indeciesOfRulesToDelete.rbegin(); rit != indeciesOfRulesToDelete.rend(); rit++) {
		auto tempIt = rules.begin() + (*rit);
		rules.erase(tempIt);
	}

	//Add S->@ if neccesary where S is starting symbol
	if (epsilonFromStartNeeded) {
		ProductionRule serule = { this->startSymbol, {"@"} };
		rules.push_back(serule);
	}
}

//returns vector of pairs of strings where every string of first vector is related with every string of second vector
vector<pair <string, string> > crossJoin (const vector<string>& A,const vector<string>& B) { //stackoverflow said return by value
	vector<pair <string, string> > result;
	for (unsigned i = 0; i < A.size(); i++) {
		for (unsigned j = 0; j < B.size(); j++) {
			pair<string, string> temp = { A[i], B[j] };
			result.push_back(temp);
		}
	}
	return result;
}

bool Grammar::CYK(string word) const {
	if (!Chomsky()) {
		cerr << RED_TEXT << "Grammar is not in Chomsky Normal Form. Algorithm CYK cannot be executed.\n" <<
									"You can use 'chomskify' command to make grammar in Chomsky Normal Form." << RESET_COLORING << endl;
		return false;
	}
	//Algorithm at page 33 at : https://learn.fmi.uni-sofia.bg/pluginfile.php/193362/mod_resource/content/1/3contextfreegram.pdf

	unsigned wordLength = word.length();
	
	map <pair<int, int>, vector<string> > table;	//key: (i,j), value {set of NTs} -- represents table from algorithm
													//Empty cells simply won't exist in map.
	for (unsigned i = 0; i < wordLength; i++) { //Fill first row of table
		for (const auto& rule : rules) {
			if (rule.product.size() == 1 && (rule.product[0][0] == word[i])) { //no need to check if product is terminal since CNF
				auto tit = table.find({ i, 0} ); //if key is found in map add NT to its value IF NOT ALREADY EXISTING
				if (tit != table.end()) {
					bool already_exists = false;;
					for (auto rpit = (*tit).second.cbegin(); !already_exists && rpit != (*tit).second.cend(); ++rpit) {
						already_exists = (*rpit) == rule.nonTerminal;
					}
					if(!already_exists)
						(*tit).second.push_back(rule.nonTerminal);
				}
				else {	//if key is not found in map add new elepemt to map with given key and value vector with only one string - NT of rule
					vector<string> tempVS;
					tempVS.push_back(rule.nonTerminal);
					pair<int, int> tempPair = { i, 0 };
					table.emplace(tempPair, tempVS );
				}
			}
		}
	}

	for (unsigned j = 1; j < wordLength; j++) { //Fill rows 2+ of table
		for (unsigned i = 0; i < wordLength - j; i++) {
			for (unsigned k = 0; k < j ; k++) {
				auto tit1 = table.find({ i, k });
				auto tit2 = table.find({ i + k + 1, j - k - 1});
				if (tit1 != table.end() && tit2 != table.end() ) { // Both cells are not empty
					vector<pair<string, string> > temp = crossJoin((*tit1).second, (*tit2).second);
					for (auto vectorElement : temp) { //iterate over rules and try to find Nt that can be replaced with any of elements in crossJoined vector - check algorithm
						for (const auto rule : rules) {
							//Elements in cross joined vector consist of 2 NTs so searching for rules with products of size 2
							if (rule.product.size() == 2 && rule.product[0] == vectorElement.first && rule.product[1] == vectorElement.second) {
								auto tit = table.find({ i, j }); //if key is found in map add NT to its value IF NOT ALREADY EXISTING -- key = cell
								if (tit != table.end()) {
									bool already_exists = stringOccursInVector((*tit).second, rule.nonTerminal);
									if (!already_exists)
										(*tit).second.push_back(rule.nonTerminal);
								}
								else {	//if key is not found in map add new elepemt to map with given key and value vector with only one string - NT of rule
									vector<string> tempVS;
									tempVS.push_back(rule.nonTerminal);
									pair<int, int> tempPair = { i, j };
									table.emplace(tempPair, tempVS);
								}
							}
						}
					}
				}
			}
		}
	}

	auto tit = table.find({ 0, wordLength - 1});
	if (tit != table.end()) {
		for (auto i : (*tit).second) { //search for S -- check algorithm
			if (i == startSymbol) {
				cout << BRIGHT_GREEN_TEXT << "Word '" << word << "' is recognized by Grammar<"<< this->id << ">!" << RESET_COLORING << endl;
				return true;
			}
		}
	}
	cout << BRIGHT_RED_TEXT << "Word '" << word << "' is NOT recognized by Grammar<" << this->id << ">!" << RESET_COLORING << endl;
	return false;
}

bool Grammar::Empty() const {
	//Algorithm at page 55 at : https://learn.fmi.uni-sofia.bg/pluginfile.php/193362/mod_resource/content/1/3contextfreegram.pdf
	vector<string> marked;
	for (const char terminal : terminals) { //Add all terminals to vector representing 'marked' T/NTs
		string temp;
		temp = terminal;
		marked.push_back(temp);
		marked.push_back("@");
	}
	bool added = false;
	do {
		added = false;
		for (auto& rule : rules) {
			bool addToMarked = true;
			auto rpit = rule.product.cbegin();
			while (addToMarked && rpit != rule.product.cend()) { //if whole product is made up from elements of marked -> add NT
				addToMarked = stringOccursInVector(marked, (*rpit));
				rpit++;
			}
			if (addToMarked && !stringOccursInVector(marked, rule.nonTerminal)) {
				marked.push_back(rule.nonTerminal);
				added = true;
			}
		}
	} while (added); //repeat untill there are NTs to add to marked
	return !stringOccursInVector(marked, startSymbol);
}

void Grammar::print(std::ostream& os) {
	sortRules();

	const string indent = "   ";
	const string indentProductions = "                     ";
	os << YELLOW_TEXT << "Grammar<" << id << '>'<< RESET_COLORING << endl;
	os << indent << "Non-terminals: ";
	unsigned nonTerminalsCount = nonTerminals.size();
	unsigned terminalsCount = terminals.size();
	unsigned rulesCount = rules.size();
	for (unsigned i = 0; i < nonTerminalsCount; i++) {
		os << nonTerminals[i] ;
		if (i < nonTerminalsCount - 1)
			os << ", ";
	}
	os << '\n';
	os << indent <<"Terminals: ";
	for (unsigned i = 0; i < terminalsCount; i++) {
		os << terminals[i];
		if (i < terminalsCount - 1)
			os << ", ";
		
	}
	os << '\n';
	os << indent << "Starting Symbol: " << startSymbol << '\n';
	os << indent << "Produciton Rules: ";
	for (int i = 0; i < rules.size(); i++) {
		if(i > 0)
			os << indentProductions;
		os << i << ". "<<  rules[i].nonTerminal << "->";
		for (auto p : rules[i].product) {
			os << p;
		}
		os << '\n';
	}
	if (rules.empty())
		os << '\n';
}

//Removes unnecessary @ in products 
bool Grammar::addRule(const std::string& rule, bool printInfo) { //printInfo = false - function will show nothing on console no matter what has it done
	//Check sttring not empty
	if (rule.empty()) {
		if(printInfo)
			cerr << RED_TEXT << "Invalid rule entered!" << RESET_COLORING << endl;
		return false;
	}
	//Check if inputted correctly 
	unsigned found = rule.find("->");
	if (found == string::npos) {
		if(printInfo)
			cerr << RED_TEXT << "Invalid rule entered!" << RESET_COLORING << endl;
		return false;
	}
	else {
		ProductionRule newRule;
		string _nonTerminal;
		_nonTerminal.assign(rule, 0, found); //Everything before -> is NT
		if (!isExistingNonTerminal(_nonTerminal)) {
			if(printInfo)
				cerr << RED_TEXT << "Invalid rule entered! Non-terminal '" << _nonTerminal << "'does not exist in grammar!" << RESET_COLORING << endl;
			return false;
		}
		else { //Starts with NT and has -> afterwards
			newRule.nonTerminal = _nonTerminal;
			string _product;
			_product.assign(rule, found + 2, rule.length()); //Everything after -> is product
			unsigned _productLength = _product.length();
			if (_productLength == 0) {
				if(printInfo)
					cerr << RED_TEXT << "Invalid rule entered!" << RESET_COLORING << endl;
				return false;
			}
			for (unsigned i = 0; i < _productLength; i++) {
				if (_product[i] == '@' && _productLength > 1) { // if product contains anything else but @ ( has siza > 1 ) then @ is useless
					continue;
				}
				else if (isExistingTerminal(_product[i])) { // hover isExistingTerminal -- if a terminal simply add it as a part of product
					string productPart = "";
					productPart.push_back(_product[i]);
					newRule.product.push_back(productPart);
				}
				else if (_product[i] >= 'A' && _product[i] <= 'Z') {
					if ((i + 1 >= _productLength) || isCapitalLetter(_product[i+1]) || (isExistingTerminal(_product[i + 1]))) { //This is a 1 sized NT (capital letter) then add it
						string productPart = "";
						productPart.push_back(_product[i]);
						if (isExistingNonTerminal(productPart)) {
							newRule.product.push_back(productPart);
						}
						else {
							if(printInfo)
								cerr << RED_TEXT << "Invalid rule entered! Non-terminal '" << productPart << "' does not exist in grammar!" << RESET_COLORING << endl;
							return false;
						}
					}
					else if (_product[i + 1] == '_') {
						unsigned foundUnderscope = _product.find('_', i + 2); //if there is a _ there should be second one to indicate end
						if (foundUnderscope == string::npos) {
							if(printInfo)
								cerr << RED_TEXT << "Invalid rule entered! Non-terminal names with only one underscore are not allowed!" << RESET_COLORING << endl;
							return false;
						}
						else {
							string::const_iterator start = _product.begin() + i;
							string::const_iterator end = _product.begin() + foundUnderscope + 1;
							string productPart = "";
							productPart.assign(start, end); //eveything form capital letter to second _ is a NT with an index
							if (isExistingNonTerminal(productPart)) {
								newRule.product.push_back(productPart);
								i = foundUnderscope;
							}
							else {
								if(printInfo)
									cerr << RED_TEXT << "Invalid rule entered! Non-terminal '" << productPart << "' does not exist in grammar!" << RESET_COLORING << endl;
								return false;
							}
						}
					}
				}
				else {
					if(printInfo)
						cerr << RED_TEXT << "Invalid rule entered! Terminal '" << _product[i] << "' does not exist in grammar!" << RESET_COLORING << endl;
					return false;
				}
			}
			rules.push_back(newRule);
			if(printInfo)
				cerr << GREEN_TEXT << "Rule '" << rule << "' was added to grammar with id '" << this->id<<"'" << RESET_COLORING << endl;
			return true;
		}
	}
}

bool Grammar::removeRule(unsigned number) {
	unsigned rulesCount = rules.size();
	if (number < 0 || number >= rulesCount) {
		cerr << RED_TEXT << "Attempt to delete rule at invalid index" << RESET_COLORING << endl;
		return false;
	}
	else {
		productions::const_iterator del = rules.begin() + number;
		rules.erase(del);
		cerr << BRIGHT_RED_TEXT << "Rule at index '"<< number << "' in grammar with id '" << this->id << "' was deleted!" << RESET_COLORING << endl;
		return true;
	}
}

void Grammar::save(std::ostream& os) {
	for (unsigned i = 0; i < nonTerminals.size(); i++) {
		os << nonTerminals[i];
		if (i < nonTerminals.size() - 1)
			os << ", ";
	}
	os << '\n';
	for (unsigned i = 0; i < terminals.size(); i++) {
		os << terminals[i];
		if (i < terminals.size() - 1)
			os << ", ";
	}
	os << '\n';
	os << this->startSymbol << '\n'; 
	for (unsigned i = 0; i < rules.size(); i++) {
		os << rules[i].nonTerminal << "->";
		for (auto& rp : rules[i].product) {
			os << rp;
		}
		if (i < rules.size() - 1) {
			os << '\n';
		}
	}
}
