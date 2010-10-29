#ifndef RANK_DEP_ANALYSIS_H
#define RANK_DEP_ANALYSIS_H

#include "common.h"
#include "cfgUtils.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "printAnalysisStates.h"

extern int sgnAnalysisDebugLevel;

// Maintains sign information about live variables. If a given variable may be either positive or negative, this object becomes top.
// There is one SgnLattice object for every variable
class MPIRankNProcsDepLattice : public FiniteLattice
{
	public:
/*	
	         <true, true>
	           /       \  -- 
	 <true, false>    <false, true>
	           \       /
	        <false, false>
	               |
	         uninitialized
*/	
	private:

	bool initialized;
	bool rankDep;
	bool nprocsDep;
	/* // Variables that are equal to the process rank
	varIDSet rankVars;
	// Variables that are equal to the number of processes
	varIDSet nprocsVars;*/
	
	public:
	
	MPIRankNProcsDepLattice()
	{
		initialized = false;
		rankDep     = false;
		nprocsDep   = false;
	}
	
	MPIRankNProcsDepLattice(const MPIRankNProcsDepLattice& that)
	{
		this->initialized = that.initialized;
		this->rankDep     = that.rankDep;
		this->nprocsDep   = that.nprocsDep;
	}
	
	// initializes this Lattice to its default state, if it is not already initialized
	void initialize()
	{
		if(!initialized)
		{
			initialized = true;
			rankDep     = false;
			nprocsDep   = false;
		}
		//printf("MPIRankNProcsDepLattice::initialize() : %s\n", str().c_str());
	}
	
	// returns a copy of this lattice
	Lattice* copy() const;
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// overwrites the state of this Lattice with that of that Lattice
	// returns true if this causes this lattice to change and false otherwise
	//bool copyMod(Lattice* that_arg);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that);
	
	bool operator==(Lattice* that);
	
	// returns the current state of this object
	bool getRankDep() const;
	bool getNprocsDep() const;
	
	// set the current state of this object, returning true if it causes 
	// the object to change and false otherwise
	bool setRankDep(bool rankDep);
	bool setNprocsDep(bool nprocsDep);
	
	// Sets the state of this lattice to bottom (false, false)
	// returns true if this causes the lattice's state to change, false otherwise
	bool setToBottom();
	
	// Sets the state of this lattice to bottom (true, true)
	// returns true if this causes the lattice's state to change, false otherwise
	bool setToTop();
		
	string str(string indent="");
};

class MPIRankDepAnalysis : public IntraFWDataflow
{	
	public:
	MPIRankDepAnalysis(): IntraFWDataflow()
	{	}
	
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	//vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
			
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
};

void runMPIRankDepAnalysis();

// Returns whether the given variable at the given DataflowNode depends on the process' rank
bool isMPIRankVarDep(const Function& func, const DataflowNode& n, varID var);

// Returns whether the given variable at the given DataflowNode depends on the number of processes
bool isMPINprocsVarDep(const Function& func, const DataflowNode& n, varID var);

// Sets rankDep and nprocsDep to true if some variable in the expression depends on the process' rank or 
// the number of processes, respectively. False otherwise.
bool isMPIDep(const Function& func, const DataflowNode& n, bool& rankDep, bool& nprocsDep);

// Returns whether some variable at the given DataflowNode depends on the process' rank
bool isMPIRankDep(const Function& func, const DataflowNode& n);

// Returns whether some variable at the given DataflowNode depends on the number of processes
bool isMPINprocsDep(const Function& func, const DataflowNode& n);

#endif
