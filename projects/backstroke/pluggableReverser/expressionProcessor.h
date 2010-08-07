#ifndef REVERSE_COMPUTATION_EXP_PROCESSOR_H
#define REVERSE_COMPUTATION_EXP_PROCESSOR_H

#include <rose.h>
#include "eventProcessor.h"

//! This handler generates a NULL reverse expression.
class NullExpressionProcessor : public ExpressionProcessor
{
public:
    virtual InstrumentedExpressionVec process(SgExpression* exp, const VariableVersionTable& var_table);
};

//! This handler just store and restore a value, once the value is modified (state saving style).
class StoreAndRestoreExpressionProcessor : public ExpressionProcessor
{
public:
    virtual InstrumentedExpressionVec process(SgExpression* exp, const VariableVersionTable& var_table);
};

//! This handler handles constructive assignment, like ++, +=, -=, etc.
class ConstructiveExpressionProcessor : public ExpressionProcessor
{
public:
    virtual InstrumentedExpressionVec process(SgExpression* exp, const VariableVersionTable& var_table);
};

//! This handler handles more constructive assignment (for example, a = b - a).
class ConstructiveAssignmentProcessor : public ExpressionProcessor
{
public:
    virtual InstrumentedExpressionVec process(SgExpression* exp, const VariableVersionTable& var_table);
};

#endif    
