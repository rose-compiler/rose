#include "pCFG.h"
#include <sys/time.h>
static int profileLevel=1;

#define startProfileFunc(funcName)     \
	struct timeval startTime, endTime;  \
	if(profileLevel>=1)                 \
	{                                   \
		cout << funcName << "() <<< \n"; \
		gettimeofday(&startTime, NULL);  \
	}

#define endProfileFunc(funcName)                                                                                                                         \
	if(profileLevel>=1)                                                                                                                                   \
	{                                                                                                                                                     \
		gettimeofday(&endTime, NULL);                                                                                                                      \
		cout << funcName << "() >>> time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n"; \
	}


/**************************
 ******** pCFGNode ********
 **************************/

pCFGNode::pCFGNode()
{ }

pCFGNode::pCFGNode(const vector<DataflowNode>& pSetDFNodes)
{
	this->pSetDFNodes = pSetDFNodes;
}

// Initializes the given pCFG node with the given vector of DataflowNodes
void pCFGNode::init(const vector<DataflowNode>& pSetDFNodes)
{
	this->pSetDFNodes = pSetDFNodes;
	/*int i=0;
	for(set<DataflowNode>::const_iterator it=pSetDFNodes.begin(); it!=pSetDFNodes.end(); it++, i++)
		this->pSetDFNodes.push_back(*it);* /
	this->pSetDFNodes = pSetDFNodes;
	
	// Give pSetDFNodes a canonical ordering
	sort(this->pSetDFNodes.begin() this->pSetDFNodes.end());*/
}

bool pCFGNode::operator = (const pCFGNode& that)
{
	pSetDFNodes = that.pSetDFNodes;
}

bool pCFGNode::operator == (const pCFGNode& that) const
{
	vector<DataflowNode> this_pSetDFNodes = this->pSetDFNodes;
	vector<DataflowNode> that_pSetDFNodes = that.pSetDFNodes;
	
	// Give both this_pSetDFNodes and that_pSetDFNodes canonical orderings
	sort(this_pSetDFNodes.begin(), this_pSetDFNodes.end());
	sort(that_pSetDFNodes.begin(), that_pSetDFNodes.end());
	
	return this_pSetDFNodes == that_pSetDFNodes;
}

bool pCFGNode::operator != (const pCFGNode& that) const
{
	return !(*this == that);
}

bool pCFGNode::operator < (const pCFGNode& that) const
{
	vector<DataflowNode> this_pSetDFNodes = this->pSetDFNodes;
	vector<DataflowNode> that_pSetDFNodes = that.pSetDFNodes;
	
	// Give both this_pSetDFNodes and that_pSetDFNodes canonical orderings
	sort(this_pSetDFNodes.begin(), this_pSetDFNodes.end());
	sort(that_pSetDFNodes.begin(), that_pSetDFNodes.end());
	
	return this_pSetDFNodes < that_pSetDFNodes;
}

// Advances the given process set to the given DataflowNode
void pCFGNode::advance(int pSet, const DataflowNode& n)
{
	ROSE_ASSERT(0<=pSet && pSet<pSetDFNodes.size());
	pSetDFNodes[pSet] = n;
}

// Advances the given process set to the only outgoing descendant of its current dataflow node
void pCFGNode::advanceOut(int pSet)
{
	ROSE_ASSERT(0<=pSet && pSet<pSetDFNodes.size());
	vector<DataflowEdge> edges = pSetDFNodes[pSet].outEdges();
	ROSE_ASSERT(edges.size()==1);
	pSetDFNodes[pSet] = (*(edges.begin())).target();
}

// Advances the given process set to the only incoming descendant of its current dataflow node
void pCFGNode::advanceIn(int pSet)
{
	ROSE_ASSERT(0<=pSet && pSet<pSetDFNodes.size());
	vector<DataflowEdge> edges = pSetDFNodes[pSet].inEdges();
	ROSE_ASSERT(edges.size()==1);
	pSetDFNodes[pSet] = (*(edges.begin())).source();
}

// Returns the given process set's current DataflowNode
const DataflowNode& pCFGNode::getCurNode(int pSet) const
{
	ROSE_ASSERT(0<=pSet && pSet<pSetDFNodes.size());
	return pSetDFNodes[pSet];
}
	
const vector<DataflowNode>& pCFGNode::getPSetDFNodes() const
{
	return pSetDFNodes;
}

// Removes the given process set from this pCFGNode
bool pCFGNode::removePSet(int pSet)
{
	ROSE_ASSERT(pSet>=0 && pSet<pSetDFNodes.size());
	pSetDFNodes.erase(pSetDFNodes.begin()+pSet);	
}

// Adds a new process set to the given DFNode. This process set will start out at the given DataflowNode.
// Returns the numeric id of the new process set
int pCFGNode::createPSet(const DataflowNode& startNode)
{
	pSetDFNodes.push_back(startNode);
	return pSetDFNodes.size()-1;
}

string pCFGNode::str(string indent) const
{
	ostringstream outs;
	outs << /*indent << */"[pCFGNode : \n";
	for(vector<DataflowNode>::const_iterator it=pSetDFNodes.begin(); it!=pSetDFNodes.end(); )
	{
		outs << indent << "    <" << (*it).getNode()->class_name() << " | " << (*it).getNode()->unparseToString() << " | " << (*it).getIndex() << ">";
		it++;
		if(it!=pSetDFNodes.end())
			outs << "\n";
	}
	outs << "]";
	return outs.str();
}

/***************************
 ******** pCFGState ********
 ***************************/

map<pCFGNode, NodeState*> pCFGState::nodeStateMap;
bool pCFGState::nodeStateMapInit;

// Returns the NodeState object associated with the given pCFG node.
// Uses the given pCFG_FWDataflow to initialize any NodeState object that may need to be created.
NodeState* pCFGState::getNodeState(const Function& func, const pCFGNode& n, pCFG_FWDataflow* intraAnalysis)
{
	if(analysisDebugLevel>=1) 
		cout << "pCFGState::getNodeState() n="<<n.str()<<"\n";
	// If the given pCFG node has already been visited
	map<pCFGNode, NodeState*>::iterator loc = nodeStateMap.find(n);
	if(loc != nodeStateMap.end())
	{
		if(analysisDebugLevel>=1) 
			cout << "    node found, initialized="<<loc->second->isInitialized((Analysis*)intraAnalysis)<<"\n";
		
		// If the given analysis has registered data at this node, return the node as it is
		// Otherwise, have the analysis initialize its state at this node
		if(!(loc->second->isInitialized((Analysis*)intraAnalysis)))
			initNodeState(func, n, loc->second, intraAnalysis);
		return loc->second;
	}
	else
	{
		if(analysisDebugLevel>=1) 
			cout << "    node NOT found\n";
		// If we don't find a NodeState associated with this pCFGNode, generate it,
		// initialize the analysis state and add the new NodeState to the map.
		NodeState* ns = new NodeState();
		initNodeState(func, n, ns, intraAnalysis);
		nodeStateMap[n] = ns;
		return ns;
	}
}

// Initializes the node state of a given pCFG Node from the given analysis
void pCFGState::initNodeState(const Function& func, const pCFGNode& n, NodeState* state, pCFG_FWDataflow* intraAnalysis)
{
	//cout << "pCFGState::initNodeState\n";
	vector<Lattice*>  initLats;
	vector<NodeFact*> initFacts;
	intraAnalysis->genInitState(func, n, *state, initLats, initFacts);
	state->setLattices((Analysis*)intraAnalysis, initLats);
	state->setFacts((Analysis*)intraAnalysis, initFacts);
}

/***********************
 *** pCFG_Checkpoint ***
 ***********************/

pCFG_Checkpoint::pCFG_Checkpoint(const pCFG_Checkpoint &that): n(that.n), func(that.func)
{
	this->fState          = that.fState;
	this->activePSets     = that.activePSets;
	this->blockedPSets    = that.blockedPSets;
	this->releasedPSets   = that.releasedPSets;
	this->isSplit         = that.isSplit;
	this->splitPSet       = that.splitPSet;
	this->splitCondition  = that.splitCondition;
}

pCFG_Checkpoint::pCFG_Checkpoint(pCFGNode n, const Function& func, NodeState* fState,
	                              const set<int>& activePSets, const set<int>& blockedPSets, const set<int>& releasedPSets, 
	                              bool isSplit, int splitPSet,
	                              /*pCFG_SplitConditions splitConditions*/ConstrGraph* splitCondition) : n(n), func(func)
{
	this->fState          = fState;
	this->activePSets     = activePSets;
	this->blockedPSets    = blockedPSets;
	this->releasedPSets   = releasedPSets;
	this->isSplit         = isSplit;
	this->splitPSet       = splitPSet;
	this->splitCondition  = splitCondition;
}
	
// The constructor that corresponds to process set splits
pCFG_Checkpoint::pCFG_Checkpoint(pCFGNode n, const Function& func, NodeState* fState,
                                 const set<int>& activePSets, const set<int>& blockedPSets, const set<int>& releasedPSets, int splitPSet,
                                 /*pCFG_SplitConditions splitConditions*/ConstrGraph* splitCondition) : n(n), func(func)
{
	this->fState          = fState;
	this->activePSets     = activePSets;
	this->blockedPSets    = blockedPSets;
	this->releasedPSets   = releasedPSets;
	this->isSplit         = true;
	this->splitPSet       = splitPSet;
	this->splitCondition  = splitCondition;
}

// The constructor that corresponds to blocked process sets
pCFG_Checkpoint::pCFG_Checkpoint(pCFGNode n, const Function& func, NodeState* fState,
                                 set<int> activePSets, set<int> blockedPSets) : n(n), func(func)
{
	this->fState          = fState;
	this->activePSets     = activePSets;
	this->blockedPSets    = blockedPSets;
	this->isSplit         = false;
}

pCFG_Checkpoint::~pCFG_Checkpoint()
{ 
	delete splitCondition;
}

// Returns true if this checkpoint is equivalent to that checkpoint (same location in the pCFG 
// but possibly different dataflow state) and false otherwise.
bool pCFG_Checkpoint::equiv(const pCFG_Checkpoint& that) const
{
	return fState          == that.fState &&
	       activePSets     == that.activePSets &&
	       blockedPSets    == that.blockedPSets &&
	       releasedPSets   == that.releasedPSets &&
	       isSplit         == that.isSplit &&
	       splitPSet       == that.splitPSet;
}

// Merges the conditions of the two equivalent checkpoints and updates this checkpoint with the merge
// Returns true if this causes this checkpoint to change and false otherwise.
bool pCFG_Checkpoint::mergeUpd(const pCFG_Checkpoint& that)
{
	return splitCondition->meetUpdate(that.splitCondition);
}
	
// Move the given process set from blocked to active status
void pCFG_Checkpoint::resumePSet(int pSet)
{
	ROSE_ASSERT(blockedPSets.find(pSet)!=blockedPSets.end());
	blockedPSets.erase(pSet);
	activePSets.insert(pSet);
	releasedPSets.insert(pSet);
}

string pCFG_Checkpoint::str(string indent) const
{
	ostringstream outs;
	outs << /*indent << */"[pCFG_Checkpoint : \n"; //fflush(stdout);
	outs << indent << "    n = " << n.str() << "\n";
	outs << indent << "    activePSets = <" ;
	for(set<int>::iterator it=activePSets.begin(); it!=activePSets.end(); )
	{
		outs << *it;
		it++;
		if(it!=activePSets.end())
			outs << ", ";
	}
	outs << ">\n";
	
	outs << indent << "    blockedPSets = <" ;
	for(set<int>::iterator it=blockedPSets.begin(); it!=blockedPSets.end(); )
	{
		outs << *it;
		it++;
		if(it!=blockedPSets.end())
			outs << ", ";
	}
	outs << ">\n";
	
	outs << indent << "    releasedPSets = <" ;
	for(set<int>::iterator it=releasedPSets.begin(); it!=releasedPSets.end(); )
	{
		outs << *it;
		it++;
		if(it!=releasedPSets.end())
			outs << ", ";
	}
	outs << ">\n";
	
	if(isSplit)
	{
		outs << indent << "    isSplit = " << isSplit << "\n";
		outs << indent << "    splitPSet = " << splitPSet << "\n";
		outs << indent << "    splitCondition = " << splitCondition->str() << "]";
	}
	else
		outs << indent << "    isSplit = " << isSplit << "]";

	return outs.str();
}

/*********************************
 ******** pCFG_FWDataflow ********
 *********************************/

/*varID pCFG_FWDataflow::rankVar;
varID pCFG_FWDataflow::nprocsVar;
contRangeProcSet pCFG_FWDataflow::rankSet;*/

// Runs the intra-procedural analysis on the given function. Returns true if 
// the function's NodeState gets modified as a result and false otherwise.
// state - the function's NodeState
bool pCFG_FWDataflow::runAnalysis(const Function& func, NodeState* fState)
{
	bool ret=false;
	
	pCFG_Checkpoint* curChkpt=NULL;

	// Keep processing partitions until dataflow along the entire pCFG has reached a fixed point
	do
	{
		if(analysisDebugLevel>=1) 
			cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
		// If there are any checkpoints left, pick one and continue the analysis from its pCFG node
		if(partitionChkpts.size()>0)
		{
			curChkpt = *(partitionChkpts.begin());
			partitionChkpts.erase(partitionChkpts.begin());
			if(analysisDebugLevel>=1) 
				cout << "@@@ RESUMING partition from checkpoint "<<curChkpt->str("@@@    ")<<"\n";
		}
		else
		{
			if(analysisDebugLevel>=1) 
				cout << "@@@ Starting FRESH partition\n";
		}
			
		runAnalysis_pCFG(func, fState, curChkpt);

		if(analysisDebugLevel>=1) 
			cout << "@@@ Partition reached BLOCKED point or SPLIT. partitionChkpts.size()="<<partitionChkpts.size()<<"\n";
		// The current partition has reached a blocked point
		
		// If there are currently multiple active partitions, try to condense partitions
		// that are blocked at the same pCFG node
		if(partitionChkpts.size()>1)
		{
			// Set of partition checkpoints after aggregation
			set<pCFG_Checkpoint*> newPartitions;
			
	/*cout << "pCFG_FWDataflow::runAnalysis, partitionChkpts.size()="<<partitionChkpts.size()<<"\n";
	for(set<pCFG_Checkpoint*>::const_iterator it=partitionChkpts.begin();
	    it!=partitionChkpts.end(); it++)
	{
		cout << "----"<<(*it)->str("    ")<<"\n";
	}*/

			// See if we can condense any pair of partitions into a single partition
			set<pCFG_Checkpoint*>::iterator itA=partitionChkpts.begin();
			while(itA!=partitionChkpts.end())
			{
				//cout << "runAnalysis() Partition Condense: (*itA) = "<<(*itA)->str()<<"\n";
				const pCFGNode& nA = (*itA)->n;
				//cout << "runAnalysis() Partition Condense: nA = "<<nA.str()<<"\n";
				
				// Set of partitions that are equivalent to this one;
				set<pCFG_Checkpoint*> equivParts;
				for(set<pCFG_Checkpoint*>::iterator itB=itA; itB!=partitionChkpts.end(); itB++)
				{
					//cout << "runAnalysis() Partition Condense: (*itB) = "<<(*itB)->str()<<"\n";
					const pCFGNode& nB = (*itB)->n;
					//cout << "runAnalysis() Partition Condense: nB = "<<nB.str()<<"\n";
					
					// If the two partitions are different but are currently at the same pCFG node
					if(itA!=itB && nA==nB)
					{
						// The two checkpoints must be equivalent (same execution state but possibly 
						// different dataflow states)
						ROSE_ASSERT((*itA)->equiv(*(*itB)));
						// Record that these checkpoints are equivalent
						equivParts.insert(*itB);
					}
				}
				
				// If the current checkpoint *itA is equivalent to multiple other checkpoints, 
				// merge them all into a single checkpoint
				if(equivParts.size()>0)
				{
					//cout << "@@@ Merging equivalent partitions:\n";
					//cout << "    0: "<<(*itA)->str("        ")<<"\n";
					
					int i=1;
					for(set<pCFG_Checkpoint*>::iterator itE=equivParts.begin(); itE!=equivParts.end(); itE++, i++)
					{
						//cout << "    "<<i<<": "<<(*itE)->str("        ")<<"\n";
						// Merge the current equivalent checkpoint into *itA (i.e. merge 
						// their dataflow states) and delete it
						(*itA)->mergeUpd(*(*itE));
						partitionChkpts.erase(*itE);
						delete *itE;
					}
					//cout <<"     ===>\n";
					if(analysisDebugLevel>=1) 
						cout <<"         "<<(*itA)->str("        ")<<"\n";
				}//
				// Move checkpoint *itA from partitionChkpts to newPartitions
				newPartitions.insert(*itA);
				partitionChkpts.erase(*itA);
				
				// Move on to the next unmerged partition
				itA=partitionChkpts.begin();
			}
			
			// Replace the set of partition checkpoints with the new set of merged checkpoints
			partitionChkpts = newPartitions;
			
			ROSE_ASSERT(partitionChkpts.size()>0);
		}
		
		if(analysisDebugLevel>=1) 
			cout << "@@@ Partitions CONDENSED. partitionChkpts.size()="<<partitionChkpts.size()<<"\n";
	}
	while(partitionChkpts.size()>0);

	// If there are no more checkpoints left, the dataflow analysis has reached a 
	// fixed point on the pCFG
	cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
	cout << "@@@@@@@@@@@@         pCFG FIXED POINT         @@@@@@@@@@@@@@@\n";
	cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";

	// Return true if any partition caused the function's final dataflow state to change
	// and false otherwise.
//printf("    returning %d\n", ret);


	// We're truly done with this function
/*	cout << "(*(NodeState::getNodeStates(funcCFGEnd).begin()))->getLatticeAbove(this) == fState->getLatticeBelow(this):\n";
	printf("fState->getLatticeBelow(this).size()=%d\n", fState->getLatticeBelow(interAnalysis).size());
	printf("fState->getLatticeAbove(this).size()=%d\n", fState->getLatticeAbove(interAnalysis).size());*/
	
	/*
		// Test if the Lattices at the bottom of the function after the forward analysis are equal to their
		// original values in the function state.
		bool modified = !NodeState::eqLattices(
			                      (*(NodeState::getNodeStates(funcCFGEnd).begin()))->getLatticeAbove(this),
			                      fState->getLatticeBelow(interAnalysis this));
			
		// Update the the function's exit NodeState with the final state of this function's dataflow analysis.
		NodeState* exitState = *(NodeState::getNodeStates(funcCFGEnd).begin());
		NodeState::copyLattices_bEQa(interAnalysis this, *fState, this, *exitState);

		return modified;
	*/
	
// !!! still need to figure out how this intra-procedural analysis fits into an inter-procedural analysis
	return ret;
}

// Runs the dataflow analysis on the given partition, resuming from the given checkpoint.
// Specifically runAnalysis_pCFG() starts the pCFG dataflow analysis at some pCFG node and continues for 
//    as long as it can make progress.
bool pCFG_FWDataflow::runAnalysis_pCFG(pCFG_Checkpoint* chkpt)
{
	return runAnalysis_pCFG(chkpt->func, chkpt->fState, chkpt);
}

/*pCFGNode watchN;
bool watching = false;
Function watchFunc;
pCFG_FWDataflow* watchObj;

void printWatch(string marker)
{
	if(watching)
	{
		NodeState* watchState = pCFGState::getNodeState(watchFunc, watchN, watchObj);	
		cout << marker << ": watchState="<<watchState<<"\n"; fflush(stdout);
		vector<Lattice*>& watchDfInfoBelow  = watchState->getLatticeBelowMod(watchObj);	
		Lattice* watchLatBelow = *(watchDfInfoBelow.begin());
		cout << marker << ": watchLatBelow="<<watchLatBelow<<"\n"; fflush(stdout);
		ConstrGraph* watchCGBelow = dynamic_cast<ConstrGraph*>(watchLatBelow);
		cout << marker << ": watchCGBelow="<<watchCGBelow<<"\n"; fflush(stdout);
		
		vector<Lattice*>& watchDfInfoAbove  = watchState->getLatticeAboveMod(watchObj);	
		Lattice* watchLatAbove = *(watchDfInfoAbove.begin());
		cout << marker << ": watchLatAbove="<<watchLatAbove<<"\n"; fflush(stdout);
		ConstrGraph* watchCGAbove = dynamic_cast<ConstrGraph*>(watchLatAbove);
		cout << marker << ": watchCGAbove="<<watchCGAbove<<"\n"; fflush(stdout);
	}	
}*/

bool pCFG_FWDataflow::runAnalysis_pCFG(const Function& func, NodeState* fState, pCFG_Checkpoint* chkpt)
{
	DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(func.get_definition());
	DataflowNode funcCFGEnd = cfgUtils::getFuncEndCFG(func.get_definition());
	
	if(analysisDebugLevel>=1 && chkpt==NULL) {
		printf("############################\n");
		printf("#### Function %s() ####\n", func.get_name().str());
		printf("############################\n");
	}
	
	/*if(chkpt==NULL)
	{
		// initialize the function's entry NodeState 
		NodeState* entryState = *(NodeState::getNodeStates(funcCFGStart).begin());
		//printf("before copyLattices on <%s | %s>\n", funcCFGStart.getNode()->class_name().c_str(), funcCFGStart.getNode()->unparseToString().c_str());
		NodeState::copyLattices_aEQa(this, *entryState, interAnalysis* this, *fState);
	}*/
	
	if(analysisDebugLevel>=1)
	{
		printf("pCFG_FWDataflow::runAnalysis() function %s()\n", func.get_name().str());
		if(chkpt)
			printf("    chkpt=%p=%s\n", chkpt, chkpt->str("    ").c_str());
	}

	// Dataflow Iterator that iterates over all the nodes in this function
	//VirtualCFG::dataflow dfIt(funcCFGStart, funcCFGEnd);
	
	// Set of nodes that this analysis has blocked progress on until the next join point
	set<DataflowNode> joinNodes;
	
	// The pCFG node that the analysis is currently at
	pCFGNode n;
	
	// Process sets that are currently active (i.e. not blocked).
	set<int> activePSets;
	// Process sets that are currently blocked on communication or the end of the function.
	set<int> blockedPSets;
	// Process sets that are resumed from blocked state. For each such process set we 
	// must skip the call to the transfer function for their next dataflow node because they
	// already called the transfer function when they originally became blocked on the node.
	set<int> releasedPSets;
	
	if(chkpt==NULL)
	{
		// If we're not restarting from a checkpoint, initialize the analysis to start with a
		// single active process set that starts at the top of the function.
		vector<DataflowNode> initDFNodes;
		initDFNodes.push_back(funcCFGStart);
		n.init(initDFNodes);
		activePSets.insert(0);
	}
	else
	{
		// Restart the analysis state from the checkpoint
		n = chkpt->n;
		activePSets  = chkpt->activePSets;
		blockedPSets = chkpt->blockedPSets;
		releasedPSets  = chkpt->releasedPSets;
		
		if(analysisDebugLevel>=1) cout << "@@@ Restarting analysis from checkpoint "<<chkpt->str("      ")<<"\n";
		
		// Initialize this partition's dataflow info from the condition in the checkpoint
		/*NodeState* state = pCFGState::getNodeState(func, n, this);
		if(chkpt->isSplit)
		{
			initDFfromPartCond(func, n, chkpt->splitPSet, *(pCFGState::getNodeState(func, n, this)),
	                         state->getLatticeAbove((Analysis*)this), state->getFacts((Analysis*)this),
	                         chkpt->splitCondition);
		}*/
	}
	ROSE_ASSERT(activePSets.size() + blockedPSets.size() == n.getPSetDFNodes().size());
	ROSE_ASSERT(releasedPSets.size() <= n.getPSetDFNodes().size());
	
	// Outer work loop, computes dataflow on all process sets and performs send-receive matching when 
	// all process sets become blocked
	do
	{
		// True if the information below this pCFGNode is modified as a result of processing
		//    (i.e. incorporating dataflow info from predecessors and running the transfer function)
		// False otherwise.
		// modified is reset to false whenever we move on to a new pCFGNode, which is not necessarily
		//    every time the per-process set while loop iterates. For example, if a given process 
		//    set blocks and another one is scheduled, we stay at the same pCFGNode.
		bool modified = false;
		
		// Process dataflow of all the process sets until they all become blocked
		while(activePSets.size()>0)
		{
			int curPSet;
			// The current process set is the next active one or the process set that 
			// performed the split from which we're restarting 
			// (the later special condition is only needed to keep the output more readable)
			if(chkpt==NULL || !(chkpt->isSplit)) 
			{
				//curPSet = *(activePSets.begin());
				
				// make sure to pick pSet 1 last (this is a hack to postpone the fix where we need to
				// keep the states of the different process sets separate until fully-blocked points)
				curPSet = -1;
				for(set<int>::iterator it=activePSets.begin(); it!=activePSets.end(); it++)
					if(*it!=1) curPSet = *it;
				if(curPSet == -1) curPSet=1;

if(analysisDebugLevel>=1) 				
{
	cout << "runAnalysis_pCFG() A activePSets ["<<activePSets.size()<<"]=";   for(set<int>::iterator it=activePSets.begin();    it!=activePSets.end();   it++) { cout << *it << " "; } cout <<"\n";
	cout << "runAnalysis_pCFG() A blockedPSets ["<<blockedPSets.size()<<"]=";  for(set<int>::iterator it=blockedPSets.begin();   it!=blockedPSets.end();  it++) { cout << *it << " "; } cout <<"\n";
	cout << "runAnalysis_pCFG() A releasedPSets ["<<releasedPSets.size()<<"]="; for(set<int>::iterator it=releasedPSets.begin();  it!=releasedPSets.end(); it++) { cout << *it << " "; } cout <<"\n";
}
			}
			else
				curPSet = chkpt->splitPSet;
				
			// If we're restarting, we don't need the checkpoint any more
			if(chkpt)
			{
				delete chkpt;
				chkpt = NULL;
			}
			
			// Process curPSet until it becomes blocked, at which point we'll 
			// break out of the loop
			while(1)
			{
				// Record that we've now visited this pCFGNode
				pair<set<pCFGNode>::iterator, bool> insertRet = visitedPCFGNodes.insert(n);
				// True if this is the first time this analysis has visited this pCFGNode, false otherwise.
				bool firstTimeVisited = insertRet.second;
	
				const DataflowNode& dfNode = n.getCurNode(curPSet);
				SgNode* sgn = dfNode.getNode();
				
				NodeState* state = pCFGState::getNodeState(func, n, this);
				//printf("state=%p\n", state);
				ROSE_ASSERT(state);
		
if(analysisDebugLevel>=1)
{
	cout << "runAnalysis_pCFG() B activePSets ["<<activePSets.size()<<"]=";   for(set<int>::iterator it=activePSets.begin();    it!=activePSets.end();   it++) { cout << *it << " "; } cout <<"\n";
	cout << "runAnalysis_pCFG() B blockedPSets ["<<blockedPSets.size()<<"]=";  for(set<int>::iterator it=blockedPSets.begin();   it!=blockedPSets.end();  it++) { cout << *it << " "; } cout <<"\n";
	cout << "runAnalysis_pCFG() B releasedPSets ["<<releasedPSets.size()<<"]="; for(set<int>::iterator it=releasedPSets.begin();  it!=releasedPSets.end(); it++) { cout << *it << " "; } cout <<"\n";
}
				
				if(analysisDebugLevel>=1){
					cout << "====================================================================\n";
					cout << "pSet "<<curPSet<<": Current Node "<<dfNode.str()<<", firstTimeVisited="<<firstTimeVisited<<", modified="<<modified<<"\n";
					cout << "activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<"\n";
				}			
/*if(isSgFunctionDefinition(dfNode.getNode()))
{
	watchN = n;
	watching = true;
	watchFunc = func;
	watchObj = this;
}*/				
				vector<Lattice*>& dfInfoAbove  = state->getLatticeAboveMod(this);
				vector<Lattice*>& dfInfoBelow  = state->getLatticeBelowMod(this);	
	
				// The pCFG node that corresponds to curPSet advancing along the outgoing edge.
				// Will be set by the code inside the if statement below to advance the 
				// analysis along the pCFG.
				pCFGNode descN(n);
				// Set to true if we want to propagate the state at the bottom
				// of this pCFGNode to its descendant, false otherwise
				bool propagateToDesc=true;
				
				// ---------------------------------------------------------------------------------------
				
				//latticeCopyFill(aboveCopy, pCFGState::getNodeState(func, n, this)->getLatticeAbove(this));					
				// See if we need to merge the process sets in this pCFGNode because two sets 
				// are at the same DataflowNodes and have the same blocked/active status
				bool merged = mergePCFGNodes(n, descN, dfInfoAbove, func,  
                                         activePSets, blockedPSets, releasedPSets);
if(analysisDebugLevel>=1) cout << "merged="<<merged<<"\n";
				// If we did end up merging some process sets
				if(merged)
				{
					if(analysisDebugLevel>=1) 
						cout << "@@@ Merging completed. Propagating dataflow state to mergedN = "<<descN.str("          ")<<"\n";
					// We've identified a new descendant node and will now propagate 
					// n's dataflow state to that node
					//descN = mergedN;
					modified = true;
					
					// Set dfInfoBelow to the the state ABOVE this node so that it can be propagated
					// to the merged descN
					dfInfoBelow = dfInfoAbove;	
				}
				// Else, if we didn't merge and the current process set was resumed from blocked 
				// status, skip the transfer function
				else if(releasedPSets.erase(curPSet) > 0)
				{
					if(analysisDebugLevel>=1) 
						cout << "@@@ Resuming previously blocked pSet "<<curPSet<<"\n";
					descN.advanceOut(curPSet);
					
					/*
					// If this is the first time this analysis has visited this pCFGNode, the dataflow
					//    info below this node will not be initialized. Since in this case we're skipping 
					//    over the transfer function and the associated code that propagates dataflow info
					//    from above this pCFGNode to below it, we'll need to make this propagation
					//    (just copy, no transfer) now.
					if(firstTimeVisited)
					{
						const vector<Lattice*>& dfInfoAbove  = state->getLatticeAbove(this);
						vector<Lattice*>& dfInfoBelow        = state->getLatticeBelowMod(this);
						
						int j=0;
						vector<Lattice*>::const_iterator itA;
						vector<Lattice*>::iterator       itB;
						ROSE_ASSERT(dfInfoAbove.size() == dfInfoBelow.size());
						for(itA = dfInfoAbove.begin(), itB = dfInfoBelow.begin();
						    itA != dfInfoAbove.end() && itB != dfInfoBelow.end(); 
						    itA++, itB++, j++)
						{
							if(analysisDebugLevel>=1){
								cout << "    Just-Copy Above: Lattice "<<j<<": \n    "<<(*itA)->str("        ")<<"\n";
								cout << "    Just-Copy Below: Lattice "<<j<<": \n    "<<(*itB)->str("        ")<<"\n";
							}
							(*itB)->copy(*itA);
						}
						modified = true;
					}
					else*/
						// Update the information below this node from the information above, taking care of the
						// necessary union and widening.
						modified = updateLattices(dfInfoBelow, dfInfoAbove, false) || modified;
				}
				// If we didn't merge and this is a regular process set that was not just resumed
				else
				{
					// -------------------------------------------------------------------------
					// Overwrite the Lattices below this node with the lattices above this node.
					// The transfer function will then operate on these Lattices to produce the
					// correct state below this node.
					
					// The new information below this pCFGNode. Initially a copy of the above information
					vector<Lattice*> dfInfoNewBelow;
					
					int j=0;
					/*vector<Lattice*>::const_iterator itA;
					vector<Lattice*>::iterator       itB;
					//printf("                 dfInfoAbove.size()=%d, dfInfoBelow.size()=%d, this=%p\n", dfInfoAbove.size(), dfInfoBelow.size(), this);
					ROSE_ASSERT(dfInfoAbove.size() == dfInfoBelow.size());
					for(itA = dfInfoAbove.begin(), itB = dfInfoBelow.begin();
					    itA != dfInfoAbove.end() && itB != dfInfoBelow.end(); 
					    itA++, itB++, j++)
					{
						if(analysisDebugLevel>=1){
							cout << "    Pre-Transfer Above: Lattice "<<j<<": \n    "<<(*itA)->str("        ")<<"\n";
							cout << "    Pre-Transfer Below: Lattice "<<j<<": \n    "<<(*itB)->str("        ")<<"\n";
						}
						(*itB)->copy(*itA);
					}*/
					
					// Initialize dfInfoNewBelow to be the copy of the information above this node.
					//    It will then get pushed through the transfer function before being unioned 
					//    with and widened into the information below this node.
					for(vector<Lattice*>::const_iterator itA=dfInfoAbove.begin(); itA != dfInfoAbove.end(); itA++)
					{
						if(analysisDebugLevel>=1){
							cout << "    Pre-Transfer Above: Lattice "<<j<<": \n    "<<(*itA)->str("        ")<<"\n";
						}
						dfInfoNewBelow.push_back((*itA)->copy());
					}
					
					/*for(vector<Lattice*>::const_iterator itB=dfInfoBelow.begin(); itB != dfInfoBelow.end(); itB++)
					{
						if(analysisDebugLevel>=1){
							cout << "    Pre-Transfer Below: Lattice "<<j<<": \n    "<<(*itB)->str("        ")<<"\n";
						}
					}*/
					
					// <<<<<<<<<<<<<<<<<<< TRANSFER FUNCTION <<<<<<<<<<<<<<<<<<<
					bool splitPSet = false;
					bool splitPNode = false;
					bool blockPSet = false;
					bool deadPSet  = false;
					// If we're currently at a function call, use the associated inter-procedural
					// analysis to determine the effect of this function call on the dataflow state.
					if(isSgFunctionCallExp(sgn))
					{
						Function calledFunc(isSgFunctionCallExp(sgn));
						if(calledFunc.get_definition())
						{
							/*dynamic_cast<InterProceduralDataflow*>(interAnalysis)->
							      transfer(calledFunc, dfNode, *state, dfInfoBelow, true);*/
						}
						else
							modified = partitionTranfer(n, descN, curPSet, func, fState, dfNode, state, dfInfoBelow, dfInfoNewBelow, 
							                            deadPSet, splitPSet, splitPNode, blockPSet, 
							                            activePSets, blockedPSets, releasedPSets, funcCFGEnd) || modified;
					}
					// Otherwise, call the user's transfer function
					else
						modified = partitionTranfer(n, descN, curPSet, func, fState, dfNode, state, dfInfoBelow, dfInfoNewBelow, 
						                            deadPSet, splitPSet, splitPNode, blockPSet, 
						                            activePSets, blockedPSets, releasedPSets, funcCFGEnd) || modified;
					// Either all three flags are =false or only one is =true
					ROSE_ASSERT((!splitPSet && !splitPNode && !blockPSet) ||
					            XOR(XOR(splitPSet, splitPNode), blockPSet));
					
					// If curPSet is at the end of the function, it becomes blocked
					if(n.getCurNode(curPSet) == funcCFGEnd)
					{
						if(analysisDebugLevel>=1) 
							cout << "@@@ Process set "<<curPSet<<" reached function end: transitioning from active to blocked status.\n";
						
						// Move curPSet from active to blocked status
						movePSet(curPSet, blockedPSets, activePSets);
						// Move on to the next active process set (if any)
						break;
					}
					
					// If the current process set has reached an impossible dataflow state, stop progress 
					// along the current pCFG node
					if(deadPSet)
						return false;
					// If the current process set wants to be split, break out of this analysis and let the parent 
					// partitioned analysis re-schedule this analysis
					else if(splitPNode)
						return false;
					// If the current process set was split into multiple process sets, descN was updated by
					// partitionTransfer to be the new pCFGNode, which contains the additional split process sets.
					// Advance to the next pCFG node, which corresponds to curPSet advancing to its next DataflowNode.
					else if(splitPSet)
					{
						/*cout << "activePSets = ";
						for(set<int>::iterator it=activePSets.begin(); it!=activePSets.end(); it++)
						{ cout << *it << " "; }
						cout <<"\n";*/
					}
					// If the current process set wants to block at the current node
					else if(blockPSet)
					{
						if(analysisDebugLevel>=1) 
							cout << "@@@ Process set "<<curPSet<<" transitioning from active to blocked status.\n";
						
						// Move curPSet from active to blocked status
						movePSet(curPSet, blockedPSets, activePSets);
						// Move on to the next active process set (if any)
						break;
					}
					else
						descN.advanceOut(curPSet);
					// >>>>>>>>>>>>>>>>>>> TRANSFER FUNCTION >>>>>>>>>>>>>>>>>>>
				} // else - if(releasedPSets.erase(curPSet) > 0)
				
				// Print out the post-transfer lattices
				if(analysisDebugLevel>=1)
				{
					int j=0;
					for(vector<Lattice*>::const_iterator itB = dfInfoBelow.begin();
					    itB != dfInfoBelow.end(); itB++, j++)
						cout << "    Transferred: Lattice "<<j<<": \n    "<<(*itB)->str("        ")<<"\n";
					printf("    transferred, modified=%d\n", modified);
					printf("    ------------------\n");
				}

				// If the state below this node got modified from the state above this node or by the transfer 
				// function, advance to descN
				if(modified)
				{
					// Examine the descendant pCFGNode
					if(analysisDebugLevel>=1)
					{
						cout << "    Descendant ("<<descN.str()<<"):\n";
						cout << "    ~~~~~~~~~~~~\n";
					}
					
					// The state of the next pCFG node
					NodeState* nextState = pCFGState::getNodeState(func, descN, this);
					if(analysisDebugLevel>=1) printf("nextState=%p\n", nextState);
					ROSE_ASSERT(nextState);
					
					// Propagate the Lattices below this node to its descendant
					/*modified = */propagateFWStateToNextNode(func, dfInfoBelow, n, *state, 
					                                      nextState->getLatticeAboveMod(this), descN, 
					                                      (ConstrGraph*)NULL, 0)/* || modified*/;
					if(analysisDebugLevel>=1)
					{
						printf("    propagated, modified=%d\n", modified);
						printf("    ^^^^^^^^^^^^^^^^^^\n"); fflush(stdout);
					}
				
					n = descN;
					// We're moving onto the next pCFGNode, so reset the modified flag
					modified = false;
				}
				// Otherwise, this process set becomes blocked
				else
					// !!! Not sure if it is correct to block process sets that have reached a fixed point. We may need
					// !!! to add an extra status to handle just this case.
					//break;
					return true;
			} // while(1)
		
if(analysisDebugLevel>=1)
{	
	cout << "runAnalysis_pCFG() C activePSets ["<<activePSets.size()<<"]=";   for(set<int>::iterator it=activePSets.begin();    it!=activePSets.end();   it++) { cout << *it << " "; } cout <<"\n";
	cout << "runAnalysis_pCFG() C blockedPSets ["<<blockedPSets.size()<<"]=";  for(set<int>::iterator it=blockedPSets.begin();   it!=blockedPSets.end();  it++) { cout << *it << " "; } cout <<"\n";
	cout << "runAnalysis_pCFG() C releasedPSets ["<<releasedPSets.size()<<"]="; for(set<int>::iterator it=releasedPSets.begin();  it!=releasedPSets.end(); it++) { cout << *it << " "; } cout <<"\n";
}
		} // while(activePSets.size()>0)

		ROSE_ASSERT(activePSets.size() == 0);
		ROSE_ASSERT(blockedPSets.size() == n.getPSetDFNodes().size());
		ROSE_ASSERT(releasedPSets.size() == 0);
		
		// All process sets are now blocked, so we should match sends to receives, if possible
		int splitPSet;
		vector<ConstrGraph*> splitConditions;
		vector<DataflowNode> splitPSetNodes;
		vector<bool>         splitPSetActive;
		vector<bool>         pSetActive;
		
		vector<Lattice*> aboveCopy;
		latticeCopyFill(aboveCopy, pCFGState::getNodeState(func, n, this)->getLatticeAbove(this));
		
		/*set<pCFG_Checkpoint*> splitChkpts = matchSendsRecvs(n, pCFGState::getNodeState(func, n, this), 
		                                                        activePSets, blockedPSets, releasedPSets, func, fState);*/
		matchSendsRecvs(n, aboveCopy, pCFGState::getNodeState(func, n, this), splitPSet, 
		                splitConditions, splitPSetNodes, splitPSetActive, pSetActive, func, fState);
		
		if(analysisDebugLevel>=1)
		{
			cout << "After matchSendsRecvs() : splitConditions.size()="<<splitConditions.size()<<"\n";
			cout << "runAnalysis_pCFG() activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<"\n";
		}
		
		// If a split was indeed performed
		if(splitConditions.size()>0)
		{
//cout << "aboveCopy.size="<<aboveCopy.size()<<"\n";
			
			// The pCFGNode that this analysis will transition to after the split
			pCFGNode descN(n);
			performPSetSplit(n, descN, splitPSet, aboveCopy, 
			          splitConditions, splitPSetNodes, splitPSetActive, 
			          func, fState, pCFGState::getNodeState(func, n, this), 
			          activePSets, blockedPSets, releasedPSets, true);
			
			// Propagate the post-split dataflow information from above this pCFGNode to below it, performing
			// any needed unions and widenings. We will then propagate this below information from n to descN.
			// /*modified = */updateLattices(pCFGState::getNodeState(func, n, this)->getLatticeBelowMod(this), 
			//                               pCFGState::getNodeState(func, n, this)->getLatticeAboveMod(this), 
			//                               false)/* || modified*/;

if(analysisDebugLevel>=1)
{
	cout << "runAnalysis_pCFG() D pSetActive =";    for(vector<bool>::iterator it=pSetActive.begin(); it!=pSetActive.end();    it++) { cout << *it << " "; } cout <<"\n";
	cout << "runAnalysis_pCFG() D activePSets ["<<activePSets.size()<<"]=";   for(set<int>::iterator it=activePSets.begin();    it!=activePSets.end();   it++) { cout << *it << " "; } cout <<"\n";
	cout << "runAnalysis_pCFG() D blockedPSets ["<<blockedPSets.size()<<"]=";  for(set<int>::iterator it=blockedPSets.begin();   it!=blockedPSets.end();  it++) { cout << *it << " "; } cout <<"\n";
	cout << "runAnalysis_pCFG() D releasedPSets ["<<releasedPSets.size()<<"]="; for(set<int>::iterator it=releasedPSets.begin();  it!=releasedPSets.end(); it++) { cout << *it << " "; } cout <<"\n";
}
			
			// Activate all the non-split sets for which activation was requested
			int curPSet=0;
			for(vector<bool>::iterator it=pSetActive.begin(); it!=pSetActive.end(); it++, curPSet++)
				if(*it)
				{
					ROSE_ASSERT(movePSet(curPSet, activePSets, blockedPSets));
					releasedPSets.insert(curPSet);
				}
			if(analysisDebugLevel>=1) 
				cout << "runAnalysis_pCFG() pSetActive.size()="<<pSetActive.size()<<"  activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<"\n";
			
			NodeState* curState = pCFGState::getNodeState(func, n, this);
			// The state of the next pCFG node
			NodeState* nextState = pCFGState::getNodeState(func, descN, this);
			//printf("nextState=%p\n", nextState);
			ROSE_ASSERT(nextState);
			
			// Propagate the Lattices below this node to its descendant. /*We propagate to the lattices BELOW the
			//    descendant pCFGNode because the send-receive matching was performed while both the sender and the 
			//    receiver were in the "middle" of the node: blocked on the trasfer function. As such, when the
			//    original node transitions to its descendant, the dataflow info will flow to the point in the node
			//    below the transition function: the lattices BELOW the descN pCFGNode.*/
			/*bool modified = */propagateFWStateToNextNode(func, 
			                                      /*curState->getLatticeBelow(this), n, *curState, 
			                                      nextState->getLatticeAboveMod(this), descN, */
			                                      aboveCopy, n, *curState, 
			                                      nextState->getLatticeAboveMod(this), descN, 
			                                      (ConstrGraph*)NULL, 0);
			
			// Record that we've visited the descendant node since we will be restarting from immediately after 
			// the transfer function would have been called on this node
			visitedPCFGNodes.insert(descN);
			
			// Transition to the next pCFGNode
			n = descN;
			
			// We've now updated the state above the next pCFGNode. If this results in no change in the state 
			// below this node, we'll see this when we try to process it and discover that all of its process
			// sets immediately become blocked. At this point we'll dump this pCFGNode and move on with others.
			
			// !!! Not sure if this is exactly what would happen. We may get into an a situation where after
			// !!! the all-blocked point, we'll run send-receive matching, and either succeed and propagate
			// !!! something nasty or fail and abort. We may need to introduce a special state for nodes
			// !!! on which the analysis has reached a fixed point
			
			// If we transferred the current dataflow state from n to descN and this did not
			// cause the dataflow state to change, we're done with this pCFGNode
			/*if(!modified)
				return true;*/
		}
		
		// Deallocates aboveCopy since we've now performed the split and if needed, propagated 
		// these lattices to the next pCFGNode
		deleteLattices(aboveCopy);
		          
		// If we must split this analysis into multiple partitions, request this split from the parent partitioned analysis
		/*if(splitChkpts.size()>0)
		{
			split(splitChkpts);
			return false;
		}*/
		// Otherwise, continue looping for as long as we still have active partitions
	} while(activePSets.size()>0);
	
	// All process sets are blocked and they cannot be unblocked via send-receive matching.
	
	// Check if this state was reached because all process sets are at the end of the application 
	int curPSet=0;
	for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); 
	    it!=n.getPSetDFNodes().end(); it++, curPSet++)
	{
		// If some process set is not at the end of this function
		if(*it != funcCFGEnd)
		{
			cout << "WARNING: in un-releaseable state process set "<<curPSet<<" is not at function end! Instead, n.pSetDFNodes["<<curPSet<<"]="<<(*it).str()<<"\n";
			//ROSE_ASSERT(0);
			// Checkpoint this analysis. We may return to it later and discover that not all these process states
			// are actually possible.
			if(analysisDebugLevel>=1) 
				cout << "@@@ Shelving this blocked partition for now.\n";
			set<pCFG_Checkpoint*> chkpt;
			chkpt.insert(new pCFG_Checkpoint(n, func, fState, activePSets, blockedPSets));
			split(chkpt);
		}
	}
	
	// At this point the analysis has reached the end of the function and does not need to propagate
	// any dataflow further down this function's pCFG. We will now return without creating a checkpoint.
	

// !!! Need to return something useful
	return true;
}

// Merge any process sets that are at equivalent dataflow states.
// Return true if any process sets were merged, false otherwise.
// If no processes are merged, mergePCFGNodes does not modify mergedN, dfInfo, activePSets, blockedPSets or releasedPSets.
bool pCFG_FWDataflow::mergePCFGNodes(const pCFGNode& n, pCFGNode& mergedN, vector<Lattice*>& dfInfo,
                                     const Function& func, 
                                     set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets)
{
	startProfileFunc("mergePCFGNodes");
	mergedN = n;

if(analysisDebugLevel>=1) 
	cout << "mergePCFGNodes() n="<<n.str()<<"\n";

	// Maps the various Dataflow states that different process sets may be at (combination
	// of DataflowNode and active/blocked status) to the sets of process sets at those states
	map<pair<DataflowNode, bool>, list<int> > setNodes;
	int curPSet=0;
	for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); 
	    it!=n.getPSetDFNodes().end(); it++, curPSet++)
	{
		pair <DataflowNode, bool> p(*it, (activePSets.find(curPSet) != activePSets.end()));
		setNodes[p].push_back(curPSet);
	}
	
	// Check to see if there is more than one process set in any given state
	bool merged=false; // set to true if we do merge some process sets
	for(map<pair<DataflowNode, bool>, list<int> >::iterator itMergSet=setNodes.begin();
	    itMergSet!=setNodes.end(); itMergSet++)
	{
		if(itMergSet->second.size()>1)
		{
			if(analysisDebugLevel>=1)
			{
				cout << "Merging process sets [";
				for(list<int>::iterator itMerge=itMergSet->second.begin(); itMerge!=itMergSet->second.end(); itMerge++)
					cout << *itMerge << " ";
				cout << "]\n";
			}
			
			// The process sets in itMergSet->second are all at the same dataflow state (DataflowNode + active/blocked status).
			
			// Sort process sets to be merged to ensure that the one with the minimum value is first
			// (Keeps things orderly for the sake of debugging)
			itMergSet->second.sort();
			
			// Merge their dataflow information and their DataflowNodes within the pCFGNode.
			// The space of process set IDs will be compressed to remove the holes left by the removal, 
			// with the migrations of process set ids recorded in pSetMigrations.
			map<int, int> pSetMigrations;
			mergePCFGStates(itMergSet->second, n, func, dfInfo, *(pCFGState::getNodeState(func, n, this)), pSetMigrations);
				
			// Update mergedN by removing all the merged process sets, except the one with the minimum value.
			list<int>::iterator itMerged=itMergSet->second.begin();
			itMerged++;
			for(; itMerged!=itMergSet->second.end(); itMerged++)
			{
				if(analysisDebugLevel>=1) 
				{ cout << "Removing merged process set "<<(*itMerged)<<"\n"; fflush(stdout); }
				mergedN.removePSet(*itMerged);
				activePSets.erase(*itMerged);
				blockedPSets.erase(*itMerged);
				releasedPSets.erase(*itMerged);
			}
			
			// Adjust activePSets, blockedPSets and releasedPSets to account for the fact that some
			// process sets have changed ids
			if(analysisDebugLevel>=1) cout << "migrations:\n";
			for(map<int, int>::iterator it=pSetMigrations.begin(); it!=pSetMigrations.end(); it++)
			{
				if(analysisDebugLevel>=1) cout << "    "<<it->first<<" -> "<<it->second<<"\n";
				
				if(activePSets.find(it->first)!=activePSets.end())
				{ 
					ROSE_ASSERT(activePSets.erase(it->first));
					activePSets.insert(it->second);
				}
				if(blockedPSets.find(it->first)!=blockedPSets.end())
				{ 
					ROSE_ASSERT(blockedPSets.erase(it->first));
					blockedPSets.insert(it->second);
				}
				if(releasedPSets.find(it->first)!=releasedPSets.end())
				{ 
					ROSE_ASSERT(releasedPSets.erase(it->first));
					releasedPSets.insert(it->second);
				}
			}
			
			merged = true;
		}
	}
	endProfileFunc("mergePCFGNodes");
	
	return merged;
}

// Adds the set of checkpoints to the overall set of checkpoints currently active
// in this analysis. The caller is required to return immediately to the surrounding
// runAnalysisResume() call.
void pCFG_FWDataflow::split(const set<pCFG_Checkpoint*>& splitChkpts)
{
	for(set<pCFG_Checkpoint*>::const_iterator it=splitChkpts.begin();
	    it!=splitChkpts.end(); it++)
	{
		partitionChkpts.insert(*it);
	}
	
	if(analysisDebugLevel>=1) 
	{
		cout << "pCFG_FWDataflow::split, partitionChkpts.size()="<<partitionChkpts.size()<<"\n";
		for(set<pCFG_Checkpoint*>::const_iterator it=partitionChkpts.begin();
		    it!=partitionChkpts.end(); it++)
		{
			cout << "----"<<(*it)<<" : "<<(*it)->str("    ")<<"\n";
		}
	}
}

// Fills tgtLat with copies of the lattices in srcLat. tgtLat is assumed to be an empty vector
void pCFG_FWDataflow::latticeCopyFill(vector<Lattice*>& tgtLat, const vector<Lattice*>& srcLat)
{
	for(vector<Lattice*>::const_iterator it=srcLat.begin(); it!=srcLat.end(); it++)
	{
		Lattice* lat = (*it)->copy();
		//cout << "latticeCopyFill() *it="<<(*it)<<"  lat="<<lat<<"\n";
		tgtLat.push_back(lat);
	}
}

// Deallocates all the lattices in lats and empties it out
void pCFG_FWDataflow::deleteLattices(vector<Lattice*>& lats)
{
	for(vector<Lattice*>::iterator it=lats.begin(); it!=lats.end(); it++)
		delete *it;
	lats.clear();
}

// Call the analysis transfer function. 
// If the transfer function wants to block on the current DataflowNode, sets blockPSet to true.
// If the transfer function wants to split a process set, resulting in a new pCFGNode that contains more 
//    process sets, advances descN to the new pCFGNode and sets splitPSet to true. It is assumed that
//    at the start of the function n == descN.
// If the transfer function wants to split across multiple descendant pCFGNodes, partitionTranfer() 
//    generatesthe resulting checkpoints, performs the split and sets splitPNodes to true.
// Otherwise, sets neither to true.
bool pCFG_FWDataflow::partitionTranfer(
                           const pCFGNode& n, pCFGNode& descN, int curPSet, const Function& func, NodeState* fState, 
                           const DataflowNode& dfNode, NodeState* state, 
                           vector<Lattice*>& dfInfoBelow, vector<Lattice*>& dfInfoNewBelow, 
                           bool& deadPSet, bool& splitPSet, bool& splitPNode, bool& blockPSet, 
                           set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets,
                           const DataflowNode& funcCFGEnd)
{
	startProfileFunc("partitionTranfer");
	
	bool modified = false;
	vector<ConstrGraph*> splitConditions;
	vector<DataflowNode> splitPSetNodes;
	splitPSet = false;
	splitPNode = false;
	blockPSet = false;
	
	
	ROSE_ASSERT(n == descN);
	
	modified = transfer(n, curPSet, func, *state, /*dfInfoBelow*/dfInfoNewBelow, deadPSet, splitPSet, splitPSetNodes, 
	                    splitPNode, splitConditions, blockPSet) || modified;
	if(analysisDebugLevel>=1) 
		cout << "deadPSet="<<deadPSet<<" splitPSet="<<splitPSet<<" splitPNode="<<splitPNode<<" blockPSet="<<blockPSet<<"\n";
	if(deadPSet)
	{
		endProfileFunc("partitionTranfer");
		return modified;
	}
	
	ROSE_ASSERT((!splitPSet && !splitPNode && !blockPSet) ||
	            XOR(XOR(splitPSet, splitPNode), blockPSet));
		
	// If we're not going to block on this node, update the state below it.
	// (if we are going to block, we'll just dump the current dfInfoNewBelow 
	//  and move on to the next process set)
	if(!blockPSet && n.getCurNode(curPSet) != funcCFGEnd)
		// Incorporate dfInfoNewBelow(the copy of the information above the node, after the transfer function) into dfInfoBelow,
		// the information below the node. Also, DEALLOCATE the lattices in dfInfoNewBelow since they have now served their purpose
		// of ferrying data from dfInfoAbove, through the transfer function, to dfInfoBelow
		modified = updateLattices(dfInfoBelow, dfInfoNewBelow, true) || modified;
		// !!! Note: this assumes that if the transfer function blocks a process set, it is ok to drop all of the
		// !!! changes that it may have made to the dataflow state.
	
	// If the analysis wants to be split (i.e. its dataflow state propagated to multiple pCFGNodes), perform the split
	if(splitPNode)
	{
		if(analysisDebugLevel>=1)
		{ printf("    Splitting analysis process set %d\n", curPSet); }
		//int i=0;
		set<pCFG_Checkpoint*> splitChkpts;
		
		// The descendant DataflowNodes of curPSet
		vector<DataflowEdge> edges = dfNode.outEdges();
		
		// There is one condition for each descendant
		ROSE_ASSERT(edges.size() == splitConditions.size());
		
		vector<DataflowEdge>::iterator edgeIt;
		vector<ConstrGraph*>::iterator splitIt;
		for(splitIt=splitConditions.begin(), edgeIt=edges.begin();
		    splitIt!=splitConditions.end() && edgeIt!=edges.end(); 
		    splitIt++, edgeIt++)
		{
			if(analysisDebugLevel>=1) 
			{
				cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n";
				cout << "&&& partitionTranfer() Creating new Descendant node &&&\n";
			}

			// The pCFG node that corresponds to curPSet advancing along the current outgoing edge
			pCFGNode descN(n);
			descN.advance(curPSet, (*edgeIt).target());
			pCFG_Checkpoint* newChkpt = new pCFG_Checkpoint(descN, func, fState, activePSets, blockedPSets, releasedPSets, curPSet, *splitIt);
			// Add the checkpoint that corresponds to the next split to the vector of checkpoints
			splitChkpts.insert(newChkpt);
			
			// Propagate this pCFGNode's dataflow info to the current descendant
			// The state of the next pCFG node
			NodeState* nextState = pCFGState::getNodeState(func, descN, this);
			ROSE_ASSERT(nextState);
				
			if(analysisDebugLevel>=1) 
				cout << "nextState="<<nextState<<"\n";
			// Propagate the Lattices below this node to its descendant
			/*modified = */propagateFWStateToNextNode(func, dfInfoBelow, n, *state, 
			                                          nextState->getLatticeAboveMod(this), descN,
			                                          *splitIt, curPSet)/* || modified*/;
		
			/*if(analysisDebugLevel>=1)
			{ printf("        Split condition %d: \n%s\n", i, ((*splitIt)->str("            ")).c_str()); }*/
			if(analysisDebugLevel>=1) 
			{
				cout << "newChkpt = "<<newChkpt->str()<<"\n";
				cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n";
			}
		}
		
		split(splitChkpts);
		// After the split, we'll break out of the analysis. The parent PartitionedAnalysis will 
		// then schedule the analysis on each descendant pCFG node appropriately.
	}
	// Else, if we've added new process sets and both the current and the new process sets will 
	// advance to the DataflowNodes in splitPSetNodes
	else if(splitPSet)
	{
		// All the split sets will be active 
		vector<bool> splitPSetActive;
		for(vector<ConstrGraph*>::iterator splitIt=splitConditions.begin();
		    splitIt!=splitConditions.end(); splitIt++)
			splitPSetActive.push_back(true);

		performPSetSplit(n, descN, curPSet, dfInfoBelow, splitConditions, splitPSetNodes, splitPSetActive, 
		                 func, fState, state, activePSets, blockedPSets, releasedPSets, false);
	}
	
	endProfileFunc("partitionTranfer");
	
	return modified;
}

// Updates the lattices in tgtLattices from the lattices in srcLattices. For a given lattice pair <t, s>
//    (t from tgtLattices, s from srcLattices), t = t widen (t union s).
// If delSrcLattices==true, deletes all the lattices in srcLattices.
// Returns true if this causes the target lattices to change, false otherwise.
bool pCFG_FWDataflow::updateLattices(vector<Lattice*>& tgtLattices, vector<Lattice*>& srcLattices, bool delSrcLattices)
{
	bool modified = false;
	
	// Incorporate srcLattices into tgtLattices, the information below the node.
	vector<Lattice*>::iterator itTgt, itSrc;
	int i=0;
	for(itTgt=tgtLattices.begin(), itSrc=srcLattices.begin();
	    itTgt!=tgtLattices.end() && itSrc!=srcLattices.end(); 
	    itTgt++, itSrc++, i++)
	{
		if(analysisDebugLevel>=1) 
		{
			cout << "updateLattices() Lattice "<<i<<" Below Info:"<<(*itTgt)->str("      ")<<"\n";
			cout << "updateLattices() Lattice "<<i<<" New Transferred Below Info:"<<(*itSrc)->str("      ")<<"\n";
		}
		
		// Finite Lattices can use the regular meet operator, while infinite Lattices
		// must also perform widening to ensure convergence.
		if((*itTgt)->finiteLattice())
		{
			modified = (*itTgt)->meetUpdate(*itSrc) || modified;
			if(analysisDebugLevel>=1) 
				cout << "updateLattices() Lattice "<<i<<" After Meet Below Info:"<<(*itTgt)->str("      ")<<"\n";
		}
		else
		{
			(*itSrc)->meetUpdate(*itTgt);
			if(analysisDebugLevel>=1) 
				cout << "updateLattices() Lattice "<<i<<" After Meet Below Info:"<<(*itSrc)->str("      ")<<"\n";
			modified =  dynamic_cast<InfiniteLattice*>(*itTgt)->widenUpdate(dynamic_cast<InfiniteLattice*>(*itSrc)) || modified;
			if(analysisDebugLevel>=1) 
				cout << "updateLattices() Lattice "<<i<<" After Widening Below Info:"<<(*itTgt)->str("      ")<<"\n";
		}
		
		// Delete the current lattice in srcLattices
		if(delSrcLattices)
			delete *itSrc;
	}
	
	if(analysisDebugLevel>=1)
	{
		if(modified)
			cout << "updateLattices() Lattice information Below MODIFIED\n";
		else
			cout << "updateLattices() Lattice information Below NOT Modified\n";
	}
	
	return modified;
}

// Split the process set pSet in pCFGNOde n, resulting in a new pCFGNode that contains more 
//    process sets. Advances descN to be this new pCFGNode and updates activePSets to make
//    the newly-created process sets active. It is assumed that at the start of the function n == descN.
// splitConditions - set of logical conditions that apply to each split process set
// splitPSetNodes - the DataflowNodes that each split process set starts at.
// if freshPSet==true, each partition will get a completely new process set and thus
//    must be set fresh. If freshPSet==false, each partition's process set is simply
//    an updated version of the old process set (i.e. an extra condition is applied).
void pCFG_FWDataflow::performPSetSplit(const pCFGNode& n, pCFGNode& descN, int pSet, 
                                vector<Lattice*>& dfInfo,
                                vector<ConstrGraph*> splitConditions, vector<DataflowNode> splitPSetNodes,
                                vector<bool>& splitPSetActive,
                                const Function& func, NodeState* fState, NodeState* state, 
                                set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets,
                                bool freshPSet)
{
	startProfileFunc("performPSetSplit");
	
	ROSE_ASSERT(splitPSetNodes.size()>=2);
	
	if(analysisDebugLevel>=1) 
		cout << "performPSetSplit(pSet="<<pSet<<") splitPSetNodes.size()="<<splitPSetNodes.size()<<"\n";
	// Update descN to correspond to the new pCFGNode that results from the creation
	// of the new process sets.
	vector<int> newPSets; // The newly-generated process sets
	descN.advance(pSet, splitPSetNodes[0]); // pSet moves on to the first DataflowNode in splitPSetNodes
	// Other process sets get the other DataflowNodes in splitPSetNodes
	vector<DataflowNode>::iterator splitDFIt=splitPSetNodes.begin(); splitDFIt++;
	for(; splitDFIt!=splitPSetNodes.end(); splitDFIt++)
		newPSets.push_back(descN.createPSet(*splitDFIt));
	
	if(analysisDebugLevel>=1) 
		cout << "descN="<<descN.str()<<"\n";
	/*cout << "newPSets ("<<newPSets.size()<<")= ";
	for(set<int>::iterator it=newPSets.begin(); it!=newPSets.end(); it++)
		cout << *it << " ";
	cout << "\n";*/
	
	// Get the state of the descendant pCFGNode
	// NodeState* descState = pCFGState::getNodeState(func, descN, this);
	
	if(analysisDebugLevel>=1)
	{
		cout << "^^^^^^^^^^\n";
		cout << "performPSetSplit: Creating New Process Set Conditions from process set "<<pSet<<"\n";
	}
	
	// If each partition will get fresh process set bounds information, reset pSet's 
	//    bounds information from cg before doing anything else so that
	// 1. pSet does get fresh bounds and 
	// 2. other process sets' state is not polluted with pSet's stale bounds
	if(freshPSet)
		resetPSet(pSet, dfInfo);
	
	// Copy the state from pSet to all the newly-generated process sets
	// and activate these process sets
	vector<ConstrGraph*>::iterator splitCondIt = splitConditions.begin(); splitCondIt++;
	vector<bool>::iterator activeIt            = splitPSetActive.begin(); activeIt++;
	if(analysisDebugLevel>=1) 
		cout << "newPSets.size()="<<newPSets.size()<<"\n";
	//cout << "newPSets.size()="<<newPSets.size()<<" splitConditions.size()="<<splitConditions.size()<<" splitPSetActive.size()="<<splitPSetActive.size()<<"\n";
	ROSE_ASSERT(((newPSets.size()+1) == splitConditions.size()) && (splitConditions.size() == splitPSetActive.size()));
	for(vector<int>::iterator it=newPSets.begin(); 
	    it!=newPSets.end() && splitCondIt!=splitConditions.end() && activeIt!=splitPSetActive.end(); 
	    it++, splitCondIt++, activeIt++)
	{
		int newPSet = *it;
		// if the current split process set is supposed to be active, activate it
		if(analysisDebugLevel>=1) 
			cout << "performPSetSplit() splitPSetActive[newPSet="<<newPSet<<"]="<<(*activeIt)<<" activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<"\n";
		if(*activeIt)
			activePSets.insert(newPSet);
		else
			blockedPSets.insert(newPSet);
			
		// Create the new process set's state as a copy of pSet's state with the additional info in *splitCondIt
		copyPSetState(func, descN, pSet, newPSet, *state,
                    dfInfo, state->getFactsMod((Analysis*)this), *splitCondIt, freshPSet);
      
      delete *splitCondIt;
	}
	if(analysisDebugLevel>=1) 
		cout << "performPSetSplit() freshPSet="<<freshPSet<<" activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<"\n";
	
	// Update the state of pSet from its condition
	initPSetDFfromPartCond(func, n, pSet, dfInfo, state->getFactsMod((Analysis*)this), *(splitConditions.begin()));
	if(analysisDebugLevel>=1) 
		cout << "performPSetSplit() splitPSetActive[pSet="<<pSet<<"]="<<*(splitPSetActive.begin())<<" activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<"\n";
	if(*(splitPSetActive.begin()))
	{
		bool wasBlocked = movePSet(pSet, activePSets, blockedPSets, true);
		if(wasBlocked) releasedPSets.insert(pSet);
	}
	delete *(splitConditions.begin());
	if(analysisDebugLevel>=1) 
		cout << "performPSetSplit() activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<"\n";
	
	endProfileFunc("performPSetSplit");
	
	/*cout << "^^^^^^^^^^\n";
	cout << "partitionTranfer: Initializing New Process Set Conditions\n";
	
	// Update each process set's state from its respective split condition
	vector<ConstrGraph*>::iterator splitCondIt;
	int pSetIndex=-1;
	for(splitDFIt=splitPSetNodes.begin(), splitCondIt=splitConditions.begin();
	    splitDFIt!=splitPSetNodes.end() && splitCondIt!=splitConditions.end(); 
	    splitDFIt++, splitCondIt++, pSetIndex++)
	{ 
		int pSet;
		if(pSetIndex==-1) pSet = pSet;
		else              pSet = newPSets[pSetIndex];
			
		initDFfromPartCond(func, n, pSet, *state, dfInfoBelow, state->getFactsMod((Analysis*)this), *splitCondIt);
	}*/
}

// Propagates the dataflow info from the current node's NodeState (curNodeLattices) to the next node's 
//     NodeState (nextNodeLattices).
// Returns true if the next node's meet state is modified and false otherwise.
/*bool */ void pCFG_FWDataflow::propagateFWStateToNextNode(const Function& func, 
                      const vector<Lattice*>& curNodeLattices, const pCFGNode& curNode, const NodeState& curNodeState, 
                      vector<Lattice*>& nextNodeLattices, const pCFGNode& nextNode,
                      ConstrGraph* partitionCond, int pSet)
{
	
	//bool modified = false;
	vector<Lattice*>::const_iterator itC;
	vector<Lattice*>::iterator       itN;
	if(analysisDebugLevel>=1)
	{
		if(analysisDebugLevel>=1)
		{
			cout << "\n        Propagating to Next Node: "<<nextNode.str("        ")<<"\n";
			cout << "nextNodeLattices.size()="<<nextNodeLattices.size()<<", curNodeState="<<&curNodeState<<"  \n";
		}
		int j=0;
		for(itC=curNodeLattices.begin(), itN=nextNodeLattices.begin(); 
		    itC!=curNodeLattices.end() && itN!=nextNodeLattices.end(); itC++, itN++, j++)
		{
			if(analysisDebugLevel>=1)
			{
				cout << "        Current node below: Lattice "<<j<<": "<<(*itC)->str("            ")<<"\n";
				cout << "        Next node above: Lattice "<<j<<": "<<(*itN)->str("            ")<<"\n";
			}
			/*cout << "-----\n"; fflush(stdout);
			ConstrGraph* cg = dynamic_cast<ConstrGraph*>(*itN);
			cout << "cg = "<<cg<<"\n";
			varID nprocsVar("nprocsVar");
			affineInequality* val = cg->getVal(zeroVar, nprocsVar);
			cout << "val = "<<val<<"-----\n"; fflush(stdout);
			cout << "        Next node above: Lattice "<<j<<": "<<(*itN)->str("            ")<<"\n";*/
		}
	}
	// If we need to apply a partition condition to the meet lattices, use the 
	// initPSetDFfromPartCond() function provided by the specific analysis
	vector<Lattice*> curNodeLattices_withPartCond;
	if(analysisDebugLevel>=1) 
		cout << "partitionCond = "<<partitionCond<<"\n";
	if(partitionCond)
	{
		for(vector<Lattice*>::const_iterator it=curNodeLattices.begin();
		    it!=curNodeLattices.end(); it++)
			curNodeLattices_withPartCond.push_back((*it)->copy());
		
		// Update the state of pSet from partitionCond
		/*modified = */initPSetDFfromPartCond(func, curNode, pSet,  
		                                  curNodeLattices_withPartCond, curNodeState.getFacts((Analysis*)this),
		                                  partitionCond)/* || modified*/;
	}
	else
	{
		curNodeLattices_withPartCond = curNodeLattices;
	}
	

	// Update forward info above nextNode from the forward info below curNode.
	
	// Compute the meet of the dataflow information along the curNode->nextNode edge with the 
	// next node's current state one Lattice at a time and save the result above the next node.
	for(itC=curNodeLattices_withPartCond.begin(), itN=nextNodeLattices.begin();
	    itC!=curNodeLattices_withPartCond.end() && itN!=nextNodeLattices.end(); 
	    itC++, itN++)
	{
		(*itN)->copy(*itC);
/*cout << "Propagated: itC="<<(*itC)->str()<<"\n";		
cout << "Propagated: itN="<<(*itN)->str()<<"\n";*/

		/*
		// Finite Lattices can use the regular meet operator, while infinite Lattices
		// must also perform widening to ensure convergence.
		if((*itN)->finiteLattice())
		{
			modified = (*itN)->meetUpdate(*itC) || modified;
		}
		else
		{
			//InfiniteLattice* meetResult = (InfiniteLattice*)itN->second->meet(itC->second);
			InfiniteLattice* meetResult = dynamic_cast<InfiniteLattice*>((*itN)->copy());
			meetResult->meetUpdate(*itC);
			
			cout << "        Infinite Lattices\n";
			cout << "            *itN: " << dynamic_cast<InfiniteLattice*>(*itN)->str("            ") << "\n";
			cout << "            *itC: " << dynamic_cast<InfiniteLattice*>(*itC)->str("            ") << "\n";
			cout << "            meetResult: " << meetResult->str("") << "\n";
			
			// widen the resulting meet
			modified =  dynamic_cast<InfiniteLattice*>(*itN)->widenUpdate(meetResult);
			delete meetResult;* /
			
			cout << "        Infinite Lattices\n";
			cout << "            *itN: " << dynamic_cast<InfiniteLattice*>(*itN)->str("            ") << "\n";
			cout << "            *itC: " << dynamic_cast<InfiniteLattice*>(*itC)->str("            ") << "\n";
			modified =  dynamic_cast<InfiniteLattice*>(*itN)->meetUpdate((*itC);
			cout << "            meetResult: " << (*itN)->str("") << "\n";
		}*/
	}

	// If we had to generate copies of the curNodeLattices to incorporate the partition condition
	// into those lattices, delete these objects
	if(partitionCond)
	{
		for(vector<Lattice*>::iterator it=curNodeLattices_withPartCond.begin();
		    it!=curNodeLattices_withPartCond.end(); it++)
			delete *it;
	}
	
	
	/*if(analysisDebugLevel>=1){
		if(modified)
		{
			cout << "        Next node's in-data modified.\n";
			int j=0;
			for(itN=nextNodeLattices.begin(); itN!=nextNodeLattices.end(); itN++, j++)
			{
				cout << "        Propagated: Lattice "<<j<<": \n"<<(*itN)->str("            ")<<"\n";
			}
		}
		else
			cout << "        No modification on this node\n";
	}*/

	//return modified;
}


// Moves the given process set from srcPSets to destPSets. If noSrcOk==false, does nothing
// if pSet is not inside srcPSets. If noSrcOk==true, adds pSet to destPSets regardless.
// Returns true if the process set was in srcPSets and false otherwise.
bool pCFG_FWDataflow::movePSet(int pSet, set<int>& destPSets, set<int>& srcPSets, bool noSrcOk)
{
	// if pSet was in fact in src
	if(srcPSets.erase(pSet)>0)
	{
		if(analysisDebugLevel>=1) cout << "movePSet("<<pSet<<") Moving\n";
		// Move it to destPSets
		destPSets.insert(pSet);
		return true;
	}
	else if(noSrcOk)
	{
		if(analysisDebugLevel>=1) cout << "movePSet("<<pSet<<") Inserting\n";
		// Insert it into destPSets
		destPSets.insert(pSet);
	}
	else
	{
		if(analysisDebugLevel>=1) cout << "movePSet("<<pSet<<") Noop\n";
	}
	
	return false;
}

