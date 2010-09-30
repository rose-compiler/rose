#include "stateSavingStatementHandler.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <utilities/Utilities.h>

using namespace std;
using namespace boost;
using namespace boost::lambda;
using namespace SageBuilder;
using namespace SageInterface;
#define foreach BOOST_FOREACH

/** Get the left most variable. For example, a.b returns a, a->b returns a. */
SgVarRefExp* getMostLeftVariable(SgExpression* exp)
{
	if (SgVarRefExp* var_ref = isSgVarRefExp(exp))
		return var_ref;
	if (SgDotExp* dot_exp = isSgDotExp(exp))
		return getMostLeftVariable(dot_exp->get_lhs_operand());
	if (SgArrowExp* arrow_exp = isSgArrowExp(exp))
		return getMostLeftVariable(arrow_exp->get_lhs_operand());
	return NULL;
}

vector<SgExpression*> getAllModifiedVariables(SgStatement* stmt)
{
	vector<SgExpression*> modified_vars;

	vector<SgExpression*> exps = backstroke_util::querySubTree<SgExpression>(stmt);
	foreach (SgExpression* exp, exps)
	{
		SgExpression* var = NULL;

		if (backstroke_util::isAssignmentOp(exp))
		{
			var = isSgBinaryOp(exp)->get_lhs_operand();
		}
		else if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp))
		{
			var = isSgUnaryOp(exp)->get_operand();
		}
		else if (isSgFunctionCallExp(exp))
		{
			// This part should be refined.
			ROSE_ASSERT(false);
		}

		if (var)
		{
			if (SgVarRefExp* var_ref = getMostLeftVariable(var))
			{
				// Get the declaration of this variable to see if it's declared inside of the given statement.
				// In this case, we don't have to store this variable.
				SgDeclarationStatement* decl = var_ref->get_symbol()->get_declaration()->get_declaration();
				if (!isAncestor(stmt, decl))
				{
					// We store each variable once.
					if (find_if(modified_vars.begin(), modified_vars.end(),
							bind(backstroke_util::areSameVariable, _1, var)) == modified_vars.end())
						modified_vars.push_back(var);
				}
			}
		}
	}

	return modified_vars;
}

StatementReversal StateSavingStatementHandler::generateReverseAST(SgStatement* stmt, const EvaluationResult& eval_result)
{
	EvaluationResultAttributePtr attr = eval_result.getAttribute();
	vector<SgExpression*> modified_vars = attr->getAttribute<vector<SgExpression*> >();

	SgBasicBlock* fwd_stmt = buildBasicBlock();
    SgBasicBlock* rvs_stmt = buildBasicBlock();

	appendStatement(copyStatement(stmt), fwd_stmt);

	foreach (SgExpression* var, modified_vars)
	{
		SgExpression* fwd_exp = pushVal(copyExpression(var));
		SgExpression* rvs_exp = buildBinaryExpression<SgAssignOp>(
			copyExpression(var), popVal(var->get_type()));
		
		appendStatement(buildExprStatement(fwd_exp), fwd_stmt);
		appendStatement(buildExprStatement(rvs_exp), rvs_stmt);
	}

	return StatementReversal(fwd_stmt, rvs_stmt);
}

std::vector<EvaluationResult> StateSavingStatementHandler::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
	vector<EvaluationResult> results;
	if (isSgWhileStmt(stmt) == NULL)
		return results;

	// In case of infinite calling to this function.
	if (evaluating_stmts_.count(stmt) > 0)
		return results;
	evaluating_stmts_.insert(stmt);

	cout << "Enter\n";

	//vector<EvaluationResult> eval_results = evaluateStatement(stmt, var_table);
	evaluating_stmts_.erase(stmt);

	vector<SgExpression*> modified_vars = getAllModifiedVariables(stmt);

	cout << "Got all modified vars.\n";

	cout << "\n\n";
	var_table.print();
	cout << "\n\n";

	VariableVersionTable new_table = var_table;
	new_table.reverseVersionAtStatementStart(modified_vars, stmt);

	cout << "\n\n";
	new_table.print();
	cout << "\n\n";

#if 0
	foreach (const EvaluationResult& eval_result, eval_results)
	{
		EvaluationResult result(this, stmt, new_table);
		result.addChildEvaluationResult(eval_result);
		results.push_back(result);
	}
#endif
	EvaluationResult result(this, stmt, new_table);
	
	// Add the attribute to the result.
	EvaluationResultAttributePtr attr(new EvaluationResultAttribute);
	attr->setAttribute(modified_vars);
	result.setAttribute(attr);

	results.push_back(result);

	return results;
}