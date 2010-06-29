#pragma once

#include "rose.h"
#include <boost/tuple/tuple.hpp>

class CFGReverserProofofConcept
{
public:
	/** Initialize the reverser for a given AST. */
	CFGReverserProofofConcept(SgProject* project);

	/**
	 * @param body function body to reverse
	 * @return first result is the forward body; second result is the reverse body
	 */
	boost::tuple<SgBasicBlock*, SgBasicBlock*> ReverseFunctionBody(SgBasicBlock* body);

private:

	void handleExpression(SgExpression* expression);

	void handleVarRef(SgVarRefExp* varRef);

	void useReachingDefinition(SgVarRefExp* varRef, SgNode* reachingDefinition);

	/** The def-use analysis used during the reversal. */
	DefUseAnalysis defUseAnalysis;
};
