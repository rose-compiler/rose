#ifndef REVERSE_COMPUTATION_EXP_PROCESSOR_H
#define REVERSE_COMPUTATION_EXP_PROCESSOR_H

#include <rose.h>
#include "eventProcessor.h"


/*
 * If a variable is modified using destruction operation, it can be 
 * recovered by restoring or retrieving its value from others. That is 
 * store and restore and akgul style expression processor in our current
 * implementation. I think those two functions should be reused by all 
 * destructive operations: assignment, function call, a local variable 
 * leaving its scope, etc.
 */

//! This handler generates a NULL reverse expression.
class NullExpressionProcessor : public ExpressionProcessor
{
public:
    virtual ExpressionReversalVec process(SgExpression* exp, const VariableVersionTable& var_table, bool isReverseValueUsed);
};

//! This handler just store and restore a value, once the value is modified (state saving style).
class StoreAndRestoreExpressionProcessor : public ExpressionProcessor
{
public:
    virtual ExpressionReversalVec process(SgExpression* exp, const VariableVersionTable& var_table, bool isReverseValueUsed);
};

//! This handler handles constructive assignment, like ++, +=, -=, etc.
class ConstructiveExpressionProcessor : public ExpressionProcessor
{
public:
    virtual ExpressionReversalVec process(SgExpression* exp, const VariableVersionTable& var_table, bool isReverseValueUsed);
};

//! This handler handles more constructive assignment (for example, a = b - a).
class ConstructiveAssignmentProcessor : public ExpressionProcessor
{
public:
    virtual ExpressionReversalVec process(SgExpression* exp, const VariableVersionTable& var_table, bool isReverseValueUsed);
};

#endif    
