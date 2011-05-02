#include "common.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include <vector>
#include <set>
#include <map>

int analysisDebugLevel=1;

using namespace std;

/*******************************
 *** IntraProceduralAnalysis ***
 *******************************/

IntraProceduralAnalysis::~IntraProceduralAnalysis() {}

/*******************************
 *** InterProceduralAnalysis ***
 *******************************/
InterProceduralAnalysis::~InterProceduralAnalysis() {}

/*************************************
 *** UnstructuredPassIntraAnalysis ***
 *************************************/

// runs the intra-procedural analysis on the given function, returns true if 
// the function's NodeState gets modified as a result and false otherwise
// state - the function's NodeState
bool UnstructuredPassIntraAnalysis::runAnalysis(const Function& func, NodeState* state)
{
	DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(func.get_definition());
	DataflowNode funcCFGEnd = cfgUtils::getFuncEndCFG(func.get_definition());
	
	if(analysisDebugLevel>=2){
		printf("UnstructuredPassIntraAnalysis::runAnalysis() function %s()\n", func.get_name().str());
	}
	
	// iterate over all the nodes in this function
	for(VirtualCFG::iterator it(funcCFGStart); it!=VirtualCFG::dataflow::end(); it++)
	{
		DataflowNode n = *it;
		// The number of NodeStates associated with the given dataflow node
		//int numStates=NodeState::numNodeStates(n);
		// The actual NodeStates associated with the given dataflow node
		const vector<NodeState*> nodeStates = NodeState::getNodeStates(n);
		
		// Visit each CFG node
		for(vector<NodeState*>::const_iterator itS = nodeStates.begin(); itS!=nodeStates.end(); itS++)
			visit(func, n, *(*itS));
	}
	return false;
}

/*************************************
 *** UnstructuredPassInterAnalysis ***
 *************************************/
void UnstructuredPassInterAnalysis::runAnalysis()
{
	set<FunctionState*> allFuncs = FunctionState::getAllDefinedFuncs();
	
	// iterate over all functions with bodies
	for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)
	{
		FunctionState* fState = *it;
		intraAnalysis->runAnalysis(fState->func, &(fState->state));
	}
}

/*class Dataflow : virtual public Analysis
{

};*/

/*******************************
 *** InterProceduralDataflow ***
 *******************************/

InterProceduralDataflow::InterProceduralDataflow(IntraProceduralDataflow* intraDataflowAnalysis) : 
	InterProceduralAnalysis((IntraProceduralAnalysis*)intraDataflowAnalysis)
{
	printf("InterProceduralDataflow() intraAnalysis=%p, intraDataflowAnalysis=%p\n", (Analysis*)intraAnalysis, intraDataflowAnalysis);
	set<FunctionState*> allFuncs = FunctionState::getAllDefinedFuncs();
	
	// Initialize the dataflow state of all functions with bodies
	for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)	
	{
		FunctionState* funcS = *it;
		Function func = funcS->func;
		//cout << "func="<<func.get_name().getString()<<"() func.get_definition()="<<func.get_definition()<<"\n";
		if(func.get_definition())
		{
			/*vector<Lattice*> initLats = intraDataflowAnalysis->genInitState(
			                                 func, func.get_definition()->cfgForBeginning(), funcS->state);*/
			
			// !!! NOTE: cfgForEnd() AND cfgForBeginning() PRODUCE THE SAME SgFunctionDefinition SgNode BUT THE DIFFERENT INDEXES
			// !!!       (0 FOR BEGINNING AND 3 FOR END). AS SUCH, IT DOESN'T MATTER WHICH ONE WE CHOOSE. HOWEVER, IT DOES MATTER
			// !!!       WHETHER WE CALL genInitState TO GENERATE THE STATE BELOW THE NODE (START OF THE FUNCTION) OR ABOVE IT 
			// !!!       (END OF THE FUNCTION). THE CAPABILITY TO DIFFERENTIATE THE TWO CASES NEEDS TO BE ADDED TO genInitState
			// !!!       AND WHEN IT IS, WE'LL NEED TO CALL IT INDEPENDENTLY FOR cfgForEnd() AND cfgForBeginning() AND ALSO TO MAKE
			// !!!       TO SET THE LATTICES ABOVE THE ANALYSIS 
			vector<Lattice*>  initLats;
			vector<NodeFact*> initFacts;
			
			/*DataflowNode end = func.get_definition()->cfgForEnd();
			intraDataflowAnalysis->genInitState(func, func.get_definition()->cfgForEnd(), funcS->state,
			                                    initLats, initFacts);
			
			printf("InterProceduralAnalysis initLats at Ending %s(): node=%s\n", (*it)->func.get_name().str(), end.str().c_str());
				for(vector<Lattice*>::iterator it = initLats.begin(); 
				    it!=initLats.end(); it++)
				{	
					cout << *it << ": " << (*it)->str("    ") << "\n";
				}
			*/
			DataflowNode begin = func.get_definition()->cfgForBeginning();
			cout << "begin="<<begin.getNode()<<" = <"<<begin.getNode()->unparseToString()<<" | "<<begin.getNode()->class_name()<<">\n";
			intraDataflowAnalysis->genInitState(func, func.get_definition()->cfgForBeginning(), funcS->state,
			                                    initLats, initFacts);
			// Make sure that the starting lattices are initialized
			for(vector<Lattice*>::iterator it=initLats.begin(); it!=initLats.end(); it++)
				(*it)->initialize();
			
			/*if(analysisDebugLevel>=1){
				printf("InterProceduralAnalysis initLats at Beginning %s(): node=%s\n", func.get_name().str(), begin.str().c_str());
				for(vector<Lattice*>::iterator it = initLats.begin(); 
				    it!=initLats.end(); it++)
				{	
					cout << *it << ": " << (*it)->str("    ") << "\n";
				}
			}*/
			funcS->state.setLattices((Analysis*)intraAnalysis, initLats);
			funcS->state.setFacts((Analysis*)intraAnalysis, initFacts);
			cout << "Initialized state of function "<<func.get_name().getString()<<"(), state="<<(&(funcS->state))<<"\n";
			cout << "    "<<funcS->state.str(intraDataflowAnalysis, "    ")<<"\n";
			
			// Initialize funcS->retState with initLats. 
			// !!!Need to make a copy of initLats first
			//funcS->retState.setLatticeBelow((Analysis*)intraAnalysis, initLats);
			
			// We do not initialize retState since we don't know the type of the lattice that will capture
			// the projection of the normal lattice type on the return variables. However, there is no
			// need to provide default lattices since DFStateAtReturns and MergeAllReturnStates can
			// deal with Lattice* vectors that start out empty.
			
			// We do not need to deallocate facts in initFacts and lattices in initLats since setFacts() and
			// setLattices() have taken the original facts in initFacts and did not make private copies of them
			
			/*const vector<Lattice*>* funcLatticesBefore = &(funcS->state.getLatticeAbove((Analysis*)intraAnalysis));
			printf("        funcLatticesBefore->size()=%d, intraAnalysis=%p\n", funcLatticesBefore->size(), (Analysis*)intraAnalysis);*/
		}
	}
}

/*************************
 *** InitDataflowState ***
 *************************/

void InitDataflowState::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
	SgNode* sgn = n.getNode();
	if(analysisDebugLevel>=2){
		printf("InitDataflowState::visit() sgn=%p<%s | %s>, dfAnalysis=%p\n", sgn, sgn->unparseToString().c_str(), sgn->class_name().c_str(), (Analysis*)dfAnalysis);
	}
	// generate a new initial state for this node
	//vector<Lattice*> initLats = dfAnalysis->genInitState(func, n, state);
	vector<Lattice*>  initLats;
	vector<NodeFact*> initFacts;
	dfAnalysis->genInitState(func, n, state, initLats, initFacts);
	
	/*if(analysisDebugLevel>=2){
		int i=0;
		for(vector<Lattice*>::iterator l=initLats.begin(); l!=initLats.end(); l++, i++)
			cout << "Lattice "<<i<<": "<<(*l)->str("            ")<<"\n";
		
		i=0;
		for(vector<NodeFact*>::iterator f=initFacts.begin(); f!=initFacts.end(); f++, i++)
			cout << "Lattice "<<i<<": "<<(*f)->str("            ")<<"\n";
	}*/
		                                    
	//printf("InitDataflowState::visit() calling state.setLattices()\n");
	state.setLattices((Analysis*)dfAnalysis, initLats);
	state.setFacts((Analysis*)dfAnalysis, initFacts);
	
	if(analysisDebugLevel>=2){
		cout << "    state="<<state.str((Analysis*)dfAnalysis, "    ")<<"\n";
	}
	
	/*vector<Lattice*> initState = dfAnalysis->genInitState(func, n, state);
	printf("InitDataflowState::visit() 1\n");
			
	for(int i=0; i<initState.size(); i++)
	{
		printf("                         i=%d, initState[i]=%s\n", i, initState[i]->str("").c_str());
		state.addLattice((Analysis*)dfAnalysis, i, initState[i]);
		printf("                         state->getLatticeAbove((Analysis*)dfAnalysis).size()=%d\n", state.getLatticeAbove((Analysis*)dfAnalysis).size());
		//printf("                         state->getLatticeBelow((Analysis*)dfAnalysis).size()=%d\n", state.getLatticeBelow((Analysis*)dfAnalysis).size());
	}*/
	
	//const vector<Lattice*>& masterLatBel = state.getLatticeBelow((Analysis*)dfAnalysis);
	//printf("    creator=%p, state=%p, masterLatBel.size()=%d\n", (Analysis*)dfAnalysis, &state, masterLatBel.size());
}


/****************************
 *** FindAllFunctionCalls ***
 ****************************/
 
void FindAllFunctionCalls::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
	SgNode* sgn = n.getNode();
	if(analysisDebugLevel>=2){
		printf("FindAllFunctionCalls::visit() sgn=%p<%s | %s>\n", sgn, sgn->unparseToString().c_str(), sgn->class_name().c_str());
	}
	
	// If this is a function call, find the function that is being called and if it is 
	// in funcsToFind, record the call in funcCalls
	if(isSgFunctionCallExp(sgn)) {
		for(set<Function>::iterator func=funcsToFind.begin(); func!=funcsToFind.end(); func++) {
			if((*func).get_declaration() == isSgFunctionCallExp(sgn)->getAssociatedFunctionDeclaration()) {
				funcCalls[*func].insert(n);
				break;
			}
		}
	}
}

/***********************************
 *** IntraUniDirectionalDataflow ***
 ***********************************/


// Propagates the dataflow info from the current node's NodeState (curNodeState) to the next node's 
//     NodeState (nextNodeState).
// Returns true if the next node's meet state is modified and false otherwise.
bool IntraUniDirectionalDataflow::propagateStateToNextNode(
                      const vector<Lattice*>& curNodeState, DataflowNode curNode, int curNodeIndex,
                      const vector<Lattice*>& nextNodeState, DataflowNode nextNode)
{
	bool modified = false;
	vector<Lattice*>::const_iterator itC, itN;
	if(analysisDebugLevel>=1){
		printf("\n        Propagating to Next Node: %p[%s]<%s>\n", nextNode.getNode(), nextNode.getNode()->class_name().c_str(), nextNode.getNode()->unparseToString().c_str());
		int j;
		for(j=0, itC = curNodeState.begin(); itC != curNodeState.end(); itC++, j++)
			cout << "        Cur node: Lattice "<<j<<": \n            "<<(*itC)->str("            ")<<"\n";
		for(j=0, itN = nextNodeState.begin(); itN != nextNodeState.end(); itN++, j++)
			cout << "        Next node: Lattice "<<j<<": \n            "<<(*itN)->str("            ")<<"\n";
	}

	// Update forward info above nextNode from the forward info below curNode.
	
	// Compute the meet of the dataflow information along the curNode->nextNode edge with the 
	// next node's current state one Lattice at a time and save the result above the next node.
	for(itC = curNodeState.begin(), itN = nextNodeState.begin();
	    itC != curNodeState.end() && itN != nextNodeState.end(); 
	    itC++, itN++)
	{
		// Finite Lattices can use the regular meet operator, while infinite Lattices
		// must also perform widening to ensure convergence.
		if((*itN)->finiteLattice())
			modified = (*itN)->meetUpdate(*itC) || modified;
		else
		{
			//InfiniteLattice* meetResult = (InfiniteLattice*)itN->second->meet(itC->second);
			InfiniteLattice* meetResult = dynamic_cast<InfiniteLattice*>((*itN)->copy());
			cout << "        *itN: " << dynamic_cast<InfiniteLattice*>(*itN)->str("            ") << "\n";
			cout << "        *itC: " << dynamic_cast<InfiniteLattice*>(*itC)->str("            ") << "\n";
			meetResult->meetUpdate(*itC);
			cout << "        meetResult: " << meetResult->str("            ") << "\n";
		
			// Widen the resulting meet
			modified =  dynamic_cast<InfiniteLattice*>(*itN)->widenUpdate(meetResult);
			delete meetResult;
		}
	}
	
	if(analysisDebugLevel>=1) {
		if(modified)
		{
			cout << "        Next node's in-data modified. Adding...\n";
			int j=0;
			for(itN = nextNodeState.begin(); itN != nextNodeState.end(); itN++, j++)
			{
				cout << "        Propagated: Lattice "<<j<<": \n            "<<(*itN)->str("            ")<<"\n";
			}
		}
		else
			cout << "        No modification on this node\n";
	}

	return modified;
}

/*************************
 *** IntraFWDataflow ***
 *************************/

// Runs the intra-procedural analysis on the given function. Returns true if 
// the function's NodeState gets modified as a result and false otherwise.
// state - the function's NodeState
bool IntraFWDataflow::runAnalysis(const Function& func, NodeState* fState, bool analyzeDueToCallers, set<Function> calleesUpdated)
{
	// Make sure that we've been paired with a valid inter-procedural dataflow analysis
	ROSE_ASSERT(dynamic_cast<InterProceduralDataflow*>(interAnalysis));
	if(analysisDebugLevel>=1) {
		printf("############################\n");
		printf("#### Function %s() ####\n", func.get_name().str());
		printf("############################\n");
		cout << "analyzeDueToCallers="<<analyzeDueToCallers<<" calleesUpdated=";
		for(set<Function>::iterator f=calleesUpdated.begin(); f!=calleesUpdated.end(); f++)
			cout << f->get_name().getString()<<", ";
		cout << "\n";
	}
	
	// Set of functions that have already been visited by this analysis, used
	// to make sure that the dataflow state of previously-visited functions is
	// not re-initialized when they are visited again.
	//static set<Function> visited;
	/*cout << "visited (#"<<visited.size()<<")=\n";
	for(set<Function>::iterator f=visited.begin(); f!=visited.end(); f++)
		cout << "    "<<f->str("        ")<<"\n";*/
	
	// Initialize the lattices used by this analysis, if this is the first time the analysis visits this function
	if(visited.find(func) == visited.end())
	{
		//cout << "Initializing Dataflow State\n"; cout.flush();
		InitDataflowState ids(this/*, initState*/);
		ids.runAnalysis(func, fState);

		//UnstructuredPassInterAnalysis upia_ids(ids);
		//upia_ids.runAnalysis();
		visited.insert(func);
	}

	DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(func.get_definition());
	DataflowNode funcCFGEnd   = cfgUtils::getFuncEndCFG(func.get_definition());
		
	cout << "funcCFGStart="<<funcCFGStart.getNode()<<" = <"<<funcCFGStart.getNode()->unparseToString()<<" | "<<funcCFGStart.getNode()->class_name()<<" | "<<funcCFGStart.getIndex()<<">\n";
	cout << "funcCFGEnd="<<funcCFGEnd.getNode()<<" = <"<<funcCFGEnd.getNode()->unparseToString()<<" | "<<funcCFGEnd.getNode()->class_name()<<" | "<<funcCFGEnd.getIndex()<<">\n";
	
	// Initialize the function's entry NodeState 
	NodeState* entryState = *(NodeState::getNodeStates(funcCFGStart).begin());
	cout << "entryState1 = "<<entryState->str(this, "    ")<<"\n";
	cout << "entryState2 = "<<entryState->str(this, "    ")<<"\n";
		
	printf("before copyLattices on <%s | %s> %d\n", funcCFGStart.getNode()->unparseToString().c_str(), funcCFGStart.getNode()->class_name().c_str(), funcCFGStart.getIndex());
	
	int i=0;
	cout << "Before: entryState-above="<<entryState<<"\n";
	for(vector<Lattice*>::const_iterator l=entryState->getLatticeAbove(this).begin(); l!=entryState->getLatticeAbove(this).end(); l++, i++)
		cout << "Lattice "<<i<<": "<<*l<<" = "<<(*l)->str("            ")<<"\n";
	i=0;
	cout << "Before: fState-above="<<fState<<"\n";
	for(vector<Lattice*>::const_iterator l=fState->getLatticeAbove(this).begin(); l!=fState->getLatticeAbove(this).end(); l++, i++)
		cout << "Lattice "<<i<<": "<<*l<<" = "<<(*l)->str("            ")<<"\n";
	cout << "this="<<this<<"\n";
	
	cout << "entryState3 = "<<entryState->str(this, "    ")<<"\n";
	
	NodeState::copyLattices_aEQa(this, *entryState, /*interAnalysis*/this, *fState);
	i=0;
	cout << "after: entryState-above=\n";
	for(vector<Lattice*>::const_iterator l=entryState->getLatticeAbove(this).begin(); l!=entryState->getLatticeAbove(this).end(); l++, i++)
		cout << "Lattice "<<i<<": "<<(*l)->str("            ")<<"\n";
	
	//printf("IntraFWDataflow::runAnalysis() function %s()\n", func.get_name().str());
	
	// Initialize the set of nodes that this dataflow will iterate over
	VirtualCFG::dataflow it(funcCFGEnd);
	
	// If we're analyzing this function because the dataflow information coming in from its callers
	// has changed, add the function's entry point
	if(analyzeDueToCallers) it.add(funcCFGStart);
	
	// If we're analyzing this function because of a change in the exit dataflow states of some of the 
	// functions called by this function (these functions are recorded in calleesUpdated), add the calls
	// by this function to those functions to the dataflow iterator
	FindAllFunctionCalls fafc(calleesUpdated);
	fafc.runAnalysis(func, fState);
	for(map<Function, set<DataflowNode> >::iterator callee=fafc.getFuncCalls().begin(); callee!=fafc.getFuncCalls().end(); callee++)
		for(set<DataflowNode>::iterator callNode=callee->second.begin(); callNode!=callee->second.end(); callNode++)
		{
			it.add(*callNode);
		}
	
	//cout << "analyzeDueToCallers="<<analyzeDueToCallers<<" #calleesUpdated="<<calleesUpdated.size()<<" it="<<it.str()<<"\n";
	
	// Iterate over the nodes in this function that are downstream from the nodes added above
	//for(VirtualCFG::dataflow it(funcCFGStart, funcCFGEnd); it!=VirtualCFG::dataflow::end(); it++)
	for(; it!=VirtualCFG::dataflow::end(); it++)
	{
		DataflowNode n = *it;
		SgNode* sgn = n.getNode();
		if(analysisDebugLevel>=1){
			printf("====================================================================\n");
			printf("Current Node %p<%s | %s | %d>\n", sgn, sgn->unparseToString().c_str(), sgn->class_name().c_str(), n.getIndex());
		}
		bool modified = false;
		
		// the number of NodeStates associated with the given dataflow node
		int numStates=NodeState::numNodeStates(n);
		// the NodeStates themselves
		const vector<NodeState*> nodeStates = NodeState::getNodeStates(n);
		//printf("                               nodeStates.size()=%d\n", nodeStates.size());
		int i=0;
		//NodeState* state = NodeState::getNodeState(n, 0);
		NodeState* state=NULL;
		//ROSE_ASSERT(state);
		
		// Iterate over all of this node's NodeStates
		//for(int i=0; i<numStates;)
		for(vector<NodeState*>::const_iterator itS = nodeStates.begin(); itS!=nodeStates.end(); )
		{
			state = *itS;
			//printf("                               state=%p\n", state);
				
			// reset the modified state, since only the last NodeState's change matters
			//modified = false; 
			const vector<Lattice*> dfInfoAbove  = state->getLatticeAbove((Analysis*)this);
			const vector<Lattice*> dfInfoBelow  = state->getLatticeBelow((Analysis*)this);
			
			// Ensure that the lattices above this node are initialized
			/*for(vector<Lattice*>::const_iterator itA = dfInfoAbove.begin(); itA != dfInfoAbove.end(); itA++) {
				cout << "Initializing itA="<<(*itA)->str("    ")<<"\n";
				(*itA)->initialize();
				cout << "Result: itA="<<(*itA)->str("    ")<<"\n";
			}*/
			
			// Overwrite the Lattices below this node with the lattices above this node.
			// The transfer function will then operate on these Lattices to produce the
			// correct state below this node.
			
			//printf("                 dfInfoAbove.size()=%d, dfInfoBelow.size()=%d, this=%p\n", dfInfoAbove.size(), dfInfoBelow.size(), this);
			vector<Lattice*>::const_iterator itA, itB;
			int j=0;
			for(itA = dfInfoAbove.begin(), itB = dfInfoBelow.begin();
			    itA != dfInfoAbove.end() && itB != dfInfoBelow.end(); 
			    itA++, itB++, j++)
			{
				if(analysisDebugLevel>=1){
					cout << "    Meet Above: Lattice "<<j<<": \n        "<<(*itA)->str("            ")<<"\n";
					cout << "    Meet Below: Lattice "<<j<<": \n        "<<(*itB)->str("            ")<<"\n";
				}
				(*itB)->copy(*itA);
				/*if(analysisDebugLevel>=1){
					cout << "    Copied Meet Below: Lattice "<<j<<": \n        "<<(*itB)->str("            ")<<"\n";
				}*/
			}
			
			// =================== TRANSFER FUNCTION ===================
			
			// If we're currently at a function call, use the associated inter-procedural
			// analysis to determine the effect of this function call on the dataflow state.
			if(isSgFunctionCallExp(sgn))
			{
				vector<Lattice*>* retState=NULL;
				dynamic_cast<InterProceduralDataflow*>(interAnalysis)->
				      transfer(func, n, *state, dfInfoBelow, &retState, true);
				
				// Incorporate information about the function's return value into the caller's dataflow state
				// as the information of the SgFunctionCallExp
				if(retState && !(retState->size()==0 || (retState->size() == dfInfoBelow.size()))) {
					cout << "#retState="<<retState->size()<<"\n";
					for(vector<Lattice*>::iterator ml=retState->begin(); ml!=retState->end(); ml++)
						cout << "        "<<(*ml)->str("            ")<<"\n";
					cout << "#dfInfoBelow="<<dfInfoBelow.size()<<"\n";
					for(vector<Lattice*>::const_iterator l=dfInfoBelow.begin(); l!=dfInfoBelow.end(); l++)
						cout << "        "<<(*l)->str("            ")<<"\n";
				}
				ROSE_ASSERT(retState==NULL || retState->size()==0 || (retState->size() == dfInfoBelow.size()));
				if(retState)
				{
					vector<Lattice*>::iterator lRet;
					vector<Lattice*>::const_iterator lDF;
					for(lRet=retState->begin(), lDF=dfInfoBelow.begin(); 
					    lRet!=retState->end(); lRet++, lDF++) {
					    cout << "    lDF Before="<<(*lDF)->str("        ")<<"\n";
					    cout << "    lRet Before="<<(*lRet)->str("        ")<<"\n";
						(*lDF)->unProject(isSgFunctionCallExp(sgn), *lRet);
						cout << "    lDF After="<<(*lDF)->str("        ")<<"\n";
					}
				}
			}
			// Otherwise, call the user's transfer function
			//else
				/*modified = */transfer(func, n, *state, dfInfoBelow);
			
			// =================== TRANSFER FUNCTION ===================
			if(analysisDebugLevel>=1)
			{
				j=0;
				for(itB = dfInfoBelow.begin();
				    itB != dfInfoBelow.end(); itB++, j++)
				{
					cout << "    Transferred: Lattice "<<j<<": \n        "<<(*itB)->str("            ")<<"\n";
				}
				printf("    transferred, modified=%d\n", modified);
			}

			// Look at the next NodeState
			i++; itS++;
			// if this is not the last NodeState associated with this CFG node
			//if((i+1)<numStates)
			if(itS!=nodeStates.end())
			{
				// Get the next NodeState
				//NodeState* nextState = NodeState::getNodeState(n, i);
				NodeState* nextState = *itS;
				ROSE_ASSERT(nextState);
				modified = propagateStateToNextNode(
                                 dfInfoBelow, n, i-1,
                                 nextState->getLatticeAbove((Analysis*)this), n) || modified;
         }
			if(analysisDebugLevel>=1){
				printf("    ------------------\n");
			}
		}
		ROSE_ASSERT(state);
		
/*			// if there has been a change in the dataflow state immediately below this node AND*/
		// If this is not the last node in the function
		if(/*modified && */*it != funcCFGEnd)
		{
			// iterate over all descendants
			vector<DataflowEdge> edges = n.outEdges();
			if(analysisDebugLevel>=1) {
				printf("    Descendants (%lu):\n", (unsigned long)(edges.size()));
				printf("    ~~~~~~~~~~~~\n");
			}
			
			//printf("Number of descendants = %d\n", edges.size());
			for(vector<DataflowEdge>::iterator ei = edges.begin(); ei!=edges.end(); ei++)
			{
				// The CFG node corresponding to the current descendant of n
				DataflowNode nextNode = (*ei).target();
				SgNode *nextSgNode = nextNode.getNode();
				if(analysisDebugLevel>=1)
					printf("    Descendant: %p<%s | %s>\n", nextSgNode, nextSgNode->class_name().c_str(), nextSgNode->unparseToString().c_str());
			
				NodeState* nextState = NodeState::getNodeState(nextNode, 0);
				ROSE_ASSERT(nextSgNode && nextState);
				
				// Propagate the Lattices below this node to its descendant
				modified = propagateStateToNextNode(
				                       state->getLatticeBelow((Analysis*)this), n, numStates-1, 
				                       nextState->getLatticeAbove((Analysis*)this), nextNode);
				if(analysisDebugLevel>=1){
					printf("    propagated, modified=%d\n", modified);
					printf("    ^^^^^^^^^^^^^^^^^^\n");
				}
				// If the next node's state gets modified as a result of the propagation, 
				// add the node to the processing queue.
				if(modified)
					it.add(nextNode);
			}
		}
	}
	
/*	cout << "(*(NodeState::getNodeStates(funcCFGEnd).begin()))->getLatticeAbove((Analysis*)this) == fState->getLatticeBelow((Analysis*)this):\n";
	printf("fState->getLatticeBelow((Analysis*)this).size()=%d\n", fState->getLatticeBelow((Analysis*)interAnalysis).size());
	printf("fState->getLatticeAbove((Analysis*)this).size()=%d\n", fState->getLatticeAbove((Analysis*)interAnalysis).size());*/
	// Test if the Lattices at the bottom of the function after the forward analysis are equal to their
	// original values in the function state.
	bool modified = !NodeState::eqLattices(
		                      (*(NodeState::getNodeStates(funcCFGEnd).begin()))->getLatticeAbove((Analysis*)this),
		                      fState->getLatticeBelow(/*interAnalysis*/(Analysis*)this));
	
	// Update the the function's exit NodeState with the final state of this function's dataflow analysis.
	/*NodeState* exitState = *(NodeState::getNodeStates(funcCFGEnd).begin());

	const vector<Lattice*>& funcLatticesExit = exitState->getLatticeAbove(this);
	cout << "    exitState Above\n";
	for(vector<Lattice*>::const_iterator it = funcLatticesExit.begin(); it !=funcLatticesExit.end(); it++)
		cout << "        "<<(*it)->str("        ")<<"\n";

	const vector<Lattice*>& funcLatticesAfter = fState->getLatticeBelow(this);
	cout << "    fState.below Before\n";
	for(vector<Lattice*>::const_iterator it = funcLatticesAfter.begin(); it !=funcLatticesAfter.end(); it++)
		cout << "        "<<(*it)->str("        ")<<"\n";

	cout <<"     Equal = "<<NodeState::eqLattices(exitState->getLatticeAbove((Analysis*)this), fState->getLatticeBelow((Analysis*)this))<<"\n";;
	
	NodeState::copyLattices_bEQa(/ *interAnalysis* /this, *fState, this, *exitState);

	{
		cout << "    fState.below After\n";
		const vector<Lattice*>& funcLatticesAfter = fState->getLatticeBelow(this);
		for(vector<Lattice*>::const_iterator it = funcLatticesAfter.begin(); it!=funcLatticesAfter.end(); it++)
			cout << "        "<<(*it)->str("        ")<<"\n";
	}*/
	
	return modified;
}


/*************************
 *** IntraBWDataflow ***
 *************************/

// Runs the intra-procedural analysis on the given function. Returns true if 
// the function's NodeState gets modified as a result and false otherwise.
// state - the function's NodeState
bool IntraBWDataflow::runAnalysis(const Function& func, NodeState* fState, bool analyzeDueToCallers, set<Function> calleesUpdated)
{
	// Make sure that we've been paired with a valid inter-procedural dataflow analysis
	ROSE_ASSERT(dynamic_cast<InterProceduralDataflow*>(interAnalysis));
	
	if(analysisDebugLevel>=1){
		printf("############################\n");
		printf("#### Function %s() ####\n", func.get_name().str());
		printf("############################\n");
	}
	
	// !!! NEED TO UPDATE ANALYSIS TO ADD FUNCTIONS IN calleesUpdated TO THE LIST OF NODES TO BE ITERATED OVER
	
	// Set of functions that have already been visited by this analysis, used
	// to make sure that the dataflow state of previously-visited functions is
	// not re-initialized when they are visited again.
	//static set<Function> visited;
	/*cout << "visited (#"<<visited.size()<<")=\n";
	for(set<Function>::iterator f=visited.begin(); f!=visited.end(); f++)
		cout << "    "<<f->str("        ")<<"\n";*/
	
	// Initialize the lattices used by this analysis, if this is the first time the analysis visits this function
	if(visited.find(func) == visited.end())
	{
		//cout << "Initializing Dataflow State\n"; cout.flush();
		InitDataflowState ids(this/*, initState*/);
		ids.runAnalysis(func, fState);

		//UnstructuredPassInterAnalysis upia_ids(ids);
		//upia_ids.runAnalysis();
		visited.insert(func);
	}

	DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(func.get_definition());
	DataflowNode funcCFGEnd   = cfgUtils::getFuncEndCFG(func.get_definition());
		
	//printf("funcCFGStart <%s | %s>\n", funcCFGStart.getNode()->unparseToString().c_str(), funcCFGStart.getNode()->class_name().c_str());
	//printf("funcCFGEnd <%s | %s>\n", funcCFGEnd.getNode()->unparseToString().c_str(), funcCFGEnd.getNode()->class_name().c_str());
	
	// Initialize the function's entry NodeState 
	NodeState* entryState = *(NodeState::getNodeStates(funcCFGEnd).rbegin());
	//printf("before copyLattices on <%s | %s>\n", funcCFGStart.getNode()->class_name().c_str(), funcCFGStart.getNode()->unparseToString().c_str());
	NodeState::copyLattices_bEQb(this, *entryState, /*interAnalysis*//*this, */*fState);
	
	//printf("IntraFWDataflow::runAnalysis() function %s()\n", func.get_name().str());
	// Iterate over all the nodes in this function
	for(VirtualCFG::back_dataflow it(funcCFGEnd, funcCFGStart); it!=VirtualCFG::back_dataflow::end(); it++)
	//for(VirtualCFG::back_dataflow it(funcCFGEnd); it!=VirtualCFG::back_dataflow::end(); it++)
	{
		DataflowNode n = *it;
		SgNode* sgn = n.getNode();
		if(analysisDebugLevel>=1){
			printf("====================================================================\n");
			printf("Current Node %p<%s | %s | %d>\n", sgn, sgn->unparseToString().c_str(), sgn->class_name().c_str(), n.getIndex());
		}
		bool modified = false;
		
		// the number of NodeStates associated with the given dataflow node
		int numStates=NodeState::numNodeStates(n);
		// the NodeStates themselves
		const vector<NodeState*> nodeStates = NodeState::getNodeStates(n);
		//printf("                               nodeStates.size()=%d\n", nodeStates.size());
		int i=0;
		//NodeState* state = NodeState::getNodeState(n, 0);
		NodeState* state=NULL;
		//ROSE_ASSERT(state);
		
		// Iterate over all of this node's NodeStates
		//for(int i=0; i<numStates;)
		for(vector<NodeState*>::const_reverse_iterator itS = nodeStates.rbegin(); itS!=nodeStates.rend(); )
		{
			state = *itS;
			//printf("                               state=%p\n", state);
				
			// reset the modified state, since only the last NodeState's change matters
			//modified = false; 
			
			const vector<Lattice*> dfInfoAbove  = state->getLatticeAbove((Analysis*)this);
			const vector<Lattice*> dfInfoBelow  = state->getLatticeBelow((Analysis*)this);
			
			/* // Ensure that the lattices below this node are initialized
			for(vector<Lattice*>::const_iterator itB = dfInfoBelow.begin(); itB != dfInfoBelow.end(); itB++)
				(*itB)->initialize();*/
			
			// Overwrite the Lattices above this node with the lattices below this node.
			// The transfer function will then operate on these Lattices to produce the
			// correct state below this node.
			
			//printf("                 dfInfoAbove.size()=%d, dfInfoBelow.size()=%d, this=%p\n", dfInfoAbove.size(), dfInfoBelow.size(), this);
			vector<Lattice*>::const_iterator itA, itB;
			int j=0;
			for(itA = dfInfoAbove.begin(), itB = dfInfoBelow.begin();
			    itA != dfInfoAbove.end() && itB != dfInfoBelow.end(); 
			    itA++, itB++, j++)
			{
				if(analysisDebugLevel>=1){
					cout << "    Meet Below: Lattice "<<j<<": \n    "<<(*itB)->str("        ")<<"\n";
					cout << "    Meet Above: Lattice "<<j<<": \n    "<<(*itA)->str("        ")<<"\n";
				}
				(*itA)->copy(*itB);
			}
			
			// =================== TRANSFER FUNCTION ===================
			
			// If we're currently at a function call, use the associated inter-procedural
			// analysis to determine the effect of this function call on the dataflow state.
			if(isSgFunctionCallExp(sgn))
			{
				vector<Lattice*>* retState;
				dynamic_cast<InterProceduralDataflow*>(interAnalysis)->
				      transfer(func, n, *state, dfInfoAbove, &retState, true);
				
				// NEED TO INCORPORATE INFORMATION ABOUT RETURN INTO DATAFLOW SOMEHOW
			}
			// Otherwise, call the user's transfer function
			//else
				/*modified = */
			transfer(func, n, *state, dfInfoAbove);
			
			// =================== TRANSFER FUNCTION ===================
			if(analysisDebugLevel>=1)
			{
				j=0;
				for(itA = dfInfoAbove.begin();
				    itA != dfInfoAbove.end(); itA++, j++)
				{
					cout << "    Transferred: Lattice "<<j<<": \n    "<<(*itA)->str("        ")<<"\n";
				}
				//printf("    transferred, modified=%d\n", modified);
			}

			// Look at the next NodeState
			i++; itS++;
			// if this is not the last NodeState associated with this CFG node
			//if((i+1)<numStates)
			if(itS!=nodeStates.rend())
			{
				// Get the next NodeState
				//NodeState* nextState = NodeState::getNodeState(n, i);
				NodeState* nextState = *itS;
				ROSE_ASSERT(nextState);
				modified = propagateStateToNextNode(
                                 dfInfoAbove, n, i-1,
                                 nextState->getLatticeBelow((Analysis*)this), n) || modified;
         }
			if(analysisDebugLevel>=1){
				printf("    ------------------\n");
			}
		}
		ROSE_ASSERT(state);
		
/*			// if there has been a change in the dataflow state immediately below this node AND*/
		// If this is not the last node in the function
		if(/*modified && */*it != funcCFGStart)
		{
			// iterate over all descendants
			vector<DataflowEdge> edges = n.inEdges();
			if(analysisDebugLevel>=1){
				printf("    Descendants (%lu):\n", (unsigned long)(edges.size()));
				printf("    ~~~~~~~~~~~~\n");
			}

			//printf("Number of descendants = %d\n", edges.size());
			for(vector<DataflowEdge>::iterator ei = edges.begin(); ei!=edges.end(); ei++)
			{
				// The CFG node corresponding to the current descendant of n
				DataflowNode nextNode = (*ei).source();
				SgNode *nextSgNode = nextNode.getNode();
				if(analysisDebugLevel>=1)
					printf("    Descendant: %p<%s | %s>\n", nextSgNode, nextSgNode->class_name().c_str(), nextSgNode->unparseToString().c_str());
			
				NodeState* nextState = NodeState::getNodeState(nextNode, 0);
				ROSE_ASSERT(nextSgNode && nextState);
				
				// Propagate the Lattices below this node to its descendant
				modified = propagateStateToNextNode(
				                       state->getLatticeAbove((Analysis*)this), n, numStates-1, 
				                       nextState->getLatticeBelow((Analysis*)this), nextNode);
				if(analysisDebugLevel>=1){
					printf("    propagated, modified=%d\n", modified);
					printf("    ^^^^^^^^^^^^^^^^^^\n");
				}
				// If the next node's state gets modified as a result of the propagation, 
				// add the node to the processing queue.
				if(modified)
					it.add(nextNode);
			}
		}
	}
	
/*	cout << "(*(NodeState::getNodeStates(funcCFGEnd).begin()))->getLatticeAbove((Analysis*)this) == fState->getLatticeBelow((Analysis*)this):\n";
	printf("fState->getLatticeAbove((Analysis*)this).size()=%d\n", fState->getLatticeAbove((Analysis*)interAnalysis).size());
	printf("fState->getLatticeBelow((Analysis*)this).size()=%d\n", fState->getLatticeBelow((Analysis*)interAnalysis).size());*/
	// Test if the Lattices at the bottom of the function after the forward analysis are equal to their
	// original values in the function state.
	bool modified = !NodeState::eqLattices(
		                      (*(NodeState::getNodeStates(funcCFGStart).rbegin()))->getLatticeBelow((Analysis*)this),
		                      fState->getLatticeAbove(/*interAnalysis*/(Analysis*)this));
		
	// Update the the function's exit NodeState with the final state of this function's dataflow analysis.
	NodeState* exitState = *(NodeState::getNodeStates(funcCFGStart).rbegin());
	NodeState::copyLattices_aEQb(/*interAnalysis*/this, *fState, /*this, */*exitState);
		
	return modified;
}

/**********************************
 *** IntraFWPerVariableDataflow ***
 ********************************** /

IntraFWPerVariableDataflow::IntraFWPerVariableDataflow(bool includeScalars, bool includeArrays)
{
	this->includeScalars = includeScalars;
	this->includeArrays = includeArrays;
	constVarLattices_init = false;
}

// returns the set of global variables(scalars and/or arrays)
varIDSet& IntraFWPerVariableDataflow::getGlobalVars()
{
	if(includeScalars)
	{
		if(includeArrays)
			return varSets::getGlobalVars(getProject());
		else
			return varSets::getGlobalScalars(getProject());
	}
	else
		return varSets::getGlobalArrays(getProject());
}

// returns the set of variables(scalars and/or arrays) declared in this function
varIDSet& IntraFWPerVariableDataflow::getLocalVars(Function func)
{
	if(includeScalars)
	{
		if(includeArrays)
			return varSets::getLocalVars(func);
		else
			return varSets::getLocalScalars(func);
	}
	else
		return varSets::getLocalArrays(func);
}

// returns the set of variables(scalars and/or arrays) referenced in this function
varIDSet& IntraFWPerVariableDataflow::getRefVars(Function func)
{
	if(includeScalars)
	{
		if(includeArrays)
			return varSets::getLocalVars(func);
		else
			return varSets::getLocalScalars(func);
	}
	else
		return varSets::getLocalArrays(func);
}

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
vector<Lattice*> IntraFWPerVariableDataflow::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
{
	printf("IntraFWPerVariableDataflow::genInitState(%s(), <%s | %s>)\n", func.get_name().str(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
		
	// initState contains the non-variable state
	vector<Lattice*> initState;
	Lattice* newLattice = genInitNonVarState(func, n, state);
	if(newLattice!=NULL) initState.push_back(newLattice);
	varIDSet& refVars = getRefVars(func);

	// if varLatticeIndex has not yet been set up for this function
	if(varLatticeIndex.find(func) == varLatticeIndex.end())
	{
		map<varID, int> varIndex;
		int varLatticeCntr=initState.size();
		for(varIDSet::iterator it = refVars.begin(); it!=refVars.end(); it++, varLatticeCntr++)
			varIndex[*it] = varLatticeCntr;
		varLatticeIndex[func] = varIndex;
	}
	
	// iterate over all the variables (arrays and/or scalars) referenced in this function
	// adding their initial lattices to initState
	for(varIDSet::iterator it = refVars.begin(); it!=refVars.end(); it++)
	{
		newLattice = genInitVarState(func, n, state);
		if(newLattice!=NULL) initState.push_back(newLattice);
	}
	
	return initState;
}

Lattice* IntraFWPerVariableDataflow::getVarLattice(const Function& func, const varID& var, const vector<Lattice*>& dfInfo)
{
	// initialize constVarLattices, if necessary
	if(!constVarLattices_init)
	{
		constVarLattices = genConstVarLattices();
		constVarLattices_init=true;
	}
	
	map<varID, Lattice*>::iterator constIt;
	// if this is a constant variable
	if((constIt = constVarLattices.find(var)) != constVarLattices.end())
		// return its lattice
		return constIt->second;
	// else, if this is a regular variable
	else
	{
		int varIndex = (varLatticeIndex.find(func)->second).find(var)->second;
		return dfInfo[varIndex];
	}
}*/

/******************************************************
 ***            printDataflowInfoPass               ***
 *** Prints out the dataflow information associated ***
 *** with a given analysis for every CFG node a     ***
 *** function.                                      ***
 ******************************************************/

//vector<Lattice*> printDataflowInfoPass::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void printDataflowInfoPass::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                                         vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
	
{
	//vector<Lattice*> initState;
	initLattices.push_back((Lattice*)(new BoolAndLattice()));
	//return initState;
}
	
bool printDataflowInfoPass::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	printf("-----------------------------------------\n");
	printf("Node: <%s | %s>\n", n.getNode()->unparseToString().c_str(), n.getNode()->class_name().c_str());
	// print out all the dataflow facts associated with analysis at this node
	
	const /*map <int, NodeFact*>*/vector<NodeFact*> facts = state.getFacts(analysis);
	int i=0;
	for(/*map <int, NodeFact*>*/vector<NodeFact*>::const_iterator it = facts.begin(); it!=facts.end(); it++, i++)
	{
		//cout << "Fact "<<it->first<<": \n    "<<it->second->str("        ")<<"\n";
		cout << "Fact "<<i<<": \n    "<<(*it)->str("        ")<<"\n";
	}
	
	const vector<Lattice*> dfInfoAbove  = state.getLatticeAbove((Analysis*)analysis);
	const vector<Lattice*> dfInfoBelow  = state.getLatticeBelow((Analysis*)analysis);
	
	vector<Lattice*>::const_iterator itA, itB;
	for(itA = dfInfoAbove.begin(), itB = dfInfoBelow.begin();
	    itA != dfInfoAbove.end() && itB != dfInfoBelow.end(); 
	    itA++, itB++)
	{
		cout << "    Lattice Above "<<*itA<<": \n    "<<(*itA)->str("        ")<<"\n";
		cout << "    Lattice Below "<<*itB<<": \n    "<<(*itB)->str("        ")<<"\n";
	}
	
	return dynamic_cast<BoolAndLattice*>(dfInfo[0])->set(true);
}

/*************************************
 *** UnstructuredPassInterDataflow ***
 *************************************/
void UnstructuredPassInterDataflow::runAnalysis()
{
	set<FunctionState*> allFuncs = FunctionState::getAllDefinedFuncs();
	
	// iterate over all functions with bodies
	for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)
	{
		const Function& func = (*it)->func;
		FunctionState* fState = FunctionState::getDefinedFuncState(func);
		
		// Call the current intra-procedural dataflow as if it were a generic analysi
		intraAnalysis->runAnalysis(func, &(fState->state));
	}
}

/************************
 *** DFStateAtReturns ***
 ************************/

/*DFStateAtReturns::DFStateAtReturns()
{}*/

DFStateAtReturns::DFStateAtReturns(vector<Lattice*>& latsAtFuncReturn, vector<Lattice*>& latsRetVal) : 
		latsAtFuncReturn(latsAtFuncReturn), latsRetVal(latsRetVal)
{}

// Returns a copy of this node fact
NodeFact* DFStateAtReturns::copy() const{
/*	DFStateAtReturns* newState = new DFStateAtReturns();
	for(vector<Lattice*>::const_iterator l=latsAtFuncReturn.begin(); l!=latsAtFuncReturn.end(); l++)
		newState->latsAtFuncReturn.push_back((*l)->copy());
	for(vector<Lattice*>::const_iterator l=latsRetVal.begin(); l!=latsRetVal.end(); l++)
		newState->latsRetVal.push_back((*l)->copy());
	return newState;*/
	// We don't need a copy method
	ROSE_ASSERT(0);
	return NULL;
}

// Applies the MergeAllReturnStates analysis on the given function, incorporating the results into
// the lattices held by this object.
// Returns true of the lattices change as a result and false otherwise.
bool DFStateAtReturns::mergeReturnStates(const Function& func, FunctionState* fState, IntraProceduralDataflow* intraAnalysis)
{
	// Create a MergeAllReturnStates, initializing its lattices with this object's lattices to make sure
	// that the analysis pass simply updates these original lattices in-place, also updating the state of 
	// this object in the process.
	//cout << "DFStateAtReturns: STARTING PASS\n";
	MergeAllReturnStates mars(intraAnalysis, latsAtFuncReturn, latsRetVal/*, MergeAllReturnStates::above*/);
	mars.runAnalysis(func, &(fState->state));
	// If this is the first time we're computing the lattices at return statements, initialize lattices
	if(latsAtFuncReturn.size()==0) latsAtFuncReturn = mars.getMergedLatsRetStmt();
	if(latsRetVal.size()==0)       latsRetVal       = mars.getMergedLatsRetVal();
	
	if(analysisDebugLevel>=1) {
		cout << "DFStateAtReturns final #latsAtFuncReturn="<<latsAtFuncReturn.size()<<": \n";
		for(vector<Lattice*>::iterator l=latsAtFuncReturn.begin(); l!=latsAtFuncReturn.end(); l++)
				cout << "    "<<(*l)->str("            ")<<"\n";
		
		cout << "DFStateAtReturns final #latsRetVal="<<latsRetVal.size()<<": \n";
		for(vector<Lattice*>::iterator l=latsRetVal.begin(); l!=latsRetVal.end(); l++)
				cout << "    "<<(*l)->str("            ")<<"\n";
		
		cout << "modified = "<<mars.getModified()<<"\n";
	}
	return mars.getModified();
}

string DFStateAtReturns::str(string indent) {
	ostringstream outs;
	outs << "<DFStateAtReturns: \n";
	outs << indent << " latsAtFuncReturn=\n";
	for(vector<Lattice*>::iterator l=latsAtFuncReturn.begin(); l!=latsAtFuncReturn.end(); ) {
		outs << indent << "    "<<(*l)->str(indent+"    ");
		l++;
		if(l!=latsAtFuncReturn.end()) outs <<"\n";
	}
	outs << "\n";
	outs << indent << " latsRetVal=\n";
	for(vector<Lattice*>::iterator l=latsRetVal.begin(); l!=latsRetVal.end(); ) {
		outs << indent << "    "<<(*l)->str(indent+"    ");
		l++;
		if(l!=latsRetVal.end()) outs <<"\n";
	}
	outs << ">";
	return outs.str();
}

/****************************
 *** MergeAllReturnStates ***
 ****************************/

void MergeAllReturnStates::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
	SgNode* sgn = n.getNode();
	if(analysisDebugLevel>=1) printf("MergeAllReturnStates::visit() func=%s() sgn=%p<%s | %s>\n", func.get_name().getString().c_str(), sgn, sgn->unparseToString().c_str(), sgn->class_name().c_str());
	//cout << "visit <<<: modified="<<modified<<"\n";
	
	// If this is an explicit return statement
	if(isSgReturnStmt(sgn)) {
		if(analysisDebugLevel>=1)
			printf("MergeAllReturnStates::visit() isSgReturnStmt(sgn)->get_expression()=%p<%s | %s>\n", 
			       isSgReturnStmt(sgn)->get_expression(), isSgReturnStmt(sgn)->get_expression()->unparseToString().c_str(), 
			       isSgReturnStmt(sgn)->get_expression()->class_name().c_str());

		ROSE_ASSERT(NodeState::getNodeStates(n).size()==1);
		NodeState* state = *(NodeState::getNodeStates(n).begin());

		// Incorporate the entire dataflow state at the return statement
		if(analysisDebugLevel>=1) cout << "    Merging dataflow state at return statement\n";
		modified = mergeLats(mergedLatsRetStmt, state->getLatticeAbove(analysis)) || modified;

		// Incorporate just the portion of the dataflow state that corresponds to the value being returned,
		// assuming that any information is available
		vector<Lattice*> exprLats;
		for(vector<Lattice*>::const_iterator l=state->getLatticeAbove(analysis).begin(); l!=state->getLatticeAbove(analysis).end(); l++)
			exprLats.push_back((*l)->project(isSgReturnStmt(sgn)->get_expression()));
		if(analysisDebugLevel>=1) cout << "    Merging dataflow state of return value\n";
		modified = mergeLats(mergedLatsRetVal, exprLats) || modified; 
	}
	// If this is the end of a function, which is an implicit return that has no return value
	else if(isSgFunctionDefinition(sgn)) {
		if(analysisDebugLevel>=1)
			printf("MergeAllReturnStates::visit() isSgFunctionDefinition\n");
		
		ROSE_ASSERT(NodeState::getNodeStates(n).size()==1);
		NodeState* state = *(NodeState::getNodeStates(n).begin());
		
		// Incorporate the entire dataflow state at the implicit return statement
		modified = mergeLats(mergedLatsRetStmt, state->getLatticeAbove(analysis)) || modified;
	}
	//cout << "visit >>>: modified="<<modified<<"\n";
}

// Merges the lattices in the given vector into mergedLats. 
// Returns true of mergeLats changes as a result and false otherwise.
bool MergeAllReturnStates::mergeLats(vector<Lattice*>& mergedLat, const vector<Lattice*>& lats) {
	// If this is the first return statement we've observed, initialize mergedLat with its lattices
	if(mergedLat.size()==0) {
		if(analysisDebugLevel>=1) {
			cout << "    Fresh lattice: \n";
			for(vector<Lattice*>::const_iterator l=lats.begin(); l!=lats.end(); l++) {
				mergedLat.push_back((*l)->copy());
				cout << "        "<<(*l)->str("        ")<<"\n";
			}
		}
		return true;
	// Otherwise, merge lats into mergedLats
	} else {
		if(mergedLat.size()!=lats.size()) {
			cout << "#mergedLat="<<mergedLat.size()<<"\n";
			for(vector<Lattice*>::iterator ml=mergedLat.begin(); ml!=mergedLat.end(); ml++)
				cout << "        "<<(*ml)->str("            ")<<"\n";
			cout << "#lats="<<lats.size()<<"\n";
			for(vector<Lattice*>::const_iterator l=lats.begin(); l!=lats.end(); l++)
				cout << "        "<<(*l)->str("            ")<<"\n";
		}
		ROSE_ASSERT(mergedLat.size()==lats.size());
		vector<Lattice*>::const_iterator l;
		vector<Lattice*>::iterator ml;
		bool modified = false;
		if(analysisDebugLevel>=1) cout << "    Updating lattice: \n";
		for(l=lats.begin(), ml=mergedLat.begin(); l!=lats.end(); l++, ml++) {
			if(analysisDebugLevel>=1) {
				cout << "        Update: "<<(*l)->str("            ")<<"\n";
				cout << "        + Orig: "<<(*ml)->str("            ")<<"\n";
			}
			modified = (*ml)->meetUpdate(*l) || modified;
			if(analysisDebugLevel>=1) {
				cout << "        => New: "<<(*ml)->str("            ")<<"\n";
				cout << "        modified="<<modified<<"\n";
			}
		}
		return modified;
	}
}

// Deallocates all the merged lattices
MergeAllReturnStates::~MergeAllReturnStates()
{
	/*for(vector<Lattice*>::iterator ml=mergedLat.begin(); ml!=mergedLat.end(); ml++)
		delete *ml;*/
}

/*************************************************
 *** ContextInsensitiveInterProceduralDataflow ***
 *************************************************/

ContextInsensitiveInterProceduralDataflow::ContextInsensitiveInterProceduralDataflow
	      (IntraProceduralDataflow* intraDataflowAnalysis, SgIncidenceDirectedGraph* graph) :
	                       InterProceduralAnalysis((IntraProceduralAnalysis*)intraDataflowAnalysis),
	                       InterProceduralDataflow(intraDataflowAnalysis), 
	                       TraverseCallGraphDataflow(graph)
{
	// Record that the functions that have no callers are being analyzed because the data flow at their
	// callers (the environment) has changed. This is done to jump-start the analysis.
	for(set<const CGFunction*>::iterator func=noPred.begin(); func!=noPred.end(); func++)
		remainingDueToCallers.insert(**func);
	
	// Record as part of each FunctionState the merged lattice states above the function's return statements
	set<FunctionState*> allFuncs = FunctionState::getAllDefinedFuncs();
	for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)	
	{
		FunctionState* funcS = *it;
		if(funcS->func.get_definition())
//DFStateAtReturns NEED REFERENCES TO vector<Lattice*>'S RATHER THAN COPIES OF THEM
			funcS->state.addFact(this, 0, new DFStateAtReturns(funcS->state.getLatticeBelowMod((Analysis*)intraDataflowAnalysis), 
			                                                   funcS->retState.getLatticeBelowMod((Analysis*)intraDataflowAnalysis)));
	}
	
/*	set<FunctionState*> allFuncs = FunctionState::getAllDefinedFuncs();
	//Function main;
	
	// initialize the dataflow state of all functions with bodies
	for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)	
	{
		FunctionState* funcS = *it;
		Function func = funcS->func;
		vector<Lattice*> initState = intraDataflowAnalysis->genInitState(
		                                 func, func.get_definition()->cfgForBeginning(), funcS->state);

printf("ContextInsensitiveInterProceduralDataflow initState %s():\n", (*it)->func.get_name().str());
for(vector<Lattice*>::iterator it = initState.begin(); 
    it!=initState.end(); it++)
{	
	cout << *it << ": " << (*it)->str("    ") << "\n";
}
	
		funcS->state.setLattices(this, initState);
		
		 // remember the main() function
		if(strcmp(func.get_name().str(), "main")==0)
			main = func;
	}
	
	for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)	
	{
		FunctionState* funcS = *it;
	}*/
}
	
// The transfer function that is applied to SgFunctionCallExp nodes to perform the appropriate state transfers
// fw - =true if this is a forward analysis and =false if this is a backward analysis
// n - the dataflow node that is being processed
// state - The NodeState object that describes the dataflow state immediately before (if fw=true) or immediately after 
//         (if fw=false) the SgFunctionCallExp node, as established by earlier analysis passes
// dfInfo - The Lattices that this transfer function operates on. The function propagates them 
//          to the calling function and overwrites them with the dataflow result of calling this function.
// retState - Pointer reference to a Lattice* vector that will be assigned to point to the lattices of
//          the function call's return value. The callee may not modify these lattices.
// Returns true if any of the input lattices changed as a result of the transfer function and
//    false otherwise.	
bool ContextInsensitiveInterProceduralDataflow::transfer(
	                     const Function& func, const DataflowNode& n, NodeState& state, 
	                     const vector<Lattice*>& dfInfo, vector<Lattice*>** retState, bool fw)
{
	bool modified = false;
	SgFunctionCallExp* call = isSgFunctionCallExp(n.getNode());
	Function callee(call);
	ROSE_ASSERT(call);
	
	if(analysisDebugLevel > 0)
		printf("ContextInsensitiveInterProceduralDataflow::transfer %s()=>%s()\n", func.get_name().str(), callee.get_name().str());
	if(callee.get_definition())
	{
		FunctionState* funcS = FunctionState::getDefinedFuncState(callee);
		// The lattices before the function (forward: before=above, after=below; backward: before=below, after=above)
		const vector<Lattice*>* funcLatticesBefore;
		if(fw) funcLatticesBefore = &(funcS->state.getLatticeAbove((Analysis*)intraAnalysis));
		else   funcLatticesBefore = &(funcS->state.getLatticeBelow((Analysis*)intraAnalysis));
			
		//if(analysisDebugLevel > 0)
		//	printf("        dfInfo.size()=%d, funcLatticesBefore->size()=%d, this=%p\n", dfInfo.size(), funcLatticesBefore->size(), this);
		
		// Update the function's entry/exit state	with the caller's state at the call site
		vector<Lattice*>::const_iterator itCalleeBefore, itCallerBefore;
		for(itCallerBefore = dfInfo.begin(), itCalleeBefore = funcLatticesBefore->begin(); 
		    itCallerBefore!=dfInfo.end() && itCalleeBefore!=funcLatticesBefore->end(); 
		    itCallerBefore++, itCalleeBefore++)
		{
			Lattice* calleeL = *itCalleeBefore;
			Lattice* callerL = *itCallerBefore;
			
			if(analysisDebugLevel>=1) {
				cout << "      callerL=["<<calleeL<<"] "<<callerL->str("        ")<<"\n";
				cout << "      Before calleeL=["<<calleeL<<"] "<<calleeL->str("        ")<<"\n";
			}
			// Create a copy of the current lattice, remapped for the called function's variables
			Lattice* remappedL = callerL->copy();
			map<varID, varID> argParamMap;
			FunctionState::setArgParamMap(call, argParamMap);
			/*cout << "#argParamMap="<<argParamMap.size()<<"\n";
			for(map<varID, varID>::iterator it = argParamMap.begin(); it!=argParamMap.end(); it++)
			{ printf("argParamMap[%s] = %s \n", it->first.str().c_str(), it->second.str().c_str()); }*/
			remappedL->remapVars(argParamMap, callee);
		
			//cout << "      remappedL=["<<calleeL<<"] "<<remappedL->str("        ")<<"\n";
			
			// update the callee's Lattice with the new information at the call site
			modified = calleeL->meetUpdate(remappedL) || modified;
			
			if(analysisDebugLevel>=1)
				cout << "      After modified = "<<modified << "calleeL=["<<calleeL<<"] "<<calleeL->str("        ")<<"\n";
					
//!!!		delete remappedL;
		}
		
		// If this resulted in the dataflow information before the callee changing, add it to the remaining list.
		if(modified) {
			if(analysisDebugLevel > 0)
				printf("ContextInsensitiveInterProceduralDataflow::transfer Incoming Dataflow info modified\n");
			// Record that the callee function needs to be re-analyzed because of new information from the caller
			TraverseCallGraphDataflow::addToRemaining(getFunc(callee));
			remainingDueToCallers.insert(getFunc(callee));
		}
		
		// The lattices after the function (forward: before=above, after=below; backward: before=below, after=above).
		const vector<Lattice*>* funcLatticesAfter;
		if(fw) funcLatticesAfter = &(funcS->state.getLatticeBelow((Analysis*)intraAnalysis));
		else   funcLatticesAfter = &(funcS->state.getLatticeAbove((Analysis*)intraAnalysis));
			
		//printf("        funcLatticesAfter->size()=%d\n",  funcLatticesAfter->size());
		if(analysisDebugLevel>=1)
				cout << "      ---------------------------------------\n";
		
		// Transfer the result of the function call into the dfInfo Lattices.
		vector<Lattice*>::const_iterator itCalleeAfter, itCallerAfter;
		for(itCallerAfter = dfInfo.begin(), itCalleeAfter = funcLatticesAfter->begin(); 
		    itCallerAfter!=dfInfo.end() && itCalleeAfter!=funcLatticesAfter->end(); 
		    itCallerAfter++, itCalleeAfter++)
		{
			Lattice* callerL = *itCallerAfter;
			Lattice* calleeL = *itCalleeAfter;
			//cout << "      calleeL-after=["<<calleeL<<"] "<<calleeL->str("        ")<<"\n";
			
			// Create a copy of the current lattice, remapped for the callee function's variables
			Lattice* remappedL = calleeL->copy();
			if(analysisDebugLevel>=1)
				cout << "      remappedL-after=["<<remappedL<<"] "<<calleeL->str("        ")<<"\n";
			map<varID, varID> paramArgByRefMap;
			FunctionState::setParamArgByRefMap(call, paramArgByRefMap);
			/*cout << "#paramArgByRefMap="<<paramArgByRefMap.size()<<"\n";
			for(map<varID, varID>::iterator it = paramArgByRefMap.begin(); it!=paramArgByRefMap.end(); it++)
			{ printf("paramArgByRefMap[%s] = %s \n", it->first.str().c_str(), it->second.str().c_str()); }*/
			remappedL->remapVars(paramArgByRefMap, func);
			
			//cout << "      callerL-after=["<<callerL<<"] "<<callerL->str("        ")<<"\n";
			//cout << "      +remappedL-after=["<<remappedL<<"] "<<remappedL->str("        ")<<"\n";
			
			// update the caller's Lattice with the new information at the call site
			callerL->incorporateVars(remappedL);
			
			if(analysisDebugLevel>=1)
				cout << "      ==> callerL-after=["<<callerL<<"] "<<callerL->str("        ")<<"\n";
			//cout << "      calleeL-after=["<<calleeL<<"] "<<calleeL->str("        ")<<"\n";
			modified = true;
	//!!!		delete remappedL;
		}
		
		// Point retState to the lattices of the function's return values
		*retState = &(funcS->retState.getLatticeBelowMod((Analysis*)intraAnalysis));
		
		/*cout << "      retState="<<retState<<"\n";
		for(vector<Lattice*>::iterator l=(*retState)->begin(); l!=(*retState)->end(); l++)
			cout << "          "<<(*l)->str("            ")<<"\n";*/
	}
	// Don't do anything for functions with no definitions
	else
	{
		
	}
	
	return modified;
}

// Uses TraverseCallGraphDataflow to traverse the call graph.
void ContextInsensitiveInterProceduralDataflow::runAnalysis()
{
	traverse();
}

// Runs the intra-procedural analysis every time TraverseCallGraphDataflow passes a function.
void ContextInsensitiveInterProceduralDataflow::visit(const CGFunction* funcCG)
{
	Function func = *funcCG;
	if(func.get_definition())
	{
		FunctionState* fState = FunctionState::getDefinedFuncState(func);
		
		if(analysisDebugLevel>=1){
			printf("ContextInsensitiveInterProceduralDataflow function %s\n", func.get_name().str()); fflush(stdout);
		}
	
		/*if(analysisDebugLevel>=1) {	
			for(vector<Lattice*>::const_iterator it = fState->state.getLatticeAbove((Analysis*)intraAnalysis).begin();
			    it!=fState->state.getLatticeAbove((Analysis*)intraAnalysis).end(); it++)
			{
				cout << (*it)->str("    ") << "\n";	
			}
		}*/
		
		// Run the intra-procedural dataflow analysis on the current function
		dynamic_cast<IntraProceduralDataflow*>(intraAnalysis)->
		                        runAnalysis(func, &(fState->state), 
		                                    remainingDueToCallers.find(func)!=remainingDueToCallers.end(),
		                                    remainingDueToCalls[func]);
		
		// Merge the dataflow states above all the return statements in the function, storing the results in Fact 0 of the function
		DFStateAtReturns* dfsar = dynamic_cast<DFStateAtReturns*>(fState->state.getFact(this, 0));
		bool modified = dfsar->mergeReturnStates(func, fState, dynamic_cast<IntraProceduralDataflow*>(intraAnalysis));	
		
		if(analysisDebugLevel>=1) {
			printf("function %s %s\n", func.get_name().str(), modified? "modified": "not modified");
			cout << "remaining = ";
			for(list<const CGFunction*>::iterator f=remaining.begin(); f!=remaining.end(); f++)
				cout << (*f)->get_name().getString() << ", ";
			cout << "\n";
			
			/*printf("State below:\n");
			for(vector<Lattice*>::const_iterator it = fState->state.getLatticeBelow((Analysis*)this).begin();
			    it!=fState->state.getLatticeBelow((Analysis*)this).end(); it++)
			{
				cout << (*it)->str("    ") << "\n";	
			}*/
			cout << "States at Return Statements:\n";
			for(vector<Lattice*>::iterator it = dfsar->getLatsAtFuncReturn().begin();
			    it!=dfsar->getLatsAtFuncReturn().end(); it++)
			{
				cout << (*it)->str("    ") << "\n";	
			}
			
			vector<Lattice*> retState = fState->retState.getLatticeBelow((Analysis*)intraAnalysis);
			cout << "retState: \n";
			for(vector<Lattice*>::iterator it = retState.begin(); it!=retState.end(); it++)
				cout << (*it)->str("    ") << "\n";	
			
			cout << "States of Return Values: "<<&(dfsar->getLatsRetVal())<<"\n";
			for(vector<Lattice*>::iterator it = dfsar->getLatsRetVal().begin();
			    it!=dfsar->getLatsRetVal().end(); it++)
			{
				cout << (*it)->str("    ") << "\n";	
			}
		}
		
		// If this function's final dataflow state was modified, its callers must be 
		// placed back onto the remaining list, recording that they're on the list
		// because of their calls to this function
		if(modified)
		{
			//cout << "Inserting Callers\n";
			for(CGFunction::iterator it = funcCG->predecessors(); it!=funcCG->end(); it++)
			{
				const CGFunction* caller = it.getTarget(functions);
				
				//cout << "Caller of "<<funcCG->get_name().getString()<<": "<<caller->get_name().getString()<<"\n";
				addToRemaining(caller);
				remainingDueToCalls[caller].insert(func);
			}
		}
	}
}
