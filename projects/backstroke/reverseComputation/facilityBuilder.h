#ifndef REVERSE_COMPUTATAION_FACILITYBUILDER_H 
#define REVERSE_COMPUTATAION_FACILITYBUILDER_H 

#include <string>
#include <vector>

using std::string;
using std::vector;

class SgFunctionDeclaration;
class SgStatement;

const string INT_MEM_NAME = "m_int";
const string INT_ARRAY_MEM_NAME = "m_int_array";
const int ARRAY_SIZE = 100;

SgFunctionDeclaration* buildCompareFunction();
SgFunctionDeclaration* buildMainFunction(const vector<SgStatement*>& inits, int events_num);
SgFunctionDeclaration* buildInitializationFunction();


#endif
