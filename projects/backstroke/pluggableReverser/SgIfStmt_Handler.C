#include "SgIfStmt_Handler.h"
#include "pluggableReverser/variableVersionTable.h"
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

using namespace std;
using namespace boost;
using namespace SageInterface;
using namespace SageBuilder;

StatementReversal SgIfStmt_Handler::generateReverseAST(SgStatement* stmt, const EvaluationResult& evalResult)
{
	ROSE_ASSERT(evalResult.getChildResults().size() == 3);
	SgIfStmt* if_stmt = isSgIfStmt(stmt);
	ROSE_ASSERT(if_stmt);

	// Get three child handlers.
	StatementReversal proc_cond = evalResult.getChildResults()[2].generateReverseStatement();
	StatementReversal proc_true_body = evalResult.getChildResults()[1].generateReverseStatement();
	StatementReversal proc_false_body = evalResult.getChildResults()[0].generateReverseStatement();

	SgBasicBlock* fwd_true_block_body = isSgBasicBlock(proc_true_body.forwardStatement);
	SgBasicBlock* fwd_false_block_body = isSgBasicBlock(proc_false_body.forwardStatement);

	ROSE_ASSERT(fwd_true_block_body);
	ROSE_ASSERT(fwd_false_block_body);


	// Get the attribute which tells us whether to store the branch flag.
	SgExpression* condition = evalResult.getAttribute<SgExpression*>();
	SgStatement* cond_stmt = NULL;
	if (condition)
		cond_stmt = buildExprStatement(condition);
	else
	{
		// In this situation, we don't have to store branch flag.
		appendStatement(buildExprStatement(
				pushVal(buildBoolValExp(true))),
				fwd_true_block_body);
		appendStatement(buildExprStatement(
				pushVal(buildBoolValExp(false))),
				fwd_false_block_body);
		cond_stmt = buildExprStatement(popVal(buildBoolType()));
	}

	SgStatement* fwd_stmt = buildIfStmt(proc_cond.forwardStatement, proc_true_body.forwardStatement, proc_false_body.forwardStatement);
	SgStatement* rvs_stmt = buildIfStmt(cond_stmt, proc_true_body.reverseStatement, proc_false_body.reverseStatement);
	rvs_stmt = buildBasicBlock(rvs_stmt, proc_cond.reverseStatement);

	return StatementReversal(fwd_stmt, rvs_stmt);
}

EvaluationResult SgIfStmt_Handler::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
	SgIfStmt* if_stmt = isSgIfStmt(stmt);
	if (if_stmt == NULL)
		return EvaluationResult();

	// Make sure every if statement has a true and false body after being normalized.
	ROSE_ASSERT(if_stmt->get_false_body());

	SgBasicBlock* true_body = isSgBasicBlock(if_stmt->get_true_body());
	SgBasicBlock* false_body = isSgBasicBlock(if_stmt->get_false_body());
	ROSE_ASSERT(true_body && false_body);

	VariableVersionTable true_body_var_table, false_body_var_table;
	tie(true_body_var_table, false_body_var_table) = var_table.getVarTablesForIfBodies(true_body, false_body);

	EvaluationResult true_body_res = evaluateStatement(true_body, true_body_var_table);
	EvaluationResult false_body_res = evaluateStatement(false_body, false_body_var_table);

	SimpleCostModel cost;

	// Set the cost of true body.
	cost.setBranchCost(if_stmt, true_body_res.getCost(), true);

	// Set the cost of false body.
	cost.setBranchCost(if_stmt, false_body_res.getCost(), false);

	// Here we merge two variable version table from true and false bodies.
	// For each variable, if it has the different versions from those two tables, we
	// clear its version (it has no version now).
	VariableVersionTable new_table = true_body_res.getVarTable();
	new_table.setUnion(false_body_res.getVarTable());

	EvaluationResult cond_result = evaluateStatement(if_stmt->get_conditional(), new_table);


	EvaluationResult totalEvaluationResult(this, stmt, new_table);
	totalEvaluationResult.addChildEvaluationResult(false_body_res);
	totalEvaluationResult.addChildEvaluationResult(true_body_res);
	totalEvaluationResult.addChildEvaluationResult(cond_result);

	totalEvaluationResult.setVarTable(new_table);

	// Here we should do an analysis to decide whether to store the branch flag.
	// If the value of the condition is not modified during both true and false bodies,
	// we can still use that expression. Or we can retrieve the same value from other
	// expressions. Even more complicated analysis may be performed here.

	SimpleCostModel new_cost = cost;
	SgExpression* attr = NULL;

	SgExpression* cond = isSgExprStatement(if_stmt->get_conditional())->get_expression();
	if (SgExpression * restored_exp = restoreExpressionValue(cond, var_table))
	{
		// If we can restore the value of condition, we can use it without storing the flag.
		attr = restored_exp;
	}
	else
	{
		// Since we store the branch flag here, we add the cost by 1.
		new_cost.increaseStoreCount();
	}

	totalEvaluationResult.setCost(new_cost);
	totalEvaluationResult.setAttribute(attr);

	return totalEvaluationResult;
}