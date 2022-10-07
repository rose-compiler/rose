#ifndef PHI_STATEMENT_C
#define PHI_STATEMENT_C

#include"PhiStatement.h"

#include<sstream>

using namespace std;

string PhiStatement::toString()
{
	stringstream s;
	s << "varName: " << varName << "; newNumber: " << newNumber << "; trueNumber: " << trueNumber << "; falseNumber: " << falseNumber;
	return s.str(); 
}



#endif
