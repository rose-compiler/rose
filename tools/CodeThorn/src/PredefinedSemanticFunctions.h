#ifndef PREDEFINED_SEMANTIC_FUNCTIONS_H
#define PREDEFINED_SEMANTIC_FUNCTIONS_H

#include "EStateTransferFunctions.h"

namespace PredefinedSemanticFunctions {

  CodeThorn::SingleEvalResult evalFunctionCallMemCpy(CodeThorn::EStateTransferFunctions* estf, SgFunctionCallExp* funCall, CodeThorn::EStatePtr estate);
  CodeThorn::SingleEvalResult evalFunctionCallStrLen(CodeThorn::EStateTransferFunctions* estf, SgFunctionCallExp* funCall, CodeThorn::EStatePtr estate);
} // end of namespace PredefinedSemanticFunctions

#endif
