#include "sage3basic.h"
#include "dead_path_elim_analysis.h"

using namespace std;
using namespace dbglog;

namespace fuse {

int deadPathElimAnalysisDebugLevel=0;
  
/****************************
 ***** DeadPathElimPart *****
 ****************************/

DeadPathElimPart::DeadPathElimPart(PartPtr base, ComposedAnalysis* analysis) :
  Part(analysis, base)
{}

DeadPathElimPart::DeadPathElimPart(const DeadPathElimPart& that) :
  Part((const Part&)that)
{}

// Returns a shared pointer to this of type DeadPathElimPartPtr
DeadPathElimPartPtr DeadPathElimPart::get_shared_this()
{ return dynamicPtrCast<DeadPathElimPart>(makePtrFromThis(shared_from_this())); }

// -------------------------------------------
// Functions that need to be defined for Parts
// -------------------------------------------

list<PartEdgePtr> DeadPathElimPart::outEdges()
{
  list<PartEdgePtr> baseEdges = getParent()->outEdges();
  list<PartEdgePtr> dpeEdges;
  
  scope reg(txt()<<"DeadPathElimPart::outEdges() #baseEdges="<<baseEdges.size(), scope::medium, 2, deadPathElimAnalysisDebugLevel);
  
  // The NodeState at the current part
  NodeState* outState = NodeState::getNodeState(analysis, getParent());
  //dbg << "outState="<<outState->str(analysis)<<endl;
  // Consider all the DeadPathElimParts along all of this part's outgoing edges. Since this is a forward
  // analysis, they are maintained separately
  for(list<PartEdgePtr>::iterator be=baseEdges.begin(); be!=baseEdges.end(); be++) {
    if(deadPathElimAnalysisDebugLevel>=2) dbg << "be="<<be->str()<<endl;
    DeadPathElimPartEdge* outPartEdge = dynamic_cast<DeadPathElimPartEdge*>(outState->getLatticeBelow(analysis, *be, 0));
    assert(outPartEdge);
    if(deadPathElimAnalysisDebugLevel>=2) dbg << "outPartEdge="<<outPartEdge->str()<<endl;
    
    //dbg << "outPartEdge (live="<<(outPartEdge->level==live)<<")="<<outPartEdge->str()<<endl;
    if(outPartEdge->level==live)
      // Create a new DeadPathElimPartEdgePtr from a copy of outPartEdge to ensure that the original is not deallocated
      // when the shared pointer goes out of scope.
      dpeEdges.push_back(initPtr(dynamic_cast<DeadPathElimPartEdge*>(outPartEdge)));
      /*dpeEdges.push_back(makePtr<DeadPathElimPartEdge>(
                             / *makePtrFromThis(shared_from_this()),
                             // Create a shared pointer from a copy of the raw pointer to make sure that the original 
                             // object is not deleted when the reference count drops to 0.
                             initPtr(dynamic_cast<DeadPathElimPart*>(outPart->copy())),* /
                             *be, analysis));*/
  }
  return dpeEdges;
}

list<PartEdgePtr> DeadPathElimPart::inEdges()
{
  list<PartEdgePtr> baseEdges = getParent()->inEdges();
  list<PartEdgePtr> dpeEdges;
  
  scope reg(txt()<<"DeadPathElimPart::inEdges() #baseEdges="<<baseEdges.size(), scope::medium, 2, deadPathElimAnalysisDebugLevel);
  
  // Since this is a forward analysis, information from preceding parts is aggregated under the NULL edge
  // of this part. As such, to get the parts that lead to this part we need to iterate over the incoming edges
  // and then look at the parts they arrive from.
  for(list<PartEdgePtr>::iterator be=baseEdges.begin(); be!=baseEdges.end(); be++) {
    if(deadPathElimAnalysisDebugLevel>=2) dbg << "be="<<be->str()<<endl;
    NodeState* inState = NodeState::getNodeState(analysis, (*be)->source());
    { scope inscope("inState", scope::low, deadPathElimAnalysisDebugLevel, 2); if(deadPathElimAnalysisDebugLevel>=2) dbg << inState->str()<<endl; }
    DeadPathElimPartEdge* inPartEdge = dynamic_cast<DeadPathElimPartEdge*>(inState->getLatticeBelow(analysis, *be, 0));
    assert(inPartEdge);
    if(deadPathElimAnalysisDebugLevel>=2) dbg << "inPartEdge="<<inPartEdge->str()<<endl;
    
    if(inPartEdge->level==live)
      // Create a new DeadPathElimPartEdgePtr from a copy of outPartEdge to ensure that the original is not deallocated
      // when the shared pointer goes out of scope.
      dpeEdges.push_back(initPtr(dynamic_cast<DeadPathElimPartEdge*>(inPartEdge->copy())));
      /*dpeEdges.push_back(makePtr<DeadPathElimPartEdge>(
                           // Create a shared pointer from a copy of the raw pointer to make sure that the original 
                           // object is not deleted when the reference count drops to 0.
                           / *initPtr(dynamic_cast<DeadPathElimPart*>(inPart->copy())),
                           makePtrFromThis(shared_from_this()),* /
                           *be, analysis));*/
  }
  return dpeEdges;
}

set<CFGNode> DeadPathElimPart::CFGNodes() const
{
  return getParent()->CFGNodes();
}

// If this Part corresponds to a function call/return, returns the set of Parts that contain
// its corresponding return/call, respectively.
set<PartPtr> DeadPathElimPart::matchingCallParts() const {
  set<PartPtr> matchParts;
  
  // Wrap the parts returned by the call to the parent Part with DeadPathElimPart
  set<PartPtr> parentMatchParts = getParent()->matchingCallParts();
  for(set<PartPtr>::iterator mp=parentMatchParts.begin(); mp!=parentMatchParts.end(); mp++) {
    matchParts.insert(makePtr<DeadPathElimPart>(*mp, analysis));
  }
  return matchParts;
}

/*
// Let A={ set of execution prefixes that terminate at the given anchor SgNode }
// Let O={ set of execution prefixes that terminate at anchor's operand SgNode }
// Since to reach a given SgNode an execution must first execute all of its operands it must
//    be true that there is a 1-1 mapping m() : O->A such that o in O is a prefix of m(o).
// This function is the inverse of m: given the anchor node and operand as well as the
//    Part that denotes a subset of A (the function is called on this part), 
//    it returns a list of Parts that partition O.
list<PartPtr> DeadPathElimPart::getOperandPart(SgNode* anchor, SgNode* operand)
{
  if(level==live) {
    list<PartPtr> baseOpParts = base->getOperandPart(anchor, operand);
    list<PartPtr> dpeOpParts;
    for(list<PartPtr>::iterator p=baseOpParts.begin(); p!=baseOpParts.end(); p++) {
      NodeState* inState = NodeState::getNodeState(analysis, (*be)->source());
      DeadPathElimPart* inPart = dynamic_cast<DeadPathElimPart*>(inState->getLatticeAbove(analysis, *be, 0));
      assert(inPart);
      dpeOpParts = make_part<DeadPathElimParts>()
    }
    return baseOpParts;
  } else {
    list<PartPtr> emptyL;
    return emptyL;
  }
}
*/
// Returns a PartEdgePtr, where the source is a wild-card part (NULLPart) and the target is this Part
PartEdgePtr DeadPathElimPart::inEdgeFromAny()
{ return makePtr<DeadPathElimPartEdge>(/*NULLPart, makePtrFromThis(shared_from_this()), */
                                        getParent()->inEdgeFromAny(), analysis); }

// Returns a PartEdgePtr, where the target is a wild-card part (NULLPart) and the source is this Part
PartEdgePtr DeadPathElimPart::outEdgeToAny()
{ return makePtr<DeadPathElimPartEdge>(/*makePtrFromThis(shared_from_this()), NULLPart,*/
                                        getParent()->outEdgeToAny(), analysis); }

bool DeadPathElimPart::equal(const PartPtr& o) const
{
  const DeadPathElimPartPtr that = dynamicConstPtrCast<DeadPathElimPart>(o);
  assert(that.get());
  assert(analysis == that->analysis);
  
  return getParent() == that->getParent();
}

bool DeadPathElimPart::less(const PartPtr& o) const
{
  const DeadPathElimPartPtr that = dynamicConstPtrCast<DeadPathElimPart>(o);
  assert(that.get());
  assert(analysis == that->analysis);
  
  return getParent() < that->getParent();
}

// Pretty print for the object
std::string DeadPathElimPart::str(std::string indent)
{
  ostringstream oss;
  oss << "[DPEPart: "<<getParent()->str()<<"]";
  return oss.str();
}

/************************************
 ***** DeadPathElimPartEdge *****
 ************************************/
/* GB 2012-10-15 - Commented out because this constructor makes it difficult to set the lattice of the created edge
DeadPathElimPartEdge::DeadPathElimPartEdge(DeadPathElimPartPtr src, DeadPathElimPartPtr tgt, 
                                           PartEdgePtr baseEdge, DeadPatComposedAnalysishElimAnalysis* analysis) : 
    Lattice(baseEdge), FiniteLattice(baseEdge), baseEdge(baseEdge), src(src), tgt(tgt), level(bottom), analysis(analysis)
{}*/

// Constructor to be used when constructing the edges (e.g. from genInitLattice()).
DeadPathElimPartEdge::DeadPathElimPartEdge(PartEdgePtr baseEdge, ComposedAnalysis* analysis, DPELevel level) : 
        Lattice(baseEdge), FiniteLattice(baseEdge), PartEdge(analysis, baseEdge)
{
  src = latPEdge->source() ? makePtr<DeadPathElimPart>(latPEdge->source(), analysis) : dynamicPtrCast<DeadPathElimPart>(NULLPart);
  tgt = latPEdge->target() ? makePtr<DeadPathElimPart>(latPEdge->target(), analysis) : dynamicPtrCast<DeadPathElimPart>(NULLPart);
  /*dbg << "DeadPathElimPartEdge::DeadPathElimPartEdge()"<<endl;
  dbg << "latPEdge="<<latPEdge->str()<<endl;
  dbg << "src="<<(src? src->str() : "NULL")<<endl;
  dbg << "tgt="<<(tgt? tgt->str() : "NULL")<<endl;*/

  this->level = level;
}

// Constructor to be used when traversing the part graph created by the DeadPathElimAnalysis, after
// all the DeadPathElimPartEdges have been constructed and stored in NodeStates.
DeadPathElimPartEdge::DeadPathElimPartEdge(PartEdgePtr baseEdge, ComposedAnalysis* analysis) : 
        Lattice(baseEdge), FiniteLattice(baseEdge), PartEdge(analysis, baseEdge)
{
  /*src = base.source() ? makePtr<DeadPathElimPart>(base.source(), analysis, bottom) : NULLPart;
  tgt = base.target() ? makePtr<DeadPathElimPart>(base.target(), analysis, bottom) : NULLPart;
  level = bottom;*/

  // Look up this edge in the results of the DeadPathElimAnalysis results and copy data from that edge into this object
  //DeadPathElimPartEdge* dpeEdge;
  if(latPEdge->source()) src=makePtr<DeadPathElimPart>(latPEdge->source(), analysis);
  if(latPEdge->target()) tgt=makePtr<DeadPathElimPart>(latPEdge->target(), analysis);

  // If the source is not a wildcard, look for the record in the source part, which maintains separate information
  // for all the outgoing edges
  //dbg << "latPEdge="<<latPEdge->str()<<endl;
  //dbg << "this="<<str()<<endl;
  // If the edge has a concrete source and target
  if(latPEdge->source() && latPEdge->target()) {
    /*DeadPathElimPartPtr sourceDPEPart = makePtr<DeadPathElimPart>(latPEdge->source(), analysis, bottom);
    dbg << "seEdge->source()="<<latPEdge->source()->str()<<endl;
    dbg << "seEdge->target()="<<latPEdge->target()->str()<<endl;
    dbg << "sourceDPEPart="<<sourceDPEPart->str()<<endl;*/
    NodeState* state = NodeState::getNodeState(analysis, latPEdge->source());
    /*list<PartEdgePtr> edges = latPEdge->source()->outEdges();
    dbg << "source->outEdges="<<endl;
    for(list<PartEdgePtr>::iterator e=edges.begin(); e!=edges.end(); e++)
    { dbg << (*e)->str()<<endl; }*/
    
    if(deadPathElimAnalysisDebugLevel>=2) {
      dbg << "source state="<<endl<<"        "<<state->str(analysis, "        ")<<endl;
      dbg << "latPEdge="<<latPEdge->str("        ")<<endl;
    }
    // Get the DeadPathElimPartEdge that is stored along latPEdge at the NodeState of its source part
    DeadPathElimPartEdge* dpeEdge = dynamic_cast<DeadPathElimPartEdge*>(state->getLatticeBelow(analysis, latPEdge, 0));
    if(deadPathElimAnalysisDebugLevel>=2) dbg << "dpeEdge lattice = "<<state->getLatticeBelow(analysis, latPEdge, 0)->str("        ")<<endl;
    level = dpeEdge->level;
  // If the target is a wildcard look at the source part and aggregate the DPEEdges along all the outgoing paths.
  // The resulting edge is live if any of the outgoing edges are live.
  } else if(latPEdge->source()) {
    NodeState* state = NodeState::getNodeState(analysis, latPEdge->source());
  
    if(deadPathElimAnalysisDebugLevel>=2) {
      dbg << "source state="<<endl<<"        "<<state->str(analysis, "        ")<<endl;
      dbg << "latPEdge="<<latPEdge->str("        ")<<endl;
    }
    
    // Merge the lattices along all the outgoing edges
    map<PartEdgePtr, std::vector<Lattice*> >& e2lats = state->getLatticeBelowAllMod(analysis);
    assert(e2lats.size()>=1);

    level = dead;
    for(map<PartEdgePtr, std::vector<Lattice*> >::iterator lats=e2lats.begin(); lats!=e2lats.end(); lats++) {
      PartEdge* edgePtr = lats->first.get();
      dbg << "edgePtr="<<edgePtr->str("        ")<<endl;
      assert(edgePtr->source() == latPEdge.get()->source());
      
      DeadPathElimPartEdge* dpeEdge = dynamic_cast<DeadPathElimPartEdge*>(state->getLatticeBelow(analysis, lats->first, 0));
      assert(dpeEdge);

      if(dpeEdge->level == live) level = live;
    }
  // If the source is a wildcard, look for the record in the target part where all the edges are aggregated
  } else if(latPEdge->target()) {
    assert(latPEdge->target());
    DeadPathElimPartPtr targetDPEPart = makePtr<DeadPathElimPart>(latPEdge->target(), analysis);
    NodeState* state = NodeState::getNodeState(analysis, latPEdge->target());
    DeadPathElimPartEdge* dpeEdge = dynamic_cast<DeadPathElimPartEdge*>(state->getLatticeAbove(analysis, 0));
    level = dpeEdge->level;
  }
  //assert(dpeEdge);
  
  /*// Copy its data to this object. If source or target information is not available from dpeEdge but
  // is available from the latPEdge, use its information. In general we only need source information
  // for results of getOperandPartEdge() and in this case the dpeEdge will not have the source because
  level = dpeEdge->level;
  if(dpeEdge->src) src = dpeEdge->src;
  else if(latPEdge->source()) src = makePtr<DeadPathElimPart>(latPEdge->source(), analysis);
  if(dpeEdge->tgt) tgt = dpeEdge->tgt;
  else if(latPEdge->target()) tgt = makePtr<DeadPathElimPart>(latPEdge->target(), analysis);*/
}

DeadPathElimPartEdge::DeadPathElimPartEdge(const DeadPathElimPartEdge& that) :
  Lattice(that.latPEdge), 
  FiniteLattice(that.latPEdge), 
  PartEdge((const PartEdge&)that), 
  src(that.src), tgt(that.tgt), level(that.level)
{}

// Returns a shared pointer to this of type DeadPathElimPartEdgePtr
DeadPathElimPartEdgePtr DeadPathElimPartEdge::get_shared_this()
{ return dynamicPtrCast<DeadPathElimPartEdge>(makePtrFromThis(shared_from_this())); }


PartPtr DeadPathElimPartEdge::source() const
{ return src; }

PartPtr DeadPathElimPartEdge::target() const
{ return tgt; }

// Overload the setPartEdge (from Lattice) and setParent (from Part) methods to ensure that they
// are always set in a consistent manner regardless of which one is called
// Sets the PartEdge that this Lattice's information corresponds to. 
// Returns true if this causes the edge to change and false otherwise
bool DeadPathElimPartEdge::setPartEdge(PartEdgePtr latPEdge)
{
  bool modified = Lattice::setPartEdge(latPEdge);
  PartEdge::setParent(latPEdge);
  
  // Look up this edge in the results of the DeadPathElimAnalysis results and copy data from that edge into this object
  if(latPEdge->source()) src=makePtr<DeadPathElimPart>(latPEdge->source(), analysis);
  if(latPEdge->target()) tgt=makePtr<DeadPathElimPart>(latPEdge->target(), analysis);
  
  return modified;
}

// Sets this Part's parent
void DeadPathElimPartEdge::setParent(PartEdgePtr parent)
{
  Lattice::setPartEdge(parent);
  PartEdge::setParent(parent);
}

// Let A={ set of execution prefixes that terminate at the given anchor SgNode }
// Let O={ set of execution prefixes that terminate at anchor's operand SgNode }
// Since to reach a given SgNode an execution must first execute all of its operands it must
//    be true that there is a 1-1 mapping m() : O->A such that o in O is a prefix of m(o).
// This function is the inverse of m: given the anchor node and operand as well as the
//    PartEdge that denotes a subset of A (the function is called on this PartEdge), 
//    it returns a list of PartEdges that partition O.
std::list<PartEdgePtr> DeadPathElimPartEdge::getOperandPartEdge(SgNode* anchor, SgNode* operand)
{
  // operand precedes anchor in the CFG, either immediately or at some distance. As such, the edge
  //   we're looking for is not necessarily the edge from operand to anchor but rather the first
  //   edge along the path from operand to anchor. Since operand is part of anchor's expression
  //   tree we're guaranteed that there is only one such path.
  // The implementor of the partition we're running on may have created multiple parts for 
  //   operand to provide path sensitivity and indeed, may have created additional outgoing edges
  //   from each of the operand's parts. Fortunately, since in the original program the original
  //   edge led from operand to anchor and the implementor of the partition could have only hierarchically 
  //   refined the original partition, all the new edges must also lead from operand to anchor.
  //   As such, the returned list contains all the outgoing edges from all the parts that correspond
  //   to operand.
  // Note: if the partitioning process is not hierarchical we may run into minor trouble since the 
  //   new edges from operand may lead to parts other than anchor. However, this is just an issue
  //   of precision since we'll account for paths that are actually infeasible.
  
  // The target of this edge identifies the termination point of all the execution prefixes
  // denoted by this edge. We thus use it to query for the parts of the operands and only both
  // if this part is itself live.
  scope reg("DeadPathElimPartEdge::getOperandPartEdge()", scope::medium, deadPathElimAnalysisDebugLevel, 1);
  if(deadPathElimAnalysisDebugLevel>=1) dbg << "anchor="<<SgNode2Str(anchor)<<" operand="<<SgNode2Str(operand)<<endl;
  
  if(level==live) {
    std::list<PartEdgePtr> baseEdges = getParent()->getOperandPartEdge(anchor, operand);
    std::list<PartEdgePtr> dpeEdges;
    for(std::list<PartEdgePtr>::iterator e=baseEdges.begin(); e!=baseEdges.end(); e++) {
      if(deadPathElimAnalysisDebugLevel>=1) dbg << "e="<<(*e)->str()<<endl;
      PartEdgePtr dpeEdge = makePtr<DeadPathElimPartEdge>(*e, analysis);
      { scope reg("dpeEdge", scope::low, deadPathElimAnalysisDebugLevel, 2);
      if(deadPathElimAnalysisDebugLevel>=1) dbg<<dpeEdge->str()<<endl; }
      dpeEdges.push_back(dpeEdge);
    }
    return dpeEdges;
/*            
    for(list<PartPtr>::iterator opP=opParts.begin(); opP!=opParts.end(); opP++) {
      list<PartEdgePtr> edges = (*opP)->outEdges();
      for(list<PartEdgePtr>::iterator e=edges.begin(); e!=edges.end(); e++) {
         2*dbg << "opP = "<<(*opP)->str()<<endl;
        dbg << "e = "<<(*e)->str()<<endl;
        dbg << "e->target() = "<<(*e)->target()->str()<<endl;* /
        assert(src || tgt);
        DeadPathElimAnalysis* analysis = (src? src->analysis : tgt->analysis);
        PartEdgePtr edge = makePtr<DeadPathElimPartEdge>(makePtr<DeadPathElimPart>((*opP)->inEdgeFromAny(), analysis), 
                                                          makePtr<DeadPathElimPart>((*e)->target()->inEdgeFromAny(), analysis));
        //dbg << "edge = "<<edge->str()<<endl;
        l.push_back(edge);
      }
    }
    return l;*/
  } else {
    list<PartEdgePtr> emptyL;
    return emptyL;
  }
}

// If the source Part corresponds to a conditional of some sort (if, switch, while test, etc.)
// it must evaluate some predicate and depending on its value continue, execution along one of the
// outgoing edges. The value associated with each outgoing edge is fixed and known statically.
// getPredicateValue() returns the value associated with this particular edge. Since a single 
// Part may correspond to multiple CFGNodes getPredicateValue() returns a map from each CFG node
// within its source part that corresponds to a conditional to the value of its predicate along 
// this edge. 
std::map<CFGNode, boost::shared_ptr<SgValueExp> > DeadPathElimPartEdge::getPredicateValue()
{
  return latPEdge->getPredicateValue();
}

// Adds a mapping from a CFGNode to the outcome of its predicate
void DeadPathElimPartEdge::mapPred2Val(CFGNode n, boost::shared_ptr<SgValueExp> val)
{
  predVals[n] = val;
}

// Empties out the mapping of CFGNodes to the outcomes of their predicates
void DeadPathElimPartEdge::clearPred2Val()
{
  predVals.clear();
}

bool DeadPathElimPartEdge::equal(const PartEdgePtr& o) const
{
  const DeadPathElimPartEdgePtr that = dynamicConstPtrCast<DeadPathElimPartEdge>(o);
  assert(that.get());
  if(latPEdge==that->latPEdge) {
    assert(src==that->src);
    assert(tgt==that->tgt);
    return true;
  } else
    return false;
}

bool DeadPathElimPartEdge::less(const PartEdgePtr& o)  const
{
  const DeadPathElimPartEdgePtr that = dynamicConstPtrCast<DeadPathElimPartEdge>(o);
  assert(that.get());

  return latPEdge < that->latPEdge;
}

// Pretty print for the object
std::string DeadPathElimPartEdge::str(std::string indent)
{
  ostringstream oss;
  if(latPEdge != getParent()) {
    dbg << "DeadPathElimPartEdge"<<endl;
    dbg << "this="<<"[DPEEdge("<<(level==dead? "D": (level==live? "L": (level==bottom? "B": "<font color=\"#FF0000\"><b>??? </b></font>")))<<"): "<<
                      (src ? src->str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;"): "NULL")<<" ==&gt; " <<
                      (tgt ? tgt->str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;"): "NULL")<<endl;
    dbg << "latPEdge="<<latPEdge->str()<<endl;
    dbg << "getParent()="<<getParent()->str()<<endl;
  }
  assert(latPEdge == getParent());
  oss << "[DPEEdge("<<(level==dead? "D": (level==live? "L": (level==bottom? "B": "<font color=\"#FF0000\"><b>??? </b></font>")))<<"): "<<
                      (src ? src->str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;"): "NULL")<<" ==&gt; " <<
                      (tgt ? tgt->str(indent+"&nbsp;&nbsp;&nbsp;&nbsp;"): "NULL")<<
                      ", "<<endl;
  oss << indent << "    latPEdge="<<latPEdge->str()/*<<", parent=<"<<getParent()->str()*/<<"]";
  return oss.str();
}

// ----------------------------------------------
// Functions that need to be defined for Lattices
// ----------------------------------------------

void DeadPathElimPartEdge::initialize() { }

// Returns a copy of this lattice
Lattice* DeadPathElimPartEdge::copy() const
{ return new DeadPathElimPartEdge(*this); }

// Overwrites the state of "this" Lattice with "that" Lattice
void DeadPathElimPartEdge::copy(Lattice* that_arg)
{
  Lattice::copy(that_arg);
  
  DeadPathElimPartEdge* that = dynamic_cast<DeadPathElimPartEdge*>(that_arg);
  assert(that);
  assert(PartEdge::compatible(*that));
  
  src   = that->src;
  tgt   = that->tgt;
  level = that->level;
}

bool DeadPathElimPartEdge::operator==(Lattice* that_arg) /*const*/
{
  // NOTE: because Lattices use pointers and Parts use boost::shared_ptrs we can't take advantage
  // of the base operator== from PartEdge. However, in this case this does not matter since Lattices
  // from different analyses can never be compared.
  DeadPathElimPartEdge* that = dynamic_cast<DeadPathElimPartEdge*>(that_arg);
  assert(that);
  assert(analysis == that->analysis);
  
  if(latPEdge==that->latPEdge) {
    assert(src==that->src);
    assert(tgt==that->tgt);
    return true;
  } else
    return false;
}

// Called by analyses to transfer this lattice's contents from across function scopes from a caller function 
//    to a callee's scope and vice versa. If this this lattice maintains any information on the basis of 
//    individual MemLocObjects these mappings must be converted, with MemLocObjects that are keys of the ml2ml 
//    replaced with their corresponding values. If a given key of ml2ml does not appear in the lattice, it must
//    be added to the lattice and assigned a default initial value. In many cases (e.g. over-approximate sets 
//    of MemLocObjects) this may not require any actual insertions. If the value of a given ml2ml mapping is 
//    NULL (empty boost::shared_ptr), any information for MemLocObjects that must-equal to the key should be 
//    deleted.
// Since the function is called for the scope change across some Part, it needs to account for the fact that
//    the keys in ml2ml are in scope on one side of Part, while the values on the other side. Specifically, it is
//    guaranteed that the keys are in scope at the edge returned by getPartEdge() while the values are in scope
//    at newPEdge.
// remapML must return a freshly-allocated object.
Lattice* DeadPathElimPartEdge::remapML(const std::set<MLMapping>& ml2ml, PartEdgePtr newPEdge) {
  return copy();
}
  
// Adds information about the MemLocObjects in newL to this Lattice, overwriting any information previously 
//    maintained in this lattice about them.
// Returns true if the Lattice state is modified and false otherwise.
bool DeadPathElimPartEdge::replaceML(Lattice* newL)
{
  return false;
}

// Computes the meet of this and that and saves the result in this
// Returns true if this causes this to change and false otherwise
bool DeadPathElimPartEdge::meetUpdate(Lattice* that_arg)
{
  DeadPathElimPartEdge* that = dynamic_cast<DeadPathElimPartEdge*>(that_arg);
  assert(that);
  // We don't check this becase when we meet information from a caller with information from a callee its a pain
  // to convert the edges from caller scope to callee scope, although this may be a good idea in the future to 
  // clean up the code.
  //assert(latPEdge==that->latPEdge);
  // We don't need to make sure that the sources are the same since they will be wildcards but will not necessarily be equal to each other
  //assert(src==that->src);
  //assert(tgt==that->tgt);
  assert(analysis==that->analysis);
  
  // The result of the meet is the max of the lattice points of the two arguments
  bool modified = (level<that->level);
  if(deadPathElimAnalysisDebugLevel>=1) dbg << "DeadPathElimPartEdge::meetUpdate() level="<<level<<" that->level="<<that->level<<" newLevel="<<(level<that->level? that->level: level)<<endl;
  level = (level<that->level? that->level: level);

  // Copy the new level to the source and target of the edge
  /*if(src) src->level = level;
  if(tgt) tgt->level = level;*/
  
  // Union the predVals maps
  for(map<CFGNode, boost::shared_ptr<SgValueExp> >::iterator v=that->predVals.begin(); v!=that->predVals.end(); v++) {
    // If both edges have a mapping for the current CFGNode, they must be the same
    if(predVals.find(v->first) != predVals.end())
      assert(ValueObject::equalValueExp(predVals[v->first].get(), v->second.get()));
    // Otherwise, add the new mapping to predVals
    else {
      predVals[v->first] = v->second;
      modified = true;
    }
  }

  if(deadPathElimAnalysisDebugLevel>=1) dbg << "DeadPathElimPartEdge::meetUpdate() final="<<str()<<endl;
      
  return modified;
}

// Set this Lattice object to represent the set of all possible execution prefixes.
// Return true if this causes the object to change and false otherwise.
bool DeadPathElimPartEdge::setToFull()
{
  bool modified = level!=live;
  level = live;
  return modified;
}

// Set this Lattice object to represent the of no execution prefixes (empty set)
// Return true if this causes the object to change and false otherwise.
bool DeadPathElimPartEdge::setToEmpty()
{
  bool modified = level!=bottom;
  level = bottom;
  return modified;
}

// Set all the information associated Lattice object with this MemLocObjectPtr to full.
// Return true if this causes the object to change and false otherwise.
bool DeadPathElimPartEdge::setMLValueToFull(MemLocObjectPtr ml)
{
  // Do nothing since this object does not contain information about MemLocObjects
  return false;
}

// Set this Lattice object to represent a dead part
// Return true if this causes the object to change and false otherwise.
bool DeadPathElimPartEdge::setToDead()
{
  bool modified = level!=bottom;
  level = dead;
  return modified;
}

// Returns whether this lattice denotes the set of all possible execution prefixes.
bool DeadPathElimPartEdge::isFull()
{ return level == live; }

// Returns whether this lattice denotes the empty set.
bool DeadPathElimPartEdge::isEmpty()
{ return level == bottom; }

// Returns whether this lattice denotes the set of all possible execution prefixes.
bool DeadPathElimPartEdge::isFull(PartEdgePtr pedge)
{ return level == live; }

// Returns whether this lattice denotes the empty set.
bool DeadPathElimPartEdge::isEmpty(PartEdgePtr pedge)
{ return level == bottom; }

/*TO DO LIST
----------
- extend ValueObjectPtr to provide the cardinality of the set, a way to enumerate it if finite
- update stx_analysis.C::isLive to use Method 3, using the above API*/

/********************************
 ***** DeadPathElimAnalysis *****
 ********************************/

boost::shared_ptr<DFTransferVisitor> DeadPathElimAnalysis::getTransferVisitor(PartPtr part, 
                                   CFGNode cn, NodeState& state, std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo)
{ 
  return boost::shared_ptr<DeadPathElimTransfer>(new DeadPathElimTransfer(part, cn, state, dfInfo, this));
}

/********************************
 ***** DeadPathElimTransfer *****
 ********************************/

DeadPathElimTransfer::DeadPathElimTransfer(PartPtr part, CFGNode cn, NodeState &s,  
                                           std::map<PartEdgePtr, std::vector<Lattice*> > &dfInfo, DeadPathElimAnalysis* dpea)
   : DFTransferVisitor(part, cn, s, dfInfo),
     dpea(dpea),
     part(part),
     cn(cn),
     modified(false)
{ }

bool DeadPathElimTransfer::finish() { 
  return modified;
  
  // !!! WHY AREN'T WE DEALLOCATING DeadPathElimPartEdge* dfEdge = dynamic_cast<DeadPathElimPartEdge*>(*dfInfo[NULLPartEdge].begin());????
}

// General function for SgNodes with 2 outgoing edges, where the first edge must/may be taken when some value (provided)
// is definitely true and the second edge must/may taken when this value is false.
// trueBranchMayMust - set to may/must if the true branch is taken when the value may/must be true
// falseBranchMayMust - set to may/must if the false branch is taken when the value may/must be false
void DeadPathElimTransfer::visit2OutNode(SgNode* sgn, ValueObjectPtr val, maymust trueBranchMayMust, maymust falseBranchMayMust) {
  scope s("visit2OutNode", scope::medium, deadPathElimAnalysisDebugLevel, 1);
  // If the conditional has a concrete value, replace the NULL-keyed dfInfo with two copies of the lattice for each
  // successor, one of which is live and the other dead
  dbg << "val="<<val->str()<<", val->isConcrete()="<<val->isConcrete()<<endl;
  if(val->isConcrete()) {
    set<boost::shared_ptr<SgValueExp> > concreteVals = val->getConcreteValue();
    dbg << "#concreteVals="<<concreteVals.size()<<endl;
    
    // If there is just one option and it is interpretable as a boolean
    if(concreteVals.size()==1 && ValueObject::isValueBoolCompatible(*concreteVals.begin())) {
      boost::shared_ptr<SgValueExp> concreteVal = *concreteVals.begin();
    
      // Get the edge that is propagated along the incoming dataflow path
      DeadPathElimPartEdge* dfEdge = dynamic_cast<DeadPathElimPartEdge*>(*dfInfo[NULLPartEdge].begin());
      // Adjust the base Edge so that it now starts at its original target part and terminates at NULL
      // (i.e. advance it forward by one node without specifying the target yet)
      dfEdge->src = dfEdge->tgt;
      dfEdge->tgt = NULLPart;
      dfEdge->clearPred2Val(); // Reset its predicate values

      // Record the lattice value of the incoming edge
      DPELevel dfLevel = dfEdge->level;

      // Empty out dfInfo in preparation of it being overwritten
      dfInfo.clear();

      // The concrete value inside val
      bool IfPredValue = ValueObject::SgValue2Bool(concreteVal);

      // Consider all the source part's outgoing edges (implemented by a server analysis)
      std::list<PartEdgePtr> edges = part->outEdges();
      if(deadPathElimAnalysisDebugLevel>=1) dbg << "IfPredValue="<<IfPredValue<<" edges.size()="<<edges.size()<<endl;
      assert(edges.size()==1 || edges.size()==2);
      for(std::list<PartEdgePtr>::iterator e=edges.begin(); e!=edges.end(); e++) {
        std::map<CFGNode, boost::shared_ptr<SgValueExp> > pv = (*e)->getPredicateValue();
        if(deadPathElimAnalysisDebugLevel>=1) {
          dbg << "e="<<(*e)->str()<<endl;
          dbg << "cn="<<CFGNode2Str(cn)<<" pv="<<endl;
          for(map<CFGNode, boost::shared_ptr<SgValueExp> >::iterator v=pv.begin(); v!=pv.end(); v++)
          { indent ind; dbg << CFGNode2Str(v->first) << "("<<(v->first==cn)<<"|"<<(v->first.getNode()==cn.getNode())<<") =&gt; "<<SgNode2Str(v->second.get())<<endl; }
        }

        assert(pv.find(cn) != pv.end());
        assert(ValueObject::isValueBoolCompatible(pv[cn]));

        // Create a DeadPathElimPartEdge to wrap this server analysis-implemented edge
        DeadPathElimPartEdge* dpeEdge;
        // If this is the first edge to synthesize, make the dfEdge into true branch DeadPathElimPartEdge
        if(dfInfo.size()==0) dpeEdge = dfEdge;
        else                 dpeEdge = dynamic_cast<DeadPathElimPartEdge*>(dfEdge->copy());
        assert(dfEdge);

        // If the current edge corresponds to the true branch
        if(ValueObject::SgValue2Bool(pv[cn])) {
          // Set the level of the true edge to live/dead if the outcome of this conditional is true/false 
          // and the incoming edge was live
          if(IfPredValue==true && trueBranchMayMust==must) 
            dpeEdge->level = (dfLevel==live? live: dfLevel);
          else
            dpeEdge->level = (dfLevel==live? dead: dfLevel);

          // Add the true predicate mapping to this edge
          dpeEdge->mapPred2Val(cn, boost::shared_ptr<SgValueExp>(SageBuilder::buildBoolValExp(true)));

          if(deadPathElimAnalysisDebugLevel>=1) dbg << "True Edge="<<dpeEdge->str()<<endl;
        // Else, if the current edge corresponds to the false branch
        } else {
          // Set the level of the true edge to live/dead if the outcome of this conditional is true/false 
          // and the incoming edge was live
          if(IfPredValue==false && falseBranchMayMust==must)
            dpeEdge->level = (dfLevel==live? live: dfLevel);
          else
            dpeEdge->level = (dfLevel==live? dead: dfLevel);

          // Add the false predicate mapping to this edge
          dpeEdge->mapPred2Val(cn, boost::shared_ptr<SgValueExp>(SageBuilder::buildBoolValExp(false)));

          if(deadPathElimAnalysisDebugLevel>=1) dbg << "False Edge="<<dpeEdge->str()<<endl;
        }

        // Set this dpeEdge's target to be the same as the target of the current server edge but using the edge's level
        dpeEdge->tgt = makePtr<DeadPathElimPart>((*e)->target(), dpea);

        // Set this dpeEdges's baseEdge to be the current edge using both Lattice API (setPartEdge) and Part API (setParent)
        dpeEdge->setPartEdge(*e);
        dpeEdge->setParent(*e);

        // Add the current DeadPathElimPartEdge to dfInfo
        vector<Lattice *> dfLatVec; 
        dfLatVec.push_back(dpeEdge);
        dfInfo[*e] = dfLatVec;
      }

      modified = true;
    } else
      visit((SgNode*)sgn);
  } else
    visit((SgNode*)sgn);
}

void DeadPathElimTransfer::visit(SgIfStmt *sgn)
{
  if(deadPathElimAnalysisDebugLevel>=1) dbg << "DeadPathElimTransfer::visit(SgIfStmt), conditional="<<SgNode2Str(sgn->get_conditional())<<" isSgExprStmt="<<isSgExprStatement(sgn->get_conditional())<<endl;
  if(SgExprStatement* es=isSgExprStatement(sgn->get_conditional())) {
    indent ind;
    // Get the value of the predicate test in the SgIfStmt's conditional
    ValueObjectPtr val = dpea->getComposer()->OperandExpr2Val(sgn, es->get_expression(), part->inEdgeFromAny(), dpea);
    visit2OutNode(sgn, val, must, must);
  } else {
    visit((SgNode*)sgn);
    assert(0);
  }
}

void DeadPathElimTransfer::visit(SgAndOp *op)
{
  if(deadPathElimAnalysisDebugLevel>=1) dbg << "DeadPathElimTransfer::visit(SgAndOp), op="<<SgNode2Str(op)<<endl;
  // If this is the portion of the short-circuit operation after the first argument was evaluated but before
  // the second argument
  if(cn.getIndex()==1) {
    ValueObjectPtr val = dpea->getComposer()->OperandExpr2Val(op, op->get_lhs_operand(), part->inEdgeFromAny(), dpea);
    visit2OutNode(op, val, may, must);
  } else {
    visit((SgNode*)op);
  }
}

void DeadPathElimTransfer::visit(SgOrOp *op)
{
  if(deadPathElimAnalysisDebugLevel>=1) dbg << "DeadPathElimTransfer::visit(SgOrOp), op="<<SgNode2Str(op)<<endl;
  // If this is the portion of the short-circuit operation after the first argument was evaluated but before
  // the second argument
  if(cn.getIndex()==1) {
    ValueObjectPtr val = dpea->getComposer()->OperandExpr2Val(op, op->get_lhs_operand(), part->inEdgeFromAny(), dpea);
    visit2OutNode(op, val, must, may);
  } else {
    visit((SgNode*)op);
  }
}

void DeadPathElimTransfer::visit(SgNode *sgn)
{
  // Get the edge that is propagated along the incoming dataflow path
  DeadPathElimPartEdge* dfEdge = dynamic_cast<DeadPathElimPartEdge*>(*dfInfo[NULLPartEdge].begin());
  // Adjust the base Edge so that it now starts at its original target part and terminates at NULL
  // (i.e. advance it forward by one node without specifying the target yet)
  dfEdge->src = dfEdge->tgt;
  dfEdge->tgt = NULLPart;
  dfEdge->clearPred2Val();
  
  // Consider all the source part's outgoing edges (implemented by a server analysis)
  std::list<PartEdgePtr> baseEdges = part->outEdges();
  
  // If this abstract state has no outgoing edges, return without modifying dfInfo. This is because
  // the composer may wish to do something with the resulting Lattice (e.g. copy it on a wildcard outgoing edge) 
  // even if its information content has not changed.
  if(baseEdges.size()==0) { modified=true; return; }
  
  // Empty out dfInfo in preparation for it being overwritten
  dfInfo.clear();

  for(std::list<PartEdgePtr>::iterator e=baseEdges.begin(); e!=baseEdges.end(); e++) {
    // Create a DeadPathElimPartEdge to this server analysis-implemented edge
    DeadPathElimPartEdge* dpeEdge;
    // If this is the first edge to synthesize, make the dfEdge into true branch DeadPathElimPartEdge
    if(dfInfo.size()==0) dpeEdge = dfEdge;
    else                 dpeEdge = dynamic_cast<DeadPathElimPartEdge*>(dfEdge->copy());
    assert(dpeEdge);
    
    // Set this dpeEdge's target to be the same as the current server edge's target but with the dfEdge's level
    dpeEdge->tgt = makePtr<DeadPathElimPart>((*e)->target(), dpea);
    
    // Set this dpeEdges's baseEdge to be the current edge using both Lattice API (setPartEdge) and PartEdge API (setParent)
    dpeEdge->setPartEdge(*e);
    dpeEdge->setParent(*e);
    
    vector<Lattice *> dfLatVec; 
    dfLatVec.push_back(dpeEdge);
    dfInfo[*e] = dfLatVec;
  }
}

DeadPathElimAnalysis::DeadPathElimAnalysis()
{}

// Initializes the state of analysis lattices at the given function, part and edge into our out of the part
// by setting initLattices to refer to freshly-allocated Lattice objects.
void DeadPathElimAnalysis::genInitLattice(PartPtr part, PartEdgePtr pedge, 
                                          std::vector<Lattice*>& initLattices)
{
  DeadPathElimPartEdge* newPartEdge = new DeadPathElimPartEdge(pedge, this, bottom);
  
  /*dbg << "DeadPathElimAnalysis::genInitLattice()"<<endl;
  indent ind;
  dbg << "part="<<part->str()<<endl;*/
  
  // If this an the entry node of this function, set newPart to live
  set<PartPtr> startParts = getComposer()->GetStartAStates(this);
  if(startParts.find(part) != startParts.end()) {
    newPartEdge->setToFull();
  }
  if(deadPathElimAnalysisDebugLevel>=2) dbg << "genInitLattice() newPartEdge="<<newPartEdge->str()<<endl;
  initLattices.push_back(newPartEdge);
}

bool DeadPathElimAnalysis::transfer(PartPtr part, CFGNode cn, NodeState& state, 
                                    std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo)
{
  assert(0); 
  return false;
}

/*// Calls composer->Expr2Val() on the base edge of pedge
ValueObjectPtr   DeadPathElimAnalysis::Expr2Val    (SgNode* n, PartEdgePtr pedge)
{
  DeadPathElimPartEdgePtr dpeEdge = dynamicPtrCast<DeadPathElimPartEdge>(pedge);
  assert(dpeEdge);
  return getComposer()->Expr2Val(n, dpeEdge->getPartEdge(), this);
}

// Calls composer->Expr2CodeLoc() on the base edge of pedge
MemLocObjectPtr  DeadPathElimAnalysis::Expr2MemLoc (SgNode* n, PartEdgePtr pedge)
{
  DeadPathElimPartEdgePtr dpeEdge = dynamicPtrCast<DeadPathElimPartEdge>(pedge);
  assert(dpeEdge);
  // MemLocObjectPtrPair p = getComposer()->Expr2MemLoc(n, dpeEdge->getPartEdge(), this);
  // return (p.mem ? p.mem : p.expr);
  MemLocObjectPtr p = getComposer()->Expr2MemLoc(n, dpeEdge->getPartEdge(), this);
  return p;
}

// Calls composer->Expr2CodeLoc() on the base edge of pedge
CodeLocObjectPtr DeadPathElimAnalysis::Expr2CodeLoc(SgNode* n, PartEdgePtr pedge)
{
  DeadPathElimPartEdgePtr dpeEdge = dynamicPtrCast<DeadPathElimPartEdge>(pedge);
  assert(dpeEdge);
  CodeLocObjectPtrPair p = getComposer()->Expr2CodeLoc(n, dpeEdge->getPartEdge(), this);
  return (p.mem ? p.mem : p.expr);
}

// Returns whether the given AbstractObject is live at the given part edge
bool DeadPathElimAnalysis::isLiveVal    (ValueObjectPtr val,  PartEdgePtr pedge)
{
  DeadPathElimPartEdgePtr dpeEdge = dynamicPtrCast<DeadPathElimPartEdge>(pedge);
  assert(dpeEdge);
  return getComposer()->isLiveVal(val, dpeEdge->getPartEdge(), this);
}

bool DeadPathElimAnalysis::isLiveMemLoc (MemLocObjectPtr ml,  PartEdgePtr pedge)
{
  DeadPathElimPartEdgePtr dpeEdge = dynamicPtrCast<DeadPathElimPartEdge>(pedge);
  assert(dpeEdge);
  return getComposer()->isLiveMemLoc(ml, dpeEdge->getPartEdge(), this);
}

bool DeadPathElimAnalysis::isLiveCodeLoc(CodeLocObjectPtr cl, PartEdgePtr pedge)
{
  DeadPathElimPartEdgePtr dpeEdge = dynamicPtrCast<DeadPathElimPartEdge>(pedge);
  assert(dpeEdge);
  return getComposer()->isLiveCodeLoc(cl, dpeEdge->getPartEdge(), this);
}*/

// Return the anchor Parts of the application
set<PartPtr> DeadPathElimAnalysis::GetStartAStates_Spec()
{
  set<PartPtr> startDPEParts;
  set<PartPtr> baseStartParts = getComposer()->GetStartAStates(this);
  for(set<PartPtr>::iterator baseSPart=baseStartParts.begin(); baseSPart!=baseStartParts.end(); baseSPart++) {
    NodeState* startState = NodeState::getNodeState(this, *baseSPart);
    if(deadPathElimAnalysisDebugLevel>=2) {
      dbg << "startPart = "<<baseSPart->get()->str()<<endl;
      dbg << "startState = "<<startState->str(this)<<endl;
    }

    DeadPathElimPartEdge* startDPEPartEdge = dynamic_cast<DeadPathElimPartEdge*>(startState->getLatticeAbove(this, 0));
    assert(startDPEPartEdge);
    startDPEParts.insert(startDPEPartEdge->target());
  }
  return startDPEParts;
  // Cache startDPEParts result
//  return startDPEPartEdge->target();
/*  DeadPathElimPartPtr startDPEPartCopy = initPtr(new DeadPathElimPart(startDPEPartEdge->target()));
  dbg << "startDPEPartCopy = "<<startDPEPartCopy->str()<<endl;
  list<PartEdgePtr> dpeOutEdges = startDPEPartCopy->outEdges();
  dbg << "dpeOutEdges="<<endl;
  { indent ind;
    for(list<PartEdgePtr>::iterator e=dpeOutEdges.begin(); e!=dpeOutEdges.end(); e++) {
      dbg << (*e)->str()<<endl;
    } }
  
  return startDPEPartCopy;*/
}

set<PartPtr> DeadPathElimAnalysis::GetEndAStates_Spec()
{
  set<PartPtr> endParts = getComposer()->GetEndAStates(this);
  set<PartPtr> endDPEParts;
  for(set<PartPtr>::iterator baseEPart=endParts.begin(); baseEPart!=endParts.end(); baseEPart++) {
    NodeState* endState = NodeState::getNodeState(this, *baseEPart);
    if(deadPathElimAnalysisDebugLevel>=2) {
      dbg << "endPart = "<<baseEPart->get()->str()<<endl;
      dbg << "endState = "<<endState->str(this)<<endl;
    }
    DeadPathElimPartEdge* endDPEPartEdge = dynamic_cast<DeadPathElimPartEdge*>(endState->getLatticeAbove(this, 0));
    assert(endDPEPartEdge);
    
    endDPEParts.insert(endDPEPartEdge->target());
    //return initPtr(new DeadPathElimPart(*endDPEPart));
  }
  return endDPEParts;
}

// Given a PartEdge pedge implemented by this ComposedAnalysis, returns the part from its predecessor
// from which pedge was derived. This function caches the results if possible.
/*PartEdgePtr DeadPathElimAnalysis::convertPEdge_Spec(PartEdgePtr pedge)
{
  DeadPathElimPartEdgePtr dpeEdge = dynamicPtrCast<DeadPathElimPartEdge>(pedge);
  assert(dpeEdge);
  return dpeEdge->getPartEdge();
}*/

}; // namespace fuse
