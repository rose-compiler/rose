#include "statementProcessor.h"
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include "utilities/CPPDefinesAndNamespaces.h"

using namespace SageInterface;
using namespace SageBuilder;

StatementReversal ExprStatementProcessor::process(SgStatement* stmt, const EvaluationResult& evaluationResult)
{
    SgExprStatement* exp_stmt = isSgExprStatement(stmt);
    ROSE_ASSERT(exp_stmt);
	ROSE_ASSERT(evaluationResult.getStatementProcessor() == this);
	ROSE_ASSERT(evaluationResult.getChildResults().size() == 1);
    
    ExpressionReversal exp = processExpression(exp_stmt->get_expression(), evaluationResult.getChildResults().front());

    SgStatement *fwd_stmt = NULL, *rvs_stmt = NULL;

    if (exp.fwd_exp)
        fwd_stmt = buildExprStatement(exp.fwd_exp);
    if (exp.rvs_exp)
        rvs_stmt = buildExprStatement(exp.rvs_exp);

    return StatementReversal(fwd_stmt, rvs_stmt);
}

vector<EvaluationResult> ExprStatementProcessor::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
    vector<EvaluationResult> results;
    SgExprStatement* exp_stmt = isSgExprStatement(stmt);
    if (exp_stmt == NULL)
        return results;
    
     vector<EvaluationResult> potentialExprReversals = evaluateExpression(exp_stmt->get_expression(), var_table);
	 foreach(EvaluationResult& potentialExprReversal, potentialExprReversals)
	 {
		 EvaluationResult statementResult(this, var_table);
		 statementResult.addChildEvaluationResult(potentialExprReversal);
		 results.push_back(statementResult);
	 }

    ROSE_ASSERT(!results.empty());

    return results;
}


StatementReversal BasicBlockProcessor::process(SgStatement* stmt, const EvaluationResult& evaluationResult)
{
    SgBasicBlock* body = isSgBasicBlock(stmt);
    ROSE_ASSERT(body);
	ROSE_ASSERT(evaluationResult.getChildResults().size() == body->get_statements().size());

    SgBasicBlock* fwd_body = buildBasicBlock();
    SgBasicBlock* rvs_body = buildBasicBlock();

	int childResultIndex = body->get_statements().size() - 1;
    foreach (SgStatement* stmt, body->get_statements())
    {
		const EvaluationResult& childResult = evaluationResult.getChildResults().at(childResultIndex);
		childResultIndex--;
        StatementReversal proc_stmt = processStatement(stmt, childResult);

        if (proc_stmt.fwd_stmt)
            appendStatement(proc_stmt.fwd_stmt, fwd_body);
        if (proc_stmt.rvs_stmt)
            prependStatement(proc_stmt.rvs_stmt, rvs_body);
    }

    return StatementReversal(fwd_body, rvs_body);
}


vector<EvaluationResult> BasicBlockProcessor::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
    vector<EvaluationResult> results;
    
    SgBasicBlock* body = isSgBasicBlock(stmt);
    if (body == NULL)
        return results;

    cout << body->get_statements().size() << endl;
    if (body->get_statements().empty())
    {
        results.push_back(EvaluationResult(this, var_table));
        return results;
    }

    // Use two vectors to store intermediate results.
    vector<EvaluationResult> queue[2];

    int i = 0;
    queue[i].push_back(EvaluationResult(this, var_table));

    reverse_foreach (SgStatement* stmt, body->get_statements())
    {
        foreach (EvaluationResult& existingPartialResult, queue[i])
        {
            vector<EvaluationResult> results = evaluateStatement(stmt, existingPartialResult.getVarTable());
            
            ROSE_ASSERT(!results.empty());

            foreach (EvaluationResult& res, results)
            {
                // Update the result.
                EvaluationResult new_result(existingPartialResult);
				new_result.addChildEvaluationResult(res);

                queue[1-i].push_back(new_result);
            }
        }
        
        queue[i].clear();
        // Switch the index between 0 and 1.
        i = 1 - i;
    }

#if 0
    // Remove all local variables from variable version table since we will not use them anymore. 
    // This is helpful to prune branches by comparing variable version tables. 
    foreach (EvaluationResult& result, queue[i])
    {
        foreach (SgInitializedName* var, local_vars)
            result.var_table.removeVariable(var);
    }
#endif

    return queue[i];
}
