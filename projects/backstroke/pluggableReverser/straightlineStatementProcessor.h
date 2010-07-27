#pragma once

#include "eventProcessor.h"

/** The straightline statement processor runs the expression/statement handlers in the order in which they were installed
  * and picks the first valid result. This allows for very fast code generation. */
class StraightlineStatementProcessor : public StatementProcessor
{
public:
	virtual std::vector<StatementObject> process(SgStatement* statement, const VariableVersionTable& variableTable);

private:

	/** Process an expression statement by using the first expression handler returning a valid result. */
	std::vector<StatementObject> processExpressionStatement(SgExprStatement* statement, const VariableVersionTable& variableTable);

	std::vector<StatementObject> processBasicBlock(SgBasicBlock* basicBlock, const VariableVersionTable& variableTable);
};
