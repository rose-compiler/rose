#pragma once

#include "handlerTypes.h"

//! This handler handles reversing the side effects of variable declarations. Declarations are inserted
//! unmodified in the forward code. For example, 'int a = i++' is reversed as 'i--;'.
//! Installing this handler is always necessary.
class SgVariableDeclaration_Handler : public StatementReversalHandler
{
public:
    SgVariableDeclaration_Handler() { name_ = "Variable Declaration Handler"; }

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult);
    virtual EvaluationResult evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};


