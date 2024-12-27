#include "sage3basic.h"
#include "fixupUseAndUsingDeclarations.h"

void
fixupFortranUseDeclarations(SgNode* /*node*/) {
    TimingPerformance timer ("Fixup Fortran Use Declarations:");
    FixupFortranUseDeclarations astFixupTraversal;
}

void
FixupFortranUseDeclarations::visit(SgNode* /*node*/) {
}



