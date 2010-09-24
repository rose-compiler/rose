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

set<SgExpression*> getAllModifiedVariables(SgStatement* stmt)
{
	set<SgExpression*> modified_vars;

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
			SgVarRefExp* var_ref = getMostLeftVariable(var);

			if (var_ref)
			{
				// Get the declaration of this variable to see if it's declared inside of the given statement.
				// In this case, we don't have to store this variable.
				SgDeclarationStatement* decl = var_ref->get_symbol()->get_declaration()->get_declaration();
				if (!isAncestor(stmt, decl))
				{
					// We store each variable once.
					if (find_if(modified_vars.begin(), modified_vars.end(),
							bind(backstroke_util::areSameVariable, _1, var)) == modified_vars.end())
						modified_vars.insert(var);
				}
			}
		}
	}
}

StatementReversal StateSavingStatementHandler::generateReverseAST(SgStatement* stmt, const EvaluationResult& eval_result)
{
	return StatementReversal(NULL, NULL);
}

std::vector<EvaluationResult> StateSavingStatementHandler::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
	vector<EvaluationResult> results;

	vector<EvaluationResult> eval_results = evaluateStatement(stmt, var_table);
	set<SgExpression*> modified_vars = getAllModifiedVariables(stmt);


	VariableVersionTable new_table = var_table;
	foreach (SgExpression* var, modified_vars)
	{
		//new_table.reverseVersion(var, stmt);
	}

	foreach (const EvaluationResult& eval_result, eval_results)
	{
		EvaluationResult new_eval_result(this, stmt, new_table);
	}


	// For each result from other handlers, we try to store all kinds of combinations of modifed variables.
	return results;
}