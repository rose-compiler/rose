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
#include "printAnalysisStates.h"

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>

extern int liveDeadAnalysisDebugLevel;

// Lattice that stores the variables that are live at a given DataflowNode
class LiveVarsLattice : public FiniteLattice
{
	public:
	std::set<varID> liveVars;
	
	public:
	LiveVarsLattice();
	LiveVarsLattice(const varID& var);
	LiveVarsLattice(const std::set<varID>& liveVars);
		
	// Initializes this Lattice to its default state, if it is not already initialized
	void initialize();
	
	// Returns a copy of this lattice
	Lattice* copy() const;
	
	// Overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// Called by analyses to create a copy of this lattice. However, if this lattice maintains any 
	//    information on a per-variable basis, these per-variable mappings must be converted from 
	//    the current set of variables to another set. This may be needed during function calls, 
	//    when dataflow information from the caller/callee needs to be transferred to the callee/calleer.
	// We do not force child classes to define their own versions of this function since not all
	//    Lattices have per-variable information.
	// varNameMap - maps all variable names that have changed, in each mapping pair, pair->first is the 
	//              old variable and pair->second is the new variable
	// func - the function that the copy Lattice will now be associated with
	void remapVars(const std::map<varID, varID>& varNameMap, const Function& newFunc);
	
	// Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
	// that contains data for a set of variables and incorporateVars must overwrite the state of just
	// those variables, while leaving its state for other variables alone.
	// We do not force child classes to define their own versions of this function since not all
	//    Lattices have per-variable information.
	void incorporateVars(Lattice* that_arg);
	
	// Returns a Lattice that describes the information known within this lattice
	// about the given expression. By default this could be the entire lattice or any portion of it.
	// For example, a lattice that maintains lattices for different known variables and expression will 
	// return a lattice for the given expression. Similarly, a lattice that keeps track of constraints
	// on values of variables and expressions will return the portion of the lattice that relates to
	// the given expression. 
	// It it legal for this function to return NULL if no information is available.
	// The function's caller is responsible for deallocating the returned object
	Lattice* project(SgExpression* expr);
	
	// The inverse of project(). The call is provided with an expression and a Lattice that describes
	// the dataflow state that relates to expression. This Lattice must be of the same type as the lattice
	// returned by project(). unProject() must incorporate this dataflow state into the overall state it holds.
	// Call must make an internal copy of the passed-in lattice and the caller is responsible for deallocating it.
	// Returns true if this causes this to change and false otherwise.
	bool unProject(SgExpression* expr, Lattice* exprState);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that_arg);
	
	bool operator==(Lattice* that);
	
	// Functions used to inform this lattice that a given variable is now in use (e.g. a variable has entered 
	//    scope or an expression is being analyzed) or is no longer in use (e.g. a variable has exited scope or
	//    an expression or variable is dead).
	// It is assumed that a newly-added variable has not been added before and that a variable that is being
	//    removed was previously added
	// Returns true if this causes the lattice to change and false otherwise.
	bool addVar(const varID& var);
	bool remVar(const varID& var);
	
	// Returns true if the given variable is recorded as live and false otherwise
	bool isLiveVar(varID var);
			
	// The string that represents this object
	// If indent!="", every line of this string must be prefixed by indent
	// The last character of the returned string should not be '\n', even if it is a multi-line string.
	std::string str(std::string indent="");
};

// Virtual class that allows users of the LiveDeadVarsAnalysis to mark certain variables as 
// being used inside a function call if the function's body is not available.
class funcSideEffectUses
{
public:
  // Returns the set of variables that are used in a call to the given function for which a body has not been provided.
  // The function is also provided with the DataflowNode where the function was called, as well as its state.
  virtual std::set<varID> usedVarsInFunc(const Function& func, const DataflowNode& n, NodeState& state)=0;
};

class LiveDeadVarsTransfer : public IntraDFTransferVisitor
{
  std::string indent;
  LiveVarsLattice* liveLat;

  bool modified;
  // Expressions that are assigned by the current operation
  std::set<SgExpression*> assignedExprs;
  // Variables that are assigned by the current operation
  std::set<varID> assignedVars;
  // Variables that are used/read by the current operation
  std::set<varID> usedVars;

  funcSideEffectUses *fseu;

  friend class LDVAExpressionTransfer;

  // Note that the variable corresponding to this expression is used
  void used(SgExpression *);

public:
  LiveDeadVarsTransfer(const Function &f, const DataflowNode &n, NodeState &s, const std::vector<Lattice*> &d, funcSideEffectUses *fseu_)
    : IntraDFTransferVisitor(f, n, s, d), indent("    "), liveLat(dynamic_cast<LiveVarsLattice*>(*(dfInfo.begin()))), modified(false), fseu(fseu_)
  {
	if(liveDeadAnalysisDebugLevel>=1) Dbg::dbg << indent << "liveLat="<<liveLat->str(indent + "    ")<<std::endl;
	// Make sure that all the lattice is initialized
	liveLat->initialize();
  }

  bool finish();

  void visit(SgExpression *);
  void visit(SgInitializedName *);
  void visit(SgReturnStmt *);
  void visit(SgExprStatement *);
  void visit(SgCaseOptionStmt *);
  void visit(SgIfStmt *);
  void visit(SgForStatement *);
  void visit(SgWhileStmt *);
  void visit(SgDoWhileStmt *);
};

/* Computes an over-approximation of the set of variables that are live at each DataflowNode. It may consider a given
   variable as live when in fact it is not. */
// !!! CURRENTLY THE ANALYSIS IS IMPRECISE BECAUSE:
// !!!    - IF THERE IS A VARIABLE USE WHERE THE IDENTITY OF THE VARIABLE IS COMPUTED THROUGH AN EXPRESSION, WE DO NOT 
// !!!         RESPOND BY CONSERVATIVELY MAKING ALL VARIABLES LIVE.
// !!!    - IT DOES NOT CONSIDER INTERNALS OF ARRAYS OR OTHER HEAP MEMORY
class LiveDeadVarsAnalysis : public IntraBWDataflow
{
	protected:
	funcSideEffectUses* fseu;
	
	public:
	LiveDeadVarsAnalysis(SgProject *project, funcSideEffectUses* fseu=NULL);
	
	// Generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                  std::vector<Lattice*>& initLattices, std::vector<NodeFact*>& initFacts);
	
  boost::shared_ptr<IntraDFTransferVisitor> getTransferVisitor(const Function& func, const DataflowNode& n,
                                                               NodeState& state, const std::vector<Lattice*>& dfInfo)
  { return boost::shared_ptr<IntraDFTransferVisitor>(new LiveDeadVarsTransfer(func, n, state, dfInfo, fseu)); }

  bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo) { assert(0); return false; }
};

// Initialize vars to hold all the variables and expressions that are live at DataflowNode n
void getAllLiveVarsAt(LiveDeadVarsAnalysis* ldva, const DataflowNode& n, const NodeState& state, std::set<varID>& vars, std::string indent="");

// Returns the set of variables and expressions that are live at DataflowNode n
std::set<varID> getAllLiveVarsAt(LiveDeadVarsAnalysis* ldva, const DataflowNode& n, const NodeState& state, std::string indent="");

class VarsExprsProductLattice: public virtual ProductLattice
{
	protected:
	// Sample lattice that will be initially associated with every variable (before the analysis)
	Lattice* perVarLattice;
		
	// Lattice that corresponds to allVar;
	Lattice* allVarLattice;
	
	// Map of lattices that correspond to constant variables
	std::map<varID, Lattice*> constVarLattices;
	
	// Maps variables in a given function to the index of their respective Lattice objects in 
	// the ProductLattice::lattice[] array
	std::map<varID, int> varLatticeIndex;
	
	// The analysis that identified the variables that are live at this Dataflow node
	LiveDeadVarsAnalysis* ldva;
	
	// Dataflow node that this lattice is associated with and its corresponding node state.
	DataflowNode n;
	const NodeState& state;
	
	protected:
	// Minimal constructor that initializes just the portions of the object required to make an 
	// initial blank VarsExprsProductLattice
	VarsExprsProductLattice(const DataflowNode& n, const NodeState& state);
	
	// Retrns a blank instance of a VarsExprsProductLattice that only has the fields n and state set
	virtual VarsExprsProductLattice* blankVEPL(const DataflowNode& n, const NodeState& state)=0;
	
	public:
	// creates a new VarsExprsProductLattice
	// perVarLattice - sample lattice that will be associated with every variable in scope at node n
	//     it should be assumed that the object pointed to by perVarLattice will be either
	//     used internally by this VarsExprsProductLatticeobject or deallocated
	// constVarLattices - map of additional variables and their associated lattices, that will be 
	//     incorporated into this VarsExprsProductLatticein addition to any other lattices for 
	//     currently live variables (these correspond to various useful constant variables like zeroVar)
	// allVarLattice - the lattice associated with allVar (the variable that represents all of memory)
	//     if allVarLattice==NULL, no support is provided for allVar
	// func - the current function
	// n - the dataflow node that this lattice will be associated with
	// state - the NodeState at this dataflow node
	VarsExprsProductLattice(Lattice* perVarLattice, 
	                        const std::map<varID, Lattice*>& constVarLattices, 
	                        Lattice* allVarLattice,
	                        LiveDeadVarsAnalysis* ldva, 
	                        const DataflowNode& n, const NodeState& state);
	                        
	// Create a copy of that. It is assumed that the types of all the lattices in  VarsExprsProductLattice that are
	// the same as in this.
	VarsExprsProductLattice(const VarsExprsProductLattice& that);
	
	~VarsExprsProductLattice();
	
	public:
	
	// Returns the Lattice mapped to the given variable of NULL if nothing is mapped to it
	Lattice* getVarLattice(const varID& var);
	
	// Returns the set of all variables mapped by this VarsExprsProductLattice
	std::set<varID> getAllVars();
	
	protected:
	
	// Returns the index of var among the variables associated with func
	// or -1 otherwise
	int getVarIndex(const varID& var);
	
	public:
	
	// Overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	// Set this to be a copy of that. It is assumed that the types of all the lattices in  VarsExprsProductLattice 
	// that are the same as in this.
	void copy(const VarsExprsProductLattice* that);
	
	// Called by analyses to create a copy of this lattice. However, if this lattice maintains any 
	//    information on a per-variable basis, these per-variable mappings must be converted from 
	//    the current set of variables to another set. This may be needed during function calls, 
	//    when dataflow information from the caller/callee needs to be transferred to the callee/calleer.
	// We do not force child classes to define their own versions of this function since not all
	//    Lattices have per-variable information.
	// varNameMap - maps all variable names that have changed, in each mapping pair, pair->first is the 
	//              old variable and pair->second is the new variable
	// func - the function that the copy Lattice will now be associated with
	/*Lattice**/void remapVars(const std::map<varID, varID>& varNameMap, const Function& newFunc);
	
	// Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
	// that contains data for a set of variables and incorporateVars must overwrite the state of just
	// those variables, while leaving its state for other variables alone.
	// We do not force child classes to define their own versions of this function since not all
	//    Lattices have per-variable information.
	void incorporateVars(Lattice* that);
	
	// Returns a Lattice that describes the information known within this lattice
	// about the given expression. By default this could be the entire lattice or any portion of it.
	// For example, a lattice that maintains lattices for different known variables and expression will 
	// return a lattice for the given expression. Similarly, a lattice that keeps track of constraints
	// on values of variables and expressions will return the portion of the lattice that relates to
	// the given expression. 
	// It it legal for this function to return NULL if no information is available.
	// The function's caller is responsible for deallocating the returned object
	Lattice* project(SgExpression* expr);
	
	// The inverse of project(). The call is provided with an expression and a Lattice that describes
	// the dataflow state that relates to expression. This Lattice must be of the same type as the lattice
	// returned by project(). unProject() must incorporate this dataflow state into the overall state it holds.
	// Call must make an internal copy of the passed-in lattice and the caller is responsible for deallocating it.
	// Returns true if this causes this to change and false otherwise.
	bool unProject(SgExpression* expr, Lattice* exprState);
	
	// Functions used to inform this lattice that a given variable is now in use (e.g. a variable has entered 
	//    scope or an expression is being analyzed) or is no longer in use (e.g. a variable has exited scope or
	//    an expression or variable is dead).
	// Returns true if this causes this Lattice to change and false otherwise.
	bool addVar(const varID& var);
	bool remVar(const varID& var);
	
	// Sets the lattice of the given var to be lat. 
	// If the variable is already mapped to some other Lattice, 
	//   If *(the current lattice) == *lat, the mapping is not changed
	//   If *(the current lattice) != *lat, the current lattice is deallocated and var is mapped to lat->copy()
	// Returns true if this causes this Lattice to change and false otherwise.
	bool addVar(const varID& var, Lattice* lat);
	
	// The string that represents this object
	// If indent!="", every line of this string must be prefixed by indent
	// The last character of the returned string should not be '\n', even if it is a multi-line string.
	std::string str(std::string indent="");
};

class FiniteVarsExprsProductLattice : public virtual VarsExprsProductLattice, public virtual FiniteProductLattice
{
	protected:
	// Minimal constructor that initializes just the portions of the object required to make an 
	// initial blank VarsExprsProductLattice
	FiniteVarsExprsProductLattice(const DataflowNode& n, const NodeState& state);
	
	// Retrns a blank instance of a VarsExprsProductLattice that only has the fields n and state set
	VarsExprsProductLattice* blankVEPL(const DataflowNode& n, const NodeState& state);
		
	public:
	// creates a new VarsExprsProductLattice
	// perVarLattice - sample lattice that will be associated with every variable in scope at node n
	//     it should be assumed that the object pointed to by perVarLattice will be either
	//     used internally by this VarsExprsProductLattice object or deallocated
	// constVarLattices - map of additional variables and their associated lattices, that will be 
	//     incorporated into this VarsExprsProductLattice in addition to any other lattices for 
	//     currently live variables (these correspond to various useful constant variables like zeroVar)
	// allVarLattice - the lattice associated with allVar (the variable that represents all of memory)
	//     if allVarLattice==NULL, no support is provided for allVar
	// func - the current function
	// n - the dataflow node that this lattice will be associated with
	// state - the NodeState at this dataflow node
	FiniteVarsExprsProductLattice(Lattice* perVarLattice, 
	                             const std::map<varID, Lattice*>& constVarLattices, 
	                             Lattice* allVarLattice,
	                             LiveDeadVarsAnalysis* ldva, 
	                             const DataflowNode& n, const NodeState& state);
	
	FiniteVarsExprsProductLattice(const FiniteVarsExprsProductLattice& that);
	
	// returns a copy of this lattice
	Lattice* copy() const;
};

class InfiniteVarsExprsProductLattice: public virtual VarsExprsProductLattice, public virtual InfiniteProductLattice
{
	protected:
	// Minimal constructor that initializes just the portions of the object required to make an 
	// initial blank VarsExprsProductLattice
	InfiniteVarsExprsProductLattice(const DataflowNode& n, const NodeState& state);
	
	// Retrns a blank instance of a VarsExprsProductLattice that only has the fields n and state set
	VarsExprsProductLattice* blankVEPL(const DataflowNode& n, const NodeState& state);
	
	public:
	// creates a new VarsExprsProductLattice
	// perVarLattice - sample lattice that will be associated with every variable in scope at node n
	//     it should be assumed that the object pointed to by perVarLattice will be either
	//     used internally by this VarsExprsProductLatticeobject or deallocated
	// constVarLattices - map of additional variables and their associated lattices, that will be 
	//     incorporated into this VarsExprsProductLatticein addition to any other lattices for 
	//     currently live variables (these correspond to various useful constant variables like zeroVar)
	// allVarLattice - the lattice associated with allVar (the variable that represents all of memory)
	//     if allVarLattice==NULL, no support is provided for allVar
	// func - the current function
	// n - the dataflow node that this lattice will be associated with
	// state - the NodeState at this dataflow node
	InfiniteVarsExprsProductLattice(Lattice* perVarLattice, 
	                                const std::map<varID, Lattice*>& constVarLattices, 
	                                Lattice* allVarLattice,
	                                LiveDeadVarsAnalysis* ldva, 
	                                const DataflowNode& n, const NodeState& state);
	
	InfiniteVarsExprsProductLattice(const FiniteVarsExprsProductLattice& that);
	
	// returns a copy of this lattice
	Lattice* copy() const;
};

// prints the Lattices set by the given LiveDeadVarsAnalysis 
void printLiveDeadVarsAnalysisStates(LiveDeadVarsAnalysis* da, std::string indent="");

#endif
