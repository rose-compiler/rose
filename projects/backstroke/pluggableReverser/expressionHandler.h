#ifndef REVERSE_COMPUTATION_EXP_HANDLER_H
#define REVERSE_COMPUTATION_EXP_HANDLER_H

#include <rose.h>
#include "utilities/types.h"

//! This handler just store and restore a value, once the value is modified (state saving style).
ExpPairs storeAndRestore(SgExpression* exp);

//! This handler handles constructive assignment, like ++, +=, -=, etc.
ExpPairs processConstructiveExp(SgExpression* exp);

//! This handler handles more constructive assignment (for example, a = b - a).
ExpPairs processConstructiveAssignment(SgExpression* exp);

#endif    
