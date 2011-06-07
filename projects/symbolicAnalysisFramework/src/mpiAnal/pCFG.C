// ??? : As we move around the pCFG are we keeping the mapping of numeric process set id to 
// ??? : DataflowNodes consistent? This is important for merging states every time we visit the same pCFGNode

// ??? : Are we correctly managing our understanding of which process sets are non-empty? Right now we assume 
// ??? : that when we perform a send-receive match that initially both the senders and receivers sets are non-empty
// ??? : and that mesgExpr::getIdentityDomain() ensures that after matching the sets of matched senders and receivers
// ??? : are non-empty while the non-matched process sets may be empty. Is this always correct?

#include "pCFG.h"
#include <sys/time.h>
static int profileLevel=1;

#define startProfileFunc(funcName, indent)     \
	struct timeval startTime, endTime;  \
	if(profileLevel>=1)                 \
	{                                   \
		Dbg::dbg << indent << funcName << "() {{{ "<<endl; \
		gettimeofday(&startTime, NULL);  \
	}

#define endProfileFunc(funcName, indent)                                                                                                                         \
	if(profileLevel>=1)                                                                                                                                   \
	{                                                                                                                                                     \
		gettimeofday(&endTime, NULL);                                                                                                                      \
		Dbg::dbg << indent << funcName << "() }}} time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<endl; \
	}

void printPSetStatus(ostream& out, const pCFGNode& n, const set<unsigned int>& activePSets, const set<unsigned int>& blockedPSets, const set<unsigned int>& releasedPSets, string indent="")
{
	out << indent << "activePSets = [";
	for(set<unsigned int>::const_iterator curPSet=activePSets.begin(); curPSet!=activePSets.end(); ) { out << *curPSet; curPSet++; if(curPSet!=activePSets.end()) out << ", "; }
	out << "]\n";
	for(set<unsigned int>::const_iterator curPSet=activePSets.begin(); curPSet!=activePSets.end(); curPSet++) {
		const DataflowNode& dfNode = n.getCurNode(*curPSet);
		out << "    rank "<<*curPSet << ": &lt;" << dfNode.getNode()->class_name() << " | " << Dbg::escape(dfNode.getNode()->unparseToString()) << "&gt;" << endl;
	}
	
	out << indent << "blockedPSets = [";
	for(set<unsigned int>::const_iterator curPSet=blockedPSets.begin(); curPSet!=blockedPSets.end(); ) { out << *curPSet; curPSet++; if(curPSet!=blockedPSets.end()) out << ", "; }
	out << "]\n";
	for(set<unsigned int>::const_iterator curPSet=blockedPSets.begin(); curPSet!=blockedPSets.end(); curPSet++) {
		const DataflowNode& dfNode = n.getCurNode(*curPSet);
		out << "    rank "<<*curPSet << ": &lt;" << dfNode.getNode()->class_name() << " | " << Dbg::escape(dfNode.getNode()->unparseToString()) << "&gt;" << endl;
	}
	
	out << indent << "releasedPSets = [";
	for(set<unsigned int>::const_iterator curPSet=releasedPSets.begin(); curPSet!=releasedPSets.end(); ) { out << *curPSet; curPSet++; if(curPSet!=releasedPSets.end()) out << ", "; }
	out << "]\n";
	for(set<unsigned int>::const_iterator curPSet=releasedPSets.begin(); curPSet!=releasedPSets.end(); curPSet++) {
		const DataflowNode& dfNode = n.getCurNode(*curPSet);
		out << "    rank "<<*curPSet << ": &lt;" << dfNode.getNode()->class_name() << " | " << Dbg::escape(dfNode.getNode()->unparseToString()) << "&gt;" << endl;
	}
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
	
	// Give pSetDFNodes a canonical ordering
	sort(this->pSetDFNodes.begin(), this->pSetDFNodes.end());
}

void pCFGNode::operator = (const pCFGNode& that)
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
void pCFGNode::advance(unsigned int pSet, const DataflowNode& n)
{
	ROSE_ASSERT(0<=pSet && pSet<pSetDFNodes.size());
	pSetDFNodes[pSet] = n;
}

// Advances the given process set to the only outgoing descendant of its current dataflow node
void pCFGNode::advanceOut(unsigned int pSet)
{
	ROSE_ASSERT(0<=pSet && pSet<pSetDFNodes.size());
	vector<DataflowEdge> edges = pSetDFNodes[pSet].outEdges();
	ROSE_ASSERT(edges.size()==1);
	pSetDFNodes[pSet] = (*(edges.begin())).target();
}

// Advances the given process set to the only incoming descendant of its current dataflow node
void pCFGNode::advanceIn(unsigned int pSet)
{
	ROSE_ASSERT(0<=pSet && pSet<pSetDFNodes.size());
	vector<DataflowEdge> edges = pSetDFNodes[pSet].inEdges();
	ROSE_ASSERT(edges.size()==1);
	pSetDFNodes[pSet] = (*(edges.begin())).source();
}

// Returns the given process set's current DataflowNode
const DataflowNode& pCFGNode::getCurNode(unsigned int pSet) const
{
	ROSE_ASSERT(0<=pSet && pSet<pSetDFNodes.size());
	return pSetDFNodes[pSet];
}

const vector<DataflowNode>& pCFGNode::getPSetDFNodes() const
{
	return pSetDFNodes;
}

// Removes the given process set from this pCFGNode, pSets with numbers above pSet in this pCFGNode get 
// renumbered to fill the hole created by the deletion: newPSet = oldPSet-1.
void pCFGNode::removePSet(unsigned int pSet)
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
	outs << "[pCFGNode : "<<endl;
	for(vector<DataflowNode>::const_iterator it=pSetDFNodes.begin(); it!=pSetDFNodes.end(); )
	{
		outs << indent << "    [" << (*it).getNode()->class_name() << " | " << (*it).getNode()->unparseToString() << " | " << (*it).getIndex() << "]";
		it++;
		if(it!=pSetDFNodes.end())
			outs << endl;
	}
	outs << "]";
	return Dbg::escape(outs.str());
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
	//if(analysisDebugLevel>=1) Dbg::dbg << "pCFGState::getNodeState() n="<<n.str()<<endl;
	
	// If the given pCFG node has already been visited
	map<pCFGNode, NodeState*>::iterator loc = nodeStateMap.find(n);
	if(loc != nodeStateMap.end())
	{
		//if(analysisDebugLevel>=1) Dbg::dbg << "    node found, initialized="<<loc->second->isInitialized((Analysis*)intraAnalysis)<<endl;
		
		// If the given analysis has registered data at this node, return the node as it is
		// Otherwise, have the analysis initialize its state at this node
		if(!(loc->second->isInitialized((Analysis*)intraAnalysis)))
			initNodeState(func, n, loc->second, intraAnalysis);
		return loc->second;
	}
	else
	{
		// if(analysisDebugLevel>=1) Dbg::dbg << "    node NOT found"<<endl;
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
	//Dbg::dbg << "pCFGState::initNodeState"<<endl;
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
	                              const set<unsigned int>& activePSets, const set<unsigned int>& blockedPSets, const set<unsigned int>& releasedPSets, 
	                              bool isSplit, unsigned int splitPSet,
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
                                 const set<unsigned int>& activePSets, const set<unsigned int>& blockedPSets, const set<unsigned int>& releasedPSets, unsigned int splitPSet,
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
                                 set<unsigned int> activePSets, set<unsigned int> blockedPSets) : n(n), func(func)
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
	return splitCondition->meetUpdate(that.splitCondition, "    ");
}
	
// Move the given process set from blocked to active status
void pCFG_Checkpoint::resumePSet(unsigned int pSet)
{
	ROSE_ASSERT(blockedPSets.find(pSet)!=blockedPSets.end());
	blockedPSets.erase(pSet);
	activePSets.insert(pSet);
	releasedPSets.insert(pSet);
}

string pCFG_Checkpoint::str(string indent) const
{
	ostringstream outs;
	outs << /*indent << */"[pCFG_Checkpoint : "<<endl; //fflush(stdout);
	outs << indent << "    n = " << n.str(indent+"    ") << endl;
	
	printPSetStatus(outs, n, activePSets, blockedPSets, releasedPSets, indent);
	
	if(isSplit)
	{
		outs << indent << "    isSplit = " << isSplit << endl;
		outs << indent << "    splitPSet = " << splitPSet << endl;
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
// NOTE: !!!There is currently no support for connecting this analysis to inter-procedural analyses!!!
bool pCFG_FWDataflow::runAnalysis(const Function& func, NodeState* fState, bool analyzeDueToCallers, set<Function> calleesUpdated)
{
	bool ret=false;
	
	pCFG_Checkpoint* curChkpt=NULL;

	// Keep processing partitions until dataflow along the entire pCFG has reached a fixed point
	do
	{
		if(analysisDebugLevel>=1) 
			Dbg::dbg << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
		// If there are any checkpoints left, pick one and continue the analysis from its pCFG node
		if(partitionChkpts.size()>0)
		{
			curChkpt = *(partitionChkpts.begin());
			partitionChkpts.erase(partitionChkpts.begin());
			if(analysisDebugLevel>=1) 
				Dbg::dbg << "@@@ RESUMING partition from checkpoint "<<curChkpt->str("@@@    ")<<endl;
		}
		else
		{
			if(analysisDebugLevel>=1) 
				Dbg::dbg << "@@@ Starting FRESH partition"<<endl;
		}
		
		// Run the analysis on the current partition. This function updates partitionChkpts so that after the call
		// returns, it contains the entire working set of the analysis. As such, curChkpt may be ignored after this call.
		runAnalysis_pCFG(func, fState, curChkpt);
		Dbg::dbg << "partitionChkpts.size()="<<partitionChkpts.size()<<endl;
		cout << "partitionChkpts.size()="<<partitionChkpts.size()<<endl;

		if(analysisDebugLevel>=1) 
			Dbg::dbg << "@@@ Partition reached BLOCKED point or SPLIT. partitionChkpts.size()="<<partitionChkpts.size()<<endl;
		// The current partition has reached a blocked point
		
		// If there are currently multiple active partitions, try to condense partitions
		// that are blocked at the same pCFG node
		if(partitionChkpts.size()>1)
		{
			// Set of partition checkpoints after aggregation
			set<pCFG_Checkpoint*> newPartitions;
			
	/*Dbg::dbg << "pCFG_FWDataflow::runAnalysis, partitionChkpts.size()="<<partitionChkpts.size()<<endl;
	for(set<pCFG_Checkpoint*>::const_iterator it=partitionChkpts.begin();
	    it!=partitionChkpts.end(); it++)
	{
		Dbg::dbg << "----"<<(*it)->str("    ")<<endl;
	}*/
			Dbg::dbg << "Condensing partitions"<<endl;
			// See if we can condense any pair of partitions into a single partition
			set<pCFG_Checkpoint*>::iterator itA=partitionChkpts.begin();
			while(itA!=partitionChkpts.end())
			{
				//Dbg::dbg << "runAnalysis() Partition Condense: (*itA) = "<<(*itA)->str()<<endl;
				const pCFGNode& nA = (*itA)->n;
				Dbg::dbg << "    runAnalysis() Partition Condense: nA = "<<nA.str()<<endl;
				
				// Set of partitions that are equivalent to this one;
				set<pCFG_Checkpoint*> equivParts;
				for(set<pCFG_Checkpoint*>::iterator itB=itA; itB!=partitionChkpts.end(); itB++)
				{
					//Dbg::dbg << "runAnalysis() Partition Condense: (*itB) = "<<(*itB)->str()<<endl;
					const pCFGNode& nB = (*itB)->n;
					Dbg::dbg << "        runAnalysis() Partition Condense: nB = "<<nB.str()<<endl;
					
					// If the two partitions are different but are currently at the same pCFG node
					if(itA!=itB && nA==nB)
					{
						// The two checkpoints must be equivalent (same execution state but possibly 
						// different dataflow states)
						ROSE_ASSERT((*itA)->equiv(*(*itB)));
						// Record that these checkpoints are equivalent
						Dbg::dbg << "            Checkpoints Equivalent."<<endl;
						equivParts.insert(*itB);
					}
				}
				
				Dbg::dbg << "#equivParts = "<<equivParts.size()<<endl;
				
				// If the current checkpoint *itA is equivalent to multiple other checkpoints, 
				// merge them all into a single checkpoint
				if(equivParts.size()>0)
				{
					Dbg::dbg << "@@@ Merging equivalent partitions:"<<endl;
					Dbg::dbg << "    0: "<<(*itA)->str("        ")<<endl;
					
					int i=1;
					for(set<pCFG_Checkpoint*>::iterator itE=equivParts.begin(); itE!=equivParts.end(); itE++, i++)
					{
						//Dbg::dbg << "    "<<i<<": "<<(*itE)->str("        ")<<endl;
						// Merge the current equivalent checkpoint into *itA (i.e. merge 
						// their dataflow states) and delete it
						(*itA)->mergeUpd(*(*itE));
						partitionChkpts.erase(*itE);
						delete *itE;
					}
					//Dbg::dbg <<"     ===>"<<endl;
					if(analysisDebugLevel>=1) 
						Dbg::dbg <<"         "<<(*itA)->str("        ")<<endl;
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
			Dbg::dbg << "@@@ Partitions CONDENSED. #partitionChkpts="<<partitionChkpts.size()<<endl;
	}
	while(partitionChkpts.size()>0);

	// If there are no more checkpoints left, the dataflow analysis has reached a 
	// fixed point on the pCFG
	Dbg::dbg << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
	Dbg::dbg << "@@@@@@@@@@@@         pCFG FIXED POINT         @@@@@@@@@@@@@@@"<<endl;
	Dbg::dbg << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;

	// Return true if any partition caused the function's final dataflow state to change
	// and false otherwise.
//printf("    returning %d\n", ret);


	// We're truly done with this function
/*	Dbg::dbg << "(*(NodeState::getNodeStates(funcCFGEnd).begin()))->getLatticeAbove(this) == fState->getLatticeBelow(this):"<<endl;
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
// Updates partitionChkpts with the partitions that result from this fragment of analysis:
//    - If it terminates at the end of the function, nothing is added. 
//    - Alternately, it may split the current checkpoint, adding the split partitions to partitionChkpts.
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
		Dbg::dbg << marker << ": watchState="<<watchState<<endl; fflush(stdout);
		vector<Lattice*>& watchDfInfoBelow  = watchState->getLatticeBelowMod(watchObj);	
		Lattice* watchLatBelow = *(watchDfInfoBelow.begin());
		Dbg::dbg << marker << ": watchLatBelow="<<watchLatBelow<<endl; fflush(stdout);
		ConstrGraph* watchCGBelow = dynamic_cast<ConstrGraph*>(watchLatBelow);
		Dbg::dbg << marker << ": watchCGBelow="<<watchCGBelow<<endl; fflush(stdout);
		
		vector<Lattice*>& watchDfInfoAbove  = watchState->getLatticeAboveMod(watchObj);	
		Lattice* watchLatAbove = *(watchDfInfoAbove.begin());
		Dbg::dbg << marker << ": watchLatAbove="<<watchLatAbove<<endl; fflush(stdout);
		ConstrGraph* watchCGAbove = dynamic_cast<ConstrGraph*>(watchLatAbove);
		Dbg::dbg << marker << ": watchCGAbove="<<watchCGAbove<<endl; fflush(stdout);
	}	
}*/

bool pCFG_FWDataflow::runAnalysis_pCFG(const Function& func, NodeState* fState, pCFG_Checkpoint* chkpt)
{
	string indent="";
	DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(func.get_definition());
	DataflowNode funcCFGEnd = cfgUtils::getFuncEndCFG(func.get_definition());
	
	ostringstream funcNameStr; funcNameStr << "Function "<<func.get_name().getString()<<"()";
	bool enteredFunction=false;
	if(analysisDebugLevel>=1 && chkpt==NULL) {
		Dbg::enterFunc(funcNameStr.str());
		enteredFunction = true;
		Dbg::dbg << indent <<"Entering "<<funcNameStr.str()<<endl;
		cout << indent <<"Entering "<<funcNameStr.str()<<endl;
	}
	
	/*if(chkpt==NULL)
	{
		// initialize the function's entry NodeState 
		NodeState* entryState = *(NodeState::getNodeStates(funcCFGStart).begin());
		//printf("before copyLattices on [%s | %s]\n", funcCFGStart.getNode()->class_name().c_str(), funcCFGStart.getNode()->unparseToString().c_str());
		NodeState::copyLattices_aEQa(this, *entryState, interAnalysis* this, *fState);
	}*/
	
	if(analysisDebugLevel>=1)
	{
		if(chkpt) Dbg::dbg << indent <<"    chkpt="<<chkpt<<"="<<chkpt->str(indent+"    ")<<endl;
	}
	
	// Set of nodes that this analysis has blocked progress on until the next join point
	set<DataflowNode> joinNodes;
	
	// The pCFG node that the analysis is currently at
	pCFGNode n;
	
	// Process sets that are currently active (i.e. not blocked).
	set<unsigned int> activePSets;
	// Process sets that are currently blocked on communication or the end of the function.
	set<unsigned int> blockedPSets;
	// Process sets that are resumed from blocked state. For each such process set we 
	// must skip the call to the transfer function for their next dataflow node because they
	// already called the transfer function when they originally became blocked on the node.
	set<unsigned int> releasedPSets;
	
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
		
		if(analysisDebugLevel>=1) Dbg::dbg << indent << "@@@ Restarting analysis from checkpoint "<<chkpt->str(indent+"      ")<<endl;
		
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
			// Choose the next process set to focus on
			int curPSet;
			// The current process set is the next active one or the process set that 
			// performed the split from which we're restarting 
			// (the latter special condition is only needed to keep the output more readable)
			if(chkpt==NULL || !(chkpt->isSplit))
			{
				//curPSet = *(activePSets.begin());
				
				// make sure to pick pSet 1 last (this is a hack to postpone the fix where we need to
				// keep the states of the different process sets separate until fully-blocked points)
				curPSet = -1;
				for(set<unsigned int>::iterator it=activePSets.begin(); it!=activePSets.end(); it++)
					if(*it!=1) curPSet = *it;
				if(curPSet == -1) curPSet=1;

				if(analysisDebugLevel>=1) printPSetStatus(Dbg::dbg, n, activePSets, blockedPSets, releasedPSets, indent);
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
				
				ostringstream nodeNameStr; nodeNameStr << "pSet "<<curPSet<<": <span style=\"font-weight:bold; color=red;\">Current Node "<<Dbg::escape(dfNode.str())<<"</span>";
				if(analysisDebugLevel>=1){
					Dbg::enterFunc(nodeNameStr.str());
					Dbg::dbg << "firstTimeVisited="<<firstTimeVisited<<", modified="<<modified<<" activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<endl;
					cout << "Entering "<<nodeNameStr.str()<<": activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<endl;
				}
				
				if(analysisDebugLevel>=1) {
					printPSetStatus(Dbg::dbg, n, activePSets, blockedPSets, releasedPSets, indent);
					Dbg::dbg << "Checkpoints="<<endl;
					int i=0;
					for(set<pCFG_Checkpoint*>::iterator c=partitionChkpts.begin(); c!=partitionChkpts.end(); c++, i++) {
						Dbg::dbg << "    Chkpt "<<i<<": "<<(*c)->str("        ") << endl;
					}
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
				
				// ---------------------------------------------------------------------------------------
				
				// See if we need to merge the process sets in this pCFGNode because two sets 
				// are at the same DataflowNodes and have the same blocked/active status
				bool merged = mergePCFGNodes(n, descN, func, *state, dfInfoAbove, 
                                         activePSets, blockedPSets, releasedPSets, indent+"    ");
if(analysisDebugLevel>=1) Dbg::dbg << indent << "merged="<<merged<<endl;
				// If we did end up merging some process sets
				if(merged)
				{
					if(analysisDebugLevel>=1) 
						Dbg::dbg << indent << "@@@ Merging completed. Propagating dataflow state to mergedN = "<<descN.str("          ")<<endl;
					// We've identified a new descendant node and will now propagate 
					// n's dataflow state to that node
					//descN = mergedN;
					modified = true;
					
					// Set dfInfoBelow to the the state ABOVE this node so that it can be propagated
					// to the merged descN
					// !!! What happens to the original Lattices of dfInfoBelow? Are they leaked?
					// !!! Are there now references of the lattices in dfInfoAbove? If so, does it mean they're changed during this function?
					dfInfoBelow = dfInfoAbove;	
				}
				// Else, if we didn't merge and the current process set was resumed from blocked 
				// status, skip the transfer function
				else if(releasedPSets.erase(curPSet) > 0)
				{
					if(analysisDebugLevel>=1) 
						Dbg::dbg << indent << "@@@ Resuming previously blocked pSet "<<curPSet<<endl;
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
								Dbg::dbg << "    Just-Copy Above: Lattice "<<j<<": \n    "<<(*itA)->str("        ")<<endl;
								Dbg::dbg << "    Just-Copy Below: Lattice "<<j<<": \n    "<<(*itB)->str("        ")<<endl;
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
							Dbg::dbg << "    Pre-Transfer Above: Lattice "<<j<<": \n    "<<(*itA)->str("        ")<<endl;
							Dbg::dbg << "    Pre-Transfer Below: Lattice "<<j<<": \n    "<<(*itB)->str("        ")<<endl;
						}
						(*itB)->copy(*itA);
					}*/
					
					// Initialize dfInfoNewBelow to be the copy of the information above this node.
					//    It will then get pushed through the transfer function before being unioned 
					//    with and widened into the information below this node.
					for(vector<Lattice*>::const_iterator itA=dfInfoAbove.begin(); itA != dfInfoAbove.end(); itA++)
					{
						if(analysisDebugLevel>=1){
							Dbg::dbg << indent << "    Pre-Transfer Above: Lattice "<<j<<": \n    "<<(*itA)->str("        ")<<endl;
						}
						dfInfoNewBelow.push_back((*itA)->copy());
					}
					
					/*for(vector<Lattice*>::const_iterator itB=dfInfoBelow.begin(); itB != dfInfoBelow.end(); itB++)
					{
						if(analysisDebugLevel>=1){
							Dbg::dbg << "    Pre-Transfer Below: Lattice "<<j<<": \n    "<<(*itB)->str("        ")<<endl;
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
					
					Dbg::dbg << indent << "deadPSet="<<deadPSet<<", splitPSet="<<splitPSet<<", splitPNode="<<splitPNode<<", blockPSet="<<blockPSet<<endl; 
					
					// If curPSet is at the end of the function, it becomes blocked
					if(n.getCurNode(curPSet) == funcCFGEnd)
					{
						if(analysisDebugLevel>=1) 
							Dbg::dbg << indent << "@@@ Process set "<<curPSet<<" reached function end: transitioning from active to blocked status.\n";
						
						// Move curPSet from active to blocked status
						movePSet(curPSet, blockedPSets, activePSets);
						// Move on to the next active process set (if any)
						if(analysisDebugLevel>=1) Dbg::exitFunc(nodeNameStr.str());
						break;
					}
					
					// If the current process set has reached an impossible dataflow state, stop progress 
					// along the current pCFG node
					if(deadPSet) {
						if(analysisDebugLevel>=1) { 
							Dbg::exitFunc(nodeNameStr.str());
							if(enteredFunction) Dbg::exitFunc(funcNameStr.str());
						}
						return false;
					// If the current process set wants to be split, break out of this analysis and let the parent 
					// partitioned analysis re-schedule this analysis
					} else if(splitPNode) {
						if(analysisDebugLevel>=1) {
							Dbg::dbg << "Exiting runAnalysis_pCFG("<<func.get_name().getString()<<"()) nodeNameStr="<<Dbg::escape(nodeNameStr.str())<<endl;
							cout << "Exiting runAnalysis_pCFG("<<func.get_name().getString()<<"()) nodeNameStr="<<nodeNameStr.str()<<endl;
							Dbg::exitFunc(nodeNameStr.str());
							if(enteredFunction) {
								Dbg::dbg << "Exiting runAnalysis_pCFG("<<func.get_name().getString()<<"()) funcNameStr="<<funcNameStr.str()<<endl;
								cout << "Exiting runAnalysis_pCFG("<<func.get_name().getString()<<"()) funcNameStr="<<funcNameStr.str()<<endl;
								Dbg::exitFunc(funcNameStr.str());
							}
						}
						Dbg::dbg << "Exiting runAnalysis_pCFG("<<func.get_name().getString()<<"())\n";
						return false;
					// If the current process set was split into multiple process sets, descN was updated by
					// partitionTransfer to be the new pCFGNode, which contains the additional split process sets.
					// Advance to the next pCFG node, which corresponds to curPSet advancing to its next DataflowNode.
					} else if(splitPSet)
					{
						/*Dbg::dbg << "activePSets = ";
						for(set<int>::iterator it=activePSets.begin(); it!=activePSets.end(); it++)
						{ Dbg::dbg << *it << " "; }
						Dbg::dbg <<endl;*/
					}
					// If the current process set wants to block at the current node
					else if(blockPSet)
					{
						if(analysisDebugLevel>=1) 
							Dbg::dbg << indent << "@@@ Process set "<<curPSet<<" transitioning from active to blocked status.\n";
						
						// Move curPSet from active to blocked status
						movePSet(curPSet, blockedPSets, activePSets);
						// Move on to the next active process set (if any)
						if(analysisDebugLevel>=1) Dbg::exitFunc(nodeNameStr.str());
						break;
					}
					else
						descN.advanceOut(curPSet);
					// >>>>>>>>>>>>>>>>>>> TRANSFER FUNCTION >>>>>>>>>>>>>>>>>>>
				}
				
				// Print out the post-transfer lattices
				if(analysisDebugLevel>=1)
				{
					int j=0;
					for(vector<Lattice*>::const_iterator itB = dfInfoBelow.begin();
					    itB != dfInfoBelow.end(); itB++, j++)
						Dbg::dbg << indent << "    Transferred: Lattice "<<j<<": \n    "<<(*itB)->str(indent+"        ")<<endl;
					Dbg::dbg << indent <<"    transferred, modified="<<modified<<endl;
					Dbg::dbg << indent <<"    ------------------\n";
				}

				// If the state below this node got modified from the state above this node or by the transfer 
				// function, advance to descN
				if(modified)
				{
					// Examine the descendant pCFGNode
					if(analysisDebugLevel>=1)
					{
						Dbg::dbg << indent << "    Descendant ("<<Dbg::escape(descN.str(indent+"    "))<<"):\n";
						Dbg::dbg << indent << "    ~~~~~~~~~~~~\n";
					}
					
					// The state of the next pCFG node
					NodeState* nextState = pCFGState::getNodeState(func, descN, this);
					if(analysisDebugLevel>=1) Dbg::dbg << indent << "nextState="<<nextState<<endl;
					ROSE_ASSERT(nextState);
					
					// Propagate the Lattices below this node to its descendant
					/*modified = */propagateFWStateToNextNode(func, dfInfoBelow, n, *state, 
					                                      nextState->getLatticeAboveMod(this), descN, 
					                                      (ConstrGraph*)NULL, 0)/* || modified*/;
					if(analysisDebugLevel>=1)
					{
						Dbg::dbg << indent <<"    propagated, modified="<<modified<<endl;
						Dbg::dbg << indent <<"    ^^^^^^^^^^^^^^^^^^\n";
					}
				
					n = descN;
					// We're moving onto the next pCFGNode, so reset the modified flag
					modified = false;
				}
				// Otherwise, this process set becomes blocked
				else {
					if(analysisDebugLevel>=1) Dbg::exitFunc(nodeNameStr.str());
					// !!! Not sure if it is correct to block process sets that have reached a fixed point. We may need
					// !!! to add an extra status to handle just this case.
					//break;
					return true;
				}
				
				if(analysisDebugLevel>=1) Dbg::exitFunc(nodeNameStr.str());
			} // while(1)
			
			if(analysisDebugLevel>=1) printPSetStatus(Dbg::dbg, n, activePSets, blockedPSets, releasedPSets, indent);
		} // while(activePSets.size()>0)

		// All process sets are now blocked, so we should match sends to receives, if possible
		ROSE_ASSERT(activePSets.size() == 0);
		ROSE_ASSERT(blockedPSets.size() == n.getPSetDFNodes().size());
		ROSE_ASSERT(releasedPSets.size() == 0);
		
		unsigned int splitPSet;
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
			Dbg::dbg << indent << "After matchSendsRecvs() : splitConditions.size()="<<splitConditions.size()<<endl;
			Dbg::dbg << indent << "runAnalysis_pCFG() activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<endl;
		}
		
		// If a split was indeed performed
		if(splitConditions.size()>0)
		{
//Dbg::dbg << "aboveCopy.size="<<aboveCopy.size()<<endl;
			
			// The pCFGNode that this analysis will transition to after the split
			pCFGNode descN(n);
			performPSetSplit(n, descN, splitPSet, aboveCopy, 
			          splitConditions, splitPSetNodes, splitPSetActive, 
			          func, fState, pCFGState::getNodeState(func, n, this), 
			          activePSets, blockedPSets, releasedPSets, true, indent+"    ");
			
			// Propagate the post-split dataflow information from above this pCFGNode to below it, performing
			// any needed unions and widenings. We will then propagate this below information from n to descN.
			// /*modified = */updateLattices(pCFGState::getNodeState(func, n, this)->getLatticeBelowMod(this), 
			//                               pCFGState::getNodeState(func, n, this)->getLatticeAboveMod(this), 
			//                               false)/* || modified*/;
			
			if(analysisDebugLevel>=1)
			{
				int p=0;
				Dbg::dbg << indent << "runAnalysis_pCFG() D pSetActive =";    for(vector<bool>::iterator it=pSetActive.begin(); it!=pSetActive.end();    it++, p++) { Dbg::dbg << p << " : " << *it << ", "; } Dbg::dbg <<endl;
				printPSetStatus(Dbg::dbg, descN, activePSets, blockedPSets, releasedPSets, indent);
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
				Dbg::dbg << indent << "runAnalysis_pCFG() pSetActive.size()="<<pSetActive.size()<<"  activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<endl;
			
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
			Dbg::dbg << indent << "WARNING: in un-releaseable state process set "<<curPSet<<" is not at function end! Instead, n.pSetDFNodes["<<curPSet<<"]="<<(*it).str()<<endl;
			//ROSE_ASSERT(0);
			// Checkpoint this analysis. We may return to it later and discover that not all these process states
			// are actually possible.
			if(analysisDebugLevel>=1) 
				Dbg::dbg << indent << "@@@ Shelving this blocked partition for now.\n";
			set<pCFG_Checkpoint*> chkpt;
			chkpt.insert(new pCFG_Checkpoint(n, func, fState, activePSets, blockedPSets));
			split(chkpt);
		}
	}
	
	// At this point the analysis has reached the end of the function and does not need to propagate
	// any dataflow further down this function's pCFG. We will now return without creating a checkpoint.
	
	if(analysisDebugLevel>=1 && enteredFunction) Dbg::exitFunc(funcNameStr.str());

// !!! Need to return something useful
	return true;
}

// Merge any process sets that are at equivalent dataflow states.
// Return true if any process sets were merged, false otherwise.
// If no processes are merged, mergePCFGNodes does not modify mergedN, dfInfo, activePSets, blockedPSets or releasedPSets.
bool pCFG_FWDataflow::mergePCFGNodes(const pCFGNode& n, pCFGNode& mergedN, 
                                     const Function& func, NodeState& state, vector<Lattice*>& dfInfo, 
                                     set<unsigned int>& activePSets, set<unsigned int>& blockedPSets, set<unsigned int>& releasedPSets, string indent)
{
	startProfileFunc("mergePCFGNodes", indent);
	mergedN = n;

	if(analysisDebugLevel>=1) Dbg::dbg << indent << "mergePCFGNodes() n="<<n.str()<<endl;

	// Maps the various Dataflow states that different process sets may be at (combination
	// of DataflowNode and active/blocked status) to the sets of process sets at those states
	map<pair<DataflowNode, bool>, list<unsigned int> > setNodes;
	int curPSet=0;
	for(vector<DataflowNode>::const_iterator it=n.getPSetDFNodes().begin(); 
	    it!=n.getPSetDFNodes().end(); it++, curPSet++)
	{
		pair <DataflowNode, bool> p(*it, (activePSets.find(curPSet) != activePSets.end()));
		setNodes[p].push_back(curPSet);
	}
	
	// Check to see if there is more than one process set in any given state
	bool merged=false; // set to true if we do merge some process sets
	for(map<pair<DataflowNode, bool>, list<unsigned int> >::iterator itMergSet=setNodes.begin();
	    itMergSet!=setNodes.end(); itMergSet++)
	{
		if(itMergSet->second.size()>1)
		{
			if(analysisDebugLevel>=1)
			{
				Dbg::dbg << indent << "Merging process sets [";
				for(list<unsigned int>::iterator itMerge=itMergSet->second.begin(); itMerge!=itMergSet->second.end(); itMerge++)
					Dbg::dbg << *itMerge << " ";
				Dbg::dbg << "]\n";
			}
			
			// The process sets in itMergSet->second are all at the same dataflow state (DataflowNode + active/blocked status).
			
			// Sort process sets to be merged to ensure that the one with the minimum value is first
			// (Keeps things orderly for the sake of debugging)
			itMergSet->second.sort();
			
			// Merge their dataflow information and their DataflowNodes within the pCFGNode.
			// The space of process set IDs will be compressed to remove the holes left by the removal, 
			// with the migrations of process set ids recorded in pSetMigrations.
			map<unsigned int, unsigned int> pSetMigrations;
			
			/*{
				list<unsigned int>::iterator mergedP=itMergSet->second.begin();
				mergedP++;

				int curPSet=0, curNewPSet=0;
				for(vector<DataflowNode>::const_iterator it=mergedN.getPSetDFNodes().begin(); 
				    it!=mergedN.getPSetDFNodes().end(); it++, curPSet++) 
	    		{
	    			// If the current process set will be deleted, skip it and advance to the next deleted process set
	    			if(mergedP!=itMergSet->second.end() && *mergedP==curPSet)
	    				mergedP++;
	    			// If the current process set will not be be deleted
	    			else {
	    				// The current process set's new index within the node is incremented
	    				if(curPSet != curNewPSet) pSetMigrations.insert(make_pair(curPSet, curNewPSet));
	    				Dbg::dbg << indent << "Old process set "<<curPSet<<" will now be pSet "<<curNewPSet<<endl;
	    				curNewPSet++;
	    			}
	    		}
			}*/
			
			// Update mergeN's dataflow state 
/// ??? What if mergePCFGStates doesn't want to perform all the merges we indicated because its process set representation is not sufficiently rich?
			mergePCFGStates(itMergSet->second, mergedN, func, state, dfInfo, pSetMigrations);
			Dbg::dbg << indent << "pSetMigrations = ";
			for(map<unsigned int, unsigned int>::iterator p=pSetMigrations.begin(); p!=pSetMigrations.end(); p++)
				Dbg::dbg << "["<<p->first<<" -&gt; "<<p->second<<"] ";
			Dbg::dbg << endl;
			
			// Update mergedN by removing all the merged process sets, except the one with the minimum value.
			list<unsigned int>::iterator itMerged=itMergSet->second.begin();
			itMerged++;
			for(; itMerged!=itMergSet->second.end(); itMerged++)
			{
				if(analysisDebugLevel>=1) 
				{ Dbg::dbg << indent << "Removing merged process set "<<(*itMerged)<<endl; fflush(stdout); }
				mergedN.removePSet(*itMerged);
				activePSets.erase(*itMerged);
				blockedPSets.erase(*itMerged);
				releasedPSets.erase(*itMerged);
			}
			
			// Adjust activePSets, blockedPSets and releasedPSets to account for the fact that some
			// process sets have changed ids
			if(analysisDebugLevel>=1) Dbg::dbg << indent << "migrations:\n";
			for(map<unsigned int, unsigned int>::iterator it=pSetMigrations.begin(); it!=pSetMigrations.end(); it++)
			{
				if(analysisDebugLevel>=1) Dbg::dbg << indent << "    "<<it->first<<" -&gt; "<<it->second<<endl;
				
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
	endProfileFunc("mergePCFGNodes", indent);
	
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
		Dbg::dbg << "pCFG_FWDataflow::split, partitionChkpts.size()="<<partitionChkpts.size()<<endl;
		for(set<pCFG_Checkpoint*>::const_iterator it=partitionChkpts.begin();
		    it!=partitionChkpts.end(); it++)
		{
			Dbg::dbg << "---- Chkpt: "<<(*it)->str("    ")<<endl;
		}
	}
}

// Fills tgtLat with copies of the lattices in srcLat. tgtLat is assumed to be an empty vector
void pCFG_FWDataflow::latticeCopyFill(vector<Lattice*>& tgtLat, const vector<Lattice*>& srcLat)
{
	for(vector<Lattice*>::const_iterator it=srcLat.begin(); it!=srcLat.end(); it++)
	{
		Lattice* lat = (*it)->copy();
		//Dbg::dbg << "latticeCopyFill() *it="<<(*it)<<"  lat="<<lat<<endl;
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
//    generates the resulting checkpoints, performs the split and sets splitPNodes to true.
// Otherwise, sets neither to true.
bool pCFG_FWDataflow::partitionTranfer(
                           const pCFGNode& n, pCFGNode& descN, unsigned int curPSet, const Function& func, NodeState* fState, 
                           const DataflowNode& dfNode, NodeState* state, 
                           vector<Lattice*>& dfInfoBelow, vector<Lattice*>& dfInfoNewBelow, 
                           bool& deadPSet, bool& splitPSet, bool& splitPNode, bool& blockPSet, 
                           set<unsigned int>& activePSets, set<unsigned int>& blockedPSets, set<unsigned int>& releasedPSets,
                           const DataflowNode& funcCFGEnd)
{
	string indent="    ";
	startProfileFunc("partitionTranfer", indent);
	
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
		Dbg::dbg << indent << "deadPSet="<<deadPSet<<" splitPSet="<<splitPSet<<" splitPNode="<<splitPNode<<" blockPSet="<<blockPSet<<endl;
	if(deadPSet)
	{
		endProfileFunc("partitionTranfer", indent);
		return modified;
	}
	
	// Either all three flags are =false or only one is =true
	ROSE_ASSERT((!splitPSet && !splitPNode && !blockPSet) ||
	            XOR(XOR(splitPSet, splitPNode), blockPSet));
	
	// If we're not going to block on this node, update the state below it.
	// (if we are going to block, we'll just dump the current dfInfoNewBelow 
	//  and move on to the next process set)
	if(!blockPSet && n.getCurNode(curPSet) != funcCFGEnd)
		// Incorporate dfInfoNewBelow(the copy of the information above the node, after the transfer function) into dfInfoBelow,
		// the information below the node. Also, DEALLOCATE the lattices in dfInfoNewBelow since they have now served their purpose
		// of ferrying data from dfInfoAbove, through the transfer function, to dfInfoBelow
		modified = updateLattices(dfInfoBelow, dfInfoNewBelow, true, indent+"    ") || modified;
		// !!! What happens to the lattices in dfInfoNewBelow? Are they ever deallocated?
		
		// !!! Note: this assumes that if the transfer function blocks a process set, it is ok to drop all of the
		// !!! changes that it may have made to the dataflow state.
	
	// If the analysis wants to be split (i.e. its dataflow state propagated to multiple pCFGNodes), perform the split
	if(splitPNode)
	{
		if(analysisDebugLevel>=1)
		{ Dbg::dbg << indent <<"    Splitting analysis process set "<<curPSet<<endl; }
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
				Dbg::dbg << indent << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n";
				Dbg::dbg << indent << "&&& partitionTranfer() Creating new Descendant node &&&\n";
			}

			// The pCFG node that corresponds to curPSet advancing along the current outgoing edge
			pCFGNode descN(n);
			descN.advance(curPSet, (*edgeIt).target());
			pCFG_Checkpoint* newChkpt = new pCFG_Checkpoint(descN, func, fState, activePSets, blockedPSets, releasedPSets, curPSet, *splitIt);
			// Add the checkpoint that corresponds to the next split to the vector of checkpoints
			splitChkpts.insert(newChkpt);
			
			// Propagate this pCFGNode's dataflow info to the current descendant
			NodeState* nextState = pCFGState::getNodeState(func, descN, this); // The state of the next pCFG node
			ROSE_ASSERT(nextState);
				
			if(analysisDebugLevel>=1) 
				Dbg::dbg << indent << "nextState="<<nextState<<endl;
			// Propagate the Lattices below this node to its descendant
			/*modified = */propagateFWStateToNextNode(func, dfInfoBelow, n, *state, 
			                                          nextState->getLatticeAboveMod(this), descN,
			                                          *splitIt, curPSet)/* || modified*/;
		
			/*if(analysisDebugLevel>=1)
			{ printf("        Split condition %d: \n%s\n", i, ((*splitIt)->str("            ")).c_str()); }*/
			if(analysisDebugLevel>=1) 
			{
				Dbg::dbg << indent << "newChkpt = "<<newChkpt->str()<<endl;
				Dbg::dbg << indent << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n";
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
		                 func, fState, state, activePSets, blockedPSets, releasedPSets, false, indent+"    ");
	}
	
	endProfileFunc("partitionTranfer", indent);
	
	return modified;
}

// Updates the lattices in tgtLattices from the lattices in srcLattices. For a given lattice pair <t, s>
//    (t from tgtLattices, s from srcLattices), t = t widen (t union s).
// If delSrcLattices==true, deletes all the lattices in srcLattices.
// Returns true if this causes the target lattices to change, false otherwise.
bool pCFG_FWDataflow::updateLattices(vector<Lattice*>& tgtLattices, vector<Lattice*>& srcLattices, bool delSrcLattices, string indent)
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
			Dbg::dbg << indent << "updateLattices() Lattice "<<i<<" Below Info:"<<(*itTgt)->str(indent+"      ")<<endl;
			Dbg::dbg << indent << "updateLattices() Lattice "<<i<<" New Transferred Below Info:"<<(*itSrc)->str(indent+"      ")<<endl;
		}
		
		// Finite Lattices can use the regular meet operator, while infinite Lattices
		// must also perform widening to ensure convergence.
		if((*itTgt)->finiteLattice())
		{
			modified = (*itTgt)->meetUpdate(*itSrc) || modified;
			if(analysisDebugLevel>=1) 
				Dbg::dbg << indent << "updateLattices() Lattice "<<i<<" After Meet Below Info:"<<(*itTgt)->str(indent+"      ")<<endl;
		}
		else
		{
			(*itSrc)->meetUpdate(*itTgt);
			if(analysisDebugLevel>=1) 
				Dbg::dbg << indent << "updateLattices() Lattice "<<i<<" After Meet Below Info:"<<(*itSrc)->str(indent+"      ")<<endl;
			modified =  dynamic_cast<InfiniteLattice*>(*itTgt)->widenUpdate(dynamic_cast<InfiniteLattice*>(*itSrc)) || modified;
			if(analysisDebugLevel>=1) 
				Dbg::dbg << indent << "updateLattices() Lattice "<<i<<" After Widening Below Info:"<<(*itTgt)->str(indent+"      ")<<endl;
		}
		
		// Delete the current lattice in srcLattices
		if(delSrcLattices)
			delete *itSrc;
	}
	
	if(analysisDebugLevel>=1)
	{
		if(modified)
			Dbg::dbg << indent << "updateLattices() Lattice information Below MODIFIED\n";
		else
			Dbg::dbg << indent << "updateLattices() Lattice information Below NOT Modified\n";
	}
	
	return modified;
}

// Split the process set pSet in pCFGNode n, resulting in a new pCFGNode that contains more 
//    process sets. Advances descN to be this new pCFGNode and updates activePSets to make
//    the newly-created process sets active. 
// splitConditions - set of logical conditions that apply to each split process set
// splitPSetNodes - the DataflowNodes that each split process set starts at.
// if freshPSet==true, each partition will get a completely new process set and thus
//    must be set fresh. If freshPSet==false, each partition's process set is simply
//    an updated version of the old process set (i.e. an extra condition is applied).
void pCFG_FWDataflow::performPSetSplit(const pCFGNode& n, pCFGNode& descN, unsigned int pSet, 
                                vector<Lattice*>& dfInfo,
                                vector<ConstrGraph*> splitConditions, vector<DataflowNode> splitPSetNodes,
                                vector<bool>& splitPSetActive,
                                const Function& func, NodeState* fState, NodeState* state, 
                                set<unsigned int>& activePSets, set<unsigned int>& blockedPSets, set<unsigned int>& releasedPSets,
                                bool freshPSet, string indent)
{
	if(analysisDebugLevel>=1) Dbg::enterFunc("performPSetSplit");
	startProfileFunc("performPSetSplit", indent);
	
	ROSE_ASSERT(splitPSetNodes.size()>=2);
	
	if(analysisDebugLevel>=1) 
		Dbg::dbg << indent << "performPSetSplit(pSet="<<pSet<<") splitPSetNodes.size()="<<splitPSetNodes.size()<<endl;
	
	// Update descN to correspond to the new pCFGNode that results from the creation
	// of the new process sets.
	vector<unsigned int> newPSets; // The newly-generated process sets
	descN = n;
	descN.advance(pSet, splitPSetNodes[0]); // pSet moves on to the first DataflowNode in splitPSetNodes
	// Other process sets get the other DataflowNodes in splitPSetNodes
	vector<DataflowNode>::iterator splitDFIt=splitPSetNodes.begin(); splitDFIt++;
	for(; splitDFIt!=splitPSetNodes.end(); splitDFIt++)
		newPSets.push_back(descN.createPSet(*splitDFIt));
	
	if(analysisDebugLevel>=1) 
		Dbg::dbg << indent << "descN="<<descN.str()<<endl;
	/*Dbg::dbg << "newPSets ("<<newPSets.size()<<")= ";
	for(set<int>::iterator it=newPSets.begin(); it!=newPSets.end(); it++)
		Dbg::dbg << *it << " ";
	Dbg::dbg << endl;*/
	
	// Get the state of the descendant pCFGNode
	// NodeState* descState = pCFGState::getNodeState(func, descN, this);
	
	if(analysisDebugLevel>=1)
	{
		Dbg::dbg << indent << "^^^^^^^^^^\n";
		Dbg::dbg << indent << "performPSetSplit: Creating New Process Set Conditions from process set "<<pSet<<endl;
	}
	
	// If each partition will get fresh process set bounds information, reset pSet's 
	//    bounds information from cg before doing anything else so that
	// 1. pSet does get fresh bounds and 
	// 2. other process sets' state is not polluted with pSet's stale bounds
	if(freshPSet)
		resetPSet(pSet, dfInfo);
	
	// Copy the state from pSet to all the newly-generated process sets
	// and activate these process sets
	if(analysisDebugLevel>=1) {
		Dbg::dbg << indent << "newPSets.size()="<<newPSets.size()<<endl;
		int i=0;
		for(vector<ConstrGraph*>::iterator splitCondIt = splitConditions.begin(); splitCondIt != splitConditions.end(); splitCondIt++, i++) {
			Dbg::dbg << indent << "Condition "<<i<<": "<<(*splitCondIt)->str("")<<endl;
		}
	}
	//Dbg::dbg << "newPSets.size()="<<newPSets.size()<<" splitConditions.size()="<<splitConditions.size()<<" splitPSetActive.size()="<<splitPSetActive.size()<<endl;
	ROSE_ASSERT(((newPSets.size()+1) == splitConditions.size()) && (splitConditions.size() == splitPSetActive.size()));
	vector<ConstrGraph*>::iterator splitCondIt = splitConditions.begin(); splitCondIt++;
	vector<bool>::iterator activeIt            = splitPSetActive.begin(); activeIt++;
	for(vector<unsigned int>::iterator it=newPSets.begin(); 
	    it!=newPSets.end() && splitCondIt!=splitConditions.end() && activeIt!=splitPSetActive.end(); 
	    it++, splitCondIt++, activeIt++)
	{
		int newPSet = *it;
		// if the current split process set is supposed to be active, activate it
		if(analysisDebugLevel>=1) 
			Dbg::dbg << indent << "performPSetSplit() splitPSetActive[newPSet="<<newPSet<<"]="<<(*activeIt)<<" activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<endl;
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
		Dbg::dbg << indent << "performPSetSplit() freshPSet="<<freshPSet<<" activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<endl;
	
	// Update the state of pSet from its condition
	initPSetDFfromPartCond(func, n, pSet, dfInfo, state->getFactsMod((Analysis*)this), *(splitConditions.begin()));
	if(analysisDebugLevel>=1) 
		Dbg::dbg << indent << "performPSetSplit() splitPSetActive[pSet="<<pSet<<"]="<<*(splitPSetActive.begin())<<" activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<endl;
	if(*(splitPSetActive.begin()))
	{
		bool wasBlocked = movePSet(pSet, activePSets, blockedPSets, true);
		if(wasBlocked) releasedPSets.insert(pSet);
	}
	delete *(splitConditions.begin());
	if(analysisDebugLevel>=1) 
		Dbg::dbg << indent << "performPSetSplit() activePSets.size()="<<activePSets.size()<<"  blockedPSets.size()="<<blockedPSets.size()<<"  releasedPSets.size()="<<releasedPSets.size()<<"  n.getPSetDFNodes().size()="<<n.getPSetDFNodes().size()<<endl;
	
	endProfileFunc("performPSetSplit", indent);
	
	/*Dbg::dbg << "^^^^^^^^^^\n";
	Dbg::dbg << "partitionTranfer: Initializing New Process Set Conditions\n";
	
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
	if(analysisDebugLevel>=1) Dbg::exitFunc("performPSetSplit");
}

// Propagates the dataflow info from the current node's NodeState (curNodeLattices) to the next node's 
//     NodeState (nextNodeLattices), incorporating the partition condition (if any) into the propagated dataflow state.
// Returns true if the next node's meet state is modified and false otherwise.
/*bool */ void pCFG_FWDataflow::propagateFWStateToNextNode(const Function& func, 
                      const vector<Lattice*>& curNodeLattices, const pCFGNode& curNode, const NodeState& curNodeState, 
                      vector<Lattice*>& nextNodeLattices, const pCFGNode& nextNode,
                      ConstrGraph* partitionCond, unsigned int pSet, string indent)
{
	
	//bool modified = false;
	vector<Lattice*>::const_iterator itC;
	vector<Lattice*>::iterator       itN;
	if(analysisDebugLevel>=1)
	{
		if(analysisDebugLevel>=1)
		{
			Dbg::dbg << indent << "\n        Propagating to Next Node: "<<nextNode.str("        ")<<endl;
			Dbg::dbg << indent << "nextNodeLattices.size()="<<nextNodeLattices.size()<<", curNodeState="<<&curNodeState<<"  \n";
		}
		int j=0;
		for(itC=curNodeLattices.begin(), itN=nextNodeLattices.begin(); 
		    itC!=curNodeLattices.end() && itN!=nextNodeLattices.end(); itC++, itN++, j++)
		{
			if(analysisDebugLevel>=1)
			{
				Dbg::dbg << indent << "        Current node below: Lattice "<<j<<": "<<(*itC)->str("            ")<<endl;
				Dbg::dbg << indent << "        Next node above: Lattice "<<j<<": "<<(*itN)->str("            ")<<endl;
			}
			/*Dbg::dbg << "-----\n"; fflush(stdout);
			ConstrGraph* cg = dynamic_cast<ConstrGraph*>(*itN);
			Dbg::dbg << "cg = "<<cg<<endl;
			varID nprocsVar("nprocsVar");
			affineInequality* val = cg->getVal(zeroVar, nprocsVar);
			Dbg::dbg << "val = "<<val<<"-----\n"; fflush(stdout);
			Dbg::dbg << "        Next node above: Lattice "<<j<<": "<<(*itN)->str("            ")<<endl;*/
		}
	}
	// If we need to apply a partition condition to the meet lattices, use the 
	// initPSetDFfromPartCond() function provided by the specific analysis
	vector<Lattice*> curNodeLattices_withPartCond;
	if(analysisDebugLevel>=1) 
		Dbg::dbg << indent << "partitionCond = "<<partitionCond<<endl;
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
/*Dbg::dbg << "Propagated: itC="<<(*itC)->str()<<endl;		
Dbg::dbg << "Propagated: itN="<<(*itN)->str()<<endl;*/

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
			
			Dbg::dbg << "        Infinite Lattices\n";
			Dbg::dbg << "            *itN: " << dynamic_cast<InfiniteLattice*>(*itN)->str("            ") << endl;
			Dbg::dbg << "            *itC: " << dynamic_cast<InfiniteLattice*>(*itC)->str("            ") << endl;
			Dbg::dbg << "            meetResult: " << meetResult->str("") << endl;
			
			// widen the resulting meet
			modified =  dynamic_cast<InfiniteLattice*>(*itN)->widenUpdate(meetResult);
			delete meetResult;* /
			
			Dbg::dbg << "        Infinite Lattices\n";
			Dbg::dbg << "            *itN: " << dynamic_cast<InfiniteLattice*>(*itN)->str("            ") << endl;
			Dbg::dbg << "            *itC: " << dynamic_cast<InfiniteLattice*>(*itC)->str("            ") << endl;
			modified =  dynamic_cast<InfiniteLattice*>(*itN)->meetUpdate((*itC);
			Dbg::dbg << "            meetResult: " << (*itN)->str("") << endl;
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
			Dbg::dbg << "        Next node's in-data modified.\n";
			int j=0;
			for(itN=nextNodeLattices.begin(); itN!=nextNodeLattices.end(); itN++, j++)
			{
				Dbg::dbg << "        Propagated: Lattice "<<j<<": \n"<<(*itN)->str("            ")<<endl;
			}
		}
		else
			Dbg::dbg << "        No modification on this node\n";
	}*/

	//return modified;
}


// Moves the given process set from srcPSets to destPSets. If noSrcOk==false, does nothing
// if pSet is not inside srcPSets. If noSrcOk==true, adds pSet to destPSets regardless.
// Returns true if the process set was in srcPSets and false otherwise.
bool pCFG_FWDataflow::movePSet(unsigned int pSet, set<unsigned int>& destPSets, set<unsigned int>& srcPSets, bool noSrcOk)
{
	// if pSet was in fact in src
	if(srcPSets.erase(pSet)>0)
	{
		if(analysisDebugLevel>=1) Dbg::dbg << "movePSet("<<pSet<<") Moving\n";
		// Move it to destPSets
		destPSets.insert(pSet);
		return true;
	}
	else if(noSrcOk)
	{
		if(analysisDebugLevel>=1) Dbg::dbg << "movePSet("<<pSet<<") Inserting\n";
		// Insert it into destPSets
		destPSets.insert(pSet);
	}
	else
	{
		if(analysisDebugLevel>=1) Dbg::dbg << "movePSet("<<pSet<<") Noop\n";
	}
	
	return false;
}

