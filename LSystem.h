/* LSystem.h

   Definitions for an L System parser.

   B. Bird - 01/17/2016
*/
#ifndef LSYSTEM_H
#define LSYSTEM_H
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <list>

using namespace std;

class LSystem{
public:
	~LSystem(){ }
	
	//Generate a string from the current system with the given number of iterations
	string GenerateSystemString(int iterations);
	
	//Generate an LSystem object by parsing the given file
	//(LSystem object must be freed by the caller)
	static LSystem* ParseFile(string filename);

	enum RuleFlags{
		FLAG_EVEN = 1, //Only expand on even numbered iterations ('%' character)
		FLAG_ODD = 2, //Only expand on odd numbered iterations ('^' character)
	};
	
private:

	LSystem(){ }
	string axiom;
	struct Rule{
		char rule;
		string substitution;
		int flags;
		int lifetime;
		Rule(char rule, string substitution, int flags=0,int lifetime=0):
			rule(rule),substitution(substitution),flags(flags),lifetime(lifetime){ }
	};
	list<Rule> rules;
	void GenerateRecursive(string& buf, string input,int iterations, int maxIterations);

	void addRule(char ruleChar, const char* substitution,int flags = 0,int lifetime = 0);
	
};

#endif




