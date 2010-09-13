#pragma once

#include <rose.h>
#include <boost/tuple/tuple.hpp>
#include "utilities/types.h"
#include "VariableRenaming.h"
#include <map>
#include <set>
#include <pluggableReverser/eventProcessor.h>

class AkgulStyleExpressionProcessor : public ExpressionReversalHandler
{
public:
	virtual ExpressionReversal generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult);

	/** Attempts to reverse an expression. If the reversal fails,
	* this function returns an empty vector. */
	virtual std::vector<EvaluationResult> evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used);

	/** Returns the variable name referred by the expression. Also returns
	* the AST expression for referring to that variable (using the variable renaming analysis).
	* Handles comma ops correctly.
	*/
	static std::pair<VariableRenaming::VarName, SgExpression*> getReferredVariable(SgExpression* exp);

private:

	std::multimap<int, SgExpression*> collectUsesForVariable(VariableRenaming::VarName name, SgNode* node);
};

/** The redefine technique re-executes the reaching definition to obtain the value of a variable. */
class RedefineValueRestorer : public VariableValueRestorer
{
	/**
	* Given a variable and a version, returns an expression evaluating to the value of the variable
	* at the given version.
	*
	* @param variable name of the variable to be restored
	* @param availableVariables variables whose values are currently available
	* @param definitions the version of the variable which should be restored
	* @return expessions that when evaluated will produce the desired version of the variable
	*/
	virtual std::vector<SgExpression*> restoreVariable(VariableRenaming::VarName variable, 
			const VariableVersionTable& availableVariables, VariableRenaming::NumNodeRenameEntry definitions);

private:



	/** Returns true if an expression calls any functions or modifies any variables. */
	static bool isModifyingExpression(SgExpression* expr, VariableRenaming* variableRenamingAnalysis);
};

class ExtractFromUseRestorer : public VariableValueRestorer
{
	/**
	* Given a variable and a version, returns an expression evaluating to the value of the variable
	* at the given version.
	*
	* @param variable name of the variable to be restored
	* @param availableVariables variables whos values are currently available
	* @param definitions the version of the variable which should be restored
	* @return expessions that when evaluated will produce the desired version of the variable
	*/
	virtual std::vector<SgExpression*> restoreVariable(VariableRenaming::VarName variable, 
			const VariableVersionTable& availableVariables, VariableRenaming::NumNodeRenameEntry definitions);
};
