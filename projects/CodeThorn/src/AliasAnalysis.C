#include "AliasAnalysis.h"

using namespace SPRAY;

bool AliasAnalysis::isMayAlias(SgExpression* e1, SgExpression* e2) {
  return true;
}

bool AliasAnalysis::isMustAlias(SgExpression* e1, SgExpression* e2) {
  return false;
}
