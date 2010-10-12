#ifndef PCFG_H
#define PCFG_H

#include <vector>
#include <algorithm>
#include <iostream>

#include "common.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "affineInequality.h"
#include "ConstrGraph.h"

class pCFG_FWDataflow;

class pCFGNode
{
	// Set of Dataflow nodes that make up the current pCFG node, in canonical order
	vector<DataflowNode> pSetDFNodes;
	
	public:
	pCFGNode();
	pCFGNode(const vector<DataflowNode>& pSetDFNodes);
	
	// Initializes the given pCFG node with the given vector of DataflowNodes
	void init(const vector<DataflowNode>& pSetDFNodes);
	
	bool operator = (const pCFGNode& that);
	bool operator == (const pCFGNode& that) const;
	bool operator != (const pCFGNode& that) const;
	bool operator < (const pCFGNode& that) const;
	
	// Advances the given process set to the given DataflowNode
	void advance(int pSet, const DataflowNode& n);
	
	// Advances the given process set to the only outgoing descendant of its current dataflow node
	void advanceOut(int pSet);
		
	// Advances the given process set to the only incoming descendant of its current dataflow node
	void advanceIn(int pSet);
	
	// Returns the given process set's current DataflowNode
	const DataflowNode& getCurNode(int pSet) const;
		
	const vector<DataflowNode>& getPSetDFNodes() const;
	
	// Removes the given process set from this pCFGNode
	bool removePSet(int pSet);
	
	// Adds a new process set to the given DFNode. This process set will start out at the given DataflowNode.
	// Returns the numeric id of the new process set
	int createPSet(const DataflowNode& startNode);
	
	string str(string indent="") const;
};

class pCFGState
{
	private:
	static map<pCFGNode, NodeState*> nodeStateMap;
	static bool nodeStateMapInit;
	
	public:
	// Returns the NodeState object associated with the given pCFG node.
	// Uses the given pCFG_FWDataflow to initialize any NodeState object that may need to be created.
	static NodeState* getNodeState(const Function& func, const pCFGNode& n, pCFG_FWDataflow* intraAnalysis);
	
	// Initializes the node state of a given pCFG Node from the given analysis
	static void initNodeState(const Function& func, const pCFGNode& n, NodeState* state, pCFG_FWDataflow* intraAnalysis);
};

class pCFG_Checkpoint
{
	public: 
	pCFGNode n;
	
	// the arguments to runAnalysis() used as part of the dataflow pass
	const Function& func;
	NodeState* fState;
	
	// Set of processes that are currently active (i.e. not blocked)
	set<int> activePSets;
	
	// Set of processes that are currently blocked on communication or the end of the function
	set<int> blockedPSets;
	
	// Set of processes that are are moved from blocked to active status from the time the 
	// analysis is checkpointed to the time that it resumes
	set<int> releasedPSets;
	
	// True if this checkpoint corresponds to a process set split
	bool isSplit;
	
	// The process set that performed the split
	int splitPSet;
	
	// The conditions that pertain to the splitPSet along the various splits
	//pCFG_SplitConditions splitConditions;
	ConstrGraph* splitCondition;
	
	pCFG_Checkpoint(const pCFG_Checkpoint &that);
	
	pCFG_Checkpoint(pCFGNode n, const Function& func, NodeState* fState,
	                const set<int>& activePSets, const set<int>& blockedPSets, const set<int>& releasedPSets, 
	                bool isSplit, int splitPSet,
	                /*pCFG_SplitConditions splitConditions*/ConstrGraph* splitCondition);
	
	// The constructor that corresponds to process set splits
	pCFG_Checkpoint(pCFGNode n, const Function& func, NodeState* fState,
	                const set<int>& activePSets, const set<int>& blockedPSets, const set<int>& releasedPSets, int splitPSet,
	                /*pCFG_SplitConditions splitConditions*/ConstrGraph* splitCondition);
	
	// The constructor that corresponds to blocked process sets
	pCFG_Checkpoint(pCFGNode n, const Function& func, NodeState* fState,
	                set<int> activePSets, set<int> blockedPSets);
	
	~pCFG_Checkpoint();
	
	// Returns true if this checkpoint is equivalent to that checkpoint (same location in the pCFG 
	// but possibly different dataflow state) and false otherwise.
	bool equiv(const pCFG_Checkpoint& that) const;
	
	// Merges the conditions of the two equivalent checkpoints and updates this checkpoint with the merge
	// Returns true if this causes this checkpoint to change and false otherwise.
	bool mergeUpd(const pCFG_Checkpoint& that);
	
	// Move the given process set from blocked to active status
	void resumePSet(int pSet);
	
	string str(string indent="") const;
}; // pCFG_Checkpoint


class pCFG_FWDataflow  : virtual public IntraProceduralDataflow
{
	// Set of checkpoints for all the currently blocked partitions
	set<pCFG_Checkpoint*> partitionChkpts;
	
	// Set of pCFGNodes that the analysis has already visited. Useful for determining
	// when the analysis visits a given pCFG node for the first time.
	set<pCFGNode> visitedPCFGNodes;
	
	public:
	pCFG_FWDataflow()
	{ }
	
	pCFG_FWDataflow(const pCFG_FWDataflow& that)
	{
		this->partitionChkpts = that.partitionChkpts;
	}
	
	// Runs the intra-procedural analysis on the given function. Returns true if 
	// the function's NodeState gets modified as a result and false otherwise.
	// state - the function's NodeState
	bool runAnalysis(const Function& func, NodeState* state);
	
	// Runs the dataflow analysis on the given partition, resuming from the given checkpoint.
	// Specifically runAnalysis_pCFG() starts the pCFG dataflow analysis at some pCFG node and continues for 
	//    as long as it can make progress.
	bool runAnalysis_pCFG(pCFG_Checkpoint* chkpt);
	
	bool runAnalysis_pCFG(const Function& func, NodeState* fState, pCFG_Checkpoint* chkpt);
	
	// Merge any process sets that are at equivalent dataflow states.
	// Return true if any process sets were merged, false otherwise.
	// If no processes are merged, mergePCFGNodes does not modify mergedN, dfInfo, activePSets, blockedPSets or releasedPSets.
	bool mergePCFGNodes(const pCFGNode& n, pCFGNode& mergedN, vector<Lattice*>& dfInfo,
	                    const Function& func, 
	                    set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets);
	
	// Adds the set of checkpoints to the overall set of checkpoints currently active
	// in this analysis. The caller is required to return immediately to the surrounding
	// runAnalysisResume() call.
	void split(const set<pCFG_Checkpoint*>& splitChkpts);
	
	// Fills tgtLat with copies of the lattices in srcLat. tgtLat is assumed to be an empty vector
	void latticeCopyFill(vector<Lattice*>& tgtLat, const vector<Lattice*>& srcLat);
	
	// Deallocates all the lattices in lats and empties it out
	void deleteLattices(vector<Lattice*>& lats);
	
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts) {}
	
	// Generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	virtual void genInitState(const Function& func, const pCFGNode& n, const NodeState& state,
	                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)=0;
	
	// Copies the dataflow information from the srcPSet to the tgtPSet and updates the copy with the
	// partitionCond (presumably using initPSetDFfromPartCond). Adds the new info directly to lattices and facts.
	// It is assumed that pCFGNode n contains both srcPSet and tgtPSet.
	// If omitRankSet==true, does not copy the constraints on srcPSet's bounds variables but
	// instead just adds them with no non-trivial constraints.
	virtual void copyPSetState(const Function& func, const pCFGNode& n, 
                              int srcPSet, int tgtPSet, NodeState& state,
                              vector<Lattice*>& lattices, vector<NodeFact*>& facts, 
                              ConstrGraph* partitionCond, bool omitRankSet)=0;
	
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
	virtual bool transfer(const pCFGNode& n, int pSet, const Function& func, 
	                      NodeState& state, const vector<Lattice*>&  dfInfo, 
	                      bool& deadPSet, bool& splitPSet, vector<DataflowNode>& splitPSetNodes,
                         bool& splitPNode, vector<ConstrGraph*>& splitConditions, bool& blockPSet)=0;
	
	// Called when a partition is created to allow a specific analysis to initialize
	// its dataflow information from the partition condition
	/*virtual void initDFfromPartCond(const Function& func, const pCFGNode& n, int pSet, NodeState& state, 
	                                const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
	                                ConstrGraph* partitionCond)=0;*/
	
	// Called when a process set is partitioned to allow the specific analysis to update the
	// dataflow state for that process set with the set's specific condition.
	// Returns true if this causes the dataflow state to change and false otherwise
	virtual bool initPSetDFfromPartCond(const Function& func, const pCFGNode& n, int pSet, 
	                                   const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
	                                   ConstrGraph* partitionCond)=0;
	
	// Merge the dataflow information of two process sets. The space of process set IDs will be 
	// compressed to remove the holes left by the removal.
	// pSetMigrations (initially assumed empty) is set to indicate which process sets have moved
	//    to new ids, with the key representing the process set's original id and the value entry
	//    representing the new id.
	virtual void mergePCFGStates(const list<int>& pSetsToMerge, const pCFGNode& n, const Function& func, 
	                             const vector<Lattice*>& dfInfo, NodeState& state, map<int, int>& pSetMigrations)=0;
	

	// Performs send-receive matching on a fully-blocked analysis partition. 
	// If some process sets need to be split, returns the set of checkpoints that corresponds to this pCFG node's descendants.
	// Otherwise, returns an empty set.
	// Even if no split is required, matchSendsRecvs may modify activePSets and blockedPSets to release some process sets
	//    that participated in a successful send-receive matching.
	/*virtual set<pCFG_Checkpoint*> matchSendsRecvs(const pCFGNode& n, NodeState* state, 
	                                              set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets,
	                                              const Function& func, NodeState* fState)=0;*/
	virtual void matchSendsRecvs(const pCFGNode& n, const vector<Lattice*>& dfInfo, NodeState* state, 
	                     // Set by analysis to identify the process set that was split
	                     int& splitPSet,
	                     vector<ConstrGraph*>& splitConditions, 
	                     vector<DataflowNode>& splitPSetNodes,
	                     // for each split process set, true if its active and false if it is blocked
	                     vector<bool>&         splitPSetActive,
	                     // for each process set that was not split, true if becomes active as a result of the match,
	                     // false if its status doesn't change
	                     vector<bool>&         pSetActive,
	                     const Function& func, NodeState* fState)=0;
	
	//typedef enum {retFalse, cont, normal} partitionTranferRet;

	// Call the analysis transfer function. 
	// If the transfer function wants to block on the current DataflowNode, sets blockPSet to true.
	// If the transfer function wants to split across multiple descendants, partitionTranfer() generates
	//    the resulting checkpoints, performs the split and sets splitPSet to true.
	// Otherwise, sets neither to true.
	bool partitionTranfer(const pCFGNode& n, pCFGNode& descN, int curPSet, const Function& func, NodeState* fState, 
	                      const DataflowNode& dfNode, NodeState* state, 
	                      vector<Lattice*>& dfInfoBelow, vector<Lattice*>& dfInfoNewBelow, 
	                      bool& deadPSet, bool& splitPSet, bool& splitPNode, bool& blockPSet, 
	                      set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets,
                         const DataflowNode& funcCFGEnd);

	// Updates the lattices in tgtLattices from the lattices in srcLattices. For a given lattice pair <t, s>
	//    (t from tgtLattices, s from srcLattices), t = t widen (t union s).
	// If delSrcLattices==true, deletes all the lattices in srcLattices.
	// Returns true if this causes the target lattices to change, false otherwise.
	bool updateLattices(vector<Lattice*>& tgtLattices, vector<Lattice*>& srcLattices, bool delSrcLattices);

	// Split the process set pSet in pCFGNOde n, resulting in a new pCFGNode that contains more 
	//    process sets. Advances descN to be this new pCFGNode and updates activePSets to make
	//    the newly-created process sets active. It is assumed that at the start of the function n == descN.
	// splitConditions - set of logical conditions that apply to each split process set
	// splitPSetNodes - the DataflowNodes that each split process set starts at.
	// if freshPSet==true, each partition will get a completely new process set and thus
	//    must be set fresh. If freshPSet==false, each partition's process set is simply
	//    an updated version of the old process set (i.e. an extra condition is applied).
	void performPSetSplit(const pCFGNode& n, pCFGNode& descN, int pSet, 
	               vector<Lattice*>& dfInfo,
	               vector<ConstrGraph*> splitConditions, vector<DataflowNode> splitPSetNodes,
	               vector<bool>& splitPSetActive, 
	               const Function& func, NodeState* fState, NodeState* state, 
	               set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets,
	               bool freshPSet);
	
	// Removes all known bounds on pSet's process set in dfInfo and replaces them with the default
	// constraints on process set bounds.
	virtual void resetPSet(int pSet, vector<Lattice*>& dfInfo)=0;
	
	// propagates the forward dataflow info from the current node's NodeState (curNodeState) to the next node's 
	// NodeState (nextNodeState)
	/*bool */void propagateFWStateToNextNode(const Function& func, 
                      const vector<Lattice*>& curNodeLattices, const pCFGNode& curNode, const NodeState& curNodeState, 
                      vector<Lattice*>& nextNodeLattices, const pCFGNode& nextNode,
                      ConstrGraph* partitionCond, int pSet);
             
	// Moves the given process set from srcPSets to destPSets. If noSrcOk==false, does nothing
	// if pSet is not inside srcPSets. If noSrcOk==true, adds pSet to destPSets regardless.
	// Returns true if the process set was in srcPSets and false otherwise.
	static bool movePSet(int pSet, set<int>& destPSets, set<int>& srcPSets, bool noSrcOk=false);
};

#endif
