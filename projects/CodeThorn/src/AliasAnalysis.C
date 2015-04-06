#include "AliasAnalysis.h"

bool SPRAY::AliasAnalysis::isMayAlias(SgExpression* e1, SgExpression* e2) {
  return true;
}

bool SPRAY::AliasAnalysis::isMustAlias(SgExpression* e1, SgExpression* e2) {
  return false;
}
