#include "AliasAnalysis.h"

bool CodeThorn::AliasAnalysis::isMayAlias(SgExpression* e1, SgExpression* e2) {
  return true;
}

bool CodeThorn::AliasAnalysis::isMustAlias(SgExpression* e1, SgExpression* e2) {
  return false;
}
