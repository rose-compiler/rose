#define ANALYSIS_C
#include "sage3basic.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysis.h"
#include "latticeFull.h"
#include "compose.h"
#include "abstract_object.h"
#include "graphIterator.h"
#include <vector>
#include <set>
#include <map>
#include <boost/make_shared.hpp>

/* GB 2012-10-23: DESIGN NOTE
 * At the start of an intra-procedural analysis of a given function the function's initial dataflow state is copied 
 * from the special NodeState from inside the function's FunctionState object to the function's starting/ending Part.
 * To make our intra analyses simple we want to analyze each Part by propagating information from before it to after it
 * (fw: from incoming to outgoing edges; bw: from outgoing to incoming edges). This means that we need to ensure that
 * it is possible to create Lattices on the incoming edge of the starting Part and the outgoing edge of the ending Part
 * of a function. This is problematic because intra analyses are run upto the starting/ending Part but do not propagate
 * information to the other side (that would mean effectively falling off the edge of the function). This makes it 
 * impossible to create Lattices on these "other-side" edges. We overcome this problem in 
 * ContextInsensitiveInterProceduralDataflow (and should do the same in other analyses) by propagating the information
 * on the outgoing edges of the starting Parts / incoming edges of the ending Part one extra step to the incoming
 * edges of the starting Parts / outgoing edges of the ending Parts.
 */

using namespace std;
using namespace dbglog;
namespace fuse {
  
int analysisDebugLevel=1;

/****************
 *** Analysis ***
 ****************/

void Analysis::runAnalysis() { 
  /* GB: For some reason the compiler complains that SyntacticAnalysis doesn't implement this
         when it is implemented in its ancestor Dataflow. As such, 
         a dummy implementation is provided here. */
  assert(0); 
}

Analysis::~Analysis() {}

/*************************************
 *** UnstructuredPassAnalysis ***
 *************************************/

// runs the intra-procedural analysis on the given function, returns true if 
// the function's NodeState gets modified as a result and false otherwise
// state - the function's NodeState
void UnstructuredPassAnalysis::runAnalysis()
{
  if(analysisDebugLevel>=2)
    dbg << "UnstructuredPassAnalysis::runAnalysis()"<<endl;
  
  // Iterate over all the nodes in this function
  
  for(fw_graphEdgeIterator<PartEdgePtr, PartPtr> it(analysis->getComposer()->GetStartAStates(analysis)); 
          it!=fw_graphEdgeIterator<PartEdgePtr, PartPtr>::end(); 
          it++)
  {
    PartPtr p = it.getPart();
    NodeState* state = NodeState::getNodeState(analysis, p);
    visit(p, *state);
  }
}

/*************************
 *** InitDataflowState ***
 *************************/

void InitDataflowState::visit(PartPtr p, NodeState& state)
{
  /*ostringstream label; label << "InitDataflowState::visit() p="<<p->str()<<", analysis="<<analysis<<"="<<analysis->str()<<" state="<<&state<<endl;
  scope reg(label.str(), scope::medium, analysisDebugLevel, 1);*/
  
  // generate a new initial state for this node
  analysis->initializeState(p, state);
}

/************************
 *** ComposedAnalysis ***
 ************************/

// Propagates the dataflow info from the current node's NodeState (curNodeState) to the next node's 
//     NodeState (nextNodeState).
// Returns true if the next node's meet state is modified and false otherwise.
bool ComposedAnalysis::propagateStateToNextNode(
                map<PartEdgePtr, vector<Lattice*> >& curNodeState, PartPtr curNode, 
                map<PartEdgePtr, vector<Lattice*> >& nextNodeState, PartPtr nextNode)
{
  scope reg("propagateStateToNextNode", scope::medium, analysisDebugLevel, 1);
  bool modified = false;
  
  // curNodeState should have a single mapping to the NULLPartEdge
  assert(curNodeState.begin()->first == NULLPartEdge);
  
  vector<Lattice*>::const_iterator itC, itN;
  if(analysisDebugLevel>=1) {
    dbg << endl << "Propagating to Next Node: "<<nextNode->str()<<endl;
    dbg << "Cur Node Lattice "<<endl;
    { indent ind(analysisDebugLevel, 1); dbg<<NodeState::str(curNodeState); }
    
    dbg << "Next Node Lattice "<<endl;
    { indent ind(analysisDebugLevel, 1); dbg<<NodeState::str(nextNodeState); }
  }

  // Update forward info above nextNode from the forward info below curNode.
  
  // Compute the meet of the dataflow information along the curNode->nextNode edge with the 
  // next node's current state one Lattice at a time and save the result above the next node.
  
  // If nextNodeState is non-empty, we union curNodeState into it
  if(analysisDebugLevel>=1) dbg << "---------------------- #nextNodeState="<<nextNodeState.size()<<endl;
  if(nextNodeState.size()>0)
    modified = NodeState::unionLatticeMaps(nextNodeState, curNodeState) || modified;
  // Otherwise, we copy curNodeState[NULLPartEdge] over it
  else {
    NodeState::copyLatticesOW(nextNodeState, NULLPartEdge, curNodeState, NULLPartEdge);
    modified = true;
  }

  //dbg << "Result:"<<endl;  
  //{ indent ind(analysisDebugLevel, 1); dbg<<NodeState::str(nextNodeState); }

  if(analysisDebugLevel>=1) {
    indent ind(analysisDebugLevel, 1);
    if(modified) {
      dbg << "Next node's in-data modified. Adding..."<<endl;
      dbg << "Propagated: Lattice "<<endl;
      { indent ind(analysisDebugLevel, 1); dbg<<NodeState::str(nextNodeState); }
    }
    else
      dbg << "  No modification on this node"<<endl;
  }

  return modified;
}

} // namespace fuse;
