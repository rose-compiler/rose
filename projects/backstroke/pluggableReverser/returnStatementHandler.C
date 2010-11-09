#include "handlerTypes.h"
#include "utilities/cppDefinesAndNamespaces.h"
#include "returnStatementHandler.h"
#include <rose.h>

using namespace std;

vector<EvaluationResult> ReturnStatementHandler::evaluate(SgStatement* statement, const VariableVersionTable& var_table)
{
	//RECURSIVE_TODO: If the return statement itself contains side effects, these are discarded here
	//We should reverse the side effects of the expression after 'return'
	vector<EvaluationResult> results;

	if (SgReturnStmt* returnStatement = isSgReturnStmt(statement))
	{
		//We only allow a return statement to appear as the very last statement in a function. Assert this
		ROSE_ASSERT(SageInterface::getEnclosingFunctionDefinition(statement)->get_body()->get_statements().back() == returnStatement);
		EvaluationResult evaluation(this, statement, var_table);
		results.push_back(evaluation);
	}

	return results;
}

StatementReversal ReturnStatementHandler::generateReverseAST(SgStatement* statement, const EvaluationResult& evaluationResult)
{
	ROSE_ASSERT(evaluationResult.getStatementHandler() == this && evaluationResult.getChildResults().size() == 0);
	ROSE_ASSERT(isSgReturnStmt(statement));

	//The reverse of a return statement is a no-op
	SgStatement* forward = SageInterface::copyStatement(statement);
	SgStatement* reverse = NULL;

	return StatementReversal(forward, reverse);
}