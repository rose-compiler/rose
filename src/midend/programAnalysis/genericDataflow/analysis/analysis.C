#include "genericDataflowCommon.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "stringify.h"
#include <vector>
#include <set>
#include <map>

int analysisDebugLevel=1;

using namespace std;
using namespace Rose;

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
        ROSE_ASSERT(func.get_definition()!=NULL);
        DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(func.get_definition(),filter);

     // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
     // DataflowNode funcCFGEnd = cfgUtils::getFuncEndCFG(func.get_definition(), filter);

     // DQ (12/10/2016): If this function does not have side-effects then we can eliminate it as well.
        cfgUtils::getFuncEndCFG(func.get_definition(), filter);

        if(analysisDebugLevel>=2)
                Dbg::dbg << "UnstructuredPassIntraAnalysis::runAnalysis() function "<<func.get_name().getString()<<"()\n";
        
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
        // Go through functions one by one, call an intra-procedural analysis on each of them
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
       filter = intraDataflowAnalysis->filter; // propagate the CFG filter from intra- to inter-level, or the default filter will kick in at inter-level.
        Dbg::dbg << "InterProceduralDataflow() intraAnalysis="<<intraAnalysis<<", intraDataflowAnalysis="<<intraDataflowAnalysis<<endl;
        set<FunctionState*> allFuncs = FunctionState::getAllDefinedFuncs();
        
        // Initialize the dataflow state of all functions with bodies
        for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)        
        {
                FunctionState* funcS = *it;
                Function func = funcS->func;
                //Dbg::dbg << "func="<<func.get_name().getString()<<"() func.get_definition()="<<func.get_definition()<<endl;
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
                        
                        Dbg::dbg << "InterProceduralAnalysis initLats at Ending "<<(*it)->func.get_name().getString()<<"): node=%s\n", , end.str().c_str());
                                for(vector<Lattice*>::iterator it = initLats.begin(); 
                                    it!=initLats.end(); it++)
                                {       
                                        Dbg::dbg << *it << ": " << (*it)->str("    ") << endl;
                                }
                        */
                        DataflowNode begin(func.get_definition()->cfgForBeginning(), filter);
                        Dbg::dbg << "begin="<<begin.getNode()<<" = ["<<Dbg::escape(begin.getNode()->unparseToString())<<" | "<<begin.getNode()->class_name()<<"]"<<endl;
                        intraDataflowAnalysis->genInitState(func, DataflowNode(func.get_definition()->cfgForBeginning(), filter), funcS->state,
                                                            initLats, initFacts);
                        // Make sure that the starting lattices are initialized
                        for(vector<Lattice*>::iterator it=initLats.begin(); it!=initLats.end(); it++)
                                (*it)->initialize();
                        
                        /*if(analysisDebugLevel>=1){
                                Dbg::dbg << "InterProceduralAnalysis initLats at Beginning "<<func.get_name().getString()<<"(): node="<<begin.str()<<endl;
                                for(vector<Lattice*>::iterator it = initLats.begin(); 
                                    it!=initLats.end(); it++)
                                {       
                                        Dbg::dbg << *it << ": " << (*it)->str("    ") << endl;
                                }
                        }*/
                        funcS->state.setLattices((Analysis*)intraAnalysis, initLats);
                        funcS->state.setFacts((Analysis*)intraAnalysis, initFacts);
                        Dbg::dbg << "Initialized state of function "<<func.get_name().getString()<<"(), state="<<(&(funcS->state))<<endl;
                        Dbg::dbg << "    "<<funcS->state.str(intraDataflowAnalysis, "    ")<<endl;
                        
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
                        Dbg::dbg << "        funcLatticesBefore->size()="<<funcLatticesBefore->size()<<", intraAnalysis="<<intraAnalysis<<endl;*/
                }
        }
}

/*************************
 *** InitDataflowState ***
 *************************/

void InitDataflowState::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
        SgNode* sgn = n.getNode();
        if(analysisDebugLevel>=2)
                Dbg::dbg << "InitDataflowState::visit() sgn="<<sgn<<"["<<sgn->class_name()<<" | "<<Dbg::escape(sgn->unparseToString())<<"], dfAnalysis="<<dfAnalysis<<endl;
        
        // generate a new initial state for this node
        vector<Lattice*>  initLats;
        vector<NodeFact*> initFacts;
        dfAnalysis->genInitState(func, n, state, initLats, initFacts);
        
        /*if(analysisDebugLevel>=2){
                int i=0;
                for(vector<Lattice*>::iterator l=initLats.begin(); l!=initLats.end(); l++, i++)
                        Dbg::dbg << "Lattice "<<i<<": "<<(*l)->str("            ")<<endl;
                
                i=0;
                for(vector<NodeFact*>::iterator f=initFacts.begin(); f!=initFacts.end(); f++, i++)
                        Dbg::dbg << "Lattice "<<i<<": "<<(*f)->str("            ")<<endl;
        }*/
                                                    
        //Dbg::dbg << "InitDataflowState::visit() calling state.setLattices()"<<endl;
        state.setLattices((Analysis*)dfAnalysis, initLats);
        state.setFacts((Analysis*)dfAnalysis, initFacts);
        
        if(analysisDebugLevel>=2){
                Dbg::dbg << "    state="<<state.str((Analysis*)dfAnalysis, "    ")<<endl;
        }
        
        /*vector<Lattice*> initState = dfAnalysis->genInitState(func, n, state);
        Dbg::dbg << "InitDataflowState::visit() 1"<<endl;
                        
        for(int i=0; i<initState.size(); i++)
        {
                Dbg::dbg << "                         i="<<i<<", initState[i]="<<initState[i]->str("")<<endl;
                state.addLattice((Analysis*)dfAnalysis, i, initState[i]);
                Dbg::dbg << "                         state->getLatticeAbove((Analysis*)dfAnalysis).size()="<<state.getLatticeAbove((Analysis*)dfAnalysis).size()<<endl, );
                //Dbg::dbg << printf("                         state->getLatticeBelow((Analysis*)dfAnalysis).size()="<<state.getLatticeBelow((Analysis*)dfAnalysis).size()<<endl;
        }*/
        
        //const vector<Lattice*>& masterLatBel = state.getLatticeBelow((Analysis*)dfAnalysis);
        //printf("    creator=%p, state=%p, masterLatBel.size()=%d\n", (Analysis*)dfAnalysis, &state, masterLatBel.size());
}


/****************************
 *** FindAllFunctionCalls ***
 ****************************/
 
void FindAllFunctionCalls::visit(const Function&, const DataflowNode& n, NodeState& state)
{
        SgNode* sgn = n.getNode();
        if (analysisDebugLevel>=2){
                Dbg::dbg << "FindAllFunctionCalls::visit() sgn="<<sgn<<"["<<sgn->class_name()
                         <<" | "<<Dbg::escape(sgn->unparseToString())<<"]"<<endl;
        }
        
        // If this is a function call, find the function that is being called and if it is in funcsToFind, record the call in
        // funcCalls
        if (SgFunctionCallExp *fcall = isSgFunctionCallExp(sgn)) {
                // We can't compare SgFunctionDeclaration pointers willy nilly since a single function might have multiple
                // IR nodes (declaration, first declaration, defining declaration, etc.). Therefore, we need to convert the
                // callee to a unique node the same as we did for the set of Function objects in the funcsToFind data member.
                SgFunctionDeclaration *callee = Function::getCanonicalDecl(fcall->getAssociatedFunctionDeclaration());
                assert(callee!=NULL);

                for (set<Function>::const_iterator fi=funcsToFind.begin(); fi!=funcsToFind.end(); ++fi) {
                        if (fi->get_declaration() == callee) {
                                funcCalls[*fi].insert(n);
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
                Dbg::dbg << "\n        Propagating to Next Node: "<<nextNode.getNode()<<"["<<nextNode.getNode()->class_name()<<" | "<<Dbg::escape(nextNode.getNode()->unparseToString())<<"]"<<endl;
                int j;
                for(j=0, itC = curNodeState.begin(); itC != curNodeState.end(); itC++, j++)
                        Dbg::dbg << "        Current node: Lattice "<<j<<": \n            "<<(*itC)->str("            ")<<endl;
                for(j=0, itN = nextNodeState.begin(); itN != nextNodeState.end(); itN++, j++)
                        Dbg::dbg << "        Next/Descendant node: Lattice before propagation "<<j<<": \n            "<<(*itN)->str("            ")<<endl;
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
                        if(analysisDebugLevel>=1)
                           Dbg::dbg << "        Finite lattice: using regular meetUpdate from current'lattic into next node's lattice... "<<endl;
                        modified = (*itN)->meetUpdate(*itC) || modified;
                }
                else
                {
                        //InfiniteLattice* meetResult = (InfiniteLattice*)itN->second->meet(itC->second);
                        InfiniteLattice* meetResult = dynamic_cast<InfiniteLattice*>((*itN)->copy());
                        Dbg::dbg << "        *itN: " << dynamic_cast<InfiniteLattice*>(*itN)->str("            ") << endl;
                        Dbg::dbg << "        *itC: " << dynamic_cast<InfiniteLattice*>(*itC)->str("            ") << endl;
                        meetResult->meetUpdate(*itC);
                        Dbg::dbg << "        meetResult: " << meetResult->str("            ") << endl;
                
                        // Widen the resulting meet
                        modified =  dynamic_cast<InfiniteLattice*>(*itN)->widenUpdate(meetResult);
                        delete meetResult;
                }
        }
        
        if(analysisDebugLevel>=1) {
                if(modified)
                {
                        Dbg::dbg << "        Next node's lattice *modified* by the propagation. "<<endl;
                        int j=0;
                        for(itN = nextNodeState.begin(); itN != nextNodeState.end(); itN++, j++)
                        {
                                Dbg::dbg << "        Modified lattice "<<j<<": \n            "<<(*itN)->str("            ")<<endl;
                        }
                }
                else
                        Dbg::dbg << "        Next node's lattice is *unchanged* by the propagation. "<<endl;
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
        printf("IntraFWPerVariableDataflow::genInitState(%s(), [%s | %s])\n", func.get_name().getString(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
                
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
        Dbg::dbg << "-----#############################--------\n";
        Dbg::dbg << "Node: ["<<Dbg::escape(n.getNode()->unparseToString())<<" | "<< n.getNode()->class_name()<<"]\n";
        // print out all the dataflow facts associated with analysis at this node
        
        const /*map <int, NodeFact*>*/vector<NodeFact*> facts = state.getFacts(analysis);
        int i=0;
        for(/*map <int, NodeFact*>*/vector<NodeFact*>::const_iterator it = facts.begin(); it!=facts.end(); it++, i++)
        {
                //Dbg::dbg << "Fact "<<it->first<<": \n    "<<it->second->str("        ")<<endl;
                Dbg::dbg << "Fact "<<i<<": \n    "<<(*it)->str("        ")<<endl;
        }
        
        const vector<Lattice*> dfInfoAbove  = state.getLatticeAbove((Analysis*)analysis);
        const vector<Lattice*> dfInfoBelow  = state.getLatticeBelow((Analysis*)analysis);
        
        vector<Lattice*>::const_iterator itA, itB;
        for(itA = dfInfoAbove.begin(), itB = dfInfoBelow.begin();
            itA != dfInfoAbove.end() && itB != dfInfoBelow.end(); 
            itA++, itB++)
        {
                Dbg::dbg << "    Lattice Above "<<*itA<<": \n    "<<(*itA)->str("        ")<<endl;
                Dbg::dbg << "    Lattice Below "<<*itB<<": \n    "<<(*itB)->str("        ")<<endl;
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
/*      DFStateAtReturns* newState = new DFStateAtReturns();
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
        if(analysisDebugLevel>=1) Dbg::enterFunc("Merging Return States");
        MergeAllReturnStates mars(intraAnalysis, latsAtFuncReturn, latsRetVal/*, MergeAllReturnStates::above*/);
        mars.runAnalysis(func, &(fState->state));
        // If this is the first time we're computing the lattices at return statements, initialize lattices
        if(latsAtFuncReturn.size()==0) latsAtFuncReturn = mars.getMergedLatsRetStmt();
        if(latsRetVal.size()==0)       latsRetVal       = mars.getMergedLatsRetVal();
        
        if(analysisDebugLevel>=1) {
                Dbg::dbg << "DFStateAtReturns final #latsAtFuncReturn="<<latsAtFuncReturn.size()<<": \n";
                for(vector<Lattice*>::iterator l=latsAtFuncReturn.begin(); l!=latsAtFuncReturn.end(); l++)
                                Dbg::dbg << "    "<<(*l)->str("            ")<<endl;
                
                Dbg::dbg << "DFStateAtReturns final #latsRetVal="<<latsRetVal.size()<<": \n";
                for(vector<Lattice*>::iterator l=latsRetVal.begin(); l!=latsRetVal.end(); l++)
                                Dbg::dbg << "    "<<(*l)->str("            ")<<endl;
                
                Dbg::dbg << "modified = "<<mars.getModified()<<endl;
        }
        bool modified = mars.getModified();
        if(analysisDebugLevel>=1) Dbg::exitFunc("Merging Return States");
        return modified;
}

string DFStateAtReturns::str(string indent) {
        ostringstream outs;
        outs << "<DFStateAtReturns: \n";
        outs << indent << " latsAtFuncReturn=\n";
        for(vector<Lattice*>::iterator l=latsAtFuncReturn.begin(); l!=latsAtFuncReturn.end(); ) {
                outs << indent << "    "<<(*l)->str(indent+"    ");
                l++;
                if(l!=latsAtFuncReturn.end()) outs <<endl;
        }
        outs << endl;
        outs << indent << " latsRetVal=\n";
        for(vector<Lattice*>::iterator l=latsRetVal.begin(); l!=latsRetVal.end(); ) {
                outs << indent << "    "<<(*l)->str(indent+"    ");
                l++;
                if(l!=latsRetVal.end()) outs <<endl;
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
        if(analysisDebugLevel>=1) Dbg::dbg << "MergeAllReturnStates::visit() func="<<func.get_name().getString()<<"() sgn="<<sgn<<"["<<Dbg::escape(sgn->unparseToString())<<" | "<<sgn->class_name()<<"]\n";
        //Dbg::dbg << "visit {{{: modified="<<modified<<endl;
        
        // If this is an explicit return statement
        if(isSgReturnStmt(sgn)) {
                if(analysisDebugLevel>=1)
                        Dbg::dbg << "MergeAllReturnStates::visit() isSgReturnStmt(sgn)->get_expression()="<<isSgReturnStmt(sgn)->get_expression()<<"["<<Dbg::escape(isSgReturnStmt(sgn)->get_expression()->unparseToString())<<" | "<<isSgReturnStmt(sgn)->get_expression()->class_name()<<"]\n";

                ROSE_ASSERT(NodeState::getNodeStates(n).size()==1);
                NodeState* state = *(NodeState::getNodeStates(n).begin());

                // Incorporate the entire dataflow state at the return statement
                if(analysisDebugLevel>=1) Dbg::dbg << "    Merging dataflow state at return statement\n";
                modified = mergeLats(mergedLatsRetStmt, state->getLatticeAbove(analysis)) || modified;

                // Incorporate just the portion of the dataflow state that corresponds to the value being returned,
                // assuming that any information is available
                vector<Lattice*> exprLats;
                for(vector<Lattice*>::const_iterator l=state->getLatticeAbove(analysis).begin(); l!=state->getLatticeAbove(analysis).end(); l++)
                        exprLats.push_back((*l)->project(isSgReturnStmt(sgn)->get_expression()));
                if(analysisDebugLevel>=1) Dbg::dbg << "    Merging dataflow state of return value\n";
                modified = mergeLats(mergedLatsRetVal, exprLats) || modified; 
        }
        // If this is the end of a function, which is an implicit return that has no return value
        else if(isSgFunctionDefinition(sgn)) {
                assert(!isSgTemplateFunctionDefinition(sgn));
                if(analysisDebugLevel>=1)
                        Dbg::dbg << "MergeAllReturnStates::visit() isSgFunctionDefinition\n";
                
                ROSE_ASSERT(NodeState::getNodeStates(n).size()==1);
                NodeState* state = *(NodeState::getNodeStates(n).begin());
                
                // Incorporate the entire dataflow state at the implicit return statement
                modified = mergeLats(mergedLatsRetStmt, state->getLatticeAbove(analysis)) || modified;
        }
        //Dbg::dbg << "visit >>>: modified="<<modified<<endl;
}

// Merges the lattices in the given vector into mergedLats. 
// Returns true of mergeLats changes as a result and false otherwise.
bool MergeAllReturnStates::mergeLats(vector<Lattice*>& mergedLat, const vector<Lattice*>& lats) {
        // If this is the first return statement we've observed, initialize mergedLat with its lattices
        if(mergedLat.size()==0) {
                if(analysisDebugLevel>=1) {
                        Dbg::dbg << "    Fresh lattice: \n";
                        for(vector<Lattice*>::const_iterator l=lats.begin(); l!=lats.end(); l++) {
                                mergedLat.push_back((*l)->copy());
                                Dbg::dbg << "        "<<(*l)->str("        ")<<endl;
                        }
                }
                return true;
        // Otherwise, merge lats into mergedLats
        } else {
                if(mergedLat.size()!=lats.size()) {
                        Dbg::dbg << "#mergedLat="<<mergedLat.size()<<endl;
                        for(vector<Lattice*>::iterator ml=mergedLat.begin(); ml!=mergedLat.end(); ml++)
                                Dbg::dbg << "        "<<(*ml)->str("            ")<<endl;
                        Dbg::dbg << "#lats="<<lats.size()<<endl;
                        for(vector<Lattice*>::const_iterator l=lats.begin(); l!=lats.end(); l++)
                                Dbg::dbg << "        "<<(*l)->str("            ")<<endl;
                }
                ROSE_ASSERT(mergedLat.size()==lats.size());
                vector<Lattice*>::const_iterator l;
                vector<Lattice*>::iterator ml;
                bool modified = false;
                if(analysisDebugLevel>=1) Dbg::dbg << "    Updating lattice: \n";
                for(l=lats.begin(), ml=mergedLat.begin(); l!=lats.end(); l++, ml++) {
                        if(analysisDebugLevel>=1) {
                                Dbg::dbg << "        Update: "<<(*l)->str("            ")<<endl;
                                Dbg::dbg << "        + Orig: "<<(*ml)->str("            ")<<endl;
                        }
                        modified = (*ml)->meetUpdate(*l) || modified;
                        if(analysisDebugLevel>=1) {
                                Dbg::dbg << "        => New: "<<(*ml)->str("            ")<<endl;
                                Dbg::dbg << "        modified="<<modified<<endl;
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
                if(funcS->func.get_definition()) {
//DFStateAtReturns NEED REFERENCES TO vector<Lattice*>'S RATHER THAN COPIES OF THEM
                        std::vector<Lattice *> empty;
                        funcS->state.setLattices(intraDataflowAnalysis, empty);
                        funcS->retState.setLattices(intraDataflowAnalysis, empty);
                        funcS->state.addFact(this, 0, new DFStateAtReturns(funcS->state.getLatticeBelowMod((Analysis*)intraDataflowAnalysis), 
                                                                           funcS->retState.getLatticeBelowMod((Analysis*)intraDataflowAnalysis)));
                        Dbg::dbg << "Return state for function " << funcS << " " << funcS->func.get_name().getString() << endl
                                 << "funcS->state" << funcS->state.str(intraDataflowAnalysis) << endl;
                        //                                 << "funcS->retState="<<  funcS->retState.str(intraDataflowAnalysis) << endl;
                }
        }
        
/*      set<FunctionState*> allFuncs = FunctionState::getAllDefinedFuncs();
        //Function main;
        
        // initialize the dataflow state of all functions with bodies
        for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)        
        {
                FunctionState* funcS = *it;
                Function func = funcS->func;
                vector<Lattice*> initState = intraDataflowAnalysis->genInitState(
                                                 func, func.get_definition()->cfgForBeginning(), funcS->state);

printf("ContextInsensitiveInterProceduralDataflow initState %s():\n", (*it)->func.get_name().getString());
for(vector<Lattice*>::iterator it = initState.begin(); 
    it!=initState.end(); it++)
{       
        Dbg::dbg << *it << ": " << (*it)->str("    ") << endl;
}
        
                funcS->state.setLattices(this, initState);
                
                 // remember the main() function
                if(strcmp(func.get_name().getString(), "main")==0)
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
            Dbg::dbg << "ContextInsensitiveInterProceduralDataflow::transfer "
                     <<func.get_name().getString()<<"()=>"<<callee.get_name().getString()<<"()\n";

        if(callee.get_definition())
        {
                FunctionState* funcS = FunctionState::getDefinedFuncState(callee);
                // The lattices before the function (forward: before=above, after=below; backward: before=below, after=above)
                const vector<Lattice*>* funcLatticesBefore;
                if(fw) funcLatticesBefore = &(funcS->state.getLatticeAbove((Analysis*)intraAnalysis));
                else   funcLatticesBefore = &(funcS->state.getLatticeBelow((Analysis*)intraAnalysis));
                        
                //if(analysisDebugLevel > 0)
                //      printf("        dfInfo.size()=%d, funcLatticesBefore->size()=%d, this=%p\n",
                //             dfInfo.size(), funcLatticesBefore->size(), this);
                
                // Update the function's entry/exit state       with the caller's state at the call site
                vector<Lattice*>::const_iterator itCalleeBefore, itCallerBefore;
                for(itCallerBefore = dfInfo.begin(), itCalleeBefore = funcLatticesBefore->begin(); 
                    itCallerBefore!=dfInfo.end() && itCalleeBefore!=funcLatticesBefore->end(); 
                    itCallerBefore++, itCalleeBefore++)
                {
                        Lattice* calleeL = *itCalleeBefore;
                        Lattice* callerL = *itCallerBefore;
                        
                        if(analysisDebugLevel>=1) {
                                Dbg::dbg << "      callerL=["<<calleeL<<"] "<<callerL->str("        ")<<endl;
                                Dbg::dbg << "      Before calleeL=["<<calleeL<<"] "<<calleeL->str("        ")<<endl;
                        }
                        // Create a copy of the current lattice, remapped for the called function's variables
                        Lattice* remappedL = callerL->copy();
                        map<varID, varID> argParamMap;
                        FunctionState::setArgParamMap(call, argParamMap);
                        /*Dbg::dbg << "#argParamMap="<<argParamMap.size()<<endl;
                        for(map<varID, varID>::iterator it = argParamMap.begin(); it!=argParamMap.end(); it++)
                        { printf("argParamMap[%s] = %s \n", it->first.str().c_str(), it->second.str().c_str()); }*/
                        remappedL->remapVars(argParamMap, callee);
                
                        Dbg::dbg << "      remappedL=["<<calleeL<<"] "<<remappedL->str("        ")<<endl;
                        
                        // update the callee's Lattice with the new information at the call site
                        modified = calleeL->meetUpdate(remappedL) || modified;
                        
                        if(analysisDebugLevel>=1)
                                Dbg::dbg << "      After modified = "<<modified
                                         << "calleeL=["<<calleeL<<"] "<<calleeL->str("        ")<<endl;
                                        
//!!!           delete remappedL;
                }
                
                // If this resulted in the dataflow information before the callee changing, add it to the remaining list.
                if(modified) {
                        if(analysisDebugLevel > 0)
                                Dbg::dbg << "ContextInsensitiveInterProceduralDataflow::transfer Incoming Dataflow info modified\n";
                        // Record that the callee function needs to be re-analyzed because of new information from the caller
                        TraverseCallGraphDataflow::addToRemaining(getFunc(callee));
                        ROSE_ASSERT(getFunc(callee) != NULL);
                        remainingDueToCallers.insert(getFunc(callee));
                }
                
                // The lattices after the function (forward: before=above, after=below; backward: before=below, after=above).
                const vector<Lattice*>* funcLatticesAfter;
                if(fw) funcLatticesAfter = &(funcS->state.getLatticeBelow((Analysis*)intraAnalysis));
                else   funcLatticesAfter = &(funcS->state.getLatticeAbove((Analysis*)intraAnalysis));
                        
                //Dbg::dbg << "        funcLatticesAfter->size()="<<funcLatticesAfter->size()<<endl;
                if(analysisDebugLevel>=1)
                                Dbg::dbg << "      ----%%%%%%%%%%%%%%%%%%%%---------\n";
                
                // Transfer the result of the function call into the dfInfo Lattices.
                vector<Lattice*>::const_iterator itCalleeAfter, itCallerAfter;
                for(itCallerAfter = dfInfo.begin(), itCalleeAfter = funcLatticesAfter->begin(); 
                    itCallerAfter!=dfInfo.end() && itCalleeAfter!=funcLatticesAfter->end(); 
                    itCallerAfter++, itCalleeAfter++)
                {
                        Lattice* callerL = *itCallerAfter;
                        Lattice* calleeL = *itCalleeAfter;
                        //Dbg::dbg << "      calleeL-after=["<<calleeL<<"] "<<calleeL->str("        ")<<endl;
                        
                        // Create a copy of the current lattice, remapped for the callee function's variables
                        Lattice* remappedL = calleeL->copy();
                        if(analysisDebugLevel>=1)
                                Dbg::dbg << "      remappedL-after=["<<remappedL<<"] "
                                         <<calleeL->str("        ")<<endl << remappedL->str(" ")<<endl;
                        map<varID, varID> paramArgByRefMap;
                        FunctionState::setParamArgByRefMap(call, paramArgByRefMap);
                        /*Dbg::dbg << "#paramArgByRefMap="<<paramArgByRefMap.size()<<endl;
                        for(map<varID, varID>::iterator it = paramArgByRefMap.begin(); it!=paramArgByRefMap.end(); it++)
                        { Dbg::dbg << "paramArgByRefMap["<<it->first.str()<<"] = "<<it->second.str()<<endl; */
                        remappedL->remapVars(paramArgByRefMap, func);
                        
                        //Dbg::dbg << "      callerL-after=["<<callerL<<"] "<<callerL->str("        ")<<endl;
                        Dbg::dbg << "      +remappedL-after=["<<remappedL<<"] "<<remappedL->str("        ")<<endl;
                        
                        // update the caller's Lattice with the new information at the call site
                        callerL->incorporateVars(remappedL);
                        
                        if(analysisDebugLevel>=1)
                                Dbg::dbg << "      ==> callerL-after=["<<callerL<<"] "<<callerL->str("        ")<<endl;
                        //Dbg::dbg << "      calleeL-after=["<<calleeL<<"] "<<calleeL->str("        ")<<endl;
                        modified = true;
        //!!!           delete remappedL;
                }
                
                // Point retState to the lattices of the function's return values
                *retState = &(funcS->retState.getLatticeBelowMod((Analysis*)intraAnalysis));
                
                /*Dbg::dbg << "      retState="<<retState<<endl;
                for(vector<Lattice*>::iterator l=(*retState)->begin(); l!=(*retState)->end(); l++)
                        Dbg::dbg << "          "<<(*l)->str("            ")<<endl;*/
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
                assert(fState!=NULL);
                
                IntraProceduralDataflow *intraDataflow = dynamic_cast<IntraProceduralDataflow *>(intraAnalysis);
                assert(intraDataflow!=NULL);
                if (intraDataflow->visited.find(func) == intraDataflow->visited.end()) {
                        vector<Lattice*>  initLats;
                        vector<NodeFact*> initFacts;
                        intraDataflow->genInitState(func, cfgUtils::getFuncStartCFG(func.get_definition(), filter),
                                                    fState->state, initLats, initFacts);
                        fState->state.setLattices(intraAnalysis, initLats);
                        fState->state.setFacts(intraAnalysis, initFacts);
                }

                if(analysisDebugLevel>=1){
                        Dbg::dbg << "ContextInsensitiveInterProceduralDataflow function "<<func.get_name().getString()<<endl;
                }
        
                /*if(analysisDebugLevel>=1) {   
                        for(vector<Lattice*>::const_iterator it = fState->state.getLatticeAbove((Analysis*)intraAnalysis).begin();
                            it!=fState->state.getLatticeAbove((Analysis*)intraAnalysis).end(); it++)
                        {
                                Dbg::dbg << (*it)->str("    ") << endl; 
                        }
                }*/
                
                // Run the intra-procedural dataflow analysis on the current function
                dynamic_cast<IntraProceduralDataflow*>(intraAnalysis)->
                                        runAnalysis(func, &(fState->state), 
                                                    remainingDueToCallers.find(func)!=remainingDueToCallers.end(),
                                                    remainingDueToCalls[func]);
                
                // Merge the dataflow states above all the return statements in the function, storing the results in Fact 0 of
                // the function
                DFStateAtReturns* dfsar = dynamic_cast<DFStateAtReturns*>(fState->state.getFact(this, 0));
                ROSE_ASSERT(dfsar != NULL);
                bool modified = dfsar->mergeReturnStates(func, fState, dynamic_cast<IntraProceduralDataflow*>(intraAnalysis));  
                
                if(analysisDebugLevel>=1) {
                        Dbg::dbg << "function "<<func.get_name().getString()<<" "<<(modified? "modified": "not modified")<<endl;
                        Dbg::dbg << "remaining = ";
                        for(list<const CGFunction*>::iterator f=remaining.begin(); f!=remaining.end(); f++)
                                Dbg::dbg << (*f)->get_name().getString() << ", ";
                        Dbg::dbg << endl;
                        
                        /*Dbg::dbg << "State below:\n";
                        for(vector<Lattice*>::const_iterator it = fState->state.getLatticeBelow((Analysis*)this).begin();
                            it!=fState->state.getLatticeBelow((Analysis*)this).end(); it++)
                        {
                                Dbg::dbg << (*it)->str("    ") << endl; 
                        }*/
                        Dbg::dbg << "States at Return Statements:\n";
                        for(vector<Lattice*>::iterator it = dfsar->getLatsAtFuncReturn().begin();
                            it!=dfsar->getLatsAtFuncReturn().end(); it++)
                        {
                                Dbg::dbg << (*it)->str("    ") << endl; 
                        }
                        
                        vector<Lattice*> retState = fState->retState.getLatticeBelow((Analysis*)intraAnalysis);
                        Dbg::dbg << "retState: \n";
                        for(vector<Lattice*>::iterator it = retState.begin(); it!=retState.end(); it++)
                                Dbg::dbg << (*it)->str("    ") << endl; 
                        
                        Dbg::dbg << "States of Return Values: "<<&(dfsar->getLatsRetVal())<<endl;
                        for(vector<Lattice*>::iterator it = dfsar->getLatsRetVal().begin();
                            it!=dfsar->getLatsRetVal().end(); it++)
                        {
                                Dbg::dbg << (*it)->str("    ") << endl; 
                        }
                }
                
                // If this function's final dataflow state was modified, its callers must be 
                // placed back onto the remaining list, recording that they're on the list
                // because of their calls to this function
                if(modified)
                {
                        //Dbg::dbg << "Inserting Callers\n";
                        for(CGFunction::iterator it = funcCG->predecessors(); it!=funcCG->end(); it++)
                        {
                                const CGFunction* caller = it.getTarget(functions);
                                
                                //Dbg::dbg << "Caller of "<<funcCG->get_name().getString()<<": "
                                //         <<caller->get_name().getString()<<endl;
                                addToRemaining(caller);
                                remainingDueToCalls[caller].insert(func);
                        }
                }
        }
}
