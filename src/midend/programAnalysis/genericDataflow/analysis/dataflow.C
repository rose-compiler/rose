#include <featureTests.h>
#ifdef ROSE_ENABLE_SOURCE_ANALYSIS

#include "analysis.h"
#include "dataflow.h"
#include <assert.h>

#include <memory>
using std::unique_ptr;

#include <utility>
using std::pair;
using std::make_pair;

#include <boost/mem_fn.hpp>
using boost::mem_fn;

NodeState* IntraBWDataflow::initializeFunctionNodeState(const Function &func, NodeState *fState)
{
// DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.

  ROSE_ASSERT(func.get_definition() != NULL);
  DataflowNode funcCFGEnd   = cfgUtils::getFuncEndCFG(func.get_definition(),filter);
  
  // Initialize the function's entry NodeState 
  NodeState* entryState = *(NodeState::getNodeStates(funcCFGEnd).rbegin());
  NodeState::copyLattices_bEQb(this, *entryState, /*interAnalysis*//*this, */*fState);
  
  return entryState;
}

NodeState* IntraFWDataflow::initializeFunctionNodeState(const Function &func, NodeState *fState)
{
  ROSE_ASSERT(func.get_definition() != NULL);
  DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(func.get_definition(), filter);

  // Initialize the function's entry NodeState 
  NodeState* entryState = *(NodeState::getNodeStates(funcCFGStart).begin());
  NodeState::copyLattices_aEQa(this, *entryState, /*interAnalysis*/this, *fState);

  return entryState;
}

VirtualCFG::dataflow*
IntraFWDataflow::getInitialWorklist(const Function &func, bool firstVisit, bool analyzeDueToCallers, const set<Function> &calleesUpdated, NodeState *fState)
{
  assert(func.get_definition() != NULL);
  DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(func.get_definition(),filter);
  DataflowNode funcCFGEnd   = cfgUtils::getFuncEndCFG(func.get_definition(),filter);

  // Initialize the set of nodes that this dataflow will iterate over
  VirtualCFG::dataflow *it = new VirtualCFG::dataflow(funcCFGEnd);

  // If we're analyzing this function for the first time or because the dataflow information coming in from its
  // callers has changed, add the function's entry point
  if(firstVisit || analyzeDueToCallers) {
    it->add(funcCFGStart);
  }

  // If we're analyzing this function because of a change in the exit dataflow states of some of the 
  // functions called by this function (these functions are recorded in calleesUpdated), add the calls
  // by this function to those functions to the dataflow iterator
  FindAllFunctionCalls fafc(calleesUpdated);
  fafc.runAnalysis(func, fState);
  for(map<Function, set<DataflowNode> >::iterator callee=fafc.getFuncCalls().begin(); callee!=fafc.getFuncCalls().end(); callee++)
    for(set<DataflowNode>::iterator callNode=callee->second.begin(); callNode!=callee->second.end(); callNode++)
      {
        it->add(*callNode);
      }

  return it;
}

VirtualCFG::dataflow*
IntraBWDataflow::getInitialWorklist(const Function &func, bool /*firstVisit*/, bool /*analyzeDueToCallers*/,
                                    const set<Function> &/*calleesUpdated*/, NodeState* /*fState*/)
{
  ROSE_ASSERT(func.get_definition() != NULL);
  DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(func.get_definition(),filter);
  DataflowNode funcCFGEnd   = cfgUtils::getFuncEndCFG(func.get_definition(),filter);

  return new VirtualCFG::back_dataflow(funcCFGEnd, funcCFGStart);
}

vector<Lattice*> IntraFWDataflow::getLatticeAnte(NodeState *state) { return state->getLatticeAbove(this); }
vector<Lattice*> IntraFWDataflow::getLatticePost(NodeState *state) { return state->getLatticeBelow(this); }
vector<Lattice*> IntraBWDataflow::getLatticeAnte(NodeState *state) { return state->getLatticeBelow(this); }
vector<Lattice*> IntraBWDataflow::getLatticePost(NodeState *state) { return state->getLatticeAbove(this); }

void IntraFWDataflow::transferFunctionCall(const Function &func, const DataflowNode &n, NodeState *state)
{
  vector<Lattice*> dfInfoBelow = state->getLatticeBelow(this);

  vector<Lattice*>* retState = NULL;
  dynamic_cast<InterProceduralDataflow*>(interAnalysis)->
    transfer(func, n, *state, dfInfoBelow, &retState, true);

  if(retState && !(retState->size()==0 || (retState->size() == dfInfoBelow.size()))) {
    Dbg::dbg << "#retState="<<retState->size()<<endl;
    for(vector<Lattice*>::iterator ml=retState->begin(); ml!=retState->end(); ml++)
      Dbg::dbg << "        "<<(*ml)->str("            ")<<endl;
    Dbg::dbg << "#dfInfoBelow="<<dfInfoBelow.size()<<endl;
    for(vector<Lattice*>::const_iterator l=dfInfoBelow.begin(); l!=dfInfoBelow.end(); l++)
      Dbg::dbg << "        "<<(*l)->str("            ")<<endl;
  }

  // Incorporate information about the function's return value into the caller's dataflow state
  // as the information of the SgFunctionCallExp
  ROSE_ASSERT(retState==NULL || retState->size()==0 || (retState->size() == dfInfoBelow.size()));
  if(retState) {
    vector<Lattice*>::iterator lRet;
    vector<Lattice*>::const_iterator lDF;
    for(lRet=retState->begin(), lDF=dfInfoBelow.begin(); 
        lRet!=retState->end(); lRet++, lDF++) {
      Dbg::dbg << "    lDF Before="<<(*lDF)->str("        ")<<endl;
      Dbg::dbg << "    lRet Before="<<(*lRet)->str("        ")<<endl;
      (*lDF)->unProject(isSgFunctionCallExp(n.getNode()), *lRet);
      Dbg::dbg << "    lDF After="<<(*lDF)->str("        ")<<endl;
    }
  }
}

void IntraBWDataflow::transferFunctionCall(const Function &func, const DataflowNode &n, NodeState *state)
{
  vector<Lattice*>* retState;
  dynamic_cast<InterProceduralDataflow*>(interAnalysis)->
    transfer(func, n, *state, state->getLatticeAbove(this), &retState, false);
                                
  // NEED TO INCORPORATE INFORMATION ABOUT RETURN INTO DATAFLOW SOMEHOW
}

vector<DataflowNode> IntraUniDirectionalDataflow::gatherDescendants(vector<DataflowEdge> edges,
                                                                    DataflowNode (DataflowEdge::*edgeFn)() const)
{
  vector<DataflowNode> descendants;

  for(vector<DataflowEdge>::const_iterator ei = edges.begin(); ei!=edges.end(); ei++)
  {
    descendants.push_back(mem_fn(edgeFn)(*ei));
  }

  return descendants;
}
vector<DataflowNode> IntraFWDataflow::getDescendants(const DataflowNode &n)
{ return gatherDescendants(n.outEdges(), &DataflowEdge::target); }
vector<DataflowNode> IntraBWDataflow::getDescendants(const DataflowNode &n)
{ return gatherDescendants(n.inEdges(),  &DataflowEdge::source); }

DataflowNode IntraFWDataflow::getUltimate(const Function &func)
{ assert(func.get_definition() != NULL); return cfgUtils::getFuncEndCFG(func.get_definition(), filter); }
DataflowNode IntraBWDataflow::getUltimate(const Function &func)
{ assert(func.get_definition() != NULL); return cfgUtils::getFuncStartCFG(func.get_definition(), filter); }

// Runs the intra-procedural analysis on the given function. Returns true if 
// the function's NodeState gets modified as a result and false otherwise.
// state - the function's NodeState
bool IntraUniDirectionalDataflow::runAnalysis(const Function& func, NodeState* fState, bool analyzeDueToCallers, set<Function> calleesUpdated)
{
        // Make sure that we've been paired with a valid inter-procedural dataflow analysis
        ROSE_ASSERT(dynamic_cast<InterProceduralDataflow*>(interAnalysis));

        ostringstream funcNameStr; funcNameStr << "Function "<<func.get_name().getString()<<"()";
        if(analysisDebugLevel>=1) {
                Dbg::enterFunc(funcNameStr.str());
                Dbg::dbg << "analyzeDueToCallers="<<analyzeDueToCallers<<" calleesUpdated=";
                for(set<Function>::iterator f=calleesUpdated.begin(); f!=calleesUpdated.end(); f++)
                        Dbg::dbg << f->get_name().getString()<<", ";
                Dbg::dbg << endl;
        }
        
        // Set of functions that have already been visited by this analysis, used
        // to make sure that the dataflow state of previously-visited functions is
        // not re-initialized when they are visited again.
        
        bool firstVisit = visited.find(func) == visited.end();
        // Initialize the lattices used by this analysis, if this is the first time the analysis visits this function
        if(firstVisit)
        {
                InitDataflowState ids(this/*, initState*/);
                ids.runAnalysis(func, fState);
                visited.insert(func);
        }

     // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=deprecated-declarations.
     // DQ (12/10/2016): This is a technique to suppress warnings in generated code that we want to be an error elsewhere in ROSE. 
     // See https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html for more detail.
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
        unique_ptr<VirtualCFG::dataflow> workList(getInitialWorklist(func, firstVisit, analyzeDueToCallers, calleesUpdated, fState));

        VirtualCFG::dataflow &it = *workList;
        VirtualCFG::iterator itEnd = VirtualCFG::dataflow::end();
        
        // Iterate over the nodes in this function that are downstream from the nodes added above
        for(; it != itEnd; it++)
        {
                DataflowNode n = *it;
                SgNode* sgn = n.getNode();
                ostringstream nodeNameStr;
                nodeNameStr << "Current Node "<<sgn<<"["<<sgn->class_name()<<" | "<<Dbg::escape(sgn->unparseToString())<<" | "<<n.getIndex()<<"]";
                if(analysisDebugLevel>=1){
                        Dbg::enterFunc(nodeNameStr.str());
                }
                bool modified = false;
                
                // the number of NodeStates associated with the given dataflow node
                int numStates=NodeState::numNodeStates(n);
                ROSE_ASSERT(numStates == 1);
                // the NodeStates themselves
                const vector<NodeState*> nodeStates = NodeState::getNodeStates(n);
                NodeState* state = NULL;
                
                // Iterate over all of this node's NodeStates
                for(vector<NodeState*>::const_iterator itS = nodeStates.begin(); itS!=nodeStates.end(); )
                {
                        state = *itS;
                                
                        // =================== Copy incoming lattices to outgoing lattices ===================
                        const vector<Lattice*> dfInfoAnte = getLatticeAnte(state);
                        const vector<Lattice*> dfInfoPost = getLatticePost(state);
                                                
                        // Overwrite the Lattices below this node with the lattices above this node.
                        // The transfer function will then operate on these Lattices to produce the
                        // correct state below this node.
                        
                        vector<Lattice*>::const_iterator itA, itP;
                        int j=0;
                        for(itA  = dfInfoAnte.begin(), itP  = dfInfoPost.begin();
                            itA != dfInfoAnte.end() && itP != dfInfoPost.end(); 
                            itA++, itP++, j++)
                        {
                                if(analysisDebugLevel>=1){
                                        Dbg::dbg << " ==================================  "<<endl;
                                        Dbg::dbg << " Copy incoming lattice to outgoing lattice: "<<endl;
                                        Dbg::dbg << "  Incoming/Above Lattice "<<j<<": \n        "<<(*itA)->str("            ")<<endl;
                                        Dbg::dbg << "  Outgoing/Below Lattice before copying "<<j<<": \n        "<<(*itP)->str("            ")<<endl;
                                }
                                (*itP)->copy(*itA);
                                
                                if(analysisDebugLevel>=1){
                                        Dbg::dbg << "  Outgoing/Below Lattice after copying "<<j<<": \n        "<<(*itP)->str("            ")<<endl;
                                }
                        }
                        
                        // =================== TRANSFER FUNCTION ===================
                       
                        if(analysisDebugLevel>=1){
                          Dbg::dbg << " ==================================  "<<endl;
                          Dbg::dbg << "  Transferring the outgoing  Lattice ... "<<endl;
                        }
                        
                        //if this is a call site, call transfer function of the associated interprocedural analysis
                        if (isSgFunctionCallExp(sgn))
                          transferFunctionCall(func, n, state);

                        boost::shared_ptr<IntraDFTransferVisitor> transferVisitor = getTransferVisitor(func, n, *state, dfInfoPost);
                        sgn->accept(*transferVisitor);
                        modified = transferVisitor->finish() || modified;

                        // =================== TRANSFER FUNCTION ===================
                        if(analysisDebugLevel>=1)
                        {
                                j=0;
                                for(itP = dfInfoPost.begin();
                                    itP != dfInfoPost.end(); itP++, j++)
                                {
                                        Dbg::dbg << "    Transferred: outgoing Lattice "<<j<<": \n        "<<(*itP)->str("            ")<<endl;
                                }
                                Dbg::dbg << "    transferred, modified="<<modified<<endl;
                        }


                        // XXX: Greg believes this plurality of
                        // NodeState objects per DataflowNode is due
                        // to FunctionCallExp, and may not even be
                        // used there anymore, either

                        // Look at the next NodeState
                        itS++;
                }
                ROSE_ASSERT(state);
                
                // =================== Populate the generated outgoing lattice to descendants (meetUpdate) ===================
/*                      // if there has been a change in the dataflow state immediately below this node AND*/
                // If this is not the last node in the function
                if(/*modified && */*it != getUltimate(func))
                {
                        if(analysisDebugLevel>=1){
                          Dbg::dbg << " ==================================  "<<endl;
                          Dbg::dbg << " Propagating/Merging the outgoing  Lattice to all descendant nodes ... "<<endl;
                        }
                        // iterate over all descendants
                        vector<DataflowNode> descendants = getDescendants(n);
                        if(analysisDebugLevel>=1) {
                                Dbg::dbg << "    Descendants ("<<descendants.size()<<"):"<<endl;
                                Dbg::dbg << "    ~~~~~~~~~~~~"<<endl;
                        }
                        
                        for(vector<DataflowNode>::iterator di = descendants.begin(); di != descendants.end(); di++)
                        {
                                // The CFG node corresponding to the current descendant of n
                                DataflowNode nextNode = *di;
                                SgNode *nextSgNode = nextNode.getNode();
                                ROSE_ASSERT(nextSgNode != NULL);
                                if(analysisDebugLevel>=1)
                                        Dbg::dbg << "    Descendant: "<<nextSgNode<<"["<<nextSgNode->class_name()<<" | "<<Dbg::escape(nextSgNode->unparseToString())<<"]"<<endl;
                        
                                NodeState* nextState = NodeState::getNodeState(nextNode, 0);
                                ROSE_ASSERT(nextSgNode && nextState);
                                
                                // Propagate the Lattices below this node to its descendant
                                modified = propagateStateToNextNode(getLatticePost(state), n, numStates-1, getLatticeAnte(nextState), nextNode);
                                // If the next node's state gets modified as a result of the propagation, 
                                // add the node to the processing queue.
                                if(modified)
                                        it.add(nextNode);
                        }
                }
                
                if(analysisDebugLevel>=1) Dbg::exitFunc(nodeNameStr.str());
        }

        // Test if the Lattices at the end of the function after the analysis are equal to their
        // original values in the function state.
        bool modified = !NodeState::eqLattices(getLatticeAnte(*(NodeState::getNodeStates(getUltimate(func)).begin())),
                                               getLatticePost(fState));

        if(analysisDebugLevel>=1) Dbg::exitFunc(funcNameStr.str());
        
        return modified;
}

#endif
