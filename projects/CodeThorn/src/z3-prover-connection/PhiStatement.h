#ifndef PHI_STATEMENT_H
#define PHI_STATEMENT_H

#include<string>

using namespace std;

class PhiStatement
{
	public:
	string varName;
	int newNumber, trueNumber, falseNumber;

	PhiStatement(string varName, int newNumber, int trueNumber, int falseNumber): varName(varName), newNumber(newNumber), trueNumber(trueNumber), falseNumber(falseNumber){}

	string toString();

};

#endif
