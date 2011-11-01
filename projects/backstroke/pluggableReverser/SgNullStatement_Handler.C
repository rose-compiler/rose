#include "SgNullStatement_Handler.h"
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include "utilities/cppDefinesAndNamespaces.h"

using namespace std;

StatementReversal SgNullStatement_Handler::generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult)
{
	ROSE_ASSERT(evaluationResult.getStatementHandler() == this && evaluationResult.getChildResults().empty());
	return StatementReversal(NULL, NULL);
}

vector<EvaluationResult> SgNullStatement_Handler::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
	vector<EvaluationResult> results;
	if (isSgNullStatement(stmt))
	{
		results.push_back(EvaluationResult(this, stmt, var_table));
	}
	return results;
}