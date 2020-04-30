#ifndef PREDEFINED_SEMANTIC_FUNCTIONS_H
#define PREDEFINED_SEMANTIC_FUNCTIONS_H

#include "ExprAnalyzer.h"

namespace PredefinedSemanticFunctions {

  list<SingleEvalResultConstInt> evalFunctionCallMemCpy(ExprAnalyzer* exprAnalyzer, SgFunctionCallExp* funCall, EState estate);
  list<SingleEvalResultConstInt> evalFunctionCallStrLen(ExprAnalyzer* exprAnalyzer, SgFunctionCallExp* funCall, EState estate);

} // end of namespace PredefinedSemanticFunctions

#endif
