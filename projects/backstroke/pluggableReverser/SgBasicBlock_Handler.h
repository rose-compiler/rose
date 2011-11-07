#pragma once

#include "eventProcessor.h"

/** The straightline statement handler runs the expression/statement handlers in the order in which they were installed
  * and picks the first valid result. This allows for very fast code generation. */
class SgBasicBlock_Handler : public StatementReversalHandler
{
public:
	virtual EvaluationResult evaluate(SgStatement* stmt, const VariableVersionTable& var_table);

	virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult);


private:

	/** Process an expression statement by using the first expression handler returning a valid result. */
	EvaluationResult evaluateExpressionStatement(SgExprStatement* stmt, const VariableVersionTable& var_table);


	EvaluationResult evaluateBasicBlock(SgBasicBlock* stmt, const VariableVersionTable& var_table);
};
