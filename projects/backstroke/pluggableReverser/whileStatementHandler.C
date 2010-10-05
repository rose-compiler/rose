#include "whileStatementHandler.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <utilities/Utilities.h>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;
#define foreach BOOST_FOREACH

SgStatement* WhileStatementHandler::assembleLoopCounter(SgStatement* loop_stmt)
{
	static int counter = 0;
	string counter_name = "loop_counter_" + lexical_cast<string > (counter++);
	backstroke_util::validateName(counter_name, loop_stmt);

	SgVariableDeclaration* counter_decl = buildVariableDeclaration(
			counter_name,
			buildIntType(),
			buildAssignInitializer(buildIntVal(0)));
	SgExpression* counter_var = buildVarRefExp(counter_decl->get_variables()[0]);

	SgStatement* incr_counter = buildExprStatement(
			buildPlusPlusOp(counter_var, SgUnaryOp::prefix));

	if (SgForStatement* for_stmt = isSgForStatement(loop_stmt))
	{
		SgStatement* loop_body = for_stmt->get_loop_body();
		if (SgBasicBlock* block_body = isSgBasicBlock(loop_body))
		{
			appendStatement(incr_counter, block_body);
		}
		else
		{
			setLoopBody(for_stmt, buildBasicBlock(loop_body, incr_counter));
		}
	}
	else if (SgWhileStmt* while_stmt = isSgWhileStmt(loop_stmt))
	{
		SgStatement* loop_body = while_stmt->get_body();
		if (SgBasicBlock* block_body = isSgBasicBlock(loop_body))
		{
			appendStatement(incr_counter, block_body);
		}
		else
		{
			setLoopBody(while_stmt, buildBasicBlock(loop_body, incr_counter));
		}
	}
	else if (SgDoWhileStmt* do_while_stmt = isSgDoWhileStmt(loop_stmt))
	{
		SgStatement* loop_body = do_while_stmt->get_body();
		if (SgBasicBlock* block_body = isSgBasicBlock(loop_body))
		{
			appendStatement(incr_counter, block_body);
		}
		else
		{
			setLoopBody(do_while_stmt, buildBasicBlock(loop_body, incr_counter));
		}
	}

	SgStatement* store_counter = buildExprStatement(pushVal(copyExpression(counter_var)));

	return buildBasicBlock(counter_decl, loop_stmt, store_counter);
}

SgStatement* WhileStatementHandler::buildForLoop(SgStatement* loop_body)
{
	// build a simple for loop like: for (int i = N; i > 0; --i)

	string counter_name = "i";
	backstroke_util::validateName(counter_name, loop_body);

	SgType* counter_type = buildIntType();
	SgStatement* init = buildVariableDeclaration(
			counter_name, counter_type, buildAssignInitializer(popVal(counter_type)));
	SgStatement* test = buildExprStatement(
			buildBinaryExpression<SgGreaterThanOp >(buildVarRefExp(counter_name), buildIntVal(0)));
	SgExpression* incr = buildMinusMinusOp(buildVarRefExp(counter_name), SgUnaryOp::prefix);

	SgStatement* for_stmt = buildForStatement(init, test, incr, loop_body);
	return for_stmt;
}

StatementReversal WhileStatementHandler::generateReverseAST(SgStatement* stmt, const EvaluationResult& eval_result)
{
	ROSE_ASSERT(eval_result.getChildResults().size() == 1);
    SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
    ROSE_ASSERT(while_stmt);

    StatementReversal body_result = eval_result.getChildResults()[0].generateReverseStatement();

	SgStatement* fwd_cond = copyStatement(while_stmt->get_condition());
	SgStatement* fwd_stmt = buildWhileStmt(fwd_cond, body_result.fwd_stmt);
	fwd_stmt = assembleLoopCounter(fwd_stmt);
	
	SgStatement* rvs_stmt = buildForLoop(body_result.rvs_stmt);

    return StatementReversal(fwd_stmt, rvs_stmt);
}

vector<EvaluationResult> WhileStatementHandler::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
	// Suppose the condition of this while statement does not contain modifying expressions.
	
    vector<EvaluationResult> results;
    SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
	
	// If this while statement has any break or continue inside, we cannot handle it.
    if (while_stmt == NULL || backstroke_util::hasContinueOrBreak(while_stmt))
        return results;

	SgBasicBlock* body = isSgBasicBlock(while_stmt->get_body());
	ROSE_ASSERT(body);

#if 0
	cout << "Old table:\n";
	var_table.print();
	cout << "New table:\n";
	var_table.getVarTablesForLoopBody(body).print();
#endif

	vector<EvaluationResult> loop_body_results = evaluateStatement(body, var_table.getVarTablesForLoopBody(body));
	foreach (EvaluationResult& res, loop_body_results)
	{
		EvaluationResult final_result(this, while_stmt, res.getVarTable());
		final_result.addChildEvaluationResult(res);

		SimpleCostModel cost = res.getCost();
		cost.increaseStoreCount();
		final_result.setCost(cost);

		results.push_back(final_result);
	}

	return results;
}





