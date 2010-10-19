#ifndef DATAFLOW_H
#define DATAFLOW_H

#include "analysisCommon.h"
#include "nodeState.h"
#include "functionState.h"
#include "analysis.h"
#include "latticeFull.h"
#include <string.h>

/*************************
 *** Dataflow Analyses ***
 *************************/
class InterProceduralDataflow;

class IntraProceduralDataflow : virtual public IntraProceduralAnalysis
{
	public:
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	//virtual vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state)=0;
	virtual void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                          vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)=0;
	
	void setInterAnalysis(InterProceduralDataflow* interDataflowAnalysis)
	{ this->interAnalysis = (InterProceduralAnalysis*)interDataflowAnalysis; }
	
	void setInterAnalysis(IntraProceduralDataflow* intraDFAnalysis)
	{ this->interAnalysis = intraDFAnalysis->interAnalysis; }
	
	InterProceduralDataflow* getInterAnalysis() const
	{
		return (InterProceduralDataflow*)interAnalysis;
	}
};

class IntraUnitDataflow : virtual public IntraProceduralDataflow
{
	public:
		
	// the transfer function that is applied to every node
	// n - the dataflow node that is being processed
	// state - the NodeState object that describes the state of the node, as established by earlier 
	//         analysis passes
	// dfInfo - the Lattices that this transfer function operates on. The function takes these lattices
	//          as input and overwrites them with the result of the transfer.
	// Returns true if any of the input lattices changed as a result of the transfer function and
	//    false otherwise.
	virtual bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)=0;	
};

class InterProceduralDataflow : virtual public InterProceduralAnalysis
{
	public:
	InterProceduralDataflow(IntraProceduralDataflow* intraDataflowAnalysis);
	
	// the transfer function that is applied to SgFunctionCallExp nodes to perform the appropriate state transfers
	// fw - =true if this is a forward analysis and =false if this is a backward analysis
	// n - the dataflow node that is being processed
	// state - the NodeState object that describes the state of immediately before (if fw=true) or immediately after 
	//         (if fw=false) the SgFunctionCallExp node, as established by earlier analysis passes
	// dfInfo - the Lattices that this transfer function operates on. The function propagates them 
	//          to the calling function and overwrites them with the dataflow result of calling this function.
	// Returns true if any of the input lattices changed as a result of the transfer function and
	//    false otherwise.
	virtual bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo, bool fw)=0;	
};

class InitDataflowState : public UnstructuredPassIntraAnalysis
{
	//vector<Lattice*> initState;
	IntraProceduralDataflow* dfAnalysis;
	
	public:
	InitDataflowState(IntraProceduralDataflow* dfAnalysis/*, vector<Lattice*> &initState*/)
	{
		this->dfAnalysis = dfAnalysis;
		//this->initState = initState;
	}
	
	void visit(const Function& func, const DataflowNode& n, NodeState& state);
};

class IntraFWDataflow  : public virtual IntraUnitDataflow
{
	protected:
	//vector<Lattice*> initState;
	
	//map <DataflowNode, Lattice*> nodeState;
	//map<DataflowNode, list<Lattice*> > nodeState;
	
	public:
	/*IntraFWDataflow(vector<Lattice*> &initState)
	{
		this->initState = initState;
	}*/
	
	IntraFWDataflow()
	{}
	
	// runs the intra-procedural analysis on the given functionm returns true if 
	// the function's NodeState gets modified as a result and false otherwise
	// state - the function's NodeState
	bool runAnalysis(const Function& func, NodeState* state);
	
	private:
	// propagates the forward dataflow info from the current node's NodeState (curNodeState) to the next node's 
	// NodeState (nextNodeState)
	bool propagateFWStateToNextNode(
             const vector<Lattice*>& curNodeState, DataflowNode curDFNode, int nodeIndex,
             const vector<Lattice*>& nextNodeState, DataflowNode nextDFNode);
};

/*// Dataflow class that maintains a Lattice for every currently live variable
class IntraFWPerVariableDataflow  : public IntraFWDataflow
{
	private:
	bool includeScalars;
	bool includeArrays;
	
		
	public:
	IntraFWPerVariableDataflow(bool includeScalars, bool includeArrays);
	
	// returns the set of global variables(scalars and/or arrays)
	varIDSet& getGlobalVars();
	
	// returns the set of variables(scalars and/or arrays) declared in this function
	varIDSet& getLocalVars(Function func);
	
	// returns the set of variables(scalars and/or arrays) referenced in this function
	varIDSet& getRefVars(Function func);
	
	// generates the initial variable-specific lattice state for a dataflow node
	virtual Lattice* genInitVarState(const Function& func, const DataflowNode& n, const NodeState& state)=0;
	
	// generates the initial non-variable-specific lattice state for a dataflow node
	virtual Lattice* genInitNonVarState(const Function& func, const DataflowNode& n, const NodeState& state)=0;
	
	// Generates a map of special constant variables (such as zeroVar) and the lattices that correspond to them
	// These lattices are assumed to be constants: it is assumed that they are never modified and it is legal to 
	//    maintain only one copy of each lattice may for the duration of the analysis.
	virtual map<varID, Lattice*> genConstVarLattices() const=0;
	
	private:
	// maps variables to the index of their respective Lattice objects in a given function
	map<Function, map<varID, int> > varLatticeIndex;
	// map of lattices that correspond to constant variables
	map<varID, Lattice*> constVarLattices;
	// =true if constVarLattices has been initialized and =false otherwise
	bool constVarLattices_init;
	
	public:
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
	
	Lattice* getVarLattice(const Function& func, const varID& var, const vector<Lattice*>& dfInfo);
};*/

/******************************************************
 ***            printDataflowInfoPass               ***
 *** Prints out the dataflow information associated ***
 *** with a given analysis for every CFG node a     ***
 *** function.                                      ***
 ******************************************************/
class printDataflowInfoPass : public IntraFWDataflow
{
	Analysis* analysis;
	
	public:
	printDataflowInfoPass(Analysis *analysis)
	{
		this->analysis = analysis;
	}
	
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	//vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
	
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
};

/**********************************************************************
 ***                 UnstructuredPassInterDataflow                  ***
 *** The trivial inter-procedural dataflow where a intra-procedural ***
 *** dataflow analysis is executed once on every function in the    ***
 *** application, with no guarantees about how dataflow information ***
 *** is transmitted across function calls.                          ***
 **********************************************************************/
class UnstructuredPassInterDataflow : virtual public InterProceduralDataflow
{
	public:
	
	UnstructuredPassInterDataflow(IntraProceduralDataflow* intraDataflowAnalysis) 
	                     : InterProceduralDataflow(intraDataflowAnalysis), InterProceduralAnalysis((IntraProceduralAnalysis*)intraDataflowAnalysis)
	{}
	
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo, bool fw)
	{}
	
	void runAnalysis();
};

class ContextInsensitiveInterProceduralDataflow : virtual public InterProceduralDataflow, public TraverseCallGraphDataflow
{
	// list of functions that still remain to be processed
	list<Function> remaining;
	
	public:
	ContextInsensitiveInterProceduralDataflow(IntraProceduralDataflow* intraDataflowAnalysis, SgIncidenceDirectedGraph* graph) ;
	
	public:

	// the transfer function that is applied to SgFunctionCallExp nodes to perform the appropriate state transfers
	// fw - =true if this is a forward analysis and =false if this is a backward analysis
	// n - the dataflow node that is being processed
	// state - the NodeState object that describes the dataflow state immediately before (if fw=true) or immediately after 
	//         (if fw=false) the SgFunctionCallExp node, as established by earlier analysis passes
	// dfInfo - the Lattices that this transfer function operates on. The function propagates them 
	//          to the calling function and overwrites them with the dataflow result of calling this function.
	// Returns true if any of the input lattices changed as a result of the transfer function and
	//    false otherwise.	
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo, bool fw);
	
	// Uses TraverseCallGraphDataflow to traverse the call graph.
	void runAnalysis();
	
	// Runs the intra-procedural analysis every time TraverseCallGraphDataflow passes a function.
	void visit(const CGFunction* func);
};

#endif
