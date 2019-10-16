#pragma once

#include "Identified.h"
#include "ProducitonRule.h"
#include <vector>
#include <cassert>
#include <iostream>
#include <utility>
#include <windows.h>
#include <map>
#include <tuple>


using std::string;
using std::vector;
using std::tuple;
using std::pair;
using std::map;
using std::endl;
using std::cout;
using std::cin;
using std::cerr;
using terminalSet = std::vector<char>;
using nonTerminalSet = std::vector<string>;
using productions = std::vector<ProductionRule>;

class Grammar : public Identidied {
private:
	nonTerminalSet nonTerminals;
	terminalSet terminals;
	productions rules;
	string startSymbol;

	bool isTerminal(const char c) const;
	bool isNonTerminal(const string& s) const;

	bool isTerminalSet(const terminalSet& a) const;
	bool isNonTerminalSet(const nonTerminalSet& a) const;

	bool isExistingTerminal(const char& t) const;
	bool isExistingNonTerminal(const string& nt) const;

	bool isValidProductionRule(const ProductionRule& rule) const;
	unsigned rulesOfNonTerminalCount(const string& s) const;
	
	string createIndexedNonTerminal(char letter, unsigned index) const;
	string generateNonTerminalName(char prefLetter = '\0') const;

	bool addNonTerminal(string nt);
	bool RuleAlreadyExists(const ProductionRule& pr) const;
	
	void sortRules();
public:
	Grammar();
	Grammar(nonTerminalSet nonTerminals, terminalSet Terminals, productions rules, string startSymbol, unsigned id);
	Grammar(const Grammar& other);
	Grammar& operator = (const Grammar& other);

	Grammar& Union(const Grammar& other) const;
	Grammar& Concat(const Grammar& other) const;
	Grammar& Iter() const;
	bool Chomsky() const;
	void Chomskify();
	bool CYK(string word) const;
	bool Empty() const;

	void print(std::ostream& os = cout);
	bool addRule(const std::string& rule, bool printInfo = true);
	bool removeRule(unsigned number);
	void save(std::ostream& os);
};

inline bool isSmallLetter(char c) {
	return (c >= 'a' && c <= 'z');
}

inline bool isCapitalLetter(char c) {
	return (c >= 'A' && c <= 'Z');
}

inline bool isDigit(char c) {
	return (c >= '0' && c <= '9');
}

