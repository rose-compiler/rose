#ifndef LIVE_DEAD_VAR_ANALYSIS_H
#define LIVE_DEAD_VAR_ANALYSIS_H

#include "common.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "liveDeadVarAnalysis.h"
#include "printAnalysisStates.h"

extern int liveDeadAnalysisDebugLevel;

class LiveDeadVarsAnalysis : public IntraBWDataflow
{
	protected:
	
	
	public:
	LiveDeadVarsAnalysis(): IntraBWDataflow()
	{
	}
	
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
	
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
};

// prints the Lattices set by the given LiveDeadVarsAnalysis 
void printLiveDeadVarsAnalysisStates(LiveDeadVarsAnalysis* da, string indent="");

#endif