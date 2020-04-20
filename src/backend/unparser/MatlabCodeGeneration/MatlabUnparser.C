#include "sage3basic.h"
#include "MatlabUnparser.h"

void MatlabUnparser::backend(SgProject *project)
{
    ASSERT_not_null(project);
    
    MatlabUnparserBottomUpTraversal unparserTraversal;
    unparserTraversal.traverseInputFiles(project);
}
