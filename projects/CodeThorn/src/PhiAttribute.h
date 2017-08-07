#ifndef PHI_ATTRIBUTE_H
#define PHI_ATTRIBUTE_H

#include<AstAttributeMechanism.h>
#include<vector>
#include<PhiStatement.h>
#include<map>
#include<set>
#include"SSAGenerator.h"

using namespace std;

class PhiAttribute: public AstAttribute
{
	public:
	vector<PhiStatement*> phiStatements;	
	Condition* condition;
	map<string, int> reachingNumbers;
	set<string> varsDeclaredInTrueBranch;
	set<string> varsDeclaredInFalseBranch;
	PhiAttribute(map<string, int> reachingNumbers, Condition* condition): reachingNumbers(reachingNumbers), condition(condition){}
	virtual string toString();
	PhiStatement* getPhiFor(string varName);
	PhiStatement* generatePhiFor(string varName);
	int getReachingNumberFor(string varName);

};


#endif
