#include "handlerTypes.h"
#include "SgReturnStmt_Handler.h"
#include "utilities/utilities.h"
#include <rose.h>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

using namespace std;

EvaluationResult SgReturnStmt_Handler::evaluate(SgStatement* statement, const VariableVersionTable& var_table)
{
	EvaluationResult result;

	if (SgReturnStmt* returnStatement = isSgReturnStmt(statement))
	{
		//TODO: If the return statement itself contains side effects, these are discarded here
		//We should reverse the side effects of the expression after 'return'
		//ROSE_ASSERT(BackstrokeUtility::containsModifyingExpression(returnStatement->get_expression()));
          // MS:
          if(!BackstrokeUtility::containsModifyingExpression(returnStatement->get_expression())) {
            cout<<"WARNING: return statement itself contains side effects but they are not reversed."<<endl;
          }
			
		//We only allow a return statement to appear as the very last statement in a function. Assert this
		ROSE_ASSERT(SageInterface::getEnclosingFunctionDefinition(statement)->get_body()->get_statements().back() == returnStatement);
		result = EvaluationResult(this, statement, var_table);
	}

	return result;
}

StatementReversal SgReturnStmt_Handler::generateReverseAST(SgStatement* statement, const EvaluationResult& evaluationResult)
{
	ROSE_ASSERT(evaluationResult.getStatementHandler() == this && evaluationResult.getChildResults().size() == 0);
	ROSE_ASSERT(isSgReturnStmt(statement));

	//The reverse of a return statement is a no-op
	SgStatement* forward = SageInterface::copyStatement(statement);
	SgStatement* reverse = NULL;

	return StatementReversal(forward, reverse);
}
