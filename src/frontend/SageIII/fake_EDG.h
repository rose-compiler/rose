///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stubs for EDG functions when neither the code nor the library is available.                             __THIS_HEADER_IS_USED__
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RSIM_MultiWithConversion_H
#define RSIM_MultiWithConversion_H              /* used by MultiWithConversion.C */

#if 0 // used to fool scripts/policies/UnusedHeaders
#include "fake_EDG.h"
#endif

int edg_main(int,char * * const,class SgSourceFile &) { return 0; }
SgScopeStatement* curr_sg_scope;

std::set<SgVariableDeclaration*> nodesAddedWithinFieldUseSet;

void FixupTypeReferencesOnMemoryPool::visit ( SgNode* node ) {}

#endif
