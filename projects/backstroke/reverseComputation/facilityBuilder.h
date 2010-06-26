#ifndef REVERSE_COMPUTATAION_FACILITYBUILDER_H 
#define REVERSE_COMPUTATAION_FACILITYBUILDER_H 

#include <rose.h>
#include <string>
#include <vector>


#if 0
const string INT_MEM_NAME = "i_";
const string INT_ARRAY_MEM_NAME = "a_";
const int ARRAY_SIZE = 100;
#endif

SgFunctionDeclaration* buildCompareFunction(SgClassType*);
SgFunctionDeclaration* buildInitializationFunction(SgClassType*);

//*****************************************************************************
// inits:       global varibles which should be initialized.
// event_names: all event funtions.
//*****************************************************************************
SgFunctionDeclaration* buildMainFunction(const std::vector<SgStatement*>& inits, 
        const std::vector<std::string>& event_names, bool klee = false);

inline SgFunctionDeclaration* buildMainFunction(const std::string& event_name) 
{
    std::vector<SgStatement*> inits;
    std::vector<std::string> event_names(1, event_name);
    return buildMainFunction(inits, event_names, false);
}


#endif
