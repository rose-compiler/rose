#ifndef SCALAR_FW_DATAFLOW_H
#define SCALAR_FW_DATAFLOW_H

#include <sstream>
#include <iostream>
#include <string>
#include <functional>
#include <queue>

#include "common.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "rwAccessLabeler.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "liveDeadVarAnalysis.h"
#include "divAnalysis.h"
// GB : 2011-03-05 (Removing Sign Lattice Dependence) #include "sgnAnalysis.h"
#include "affineInequality.h"

#include "ConstrGraph.h"

class ConstrGraphAnalysis : public IntraFWDataflow
{
	protected:
	static map<varID, Lattice*> constVars;
	DivAnalysis* divAnalysis;
	// GB : 2011-03-05 (Removing Sign Lattice Dependence) SgnAnalysis* sgnAnalysis;
	//affineInequalitiesPlacer* affIneqPlacer;
	
	// The LiveDeadVarsAnalysis that identifies the live/dead state of all application variables.
	// Needed to create a FiniteVarsExprsProductLattice.
	LiveDeadVarsAnalysis* ldva; 
	
	public:
	ConstrGraphAnalysis(LiveDeadVarsAnalysis* ldva, DivAnalysis* divAnalysis/* GB : 2011-03-05 (Removing Sign Lattice Dependence) , SgnAnalysis* sgnAnalysis*/ /*, affineInequalitiesPlacer* affIneqPlacer*/): IntraFWDataflow()
	{
		this->divAnalysis = divAnalysis;
		// GB : 2011-03-05 (Removing Sign Lattice Dependence) this->sgnAnalysis = sgnAnalysis;
		this->ldva = ldva;
		//this->affIneqPlacer = affIneqPlacer;
		//rwAccessLabeler::addRWAnnotations(cfgUtils::getProject());
	}
	
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	//vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                     vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
	
	// Returns a map of special constant variables (such as zeroVar) and the lattices that correspond to them
	// These lattices are assumed to be constants: it is assumed that they are never modified and it is legal to 
	//    maintain only one copy of each lattice may for the duration of the analysis.
	//map<varID, Lattice*>& genConstVarLattices() const;
	
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
	
	// incorporates the current node's inequality information from conditionals (ifs, fors, etc.) into the current node's 
	// constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	bool incorporateConditionalsInfo(const Function& func, const DataflowNode& n,
	                                 NodeState& state, const vector<Lattice*>& dfInfo);
	
	// incorporates the current node's divisibility information into the current node's constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	bool incorporateDivInfo(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo, string indent="");
	
	// For any variable for which we have divisibility info, remove its constraints to other variables (other than its
	// divisibility variable)
	bool removeConstrDivVars(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo, string indent="");
};

// Prints the Lattices set by the given ConstrGraphAnalysis
void printConstrGraphAnalysisStates(ConstrGraphAnalysis* cga, string indent="");

#endif

