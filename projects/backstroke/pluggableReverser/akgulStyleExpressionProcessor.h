#pragma once

#include <rose.h>
#include <boost/tuple/tuple.hpp>
#include "utilities/types.h"
#include "VariableRenaming.h"
#include <map>
#include <set>
#include <pluggableReverser/eventProcessor.h>

class AkgulStyleExpressionProcessor : public ExpressionProcessor
{
public:

	/** Attempts to reverse an expression. If the reversal fails,
	  * this function returns an empty vector. */
	virtual std::vector<InstrumentedExpression> process(SgExpression* expression, const VariableVersionTable& variableTable, bool reverseValueUsed);

	/** Initialize the reverser for a given AST. */
	AkgulStyleExpressionProcessor(SgProject* project);

private:

	
	/** Reverses an assignment op. Returns true on success and false on failure.
	 * @param reverseExpressions a list of expressions, to be executed in the specified order */
	bool handleAssignOp(SgAssignOp* varRef, SgExpression*& reverseExpressions);

	/**
	 *
     * @param variable name of the variable to be restored
     * @param referenceExpression an expression that can be used in the AST to refer to that variable
     * @param useSite location where the reverse expression will go
     * @param definitions the desired version of the variable
     * @param reverseExpressions side-effect free expression that evaluates to the desired version of the given variable
     * @return true on success, false on failure
     */
	bool restoreVariable(VariableRenaming::VarName variable, SgNode* useSite,
		VariableRenaming::NumNodeRenameEntry definitions, SgExpression*& reverseExpression);

	bool useReachingDefinition(VariableRenaming::VarName destroyedVarName, SgNode* useSite,
		VariableRenaming::NumNodeRenameEntry definitions, SgExpression*& reverseExpression);

	bool extractFromUse(VariableRenaming::VarName varName, SgNode* useSite,
		VariableRenaming::NumNodeRenameEntry defintions, SgExpression*& reverseExpression);

	/** Returns the variable name referred by the expression. Also returns
	  * the AST expression for referring to that variable (using the variable renaming analysis).
	  * Handles comma ops correctly. */
	std::pair<VariableRenaming::VarName, SgExpression*> getReferredVariable(SgExpression* exp);

	/** Returns true if an expression calls any functions or modifies any variables. */
	bool isModifyingExpression(SgExpression* expr);

	/** This is a replacement for ROSE's def-use analysis. */
	VariableRenaming variableRenamingAnalysis;

	std::multimap<int, SgExpression*> collectUsesForVariable(VariableRenaming::VarName name, SgNode* node);

	std::vector<SgExpression*> findVarReferences(VariableRenaming::VarName var, SgNode* root);
};
