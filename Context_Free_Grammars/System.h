#pragma once
#include <fstream>
#include <iostream>
#include <regex>
#include "Grammar.h"

using std::endl;
using std::cout;
using std::cin;
using std::cerr;
using std::regex;
using std::sregex_iterator;

const unsigned INITIAL_CAPACITY = 32;
const unsigned RESIZE_FACTOR = 2;

class System {
private:
	Grammar* grammars;
	unsigned capacity;
	unsigned numberOfGrammars;

	bool isExisitngIndex(unsigned inedx) const;
	void resize();
	void open(const string& fileName);
	void save(unsigned id, const string& fileName);
	unsigned addGrammar(const Grammar& gram);
public:
	System();
	
	void opreate();

	~System();
};