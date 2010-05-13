#ifndef REVERSE_COMPUTATAION_FACILITYBUILDER_H 
#define REVERSE_COMPUTATAION_FACILITYBUILDER_H 

#include <string>
#include <vector>

using std::string;
using std::vector;

class SgFunctionDeclaration;
class SgStatement;
class SgClassType;

const string INT_MEM_NAME = "i_";
const string INT_ARRAY_MEM_NAME = "a_";
const int ARRAY_SIZE = 100;

SgFunctionDeclaration* buildCompareFunction(SgClassType*);
SgFunctionDeclaration* buildMainFunction(const vector<SgStatement*>& inits, int events_num);
SgFunctionDeclaration* buildInitializationFunction(SgClassType*);


#endif
