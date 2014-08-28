#include "sage3basic.h"
#include "call_context_sensitivity_analysis.h"

using namespace std;
using namespace dbglog;
namespace fuse
{

int callContextSensitivityDebugLevel=0;

/* ###########################
   ##### CallPartContext #####
   ########################### */

// Returns the current depth of the context stack
unsigned int CallPartContext::getCtxtStackDepth() const
{ return stack.size(); }

// Push the given Part onto the context stack
void CallPartContext::push(PartPtr part) {
  stack.push_back(part);
}

// Pop the given Part off the context stack
void CallPartContext::pop() {
  stack.pop_back();
}

// Returns the last (most recent) element on the stack
PartPtr CallPartContext::last() const {
  return stack.back();
}

// Returns the number of elements on the stack
int CallPartContext::size() const {
  return stack.size();
}

// Returns a list of PartContextPtr objects that denote more detailed context information about
// this PartContext's internal contexts. 
list<PartContextPtr> CallPartContext::getSubPartContexts() const {
  list<PartContextPtr> subContexts;
  for(list<PartPtr>::const_iterator i=stack.begin(); i!=stack.end(); i++) {
    list<PartPtr> curL;
    curL.push_back(*i);
    subContexts.push_back(makePtr<CallPartContext>(curL));
  }
  return subContexts;
}

bool CallPartContext::operator==(const PartContextPtr& that_arg) const {
  const CallPartContextPtr that = dynamicConstPtrCast<CallPartContext>(that_arg);
  assert(that.get());
  
  return *this == *that.get();
}

bool CallPartContext::operator<(const PartContextPtr& that_arg) const {
  const CallPartContextPtr that = dynamicConstPtrCast<CallPartContext>(that_arg);
  assert(that.get());

  return *this < *that.get();
}

bool CallPartContext::operator==(const PartContext& that_arg) const {
  //const CallPartContextPtr that = dynamicConstPtrCast<CallPartContext>(that_arg);
  //assert(that.get());
  const CallPartContext& that = dynamic_cast<const CallPartContext&>(that_arg);
  //dbg << "CallContextPart::== #stack="<<stack.size()<<" #that->stack="<<that->stack.size()<<endl;
  if(stack.size()!=that.stack.size()) return false;
  
  /*list<PartPtr>::const_iterator thisI=stack.begin(), thatI=that->stack.begin();
  for(; thisI!=stack.end(); thisI++, thatI++)
    dbg << "    =="<<(*thisI==*thatI)<<" thisI="<<thisI->get()->str()<<" thatI="<<thatI->get()->str()<<endl;*/
  return stack == that.stack;
}

bool CallPartContext::operator<(const PartContext& that_arg) const {
  //const CallPartContextPtr that = dynamicConstPtrCast<CallPartContext>(that_arg);
  //assert(that.get());
  const CallPartContext& that = dynamic_cast<const CallPartContext&>(that_arg);

  //dbg << "CallContextPart::< #stack="<<stack.size()<<" #that->stack="<<that->stack.size()<<endl;
  if(stack.size()<that.stack.size()) return true;
  if(stack.size()>that.stack.size()) return false;
  
  /*list<PartPtr>::const_iterator thisI=stack.begin(), thatI=that->stack.begin();
  for(; thisI!=stack.end(); thisI++, thatI++) {
    dbg << "    <"<<(*thisI<*thatI)<<" thisI="<<thisI->get()->str()<<" thatI="<<thatI->get()->str()<<endl;
    dbg << "    =="<<(*thisI==*thatI)<<" thisI="<<thisI->get()->str()<<" thatI="<<thatI->get()->str()<<endl;
  }*/
  //return stack < that->stack;
  
  // Lexicographic comparison
  list<PartPtr>::const_iterator thisI = stack.begin(),
                                thatI = that.stack.begin();
  for(; thisI!=stack.end() && thatI!=that.stack.end(); thisI++, thatI++) {
    if(*thisI < *thatI) return true;
    if(*thisI > *thatI) return false;
  }
  
  // The contexts are equal
  return false;
}

// Returns true if the set of sub-executions denoted by this context is a non-strict subset of the set of 
// sub-executions denoted by that context.
//bool CallPartContext::isSubsetEq(CallPartContextPtr that) const {
bool CallPartContext::isSubsetEq(const CallPartContext& that) const {
  list<PartPtr>::const_iterator itThis=stack.begin(),
                                itThat=that.stack.begin();
  for(; itThis!=stack.end() && itThat!=that.stack.end(); itThis++, itThat++) {
    // If the stacks are not equal at any level, then the sets they denote do not overlap
    if(*itThis != *itThat) return false;
  }
  
  // If we reached the end of both context lists without detecting a disagreement then the contexts are equal
  if(itThis==stack.end() && itThat==that.stack.end()) return true;
  
  // If we reached the end of that before we reached the end of this, that.stack must be less constraining
  // than this->stack and thus, the set of sub-executions denoted by that.stack must be a superset of
  // the set denoted by this->stack
  if(itThat==that.stack.end()) return true;
  // Otherwise, the inverse containment relationship holds and since we know that this->stack!=that.stack,
  // this->stack must not be a subset of that.stack
  else                           return false;
}

// Returns true if the sets of sub-executions denoted by this and that contexts overlap.
//bool CallPartContext::setOverlap(CallPartContextPtr that) const {
bool CallPartContext::setOverlap(const CallPartContext& that) const {
  list<PartPtr>::const_iterator itThis=stack.begin(),
                                itThat=that.stack.begin();
  for(; itThis!=stack.end() && itThat!=that.stack.end(); itThis++, itThat++) {
    // If the stacks are not equal at any level, then the sets they denote do not overlap
    if(*itThis != *itThat) return false;
  }
  
  // If we reached the end of both context stacks without detecting a disagreement then either
  // - The contexts are equal
  // - One of the contexts is less constraining than the other and thus, the set of sub-executions
  //   denoted by the less constraining context contains the one denoted by the more constraining
  //   context.
  // Either way, the sets denoted by the two contexts overlap
  return true;
}

// Computes the meet of this and that context and saves the result in this.
// Returns true if this causes this to change and false otherwise.
//bool CallPartContext::meetUpdate(CallPartContextPtr that, PartEdgePtr pedge)
bool CallPartContext::meetUpdate(const CallPartContext& that, PartEdgePtr pedge)
{
  // Sets this object to the common prefix of this->stack and that.stack, which is the tightest
  // context that denotes a set that contains the sets denoted by this->stack and that.stack.
  list<PartPtr>::iterator itThis=stack.begin();
  list<PartPtr>::const_iterator itThat=that.stack.begin();
  for(; itThis!=stack.end() && itThat!=that.stack.end(); itThis++, itThat++) {
    // If the stacks are not equal at some level, we must remove from this->stack everything
    // at or after this level
    if(*itThis != *itThat) {
      stack.erase(itThis, stack.end());
      // We've modified this->stack
      return true;
    }
  }
  
  // If that.stack is a sub-sequence of this->stack, remove everything from this->stack that
  // does not exist in that.stack
  if(itThis!=stack.end() && itThat==that.stack.end()) {
    stack.erase(itThis, stack.end());
    // We've modified this->stack
    return true;
  // this->stack is either equal to that.stack or is a sub-sequence of it, meaning that it is
  // already equal to the meet of this and that
  } else {
    return false;
  }    
}

// Returns whether this context denotes the set of all possible sub-execution prefixes.
bool CallPartContext::isFull(PartEdgePtr pedge) {
  // The empty calling stack denotes no constraints and thus, the set of all sub-executions
  return stack.size()==0;
}

// Returns whether this context denotes the empty set of sub-execution prefixes
bool CallPartContext::isEmpty(PartEdgePtr pedge) {
  // We can never be sure whether the set of constraints in this context is self-contradictory
  return false;
}

// Set this object to represent the set of all possible sub-executions.
// Return true if this causes the object to change and false otherwise.
bool CallPartContext::setToFull() {
  bool modified = stack.size()==0;
  stack.clear();
  return modified;
}

std::string CallPartContext::str(std::string indent) {
  std::ostringstream oss;
  oss << "[CallPartContext: ";
  for(list<PartPtr>::iterator i=stack.begin(); i!=stack.end(); i++) {
    if(i!=stack.begin()) oss << endl << indent;
    oss << (*i)->str(indent+"    ");
  }
  oss << "]";
  return oss.str();
}

/* ###########################
   ##### CallCtxSensPart #####
   ########################### */

// Creates a CallCtxSensPart from the given base, using no calling context
CallCtxSensPart::CallCtxSensPart(PartPtr base, ComposedAnalysis* analysis) :
  Part(analysis, base), recursive(false)
{
  ccsa = dynamic_cast<CallContextSensitivityAnalysis*>(analysis);
  assert(ccsa);
}

// Creates a CallCtxSensPart from the given base, using the given context information
CallCtxSensPart::CallCtxSensPart(PartPtr base, const CallPartContext& context, const Function& lastCtxtFunc, bool recursive, ComposedAnalysis* analysis) :
  Part(analysis, base), context(context), recursive(recursive), lastCtxtFunc(lastCtxtFunc)
{
  ccsa = dynamic_cast<CallContextSensitivityAnalysis*>(analysis);
  assert(ccsa);
}

// Creates a CallCtxSensPart from the given base, using the context information from contextPart
CallCtxSensPart::CallCtxSensPart(PartPtr base, CallCtxSensPartPtr contextPart, ComposedAnalysis* analysis) :
  Part(analysis, base), context(contextPart->context), recursive(contextPart->recursive), 
                        lastCtxtFunc(contextPart->lastCtxtFunc), ccsa(contextPart->ccsa)
{}

CallCtxSensPart::CallCtxSensPart(PartPtr base, const CallCtxSensPart& contextPart, ComposedAnalysis* analysis) :
  Part(analysis, base), context(contextPart.context), recursive(contextPart.recursive), 
                        lastCtxtFunc(contextPart.lastCtxtFunc), ccsa(contextPart.ccsa)
{}

CallCtxSensPart::CallCtxSensPart(const CallCtxSensPart& that): 
  Part((const Part&)that), context(that.context), recursive(that.recursive), 
                           lastCtxtFunc(that.lastCtxtFunc), ccsa(that.ccsa)
{}

// Returns a shared pointer to this of type CallCtxSensPartPtr;
CallCtxSensPartPtr CallCtxSensPart::get_shared_this()
{ return dynamicPtrCast<CallCtxSensPart>(makePtrFromThis(shared_from_this())); }

std::list<PartEdgePtr> CallCtxSensPart::outEdges()
{
  scope reg(txt() << "CallCtxSensPart::outEdges() part="<<str(), scope::medium, callContextSensitivityDebugLevel, 2);
  // For now we'll only consider Parts with a single CFGNode
  assert(CFGNodes().size()==1);
  
  list<PartEdgePtr> baseEdges = getParent()->outEdges();
  list<PartEdgePtr> ccsEdges;
  
  // The NodeState at the current part
  NodeState* outState = NodeState::getNodeState(analysis, getParent());
  if(callContextSensitivityDebugLevel>=2) dbg << "outState="<<outState->str(analysis)<<endl;
  
  // Consider all the CallCtxSensPartEdges along all of this part's outgoing edges. Since this is a forward
  // analysis, they are maintained separately
  for(list<PartEdgePtr>::iterator be=baseEdges.begin(); be!=baseEdges.end(); be++) {
    //if(callContextSensitivityDebugLevel>=1) dbg << "be="<<be->str()<<endl;
    CallCtxSensLattice* lat = dynamic_cast<CallCtxSensLattice*>(outState->getLatticeBelow(analysis, *be, 0));
    assert(lat);
    scope(txt()<<"be="<<be->str(), scope::medium, callContextSensitivityDebugLevel, 2);
    if(callContextSensitivityDebugLevel>=2) dbg << "lat="<<lat->str()<<endl;
  
    // Create CallCtxSensPartEdges for all the outgoing src->tgt CallCtxSensPart pairs in lat
    for(set<CallCtxSensPartPtr>::iterator i=lat->outgoing[get_shared_this()].begin(); i!=lat->outgoing[get_shared_this()].end(); i++)
      ccsEdges.push_back(makePtr<CallCtxSensPartEdge>(*be, get_shared_this(), *i, ccsa));
  }
  
  dbg << "#ccsEdges="<<ccsEdges.size()<<endl;
  return ccsEdges;
}

std::list<PartEdgePtr> CallCtxSensPart::inEdges() {
  scope reg(txt() << "CallCtxSensPart::inEdges() part="<<str(), scope::medium, callContextSensitivityDebugLevel, 2);
  
  // For now we'll only consider Parts with a single CFGNode
  assert(CFGNodes().size()==1);
  
  list<PartEdgePtr> baseEdges = getParent()->inEdges();
  list<PartEdgePtr> ccsEdges;
  
  // Consider all the CallCtxSensPartEdges along all of this part's outgoing edges. Since this is a forward
  // analysis, they are maintained separately
  for(list<PartEdgePtr>::iterator be=baseEdges.begin(); be!=baseEdges.end(); be++) {
    // The NodeState at the current predecessor
    NodeState* inState = NodeState::getNodeState(analysis, (*be)->source());
    if(callContextSensitivityDebugLevel>=2) {
      dbg << "be="<<be->str()<<endl;
      dbg << "inState="<<inState->str(analysis)<<endl;
    }
    
    CallCtxSensLattice* lat = dynamic_cast<CallCtxSensLattice*>(inState->getLatticeBelow(analysis, *be, 0));
    assert(lat);
    //if(callContextSensitivityDebugLevel>=1) dbg << "lat="<<lat->str()<<endl;
  
    // Create CallCtxSensPartEdges for all the incoming  src->tgt CallCtxSensPart pairs in lat
    for(set<CallCtxSensPartPtr>::iterator i=lat->incoming[get_shared_this()].begin(); i!=lat->incoming[get_shared_this()].end(); i++)
      ccsEdges.push_back(makePtr<CallCtxSensPartEdge>(*be, *i, get_shared_this(), ccsa));
  }
  
  return ccsEdges;
}

std::set<CFGNode> CallCtxSensPart::CFGNodes() const
{ return getParent()->CFGNodes(); }

// If this Part corresponds to a function call/return, returns the set of Parts that contain
// its corresponding return/call, respectively.
set<PartPtr> CallCtxSensPart::matchingCallParts() const {
  set<PartPtr> matchParts;
  
  // Wrap the parts returned by the call to the parent Part with CallCtxSensParts
  set<PartPtr> parentMatchParts = getParent()->matchingCallParts();
  for(set<PartPtr>::iterator mp=parentMatchParts.begin(); mp!=parentMatchParts.end(); mp++) {
    matchParts.insert(makePtr<CallCtxSensPart>(*mp, *this, analysis));
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
std::list<PartPtr> getOperandPart(SgNode* anchor, SgNode* operand);*/

// Returns a PartEdgePtr, where the source is a wild-card part (NULLPart) and the target is this Part
PartEdgePtr CallCtxSensPart::inEdgeFromAny()
{
  return makePtr<CallCtxSensPartEdge>(getParent()->inEdgeFromAny(), NULLPart, get_shared_this(), analysis);
}

// Returns a PartEdgePtr, where the target is a wild-card part (NULLPart) and the source is this Part
PartEdgePtr CallCtxSensPart::outEdgeToAny()
{
  return makePtr<CallCtxSensPartEdge>(getParent()->outEdgeToAny(), get_shared_this(), NULLPart, analysis);
}

// Returns the specific context of this Part. Can return the NULLPartContextPtr if this
// Part doesn't implement a non-trivial context.
PartContextPtr CallCtxSensPart::getPartContext() const
{
  return makePtr<CallPartContext>(context);
}

// Returns the specific context of this Part. Can return the NULLPartContextPtr if this
// Part doesn't implement a non-trivial context.
CallPartContextPtr CallCtxSensPart::getCCSPartContext() const
{
  return makePtr<CallPartContext>(context);
}

bool CallCtxSensPart::equal(const PartPtr& o) const {
  const CallCtxSensPartPtr that = dynamicConstPtrCast<CallCtxSensPart>(o);
  assert(that.get());
  assert(analysis == that->analysis);
  
  return getParent() == that->getParent() &&
         context == that->context;
}

bool CallCtxSensPart::less(const PartPtr& o)  const {
  const CallCtxSensPartPtr that = dynamicConstPtrCast<CallCtxSensPart>(o);
  assert(that.get());
  assert(analysis == that->analysis);
  
  if(getParent() < that->getParent()) return true;
  if(getParent() > that->getParent()) return false;
  return context < that->context;
}

// Pretty print for the object
std::string CallCtxSensPart::str(std::string indent) {
  ostringstream oss;
  
  oss << "[CallCtxSensPart: "<<getParent()->str(indent+"    ");
  oss << ", recursive="<<recursive<<", lastCtxtFunc="<<lastCtxtFunc.get_name().getString()<<"()";
  if(context.getCtxtStackDepth()>0)
    oss << endl << indent << "    context=" << context.str(indent+"        ");
  oss << "]";
  
  return oss.str();
}

/* ###############################
   ##### CallCtxSensPartEdge #####
   ############################### */

// Constructor to be used when constructing the edges (e.g. from genInitLattice()).
CallCtxSensPartEdge::CallCtxSensPartEdge(PartEdgePtr baseEdge, CallCtxSensPartPtr src, CallCtxSensPartPtr tgt, ComposedAnalysis* analysis) : 
        PartEdge(analysis, baseEdge), src(src), tgt(tgt)
{}

CallCtxSensPartEdge::CallCtxSensPartEdge(const CallCtxSensPartEdge& that) :
  PartEdge((const PartEdge&)that), 
  src(that.src), tgt(that.tgt)
{}

// Returns a shared pointer to this of type CallCtxSensPartEdgePtr
CallCtxSensPartEdgePtr CallCtxSensPartEdge::get_shared_this()
{ return dynamicPtrCast<CallCtxSensPartEdge>(makePtrFromThis(shared_from_this())); }

PartPtr CallCtxSensPartEdge::source() const
{ return src; }

PartPtr CallCtxSensPartEdge::target() const
{ return tgt; }

// Let A={ set of execution prefixes that terminate at the given anchor SgNode }
// Let O={ set of execution prefixes that terminate at anchor's operand SgNode }
// Since to reach a given SgNode an execution must first execute all of its operands it must
//    be true that there is a 1-1 mapping m() : O->A such that o in O is a prefix of m(o).
// This function is the inverse of m: given the anchor node and operand as well as the
//    PartEdge that denotes a subset of A (the function is called on this PartEdge), 
//    it returns a list of PartEdges that partition O.
// A default implementation that walks the server analysis-provided graph backwards to find 
//    matching PartEdges is provided.
std::list<PartEdgePtr> CallCtxSensPartEdge::getOperandPartEdge(SgNode* anchor, SgNode* operand) {
  scope reg("PartEdge::getOperandPartEdge()", scope::medium, callContextSensitivityDebugLevel, 2);
  if(callContextSensitivityDebugLevel>=2) {
    dbg << "anchor="<<SgNode2Str(anchor)<<" operand="<<SgNode2Str(operand)<<endl;
    dbg << "this PartEdge="<<str()<<endl;
  }
  
  std::list<PartEdgePtr> baseEdges = getParent()->getOperandPartEdge(anchor, operand);
  if(callContextSensitivityDebugLevel>=2) {
    scope regBE("baseOperandEdges", scope::medium, callContextSensitivityDebugLevel, 2);
    for(list<PartEdgePtr>::iterator be=baseEdges.begin(); be!=baseEdges.end(); be++)
      dbg << be->get()->str();
  }
  
  // Convert the list of edges into a set for easier/faster lookups
  list<PartEdgePtr> ccsEdges;
  for(list<PartEdgePtr>::iterator be=baseEdges.begin(); be!=baseEdges.end(); be++) {
    CallCtxSensPartPtr edgeSrc = makePtr<CallCtxSensPart>((*be)->source(), (src? src: tgt), analysis);
    { scope reg("edgeSrc", scope::low, callContextSensitivityDebugLevel, 2);
    if(callContextSensitivityDebugLevel>=2) dbg<<edgeSrc->str()<<endl; }
    
    CallCtxSensPartPtr edgeTgt = makePtr<CallCtxSensPart>((*be)->target(), (src? src: tgt), analysis);
    { scope reg("edgeTgt", scope::low, callContextSensitivityDebugLevel, 2);
    if(callContextSensitivityDebugLevel>=2) dbg<<edgeTgt->str()<<endl; }
    
    CallCtxSensPartEdgePtr ccsEdge = makePtr<CallCtxSensPartEdge>(*be, edgeSrc, edgeTgt, analysis);
    { scope reg("ccsEdge", scope::low, callContextSensitivityDebugLevel, 2);
    if(callContextSensitivityDebugLevel>=2) dbg<<ccsEdge->str()<<endl; }
    
    ccsEdges.push_back(ccsEdge);
  }
  
  return ccsEdges;
}

// If the source Part corresponds to a conditional of some sort (if, switch, while test, etc.)
// it must evaluate some predicate and depending on its value continue, execution along one of the
// outgoing edges. The value associated with each outgoing edge is fixed and known statically.
// getPredicateValue() returns the value associated with this particular edge. Since a single 
// Part may correspond to multiple CFGNodes getPredicateValue() returns a map from each CFG node
// within its source part that corresponds to a conditional to the value of its predicate along 
// this edge. 
std::map<CFGNode, boost::shared_ptr<SgValueExp> > CallCtxSensPartEdge::getPredicateValue()
{
  return getParent()->getPredicateValue();
}

bool CallCtxSensPartEdge::equal(const PartEdgePtr& o) const
{
  const CallCtxSensPartEdgePtr that = dynamicConstPtrCast<CallCtxSensPartEdge>(o);
  assert(that.get());
  return getParent()==that->getParent() &&
         src==that->src &&
         tgt==that->tgt;
}

bool CallCtxSensPartEdge::less(const PartEdgePtr& o)  const
{
  const CallCtxSensPartEdgePtr that = dynamicConstPtrCast<CallCtxSensPartEdge>(o);
  assert(that.get());

  return getParent() < that->getParent() ||
         (getParent()==that->getParent() && src < that->src) ||
         (getParent()==that->getParent() && src == that->src && tgt < that->tgt);
}

// Pretty print for the object
std::string CallCtxSensPartEdge::str(std::string indent)
{
  ostringstream oss;
  oss << "[CallCtxSensPartEdge: parent="<<getParent()->str(indent+"    ")<<endl;
  if(src) oss << indent << "    src="<<src->str(indent+"    ")<<endl;
  if(tgt) oss << indent << "    tgt="<<tgt->str(indent+"    ")<<"]";
  return oss.str();
}

/* #########################
   ##### CallCtxSensML #####
   ######################### */

CallCtxSensML::CallCtxSensML(SgNode* sgn, MemLocObjectPtr baseML, const CallPartContext& context, CallContextSensitivityAnalysis* ccsa) :
      MemLocObject(sgn), baseML(baseML), context(context), ccsa(ccsa) { }

CallCtxSensML::CallCtxSensML(const CallCtxSensML& that) : MemLocObject(that)
{
  baseML  = that.baseML;
  context = that.context;
  ccsa    = that.ccsa;
}
// pretty print
string CallCtxSensML::str(string indent){
  ostringstream oss;
  oss << "[CallCtxSensML: baseML="<< baseML->str(indent+"    ")<<endl;
  oss << indent << "    " << context.str(indent+"    ")<<"]";
  return oss.str();
}

// copy this object and return a pointer to it
MemLocObjectPtr CallCtxSensML::copyML() const 
{ return boost::make_shared<CallCtxSensML>(*this); }

bool CallCtxSensML::mayEqualML(MemLocObjectPtr that_arg, PartEdgePtr pedge_arg) {
  CallCtxSensMLPtr that = boost::dynamic_pointer_cast<CallCtxSensML>(that_arg);
  assert(that);
  CallCtxSensPartEdgePtr pedge = dynamicConstPtrCast<CallCtxSensPartEdge>(pedge_arg);
  assert(pedge);
  
  /*scope reg("CallCtxSensML::mayEqualML()", scope::low);
  dbg << "context="<<context.str()<<endl;
  dbg << "that->context="<<that->context.str()<<endl;
  dbg << "context.setOverlap(that->context)="<<context.setOverlap(that->context)<<endl;*/
  // If there exist sub-executions where this and that MemLocObjects may have existed together
  if(context.setOverlap(that->context))
    return ccsa->getComposer()->mayEqual(baseML, that->baseML, pedge->getParent(), ccsa);
  // Otherwise, they may not be equal
  else
    return false;
}

bool CallCtxSensML::mustEqualML(MemLocObjectPtr that_arg, PartEdgePtr pedge_arg) {
  CallCtxSensMLPtr that = boost::dynamic_pointer_cast<CallCtxSensML>(that_arg);
  assert(that);
  CallCtxSensPartEdgePtr pedge = dynamicConstPtrCast<CallCtxSensPartEdge>(pedge_arg);
  assert(pedge);
  
  // If there exist sub-executions where this and that MemLocObjects may have existed together
  //dbg << "CallCtxSensML::mustEqualML overlap="<<context.setOverlap(that->context)<<endl;
  if(context.setOverlap(that->context))
    return ccsa->getComposer()->mustEqual(baseML, that->baseML, pedge->getParent(), ccsa);
  // Otherwise, they are not must-equal
  else
    return false;
}

// Returns whether the two abstract objects denote the same set of concrete objects
bool CallCtxSensML::equalSet(AbstractObjectPtr that_arg, PartEdgePtr pedge_arg) {
  CallCtxSensMLPtr that = boost::dynamic_pointer_cast<CallCtxSensML>(that_arg);
  assert(that);
  CallCtxSensPartEdgePtr pedge = dynamicConstPtrCast<CallCtxSensPartEdge>(pedge_arg);
  assert(pedge);
  
  // If there exist sub-executions where this and that MemLocObjects may have existed together
  if(context.setOverlap(that->context))
    return ccsa->getComposer()->equalSet(baseML, that->baseML, pedge->getParent(), ccsa);
  // Otherwise, their sets must not be equal
  else
    return false;
}

// Returns whether this abstract object denotes a non-strict subset (the sets may be equal) of the set denoted
// by the given abstract object.
bool CallCtxSensML::subSet(AbstractObjectPtr that_arg, PartEdgePtr pedge_arg) {
  CallCtxSensMLPtr that = boost::dynamic_pointer_cast<CallCtxSensML>(that_arg);
  assert(that);
  CallCtxSensPartEdgePtr pedge = dynamicConstPtrCast<CallCtxSensPartEdge>(pedge_arg);
  assert(pedge);
  
  // If there exist sub-executions where this and that MemLocObjects may have existed together
  if(context.setOverlap(that->context))
    return ccsa->getComposer()->subSet(baseML, that->baseML, pedge->getParent(), ccsa);
  // Otherwise, this is not a subset of that
  else
    return false;
}

bool CallCtxSensML::isLiveML(PartEdgePtr pedge_arg) {
  CallCtxSensPartEdgePtr pedge = dynamicConstPtrCast<CallCtxSensPartEdge>(pedge_arg);
  assert(pedge);
  
  // MemLocs are only live at Parts where the context denotes a set of sub-executions that overlaps
  // with the set of sub-executions denoted by the MemLoc's context.
  // Note: we use either the source or the target context, whichever is available since 
  //       MemLocs can't be generated at context switch points, meaning that the contexts
  //       at each edge's source and destination must be identical.
  scope reg("CallCtxSensML::isLiveML()", scope::low);
  dbg << "context="<<context.str()<<endl;
  if(pedge->src) dbg << "pedge->src->context="<<pedge->src->context.str()<<endl;
  else           dbg << "pedge->tgt->context="<<pedge->tgt->context.str()<<endl;
  dbg << "overlap="<<context.setOverlap(pedge->src? pedge->src->context: pedge->tgt->context)<<endl;
  return context.setOverlap(pedge->src? pedge->src->context: pedge->tgt->context) &&
         ccsa->getComposer()->isLive(baseML, pedge->getParent(), ccsa);
}

// Computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool CallCtxSensML::meetUpdateML(MemLocObjectPtr that_arg, PartEdgePtr pedge_arg) {
  CallCtxSensMLPtr that = boost::dynamic_pointer_cast<CallCtxSensML>(that_arg);
  assert(that);
  CallCtxSensPartEdgePtr pedge = dynamicConstPtrCast<CallCtxSensPartEdge>(pedge_arg);
  assert(pedge);
  
  bool modified = false;
  modified = context.meetUpdate(that->context, pedge) || modified;
  modified = ccsa->getComposer()->meetUpdateMemLoc(baseML, that->baseML, pedge->getParent(), ccsa) || modified;
  return modified;
}

// Returns whether this AbstractObject denotes the set of all possible execution prefixes.
bool CallCtxSensML::isFull(PartEdgePtr pedge) {
  return context.isFull(pedge) && ccsa->getComposer()->isFull(baseML, pedge->getParent(), ccsa);
}

// Returns whether this AbstractObject denotes the empty set.
bool CallCtxSensML::isEmpty(PartEdgePtr pedge) {
  return context.isEmpty(pedge) || ccsa->getComposer()->isEmpty(baseML, pedge->getParent(), ccsa);
}

// Set this object to represent the set of all possible MemLocs
// Return true if this causes the object to change and false otherwise.
bool CallCtxSensML::setToFull() {
  bool modified = false; 
  modified = context.setToFull() || modified;
  assert(0);
  // WE CANNOT IMPLEMENT THIS BECAUSE SETTING baseML TO FULL WOULD CHANGE IT, WHICH WOULD BE BAD FOR
  // ALL OF ITS OTHER USERS. AS SUCH, WE NEED AbstractObjects TO IMPLEMENT A COPY FUNCTIONALITY.
  //modified = ccsa->getComposer()->setToFull(baseML, pedge->getParent(), ccsa) || modified;
  return modified;
}
// Set this Lattice object to represent the empty set of MemLocs.
// Return true if this causes the object to change and false otherwise.
bool CallCtxSensML::setToEmpty() {
  assert(0);
}

/* ##############################
   ##### CallCtxSensLattice #####
   ############################## */

CallCtxSensLattice::CallCtxSensLattice(PartEdgePtr baseEdge, ComposedAnalysis* analysis/*, const list<PartPtr>& context, bool recursive*/) : 
        Lattice(baseEdge), FiniteLattice(baseEdge)/*, context(context), recursive(recursive)*/
{}

CallCtxSensLattice::CallCtxSensLattice(const CallCtxSensLattice& that) :
  Lattice(that.latPEdge), 
  FiniteLattice(that.latPEdge), 
  /*context(that.context), recursive(that.recursive), */outgoing(that.outgoing), incoming(that.incoming)
{}

void CallCtxSensLattice::initialize() { 
  //context.clear();
  //recursive = false;
  outgoing.clear();
  incoming.clear();
}
  
// Returns a copy of this lattice
Lattice* CallCtxSensLattice::copy() const {
  return new CallCtxSensLattice(*this);
}


// Overwrites the state of "this" Lattice with "that" Lattice
void CallCtxSensLattice::copy(Lattice* that_arg) {
  Lattice::copy(that_arg);
  
  CallCtxSensLattice* that = dynamic_cast<CallCtxSensLattice*>(that_arg);
  assert(that);
  
  /*context   = that->context;
  recursive = that->recursive;*/
  outgoing  = that->outgoing;
  incoming  = that->incoming;
}

bool CallCtxSensLattice::operator==(Lattice* that_arg) /*const*/ {
  CallCtxSensLattice* that = dynamic_cast<CallCtxSensLattice*>(that_arg);
  assert(that);
  
  return latPEdge  == that->latPEdge  &&
         /*context   == that->context   &&
         recursive == that->recursive &&*/
         outgoing  == that->outgoing  &&
         incoming  == that->incoming;  
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
Lattice* CallCtxSensLattice::remapML(const std::set<MLMapping>& ml2ml, PartEdgePtr newPEdge)
{ return copy(); }

// Adds information about the MemLocObjects in newL to this Lattice, overwriting any information previously 
//    maintained in this lattice about them.
// Returns true if the Lattice state is modified and false otherwise.
bool CallCtxSensLattice::replaceML(Lattice* newL) {
  bool modified = (*this == newL);
  copy(newL);
  return modified;
}

// Updates the internal state of the key in thisMap associated with iterator iThis to incorporate
//    recursion information stored at iterator iThat. Update iThis to point to the key's location in 
//    thisMap after the update.
// Returns true if this causes thisMap to change and false otherwise.
bool CallCtxSensLattice::updateMapKey(
             std::map<CallCtxSensPartPtr, std::set<CallCtxSensPartPtr> >& thisMap, 
             std::map<CallCtxSensPartPtr, std::set<CallCtxSensPartPtr> >::iterator& iThis,
             std::map<CallCtxSensPartPtr, std::set<CallCtxSensPartPtr> >::iterator  iThat)
{
  // Incorporate information in this->outgoing key PartPtrs about any recursion that may have been discovered
  if((!iThis->first->recursive && iThat->first->recursive) ||
     (!iThis->first->lastCtxtFunc.isInitialized() && iThat->first->lastCtxtFunc.isInitialized())) {
    CallCtxSensPartPtr thisPart = iThis->first;
    set<CallCtxSensPartPtr> thisPartSet = iThis->second;
    thisMap.erase(thisPart);

    thisPart->recursive |= iThat->first->recursive;
    if(!thisPart->lastCtxtFunc.isInitialized()) 
      thisPart->lastCtxtFunc = thisPart->lastCtxtFunc;

    thisMap[thisPart] = thisPartSet;
    iThis=thisMap.find(thisPart);
    
    return true;
  }
  
  return false;
}

// Updates the internal state of the key in thisSet associated with iterator iThis to incorporate
//    recursion information stored at iterator iThat. Update iThis to point to the key's location in 
//    thisSet after the update.
// Returns true if this causes thisMap to change and false otherwise.
bool CallCtxSensLattice::updateSetElement(
             std::set<CallCtxSensPartPtr>& thisSet, 
             std::set<CallCtxSensPartPtr>::iterator& jThis,
             std::set<CallCtxSensPartPtr>::iterator  jThat)
{
  // Incorporate information about any recursion that may have been discovered
  if((!(*jThis)->recursive && (*jThat)->recursive) ||
     (!(*jThis)->lastCtxtFunc.isInitialized() && (*jThat)->lastCtxtFunc.isInitialized())) {
    CallCtxSensPartPtr thisPart = *jThis;
    thisSet.erase(thisPart);

    thisPart->recursive |= (*jThat)->recursive;
    if(thisPart->lastCtxtFunc.isInitialized())
      thisPart->lastCtxtFunc = thisPart->lastCtxtFunc;

    thisSet.insert(thisPart);
    jThis=thisSet.find(thisPart);
    
    return true;
  }
  
  return false;
}

// Computes the meet of this and that and saves the result in this
// Returns true if this causes this to change and false otherwise
bool CallCtxSensLattice::meetUpdate(Lattice* that_arg)
{
  CallCtxSensLattice* that = dynamic_cast<CallCtxSensLattice*>(that_arg);
  assert(that);
  
  bool modified = false;
  
  if(callContextSensitivityDebugLevel>=1)
    dbg << "CallCtxSensLattice::meetUpdate() #outgoing="<<outgoing.size()<<" #that->outgoing="<<that->outgoing.size()<<endl;
  // Copy all the information from that->outgoing to this->outgoing, setting modified if we end up inserting
  // any new elements into this->outgoing.
  map<CallCtxSensPartPtr, set<CallCtxSensPartPtr> >::iterator iThis=outgoing.begin(),
                                                              iThat=that->outgoing.begin();
  while(iThis!=outgoing.end() && iThat!=that->outgoing.end()) {
    // If the current src in that->outgoing does exist in this->outgoing, copy it over
    if(*iThat < *iThis) {
      outgoing[iThat->first] = iThat->second;
      modified = true;
      iThat++;
    // If the current src in this->outgoing does not exist in that->outgoing, leave it alone
    } else if(*iThat > *iThis) {
      iThis++;
    // If the current src exists in both this->outgoing and that->outgoing
    } else if(*iThat == *iThis) {
      // Incorporate information in this->outgoing key PartPtrs about any recursion that may have been discovered    
      modified = updateMapKey(outgoing, iThis, iThat) || modified;

      set<CallCtxSensPartPtr>::iterator jThis=iThis->second.begin(),
                                        jThat=iThat->second.begin();

      while(jThis!=iThis->second.end() && jThat!=iThat->second.end()) {
        // If the current src->tgt pair in that->outgoing does not exist in this->outgoing, copy it over from that->outgoing
        if(*jThat < *jThis) {
          // Insert the current element in that->outgoing[*iThis]. Since it precedes jThis, we're sure that
          // this loop will not iterate over it
          iThis->second.insert(*jThat);
          modified = true;
          jThat++;
        // If the current src->tgt pair in this->outgoing does not exist in that->outgoing, leave it alone
        } else if(*jThat > *jThis) {
          jThis++;
        // If the current src->tgt pair is shared by this->outgoing and that->outgoing
        } else if(*jThat == *jThis) {
          // Incorporate information about any recursion that may have been discovered
          modified = updateSetElement(iThis->second, jThis, jThat) || modified;
          jThis++;
          jThat++;
        }
      }
      
      iThis++;
      iThat++;
    }
  }
  
  // Copy over all the information in that->outgoing that is stored after the last Part in this->outgoing.
  while(iThat!=that->outgoing.end()) {
    outgoing[iThat->first] = iThat->second;
    modified = true;
    iThat++;
  }
    
  // Recreate the incoming map from outgoing
  incoming.clear();
  for(map<CallCtxSensPartPtr, set<CallCtxSensPartPtr> >::iterator i=outgoing.begin(); i!=outgoing.end(); i++)
  for(set<CallCtxSensPartPtr>::iterator j=i->second.begin(); j!=i->second.end(); j++)
    incoming[*j].insert(i->first);
  
  return modified;
}

// Set this Lattice object to represent the set of all possible execution prefixes.
// Return true if this causes the object to change and false otherwise.
bool CallCtxSensLattice::setToFull()
{
  assert(0);
}

// Set this Lattice object to represent the of no execution prefixes (empty set)
// Return true if this causes the object to change and false otherwise.
bool CallCtxSensLattice::setToEmpty() {
  bool modified = outgoing.size()>0 && 
                  incoming.size()>0;
  initialize();
  return modified;
}

// Set all the information associated Lattice object with this MemLocObjectPtr to full.
// Return true if this causes the object to change and false otherwise.
bool CallCtxSensLattice::setMLValueToFull(MemLocObjectPtr ml)
{ return false; }

// Returns whether this lattice denotes the set of all possible execution prefixes.
bool CallCtxSensLattice::isFull()
{ return false; }

// Returns whether this lattice denotes the empty set.
bool CallCtxSensLattice::isEmpty()
{
  return outgoing.size()==0 && 
         incoming.size()==0;
}

std::string CallCtxSensLattice::str(std::string indent) {
  ostringstream oss;
  
  oss << "[CallCtxSensLattice: outgoing("<<outgoing.size()<<")="<<endl;
  for(map<CallCtxSensPartPtr, set<CallCtxSensPartPtr> >::iterator i=outgoing.begin(); i!=outgoing.end(); i++) {
    oss << indent << (i->first.get()? i->first.get()->str(indent+"    "): "NULLPartPtr")<< " ==&gt (#"<<i->second.size()<<");"<<endl;
    for(set<CallCtxSensPartPtr>::iterator j=i->second.begin(); j!=i->second.end(); j++)
      oss << indent << "            "<<(*j? j->get()->str(indent+"        "): "NULL")<<endl;
  }
  
  /*oss << indent << "incoming("<<incoming.size()<<")="<<endl;
  for(map<CallCtxSensPartPtr, set<CallCtxSensPartPtr> >::iterator i=incoming.begin(); i!=incoming.end(); i++) {
    oss << indent << i->first.get()->str(indent+"    ")<< " ==&gt;"<<endl;
    for(set<CallCtxSensPartPtr>::iterator j=i->second.begin(); j!=i->second.end(); j++)
      oss << indent << "        "<<(*j? j->get()->str(indent+"        "): "NULL")<<endl;
  }*/
  oss << "]";
  return oss.str();
}

/* ##########################################
   ##### CallContextSensitivityAnalysis #####
   ########################################## */

// Initializes the state of analysis lattices at the given function, part and edge into our out of the part
// by setting initLattices to refer to freshly-allocated Lattice objects.
void CallContextSensitivityAnalysis::genInitLattice(PartPtr part, PartEdgePtr pedge, 
                    std::vector<Lattice*>& initLattices)
{
  CallCtxSensLattice *ccsLat = new CallCtxSensLattice(pedge, this);
  
  // If this is the starting node of the application
  set<PartPtr> startParts = getComposer()->GetStartAStates(this);
  if(startParts.find(part) != startParts.end()) {
    list<PartEdgePtr> baseEdges = part->outEdges();
    
    // The only way this could have happened is if we're just starting the analysis. As such, 
    // initialize this entry CallCtxSensLattice object with edges that thinly wrap the 
    // PartEdges of the server analysis' outgoing edges, with an empty context
    for(list<PartEdgePtr>::iterator e=baseEdges.begin(); e!=baseEdges.end(); e++) {
      CallPartContextPtr emptyContext = makePtr<CallPartContext>();
      Function emptyLastCtxtFunc;
      bool noRecursion=false;
      ccsLat->outgoing[NULLPart].
              insert(makePtr<CallCtxSensPart>((*e)->source(), emptyContext, emptyLastCtxtFunc, noRecursion, this));
    }
  }
  
  initLattices.push_back(ccsLat);
}

bool CallContextSensitivityAnalysis::transfer(PartPtr part, CFGNode cn, NodeState& state, 
              std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo)
{
  assert(dfInfo[NULLPartEdge].size()==1);
  scope reg("CallContextSensitivityAnalysis::transfer()", scope::medium, callContextSensitivityDebugLevel, 1);
  CallCtxSensLattice* oldCCSLat = dynamic_cast<CallCtxSensLattice*>(dfInfo[NULLPartEdge][0]);
  assert(oldCCSLat);
  
  list<PartEdgePtr> baseEdges = part->outEdges();
  // If this abstract state has no outgoing edges, return without modifying dfInfo. This is because
  // the composer may wish to do something with the resulting Lattice (e.g. copy it on a wildcard outgoing edge) 
  // even if its information content has not changed.
  if(baseEdges.size()==0) return true;
  
  // Empty out dfInfo in preparation of it being overwritten by separate CallCtxSensLattices for each outgoing edge
  dfInfo.clear();
  
  set<CFGNode> matchNodes;
  
  // Consider all of this part's outgoing edges and for each create an edge that starts at src and ends at the edge's target
  for(list<PartEdgePtr>::iterator e=baseEdges.begin(); e!=baseEdges.end(); e++) {
    //if(callContextSensitivityDebugLevel>=1) dbg << "baseEdge="<<e->get()->str()<<endl;
    indent ind(callContextSensitivityDebugLevel, 1);
    scope reg(txt() << "baseEdge="<<e->get()->str(), scope::medium, callContextSensitivityDebugLevel, 1);
  
    // Create a new CallCtxSensLattice along this edge
    CallCtxSensLattice* newCCSLat = new CallCtxSensLattice(*e, this);
    dfInfo[*e].push_back(newCCSLat);
    
    // Iterate through all the outgoing edges before the call and create new edges after the call
    // We call their endpoints start and source since the targets of these edges will be the sources of the new
    // edges created by the transfer function.
    for(map<CallCtxSensPartPtr, set<CallCtxSensPartPtr> >::iterator start=oldCCSLat->outgoing.begin(); start!=oldCCSLat->outgoing.end(); start++) {
      // Focus on the CallCtxSensPartEdges that derive from the current baseEdge
      //if((start->first? start->first->getParent(): NULLPart) != (*e)->source()) continue;
      
      /*if(callContextSensitivityDebugLevel>=1) dbg << "start="<<(start->first? start->first.get()->str(): "NULLPartPtr")<<endl;
      indent ind(callContextSensitivityDebugLevel, 1);*/
      scope reg(txt()<<"start="<<(start->first? start->first.get()->str(): "NULLPartPtr"), scope::medium, callContextSensitivityDebugLevel, 1);
      
      for(set<CallCtxSensPartPtr>::iterator src=start->second.begin(); src!=start->second.end(); src++) {
        // Focus on the CallCtxSensPartEdges that derive from the current baseEdge
        if((*src? (*src)->getParent(): NULLPart) != (*e)->source()) continue;
        
        //if(callContextSensitivityDebugLevel>=1) dbg << "src="<<src->get()->str()<<endl;
        scope reg(txt()<<"src="<<src->get()->str(), scope::medium, callContextSensitivityDebugLevel, 1);
        if(callContextSensitivityDebugLevel>=1) dbg << "sensDepth="<<getSensDepth()<<", src->context(#"<<(*src)->context.getCtxtStackDepth()<<")="<<src->get()->context.str()<<endl;
        //indent ind(callContextSensitivityDebugLevel, 1);
        assert((*e)->source() == (*src)->getParent());

        set<CallCtxSensPartPtr> newTargets;
        if(isOutgoingCallAmbiguous(*e)) {
          scope outScp("CallContextSensitivityAnalysis::transfer() OutgoingFuncCall", scope::medium, callContextSensitivityDebugLevel, 1);
          newTargets = createCallOutEdge(*e, *src);
        } else if(isFuncExitAmbiguous(*e, matchNodes)) {
          assert(isSgFunctionDefinition((matchNodes.begin())->getNode()));
          Function exitingFunc(isSgFunctionDefinition((matchNodes.begin())->getNode()));
          scope outScp(txt()<<"CallContextSensitivityAnalysis::transfer() Function Exit "<<exitingFunc.get_name().getString(), scope::medium, callContextSensitivityDebugLevel, 1);          
          newTargets = createFuncExitEdge(*e, *src);
        } else {
          if(callContextSensitivityDebugLevel>=1) dbg << "Internal Node" << endl;
          newTargets.insert(makePtr<CallCtxSensPart>((*e)->target(), (*src)->context, (*src)->lastCtxtFunc, (*src)->recursive, this));
        }
        
        for(set<CallCtxSensPartPtr>::iterator t=newTargets.begin(); t!=newTargets.end(); t++) {
          if(callContextSensitivityDebugLevel>=1) dbg << "<b>newTarget</b>="<<t->get()->str()<<endl;
          
          newCCSLat->outgoing[*src].insert(*t);
          newCCSLat->incoming[*t].insert(*src);
        }
      }
    }
  }
  
  // Deallocate the original Lattice
  delete oldCCSLat;
  
  return true;
}

// Returns true if the given part denotes an outgoing function call to a function that is targeted
// by other calls.
bool CallContextSensitivityAnalysis::isOutgoingCallAmbiguous(PartEdgePtr edge) {
  scope s("isOutgoingCallAmbiguous(edge)", scope::medium, callContextSensitivityDebugLevel, 1);
  if(callContextSensitivityDebugLevel>=1) dbg << "edge="<<edge->str()<<endl;
  
  set<CFGNode> matchNodes;
  if(!edge->source()->mustOutgoingFuncCall(matchNodes)) return false;
  if(matchNodes.size()!=1) return true;
  if(!edge->target()->mustFuncEntry(matchNodes)) return false;
  if(matchNodes.size()!=1) return true;
  
  Function callee = Function::getEnclosingFunction(matchNodes.begin()->getNode());
  return isOutgoingCallAmbiguous(edge, callee);
}

// Returns true if the given part denotes an outgoing function call to a function that is targeted
// by other calls.
bool CallContextSensitivityAnalysis::isOutgoingCallAmbiguous(PartEdgePtr edge, Function callee) {
  set<CFGNode> matchNodes;
  if(!edge->source()->mustOutgoingFuncCall(matchNodes)) return false;
  if(matchNodes.size()!=1) return true;
  
  scope s("isOutgoingCallAmbiguous(call->callee)", scope::medium, callContextSensitivityDebugLevel, 1);
  if(callContextSensitivityDebugLevel>=1) {
    dbg << "edge="<<edge->str()<<endl;
    dbg << "callee="<<callee.get_name().getString()<<"()"<<endl;
  }

  // If we already know whether this call is ambiguous, return immediately
  if(callAmbiguity.find(edge->source()) != callAmbiguity.end() && 
     callAmbiguity[edge->source()].find(callee) != callAmbiguity[edge->source()].end()) {
    dbg << "Recorded: "<<(callAmbiguity[edge->source()][callee]? "": "NOT")<<" Ambiguous"<<endl;
    return callAmbiguity[edge->source()][callee];
  }

  // Otherwise, compute the result now

  /*// There is ambiguity about the target of this function call if either
  list<PartEdgePtr> out=part->outEdges();
  set<PartPtr> startStates = getComposer()->GetStartAStates(this);

  //dbg << "mustIncomingFuncCall, #outEdges="<<out.size()<<endl;
  bool isAmbiguous=false;
  for(list<PartEdgePtr>::iterator e=out.begin(); e!=out.end(); e++) {
    scope s2(txt()<<"edge="<<e->get()->str(), scope::medium);
    //dbg << "(*e)->target()="<<(*e)->target()->str()<<endl;
    //dbg << "#(*e)->target()->inEdges()="<<(*e)->target()->inEdges().size()<<endl;

    assert((*e)->target()->inEdges().size()>=1);*/

    // Multiple functions call this one
    list<PartEdgePtr> tgtIn=edge->target()->inEdges();
    assert(tgtIn.size()>=1);
    if(tgtIn.size()>1) {
      // Such that at least one of the calls has a context that is different from the callee's (not a recursive call 
      // inside its own context) and is not identical to part
      for(list<PartEdgePtr>::iterator i=tgtIn.begin(); i!=tgtIn.end(); i++) {
        scope s3(txt()<<"input"<<i->get()->str(), scope::medium, callContextSensitivityDebugLevel, 1);
        if(callContextSensitivityDebugLevel>=1) {
          dbg << "(*i)->source() == edge->source()="<<((*i)->source() == edge->source())<<endl;
          dbg << "(*i)->source()->getContext() == (*e)->source()->getContext()="<<((*i)->source()->getContext() == edge->target()->getContext())<<endl;
        }

        if((*i)->source() != edge->source() && 
           (*i)->source()->getContext() != edge->target()->getContext())
        {
          //isAmbiguous = true;
          if(callContextSensitivityDebugLevel>=1) dbg << "Is Ambiguous"<<endl;
          //goto END_LOOP;
          callAmbiguity[edge->source()][callee] = true;
          return true;
        }
      }
    }

    // There is just one call AND this function may be called from outside this compilation unit
    set<PartPtr> startStates = getComposer()->GetStartAStates(this);
    if(startStates.find(edge->target())!=startStates.end()) {
      if(callContextSensitivityDebugLevel>=1) dbg << "Ambiguous since this is a Start state"<<endl;
      callAmbiguity[edge->source()][callee] = true;
      return true;
    }
  //}
  //END_LOOP:

/*    // Make sure that this outgoing call is un-ambiguous IFF the matching incoming call parts are also un-ambiguous
  if(!isAmbiguous) {
    set<PartPtr> matchingCalls = part->matchingCallParts();
    for(set<PartPtr>::iterator c=matchingCalls.begin(); c!=matchingCalls.end(); c++) {
      set<CFGNode> subMatchNodes;
      if(isIncomingCallAmbiguous(*c, subMatchNodes)) {
        scope errScp("Ambiguity mismatch in isOutgoingCallAmbiguous", scope::medium, callContextSensitivityDebugLevel, 1);
        dbg << "outgoing="<<part->str()<<endl;
        dbg << "incoming="<<c->get()->str()<<endl;
      }
      assert(!isIncomingCallAmbiguous(*c, subMatchNodes));
    }
  }*/

  callAmbiguity[edge->source()][callee] = false;
  return false;
}

// Returns true if the given part denotes a function exit may return control to multiple function call sites.
bool CallContextSensitivityAnalysis::isFuncExitAmbiguous(PartEdgePtr edge, set<CFGNode>& matchNodes) {
  if(!edge->source()->mustFuncExit(matchNodes)) return false;
  if(matchNodes.size()!=1) return true;
  
  assert(isSgFunctionDefinition(matchNodes.begin()->getNode()));
  Function returningFunc(isSgFunctionDefinition(matchNodes.begin()->getNode()));
  
  scope reg("isFuncExitAmbiguous()", scope::medium, callContextSensitivityDebugLevel, 1);
  if(callContextSensitivityDebugLevel>=1) {
    dbg << "edge="<<edge->str()<<endl;
    dbg << "returningFunc="<<returningFunc.get_name().getString()<<endl;
  }
  
  set<CFGNode> incomingMatchNodes;
  if(!edge->target()->mustIncomingFuncCall(incomingMatchNodes)) return false;
  if(incomingMatchNodes.size()!=1) return true;
  
  // Iterate over the outgoing function calls that match this incoming call
  set<PartPtr> matchingCalls = edge->target()->matchingCallParts();
  for(set<PartPtr>::iterator c=matchingCalls.begin(); c!=matchingCalls.end(); c++) {
    scope reg2(txt()<<"c="<<c->get()->str(), scope::low, callContextSensitivityDebugLevel, 1);
    // Look for the targets of the matching calls
    list<PartEdgePtr> out=c->get()->outEdges();
    for(list<PartEdgePtr>::iterator o=out.begin(); o!=out.end(); o++) {
      scope reg3(txt()<<"o="<<o->get()->str(), scope::low, callContextSensitivityDebugLevel, 1);

      set<CFGNode> callMatchNodes;
      assert((*o)->target()->mustFuncEntry(callMatchNodes));
      if(callMatchNodes.size()==1) {
        Function callee = Function::getEnclosingFunction(callMatchNodes.begin()->getNode());
        dbg << "callee="<<callee.get_name().getString()<<", (returningFunc == callee)="<<(returningFunc == callee)<<endl;
        // If this is a call to the same function as we're currently returning from, return true
        // only if it is ambiguous. Otherwise, look for other possible matches to this return
        if(returningFunc == callee && isOutgoingCallAmbiguous(*o, callee))
          return true;
      }
    }
  }
  return false;
  
  /*
    
    // There is ambiguity about the target of this function call if either
    
    // Multiple functions call this one
    list<PartEdgePtr> out=part->outEdges();
    if(out.size()>1) {
      // Such that at least two of the calls have a context that is different from the callee's (not a recursive call 
      // inside its own context)
      int numCallsDiffContext=0;
      for(list<PartEdgePtr>::iterator o=out.begin(); o!=out.end(); o++) {
        scope reg(txt()<<"o="<<o->get()->str(), scope::low, callContextSensitivityDebugLevel, 1);
        if((*o)->target()->getContext() != part->getContext())
        {
          numCallsDiffContext++;
          if(callContextSensitivityDebugLevel>=1) {
            dbg << "Out edge has different context! numCallsDiffContext="<<numCallsDiffContext<<endl;
            dbg << "(*o)->target()->getContext()="<<(*o)->target()->getContext()->str()<<endl;
            dbg << "part->getContext()="<<part->getContext()->str()<<endl;
          }
          if(numCallsDiffContext>=2) {
            if(callContextSensitivityDebugLevel>=1) dbg << "Function exit ambiguous!"<<endl;
            return true;
          }
        }
      }
    }
    
    // There is just one call AND this function may be called from outside this compilation unit
    set<PartPtr> endStates = getComposer()->GetEndAStates(this);
    if(endStates.find(part)!=endStates.end()) {
      if(callContextSensitivityDebugLevel>=1) dbg << "Function exit ambiguous since it is an End State!"<<endl;
      return true;
    }
  }
  if(callContextSensitivityDebugLevel>=1) dbg << "Function exit not ambiguous."<<endl;
  return false;*/
}

// Returns true if the given part denotes an incoming function call to a function that is targeted
// by other calls.
/*bool CallContextSensitivityAnalysis::isIncomingCallAmbiguous(PartPtr part, set<CFGNode>& matchNodes) {
  scope s1("isIncomingCallAmbiguous", scope::medium, callContextSensitivityDebugLevel, 1);
  if(callContextSensitivityDebugLevel>=1) dbg << "part="<<part->str()<<endl;
  
  if(part->mustIncomingFuncCall(matchNodes) && matchNodes.size()==1) {
    // There is ambiguity about the target of this function call if either
    
    list<PartEdgePtr> in=part->inEdges();
    set<PartPtr> endStates = getComposer()->GetEndAStates(this);
    if(callContextSensitivityDebugLevel>=1) dbg << "mustIncomingFuncCall, #inEdges="<<in.size()<<endl;
    for(list<PartEdgePtr>::iterator e=in.begin(); e!=in.end(); e++) {
      scope s2(txt()<<"edge="<<e->get()->str(), scope::medium, callContextSensitivityDebugLevel, 1);
      //dbg << "(*e)->source()="<<(*e)->source()->str()<<endl;
      //dbg << "#(*e)->source()->outEdges()="<<(*e)->source()->outEdges().size()<<endl;
      
      if(isFuncExitAmbiguous((*e)->source(), matchNodes)) {
        if(callContextSensitivityDebugLevel>=1) dbg << "Is Ambiguous"<<endl;
        return true;
      }
      
      // There is just one call AND this function may be called from outside this compilation unit
      if(endStates.find((*e)->source())!=endStates.end()) {
        if(callContextSensitivityDebugLevel>=1) dbg << "Ambiguous since this is an End State"<<endl;
        return true;
      }
      
     / *assert((*e)->source()->outEdges().size()>=1);
      
      // Multiple functions call this one
      list<PartEdgePtr> srcOut=(*e)->source()->outEdges();
      if(srcOut.size()>1) {
        // Such that at least one of the calls has a context that is different from the callee's (not a recursive call 
        // inside its own context) and is not identical to part
        for(list<PartEdgePtr>::iterator o=srcOut.begin(); o!=srcOut.end(); o++) {
          scope s3(txt()<<"output"<<o->get()->str(), scope::medium);
          dbg << "(*o)->target() == part="<<((*o)->target() == part)<<endl;
          dbg << "(*o)->target()->getContext() == (*e)->source()->getContext()="<<((*o)->target()->getContext() == (*e)->source()->getContext())<<endl;
          if((*o)->target() != part && 
             (*o)->target()->getContext() != (*e)->source()->getContext())
          {
            dbg << "Is Ambiguous"<<endl;
            return true;
          }
        }
      }* /
    }
  }
  if(callContextSensitivityDebugLevel>=1) dbg << "Is not Ambiguous"<<endl;
  return false;
}*/

// Portion of the transfer function that creates edges along the outgoing edges of function calls
set<CallCtxSensPartPtr> CallContextSensitivityAnalysis::createCallOutEdge(PartEdgePtr baseEdge, CallCtxSensPartPtr src) {
  set<CallCtxSensPartPtr> ret;

  // The function that is being entered by the current function call
  SgFunctionParameterList* calleeEntry = baseEdge->target()->mustSgNodeAll<SgFunctionParameterList>();
  assert(calleeEntry);
  Function calleeFunc(SageInterface::getEnclosingFunctionDeclaration(calleeEntry));

  // The function that contains the call
  SgFunctionCallExp* call = src->mustSgNodeAll<SgFunctionCallExp>();
  assert(call);
  Function callerFunc(SageInterface::getEnclosingFunctionDeclaration(call));
  
  // If the start->src edge has reached our sensitivity depth limit
  if(src->context.getCtxtStackDepth() == getSensDepth()) {
    if(callContextSensitivityDebugLevel>=1) dbg << "At stack depth limit."<<endl;
    // For now we'll only consider Parts with a single CFGNode
    assert(src->CFGNodes().size()==1);

    // If we've observed that the the function where the last call in tgt->context is located is or has been
    // called recursively,
    // - The flag target->recursive is already set to true
    // - We observe this at the current call
    if(callContextSensitivityDebugLevel>=1) dbg << "src->lastCtxtFunc="<<src->lastCtxtFunc.get_name().getString()<<"(), calleeFunc="<<calleeFunc.get_name().getString()<<endl;
    if(src->recursive || src->lastCtxtFunc==calleeFunc) {
      if(callContextSensitivityDebugLevel>=1) dbg << "Recursive call."<<endl;
      ret.insert(makePtr<CallCtxSensPart>(baseEdge->target(), src->context, src->lastCtxtFunc, true, this));
    // Else, if there is no recursion at this point in the analysis
    } else {
      if(callContextSensitivityDebugLevel>=1) dbg << "Non-recursive call."<<endl;
      ret.insert(makePtr<CallCtxSensPart>(baseEdge->target(), src->context, src->lastCtxtFunc, false, this));
    }
  // If we haven't yet reached the limits of our context 
  } else {
    if(callContextSensitivityDebugLevel>=1) dbg << "Stack depth limit not reached."<<endl;
    //CallPartContextPtr new_context = makePtr<CallPartContext>(src->context);
    CallPartContext new_context = src->context;
    new_context.push(baseEdge->source());
    //dbg << "#new_context="<<new_context.getCtxtStackDepth()<<endl;

    ret.insert(makePtr<CallCtxSensPart>(baseEdge->target(), new_context, 
                           // If the context has reached its limit, pass the Fucntion object that denoted the 
                           // object in which the call resides. Otherwise, pass a blank Function object
                           (new_context.getCtxtStackDepth()==getSensDepth()? callerFunc: Function()),
                           false, this));
  }
  
  return ret;
}

// Portion sof the transfer function that creates edges along the exit edges of functions
set<CallCtxSensPartPtr> CallContextSensitivityAnalysis::createFuncExitEdge(PartEdgePtr baseEdge, CallCtxSensPartPtr src) {
  set<CFGNode> matchNodes;
  
  // We must be returning to a Part that denotes the incoming half of a function call
  assert(baseEdge->target()->mustIncomingFuncCall(matchNodes) && matchNodes.size()==1);
  SgFunctionCallExp* call = baseEdge->target()->mustSgNodeAll<SgFunctionCallExp>();
  assert(call);
  Function callerFunc(SageInterface::getEnclosingFunctionDeclaration(call));

  set<CallCtxSensPartPtr> ret;
  
  // If we're currently at the limit of our context
  if(src->context.getCtxtStackDepth() == getSensDepth()) {
    if(callContextSensitivityDebugLevel>=1) dbg << "At context depth limit"<<endl;
    // If we're returning to the last function in our context
    if(src->lastCtxtFunc == callerFunc) {
      if(callContextSensitivityDebugLevel>=1) dbg << "Returning to last in context"<<endl;
      assert(src->context.getCtxtStackDepth()>0);
          
      /*dbg << "context.back()="<<src->context.last()->str()<<endl;
      dbg << "baseEdge->target())="<<baseEdge->target()->str()<<endl;*/
      
      // If this baseEdge targets the most recent call listed in the context
      if(baseEdge->target()->mustMatchFuncCall(src->context.last())) {
        // Create an edge for the case where calls to the function were matched perfectly with their exits
        // through all of its recursive invocations within the current context (if any).

        // Pop off the most recent element in context and use this reduced context in the outgoing edges
        CallPartContext new_context = src->context;
        new_context.pop();
        if(callContextSensitivityDebugLevel>=1) dbg << "Match. #new_context="<<new_context.getCtxtStackDepth()<<endl;
        ret.insert(makePtr<CallCtxSensPart>(baseEdge->target(), new_context, Function(), false, this));
      } else {
        if(callContextSensitivityDebugLevel>=1) dbg << "No Match."<<endl;
      
        // If we encounter any recursive calls to this function, then it is possible that there were more
        // calls to it within the current context than there were returns. As such, add an edge for the case
        // where we return from the function while in the same context.
        if(src->recursive) {
          if(callContextSensitivityDebugLevel>=1) dbg << "Recursive."<<endl;

          ret.insert(makePtr<CallCtxSensPart>(baseEdge->target(), src->context, src->lastCtxtFunc, true, this));
        }
      }
    // If we're returning to a function that is not the last one in our context
    } else {
      if(callContextSensitivityDebugLevel>=1) dbg << "Returning to non-last in context"<<endl;
      // Create an edge with the same context information as src
      ret.insert(makePtr<CallCtxSensPart>(baseEdge->target(), src->context, src->lastCtxtFunc, src->recursive, this));
    }
  
  // If we're not currently at full context depth and we have a non-empty context (empty contexts correspond to 
  // exits from function calls that were invoked from outside the compilation unit)
  } else if(src->context.getCtxtStackDepth()>0) {
    if(callContextSensitivityDebugLevel>=1) {
      dbg << "Not full depth"<<endl;
      dbg << "baseEdge->target()="<<baseEdge->target()->str()<<endl;
      dbg << "src->context="<<src->context.str()<<endl;
      dbg << "src->context.last()="<<src->context.last()->str()<<endl;
    }
    // If we're currently returning to the Part that contains the most recent call on the context stack
    if(baseEdge->target()->mustMatchFuncCall(src->context.last())) {
      // Pop off the most recent element in context and use this reduced context in the outgoing edges
      CallPartContext new_context = src->context;
      new_context.pop();
      if(callContextSensitivityDebugLevel>=1) dbg << "Match. #new_context="<<new_context.getCtxtStackDepth()<<endl;
      ret.insert(makePtr<CallCtxSensPart>(baseEdge->target(), new_context, Function(), false, this));
    } else 
      if(callContextSensitivityDebugLevel>=1) dbg << "No Match."<<endl;
  } else 
    if(callContextSensitivityDebugLevel>=1)
      dbg << "Cannot exit from empty context."<<endl;
  
  return ret;
}

MemLocObjectPtr CallContextSensitivityAnalysis::Expr2MemLoc(SgNode* n, PartEdgePtr pedge_arg) {
  if(callContextSensitivityDebugLevel>=1) dbg << "CallContextSensitivityAnalysis::Expr2MemLoc() pedge_arg="<<pedge_arg->str()<<endl;
  CallCtxSensPartEdgePtr pedge = dynamicConstPtrCast<CallCtxSensPartEdge>(pedge_arg);
  assert(pedge);
  
  //NodeState* state = NodeState::getNodeState(this, pedge->getParent());
  
  MemLocObjectPtr baseML = getComposer()->Expr2MemLoc(n, pedge->getParent(), this);
  // Where the edge has both a source and destination their contexts must be the same. This holds for
  // MemLocs since they cannot be created along edges where calling contexts do change.
  return boost::make_shared<CallCtxSensML>(n, baseML, (pedge->src? pedge->src->context: pedge->tgt->context), this);
}

// Return the anchor Parts of the application
set<PartPtr> CallContextSensitivityAnalysis::GetStartAStates_Spec()
{
  set<PartPtr> startStates = getComposer()->GetStartAStates(this);
  set<PartPtr> startCCSStates;
  for(set<PartPtr>::iterator s=startStates.begin(); s!=startStates.end(); s++) {
    // Only include exits of functions with no calling contexts since those that do have calling contexts
    // correspond to instances of the functions that must have been called from inside the current compilation
    // unit and could not have been called from outside
    CallCtxSensPartPtr ccs = makePtr<CallCtxSensPart>(*s, this);
    if(ccs->context.getCtxtStackDepth()==0)
      startCCSStates.insert(ccs);
  }
  return startCCSStates;
}

set<PartPtr> CallContextSensitivityAnalysis::GetEndAStates_Spec()
{
  scope reg("CallContextSensitivityAnalysis::GetEndAStates_Spec()", scope::medium, callContextSensitivityDebugLevel, 3);
  
  set<PartPtr> endStates = getComposer()->GetEndAStates(this);
  set<PartPtr> endCCSStates;
  if(callContextSensitivityDebugLevel>=3) dbg << "#endStates="<<endStates.size()<<endl;
  for(set<PartPtr>::iterator e=endStates.begin(); e!=endStates.end(); e++) {
    scope reg(txt()<<"edge="<<e->get()->str(), scope::medium, callContextSensitivityDebugLevel, 3);
    
    // Find all the contexts that this end state may appear in
    NodeState* endNodeState = NodeState::getNodeState(this, *e);
    CallCtxSensLattice* lat = dynamic_cast<CallCtxSensLattice*>(endNodeState->getLatticeAbove(this, NULLPartEdge, 0));
    assert(lat);
    
    if(callContextSensitivityDebugLevel>=3) {
      dbg << "lat="<<lat->str()<<endl;
      dbg << "#lat->incoming="<<lat->incoming.size()<<endl;
    }
    
    // Find the current ending State in all of its correct contexts
    for(map<CallCtxSensPartPtr, set<CallCtxSensPartPtr> >::iterator i=lat->incoming.begin(); i!=lat->incoming.end(); i++) {
      // Only include exits of functions with no calling contexts since those that do have calling contexts
      // correspond to instances of the functions that must have been called from inside the current compilation
      // unit and could not have been called from outside
      if(i->first->context.getCtxtStackDepth()==0) {
        if(callContextSensitivityDebugLevel>=3) dbg << "i="<<i->first.get()->str()<<endl;
        endCCSStates.insert(i->first);
      }
    }
  }
  //endCCSStates.insert(makePtr<CallCtxSensPart>(*i, this));
  return endCCSStates;
}

// Given a PartEdge pedge implemented by this ComposedAnalysis, returns the part from its predecessor
// from which pedge was derived. This function caches the results if possible.
/*PartEdgePtr CallContextSensitivityAnalysis::convertPEdge_Spec(PartEdgePtr pedge)
{
  return pedge->getParent();
}*/

}; //namespace fuse
