#pragma once
#include <string>
#include <vector>

using std::string;

struct ProductionRule {
	//constructor
	string nonTerminal;
	std::vector<string> product;

};