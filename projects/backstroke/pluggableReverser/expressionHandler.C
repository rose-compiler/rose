#include "expressionHandler.h"
#include "utilities/utilities.h"
#include "utilities/cppDefinesAndNamespaces.h"
#include "pluggableReverser/eventProcessor.h"

using namespace std;
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
	if (!BackstrokeUtility::containsModifyingExpression(exp) || reverseValueUsed)
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
	if (!BackstrokeUtility::containsModifyingExpression(exp))
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
	vector<EvaluationResult> results;
	SgExpression* var_to_save = NULL;

	if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp))
		var_to_save = isSgUnaryOp(exp)->get_operand();
	else if (SageInterface::isAssignmentStatement(exp))
		var_to_save = isSgBinaryOp(exp)->get_lhs_operand();

	if (var_to_save == NULL)
		return results;

	if (VariableRenaming::getVarName(var_to_save) != VariableRenaming::emptyName)
	{
		SgType* varType = VariableRenaming::getVarName(var_to_save).back()->get_type();
		if (SageInterface::isPointerType(varType))
		{
			fprintf(stderr, "ERROR: Correctly saving pointer types not yet implemented (it's not hard)\n");
			fprintf(stderr, "The pointer is saved an restored, rather than the value it points to!\n");
			fprintf(stderr, "Variable %s\n", var_to_save->unparseToString().c_str());
			ROSE_ASSERT(false);
		}

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
