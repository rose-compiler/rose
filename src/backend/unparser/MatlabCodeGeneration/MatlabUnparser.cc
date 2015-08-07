#include "MatlabUnparser.h"

void MatlabUnparser::backend(SgProject *project)
{
    ROSE_ASSERT(project != NULL);
    
    MatlabUnparserBottomUpTraversal unparserTraversal;
    unparserTraversal.traverseInputFiles(project);
}
