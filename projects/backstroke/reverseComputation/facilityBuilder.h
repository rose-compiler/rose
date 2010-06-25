#ifndef REVERSE_COMPUTATAION_FACILITYBUILDER_H 
#define REVERSE_COMPUTATAION_FACILITYBUILDER_H 

#include <string>
#include <vector>

class SgFunctionDeclaration;
class SgStatement;
class SgClassType;

#if 0
const string INT_MEM_NAME = "i_";
const string INT_ARRAY_MEM_NAME = "a_";
const int ARRAY_SIZE = 100;
#endif

SgFunctionDeclaration* buildCompareFunction(SgClassType*);
SgFunctionDeclaration* buildMainFunction(const std::vector<SgStatement*>& inits, 
        const std::vector<std::string> event_names, bool klee);
SgFunctionDeclaration* buildInitializationFunction(SgClassType*);


#endif
