#pragma once

#include "handlerTypes.h"

class SgIfStmt_Handler : public StatementReversalHandler
{
public:
    SgIfStmt_Handler() { name_ = "If Statement Handler"; }

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult&);
    virtual EvaluationResult evaluate(SgStatement* stmt, const VariableVersionTable& var_table);

};

