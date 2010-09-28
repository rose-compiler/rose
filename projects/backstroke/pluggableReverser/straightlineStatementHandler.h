#pragma once

#include "eventHandler.h"

/** The straightline statement handler runs the expression/statement handlers in the order in which they were installed
  * and picks the first valid result. This allows for very fast code generation. */
class StraightlineStatementHandler : public StatementReversalHandler
{
public:
	virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);

	virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult);


private:

	/** Process an expression statement by using the first expression handler returning a valid result. */
	std::vector<EvaluationResult> evaluateExpressionStatement(SgExprStatement* stmt, const VariableVersionTable& var_table);



	std::vector<EvaluationResult> evaluateBasicBlock(SgBasicBlock* stmt, const VariableVersionTable& var_table);
};
