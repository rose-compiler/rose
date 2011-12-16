#pragma once

#include "handlerTypes.h"

class SgWhileStmt_Handler : public StatementReversalHandler
{
public:
    SgWhileStmt_Handler() { name_ = "While Statement Processor"; }

	SgStatement* assembleLoopCounter(SgStatement* loop_stmt);
	SgStatement* buildForLoop(SgStatement* loop_body);

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult&);
    virtual EvaluationResult evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};

