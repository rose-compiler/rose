#ifndef SGN_ANALYSIS_H
#define SGN_ANALYSIS_H

#include "common.h"
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
class SgnLattice : public FiniteLattice
{
	public:
		
	// The different levels of this lattice
	typedef enum{
		// this object is uninitialized
		uninitialized, 
		// no information is known about the value of the variable
		bottom, 
		// this variable is = 0
		eqZero, 
		// the sign of the variable is known
		sgnKnown, 
		// this variable can be either positive or negative
		top} sgnLevels;
		
	// The different states of this lattice (in level sgnKnown)
	typedef enum{
		// This variable's state is unknown
		unknown, 
		// This variable is positive or =zero
		posZero, 
		// This variable is negative or =zero
		negZero} sgnStates;
	
	private:
	
	sgnStates sgnState;
	
	private:
	// this object's current level in the lattice: (uninitialized, bottom, sgnKnown, top)
	sgnLevels level;
	
	public:
	
	SgnLattice()
	{
		sgnState = unknown;
		level=uninitialized;
	}
	
	SgnLattice(const SgnLattice& that)
	{
		this->sgnState = that.sgnState;
		this->level = that.level;
	}
	
	// initializes this Lattice to its default state
	void initialize()
	{
		sgnState=unknown;
		level=bottom;
	}
	
	// returns a copy of this lattice
	Lattice* copy() const;
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// overwrites the state of this Lattice with that of that Lattice
	// returns true if this causes this lattice to change and false otherwise
	bool copyMod(Lattice* that_arg);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that);
	
	bool operator==(Lattice* that);
	
	// returns the current state of this object
	sgnStates getSgnState() const;
	sgnLevels getLevel() const;
	
	// Sets the state of this lattice to bottom
	// returns true if this causes the lattice's state to change, false otherwise
	bool setBot();
	
	// Sets the state of this lattice to eqZero.
	// returns true if this causes the lattice's state to change, false otherwise
	bool setEqZero();
	
	// Sets the state of this lattice to sgnKnown, with the given sign.
	// returns true if this causes the lattice's state to change, false otherwise
	bool setSgnKnown(sgnStates sgnState);
	
	// Sets the state of this lattice to sgnKnown, with the sign of the given value.
	// returns true if this causes the lattice's state to change, false otherwise
	bool set(int val);
	
	// Sets the state of this lattice to top
	// returns true if this causes the lattice's state to change, false otherwise
	bool setTop();
	
	// Increments the state of this object by increment
	// returns true if this causes the lattice's state to change, false otherwise
	bool plus(long increment);
	
	// Increments the state of this object by the contents of that
	// returns true if this causes the lattice's state to change, false otherwise
	bool plus(const SgnLattice& that);
	
	// Multiplies the state of this object by value
	// returns true if this causes the lattice's state to change, false otherwise
	bool mult(long multiplier);
	
	// Multiplies the state of this object by the contents of that
	// returns true if this causes the lattice's state to change, false otherwise
	bool mult(const SgnLattice& that);
		
	string str(string indent="");
};

class SgnAnalysis : public IntraFWDataflow
{
	protected:
	static map<varID, Lattice*> constVars;
	static bool constVars_init;
	
	public:
	SgnAnalysis(): IntraFWDataflow()
	{	}
	
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	//vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
		
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
};

// prints the Lattices set by the given SgnAnalysis 
void printSgnAnalysisStates(SgnAnalysis* sa, string indent="");


#endif
