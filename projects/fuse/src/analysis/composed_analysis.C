#include "sage3basic.h"
#include "analysis.h"
#include "composed_analysis.h"
#include "compose.h"
#include "printAnalysisStates.h"
#include "ats_graph.h"

#include <memory>
using std::auto_ptr;

#include <utility>
using std::pair;
using std::make_pair;

#include <boost/mem_fn.hpp>
using boost::mem_fn;
#include <boost/make_shared.hpp>

using namespace std;
using namespace dbglog;

namespace fuse
{
int composedAnalysisDebugLevel=1;
  
/****************************
 ***** ComposedAnalysis *****
 ****************************/

/*// Returns whether the given AbstractObject is live at the given PartEdge
// This version is a wrapper for calling type-specific versions of isLive without forcing the caller to
// care about the type of object
bool ComposedAnalysis::isLive(AbstractObjectPtr ao, PartEdgePtr pedge)
{
  ValueObjectPtr val = boost::dynamic_pointer_cast<ValueObject>(ao);
  if(val) return isLiveVal(val, pedge);
  
  MemLocObjectPtr ml = boost::dynamic_pointer_cast<MemLocObject>(ao);
  if(ml) return isLiveMemLoc(ml, pedge);
  
  CodeLocObjectPtr cl = boost::dynamic_pointer_cast<CodeLocObject>(ao);
  if(cl) return isLiveCodeLoc(cl, pedge);
  
  assert(0);
}

// Returns whether the given pair of AbstractObjects are may-equal at the given PartEdge
// This version is a wrapper for calling type-specific versions of mayEqual without forcing the caller to
// care about the type of object
bool ComposedAnalysis::mayEqual(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge)
{
  ValueObjectPtr val1 = boost::dynamic_pointer_cast<ValueObject>(ao1);
  if(val1) {
    ValueObjectPtr val2 = boost::dynamic_pointer_cast<ValueObject>(ao2);
    assert(val2);
    return mayEqualV(val1, val2, pedge);
  }
  
  MemLocObjectPtr ml1 = boost::dynamic_pointer_cast<MemLocObject>(ao1);
  if(ml1) {
    MemLocObjectPtr ml2 = boost::dynamic_pointer_cast<MemLocObject>(ao2);
    assert(ml2);
    return mayEqualML(ml1, ml2, pedge);
  }
  
  CodeLocObjectPtr cl1 = boost::dynamic_pointer_cast<CodeLocObject>(ao1);
  if(cl1) {
    CodeLocObjectPtr cl2 = boost::dynamic_pointer_cast<CodeLocObject>(ao2);
    assert(cl2);
    return mayEqualML(cl1, cl2, pedge);
  }
}

// Returns whether the given pair of AbstractObjects are must-equal at the given PartEdge
// This version is a wrapper for calling type-specific versions of mustEqual without forcing the caller to
// care about the type of object
bool ComposedAnalysis::mustEqual(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge)
{
  ValueObjectPtr val1 = boost::dynamic_pointer_cast<ValueObject>(ao1);
  if(val1) {
    ValueObjectPtr val2 = boost::dynamic_pointer_cast<ValueObject>(ao2);
    assert(val2);
    return mustEqualV(val1, val2, pedge);
  }
  
  MemLocObjectPtr ml1 = boost::dynamic_pointer_cast<MemLocObject>(ao1);
  if(ml1) {
    MemLocObjectPtr ml2 = boost::dynamic_pointer_cast<MemLocObject>(ao2);
    assert(ml2);
    return mustEqualML(ml1, ml2, pedge);
  }
  
  CodeLocObjectPtr cl1 = boost::dynamic_pointer_cast<CodeLocObject>(ao1);
  if(cl1) {
    CodeLocObjectPtr cl2 = boost::dynamic_pointer_cast<CodeLocObject>(ao2);
    assert(cl2);
    return mustEqualML(cl1, cl2, pedge);
  }
}*/

// Return the anchor Parts of a given function
std::set<PartPtr> ComposedAnalysis::GetStartAStates()
{
  static bool startStatesInitialized = false;
  // If the result of this function has not yet been computed
  if(!startStatesInitialized) {
    // Get the result and cache it
    StartAStates = GetStartAStates_Spec();
    startStatesInitialized = true;
  }
  return StartAStates;
}

std::set<PartPtr> ComposedAnalysis::GetEndAStates()
{
  static bool endStatesInitialized = false;
  // If the result of this function has not yet been computed
  if(!endStatesInitialized) { 
    // Get the result and cache it
    EndAStates = GetEndAStates_Spec();
    endStatesInitialized = true;
  }
  return EndAStates;
}

// Given a PartEdgePtr pedge implemented by this ComposedAnalysis, returns the part from its predecessor
// from which pedge was derived. This function caches the results if possible.
PartEdgePtr ComposedAnalysis::convertPEdge(PartEdgePtr pedge)
{
  // If the result of this function has been computed, return it
  map<PartEdgePtr, PartEdgePtr>::iterator oldPEdge;
  if((oldPEdge=new2oldPEdge.find(pedge)) != new2oldPEdge.end()) return oldPEdge->second;
  
  // Cache the result
  //PartEdgePtr result = convertPEdge_Spec(pedge);
  PartEdgePtr result = pedge->getParent();
  new2oldPEdge[pedge] = result;
  return result;
}

// Generates the initial lattice state for the given dataflow node, in the given function. Implementations 
// fill in the lattices above and below this part, as well as the facts, as needed. Since in many cases
// the lattices above and below each node are the same, implementors can alternately implement the 
// genInitLattice and genInitFact functions, which are called by the default implementation of initializeState.
void ComposedAnalysis::initializeState(PartPtr part, NodeState& state)
{
  if(getDirection()==none) return;
  
  // Analyses associate all arriving information with a single NULL edge and all departing information
  // with the edge on which the information departs
  if(getDirection()==fw) {
    std::vector<Lattice*> lats;
    genInitLattice(part, part->inEdgeFromAny(), lats);
    state.setLatticeAbove(this, lats);
  } else if(getDirection()==bw) {
    std::vector<Lattice*> lats;
    genInitLattice(part, part->outEdgeToAny(), lats);
    state.setLatticeBelow(this, lats);
  }
  
  // Don't initialize the departing informaiton. This will be set by ComposedAnalysis::runAnalysis() when
  // it first touches the part
  /*vector<PartEdgePtr> edges = part->outEdges();
  for(vector<PartEdgePtr>::iterator e=edges.begin(); e!=edges.end(); e++) {
    std::vector<Lattice*> lats;
    genInitLattice(func, part, *e, lats);

    if(getDirection()==fw)      state.setLatticeBelow(this, *e, lats);
    else if(getDirection()==bw) state.setLatticeAbove(this, *e, lats);
  }*/
  
  vector<NodeFact*> initFacts;
  genInitFact(part, initFacts);
  state.setFacts(this, initFacts);
}

// Runs the intra-procedural analysis on the given function. Returns true if 
// the function's NodeState gets modified as a result and false otherwise.
// state - the function's NodeState
// analyzeFromDirectionStart - If true the function should be analyzed from its starting point from the analysis' 
//    perspective (fw: entry point, bw: exit point)
void ComposedAnalysis::runAnalysis(/*NodeState* appState*/)
{
  // Make sure that we've been paired with a valid inter-procedural dataflow analysis
  //assert(dynamic_cast<InterProceduralDataflow*>(interAnalysis));

  //ostringstream funcNameStr; if(composedAnalysisDebugLevel>=1) funcNameStr << "Analysis Function "<<func.get_name().getString()<<"()";
  scope reg("ComposedAnalysis", scope::medium, composedAnalysisDebugLevel);
  
  // Quit out if this is an undirected analysis (i.e. doesn't need the fixed-point algorithm)
  if(getDirection() == none) return;
  
  /*if(composedAnalysisDebugLevel>=1) {
      dbg << "analyzeFromDirectionStart="<<analyzeFromDirectionStart<<" calleesUpdated=";
      for(set<Function>::iterator f=calleesUpdated.begin(); f!=calleesUpdated.end(); f++)
          dbg << f->get_name().getString()<<", ";
      dbg << endl;
  }*/

  // Set of all the Parts that have already been visited by the analysis
  set<PartPtr> visited;
  
  // Set of all the Parts that have been initialized
  set<PartPtr> initialized;
  
  // Re-analyze it from scratch
  set<PartPtr> startingParts = getInitialWorklist();
  set<PartPtr> ultimateParts = getUltimate();
  if(composedAnalysisDebugLevel>=2) {
    //dbg << "#startingParts="<<startingParts.size()<<" #ultimateParts="<<ultimateParts.size()<<endl;
    for(set<PartPtr>::iterator i=startingParts.begin(); i!=startingParts.end(); i++) dbg << "starting="<<i->get()->str()<<endl;
    for(set<PartPtr>::iterator i=ultimateParts.begin(); i!=ultimateParts.end(); i++) dbg << "ultimate="<<i->get()->str()<<endl;
    //for(set<PartPtr>::iterator start=startingParts.begin(); start!=startingParts.end(); start++) {
      //scope reg(txt()<<"Starting from "<<(*start)->str(), scope::medium, composedAnalysisDebugLevel, 1);
  }
  
  // Initialize the starting states
  for(set<PartPtr>::iterator s=startingParts.begin(); s!=startingParts.end(); s++) {
    NodeState* state = NodeState::getNodeState(this, *s);
    initializeState(*s, *state);
    initialized.insert(*s);
  }
  
  // Iterate over the abstract states that are downstream from the starting states
  dataflowPartEdgeIterator* curNodeIt = getIterator();
  for(set<PartPtr>::iterator s=startingParts.begin(); s!=startingParts.end(); s++) curNodeIt->addStart(*s);
  
  // Maps each Abstract State to the anchors associated with each visit to this part by the worklist algorithm
  boost::shared_ptr<std::map<PartPtr, std::list<anchor> > > partAnchors = boost::make_shared<std::map<PartPtr, std::list<anchor> > >();
  
  // Maps each Abstract State to the anchors of outgoing links that target it from the last visit to its predecessors
  map<PartPtr, set<anchor> > toAnchors;
  // Maps each Abstract State to the anchors of the AStates that lead to it, as well as the AStates themselves
  map<PartPtr, set<pair<anchor, PartPtr> > > fromAnchors;
  // Maps each Abstract State to the anchor of the last instance of a transfer function executed at this AState
  map<PartPtr, anchor> lastTransferAnchors;
  // Maps each Abstract State to the anchor of the next instance of a transfer function executed at this AState
  map<PartPtr, anchor> nextTransferAnchors;
  
  // graph widget that visualizes the flow of the worklist algorithm
  atsGraph worklistGraph((getDirection() == fw? startingParts: ultimateParts), partAnchors, getDirection() == fw, composedAnalysisDebugLevel, 1);
  
  /*{ scope itreg("Initial curNodeIt", scope::medium, composedAnalysisDebugLevel, 1);
    dbg << curNodeIt->str()<<endl; }*/
  
  while(curNodeIt && *curNodeIt!=dataflowPartEdgeIterator::end())
  {
    PartPtr part = curNodeIt->getPart();
    
    //set<anchor> toAnchorsSet; for(set<pair<anchor, PartPtr> >::iterator a=toAnchors[part].begin(); a!=toAnchors[part].end(); a++) toAnchorsSet.insert(a->first);
    scope reg(txt()<<"Cur AState "<<part->str(), toAnchors[part], scope::medium, composedAnalysisDebugLevel, 1);
    if(composedAnalysisDebugLevel>=1) { 
      // If we have previously invoked this transfer function on this Abstract State, attach the link from it to this scope
      if(nextTransferAnchors.find(part) != nextTransferAnchors.end())
         reg.attachAnchor(nextTransferAnchors[part]);
      if(composedAnalysisDebugLevel>=1 && fromAnchors.size()>0) { 
        scope backedges("Incoming Edges", scope::low, composedAnalysisDebugLevel, 1); 
        for(set<pair<anchor, PartPtr> >::iterator a=fromAnchors[part].begin(); a!=fromAnchors[part].end(); a++) 
          dbg << a->first.linkImg(a->second.get()->str())<<endl;
      }
      
      { scope nextprev("", scope::min, composedAnalysisDebugLevel, 1); 
      // If we've previously visited this Abstract State, set up a link to it
      if(lastTransferAnchors.find(part) != lastTransferAnchors.end())
        dbg << lastTransferAnchors[part].linkImg("Last visit");
      lastTransferAnchors[part] = reg.getAnchor();
      
      // Set up a link to the next visit, if any
      anchor nextVisitA;
      nextTransferAnchors[part] = nextVisitA;
      dbg << nextVisitA.linkImg("Next visit"); }      
      
      // We've found the destination of all the links that were pointing at this scope, so we now erase them
      toAnchors.erase(part);
      fromAnchors.erase(part);
      (*partAnchors)[part].push_back(reg.getAnchor());
    }
            
    // The NodeState associated with this part
    NodeState* state = NodeState::getNodeState(this, part);

    // Record that we've visited this Part
    bool modified = false;
    bool firstVisit;
    if((firstVisit = (visited.find(part) == visited.end()))) {
      visited.insert(part);
    }

    if(composedAnalysisDebugLevel>=1) { dbg << "state="<<endl<<state->str()<<endl; }

    map<PartEdgePtr, vector<Lattice*> >& dfInfoAnte = getLatticeAnte(state);
    // Create a local map for the post dataflow information. It will be deallocated 
    // at the end of the transfer function.
    map<PartEdgePtr, vector<Lattice*> > dfInfoPost;

    // Iterate over all the CFGNodes associated with this part and merge the result of applying to transfer function
    // to all of them
    set<CFGNode> v=part->CFGNodes();
    for(set<CFGNode>::iterator c=v.begin(); c!=v.end(); c++) {
      SgNode* sgn = c->getNode();

      ostringstream nodeNameStr; if(composedAnalysisDebugLevel>=(v.size()==1 ? 10: 1)) nodeNameStr << "Current CFGNode "<<part->str()<<endl;
      scope reg(nodeNameStr.str(), scope::medium, composedAnalysisDebugLevel, (v.size()==1 ? 10: 1));

      // =================== Copy incoming lattices to outgoing lattices ===================
      // For the case where dfInfoPost needs to be created fresh, this shared pointer dfInfoPostPtr will ensure that 
      // the map is deallocated when dfInfoPostPtr goes out of scope.
      boost::shared_ptr<map<PartEdgePtr, vector<Lattice*> > > dfInfoPostPtr;
      // Overwrite the Lattices below this node with the lattices above this node.
      // The transfer function will then operate on these Lattices to produce the
      // correct state below this node.

      //printf("                 dfInfoAnte.size()=%d, dfInfoPost.size()=%d, this=%p\n", dfInfoAnte.size(), dfInfoPost.size(), this);
      if(c==v.begin()) {
        if(composedAnalysisDebugLevel>=1) {
           dbg << "==================================  "<<endl;
           dbg << "  Copying incoming Lattice :"<<endl;
           {indent ind(composedAnalysisDebugLevel, 1); dbg <<NodeState::str(dfInfoAnte); }
           dbg << "  To outgoing Lattice: "<<endl;
           {indent ind(composedAnalysisDebugLevel, 1); dbg <<NodeState::str(dfInfoPost); }
        }

        // Over-write the post information with the ante information, creating it if it doesn't exist yet
        /*if(dfInfoPost.size()==0) */NodeState::copyLatticesOW(dfInfoPost, dfInfoAnte);
        // GB 2012-09-28: Do we even need to keep post information around after the transfer 
        //                function is done or can we just deallocate it?
        //else                     NodeState::copyLattices  (dfInfoPost, dfInfoAnte);
      // If this is not the first CFGNode, create a new post state. It will be merged into the lattices in 
      // the NodeState after the transfer function
      } else {
        // Since this is not the first CFGNode within Part p, create a new post state for it
        dfInfoPostPtr = boost::make_shared<map<PartEdgePtr, vector<Lattice*> > >();
        dfInfoPost = *dfInfoPostPtr.get();

        if(composedAnalysisDebugLevel>=1) {
          dbg << "=================================="<<endl;
          dbg << "Creating outgoing state from incoming state"<<endl;
        }

        NodeState::copyLatticesOW(dfInfoPost, dfInfoAnte);
      }
      
      // <<<<<<<<<<<<<<<<<<< TRANSFER FUNCTION <<<<<<<<<<<<<<<<<<<
      modified = transferDFState(part, *c, sgn, *state, dfInfoPost, ultimateParts) || modified;
      // >>>>>>>>>>>>>>>>>>> TRANSFER FUNCTION >>>>>>>>>>>>>>>>>>>

      if(composedAnalysisDebugLevel>=1) {
        {scope s("Transferred: outgoing Lattice=", scope::low, composedAnalysisDebugLevel, 1); dbg <<NodeState::str(dfInfoPost)<<endl; }
        {scope s("state=", scope::low, composedAnalysisDebugLevel, 1); dbg <<state->str()<<endl; }
        dbg << "Transferred: "<<(modified? "<font color=\"#990000\">Modified</font>": "<font color=\"#000000\">Not Modified</font>")<<endl;
      }

      // If this is not the first CFGNode within this Part, merge its outgoing lattices with the outgoing
      // lattices produced by the transfer function's execution on the prior CFGNodes in this Part
      if(c!=v.begin()) {
        if(composedAnalysisDebugLevel>=1) {
          dbg << "==================================  "<<endl;
          dbg << "Merging lattice for prior CFGNodes:"<<endl;
          {indent ind(composedAnalysisDebugLevel, 1); dbg <<NodeState::str(getLatticePost(state)); }
          dbg << "With lattice  for the current CFGNodes:"<<endl;
          {indent ind(composedAnalysisDebugLevel, 1); dbg <<NodeState::str(dfInfoPost); }
        }
        assert(getLatticePost(state).begin()->first == NULLPartEdge);
        modified = NodeState::unionLatticeMaps(getLatticePost(state), dfInfoPost) || modified;

        if(composedAnalysisDebugLevel>=1) {
          dbg << "Merged within Part: Lattice"<<endl;
          {indent ind(composedAnalysisDebugLevel, 1); dbg <<NodeState::str(getLatticePost(state)); }
        }
      }
    } // for(vector<CFGNode>::iterator c=v.begin(); c!=v.end(); c++) {

    // =================== Populate the generated outgoing lattice to descendants (meetUpdate) ===================
    // If this is not an ultimate node in the application (starting or ending)
    //if(ultimateParts.find(curNodeIt->getPart()) == ultimateParts.end())
      // Propagate the transferred dataflow information to all of this part's descendants
      propagateDF2Desc(part, modified, visited, initialized, curNodeIt, dfInfoPost, 
                       reg.getAnchor(), worklistGraph, toAnchors, fromAnchors);

    /*// Deallocate dfInfoPost and its Lattices
    for(map<PartEdgePtr, vector<Lattice*> >::iterator e=dfInfoPost.begin(); e!=dfInfoPost.end(); e++)
      for(vector<Lattice*>::iterator l=e->second.begin(); l!=e->second.end(); l++)
        delete *l;*/
    // Save dfInfoPost in the NodeState
    setLatticePost(state, dfInfoPost, firstVisit);
    
    (*curNodeIt)++;
    
    /*{ scope itreg("curNodeIt", scope::medium, composedAnalysisDebugLevel, 1);
    dbg << curNodeIt->str()<<endl; }*/
  } // while(curNodeIt!=dataflowPartEdgeIterator::end())
  //} // for(list<PartPtr>::iterator start=startingParts.begin(); start!=startingParts.end(); start++) {
  
  // Add the ultimate parts to partAnchors to make sure that they have an entry
  // since they will not be visited by the worklist algorithm
  /*
  for(set<PartPtr>::iterator u=ultimateParts.begin(); u!=ultimateParts.end(); u++) {
    scope reg(txt()<<"Ultimate AState "<<(*u).get()->str(), toAnchors[*u], scope::medium, composedAnalysisDebugLevel, 1);
    (*partAnchors)[*u].push_back(reg.getAnchor());
  }*/
  
  if(composedAnalysisDebugLevel>=1) dbg << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ComposedAnalysis::runAnalysis" << endl;
}

// Execute the analysis transfer function, updating its dataflow info.
// The final state of dfInfo will map a Lattice* object to each outgoing or incoming PartEdge.
// Returns true if the Lattices in dfInfo are modified and false otherwise.
bool ComposedAnalysis::transferDFState(PartPtr part, CFGNode cn, SgNode* sgn, NodeState& state, 
                                       map<PartEdgePtr, vector<Lattice*> >& dfInfo, const set<PartPtr>& ultimateParts)
{
  scope reg("Transferring", scope::medium, composedAnalysisDebugLevel, 1);
  bool modified = false;
/*        if (isSgFunctionCallExp(sgn))
    //transferFunctionCall(func, part, *c, state);
    dynamic_cast<InterProceduralDataflow*>(interAnalysis)->
                                             transfer(func, part, *c, *state, dfInfo[NULLPartEdge]);*/

  // When a dfInfo map goes into a transfer function it must only have one key: the NULL edge
  assert(dfInfo.size()==1);
  assert(dfInfo.find(NULLPartEdge) != dfInfo.end());
  
  boost::shared_ptr<DFTransferVisitor> transferVisitor = getTransferVisitor(part, cn, state, dfInfo);
  sgn->accept(*transferVisitor);
  modified = transferVisitor->finish() || modified;
  
  dbg << "dfInfo after transfer="<<endl;
  { indent ind; dbg << NodeState::str(dfInfo)<<endl; }

  // The transfer function must have either left dfInfo's NULL edge key alone or created one key for each
  // descendant edge
  list<PartEdgePtr> descEdges = getEdgesToDescendants(part);
  // If part is among the ultimate parts, which means that it has no descendants
  if(descEdges.size()==0 && ultimateParts.find(part)!=ultimateParts.end()) {
    dbg << "<b>Adding edge beyond ultimate part</b>"<<endl;
    // Set descEdges to contain a single wildcard edge in the direction of analysis flow so that we 
    // compute analysis results on both sides of starting and ending parts. This is important to simplify
    // interactions between forward and backward analyses since forward analyses begin their execution 
    // based on dataflow state immediately before the starting parts and backwards begin based on information
    // immediately after the ending parts.
    descEdges.push_back(getDirection() == fw ? part->outEdgeToAny(): 
                                               part->inEdgeFromAny());
  }
  
  // If the key is still the NULL edge
  if(dfInfo.size()==1 && (dfInfo.find(NULLPartEdge) != dfInfo.end())) {
    // Adjust dfInfo to make one copy of the value for each descendant edge

    if(composedAnalysisDebugLevel>=1) {
      dbg << "Descendant edges: #descEdges="<<descEdges.size()<<endl;
      /*for(list<PartEdgePtr>::iterator e=descEdges.begin(); e!=descEdges.end(); e++)
      { indent ind; dbg << ":" << (*e)->str() << endl; }*/
      /*dbg << "dfInfo="<<endl;
      { indent ind; dbg << NodeState::str(dfInfo) << endl; }*/
    }

    if(descEdges.size() > 0) {
      list<PartEdgePtr>::iterator first=descEdges.begin();
      // First, map the original key to the first descendant edge
      dfInfo[*first] = dfInfo[NULLPartEdge];
      dfInfo.erase(NULLPartEdge);
      
      // First copy this edge's value to the other descendant edges
      {list<PartEdgePtr>::iterator e=first;
      for(e++; e!=descEdges.end(); e++) {
        NodeState::copyLatticesOW(dfInfo, *e, dfInfo, *first, true);
      }}
      
      // Set to *e the PartEdge of all the lattices stored under each edge
      for(list<PartEdgePtr>::iterator e=first; e!=descEdges.end(); e++) {
        for(vector<Lattice*>::iterator l=dfInfo[*e].begin(); l!=dfInfo[*e].end(); l++) {
          //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;e="<<(*e)->str()<<endl;
          (*l)->setPartEdge(*e);//modified = (*l)->setPartEdge(*e) || modified;
        }
        
        // Remap the MemLocs inside this edge's Lattice to account for this PartEdge's changes in scope
        /*// This is done before we adjust the partEdge so that the remap function has access to the 
        // PartEdge before the scope change (its current PartEdge) and after the scope change (the PartEdge 
        // provided as the argument)*/
        remapML((getDirection()==fw? part->inEdgeFromAny(): part->outEdgeToAny()), dfInfo[*e]);
        
        { scope mpsReg("Remapped DFState", scope::low, composedAnalysisDebugLevel, 1);
        for(vector<Lattice*>::iterator df=dfInfo[*e].begin(); df!=dfInfo[*e].end(); df++)
          dbg << (*df)->str()<<endl; }
      }
      /*
      // Set to *first the PartEdge of all the lattices stored under this edge
      for(vector<Lattice*>::iterator l=dfInfo[*first].begin(); l!=dfInfo[*first].end(); l++) {
        //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;first="<<(*first)->str()<<endl;
        (*l)->setPartEdge(*first); //modified = (*l)->setPartEdge(*first) || modified;
      }

      // Now copy its value to the other descendant edges
      list<PartEdgePtr>::iterator e=first;
      for(e++; e!=descEdges.end(); e++) {
        NodeState::copyLatticesOW(dfInfo, *e, dfInfo, *first, true);
        // Set to *e the PartEdge of all the lattices stored under this edge
        for(vector<Lattice*>::iterator l=dfInfo[*e].begin(); l!=dfInfo[*e].end(); l++) {
          //dbg << "&nbsp;&nbsp;&nbsp;&nbsp;e="<<(*e)->str()<<endl;
          (*l)->setPartEdge(*e);//modified = (*l)->setPartEdge(*e) || modified;
        }
      }*/
    }
  // If the key has been changed
  } else {
    // Verify that it has been changed correctly and otherwise leave it alone
    assert(descEdges.size() == dfInfo.size());
    for(list<PartEdgePtr>::iterator e=descEdges.begin(); e!=descEdges.end(); e++)
      assert(dfInfo.find(*e) != dfInfo.end());
  }

  return modified;
}

// Propagates the Lattice* mapped to different PartEdges in dfInfo along these PartEdges
void ComposedAnalysis::propagateDF2Desc(PartPtr part, 
                                        bool modified, 
                                        // Set of all the Parts that have already been visited by the analysis
                                        set<PartPtr>& visited, 
                                        // Set of all the Parts that have been initialized
                                        set<PartPtr>& initialized,
                                        // The dataflow iterator that identifies the state of the iteration
                                        dataflowPartEdgeIterator* curNodeIt,
                                        map<PartEdgePtr, vector<Lattice*> >& dfInfo, 
                                        // anchor that denotes the current abstract state in the debug output
                                        anchor curPartAnchor,
                                        // graph widget that visualizes the flow of the worklist algorithm
                                        graph& worklistGraph,
                                        // Maps each Abstract State to the anchors of outgoing links that target it from the last visit to its predecessors
                                        map<PartPtr, set<anchor> >& toAnchors,
                                        // Maps each Abstract state to the anchors of the Parts that lead to it, as well as the Parts themselves
                                        map<PartPtr, set<pair<anchor, PartPtr> > >& fromAnchors)
{
  list<PartPtr>   descendants = getDescendants(part);
  list<PartEdgePtr> descEdges = getEdgesToDescendants(part);
  
  scope reg(txt() << " Propagating/Merging the outgoing  Lattice to all descendant nodes("<<descEdges.size()<<")", scope::medium, composedAnalysisDebugLevel, 1);
  
  // Iterate over all descendants
  list<PartPtr>::iterator d;
  list<PartEdgePtr>::iterator de;
  for(d = descendants.begin(), de = descEdges.begin(); de != descEdges.end(); d++, de++)
  {
    // The CFG node corresponding to the current descendant of n
    PartEdgePtr nextPartEdge = *de;
    PartPtr nextPart = (getDirection() == fw? nextPartEdge->target(): nextPartEdge->source());
    dbg << "nextPartEdge="<<nextPartEdge->str()<<endl;
    dbg << "nextPart="<<(nextPart? nextPartEdge->str(): "NULLPart")<<endl;
    dbg << "*d="<<(*d? (*d)->str(): "NULLPart")<<endl;
    assert(nextPart);
    
    scope regDesc(txt() << "Descendant: "<<nextPart->str(), scope::low, composedAnalysisDebugLevel, 1);
    
    // Add an anchor to toAnchors from the current Abstract State to its current descendant
    anchor toAnchor;
    dbg << toAnchor.linkImg()<<endl;
    worklistGraph.addDirEdge(curPartAnchor, toAnchor);
    toAnchors[nextPart].insert(toAnchor);
    fromAnchors[nextPart].insert(make_pair(curPartAnchor, part));
    
    set<CFGNode> matches;
    if((getDirection()==fw && part->mayIncomingFuncCall(matches)) ||
       (getDirection()==bw && part->mayOutgoingFuncCall(matches))) {
      // This should only happen on parts with a single outgoing edge. If not, we should refactor this code to do the unioning once for all edges.
      assert(descEdges.size()==1);
      scope mpReg("Replacing State at Matching Parts", scope::medium, composedAnalysisDebugLevel, 1);
      
      // The set of Parts that contain the outgoing portion of the function call for this incoming portion or
      // vice versa
      set<PartPtr> matchingParts = part->matchingCallParts();
      
      vector<Lattice*> unionLats;
      
      {scope mpsReg("matchingParts", scope::medium, composedAnalysisDebugLevel, 1);
      //for(set<PartPtr>::iterator mp=matchingParts.begin(); mp!=matchingParts.end(); mp++)
        //dbg << mp->get()->str()<<endl; }
      
      assert(matchingParts.size()>0);
      for(set<PartPtr>::iterator mp=matchingParts.begin(); mp!=matchingParts.end(); mp++) {
        scope mpsReg2(mp->get()->str(), scope::low, composedAnalysisDebugLevel, 1);
        NodeState* mpState = NodeState::getNodeState(this, *mp);
        dbg << "mpState="<<mpState->str()<<endl;
        map<PartEdgePtr, vector<Lattice*> >& mpDFInfo = (getDirection()==fw? getLatticeAnte(mpState) : getLatticePost(mpState));
        for(map<PartEdgePtr, vector<Lattice*> >::iterator df=mpDFInfo.begin(); df!=mpDFInfo.end(); df++) {
          for(unsigned int i=0; i<mpDFInfo.size(); i++) {
            if(mp==matchingParts.begin() && df==mpDFInfo.begin())
              unionLats.push_back(df->second[i]->copy());
            else
              unionLats[i]->meetUpdate(df->second[i]);
          }
        }
      }}
      
      { scope mpsReg("unionLats", scope::low, composedAnalysisDebugLevel, 1);
      for(vector<Lattice*>::iterator ul=unionLats.begin(); ul!=unionLats.end(); ul++)
        dbg << (*ul)->str()<<endl; }
      
      // Update the lattices before the function call with the remapped information produced by the function call
      for(unsigned int i=0; i<unionLats.size(); i++) {
        unionLats[i]->setPartEdge(*de);
        unionLats[i]->replaceML(dfInfo[*de][i]);
        Lattice* oldDF = dfInfo[*de][i];
        dfInfo[*de][i] = unionLats[i];
        delete oldDF;
      }
      
      { scope mpsReg("Replaced DFState", scope::low, composedAnalysisDebugLevel, 1);
      for(vector<Lattice*>::iterator df=dfInfo[*de].begin(); df!=dfInfo[*de].end(); df++)
        dbg << (*df)->str()<<endl; }
    }
    
    NodeState* nextState = NodeState::getNodeState(this, nextPart);
    
    // Initialize this descendant's state if it has not yet been
    if(initialized.find(nextPart) == initialized.end()) {
      initializeState(nextPart, *nextState);
      initialized.insert(nextPart);
    }
    
    if(composedAnalysisDebugLevel>=1) dbg << "nextState="<<nextState->str()<<endl;

    // Make sure that dfInfo has a key for this descendant
    assert(dfInfo.find(*de) != dfInfo.end());

    // The temporary dfInfo map for this descendant will have its Lattice* vector from dfInfo mapped 
    // under the NULL edge key
    map<PartEdgePtr, vector<Lattice*> > dfInfoNext;
    dfInfoNext[NULLPartEdge] = dfInfo[*de];
    
    // Propagate the Lattices below this node to its descendant
    modified = propagateStateToNextNode(dfInfoNext, part, getLatticeAnte(nextState), nextPart);
    if(composedAnalysisDebugLevel>=1){
      dbg << "Propagated/merged: "<<(modified? "<font color=\"#990000\">Modified</font>": "<font color=\"#000000\">Not Modified</font>")<<endl;
      dbg << "<hline>";
    }
    // If the next node's state gets modified as a result of the propagation, or the next node has not yet been
    // visited, add it to the processing queue.
    if(composedAnalysisDebugLevel>=1) 
      dbg << "Final modified="<<modified<<", visited="<<(visited.find(nextPart)!=visited.end())<<" nextPart="<<nextPart->str()<<endl;
    if(modified || visited.find(nextPart)==visited.end())
      curNodeIt->add(nextPartEdge);
  }
}

/*************************
 ***** UndirDataflow *****
 *****  FWDataflow   *****
 *****  BWDataflow   *****
 *************************/

std::map<PartEdgePtr, std::vector<Lattice*> > UndirDataflow::emptyMap;

  
set<PartPtr>
FWDataflow::getInitialWorklist()
{
  // Initialize the set of nodes that this dataflow will iterate over
  return getComposer()->GetStartAStates(this);
}

set<PartPtr>
BWDataflow::getInitialWorklist()
{
  return getComposer()->GetEndAStates(this);
}

map<PartEdgePtr, vector<Lattice*> >& FWDataflow::getLatticeAnte(NodeState *state) { return state->getLatticeAboveAllMod(this); }
map<PartEdgePtr, vector<Lattice*> >& FWDataflow::getLatticePost(NodeState *state) { return state->getLatticeBelowAllMod(this); }
void FWDataflow::setLatticeAnte(NodeState *state, map<PartEdgePtr, vector<Lattice*> >& dfInfo, bool overwrite) {
  if(overwrite) state->copyLatticesOW(state->getLatticeAboveAllMod(this), dfInfo);
  else          state->copyLattices  (state->getLatticeAboveAllMod(this), dfInfo);
}
void FWDataflow::setLatticePost(NodeState *state, map<PartEdgePtr, vector<Lattice*> >& dfInfo, bool overwrite) { 
  if(overwrite) state->copyLatticesOW(state->getLatticeBelowAllMod(this), dfInfo);
  else          state->copyLattices  (state->getLatticeBelowAllMod(this), dfInfo);
}
map<PartEdgePtr, vector<Lattice*> >& BWDataflow::getLatticeAnte(NodeState *state) { return state->getLatticeBelowAllMod(this); }
map<PartEdgePtr, vector<Lattice*> >& BWDataflow::getLatticePost(NodeState *state) { return state->getLatticeAboveAllMod(this); }
void BWDataflow::setLatticeAnte(NodeState *state, map<PartEdgePtr, vector<Lattice*> >& dfInfo, bool overwrite) {
  if(overwrite) state->copyLatticesOW(state->getLatticeBelowAllMod(this), dfInfo);
  else          state->copyLattices  (state->getLatticeBelowAllMod(this), dfInfo);
}
void BWDataflow::setLatticePost(NodeState *state, map<PartEdgePtr, vector<Lattice*> >& dfInfo, bool overwrite) { 
  if(overwrite) state->copyLatticesOW(state->getLatticeAboveAllMod(this), dfInfo);
  else          state->copyLattices  (state->getLatticeAboveAllMod(this), dfInfo);
}

list<PartPtr> FWDataflow::getDescendants(PartPtr part)
{ 
  list<PartPtr> descendants;
  list<PartEdgePtr> outEdges = part->outEdges();
  for(list<PartEdgePtr>::iterator ei=outEdges.begin(); ei!=outEdges.end(); ei++)
    descendants.push_back((*ei)->target());
  return descendants;
}

list<PartPtr> BWDataflow::getDescendants(PartPtr part)
{ 
  list<PartPtr> descendants;
  list<PartEdgePtr> inEdges = part->inEdges();
  for(list<PartEdgePtr>::iterator ei=inEdges.begin(); ei!=inEdges.end(); ei++)
    descendants.push_back((*ei)->source());
  return descendants;
}

list<PartEdgePtr> FWDataflow::getEdgesToDescendants(PartPtr part)
{ 
  return part->outEdges();
}

list<PartEdgePtr> BWDataflow::getEdgesToDescendants(PartPtr part)
{  
 return part->inEdges();
}

set<PartPtr> FWDataflow::getUltimate()
{ return getComposer()->GetEndAStates(this); }
set<PartPtr> BWDataflow::getUltimate()
{ return getComposer()->GetStartAStates(this); }

dataflowPartEdgeIterator* FWDataflow::getIterator()
{ 
  //set<PartPtr> terminalStates = getComposer()->GetEndAStates(this);
  return new fw_dataflowPartEdgeIterator(/*terminalStates*/);
}
dataflowPartEdgeIterator* BWDataflow::getIterator()
{ return new bw_dataflowPartEdgeIterator(/*getComposer()->GetStartAState(this)*/); }

// Remaps the given Lattice across the scope transition (if any) of the given edge, updating the lat vector
// with pointers to the updated Lattice objects and deleting old Lattice objects as needed.
void FWDataflow::remapML(PartEdgePtr fromPEdge, vector<Lattice*>& lat) {
  scope reg("FWDataflow::remapML", scope::medium, composedAnalysisDebugLevel, 1);
  for(unsigned int i=0; i<lat.size(); i++) {
    dbg << "lat["<<i<<"]="<<(lat[i]? lat[i]->str(): "NULL")<<endl;
    Lattice* newL = lat[i]->getPartEdge()->forwardRemapML(lat[i], fromPEdge);
    dbg << "newL="<<(newL? newL->str(): "NULL")<<endl;
    // If any remapping was done, update lat
    if(newL) {
      delete lat[i];
      lat[i] = newL;
    }
  }
}

// Remaps the given Lattice across the scope transition (if any) of the given edge, updating the lat vector
// with pointers to the updated Lattice objects and deleting old Lattice objects as needed.
void BWDataflow::remapML(PartEdgePtr fromPEdge, vector<Lattice*>& lat) {
  scope reg("BWDataflow::remapML", scope::medium, composedAnalysisDebugLevel, 1);
  for(unsigned int i=0; i<lat.size(); i++) {
    dbg << "lat["<<i<<"]->getPartEdge()="<<lat[i]->getPartEdge()->str()<<endl;
    Lattice* newL = lat[i]->getPartEdge()->backwardRemapML(lat[i], fromPEdge);
    // If any remapping was done, update lat
    if(newL) {
      delete lat[i];
      lat[i] = newL;
    }
  }
}


/******************************************************
 ***      printDataflowInfoPass                     ***
 *** Prints out the dataflow information associated ***
 *** with a given analysis for every CFG node a     ***
 *** function.                                      ***
 ******************************************************/

// Initializes the state of analysis lattices at the given function, part and edge into our out of the part
// by setting initLattices to refer to freshly-allocated Lattice objects.
void printDataflowInfoPass::genInitLattice(PartPtr part, PartEdgePtr pedge, 
                                           std::vector<Lattice*>& initLattices)
{
  initLattices.push_back((Lattice*)(new BoolAndLattice(0, pedge)));
}
  
bool printDataflowInfoPass::transfer(PartPtr part, CFGNode cn, NodeState& state, 
                                     std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo)
{
  dbg << "-----#############################--------\n";
  dbg << "Node: ["<<part->str()<<"\n";
  dbg << "State:\n";
  indent ind(composedAnalysisDebugLevel, 1); 
  dbg << state.str(analysis)<<endl;
  
  return dynamic_cast<BoolAndLattice*>(dfInfo[NULLPartEdge][0])->set(true);
}

/***************************************************
 ***            checkDataflowInfoPass            ***
 *** Checks the results of the composed analysis ***
 *** chain at special assert calls.              ***
 ***************************************************/

// Initializes the state of analysis lattices at the given function, part and edge into our out of the part
// by setting initLattices to refer to freshly-allocated Lattice objects.
void checkDataflowInfoPass::genInitLattice(PartPtr part, PartEdgePtr pedge, 
                                           std::vector<Lattice*>& initLattices)
{
  dbg << "<<<checkDataflowInfoPass::genInitLattice"<<endl;
  initLattices.push_back((Lattice*)(new BoolAndLattice(0, pedge)));
  dbg << ">>>checkDataflowInfoPass::genInitLattice"<<endl;
}
  
bool checkDataflowInfoPass::transfer(PartPtr part, CFGNode cn, NodeState& state, 
                                     std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo)
{
  set<CFGNode> nodes = part->CFGNodes();
  for(set<CFGNode>::iterator n=nodes.begin(); n!=nodes.end(); n++) {
    SgFunctionCallExp* call;
    if((call = isSgFunctionCallExp(n->getNode())) && n->getIndex()==2) {
      Function func(call);
      if(func.get_name().getString() == "CompDebugAssert") {
        SgExpressionPtrList args = call->get_args()->get_expressions();
        for(SgExpressionPtrList::iterator a=args.begin(); a!=args.end(); a++) {
          ValueObjectPtr v = getComposer()->OperandExpr2Val(call, *a, part->inEdgeFromAny(), this);
          assert(v);
          ostringstream errorMesg;
          if(!v->isConcrete())
            errorMesg << "Debug assertion at "<<call->get_file_info()->get_filenameString()<<":"<<call->get_file_info()->get_line()<<" failed: concrete interpretation not available! test="<<(*a)->unparseToString()<<" v="<<v->str();
          else {
            set<boost::shared_ptr<SgValueExp> > concreteVals = v->getConcreteValue();
            if(concreteVals.size()==0)
              errorMesg << "Debug assertion at "<<call->get_file_info()->get_filenameString()<<":"<<call->get_file_info()->get_line()<<" failed: interpretation not convertible to a boolean! test="<<(*a)->unparseToString()<<" v="<<v->str()<<" v->getConcreteValue() is empty!";
            
            for(set<boost::shared_ptr<SgValueExp> >::iterator cv=concreteVals.begin(); cv!=concreteVals.end(); cv++) {
              if(!ValueObject::isValueBoolCompatible(*cv))
                errorMesg << "Debug assertion at "<<call->get_file_info()->get_filenameString()<<":"<<call->get_file_info()->get_line()<<" failed: interpretation not convertible to a boolean! test="<<(*a)->unparseToString()<<" v="<<v->str()<<" v->getConcreteValue()="<<SgNode2Str(cv->get());
              else if(!ValueObject::SgValue2Bool(*cv))
                errorMesg << "Debug assertion at "<<call->get_file_info()->get_filenameString()<<":"<<call->get_file_info()->get_line()<<" failed: test evaluates to false! test="<<(*a)->unparseToString()<<" v="<<v->str()<<" v->getConcreteValue()="<<SgNode2Str(cv->get());
            }
          }
          
          if(errorMesg.str() != "") {
            cerr << errorMesg.str() << endl;
            dbg << "<h1><font color=\"#ff0000\">"<<errorMesg.str()<<"</font></h1>"<<endl;
            numErrors++;
          }
        }
      }
    }
  }
  
  return dynamic_cast<BoolAndLattice*>(dfInfo[NULLPartEdge][0])->set(true);
}

}; // namespace fuse
