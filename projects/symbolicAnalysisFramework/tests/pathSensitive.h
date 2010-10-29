#ifndef PATH_SENSITIVE_H
#define PATH_SENSITIVE_H

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
#include "divAnalysis.h"
#include "sgnAnalysis.h"
#include "affineInequality.h"
#include "placeUIDs.h"

#include "ConstrGraph.h"

class IfMeetLat : public virtual FiniteLattice
{
	friend class IfMeetDetector;
	bool initialized;
	// Binary information about the given CFG node's location inside its surrounding
	// if statements, with with 0 corresponding to the left branch and 1 corresponding 
	// to the right branch. The first entry in ifHist corresponds to the outer-most if 
	// statement and the last corresponds to the inner-most. Thus, 0, 1, 1 corresponds
	// to the * inside the following:
	// if() { if() {} else { if() {} else { * } } }
	vector<bool> ifHist;
	bool isIfMeet;
	// Set by the transfer function of IfMeetDetector to identify each CFG node n that is the
	// first node inside a branch of an if statement, the branch that they're on:
	//    left: ifLeft == n
	//    right: ifRight == n
	DataflowNode ifLeft;
	DataflowNode ifRight;
	
	// The dataflow node of the immediately surrounding if statement
	DataflowNode parentNode;
	
	public:
	IfMeetLat(DataflowNode parentNode);
	
	IfMeetLat(const IfMeetLat &that);
	
	// initializes this Lattice to its default state
	void initialize();

	// returns a copy of this lattice
	Lattice* copy() const;
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that);
	
	bool operator==(Lattice* that);
	
	// The string that represents this object
	// If indent!="", every line of this string must be prefixed by indent
	// The last character of the returned string should not be '\n', even if it is a multi-line string.
	string str(string indent="");
	
	bool getIsIfMeet();
};

class IfMeetDetector  : public virtual IntraFWDataflow
{
	protected:
	vector<Lattice*> initState;
	
	public:
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	//vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                     vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
			
	// the transfer function that is applied to every node
	// n - the dataflow node that is being processed
	// state - the NodeState object that describes the state of the node, as established by earlier 
	//         analysis passes
	// dfInfo - the Lattices that this transfer function operates on. The function takes these lattices
	//          as input and overwrites them with the result of the transfer.
	// Returns true if any of the input lattices changed as a result of the transfer function and
	//    false otherwise.
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
};

// prints the Lattices set by the given DivAnalysis 
void printIfMeetDetectorStates(IfMeetDetector* ifmd, string indent);

// Runs the IfMeetDetector analysis pass
void runIfMeetDetector(bool printStates=false);

// returns true if the given dataflow node is a meet point for an if statement and false otherwise
bool isIfMeetNode(const DataflowNode& n);


/************************
 * scalarFWDataflowPart *
 ************************/
class scalarFWDataflowPartitionedAnalysis;

class scalarFWDataflowPart : public virtual IntraPartitionFWDataflow
{
	protected:
	static map<varID, Lattice*> constVars;
	DivAnalysis* divAnalysis;
	SgnAnalysis* sgnAnalysis;
	
	public:
	scalarFWDataflowPart(DivAnalysis* divAnalysis, SgnAnalysis* sgnAnalysis, scalarFWDataflowPartitionedAnalysis* parent): 
				IntraPartitionDataflow  ((PartitionedAnalysis*)parent),
				IntraPartitionFWDataflow((PartitionedAnalysis*)parent) 
	{
		this->divAnalysis = divAnalysis;
		this->sgnAnalysis = sgnAnalysis;
		rwAccessLabeler::addRWAnnotations(cfgUtils::getProject());
	}
	
	scalarFWDataflowPart(const scalarFWDataflowPart& that): 
			IntraPartitionDataflow  ((const IntraPartitionDataflow&) that),
			IntraPartitionFWDataflow((const IntraPartitionFWDataflow&) that) 
	{
		this->divAnalysis = that.divAnalysis;
		this->sgnAnalysis = that.sgnAnalysis;
		this->constVars   = constVars;
		rwAccessLabeler::addRWAnnotations(cfgUtils::getProject());
	}
	
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	//vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                     vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
	
	// Returns a map of special constant variables (such as zeroVar) and the lattices that correspond to them
	// These lattices are assumed to be constants: it is assumed that they are never modified and it is legal to 
	//    maintain only one copy of each lattice may for the duration of the analysis.
	map<varID, Lattice*>& genConstVarLattices() const;
	
	// incorporates the current node's inequality information from conditionals (ifs, fors, etc.) into the current node's 
	// constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	bool incorporateConditionalsInfo(const Function& func, const DataflowNode& n,
	                                 NodeState& state, const vector<Lattice*>& dfInfo);
	
	// incorporates the current node's divisibility information into the current node's constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	bool incorporateDivInfo(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
	
	// For any variable for which we have divisibility info, remove its constraints to other variables (other than its
	// divisibility variable)
	bool removeConstrDivVars(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);

	// Creates a new instance of the derived object that is a copy of the original instance.
	// This instance will be used to instantiate a new partition of the analysis.
	IntraPartitionDataflow* copy();
	
	// the transfer function that is applied to every node
	// n - the dataflow node that is being processed
	// state - the NodeState object that describes the state of the node, as established by earlier 
	//         analysis passes
	// dfInfo - the Lattices that this transfer function operates on. The function takes these lattices
	//          as input and overwrites them with the result of the transfer.
	// splitAnalysis - set by callee to 
	//    - noSplit if the analysis does not want a split
	//    - splitNew if the analysis wants to perform a split and place the newly-generated partitions into
	//      a fresh split set that is nested inside this partition's current split
	//    - splitParent if the analysis wants to perform a split and place the newly-generated partitions 
	//      into this partition's current split (i.e. on the same split level as the current partition)
	// splitConditions - if splitAnalysis==splitNew or ==splitParent, the analysis sets this vector to the conditions for all the 
	//                   descendant CFG nodes in the split
	// joinNode - set to true if progress along the given dataflow node needs to be blocked until the next join point.
	//            If all paths of dataflow progress are blocked in this analysis, this is the same as the analysis 
	//            requesting to be joined.
	// Returns true if any of the input lattices changed as a result of the transfer function and
	//    false otherwise.
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo, 
	              IntraPartitionFWDataflow::splitType& splitAnalysis, vector</*LogicalCond**/printable*>& splitConditions, bool& joinNode);
	                      
	// Called when a partition is created to allow a specific analysis to initialize
	// its dataflow information from the partition condition
	void initDFfromPartCond(const Function& func, const DataflowNode& n, NodeState& state, 
	                        const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
	                        /*LogicalCond*/printable* partitionCond);
};

/***************************************
 * scalarFWDataflowPartitionedAnalysis *
 ***************************************/
class scalarFWDataflowPartitionedAnalysis : public PartitionedAnalysis
{
	public:
	scalarFWDataflowPartitionedAnalysis(DivAnalysis* divAnalysis, 
	                                    SgnAnalysis* sgnAnalysis): 
			PartitionedAnalysis(new scalarFWDataflowPart(divAnalysis, sgnAnalysis, this))
	{}

	// Called by the base PartitionedAnalysis class when all partitions in a given split have decided
	//    to join to decide whether they should be joined or all released. It may also do some
	//    processing of their state prior to the release or join.
	// Returns the set of partitions that will remain in joined status after this join. If all partitions in the split
	//    set are on this list, they are all joined(all but one will be deleted). Any remaining partitions will be released.
	set<IntraPartitionDataflow*> preJoin(partSplit* s, const Function& func, NodeState* fState,
	             const map<IntraPartitionDataflow*, IntraPartitionDataflowCheckpoint*>& parts2chkpts)
	{
		// Set all the partitions to be joined
		set<IntraPartitionDataflow*> joined;
		for(map<IntraPartitionDataflow*, IntraPartitionDataflowCheckpoint*>::const_iterator p=parts2chkpts.begin();
		    p!=parts2chkpts.end(); p++)
			joined.insert(p->first);
		return joined;
	}
	
	// Called by the base PartitionedAnalysis class when all partitions in a given split have 
	//    finished their respective executions.
	virtual void postFinish(partSplit* s, 
	                        const map<IntraPartitionDataflow*, IntraPartitionDataflowCheckpoint*>& parts2chkpts)
	{}
};

#endif
