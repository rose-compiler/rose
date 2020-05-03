#ifndef PHI_ATTRIBUTE_C
#define PHI_ATTRIBUTE_C

#include "PhiAttribute.h"

#include "PhiStatement.h"
#include <sstream>

using namespace std;

PhiAttribute::PhiAttribute(map<string, int> reachingNumbers, Condition* condition): condition(condition), reachingNumbers(reachingNumbers) {
}

std::string PhiAttribute::toString()
{
	stringstream s;
	s << "Condition: " << condition->toString() << "\n";
	s << "Number of phi statements: " << phiStatements.size() << "\n";
	vector<PhiStatement*>::iterator i = phiStatements.begin();
	while(i != phiStatements.end())
	{
		PhiStatement* phi = *i;
		s << distance(phiStatements.begin(), i) << ": " << phi->toString() << "\n";
		i++;
	}
	
	map<string, int>::iterator j = reachingNumbers.begin();
	s << "Number of reaching numbers: " << reachingNumbers.size() << "\n";
	while(j != reachingNumbers.end())
	{
		s << distance(reachingNumbers.begin(), j) <<": (" << j->first << ", " << j->second << ")" << "\n";	
		j++;
	}
	s << endl;

	return s.str();
}

PhiStatement* PhiAttribute::getPhiFor(string varName)
{
	vector<PhiStatement*>::iterator i = phiStatements.begin();
	while(i != phiStatements.end())
	{
		PhiStatement* phi = *i;
		if(phi->varName == varName) return phi;
		i++;
	}
	return NULL;
}

PhiStatement* PhiAttribute::generatePhiFor(string varName)
{
	int oldNumber = -1;
	map<string, int>::iterator i = reachingNumbers.find(varName);
	if(i != reachingNumbers.end()) oldNumber = i->second;
	PhiStatement* phi = new PhiStatement(varName, -1, oldNumber, oldNumber);
	phiStatements.push_back(phi); 
	return phi;	
}

int PhiAttribute::getReachingNumberFor(string varName)
{
	map<string, int>::iterator i = reachingNumbers.find(varName);
	if(i != reachingNumbers.end()) return i->second;
	else return -1;
}


#endif



















