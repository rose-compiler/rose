#ifndef PREDEFINED_SEMANTIC_FUNCTIONS_H
#define PREDEFINED_SEMANTIC_FUNCTIONS_H

#include "EStateTransferFunctions.h"

namespace PredefinedSemanticFunctions {

  SingleEvalResult evalFunctionCallMemCpy(EStateTransferFunctions* estf, SgFunctionCallExp* funCall, EStatePtr estate);
  SingleEvalResult evalFunctionCallStrLen(EStateTransferFunctions* estf, SgFunctionCallExp* funCall, EStatePtr estate);
} // end of namespace PredefinedSemanticFunctions

#endif
