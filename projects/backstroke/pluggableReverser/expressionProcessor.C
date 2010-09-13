#include "expressionProcessor.h"
#include "utilities/Utilities.h"
#include "utilities/CPPDefinesAndNamespaces.h"
#include "pluggableReverser/eventProcessor.h"

using namespace SageInterface;
using namespace SageBuilder;
using namespace backstroke_util;

/******************************************************************************
 ******** Definition of member functions of NullExpressionProcessor ***********/

ExpressionReversal NullExpressionHandler::generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult)
{
	return ExpressionReversal(copyExpression(exp), NULL);
}

vector<EvaluationResult> NullExpressionHandler::evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool reverseValueUsed)
{
	vector<EvaluationResult> results;

	// If the value of the expression is used, we cannot return NULL.
	if (reverseValueUsed)
		return results;
	//SgExpression* exp = exp_pkg.exp;

	// We want to check variable version table to check the validity to ignore any
	// expression or statemetn reversal. The reason is that for the following code:
	//   if () a = 0;
	//   a = 1;
	// we want to reverse it into
	//   a = pop();
	// Here we can not use this processor to process 'a = 1;', since it may be the first def.

#if 0
	if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp) || isAssignmentOp(exp))
	{
		SgExpression* var = NULL;
		if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp))
			var = isSgUnaryOp(exp)->get_operand();
		else if (isAssignmentOp(exp))
			var = isSgBinaryOp(exp)->get_lhs_operand();

		ROSE_ASSERT(isSgVarRefExp(var) || isSgDotExp(var) || isSgArrowExp(var));

		if (isStateVariable(var) && var_table.isUsingFirstDefinition(var))
			return results;
	}
#endif

	results.push_back(EvaluationResult(this, exp, var_table));
	return results;
}

/******************************************************************************
 **** Definition of member functions of IdentityExpressionProcessor ***********/

struct IdentityExpressionAttribute : public EvaluationResultAttribute
{
	bool reverseIsNull;
};

ExpressionReversal IdentityExpressionHandler::generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult)
{
	ROSE_ASSERT(evaluationResult.getExpressionHandler() == this && evaluationResult.getChildResults().size() == 0);
	IdentityExpressionAttribute* attribute = dynamic_cast<IdentityExpressionAttribute*>(evaluationResult.getAttribute().get());
	ROSE_ASSERT(attribute != NULL);

	SgExpression* forwardExpression = SageInterface::copyExpression(exp);
	SgExpression* reverseExpression;
	if (attribute->reverseIsNull)
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
	if (!backstroke_util::isModifyingExpression(exp))
	{
		EvaluationResult result(this, exp, var_table);
		IdentityExpressionAttribute* attribute = new IdentityExpressionAttribute;
		attribute->reverseIsNull = !reverseValueUsed;
		
		result.setAttribute(EvaluationResultAttributePtr(attribute));
		results.push_back(result);
	}

	return results;
}

/******************************************************************************
 **** Definition of member functions of StoreAndRestoreExpressionProcessor ****/

struct StoreAndRestoreAttribute : public EvaluationResultAttribute
{
	StoreAndRestoreAttribute(SgExpression* varExp) : var_to_save(varExp) {}

	SgExpression* var_to_save;
};

ExpressionReversal StoreAndRestoreExpressionHandler::generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult)
{
	StoreAndRestoreAttribute* attribute = dynamic_cast<StoreAndRestoreAttribute*>(evaluationResult.getAttribute().get());
	ROSE_ASSERT(attribute != NULL);
	SgExpression* var_to_save = attribute->var_to_save;
	ROSE_ASSERT(var_to_save);

	SgExpression* fwd_exp = buildBinaryExpression<SgCommaOpExp > (
			pushVal(copyExpression(var_to_save), var_to_save->get_type()),
			copyExpression(exp));
	SgExpression* rvs_exp = buildBinaryExpression<SgAssignOp > (
			copyExpression(var_to_save),
			popVal(var_to_save->get_type()));

	return ExpressionReversal(fwd_exp, rvs_exp);
}

vector<EvaluationResult> StoreAndRestoreExpressionHandler::evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used)
{
	SgExpression* var_to_save = NULL;

	if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp))
		var_to_save = isSgUnaryOp(exp)->get_operand();
	else if (isAssignmentOp(exp))
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
		result.setAttribute(EvaluationResultAttributePtr(new StoreAndRestoreAttribute(var_to_save)));
		results.push_back(result);
	}

	return results;
}

/******************************************************************************
 ***** Definition of member functions of ConstructiveExpressionProcessor ******/

ExpressionReversal ConstructiveExpressionHandler::generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult)
{
	if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp))
	{
		// Note that after normalization, a plusplus or minusminus operator expression
		// is not used by another expression, which makes its transformation much easier.

		// Make sure the expression is of integer type.
		SgExpression* operand = isSgUnaryOp(exp)->get_operand();
		if (operand->get_type()->isIntegerType())
		{
			// ++ and -- can both be reversed without state saving

			// Condition: the variable has the same index in the current table as in
			// original code.
			// For example:
			//     ++i(2);
			// To make sure it is reversed correctly, i should has the version number 2
			// in the variable version table.

			if (SgPlusPlusOp * pp_op = isSgPlusPlusOp(exp))
			{
				SgExpression* new_exp = buildMinusMinusOp(
						copyExpression(operand),
						backstroke_util::reverseOpMode(pp_op->get_mode()));
				return ExpressionReversal(copyExpression(exp), new_exp);
			}

			if (SgMinusMinusOp * mm_op = isSgMinusMinusOp(exp))
			{
				SgExpression* new_exp = buildPlusPlusOp(
						copyExpression(operand),
						backstroke_util::reverseOpMode(mm_op->get_mode()));
				return ExpressionReversal(copyExpression(exp), new_exp);
			}
		}
	}

	// The following code deals with three kinds of constructive assignment operators: +=, -=, ^=
	if (isAssignmentOp(exp))
	{
		SgExpression* lhs_operand = isSgBinaryOp(exp)->get_lhs_operand();
		SgExpression* rhs_operand = isSgBinaryOp(exp)->get_rhs_operand();

		if (lhs_operand->get_type()->isIntegerType())
		{
			// We must make sure that the rhs operand does not contain the lhs operand.
			// Or else, this operation is not constructive. For example, a += a or a += a + b.
			// This can also be done by def-use analysis.

			bool constructive = true;
			vector<SgExpression*> exp_list = backstroke_util::querySubTree<SgExpression > (rhs_operand);

			foreach(SgExpression* exp, exp_list)
			{
				if (areSameVariable(exp, lhs_operand))
				{
					constructive = false;
					break;
				}
			}

			// Condition: every variable in both lhs and rhs operand has the same index in
			// the current table as in original code.
			// For example:
			//     a(1) += b(2);
			// To make sure it is reversed correctly, a should has the version number 1 and
			// b should has the version number 2 in the variable version table.

			if (constructive)
			{
				if (isSgPlusAssignOp(exp))
				{
					SgExpression* new_exp = buildBinaryExpression<SgMinusAssignOp > (
							copyExpression(lhs_operand),
							copyExpression(rhs_operand));
					return ExpressionReversal(copyExpression(exp), new_exp);
				}

				if (isSgMinusAssignOp(exp))
				{
					SgExpression* new_exp = buildBinaryExpression<SgPlusAssignOp > (
							copyExpression(lhs_operand),
							copyExpression(rhs_operand));
					return ExpressionReversal(copyExpression(exp), new_exp);
				}

				if (isSgXorAssignOp(exp))
				{
					return ExpressionReversal(copyExpression(exp), copyExpression(exp));
				}

#if 0
				// we must ensure that the rhs operand of *= is not ZERO
				if (isSgMultAssignOp(bin_op) && !isZero(isSgValueExp(rhs_operand)))
					// if the rhs_operand is a value and the value is not 0
					return ExpPair(
						fwd_exp,
						buildBinaryExpression<SgDivAssignOp > (
						copyExpression(model_var),
						copyExpression(rhs_operand)));

				if (isSgLshiftAssignOp(bin_op))
					return ExpPair(
						fwd_exp,
						buildBinaryExpression<SgRshiftAssignOp > (
						copyExpression(model_var),
						copyExpression(rhs_operand)));
#endif
			}
		} // if (lhs_operand->get_type()->isIntegerType())
	}

	ROSE_ASSERT(false);
	return ExpressionReversal(NULL, NULL);
}

vector<EvaluationResult> ConstructiveExpressionHandler::evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used)
{
	vector<EvaluationResult> results;
	//SgExpression* exp = exp_pkg.exp;

	if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp))
	{
		// Note that after normalization, a plusplus or minusminus operator expression
		// is not used by another expression, which makes its transformation much easier.

		// Make sure the expression is of integer type.
		SgExpression* operand = isSgUnaryOp(exp)->get_operand();
		if (operand->get_type()->isIntegerType())
		{
			// ++ and -- can both be reversed without state saving

			// Condition: the variable has the same index in the current table as in
			// original code.
			// For example:
			//     ++i(2);
			// To make sure it is reversed correctly, i should has the version number 2
			// in the variable version table.


			if (var_table.checkVersionForDef(operand))
			{
				// Once reversed, the version number should backward.
				VariableVersionTable new_table(var_table);
				new_table.reverseVersion(operand);

				if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp))
				{
					EvaluationResult result = EvaluationResult(this, exp, new_table);
					results.push_back(result);
				}
			}
		}
	}

	// The following code deals with three kinds of constructive assignment operators: +=, -=, ^=
	if (isAssignmentOp(exp))
	{
		SgExpression* lhs_operand = isSgBinaryOp(exp)->get_lhs_operand();
		SgExpression* rhs_operand = isSgBinaryOp(exp)->get_rhs_operand();

		if (lhs_operand->get_type()->isIntegerType())
		{
			// We must make sure that the rhs operand does not contain the lhs operand.
			// Or else, this operation is not constructive. For example, a += a or a += a + b.
			// This can also be done by def-use analysis.

			bool constructive = true;
			vector<SgExpression*> exp_list = backstroke_util::querySubTree<SgExpression > (rhs_operand);

			foreach(SgExpression* exp, exp_list)
			{
				if (areSameVariable(exp, lhs_operand))
				{
					constructive = false;
					break;
				}
			}

			// Condition: every variable in both lhs and rhs operand has the same index in
			// the current table as in original code.
			// For example:
			//     a(1) += b(2);
			// To make sure it is reversed correctly, a should has the version number 1 and
			// b should has the version number 2 in the variable version table.

			if (var_table.checkVersionForDefUse(lhs_operand, rhs_operand) &&
					constructive)
			{
				// Once reversed, the version number should backward.
				VariableVersionTable new_table(var_table);
				new_table.reverseVersion(lhs_operand);

				if (isSgPlusAssignOp(exp) ||
						isSgMinusAssignOp(exp) ||
						isSgXorAssignOp(exp))
				{
					EvaluationResult result = EvaluationResult(this, exp, new_table);
					results.push_back(result);
				}

#if 0
				// we must ensure that the rhs operand of *= is not ZERO
				if (isSgMultAssignOp(bin_op) && !isZero(isSgValueExp(rhs_operand)))
					// if the rhs_operand is a value and the value is not 0
					return ExpPair(
						fwd_exp,
						buildBinaryExpression<SgDivAssignOp > (
						copyExpression(model_var),
						copyExpression(rhs_operand)));

				if (isSgLshiftAssignOp(bin_op))
					return ExpPair(
						fwd_exp,
						buildBinaryExpression<SgRshiftAssignOp > (
						copyExpression(model_var),
						copyExpression(rhs_operand)));
#endif
			}
		} // if (lhs_operand->get_type()->isIntegerType())
	}

	return results;
}

/*

// This function deals with assignment like a = b + c + a, which is still constructive.
InstrumentedExpressionVec ConstructiveAssignmentProcessor::process(const ExpressionPackage& exp_pkg)
{
	InstrumentedExpressionVec output;
	SgExpression* exp = exp_pkg.exp;

	if (isSgAssignOp(exp))
	{
		SgExpression* lhs_operand = isSgBinaryOp(exp)->get_lhs_operand();
		SgExpression* rhs_operand = isSgBinaryOp(exp)->get_rhs_operand();

		// We have to record the sign of each variable.
		typedef pair<SgExpression*, bool> VarWithSign;
		vector<VarWithSign> vars;

		// The following loop get all variables with their signs in the add/subtract expression.
		queue<VarWithSign> to_process;
		to_process.push(VarWithSign(rhs_operand, true));
		while (!to_process.empty())
		{
			VarWithSign var = to_process.front();
			SgExpression* e = var.first;
			bool is_plus = var.second;
			to_process.pop();

			if (SgAddOp* add_op = isSgAddOp(e))
			{
				to_process.push(VarWithSign(add_op->get_lhs_operand(), is_plus));
				to_process.push(VarWithSign(add_op->get_rhs_operand(), is_plus));
			}
			else if (SgSubtractOp* sub_op = isSgSubtractOp(e))
			{
				to_process.push(VarWithSign(sub_op->get_lhs_operand(), is_plus));
				to_process.push(VarWithSign(sub_op->get_rhs_operand(), !is_plus));
			}
			else if (SgUnaryAddOp* plus_op = isSgUnaryAddOp(e))
				to_process.push(VarWithSign(plus_op->get_operand(), is_plus));
			else if (SgMinusOp* minus_op = isSgMinusOp(e))
				to_process.push(VarWithSign(minus_op->get_operand(), !is_plus));
			else
				vars.push_back(var);
		}

		// Count the number of the same variable as lhs operand which appear in the rhs operand.
		int count = 0;
		size_t index;

		// Also check if the expression is constructive or not.
		//   If one subexpression contains the same variable as lhs operand, it is not constructive.
		//       a = a + a * b;
		//   Then check the version of every variable.

		bool constructive = exp_pkg.var_table.checkVersion(lhs_operand, rhs_operand);
		for (size_t i = 0; i < vars.size(); ++i)
		{
			if (areSameVariable(vars[i].first, lhs_operand))
			{
				++count;
				index = i;
			}
			else if (containsVariable(vars[i].first, lhs_operand))
			{
				constructive = false;
				break;
			}
		}

		SgExpression* fwd_exp = copyExpression(exp);

		// Only if the count of lhs operand in rhs operand is 1 can this expression be
		// constructive. For example, a = a + a + b is not constructive (considering overflow).
		if (constructive && count == 1)
		{
			// Once reversed, the version number should backward.
			VariableVersionTable new_var_table(exp_pkg.var_table);
			new_var_table.reverseVersion(lhs_operand);

			// The form a = b - a, the reverse expression is the same.
			if (!vars[index].second)
			{
				SgExpression* rvs_exp = buildBinaryExpression<SgAssignOp>(
						copyExpression(lhs_operand),
						copyExpression(rhs_operand));
				output.push_back(InstrumentedExpression(fwd_exp, rvs_exp, new_var_table));
				return output;
			}

			// The following code may be replaced with using replaceExpression() function.

			//vars[index].first = lhs_operand;

			// Concatenate all other variables.
			SgExpression* rvs_exp = NULL;
			for (size_t i = 0; i < vars.size(); ++i)
			{
				if (i == index) continue;

				if (rvs_exp == NULL)
				{
					rvs_exp = copyExpression(vars[i].first);
					if (!vars[i].second)
						rvs_exp = buildUnaryExpression<SgMinusOp>(rvs_exp);
				}
				else
				{
					if (vars[i].second)
						rvs_exp = buildBinaryExpression<SgAddOp>(
								rvs_exp,
								copyExpression(vars[i].first));
					else
						rvs_exp = buildBinaryExpression<SgSubtractOp>(
								rvs_exp,
								copyExpression(vars[i].first));
				}
			}

			if (vars[index].second)
			{
				// a = b + a  ->  a = a - b
				if (rvs_exp)
					rvs_exp = buildBinaryExpression<SgSubtractOp>(
							copyExpression(lhs_operand),
							rvs_exp);
				// a = +a
				else
					rvs_exp = copyExpression(lhs_operand);
			}
#if 1
			else
			{
				// a = b - a  ->  a = b - a
				if (rvs_exp)
					rvs_exp = buildBinaryExpression<SgSubtractOp>(
							rvs_exp,
							copyExpression(lhs_operand));
				// a = -a
				else
					rvs_exp = buildUnaryExpression<SgMinusOp>(
							copyExpression(lhs_operand));

			}
#endif
			rvs_exp = buildBinaryExpression<SgAssignOp>(
					copyExpression(lhs_operand),
					rvs_exp);

			output.push_back(InstrumentedExpression(fwd_exp, rvs_exp, new_var_table));
		}
	}

	return output;
}


// Process conditional expressions. Note that after normalization, the return value of 
// a conditional expression will not be used so that we can push the flag after the 
// evaluation of the true or false expression. That is:
//     a ? b : c  ==>  a ? (b, push(1)) : (c, push(0))
//                     pop() ? r(b) : r(c)
InstrumentedExpressionVec processConditionalExpression(const ExpressionPackage& exp_pkg)
{
	InstrumentedExpressionVec output;
	SgExpression* exp = exp_pkg.exp;

	if (isSgConditionalExp(exp))
	{
	}

	return output;
}
 */
