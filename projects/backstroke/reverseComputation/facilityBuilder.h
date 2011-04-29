#ifndef REVERSE_COMPUTATAION_FACILITYBUILDER_H 
#define REVERSE_COMPUTATAION_FACILITYBUILDER_H 

#include <rose.h>


SgFunctionDeclaration* buildCompareFunction(SgClassType*);
SgFunctionDeclaration* buildInitializationFunction(SgClassType*);

/*!****************************************************************************
*  inits:       global varibles which should be initialized.
*  event_names: all event funtions.
******************************************************************************/
SgFunctionDeclaration* buildMainFunction(const std::vector<SgAssignOp*>& inits,
        const std::vector<std::string>& event_names, bool klee = false);

inline SgFunctionDeclaration* buildMainFunction(const std::string& event_name) 
{
    std::vector<SgAssignOp*> inits;
    std::vector<std::string> event_names(1, event_name);
    return buildMainFunction(inits, event_names, false);
}


#endif
