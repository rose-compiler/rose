#include "whileStatementProcessor.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
#define foreach BOOST_FOREACH

StatementReversal WhileStatementProcessor::generateReverseAST(SgStatement* stmt, const EvaluationResult&)
{
	
}

vector<EvaluationResult> WhileStatementProcessor::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
	// Suppose the condition of this while statement does not contain modifying expressions.
	
    vector<EvaluationResult> results;
    SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
    if (while_stmt == NULL)
        return results;

	vector<EvaluationResult> loop_body_results = evaluateStatement(while_stmt->get_body(), var_table);
	foreach (EvaluationResult& res, loop_body_results)
	{
		EvaluationResult final_result(this, stmt, res.getVarTable());
		final_result.addChildEvaluationResult(res);

		SimpleCostModel cost = res.getCost();
		cost.increaseStoreCount();
		final_result.setCost(cost);

		results.push_back(final_result);
	}

	return results;
}





