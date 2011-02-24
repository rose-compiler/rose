#include "stateSavingStatementHandler.h"
#include "ssa/staticSingleAssignment.h"
#include "eventProcessor.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/algorithm.hpp>
#include <boost/lambda/bind.hpp>
#include <utilities/utilities.h>
#include <utilities/cppDefinesAndNamespaces.h>

using namespace SageBuilder;
using namespace SageInterface;
using namespace std;

vector<VariableRenaming::VarName> StateSavingStatementHandler::getAllDefsAtNode(SgNode* node)
{
	const IVariableFilter* filter = getVariableFilter();
	vector<VariableRenaming::VarName> modified_vars;
	foreach (const StaticSingleAssignment::VarName& var_name, getSsa()->getOriginalVarsDefinedInSubtree(node))
	{
		// Get the declaration of this variable to see if it's declared inside of the given statement.
		// If so, we don't have to store this variable.
		if (!isAncestor(node, var_name[0]->get_declaration()) && filter->isVariableInteresting(var_name))
			modified_vars.push_back(var_name);
	}

	// Sort those names in lexicographical order.
	using namespace boost::lambda;
	std::sort(modified_vars.begin(), modified_vars.end(), 
			bind(ll::lexicographical_compare(),
			bind(call_begin(), _1), bind(call_end(), _1),
			bind(call_begin(), _2), bind(call_end(), _2)));

	// Here if a def is a member of another def, we only include the latter one. For example, if both a and a.i
	// are modified, we only include a in the results.
	modified_vars.erase(
		std::unique(modified_vars.begin(), modified_vars.end(), bind(BackstrokeUtility::isMemberOf, _2, _1)),
		modified_vars.end());

	return modified_vars;
}

bool StateSavingStatementHandler::checkStatement(SgStatement* stmt) const
{
	if (isSgWhileStmt(stmt) ||
		isSgIfStmt(stmt) ||
		isSgDoWhileStmt(stmt) ||
		isSgForStatement(stmt) ||
		isSgSwitchStatement(stmt))
		return true;

	if (isSgBasicBlock(stmt))
	{
		SgNode* parent_stmt = stmt->get_parent();
		if (isSgWhileStmt(parent_stmt) ||
			isSgDoWhileStmt(parent_stmt) ||
			isSgForStatement(parent_stmt) ||
			isSgSwitchStatement(parent_stmt))
			return false;
		else
			return true;
	}
	return false;
}

StatementReversal StateSavingStatementHandler::generateReverseAST(SgStatement* stmt, const EvaluationResult& eval_result)
{
	SgBasicBlock* fwd_stmt = buildBasicBlock();
    SgBasicBlock* rvs_stmt = buildBasicBlock();

	// If the following child result is empty, we don't have to reverse the target statement.
	vector<EvaluationResult> child_result = eval_result.getChildResults();
	if (!child_result.empty())
	{
		StatementReversal child_reversal = child_result[0].generateReverseStatement();
		SageInterface::prependStatement(child_reversal.fwd_stmt, fwd_stmt);
		SageInterface::appendStatement(child_reversal.rvs_stmt, rvs_stmt);
	}
	else
	{
		//In the forward code, include a copy of the original statement
		SageInterface::prependStatement(copyStatement(stmt), fwd_stmt);
	}

	//Now, in the forward code, push all variables on the stack. Pop them in the reverse code
	vector<VariableRenaming::VarName> modified_vars = eval_result.getAttribute<vector<VariableRenaming::VarName> >();
	foreach (const VariableRenaming::VarName& var_name, modified_vars)
	{
		SgType* varType = var_name.back()->get_type();
		if (SageInterface::isPointerType(varType))
		{
			fprintf(stderr, "ERROR: Correctly saving pointer types not yet implemented (it's not hard)\n");
			fprintf(stderr, "The pointer is saved an restored, rather than the value it points to!\n");
			fprintf(stderr, "Variable %s\n", VariableRenaming::keyToString(var_name).c_str());
			ROSE_ASSERT(false);
		}

		SgExpression* var = VariableRenaming::buildVariableReference(var_name);
		SgExpression* fwd_exp = pushVal(var);
		SgExpression* rvs_exp = buildBinaryExpression<SgAssignOp>(
			copyExpression(var), popVal(var->get_type()));
		
		SageInterface::prependStatement(buildExprStatement(fwd_exp), fwd_stmt);
		SageInterface::appendStatement(buildExprStatement(rvs_exp), rvs_stmt);
	}

	return StatementReversal(fwd_stmt, rvs_stmt);
}

std::vector<EvaluationResult> StateSavingStatementHandler::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
	vector<EvaluationResult> results;

	// Currently, we just perform this state saving handler on if/while/for/do-while/switch statements and pure
	// basic block which is not the body of if/while/for/do-while/switch statements.
	if (!checkStatement(stmt))
		return results;

	// In case of infinite calling to this function.
	if (evaluating_stmts_.count(stmt) > 0)
		return results;

	vector<VariableRenaming::VarName> modified_vars = getAllDefsAtNode(stmt);

#if 0
	string name;
	if (isSgFunctionDefinition(stmt->get_parent()))
		name = isSgFunctionDefinition(stmt->get_parent())->get_declaration()->get_name();
	cout << "Modified vars in " << name << ":\n";
	foreach (const VariableRenaming::VarName& name, modified_vars)
		cout << VariableRenaming::keyToString(name) << endl;
	cout << "^^^\n";
#endif
	
	VariableVersionTable new_table = var_table;
	new_table.reverseVersionAtStatementStart(stmt);

#if 0
	cout << "\n\n";
	new_table.print();
	cout << "\n\n";
#endif

#if USE_OTHER_HANDLERS
	// Reverse the target statement using other handlers.
	evaluating_stmts_.insert(stmt);
	vector<EvaluationResult> eval_results = evaluateStatement(stmt, var_table);
	evaluating_stmts_.erase(stmt);

	// We combine both state saving and reversed target statement together.
	// In a following analysis on generated code, those extra store and restores will be removed.
	foreach (const EvaluationResult& eval_result, eval_results)
	{
		EvaluationResult result(this, stmt, new_table);
		result.addChildEvaluationResult(eval_result);
		// Add the attribute to the result.
		result.setAttribute(modified_vars);
		results.push_back(result);
	}
#endif

	// Here we just use state saving.
	EvaluationResult result(this, stmt, new_table);
	// Add the attribute to the result.
	result.setAttribute(modified_vars);
	results.push_back(result);

	return results;
}

SgStatement* StateSavingStatementHandler::generateCommitAST(const EvaluationResult& evalResult)
{
	//Pop all the functions
	SgBasicBlock* commitBody = SageBuilder::buildBasicBlock();

	vector<VariableRenaming::VarName> modified_vars = evalResult.getAttribute< vector<VariableRenaming::VarName> >();
	foreach (const VariableRenaming::VarName&  varName, modified_vars)
	{
		SgType* varType = varName.back()->get_type();
		SgExpression* popExpression = popVal_front(varType);
		SageInterface::prependStatement(SageBuilder::buildExprStatement(popExpression), commitBody);
	}

	return commitBody;
}