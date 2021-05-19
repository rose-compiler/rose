#ifndef PREDEFINED_SEMANTIC_FUNCTIONS_H
#define PREDEFINED_SEMANTIC_FUNCTIONS_H

#include "EStateTransferFunctions.h"

namespace PredefinedSemanticFunctions {

  list<SingleEvalResultConstInt> evalFunctionCallMemCpy(EStateTransferFunctions* estf, SgFunctionCallExp* funCall, EState estate);
  list<SingleEvalResultConstInt> evalFunctionCallStrLen(EStateTransferFunctions* estf, SgFunctionCallExp* funCall, EState estate);

} // end of namespace PredefinedSemanticFunctions

#endif
