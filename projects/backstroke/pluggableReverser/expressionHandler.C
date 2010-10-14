#include "expressionHandler.h"
#include "utilities/Utilities.h"
#include "utilities/CPPDefinesAndNamespaces.h"
#include "pluggableReverser/eventHandler.h"

using namespace SageBuilder;

/******************************************************************************
 ******** Definition of member functions of NullExpressionHandler  ***********/

ExpressionReversal NullExpressionHandler::generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult)
{
	return ExpressionReversal(SageInterface::copyExpression(exp), NULL);
}

vector<EvaluationResult> NullExpressionHandler::evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool reverseValueUsed)
{
	vector<EvaluationResult> results;

	// Now NullExpressionHander only handles expressions with side effects. Those without side effects are
	// handled by IdentityExpressionHandler.
	// If the value of the expression is used, we cannot return NULL.
	if (!backstroke_util::containsModifyingExpression(exp) || reverseValueUsed)
		return results;

	//We can't ignore reversing functions
	if (!NodeQuery::querySubTree(exp, V_SgFunctionCallExp).empty())
	{
		return results;
	}

	results.push_back(EvaluationResult(this, exp, var_table));
	return results;
}

/******************************************************************************
 **** Definition of member functions of IdentityExpressionHandler  ***********/

ExpressionReversal IdentityExpressionHandler::generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult)
{
	ROSE_ASSERT(evaluationResult.getExpressionHandler() == this && evaluationResult.getChildResults().size() == 0);
	bool reverseIsNull = evaluationResult.getAttribute<bool>();

	SgExpression* forwardExpression = SageInterface::copyExpression(exp);
	SgExpression* reverseExpression;
	if (reverseIsNull)
	{
		reverseExpression = NULL;
	}
	else
	{
		reverseExpression = SageInterface::copyExpression(exp);
	}

	return ExpressionReversal(forwardExpression, reverseExpression);
}

vector<EvaluationResult> IdentityExpressionHandler::evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool reverseValueUsed)
{
	vector<EvaluationResult> results;

	// If an expression does not modify any value and its value is used, the reverse is the same as itself
	if (!backstroke_util::containsModifyingExpression(exp))
	{
		EvaluationResult result(this, exp, var_table);
		result.setAttribute(!reverseValueUsed);
		results.push_back(result);
	}

	return results;
}

/******************************************************************************
 **** Definition of member functions of StoreAndRestoreExpressionHandler ****/

ExpressionReversal StoreAndRestoreExpressionHandler::generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult)
{
	SgExpression* var_to_save = evaluationResult.getAttribute<SgExpression*>();
	ROSE_ASSERT(var_to_save);

	SgExpression* fwd_exp = buildBinaryExpression<SgCommaOpExp>(
			pushVal(SageInterface::copyExpression(var_to_save)),
			SageInterface::copyExpression(exp));
	SgExpression* rvs_exp = buildBinaryExpression<SgAssignOp>(
			SageInterface::copyExpression(var_to_save),
			popVal(var_to_save->get_type()));

	return ExpressionReversal(fwd_exp, rvs_exp);
}

vector<EvaluationResult> StoreAndRestoreExpressionHandler::evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used)
{
	SgExpression* var_to_save = NULL;

	if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp))
		var_to_save = isSgUnaryOp(exp)->get_operand();
	else if (SageInterface::isAssignmentStatement(exp))
		var_to_save = isSgBinaryOp(exp)->get_lhs_operand();

	vector<EvaluationResult> results;
	if (VariableRenaming::getVarName(var_to_save) != VariableRenaming::emptyName)
	{
		// Update the variable version table.
		VariableVersionTable new_var_table = var_table;
		new_var_table.reverseVersion(var_to_save);

		// Update the cost.
		SimpleCostModel cost;
		cost.increaseStoreCount();

		EvaluationResult result(this, exp, new_var_table, cost);
		result.setAttribute(var_to_save);
		results.push_back(result);
	}

	return results;
}
