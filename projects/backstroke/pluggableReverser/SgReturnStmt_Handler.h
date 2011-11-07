#pragma once

#include "handlerTypes.h"

//! This handler reverses a return statement that appears as the very last
//! statement of a function. The reverse statement is a no-op
class SgReturnStmt_Handler : public StatementReversalHandler
{
public:
	virtual EvaluationResult evaluate(SgStatement* statement, const VariableVersionTable& var_table);

	virtual StatementReversal generateReverseAST(SgStatement* exp, const EvaluationResult& evaluationResult);
};

