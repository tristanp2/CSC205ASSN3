/* LSystem.cpp

   A parser for L-Systems.

   B. Bird - 02/08/2016
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "LSystem.h"

#define MAX_LINE 2048

using namespace std;

void LSystem::GenerateRecursive(string& buf, string input,int iterations,int maxIterations){
	for (unsigned int i = 0; i < input.length(); i++){
		if (iterations < maxIterations){
			list<Rule>::iterator rule = rules.begin();
			for( ; rule != rules.end(); rule++ ){
				if (rule->rule == input[i]){
					//Even if this rule matches, it might need to be ignored
					//either because it's dead or because one of its flags
					//causes it to be ignored
					if (	(rule->lifetime > 0 && rule->lifetime < iterations) 
						 || (rule->lifetime < 0 && (maxIterations + rule->lifetime) <= iterations)
						 || ((iterations%2 == 1) && (rule->flags & FLAG_EVEN)) //Even flag (don't expand on odd numbered iterations)
						 || ((iterations%2 == 0) && (rule->flags & FLAG_ODD)) //Odd flag (don't expand on even numbered iterations)
						 )
						 continue;
					//If the rule passes the above test, substitute recursively
					GenerateRecursive(buf,rule->substitution,iterations+1,maxIterations);
					break;
				}
			}
			if (rule != rules.end())
				continue;
		}
		buf += input[i];
	}
}

string LSystem::GenerateSystemString(int iterations){
	string buf;
	GenerateRecursive(buf,axiom,0,iterations);
	return buf;
}

void LSystem::addRule(char ruleChar, const char* substitution,int flags,int lifetime){
	Rule r(ruleChar,substitution,flags,lifetime);
	rules.push_back(r);
}

static inline void eatWhitespace(char*& str){
	while (*str == ' ')
		str++;
}
static inline void removeTrailingNewline(char* str){
	while(*str){
		if (*str == '\n'){
			*str = '\0';
			return;
		}
		str++;
	}
}
static inline void removeTrailingWhitespace(char* str){
	char* lastChar = str-1;
	while (*str){
		if (*str != '\n' && *str != ' ')
			lastChar = str;
		str++;
	}
	*(lastChar + 1) = '\0';
}

LSystem* LSystem::ParseFile(string filename){
	char lineBuf[MAX_LINE];
	char *curLine;
	char ruleChar;
	int flags,lifetime;
	int readingFlags;
	FILE* file = fopen(filename.c_str(),"r");
	if (!file)
		return NULL;
	LSystem* sys = new LSystem();
	bool found_axiom = false;
	//The first line of the file must be the axiom
	while((curLine = fgets(lineBuf,sizeof(lineBuf),file))){
		eatWhitespace(curLine);
		if (!curLine[0] || curLine[0] == '#')
			continue;
		removeTrailingWhitespace(curLine);
		sys->axiom = curLine;
		found_axiom = true;
		break;
	}
	if (!found_axiom){
		delete sys; //Fail if no axiom is found
		fclose(file);
		return NULL;
	}
	//Now read the rules
	while((curLine = fgets(lineBuf,sizeof(lineBuf),file))){
		//A rule should be in the form '<lifetime> <flags><rule char> = <rule text>\n' where <rule char> is the rule character
		//C may be preceded by modifiers that set flags (e.g. '#' sets the random flag)
		//lifetime defaults to 0 if not provided (0 meaning "forever")
		flags = 0;
		eatWhitespace(curLine);
		if (!curLine[0] || curLine[0] == '#')
			continue;
		lifetime = strtol(curLine,&curLine,10);
		eatWhitespace(curLine);
		readingFlags = 1;
		while (readingFlags){
			ruleChar = *curLine++;
			eatWhitespace(curLine);
			switch(ruleChar){
				case '%': //Even flag
					flags |= FLAG_EVEN;
					break;
				case '^': //Odd flag
					flags |= FLAG_ODD;
					break;
				default:
					readingFlags = 0;
			}
		}
		eatWhitespace(curLine);
		if (*curLine++ != '=')
			continue;
		eatWhitespace(curLine);
		removeTrailingWhitespace(curLine);
		sys->addRule(ruleChar,curLine,flags,lifetime);
	}
	return sys;
	
}
