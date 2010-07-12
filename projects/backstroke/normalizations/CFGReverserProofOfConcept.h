#pragma once

#include "rose.h"
#include <boost/tuple/tuple.hpp>
#include "reverseComputation/eventReverser.h"

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

	/** Attempts to reverse an expression. If the reversal fails,
	  * this function returns NULL_EXP_PAIR. */
	ExpPair ReverseExpression(SgExpression* expression);

private:

	

	void handleVarRef(SgVarRefExp* varRef);

	void useReachingDefinition(SgVarRefExp* varRef, SgNode* reachingDefinition);

	/** The def-use analysis used during the reversal. */
	DefUseAnalysis defUseAnalysis;
};
