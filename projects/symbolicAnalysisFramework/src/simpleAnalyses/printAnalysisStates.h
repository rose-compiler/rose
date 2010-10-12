#ifndef PRINT_ANALYSIS_STATES_H
#define PRINT_ANALYSIS_STATES_H

#include "common.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "lattice.h"

class printAnalysisStates : public UnstructuredPassIntraAnalysis
{
	public:
	Analysis* creator;
	vector<int> latticeNames;
	vector<int> factNames;
	string indent;
		
	printAnalysisStates(Analysis* creator, vector<int>& factNames, vector<int>& latticeNames, string indent);
	
	void visit(const Function& func, const DataflowNode& n, NodeState& state);
};

#endif
