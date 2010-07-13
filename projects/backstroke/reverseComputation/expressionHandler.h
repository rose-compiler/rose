#ifndef REVERSE_COMPUTATION_EXP_HANDLER_H
#define REVERSE_COMPUTATION_EXP_HANDLER_H

#include <rose.h>
#include "types.h"

ExpPair storeAndRestore(SgExpression* exp);
ExpPair processConstructiveExp(SgExpression* exp);
ExpPair processConstructiveAssignment(SgExpression* exp);

#endif    
