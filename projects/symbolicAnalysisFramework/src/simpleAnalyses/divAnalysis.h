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
#include "liveDeadVarAnalysis.h"
#include "printAnalysisStates.h"
#include "VariableStateTransfer.h"

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
	
	DivLattice(long value) {
		this->value = value;
		div = -1;
		rem = -1;
		level = valKnown;
	}
	
	DivLattice(long div, long rem) {
		value = 0;
		this->div = div;
		this->rem = rem;
		level = divKnown;
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
		if(level==uninitialized) {
			value=0;
			div=-1;
			rem=-1;
			level=bottom;
		}
		//cout << "DivLattice::initialize() new="<<str("")<<"\n";
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
	
	// Takes two lattices at level divKnown. If the two objects have div, rem pairs that make it
	// possible to add or subtract them them and produce div/rem information where div>1, 
	// returns true and sets div and rem to correspond to the sum of these values.
	// Otherwise, returns false.
	// plus - true if the caller want to see one+two and false if one-two
	static bool matchDivAddSubt(DivLattice* one, DivLattice* two, long& div, long& rem, bool plus);
	
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

class DivAnalysisTransfer : public VariableStateTransfer<DivLattice>
{
  template <class T>
  void visitIntegerValue(T *sgn);
  void transferIncrement(SgUnaryOp *sgn);
  void transferCompoundAdd(SgBinaryOp *sgn);

  typedef void (DivAnalysisTransfer::*TransferOp)(DivLattice *, DivLattice *, DivLattice *);
  template <typename T>
  void transferArith(SgBinaryOp *sgn, T transferOp);
  void transferArith(SgBinaryOp *sgn, TransferOp transferOp);
  void transferAdditive(DivLattice *arg1Lat, DivLattice *arg2Lat, DivLattice *resLat, bool isAddition);
  void transferMultiplicative(DivLattice *arg1Lat, DivLattice *arg2Lat, DivLattice *resLat);
  void transferDivision(DivLattice *arg1Lat, DivLattice *arg2Lat, DivLattice *resLat);
  void transferMod(DivLattice *arg1Lat, DivLattice *arg2Lat, DivLattice *resLat);

public:
  //  void visit(SgNode *);
  void visit(SgLongLongIntVal *sgn);
  void visit(SgLongIntVal *sgn);
  void visit(SgIntVal *sgn);
  void visit(SgShortVal *sgn);
  void visit(SgUnsignedLongLongIntVal *sgn);
  void visit(SgUnsignedLongVal *sgn);
  void visit(SgUnsignedIntVal *sgn);
  void visit(SgUnsignedShortVal *sgn);
  void visit(SgValueExp *sgn);
  void visit(SgPlusAssignOp *sgn);
  void visit(SgMinusAssignOp *sgn);
  void visit(SgMultAssignOp *sgn);
  void visit(SgDivAssignOp *sgn);
  void visit(SgModAssignOp *sgn);
  void visit(SgAddOp *sgn);
  void visit(SgSubtractOp *sgn);
  void visit(SgMultiplyOp *sgn);
  void visit(SgDivideOp *sgn);
  void visit(SgModOp *sgn);
  void visit(SgPlusPlusOp *sgn);
  void visit(SgMinusMinusOp *sgn);
  void visit(SgUnaryAddOp *sgn);
  void visit(SgMinusOp *sgn);
  bool finish() { return modified; }

  DivAnalysisTransfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
};

class DivAnalysis : public IntraFWDataflow
{
	protected:
	static map<varID, Lattice*> constVars;
	static bool constVars_init;
	
	// The LiveDeadVarsAnalysis that identifies the live/dead state of all application variables.
	// Needed to create a FiniteVarsExprsProductLattice.
	LiveDeadVarsAnalysis* ldva; 
	
	public:
	DivAnalysis(LiveDeadVarsAnalysis* ldva)
	{
		this->ldva = ldva;
	}
	
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
	//map<varID, Lattice*>& genConstVarLattices() const;
		
  bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
  { assert(0); return false; }
  boost::shared_ptr<IntraDFTransferVisitor> getTransferVisitor(const Function& func, const DataflowNode& n,
                                                            NodeState& state, const vector<Lattice*>& dfInfo)
  { return boost::shared_ptr<IntraDFTransferVisitor>(new DivAnalysisTransfer(func, n, state, dfInfo)); }
};

// prints the Lattices set by the given DivAnalysis 
void printDivAnalysisStates(DivAnalysis* da, string indent="");

#endif
