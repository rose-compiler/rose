#ifndef _NASC_H
#define _NASC_H

#include "rose.h"
#include "sageBuilder.h"
#include "sageInterface.h"

namespace NASC
{

void transformation(SgProject* project);
void createSubdomainBound(SgVariableSymbol* var,SgStatement* stmt);
void buildCenterComputation(SgScopeStatement* scope);
void buildBoundaryComputation(SgScopeStatement* scope);
}
#endif
