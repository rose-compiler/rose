#ifndef COMM_TOPO_ANALYSIS_H
#define COMM_TOPO_ANALYSIS_H

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
#include "rankDepAnalysis.h"
#include "ranknpDepIfMeet.h"
#include "mesgExpr.h"
#include "procSet.h"

#include "ConstrGraph.h"
#include "pCFG.h"

extern int MPIAnalysisDebugLevel;

/******************************
 * pCFG_contProcMatchAnalysis *
 ******************************/

class pCFG_contProcMatchAnalysis : public virtual pCFG_FWDataflow
{
	protected:
	DivAnalysis* divAnalysis;
	SgnAnalysis* sgnAnalysis;
	public:
	static varID rankVar;
	static varID nprocsVar;
	
	// Set of processes that corresponds to this partition. The actual set refers to nothing in particular on its own.
	// However, for each CFG node we'll have a NodeFact that is a copy of this rankSet, with the same lower and upper
	// bound variables but referring to that CFG node's constraint graph. This ensures that all constraint graphs
	// in this partition and any partition copied from this partition (in other words, all partitions in a given
	// partitionedAnalysis), will use the same lower and upper bound variables to define their respective sets
	// of processes, making it easy to relate the process sets of different DataflowNodes in the same partition
	// or different partitions.
	static contRangeProcSet rankSet;
	
	public:
	pCFG_contProcMatchAnalysis(DivAnalysis* divAnalysis, SgnAnalysis* sgnAnalysis)
	{
		this->divAnalysis = divAnalysis;
		this->sgnAnalysis = sgnAnalysis;
		rwAccessLabeler::addRWAnnotations(cfgUtils::getProject());
		// Make sure that rankSet is non-empty
		//rankSet.makeNonEmpty();
	}
	
	pCFG_contProcMatchAnalysis(const pCFG_contProcMatchAnalysis& that): 
			pCFG_FWDataflow((const pCFG_FWDataflow&) that)
	{
		this->divAnalysis = that.divAnalysis;
		this->sgnAnalysis = that.sgnAnalysis;
		rwAccessLabeler::addRWAnnotations(cfgUtils::getProject());
		// Make sure that rankSet is non-empty
		//rankSet.makeNonEmpty();
	}
	
	// Generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	void genInitState(const Function& func, const pCFGNode& n, const NodeState& state,
	                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
	
	// Copies the dataflow information from the srcPSet to the tgtPSet and updates the copy with the
	// partitionCond (presumably using initPSetDFfromPartCond). Adds the new info directly to lattices and facts.
	// It is assumed that pCFGNode n contains both srcPSet and tgtPSet.
	// If omitRankSet==true, does not copy the constraints on srcPSet's bounds variables but
	// instead just adds them with no non-trivial constraints.
	void copyPSetState(const Function& func, const pCFGNode& n, 
                      int srcPSet, int tgtPSet, NodeState& state,
                      vector<Lattice*>& lattices, vector<NodeFact*>& facts, 
                      ConstrGraph* partitionCond, bool omitRankSet);
	
	// Removes all known bounds on pSet's process set in dfInfo and replaces them with the default
	// constraints on process set bounds.
	void resetPSet(int pSet, vector<Lattice*>& dfInfo);
	
	// Helper method for resetPSet that makes it easier to call it from inside pCFG_contProcMatchAnalysis.
	void resetPSet(int pSet, ConstrGraph* cg, 
	               const varID& /*nprocsVarAllPSets*/nprocsVarPSet, const varID& zeroVarAllPSets);
	
	// Resets the state of rankVar inside the given constraint graph and re-establishes its basic 
	// constraints: 0<=rankVar<nprocsVar
	//static void resetRank(const pCFGNode& n, ConstrGraph* cg);
	
	// Annotate the given zeroVar and nprocsVar with the annotations that identify them as being available
	// to all process sets within the given pCFG node. Specifically, these variables are annotated with
// all the annotations of all the process sets, plus another annotation that identifies them as common.
	static void annotateCommonVars(/*pCFGNode n, */const varID& zeroVar, varID& zeroVarAnn, 
                                  const varID& nprocsVar, varID& nprocsVarAnn);
   
   // Returns the name of the variable annotation to be used when talking about process set pSet
   static string getVarAnn(int pSet);
   
	// Asserts within this constraint graph the standard invariants on process sets:
	// [0<= lb <= ub < nprocsVar] and [lb <= rankVar <= ub]
	// Returns true if this causes the constraint graph to change, false otherwise
	static bool assertProcSetInvariants(ConstrGraph* cg, const contRangeProcSet& rankSetPSet,
	                                    const varID& zeroVarAllPSets, const varID& rankVarPSet,
	                                    const varID& nprocsVarPSet);
   
   // Update varIneq's variables with the annotations that connect them to pSet
	void connectVAItoPSet(int pSet, varAffineInequality& varIneq);
	
	// Inserts the given var + c value into the updates map. This map contains expressions var + c to which we'll
	// assign the process set upper or lower bound (upper if ub=true, lower if ub=false).
	static void addToBoundUpdatesMap(ConstrGraph* cg, map<varID, int>& updates, const varID& var, int c, bool ub);
	
	// Perform the given updates to the given node set bound variable. The updates are expressed
	// as a list of <x, c> pairs (a map) and the updates to cg will be:
	//    - erase boundVar's constraints in cg
	//    - for each <x, c> in updates, add the boundVar = x + c constraint to cg
	void performBoundUpdates(ConstrGraph* cg, map<varID, int>& updates, const varID& boundVar);
	
/*	// Inserts the given <varA, varB, ineq> tuple into the given map
	static void addVarVarIneqMap(map<varID, map<varID, affineInequality> >& vvi, 
	                            const varID& varA, const varID& varB, const affineInequality& ineq);*/
	
	// incorporates the current node's inequality information from conditionals (ifs, fors, etc.) into the current node's 
	// constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	bool incorporateConditionalsInfo(const pCFGNode& n, int pSet, const Function& func, const DataflowNode& dfNode,
	                                 NodeState& state, const vector<Lattice*>& dfInfo);
	
	// incorporates the current node's divisibility information into the current node's constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	bool incorporateDivInfo(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
	
	// For any variable for which we have divisibility info, remove its constraints to other variables (other than its
	// divisibility variable)
	bool removeConstrDivVars(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);

	// Creates a new instance of the derived object that is a copy of the original instance.
	// This instance will be used to instantiate a new partition of the analysis.
	//pCFG_contProcMatchAnalysis* copy();
	
	// The transfer function that is applied to every node
	// n - the pCFG node that is being processed
	// pSet - the process set that is currently transitioning
	// func - the function that is currently being analyzed
	// state - the NodeState object that describes the state of the node, as established by earlier 
	//         analysis passes
	// dfInfo - the Lattices that this transfer function operates on. The function takes these lattices
	//          as input and overwrites them with the result of the transfer.
	// deadPSet - may be set by the call to true to indicate that the dataflow state at this node constains an 
	//            inconsistency that makes this an impossible execution state
	// splitPSet - set by the call to indicate if the current process set must be broken up into two process sets,
	//             each of which takes a different path as a result of a conditional or a loop.
	// splitPSetNodes - if splitPNode, splitNodes is filled with the nodes along which each new process set must advance
	// splitPNode - set by the call to indicate if the current process set must take two different paths as a result
	//             of a conditional or a loop, causing the analysis to advance along two different pCFGNodes
	// splitConditions - if splitPNode==true or splitPSet==true, splitConditions is filled with the information 
	//             about the state along both sides of the split of pCFGNode or pSet split
	// blockPSet - set to true by the call if progress along the given dataflow node needs to be blocked 
	//             until the next send-receive matching point. If all process sets become blocked, we 
	//             perform send-receive matching.
	// Returns true if any of the input lattices changed as a result of the transfer function and
	//    false otherwise.
	 bool transfer(const pCFGNode& n, int pSet, const Function& func, 
	               NodeState& state, const vector<Lattice*>&  dfInfo, 
	               bool& deadPSet, bool& splitPSet, vector<DataflowNode>& splitPSetNodes,
                 bool& splitPNode, vector<ConstrGraph*>& splitConditions, bool& blockPSet);
	
	// Iterates over the outgoing edges of a given node and fills splitConditions with the actual split conditions
	// of these edges. It is specifically designed to look for how each edge condition relates to tgtVar
	// and adds those conditions to splitConditions as well
	void fillEdgeSplits(const Function& func, const pCFGNode& n, int splitPSet, 
	                    const DataflowNode& dfNode, NodeState& state, ConstrGraph* cg, 
	                    vector<ConstrGraph*>& splitConditions, const varID& tgtVar, const varID& nprocsVarPSet);
	
	// If possible according to the inequalities in cg, connect the given inequality to an 
	// inequality about tgtVar and assert that new inequality in newCG.
	// Returns true if this causes cg to change, false otherwise.
	bool connectIneqToVar(ConstrGraph* cg, ConstrGraph* newCG, const varAffineInequality& ineq, 
	                      const varID& tgtVar, DataflowNode n);
	
	// Called when a partition is created to allow a specific analysis to initialize
	// its dataflow information from the partition condition
	/*void initDFfromPartCond(const Function& func, const pCFGNode& n, int pSet, NodeState& state, 
	                        const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
	                        ConstrGraph* partitionCond);*/
	
	// Called when a process set or pCFGNode is partitioned to allow the specific analysis to update the
	// dataflow state for that process set with the set's specific condition.
	// Returns true if this causes the dataflow state to change and false otherwise
	bool initPSetDFfromPartCond(const Function& func, const pCFGNode& n, int pSet, 
	                            const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
	                            ConstrGraph* partitionCond);
	
	// Version of initPSetDFfromPartCond that doesn't perform a transitive closure at the end
	bool initPSetDFfromPartCond_ex(const Function& func, const pCFGNode& n, int pSet, 
		                            const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
	                               ConstrGraph* partitionCond);
	// Merge the dataflow information of two process sets. The space of process set IDs will be 
	// compressed to remove the holes left by the removal.
	// pSetMigrations (initially assumed empty) is set to indicate which process sets have moved
	//    to new ids, with the key representing the process set's original id and the value entry
	//    representing the new id.
	void mergePCFGStates(const list<int>& pSetsToMerge, const pCFGNode& n, const Function& func, 
	                     const vector<Lattice*>& dfInfo, NodeState& state, map<int, int>& pSetMigrations);
	
	// Infers the best possible constraints on the upper and lower bounds of the process set from 
	// the constraints on rankVar
	// Returns true if this causes the dataflow state to change and false otherwise
	bool inferBoundConstraints(const Function& func, const pCFGNode& n, int pSet, ConstrGraph* cg);
	
	// Transfer constraints from other process set's nprocsVar to this process set's nprocsVar
	// Returns true if this causes the dataflow state to change and false otherwise
	bool forceNProcsUnity(const Function& func, const pCFGNode& n, int pSet, ConstrGraph* cg);
	
	// Performs send-receive matching on a fully-blocked analysis partition. 
	// If some process sets need to be split, returns the set of checkpoints that corresponds to this pCFG node's descendants.
	// Otherwise, returns an empty set.
	// Even if no split is required, matchSendsRecvs may modify activePSets and blockedPSets to release some process sets
	//    that participated in a successful send-receive matching.
	/*set<pCFG_Checkpoint*> matchSendsRecvs(const pCFGNode& n, NodeState* state, 
	                                      set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets,
	                                      const Function& func, NodeState* fState);*/
	void matchSendsRecvs(const pCFGNode& n, const vector<Lattice*>& dfInfo, NodeState* state, 
	                     // Set by analysis to identify the process set that was split
	                     int& splitPSet,
	                     vector<ConstrGraph*>& splitConditions, 
	                     vector<DataflowNode>& splitPSetNodes,
	                     // for each split process set, true if its active and false if it is blocked
	                     vector<bool>&         splitPSetActive,
	                     // for each process set that was not split, true if becomes active as a result of the match,
	                     // false if its status doesn't change
	                     vector<bool>&         pSetActive,
	                     const Function& func, NodeState* fState);
	
	// Release the given process set by moving it from from blockedPSets to activePSets and releasedPSets
	static void releasePSet(int pSet, set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets);
	
	// Split the given partition into released processes and blocked processes, using the given process sets. All process
	//    sets are assumed to use the cg constraint graph, which contains information about all of them.
	//    otherProcs1 and otherProcs2 are the remaining two sets of processes (if released/blocked are senders, then these are
	//    the receivers and vice versa).
	// Returns the set of checkpoints for partitions that result from this split
	/*set<pCFG_Checkpoint*> splitProcs(const pCFGNode& n, ConstrGraph* cg, int splitPSet, 
	                                 set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets, 
	                                 const contRangeProcSet& releasedProcs, const contRangeProcSet& blockedProcs, 
	                                 const contRangeProcSet& otherProcs1, const contRangeProcSet& otherProcs2,
	                                 const Function& func, NodeState* fState);*/
	void splitProcs(const pCFGNode& n, ConstrGraph* cg, const DataflowNode& dfNode, int splitPSet, 
                   vector<ConstrGraph*>& splitConditions, vector<DataflowNode>& splitPSetNodes,
                   // for each split process set, true if its active and false if it is blocked
	                vector<bool>& splitPSetActive,
                   const contRangeProcSet& releasedProcs, const contRangeProcSet& blockedProcs,
                   const set<contRangeProcSet>& otherProcs);

	// Create the partition condition constraint graph for process splitPSet's rank bounds being turned 
	//    into then tgtProcs set and return it.
	// otherProcs and otherProcsSet contain the other relevant process sets whose bounds need to be removed 
	//    from the partition condition's constraint graph.
	ConstrGraph* createPSetCondition(ConstrGraph* cg, int splitPSet, const contRangeProcSet& tgtProcs, 
                                    const contRangeProcSet& otherProcs, const set<contRangeProcSet>& otherProcsSet);
	
	// Replaces the upper and lower bounds of goodSet with those of rank set so that some partition can
	//     assume the identity of the ranks in goodSet.
	// Removes the upper and lower bounds of the badSets from the given constraint graph.
	static void cleanProcSets(ConstrGraph* cg, 
	                          const contRangeProcSet& rankSet, const contRangeProcSet& goodSet,
	                          const contRangeProcSet& badSet1, const contRangeProcSet& badSet2, 
	                          const contRangeProcSet& badSet3);
};

#endif
