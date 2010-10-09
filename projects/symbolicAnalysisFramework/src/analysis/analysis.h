#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"

class Analysis;

#include "lattice.h"
#include "nodeState.h"
#include "variables.h"
#include "varSets.h"
#include <vector>
#include <set>
#include <map>

using namespace std;

extern int analysisDebugLevel;

class Analysis
{
	public:
	Analysis() {}
};

class InterProceduralAnalysis;

class IntraProceduralAnalysis : virtual public Analysis
{
	protected:
	InterProceduralAnalysis* interAnalysis;
	
	public:
	void setInterAnalysis(InterProceduralAnalysis* interAnalysis)
	{ this->interAnalysis = interAnalysis; }
	
	// runs the intra-procedural analysis on the given function, returns true if 
	// the function's NodeState gets modified as a result and false otherwise
	// state - the function's NodeState
	virtual bool runAnalysis(const Function& func, NodeState* state)=0;
};

class InterProceduralAnalysis : virtual public Analysis
{
	protected:
	IntraProceduralAnalysis* intraAnalysis;
	
	InterProceduralAnalysis(IntraProceduralAnalysis* intraAnalysis)
	{
		this->intraAnalysis = intraAnalysis; 
		// inform the intra-procedural analysis that this inter-procedural analysis will be running it
		intraAnalysis->setInterAnalysis(this);
	}
	
	virtual void runAnalysis()=0;
};

/********************************
 *** UnstructuredPassAnalyses ***
 ********************************/

class UnstructuredPassIntraAnalysis : virtual public IntraProceduralAnalysis
{
	public:
	// runs the intra-procedural analysis on the given function, returns true if 
	// the function's NodeState gets modified as a result and false otherwise
	// state - the function's NodeState
	bool runAnalysis(const Function& func, NodeState* state);
	
	virtual void visit(const Function& func, const DataflowNode& n, NodeState& state)=0;
};

class UnstructuredPassInterAnalysis : virtual public InterProceduralAnalysis
{
	public:
	UnstructuredPassInterAnalysis(IntraProceduralAnalysis& intraAnalysis) : InterProceduralAnalysis(&intraAnalysis)
	{ }
		
	void runAnalysis();
};

#endif
