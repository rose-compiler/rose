#pragma once

#include "rose.h"
#include <boost/tuple/tuple.hpp>
#include "utilities/types.h"

class CFGReverserProofofConcept
{
public:
	/** Initialize the reverser for a given AST. */
	CFGReverserProofofConcept(SgProject* project);

	/** Attempts to reverse an expression. If the reversal fails,
	  * this function returns NULL_EXP_PAIR. */
	ExpPair ReverseExpression(SgExpression* expression);

private:

	

	std::vector<SgExpression*> handleAssignOp(SgVarRefExp* varRef);

	void useReachingDefinition(SgVarRefExp* varRef, SgNode* reachingDefinition);

	/** The def-use analysis used during the reversal. */
	DefUseAnalysis defUseAnalysis;
};
