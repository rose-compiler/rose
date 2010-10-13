#ifndef NODE_CONST_ANALYSIS_H
#define NODE_CONST_ANALYSIS_H

#include "common.h"
#include "genUID.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "printAnalysisStates.h"


extern int nodeConstAnalysisDebugLevel;

// For each live variable records whether this variable has not been assigned, has one value or multiple values.
// There is one nodeConstAnalysisDebugLevel object for every variable
class nodeConstLattice : public FiniteLattice
{
	private:
	// the unique ID of the value of the variable (if known)
	unsigned long valID;
		
	public:
	// The different levels of this lattice
	// this object is uninitialized
	static const int uninitialized=0; 
	
	private:
	// no information is known about the value of the variable
	// (we haven't seen an assignment)
	static const int bottom=1; 
	// the value of the variable is known
	// (we've seen exactly one assignment)
	static const int valKnown=2; 
	// this variable may have more than one value at the given CFGNode
	static const int top=3; 
	
	public:
	// public names for the different levels that correspond to the final outcome of the analysis
	static const int noAssign=bottom;
	static const int constVal=valKnown;
	static const int multVal=top;
	
	private:
	// this object's current level in the lattice: (uninitialized, bottom, valKnown, top)
	short level;
	
	public:
	
	nodeConstLattice()
	{
		valID=0;
		level=uninitialized;
	}
	
	nodeConstLattice(const nodeConstLattice& that)
	{
		this->valID = that.valID;
		this->level = that.level;
	}
	
	// initializes this Lattice to its default state, if it is not already initialized
	void initialize()
	{
		if(level == uninitialized)
		{
			valID=0;
			level=bottom;
		}
	}
	
	// returns a copy of this lattice
	Lattice* copy() const;
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that);
	
	// Computes the maximum of this node and that, which is just like meet
	// except that different values get max-ed, rather than push the result to top
	// returns true if this causes this to change and false otherwise
	bool maxUpdate(nodeConstLattice& that);
	
	// If this lattice is at level valKnown, increments the value by the given amount
	// returns true if this causes this to change and false otherwise
	bool increment(int val=1);
	
	// computes the meet of this and that and returns the result
	//Lattice* meet(Lattice* that) const;
	
	bool operator==(Lattice* that);
	
	/*private:
	// returns this object's level
	short getLevel() const;
	
	public:*/
	// returns whether the variable is constant at the current node
	short getValConst() const;
	
	// Sets the state of this lattice to bottom
	// returns true if this causes the lattice's state to change, false otherwise
	bool setToBottom();
	
	// Sets the state of this lattice to the given value.
	// returns true if this causes the lattice's state to change, false otherwise
	bool set(unsigned long valID);
	
	// Sets the state of this lattice to top
	// returns true if this causes the lattice's state to change, false otherwise
	bool setToTop();
		
	string str(string indent="");
};

class nodeConstAnalysis : public IntraFWDataflow
{
	protected:
	genUID uids;
	
	public:
	nodeConstAnalysis(): IntraFWDataflow()
	{	}
	
	/*// generates the initial variable-specific lattice state for a dataflow node
	Lattice* genInitVarState(const Function& func, const DataflowNode& n, const NodeState& state);	
	
	// generates the initial non-variable-specific lattice state for a dataflow node
	Lattice* genInitNonVarState(const Function& func, const DataflowNode& n, const NodeState& state);*/
	
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	//vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
		
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
};

// runs the nodeConstAnalysis on the project and returns the resulting nodeConstAnalysis object
nodeConstAnalysis* runNodeConstAnalysis();

// prints the Lattices set by the given nodeConstAnalysis 
void printNodeConstAnalysisStates(nodeConstAnalysis* da, string indent="");

#endif
