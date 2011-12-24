#pragma once

#include <rose.h>
#include "handlerTypes.h"

//! Handles SgNullStatement
class SgNullStatement_Handler : public StatementReversalHandler
{
public:
	SgNullStatement_Handler()
	{
		name_ = "SgNullStatement handler";
	}

	virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult);
	virtual EvaluationResult evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};
