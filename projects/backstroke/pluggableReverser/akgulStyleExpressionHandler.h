#pragma once

#include <rose.h>
#include <boost/tuple/tuple.hpp>
#include "utilities/types.h"
#include "VariableRenaming.h"
#include <map>
#include <set>
#include <pluggableReverser/eventProcessor.h>

class AkgulStyleExpressionHandler : public ExpressionReversalHandler
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


