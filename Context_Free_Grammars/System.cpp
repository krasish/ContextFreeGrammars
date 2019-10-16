#include "System.h"

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

bool System::isExisitngIndex(unsigned index) const {
	return index >= 0 && index < numberOfGrammars;
}

//Returns id of new grammar
unsigned System::addGrammar(const Grammar& gram) {
	if (numberOfGrammars + 1 >= capacity)
		resize();
	grammars[numberOfGrammars] = gram;
	grammars[numberOfGrammars].set_id(numberOfGrammars);
	cout << GREEN_TEXT << "Grammar with id '" << grammars[numberOfGrammars].get_id() << "' was added!" << RESET_COLORING << endl;
	numberOfGrammars++;
	return grammars[numberOfGrammars - 1].get_id();
}

void System::resize() {
	Grammar* newGrammars = new Grammar[capacity * RESIZE_FACTOR];
	capacity *= 2;
	for (int i = 0; i < numberOfGrammars; i++) {
		newGrammars[i] = grammars[i];
	}
	delete[] grammars;
	grammars = newGrammars;
}

void System::open(const string& fileName) {
	std::ifstream ifs(fileName);

	if (ifs.good()) {
		cout << GREEN_TEXT << "File located at '" << fileName << "' was opened!" << RESET_COLORING << endl;
		//objects needed for grammar constructor
		vector<string> nonTerminals;
		vector<char> terminals;
		string startingSymbol;
		vector<ProductionRule> rules;

		string temp; 

		//Read NTs
		getline(ifs, temp);
		string tempNT = "";
		for (const auto& ch : temp) {
			if (ch == ',') {
				nonTerminals.push_back(tempNT);
				tempNT.clear();
			}
			else if (ch == ' '){
				continue;
			}
			else{
			tempNT.push_back(ch);
			}
		}
		nonTerminals.push_back(tempNT); // Add last NT

		//Read Ts
		getline(ifs, temp);
		char tempT = '\0';
		for (const auto& ch : temp) {
			if (ch == ',') {
				terminals.push_back(tempT);
			}
			else if (ch == ' ') {
				continue;
			}
			else {
				tempT = ch;
			}
		}
		terminals.push_back(tempT); // Add last T

		//Read Starting symbol
		getline(ifs, startingSymbol);
		
		//Create Grammar
		Grammar* g = new Grammar(nonTerminals, terminals, rules, startingSymbol, numberOfGrammars);
		while (getline(ifs, temp)) {
			if (!temp.empty()) {
				if (!g->addRule(temp, false)) { //addRule return false if not succesful
					cout << BRIGHT_BLUE_TEXT << "Line '" << temp << "' is invalid rule and was ignored!" << RESET_COLORING << endl;
				}

			}

		}
		addGrammar(*g);
	}
	else {
		cerr << RED_TEXT << "File could not be opened! Check if path is correct!" << RESET_COLORING << endl;
	}
	ifs.close();
}

void System::save(unsigned id, const string& fileName) {
	if (std::ifstream(fileName).good()) {
		std::ofstream ofs(fileName);
		grammars[id].save(ofs);
		ofs.close();
		cout << GREEN_TEXT << "Grammar with id '"<< id << "' was saved in '" << fileName << "'!" << RESET_COLORING << endl;
	}
	else {
		cerr << RED_TEXT << "File could not be opened! Check if path is correct!" << RESET_COLORING << endl;
		return;
	}

}

System::System(): capacity(INITIAL_CAPACITY), numberOfGrammars(0), grammars(nullptr) {
	grammars = new Grammar[capacity];
}

void System::opreate() {

	const string openRegExpr = R"(^open \"((?:[^\"\s]|[ ])+)|(\"([^\"]+)\")\"$)";
	const string saveRegExpr = R"(^save ([1-9]\d*|0) \"((?:[^\"\s]|[ ])+)|(\"([^\"]+)\")\"$)";
	const string listRegExpr = R"(^list$)";
	const string quitRegExpr = R"(^quit$)";
	const string printRegExpr = R"(^print ([1-9]\d*|0)$)";
	const string addRuleRegExpr = R"(^add rule ([1-9]\d*|0) \"([\w\->]+)\"$)";
	const string removeRuleRegExpr = R"(^remove rule ([1-9]\d*|0) ([1-9]\d*|0)$)";
	const string unionRegExpr = R"(^union ([1-9]\d*|0) ([1-9]\d*|0)$)";
	const string concatRegExpr = R"(^concat ([1-9]\d*|0) ([1-9]\d*|0)$)";
	const string iterRegExpr = R"(^iter ([1-9]\d*|0)$)";
	const string chomskyRegExpr = R"(^chomsky ([1-9]\d*|0)$)";
	const string chomskifyRegExpr = R"(^chomskify ([1-9]\d*|0)$)";
	const string emptyRegExpr = R"(^empty ([1-9]\d*|0)$)";
	const string CYKRegExpr = R"(^CYK ([1-9]\d*|0) \"([a-z\d]+)\"$)";
	const string commandsRegExpr = R"(^commands$)";
	const string copyRegExpr = R"(^copy ([1-9]\d*|0)$)";

	const regex openPattern = regex(openRegExpr);
	const regex savePattern = regex(saveRegExpr);
	const regex listPattern = regex(listRegExpr);
	const regex quitPattern = regex(quitRegExpr);
	const regex printPattern = regex(printRegExpr);
	const regex addRulePattern = regex(addRuleRegExpr);
	const regex removeRulePattern = regex(removeRuleRegExpr);
	const regex unionPattern = regex(unionRegExpr);
	const regex concatPattern = regex(concatRegExpr);
	const regex iterPattern = regex(iterRegExpr);
	const regex chomskyPattern = regex(chomskyRegExpr);
	const regex chomskifyPattern = regex(chomskifyRegExpr);
	const regex emptyPattern = regex(emptyRegExpr);
	const regex CYKPattern = regex(CYKRegExpr);
	const regex commandsPattern = regex(commandsRegExpr);
	const regex copyPattern = regex(copyRegExpr);

	sregex_iterator iter;
	const sregex_iterator end; //end-of-sequence iterator used to determine whether matches were found
	string command;
	while (true) {

		cout << '>';
		getline(cin, command);
		if ((iter = sregex_iterator(command.begin(), command.end(), quitPattern)) != end) { //quit
			break;
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), openPattern)) != end) { // open
			string fileName = (*iter)[1];
			open(fileName);
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), savePattern)) != end) { //save
			unsigned id = std::stoi((*iter)[1]);
			if (!isExisitngIndex(id)) {
				cerr << RED_TEXT << "Grammar with id "<< id << " does not exist!" << RESET_COLORING << endl;
			}
			else {
				string fileName = (*iter)[2];
				save(id, fileName);
			}
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), listPattern)) != end) { // list
			if (numberOfGrammars == 0) {
				cerr << RED_TEXT << "No grammar exists!" << RESET_COLORING << endl;
			}
			else {
				cout << CYAN_TEXT << "List of ids:" << RESET_COLORING << endl;
				for (int i = 0; i < numberOfGrammars; i++) {
					cout << CYAN_TEXT << grammars[i].get_id() << RESET_COLORING << endl;
				}
			}
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), printPattern)) != end) { // print
			unsigned id = std::stoi((*iter)[1]);
			if (!isExisitngIndex(id)) {
				cerr << RED_TEXT << "Grammar with id " << id << " does not exist!" << RESET_COLORING << endl;
			}
			else {
				grammars[id].print();
			}
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), addRulePattern)) != end) {  // add rule
			unsigned id = std::stoi((*iter)[1]);
			if (!isExisitngIndex(id)) {
				cerr << RED_TEXT << "Grammar with id " << id << " does not exist!" << RESET_COLORING << endl;
			}
			else {
				string rule = (*iter)[2];
				grammars[id].addRule(rule);
			}
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), removeRulePattern)) != end) {  // remove rule
			unsigned id = std::stoi((*iter)[1]);
			if (!isExisitngIndex(id)) {
				cerr << RED_TEXT << "Grammar with id " << id << " does not exist!" << RESET_COLORING << endl;
			}
			else {
				unsigned ruleId = std::stoi((*iter)[2]);
				grammars[id].removeRule(ruleId);
			}	
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), unionPattern)) != end) {  // union
			unsigned id1 = std::stoi((*iter)[1]);
			unsigned id2 = std::stoi((*iter)[2]);
			if (!isExisitngIndex(id1)) {
				cerr << RED_TEXT << "Grammar with id " << id1 << " does not exist!" << RESET_COLORING << endl;
			}
			else if (!isExisitngIndex(id2)) {
				cerr << RED_TEXT << "Grammar with id " << id2 << " does not exist!" << RESET_COLORING << endl;
			}
			else {
				unsigned newId = addGrammar(grammars[id1].Union(grammars[id2]));
				cout << GREEN_TEXT << "Union of grammars '"<< id1 << "' & '"<< id2 <<"' was created and saved with id '"<< newId <<"'!" << RESET_COLORING << endl;
			}
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), concatPattern)) != end) {  // concat
			unsigned id1 = std::stoi((*iter)[1]);
			unsigned id2 = std::stoi((*iter)[2]);
			if (!isExisitngIndex(id1)) {
				cerr << RED_TEXT << "Grammar with id " << id1 << " does not exist!" << RESET_COLORING << endl;
			}
			else if (!isExisitngIndex(id2)) {
				cerr << RED_TEXT << "Grammar with id " << id2 << " does not exist!" << RESET_COLORING << endl;
			}
			else {
				unsigned newId = addGrammar(grammars[id1].Concat(grammars[id2]));
				cout << GREEN_TEXT << "Concatenation of grammars '" << id1 << "' & '" << id2 << "' was created and saved with id '" << newId << "'!" << RESET_COLORING << endl;
			}
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), iterPattern)) != end) {  // iter
			unsigned id = std::stoi((*iter)[1]);
			if (!isExisitngIndex(id)) {
				cerr << RED_TEXT << "Grammar with id " << id << " does not exist!" << RESET_COLORING << endl;
			}
			else {
				unsigned newId = addGrammar(grammars[id].Iter());
				cout << GREEN_TEXT << "Iteration of grammar '" << id <<"' was created and saved with id '" << newId << "'!" << RESET_COLORING << endl;
			}
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), chomskifyPattern)) != end) {  // chomskify
			unsigned id = std::stoi((*iter)[1]);
			if (!isExisitngIndex(id)) {
				cerr << RED_TEXT << "Grammar with id " << id << " does not exist!" << RESET_COLORING << endl;
			}
			else {
				grammars[id].Chomskify();
				cout << GREEN_TEXT << "Grammar with '" << id << "' was converted to Chomsky normal form!" << RESET_COLORING << endl;
			}
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), chomskyPattern)) != end) {  // chomsky
			unsigned id = std::stoi((*iter)[1]);
			if (!isExisitngIndex(id)) {
				cerr << RED_TEXT << "Grammar with id " << id << " does not exist!" << RESET_COLORING << endl;
			}
			else {
				if (grammars[id].Chomsky())
					cout << BRIGHT_GREEN_TEXT << "Grammar with '" << id << "' is in Chomsky normal form!" << RESET_COLORING << endl;
				else
					cout << BRIGHT_RED_TEXT << "Grammar with '" << id << "' is NOT in Chomsky normal form!" << RESET_COLORING << endl;
			}
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), emptyPattern)) != end) {  // empty
		unsigned id = std::stoi((*iter)[1]);
		if (!isExisitngIndex(id)) {
			cerr << RED_TEXT << "Grammar with id " << id << " does not exist!" << RESET_COLORING << endl;
		}
		else {
			if (grammars[id].Empty())
				cout << BRIGHT_GREEN_TEXT << "Language of grammar with '" << id << "' is empty!" << RESET_COLORING << endl;
			else
				cout << BRIGHT_RED_TEXT << "Language of grammar with '" << id << "' is NOT empty!" << RESET_COLORING << endl;
		}
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), CYKPattern)) != end) {  // CYK
		unsigned id = std::stoi((*iter)[1]);
			if (!isExisitngIndex(id)) {
				cerr << RED_TEXT << "Grammar with id " << id << " does not exist!" << RESET_COLORING << endl;
			}
			else {
				string word = (*iter)[2];
				grammars[id].CYK(word);
			}
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), copyPattern)) != end) {  // copy
			unsigned id = std::stoi((*iter)[1]);
			if (!isExisitngIndex(id)) {
				cerr << RED_TEXT << "Grammar with id " << id << " does not exist!" << RESET_COLORING << endl;
			}
			else {
				Grammar temp = grammars[id];
				unsigned nId = addGrammar(temp);
				cout << BRIGHT_CYAN_TEXT << "A copy of grammar with id '" << id << "' was created! New grammar has id '" << nId << "'." << RESET_COLORING << endl;
			}
		}
		else if ((iter = sregex_iterator(command.begin(), command.end(), commandsPattern)) != end) { //commands
			cout << BRIGHT_WHITE_TEXT << "List of available commands: " << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "1. open \"destination\" "<< BRIGHT_BLACK_TEXT <<
				"- Opens a file located at \"destination\" and \n\tcreates new grammar in system with read parameters." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "2. save <id> \"destination\" " << BRIGHT_BLACK_TEXT <<
				"- Saves grammar with <id> in flie located at \"destination\" ONLY if such file exists." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "3. list " << BRIGHT_BLACK_TEXT <<
				"- Shows list of identifiers of existing grammars." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "4. print <id> " << BRIGHT_BLACK_TEXT <<
				"- Prints grammar with identifier <id> if such exists." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "5. add rule <id> \"rule\" " << BRIGHT_BLACK_TEXT <<
				"- Adds rule to grammar with identifier <id>. Rules should look like \"A->aA\"" << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "6. remove rule <id> <n> " << BRIGHT_BLACK_TEXT <<
				"- Removes rule number <n> of grammar with identifier <id>." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "7. union <id1> <id2> " << BRIGHT_BLACK_TEXT <<
				"- Creates new grammar which is union of grammars with identifiers <id1> & <id2>." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "8. union <id1> <id2> " << BRIGHT_BLACK_TEXT <<
				"- Creates new grammar which is concatenation of grammars with identifiers <id1> & <id2>." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "9. iter <id> " << BRIGHT_BLACK_TEXT <<
				"- Creates new grammar which is iteration of grammar with identifier <id>." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "10. chomsky <id> " << BRIGHT_BLACK_TEXT <<
				"- Returns whether grammar with identifier <id> is in Chomsky normal form or not." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "11. chomskify <id> " << BRIGHT_BLACK_TEXT <<
				"- Transforms grammar with identifier <id> in Chomsky normal form." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "12. empty <id> " << BRIGHT_BLACK_TEXT <<
				"- Returns whether the language of grammar with identifier <id> is empty or not." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "13. CYK <id> \"alpha\" " << BRIGHT_BLACK_TEXT <<
				"- Performs CYK algorithm over grammar with identifier <id> with the word \"alpha\"." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "14. copy <id> " << BRIGHT_BLACK_TEXT <<
				"- Creates a new grammar - copy of grammar with identifier <id>." << RESET_COLORING << endl;
			cout << BRIGHT_WHITE_TEXT << "\t" << "15. quit " << BRIGHT_BLACK_TEXT <<
				"- Closes the program." << RESET_COLORING << endl;
			cout << BRIGHT_YELLOW_TEXT << "Commands are case and space sensitive. A single difference from layout will result in unrecognized command." << endl
				<< "Tip: Do not start commands with capital letter or put space after last expected character!" << RESET_COLORING << endl;
		}
		else {
			cerr << RED_TEXT << "Invalid command! Type 'commands' for list of available commands." << RESET_COLORING << endl;
		}
	}
}

System::~System() {
	delete[] grammars;
}
