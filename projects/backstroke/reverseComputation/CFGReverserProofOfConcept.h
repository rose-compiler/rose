#pragma once

#include <rose.h>
#include <boost/tuple/tuple.hpp>
#include "utilities/types.h"
#include "VariableRenaming.h"
#include <map>
#include <set>

class CFGReverserProofofConcept
{
public:
	/** Initialize the reverser for a given AST. */
	CFGReverserProofofConcept(SgProject* project);

	/** Attempts to reverse an expression. If the reversal fails,
	  * this function returns NULL_EXP_PAIR. */
	ExpPair ReverseExpression(SgExpression* expression);

private:

	
	/** Reverses an assignment op. Returns true on success and false on failure.
	 * @param reverseExpressions a list of expressions, to be executed in the specified order */
	bool handleAssignOp(SgAssignOp* varRef, std::vector<SgExpression*> reverseExpressions);

	/** Given a variable reference and its previous definition node, produces and expression that
	 * evaluates to the value of the variable by re-executing its definition.
	 *
     * @param destroyedVarName the variable being restored
     * @param reachingDefinition reaching definition which should be reexecuted
     * @param reverseExpressions expressions that should be executed to restore the value of the variable
     * @return  true on success, false on failure
     */
	bool useReachingDefinition(VariableRenaming::varName destroyedVarName, SgNode* reachingDefinition, std::vector<SgExpression*> reverseExpressions);

	 /** Get name:num mapping for all variables defined in the given subtree.
	  * Note that a variable may be renamed multiple times in the subtree
     *
     * This will return the combination of original and expanded defs on this node.
     *
     * ex. s.x = 5;  //This will return s.x and s  (s.x is original & s is expanded)
     *
     * @param root AST tree which will be searched for defs
     * @return A table mapping VarName->set(num, defNode) for every varName defined in the subtree
     */
	static std::map<VariableRenaming::varName, std::set<VariableRenaming::numNodeRenameEntry> > getDefsForSubtree(VariableRenaming& varRenamingAnalysis, SgNode* root);

	/** Returns true if an expression calls any functions or modifies any variables. */
	bool isModifyingExpression(SgExpression* expr);

	/** The def-use analysis used during the reversal. */
	DefUseAnalysis defUseAnalysis;

	/** This is a replacement for ROSE's def-use analysis. */
	VariableRenaming variableRenamingAnalysis;
};
