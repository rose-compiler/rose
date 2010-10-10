#ifndef REVERSE_COMPUTATION_EXP_HANDLER_H
#define REVERSE_COMPUTATION_EXP_HANDLER_H

#include <rose.h>
#include "eventHandler.h"

//! This handler generates a NULL reverse expression.
class NullExpressionHandler : public ExpressionReversalHandler
{
public:
    NullExpressionHandler() { name_ = "Null Expression Handler"; }
    virtual ExpressionReversal generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult);
    virtual std::vector<EvaluationResult> evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used);
};

//! This handler generates a reverse expression which is the same as the original expression.
class IdentityExpressionHandler : public ExpressionReversalHandler
{
public:
    IdentityExpressionHandler() { name_ = "Identity Expression Handler"; }
    virtual ExpressionReversal generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult);
    virtual std::vector<EvaluationResult> evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used);
};

//! This handler just store and restore a value, once the value is modified (state saving style).
class StoreAndRestoreExpressionHandler : public ExpressionReversalHandler
{
public:
    StoreAndRestoreExpressionHandler() { name_ = "Store And Restore Expression Handler"; }
    virtual ExpressionReversal generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult);
    virtual std::vector<EvaluationResult> evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used);
};

#endif    
