#ifndef DIV_ANALYSIS_H
#define DIV_ANALYSIS_H

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


extern int divAnalysisDebugLevel;

// Maintains value information about live variables. If a given variable may have more than one value,
//    this object maintains divisibility information about all the possible values (i.e. they're all divisible 
//    by x, with y as the remainder), with the divisior,remainder = (1,0) if nothing better can be found.
// There is one DivLattice object for every variable
class DivLattice : public FiniteLattice
{
	private:
	// the current value of the variable (if known)
	long value;
	// the <divisor, remainder> pair where for all possible values v of the variable
	// there exists a multiplier m s.t v = div * m + rem
	long div;
	long rem;
		
	public:
	// The different levels of this lattice
	// this object is uninitialized
	static const int uninitialized=0; 
	// no information is known about the value of the variable
	static const int bottom=1; 
	// the value of the variable is known
	static const int valKnown=2; 
	// value is unknown but the divisibility (div and rem) of this variable is known 
	static const int divKnown=3; 
	// this variable holds more values than can be represented using a single value and divisibility
	static const int top=4; 
	
	private:
	// this object's current level in the lattice: (uninitialized, bottom, valKnown, divKnown, top)
	short level;
	
	public:
	
	DivLattice()
	{
		value=0;
		div=-1;
		rem=-1;
		level=uninitialized;
	}
	
	DivLattice(const DivLattice& that)
	{
		this->value = that.value;
		this->div   = that.div;
		this->rem   = that.rem;
		this->level = that.level;
	}
	
	// initializes this Lattice to its default state
	void initialize()
	{
		value=0;
		div=-1;
		rem=-1;
		level=bottom;
	}
	
	// returns a copy of this lattice
	Lattice* copy() const;
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// returns true if the given value matches the given div, rem combo and false otherwise
	static bool matchDiv(long value, long div, long rem);
	
	// Takes two lattices at level divKnown. If the two objects have matching div, rem pairs, returns
	// true and sets div and rem to those mathching values. Otherwise, returns false;
	static bool matchDiv(DivLattice* one, DivLattice* two, long& div, long& rem);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that);
	
	// computes the meet of this and that and returns the result
	//Lattice* meet(Lattice* that) const;
	
	bool operator==(Lattice* that);
	
	/*// widens this from that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool widenUpdate(InfiniteLattice* that);*/
	
	// returns the current state of this object
	long getValue() const;
	long getDiv() const;
	long getRem() const;
	short getLevel() const;
	
	// Sets the state of this lattice to bottom
	// returns true if this causes the lattice's state to change, false otherwise
	bool setBot();
	
	// Sets the state of this lattice to the given value.
	// returns true if this causes the lattice's state to change, false otherwise
	bool set(long value);
	
	// Sets the state of this lattice to the given div/rem state.
	// returns true if this causes the lattice's state to change, false otherwise
	bool set(long div, long rem);
	
	// Sets the state of this lattice to top
	// returns true if this causes the lattice's state to change, false otherwise
	bool setTop();
	
	// Increments the state of this object by increment
	// returns true if this causes the lattice's state to change, false otherwise
	bool incr(long increment);
	
	// Multiplies the state of this object by value
	// returns true if this causes the lattice's state to change, false otherwise
	bool mult(long multiplier);
		
	string str(string indent="");
};

class DivAnalysis : public IntraFWDataflow
{
	protected:
	static map<varID, Lattice*> constVars;
	static bool constVars_init;
	
	public:
	DivAnalysis(): IntraFWDataflow()
	{	}
	
	/*// generates the initial variable-specific lattice state for a dataflow node
	Lattice* genInitVarState(const Function& func, const DataflowNode& n, const NodeState& state);	
	
	// generates the initial non-variable-specific lattice state for a dataflow node
	Lattice* genInitNonVarState(const Function& func, const DataflowNode& n, const NodeState& state);*/
	
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	//vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
	
	// Returns a map of special constant variables (such as zeroVar) and the lattices that correspond to them
	// These lattices are assumed to be constants: it is assumed that they are never modified and it is legal to 
	//    maintain only one copy of each lattice may for the duration of the analysis.
	map<varID, Lattice*>& genConstVarLattices() const;
		
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
};

// prints the Lattices set by the given DivAnalysis 
void printDivAnalysisStates(DivAnalysis* da, string indent="");

#endif
