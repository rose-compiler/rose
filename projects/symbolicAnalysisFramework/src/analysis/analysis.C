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
	
if(analysisDebugLevel>=1){
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
	//printf("InterProceduralDataflow() intraAnalysis=%p, intraDataflowAnalysis=%p\n", (Analysis*)intraAnalysis, intraDataflowAnalysis);
	set<FunctionState*> allFuncs = FunctionState::getAllDefinedFuncs();
	
	// Initialize the dataflow state of all functions with bodies
	for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)	
	{
		FunctionState* funcS = *it;
		Function func = funcS->func;
		if(func.get_definition())
		{
			/*vector<Lattice*> initLats = intraDataflowAnalysis->genInitState(
			                                 func, func.get_definition()->cfgForBeginning(), funcS->state);*/
			vector<Lattice*>  initLats;
			vector<NodeFact*> initFacts;
			intraDataflowAnalysis->genInitState(func, func.get_definition()->cfgForBeginning(), funcS->state,
			                                    initLats, initFacts);
	/*
	if(analysisDebugLevel>=1){	
		printf("InterProceduralAnalysis initState %s():\n", (*it)->func.get_name().str());
		for(vector<Lattice*>::iterator it = initState.begin(); 
		    it!=initState.end(); it++)
		{	
			cout << *it << ": " << (*it)->str("    ") << "\n";
		}
	}*/
			funcS->state.setLattices((Analysis*)intraAnalysis, initLats);
			funcS->state.setFacts((Analysis*)intraAnalysis, initFacts);
			
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
		                                    
	//printf("InitDataflowState::visit() calling state.setLattices()\n");
	state.setLattices((Analysis*)dfAnalysis, initLats);
	state.setFacts((Analysis*)dfAnalysis, initFacts);
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

/*************************
 *** IntraFWDataflow ***
 *************************/

// Runs the intra-procedural analysis on the given function. Returns true if 
// the function's NodeState gets modified as a result and false otherwise.
// state - the function's NodeState
bool IntraFWDataflow::runAnalysis(const Function& func, NodeState* fState)
{
	if(analysisDebugLevel>=1){
		printf("############################\n");
		printf("#### Function %s() ####\n", func.get_name().str());
		printf("############################\n");
	}
	
	// Initialize the lattices used by this analysis
	{
		InitDataflowState ids(this/*, initState*/);
		ids.runAnalysis(func, fState);

		//UnstructuredPassInterAnalysis upia_ids(ids);
		//upia_ids.runAnalysis();
	}

	DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(func.get_definition());
	DataflowNode funcCFGEnd = cfgUtils::getFuncEndCFG(func.get_definition());
	
	// Initialize the function's entry NodeState 
	NodeState* entryState = *(NodeState::getNodeStates(funcCFGStart).begin());
	//printf("before copyLattices on <%s | %s>\n", funcCFGStart.getNode()->class_name().c_str(), funcCFGStart.getNode()->unparseToString().c_str());
	NodeState::copyLattices_aEQa(this, *entryState, /*interAnalysis*/this, *fState);
	
	//printf("IntraFWDataflow::runAnalysis() function %s()\n", func.get_name().str());
	// Iterate over all the nodes in this function
	for(VirtualCFG::dataflow it(funcCFGStart, funcCFGEnd); it!=VirtualCFG::dataflow::end(); it++)
	{
		DataflowNode n = *it;
		SgNode* sgn = n.getNode();
		if(analysisDebugLevel>=1){
			printf("====================================================================\n");
			printf("Current Node %p<%s | %s>\n", sgn, sgn->unparseToString().c_str(), sgn->class_name().c_str());
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
			
			// Overwrite the Lattices below this node with the lattices above this node.
			// The transfer function will then operate on these Lattices to produce the
			// correct state below this node.
			const vector<Lattice*> dfInfoAbove  = state->getLatticeAbove((Analysis*)this);
			const vector<Lattice*> dfInfoBelow  = state->getLatticeBelow((Analysis*)this);
			
			//printf("                 dfInfoAbove.size()=%d, dfInfoBelow.size()=%d, this=%p\n", dfInfoAbove.size(), dfInfoBelow.size(), this);
			vector<Lattice*>::const_iterator itA, itB;
			int j=0;
			for(itA = dfInfoAbove.begin(), itB = dfInfoBelow.begin();
			    itA != dfInfoAbove.end() && itB != dfInfoBelow.end(); 
			    itA++, itB++, j++)
			{
				if(analysisDebugLevel>=1){
					cout << "    Meet Above: Lattice "<<j<<": \n    "<<(*itA)->str("        ")<<"\n";
					cout << "    Meet Below: Lattice "<<j<<": \n    "<<(*itB)->str("        ")<<"\n";
				}
				(*itB)->copy(*itA);
			}
			
			// =================== TRANSFER FUNCTION ===================
			
			// If we're currently at a function call, use the associated inter-procedural
			// analysis to determine the effect of this function call on the dataflow state.
			if(isSgFunctionCallExp(sgn))
			{
				dynamic_cast<InterProceduralDataflow*>(interAnalysis)->
				      transfer(func, n, *state, dfInfoBelow, true);
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
					cout << "    Transferred: Lattice "<<j<<": \n    "<<(*itB)->str("        ")<<"\n";
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
				modified = propagateFWStateToNextNode(
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
			if(analysisDebugLevel>=1){
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
				modified = propagateFWStateToNextNode(
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
	NodeState* exitState = *(NodeState::getNodeStates(funcCFGEnd).begin());
	NodeState::copyLattices_bEQa(/*interAnalysis*/this, *fState, this, *exitState);
		
	return modified;
}

// Propagates the dataflow info from the current node's NodeState (curNodeState) to the next node's 
//     NodeState (nextNodeState).
// Returns true if the next node's meet state is modified and false otherwise.
bool IntraFWDataflow::propagateFWStateToNextNode(
                      const vector<Lattice*>& curNodeState, DataflowNode curNode, int curNodeIndex,
                      const vector<Lattice*>& nextNodeState, DataflowNode nextNode)
{
	bool modified = false;
	vector<Lattice*>::const_iterator itC, itN;
	if(analysisDebugLevel>=1){
		printf("\n        Propagating to Next Node: %p[%s]<%s>\n", nextNode.getNode(), nextNode.getNode()->class_name().c_str(), nextNode.getNode()->unparseToString().c_str());
		int j=0;
		for(itN = nextNodeState.begin(); itN != nextNodeState.end(); itN++, j++)
		{
			cout << "        Next node above: Lattice "<<j<<": \n"<<(*itN)->str("            ")<<"\n";
		}
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
		{
			modified = (*itN)->meetUpdate(*itC) || modified;
		}
		else
		{
			//InfiniteLattice* meetResult = (InfiniteLattice*)itN->second->meet(itC->second);
			InfiniteLattice* meetResult = dynamic_cast<InfiniteLattice*>((*itN)->copy());
			meetResult->meetUpdate(*itC);
			
			cout << "*itN: " << dynamic_cast<InfiniteLattice*>(*itN)->str("") << "\n";
			cout << "*itC: " << dynamic_cast<InfiniteLattice*>(*itC)->str("") << "\n";
			cout << "meetResult: " << meetResult->str("") << "\n";
		
			// widen the resulting meet
			modified =  dynamic_cast<InfiniteLattice*>(*itN)->widenUpdate(meetResult);
			delete meetResult;
		}
	}
	
	if(analysisDebugLevel>=1){
		if(modified)
		{
			cout << "        Next node's in-data modified. Adding...\n";
			int j=0;
			for(itN = nextNodeState.begin(); itN != nextNodeState.end(); itN++, j++)
			{
				cout << "        Propagated: Lattice "<<j<<": \n"<<(*itN)->str("            ")<<"\n";
			}
		}
		else
			cout << "        No modification on this node\n";
	}

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
		intraAnalysis->runAnalysis(func, &(fState->state));
	}
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
	
// the transfer function that is applied to SgFunctionCallExp nodes to perform the appropriate state transfers
// fw - =true if this is a forward analysis and =false if this is a backward analysis
// n - the dataflow node that is being processed
// state - the NodeState object that describes the dataflow state immediately before (if fw=true) or immediately after 
//         (if fw=false) the SgFunctionCallExp node, as established by earlier analysis passes
// dfInfo - the Lattices that this transfer function operates on. The function propagates them 
//          to the calling function and overwrites them with the dataflow result of calling this function.
// Returns true if any of the input lattices changed as a result of the transfer function and
//    false otherwise.	
bool ContextInsensitiveInterProceduralDataflow::transfer(
	                     const Function& func, const DataflowNode& n, NodeState& state, 
	                     const vector<Lattice*>& dfInfo, bool fw)
{
	bool modified = false;
	SgFunctionCallExp* call = isSgFunctionCallExp(n.getNode());
	Function callee(call);
	ROSE_ASSERT(call);
	
	//if(analysisDebugLevel > 0)
	//	printf("ContextInsensitiveInterProceduralDataflow::transfer %s()=>%s()\n", func.get_name().str(), callee.get_name().str());
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
		vector<Lattice*>::const_iterator itCallee, itCaller;
		for(itCaller = dfInfo.begin(), itCallee = funcLatticesBefore->begin(); 
		    itCaller!=dfInfo.end() && itCallee!=funcLatticesBefore->end(); 
		    itCaller++, itCallee++)
		{
			Lattice* calleeL = *itCallee;
			Lattice* callerL = *itCaller;
			
			// Create a copy of the current lattice, remapped for the called function's variables
			Lattice* remappedL = callerL->copy();
			map<varID, varID> argParamMap;
			FunctionState::setArgParamMap(call, argParamMap);
			/*for(map<varID, varID>::iterator it = argParamMap.begin(); it!=argParamMap.end(); it++)
			{ printf("argParamMap[%s] = %s \n", it->first.str().c_str(), it->second.str().c_str()); }*/
			remappedL->remapVars(argParamMap, callee);
		
			// update the callee's Lattice with the new information at the call site
			modified = calleeL->meetUpdate(remappedL) || modified;
			
			//if(analysisDebugLevel > 0)
			//cout << "modified="<<modified<<"  calleeL = ["<<calleeL<<"] "<<calleeL->str("")<<"\n";
		
//!!!		delete remappedL;
		}
		
		// If this resulted in the dataflow information before the callee changing, add it to the remaining list.
		if(modified) TraverseCallGraphDataflow::addToRemaining(getFunc(callee));
		
		// The lattices after the function (forward: before=above, after=below; backward: before=below, after=above).
		const vector<Lattice*>* funcLatticesAfter;
		if(fw) funcLatticesAfter = &(funcS->state.getLatticeBelow((Analysis*)intraAnalysis));
		else   funcLatticesAfter = &(funcS->state.getLatticeAbove((Analysis*)intraAnalysis));
			
		//printf("        funcLatticesAfter->size()=%d\n",  funcLatticesAfter->size());
		
		// Transfer the result of the function call into the dfInfo Lattices.
		for(itCaller = dfInfo.begin(), itCallee = funcLatticesAfter->begin(); 
		    itCaller!=dfInfo.end() && itCallee!=funcLatticesAfter->end(); 
		    itCaller++, itCallee++)
		{
			Lattice* callerL = *itCaller;
			Lattice* calleeL = *itCallee;
			//cout << "      calleeL=["<<calleeL<<"] "<<calleeL->str()<<"\n";
			
			// Create a copy of the current lattice, remapped for the callee function's variables
			Lattice* remappedL = calleeL->copy();
			map<varID, varID> paramArgByRefMap;
			FunctionState::setParamArgByRefMap(call, paramArgByRefMap);
	/*		for(map<varID, varID>::iterator it = paramArgByRefMap.begin(); it!=paramArgByRefMap.end(); it++)
			{ printf("paramArgByRefMap[%s] = %s \n", it->first.str().c_str(), it->second.str().c_str()); }*/
			remappedL->remapVars(paramArgByRefMap, func);
			
			// update the callee's Lattice with the new information at the call site
			callerL->incorporateVars(remappedL);
			//cout << "      calleeL="<<calleeL->str()<<"\n";
			//cout << "      remappedL="<<remappedL->str()<<"\n";
			//cout << "      callerL="<<callerL->str()<<"\n";
			modified = true;
	//!!!		delete remappedL;
		}
	}
	// Don't do anything for functions with no bodies
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
void ContextInsensitiveInterProceduralDataflow::visit(const CGFunction* func)
{
	if(func->get_definition())
	{
		FunctionState* fState = FunctionState::getDefinedFuncState(*func);
		
		if(analysisDebugLevel>=1){
			printf("ContextInsensitiveInterProceduralDataflow function %s\n", func->get_name().str()); fflush(stdout);
		}
		
		Function func_arg = *func;
	
		if(analysisDebugLevel>=1){	
			for(vector<Lattice*>::const_iterator it = fState->state.getLatticeAbove((Analysis*)this).begin();
			    it!=fState->state.getLatticeAbove((Analysis*)this).end(); it++)
			{
				cout << (*it)->str("    ") << "\n";	
			}
		}
		
		// Run the intra-procedural dataflow analysis on the current function
		bool modified = dynamic_cast<IntraProceduralDataflow*>(intraAnalysis)->runAnalysis(func_arg, &(fState->state));
	
		if(analysisDebugLevel>=1){	
			printf("function %s %s\n", func->get_name().str(), modified? "modified": "not modified");
			printf("State below:\n");
			for(vector<Lattice*>::const_iterator it = fState->state.getLatticeBelow((Analysis*)this).begin();
			    it!=fState->state.getLatticeBelow((Analysis*)this).end(); it++)
			{
				cout << (*it)->str("    ") << "\n";	
			}
		}
		
		// If this function's final dataflow state was modified, its callers must be 
		// placed back onto the remaining list.
		if(modified)
		{
			for(CGFunction::iterator it = func->predecessors(); it!=func->end(); it++)
			{
				const CGFunction* caller = it.getTarget(functions);
				addToRemaining(caller);
			}
		}
	}
}
