#pragma once

#include <boost/function.hpp>
//#include <boost/lambda/lambda.hpp>
//#include <boost/lambda/bind.hpp>
#include "cfgUtils.h"
#include "widgets.h"
#include "comp_shared_ptr.h"

namespace fuse {

extern int partitionsDebugLevel;

// ----------------
// ----- Part -----
// ----------------

class Part;
class PartEdge;
class ComposedAnalysis;

typedef CompSharedPtr<Part> PartPtr;
typedef CompSharedPtr<const Part> ConstPartPtr;
typedef CompSharedPtr<PartEdge> PartEdgePtr;
typedef CompSharedPtr<const PartEdge> ConstPartEdgePtr;

// ------------------------
// ---- Remap Functor -----
// ------------------------

/*
 * // Base class of functors that are used to remap information from one set of MemLocObjects to another 
// at function call boundaries.
class MLRemapper
{
  public:
  // Given a lattice returns a freshly-allocated Lattice object that points to the remapped Lattice
  virtual Lattice* operator()(Lattice* lat)=0;
  
  // Equality check to help callers ensure that two different remapping functors remap in the same way
  virtual bool operator==(MLRemapper& that)=0;
};

// This derived class performs the null remapping where no information is remapped and thus works for the PartEdge base
// case where no remapping is needed.
class NullMLRemapper: public MLRemapper
{
  public:
  // Returns NULL to indicate that no remapping is needed
  Lattice* operator()(Lattice* lat) { return NULL; }
  
  // Equality check to help callers ensure that two different remapping functors remap in the same way
  bool operator==(MLRemapper& that)
  {
    // All NullMLRemapper are the same
    if(NullMLRemapper& that=dynamic_cast<NullMLRemapper&>(that))
      return true;
    
    // If this object is a MapMLRemapper but that object is not, they're not equal
    return false;
  }
};
*/

class Lattice;
class MemLocObject;
typedef boost::shared_ptr<MemLocObject> MemLocObjectPtr;

// A single mapping of a MemLoc to another MemLoc used by the MLRemapper
class MLMapping
{
  public:
  // If either from or to is NULLMemLocObject, this indicates that the other MemLoc exists purely
  // in the scope of a function and should not be propagated across function boundaries (i.e. it is
  // explicitly not mapped to anything outside the function).
  // In all current use-cases from must be non-NULL and to may be NULL, but this may change...
  MemLocObjectPtr from;
  MemLocObjectPtr to;
  
  // Indicates whether the to MemLoc should replace every instance of from MemLoc (true) or 
  // whether instances of the from MemLoc should be left alone and new mappings should be
  // created for the to MemLoc (false).
  bool replaceMapping;
  
  MLMapping(MemLocObjectPtr from, MemLocObjectPtr to, bool replaceMapping);
  
  bool operator <(const MLMapping& that) const;
  
  std::string str(std::string indent="") const;
};

// A functor that remaps information from one set of MemLocObjects to another at function call boundaries.
// from MemLocObjects that are the keys of ml2ml to their corresponding values in ml2ml. The value MemLocObjects in 
// ml2ml should be interpreted with respect to PartEdge newPEdge. It corresponds to the code region(s) to which we 
// are remapping.
class MLRemapper: public dbglog::printable
{
  // The edge for which the remapping is being performed
  PartEdgePtr pedge;
  
  // Set of sets that map that maps MemLocObjects at the edges's source to MemLocObjects at its destination 
  // (for forwards analyses) and vice versa (for backwards analyses) as they are converted by the edge. 
  // Since both the source and destination Part may maintain multiple CFGNodes, we may have a different remapping
  // for different CFGNode pairs at the source/destination Part. We thus maintain a set of such mappings for the
  // different scenarios, which are listed below.
  std::set<std::set<MLMapping > > fwML2ML, 
                                  bwML2ML;
  
  // Indicates whether pedge, fwML2ML and bwML2ML have been initialized
  bool initialized;
  
  // Edges out of a non-void return statements:
  //    return their values <-> the function's return object
  // Edges from a function's call to a the SgFunctionParameterList of a may-equal function
  //    call arguments <-> function parameters
  // Edges into of a function call from the SgFunctionDefinition of a may-equal function
  //    call arguments passed by reference <-> function parameters
  //    call return <-> the function's return object
  
  public:
  MLRemapper();
  MLRemapper(const MLRemapper& that);
  
  // This initialization method must be called by the MLRemapper's host PartEdge before it calls 
  //    forwardRemapML() or backwardRemapML(). The MLRemapper's full initalization is delayed in this way
  //    because the pedge argument is a shared_ptr created from the host PartEdge's this. shared_ptrs
  //    cannot be created from this in the object's constructor, thus forcing us to delay initialization
  //    until the host PartEdge is fully initialized.
  // It is legal to call this function multiple times but in each call the fields pedge and analysis
  //    must be identical.
  void init(PartEdgePtr pedge, ComposedAnalysis* analysis);
  
  // Given a lattice returns a freshly-allocated Lattice object that points to Lattice remapped in the forward direction.
  // Since the function is called for the scope change across some Part, it needs to account for the fact that
  //    some MemLocs are in scope on one side of Part, while others are in scope on the other side. 
  //    fromPEdge is the edge from which control is passing and the current PartEdge (same as the PartEdge of 
  //    the Lattice) is the one to which control is passing.
  Lattice* forwardRemapML(Lattice* lat, PartEdgePtr fromPEdge) const;
  
  // Given a lattice returns a freshly-allocated Lattice object that points to Lattice remapped in the backward direction
  // Since the function is called for the scope change across some Part, it needs to account for the fact that
  //    some MemLocs are in scope on one side of Part, while others are in scope on the other side. 
  //    fromPEdge is the edge from which control is passing and the current PartEdge (same as the PartEdge of 
  //    the Lattice) is the one to which control is passing.
  Lattice* backwardRemapML(Lattice* lat, PartEdgePtr fromPEdge) const;
  
  private:
  // Returns whether if the two given ml2ml maps are equal.
  static bool equalMaps(const std::set<std::set<MLMapping> >& ml2mlA,
                        const std::set<std::set<MLMapping> >& ml2mlB);
  
  public:
  bool operator==(const MLRemapper& that) const;
  
  private:
  // String representation of object
  std::string map2Str(std::set<std::set<MLMapping> >& ml2ml, std::string indent="");
  
  public:
  std::string str(std::string indent="");
};


class Composer;

// --------------------------------------------------------------------------
// ----- Support functions for MemLoc remapping across scope boundaries -----
// --------------------------------------------------------------------------

// Given a function call, sets argParamMap to map all arguments of this function call to their 
// corresponding parameters.
// Supports caller->callee transfers for forwards analyses and callee->caller transfers for backwards analyses
// (direction specified by the fw flag).
void setArgParamMap(PartEdgePtr callEdge, SgFunctionCallExp* call, 
                    std::set<MLMapping>& argParamMap,
                    Composer* composer, ComposedAnalysis* analysis,
                    bool fw);

// Given a function call, sets paramArgByRef2ParamMap to map all arguments of this function call that are passed by 
// reference to their corresponding parameters and to map the call's SgFunctionCallExp expression to the MemLocObject 
// that denotes the function's declaration (associated with its return value).
// Supports callee->caller transfers for forwards analyses and caller->callee transfers for backwards analyses.
void setArgByRef2ParamMap(PartEdgePtr callEdge, SgFunctionCallExp* call, 
                          std::set<MLMapping>& paramArgByRef2ParamMap,
                          Composer* composer, ComposedAnalysis* analysis);

// Given a map produced by setArgParamMap or setArgByRef2ParamMap, return the same map but where the key->value 
// mappings are inverted to value->key
std::set<MLMapping> invertArg2ParamMap(std::set<MLMapping> ml2ml);


// Contexts help to clarify the connection between a given Part and the properties of application
//    execution that would lead it to reach this part. For example, it may be the particular call site
//    that leads to a given instance of a function body or a given outcome of a conditional for which we've
//    created a separate sub-graph in the abstract transition system.
// Contexts are used to improve visualizations of the abstract transition system and to enable better
//    heuristics to iterate the graph.
// Contexts are maintained inside of Parts and for a given class derived from Part it must be true that 
//    all contexts in all the instances of this derived Part must be from the same sub-class of Context
//    since they'll need to be compared to each other.
class PartContext;
typedef CompSharedPtr<PartContext> PartContextPtr;
extern PartContextPtr NULLPartContextPtr;

class PartContext: public dbglog::printable//, public boost::enable_shared_from_this<PartContext>
{
  // Comparison operations must be derived on contexts to make it possible to differentiate
  // them and create data structures from them. Note that we will only need to compare 
  // contexts of Parts the parent Parts of which share a context. For example, if analysis A creates
  // Parts that are sensitive to the function a given operation is in, their contexts will hold the
  // ID of the function and implement relational operations based on function IDs. A subsequent
  // analysis B that implements calling-context sensitivity will use a context that maintains the
  // call site of a given copy of a function and will compare relative to this information.
  // However, since analysis already filters according to function ID, analysis B's relational operations
  // will not need to worry about Parts from different functions being related to each other.
  // Implementations of Context relational operations may take advantage of this whenever their form
  // of context only makes sense if other forms of context are used by prior analyses.
  public:
  
  /*
  // Return whether this context corresponds to the entry or exit of its respective parent context.
  // This information is used to guide visual layouts and transition system traversal heuristics and
  // may thus be approximate.
  virtual bool isEntry() { return false; }
  virtual bool isExit()  { return false; }*/
  
  // Returns a list of PartContextPtr objects that denote more detailed context information about
  // this PartContext's internal contexts. If there aren't any, the function may just return a list containing
  // this PartContext itself.
  virtual std::list<PartContextPtr> getSubPartContexts() const=0;/* { 
    std::list<PartContextPtr> listOfMe;
    listOfMe.push_back(makePtr<PartContext>(makePtrFromThis(shared_from_this())));
    return listOfMe;
  }*/
    
  // The default implementation corresponds to all Parts having the same context.
  virtual bool operator==(const PartContextPtr& that) const=0;// { return true; }
  virtual bool operator< (const PartContextPtr& that) const=0;// { return false; }
  
  bool operator!=(const PartContextPtr& that) const;
  bool operator>=(const PartContextPtr& that) const;
  bool operator<=(const PartContextPtr& that) const;
  bool operator> (const PartContextPtr& that) const;
};

// Combines the contexts of an individual Part and the contexts of its parent Parts
class Context;
typedef CompSharedPtr<Context> ContextPtr;
typedef CompSharedPtr<const Context> ConstContextPtr;
extern ContextPtr NULLContextPtr;

class Context//: public dbglog::printable
{
  public:
  /*ConstPartPtr part;
  Context(ConstPartPtr part): part(part) {}*/
  //PartPtr part;
  // List of the contexts of the initial part of this context and all of its ancestors, 
  // in the order of part, parent, grandparent, etc.
  std::list<PartContextPtr> partContexts;
  Context(PartPtr part);
  
  // Returns the list of PartContexts that comprise this Context. This list may include only one PartContext
  // for each Analysis that implements an Abstract Transition system.
  const std::list<PartContextPtr>& getPartContexts() const;
  
  // Returns a more detailed PartContexts lists that comprise this Context. This list contains one list for each
  // PartContext implemented by some analysis and this list contains the PartContext(s) that contain a fully detailed
  // description of the internal notion of context within the analysis.
  std::list<std::list <PartContextPtr> > getDetailedPartContexts() const;
  
  // The default implementation corresponds to all Parts having the same context.
  //static bool equalContext(ConstPartPtr a, ConstPartPtr b);
  //static bool equalContext(PartPtr a, PartPtr b);
  bool operator==(const ContextPtr& that) const;
  //static bool lessContext(PartPtr a, PartPtr b);
  //static bool lessContext(ConstPartPtr a, ConstPartPtr b);
  bool operator< (const ContextPtr& that) const;
  
  bool operator!=(const ContextPtr& that) const;
  bool operator>=(const ContextPtr& that) const;
  bool operator<=(const ContextPtr& that) const;
  bool operator> (const ContextPtr& that) const;
  
  std::string str(std::string indent="");
  // Generates the string representation of this context by recursively 
  // calling the string method of this parent's 
  //std::string str_rec(ConstPartPtr part, std::string indent="");
  //std::string str_rec(PartPtr part, std::string indent="");
};

class Part : public dbglog::printable, public boost::enable_shared_from_this<Part>
{
  protected:
  ComposedAnalysis* analysis;
  PartPtr parent;
  PartContextPtr pContext;
  
  public:
  Part(ComposedAnalysis* analysis, PartPtr parent, PartContextPtr pContext=NULLPartContextPtr) : 
    analysis(analysis), parent(parent), pContext(pContext) {}
  Part(const Part& that) : 
    analysis(that.analysis), parent(that.parent), pContext(that.pContext) {}
  
  ~Part();
  
  // Returns true if this PartEdge comes from the same analysis as that PartEdge and false otherwise
  bool compatible(const Part& that) { return analysis == that.analysis; }
  bool compatible(PartPtr that)     { return analysis == that->analysis; }
  
  // Returns the Part from which this Part is derived. This function documents the hierarchical descent of this Part
  // and makes it possible to find the common parent of parts derived from different analyses.
  // Returns NULLPart if this part has no parents (i.e. it is implemented by the syntactic analysis)
  virtual PartPtr getParent() const { return parent; }
  
  // Sets this Part's parent
  virtual void setParent(PartPtr parent) { this->parent = parent; }
  
  virtual std::list<PartEdgePtr> outEdges()=0;
  virtual std::list<PartEdgePtr> inEdges()=0;
  virtual std::set<CFGNode> CFGNodes() const=0;
  
  // If this Part corresponds to a function call/return, returns the set of Parts that contain
  // its corresponding return/call, respectively.
  virtual std::set<PartPtr> matchingCallParts() const=0;
  
  /*// Let A={ set of execution prefixes that terminate at the given anchor SgNode }
  // Let O={ set of execution prefixes that terminate at anchor's operand SgNode }
  // Since to reach a given SgNode an execution must first execute all of its operands it must
  //    be true that there is a 1-1 mapping m() : O->A such that o in O is a prefix of m(o).
  // This function is the inverse of m: given the anchor node and operand as well as the
  //    Part that denotes a subset of A (the function is called on this part), 
  //    it returns a list of Parts that partition O.
  virtual std::list<PartPtr> getOperandPart(SgNode* anchor, SgNode* operand)=0;*/
  
  // Returns a PartEdgePtr, where the source is a wild-card part (NULLPart) and the target is this Part
  virtual PartEdgePtr inEdgeFromAny()=0;
  // Returns a PartEdgePtr, where the target is a wild-card part (NULLPart) and the source is this Part
  virtual PartEdgePtr outEdgeToAny()=0;
  
  // Returns the context that includes this Part and its ancestors.
  virtual ContextPtr getContext();
  
  // Returns the specific context of this Part. Can return the NULLPartContextPtr if this
  // Part doesn't implement a non-trivial context.
  virtual PartContextPtr getPartContext() const;
  
  // Applies the given lambda to all the CFGNodes within this part.
  // Returns true if the lambda returns true on ANY of them.
  template <typename Ret> 
  Ret mapCFGNodeANY(boost::function<bool(const CFGNode&)> func) {
    Ret r = (Ret)NULL;
    std::set<CFGNode> v=CFGNodes();
    for(std::set<CFGNode>::iterator i=v.begin(); i!=v.end(); i++) {
      if((r = func(*i))) return r;
    }
    return r;
  }
  
  // Applies the given lambda to all the CFGNodes within this part.
  // Returns true if the lambda returns true on ALL of them.
  template <typename Ret> 
  Ret mapCFGNodeALL(boost::function<bool(const CFGNode&)> func){
    Ret r = (Ret)NULL;
    std::set<CFGNode> v=CFGNodes();
    for(std::set<CFGNode>::iterator i=v.begin(); i!=v.end(); i++) {
      if(!(r = func(*i))) return r;
    }
    return r;
  }
  
  // If there exist one or more CFGNodes within this part have sub-type NodeType of SgNode,
  // returns a pointer to one of them.
  template <class NodeType>
  NodeType* maySgNodeAny() const {
    std::set<CFGNode> v=CFGNodes();
    for(std::set<CFGNode>::const_iterator i=v.begin(); i!=v.end(); i++) {
      if(dynamic_cast<NodeType*>(i->getNode()) == NULL)  return NULL;
    }
    return dynamic_cast<NodeType*>(v.begin()->getNode());

    /*boost::function<NodeType* (CFGNode)> c = dynamic_cast<NodeType*>(boost::lambda::_1);
    return mapCFGNodeALL<NodeType*>(c);
    return NULL;*/
  }
  
  // If all the CFGNodes within this part have sub-type NodeType of SgNode,
  // returns a pointer to one of them.
  template <class NodeType>
  NodeType* mustSgNodeAll() const {
    std::set<CFGNode> v=CFGNodes();
    for(std::set<CFGNode>::const_iterator i=v.begin(); i!=v.end(); i++) {
      if(dynamic_cast<NodeType*>(i->getNode()) == NULL)  return NULL;
    }
    return dynamic_cast<NodeType*>(v.begin()->getNode());
    
    //boost::function<NodeType* (CFGNode)> c = dynamic_cast<NodeType*>(boost::lambda::_1);
    //return mapCFGNodeALL<NodeType*>(c);
    //return NULL;
  }
  
  // If the filter accepts (returns true) on any of the CFGNodes within this part, return true)
  bool filterAny(bool (*filter) (CFGNode cfgn));
  
  // If the filter accepts (returns true) on all of the CFGNodes within this part, return true)
  bool filterAll(bool (*filter) (CFGNode cfgn));
  
  // Returns whether this node denotes the portion of a function call that targets the entry into another function
  static bool isOutgoingFuncCall(CFGNode cfgn);
  // Returns whether all or some of this Part's CFGNodes denote the outgoing portion of a function call and
  // return the relevant CFGNode(s)
  bool mustOutgoingFuncCall(std::set<CFGNode>& ret);
  bool mayOutgoingFuncCall(std::set<CFGNode>& ret);
  
  // Returns whether this node denotes the portion of a function call to which control from the callee function
  // flows after it terminates.
  static bool isIncomingFuncCall(CFGNode cfgn);
  // Returns whether all or some of this Part's CFGNodes denote the incoming portion of a function call and
  // return the relevant CFGNode(s)
  bool mustIncomingFuncCall(std::set<CFGNode>& ret);
  bool mayIncomingFuncCall(std::set<CFGNode>& ret);
  
  // Returns whether both this and that parts have the same context and their CFGNode lists consist
  // exclusively of matching pairs of outgoing and incoming function calls (for each outgoing call in one
  // list there's an incoming call in the other and vice versa).
  bool mustMatchFuncCall(PartPtr that);
  
  // Returns whether both this and that parts have the same context and their CFGNode lists include some 
  // matching pairs of outgoing and incoming function calls.
  bool mayMatchFuncCall(PartPtr that);
  
  // Returns whether this node denotes the entry into a function
  static bool isFuncEntry(CFGNode cfgn);
  // Returns whether all or some of this Part's CFGNodes denote a function's entry node and return the relevant CFGNode(s)
  bool mustFuncEntry(std::set<CFGNode>& ret);
  bool mayFuncEntry(std::set<CFGNode>& ret);
  
  // Returns whether this node denotes the exit from a function
  static bool isFuncExit(CFGNode cfgn);
  // Returns whether all or some of this Part's CFGNodes denote a function's exit node and return the relevant CFGNode(s)
  bool mustFuncExit(std::set<CFGNode>& ret);
  bool mayFuncExit(std::set<CFGNode>& ret);
  
  // The the base equality and comparison operators are implemented in Part and these functions
  // call the equality and inequality test functions supplied by derived classes as needed
  
  // If this and that come from the same analysis, call the type-specific equality test implemented
  // in the derived class. Otherwise, these Parts are not equal.
  bool operator==(const PartPtr& that) const;
  virtual bool equal(const PartPtr& that) const=0;
  
  // If this and that come from the same analysis, call the type-specific inequality test implemented
  // in the derived class. Otherwise, determine inequality by comparing the analysis pointers.
  bool operator<(const PartPtr& that) const;
  virtual bool less(const PartPtr& that) const=0;
  
  bool operator!=(const PartPtr& that) const;
  bool operator>=(const PartPtr& that) const;
  bool operator<=(const PartPtr& that) const;
  bool operator> (const PartPtr& that) const;
};
extern PartPtr NULLPart;

class PartEdge : public dbglog::printable, public boost::enable_shared_from_this<PartEdge> {
  protected:
  ComposedAnalysis* analysis;
  PartEdgePtr parent;
  // The functor that remaps lattices across function call boundaries that this edge may be a part of
  MLRemapper remap;
  
  public:
  /*PartEdge(ComposedAnalysis* analysis, PartEdgePtr parent) :
    analysis(analysis), parent(parent), remap(boost::make_shared<NullMLRemapper>()) {}
  PartEdge(ComposedAnalysis* analysis, PartEdgePtr parent, boost::shared_ptr<MLRemapper> remap) :
    analysis(analysis), parent(parent), remap(remap) {}*/
  PartEdge(ComposedAnalysis* analysis, PartEdgePtr parent) : 
    analysis(analysis), parent(parent) {}
  PartEdge(const PartEdge& that) :
    analysis(that.analysis), parent(that.parent), remap(that.remap) {}
  
  // Returns true if this PartEdge comes from the same analysis as that PartEdge and false otherwise
  bool compatible(const PartEdge& that) { return analysis == that.analysis; }
  bool compatible(PartEdgePtr that)     { return analysis == that->analysis; }
  
  // Returns the PartEdge from which this PartEdge is derived. This function documents the hierarchical descent of this 
  // PartEdge and makes it possible to find the common parent of parts derived from different analyses.
  // Returns NULLPartEdge if this part has no parents (i.e. it is implemented by the syntactic analysis)
  virtual PartEdgePtr getParent() const { return parent; }
  
  // Sets this PartEdge's parent
  virtual void setParent(PartEdgePtr parent) { this->parent = parent; }
  
  virtual PartPtr source() const=0;
  virtual PartPtr target() const=0;
  
  // Let A={ set of execution prefixes that terminate at the given anchor SgNode }
  // Let O={ set of execution prefixes that terminate at anchor's operand SgNode }
  // Since to reach a given SgNode an execution must first execute all of its operands it must
  //    be true that there is a 1-1 mapping m() : O->A such that o in O is a prefix of m(o).
  // This function is the inverse of m: given the anchor node and operand as well as the
  //    PartEdge that denotes a subset of A (the function is called on this PartEdge), 
  //    it returns a list of PartEdges that partition O.
  // A default implementation that walks the server analysis-provided graph backwards to find 
  //    matching PartEdges is provided.
  virtual std::list<PartEdgePtr> getOperandPartEdge(SgNode* anchor, SgNode* operand);
  
  // If the source Part corresponds to a conditional of some sort (if, switch, while test, etc.)
  // it must evaluate some predicate and depending on its value continue, execution along one of the
  // outgoing edges. The value associated with each outgoing edge is fixed and known statically.
  // getPredicateValue() returns the value associated with this particular edge. Since a single 
  // Part may correspond to multiple CFGNodes getPredicateValue() returns a map from each CFG node
  // within its source part that corresponds to a conditional to the value of its predicate along 
  // this edge.
  virtual std::map<CFGNode, boost::shared_ptr<SgValueExp> > getPredicateValue()=0;
  
  // The the base equality and comparison operators are implemented in Part and these functions
  // call the equality and inequality test functions supplied by derived classes as needed
  
  // If this and that come from the same analysis, call the type-specific equality test implemented
  // in the derived class. Otherwise, these Parts are not equal.
  bool operator==(const PartEdgePtr& that) const;
  virtual bool equal(const PartEdgePtr& that) const=0;
  
  // If this and that come from the same analysis, call the type-specific inequality test implemented
  // in the derived class. Otherwise, determine inequality by comparing the analysis pointers.
  bool operator<(const PartEdgePtr& that) const;
  virtual bool less(const PartEdgePtr& that) const=0;
  
  bool operator!=(const PartEdgePtr& that) const;
  bool operator>=(const PartEdgePtr& that) const;
  bool operator<=(const PartEdgePtr& that) const;
  bool operator> (const PartEdgePtr& that) const;
  
  // Remaps the given Lattice as needed to take into account any function call boundaries.
  // Remapping is performed both in the forwards and backwards directions. 
  // Returns the resulting Lattice object, which is freshly allocated.
  // Since the function is called for the scope change across some Part, it needs to account for the fact that
//    some MemLocs are in scope on one side of Part, while others are in scope on the other side. 
//    fromPEdge is the edge from which control is passing and the current PartEdge (same as the PartEdge of 
//    the Lattice) is the one to which control is passing.
  virtual Lattice* forwardRemapML(Lattice* lat, PartEdgePtr fromPEdge);
  virtual Lattice* backwardRemapML(Lattice* lat, PartEdgePtr fromPEdge);
  
  // Returns the remapping functor
  const MLRemapper& getRemap() const
  { return remap; }
};
extern PartEdgePtr NULLPartEdge;

class IntersectionPart;
typedef CompSharedPtr<IntersectionPart> IntersectionPartPtr;
class IntersectionPartEdge;
typedef CompSharedPtr<IntersectionPartEdge> IntersectionPartEdgePtr;

// The intersection of multiple Parts and PartEdges. Partition graph intersections are primarily useful for 
// parallel composition of analyses. In this use-case we have multiple analyses, a subset of which implements their 
// own partition graphs and the rest of which do not. To support this use-case we need to 
// - Provide a way to compute intersections the sub-parts and sub-part edges that are implemented. 
//   This is implemented in methods like Part::outEdges that consider the votes from all the sub-analyses
//   and return all the permutations of those votes.
// - When queries are performed on analyses that do or do not implement partition graphs, we need to 
//   make it easy to extract the part or part edge that is meaningful to each analysis. We support this
//   by maintaining for each IntersectionPart and IntersectionPartEdge a mapping from all analyses that implement
//   partition graphs to the Part/PartEdge they implement. For all analyses that do not implement partition
//   graphs we maintain the Part/PartEdge that these analyses operate on, which is the parent of the 
//   IntersectionPart/IntersectionPartEdge.


class IntersectionPartContext : public PartContext
{
  std::set<PartContextPtr> pContexts;
  public:
  IntersectionPartContext(const std::set<PartContextPtr>& pContexts) : pContexts(pContexts) {}
  
  bool operator==(const PartContextPtr& that) const;
  bool operator< (const PartContextPtr& that) const;
  
  std::string str(std::string indent="");
};

// The intersection of multiple Parts. Maintains multiple Parts and responds to API calls with the most 
//   accurate response that its constituent objects return.
class IntersectionPart : public Part
{
  // Parts from analyses that implement partition graphs
  std::map<ComposedAnalysis*, PartPtr> parts;

  public:
  
  //IntersectionPart(PartPtr part, ComposedAnalysis* analysis);
  IntersectionPart(const std::map<ComposedAnalysis*, PartPtr>& parts, PartPtr parent, ComposedAnalysis* analysis);
  
  // Returns the Part associated with this analysis. If the analysis does not implement the partition graph
  // (is not among the keys of parts), returns the parent Part.
  PartPtr getPart(ComposedAnalysis* analysis);
  
  // Returns the list of outgoing IntersectionPartEdge of this Part, which are the cross-product of the outEdges()
  // of its sub-parts.
  std::list<PartEdgePtr> outEdges();
  /*// Recursive computation of the cross-product of the outEdges of all the sub-parts of this Intersection part.
  // Hierarchically builds a recursion tree that contains more and more combinations of PartsPtr from the outEdges
  // of different sub-parts. When the recursion tree reaches its full depth (one level per part in parts), it creates
  // an intersection the current combination of 
  // partI - refers to the current part in parts
  // outPartEdges - the list of outgoing edges of the current combination of this IntersectionPart's sub-parts, 
  //         upto partI
  void outEdges_rec(std::map<ComposedAnalysis*, PartPtr>::iterator partI, 
                    std::map<ComposedAnalysis*, PartPtr> outPartEdges, 
                    std::vector<PartEdgePtr>& edges);*/
  
  // Returns the list of incoming IntersectionPartEdge of this Part, which are the cross-product of the inEdges()
  // of its sub-parts.
  std::list<PartEdgePtr> inEdges();
  
  /*// Recursive computation of the cross-product of the inEdges of all the sub-parts of this Intersection part.
  // Hierarchically builds a recursion tree that contains more and more combinations of PartsPtr from the inEdges
  // of different sub-parts. When the recursion tree reaches its full depth (one level per part in parts), it creates
  // an intersection the current combination of 
  // partI - refers to the current part in parts
  // inPartEdges - the list of incoming edges of the current combination of this IntersectionPart's sub-parts, 
  //         upto partI
  void inEdges_rec(std::list<PartPtr>::iterator partI, std::list<PartEdgePtr> inPartEdges, 
                   std::vector<PartEdgePtr>& edges);*/
    
  // Returns the intersection of the lists of CFGNodes returned by the Parts in parts
  std::set<CFGNode> CFGNodes() const;
  
  // If this Part corresponds to a function call/return, returns the set of Parts that contain
  // its corresponding return/call, respectively.
  std::set<PartPtr> matchingCallParts() const;
  
  /*
  // Let A={ set of execution prefixes that terminate at the given anchor SgNode }
  // Let O={ set of execution prefixes that terminate at anchor's operand SgNode }
  // Since to reach a given SgNode an execution must first execute all of its operands it must
  //    be true that there is a 1-1 mapping m() : O->A such that o in O is a prefix of m(o).
  // This function is the inverse of m: given the anchor node and operand as well as the
  //    Part that denotes a subset of A (the function is called on this part), 
  //    it returns a list of Parts that partition O.
  std::list<PartPtr> getOperandPart(SgNode* anchor, SgNode* operand);
  
  // Recursive computation of the cross-product of the getOperandParts of all the sub-parts of this Intersection part.
  // Hierarchically builds a recursion tree that contains more and more combinations of PartsPtr from the inEdges
  // of different sub-parts. When the recursion tree reaches its full depth (one level per part in parts), it creates
  // an intersection the current combination of 
  // partI - refers to the current part in parts
  // accumOperandParts - the list of incoming parts of the current combination of this IntersectionPart's sub-parts, 
  //         upto partI
  void getOperandPart_rec(SgNode* anchor, SgNode* operand,
                          std::list<PartPtr>::iterator partI, std::list<PartPtr> accumOperandParts, 
                          std::list<PartPtr>& allParts);*/
  
  // Returns a PartEdgePtr, where the source is a wild-card part (NULLPart) and the target is this Part
  PartEdgePtr inEdgeFromAny();
  
  // Returns a PartEdgePtr, where the target is a wild-card part (NULLPart) and the source is this Part
  PartEdgePtr outEdgeToAny();
  
  // Two IntersectionParts are equal if their parents and all their constituent sub-parts are equal
  bool equal(const PartPtr& that) const;
  
  // Lexicographic ordering: This IntersectionPart is < that IntersectionPart if 
  // - their parents are < ordered, OR
  // - if this has fewer parts than that, OR
  // - there exists an index i in this.parts and that.parts s.t. forall j<i. this.parts[j]==that.parts[j] and 
  //   this.parts[i] < that.parts[i].
  bool less(const PartPtr& that) const;
  
  std::string str(std::string indent="");
};


class IntersectionPartEdge : public PartEdge
{
  // The edges being intersected
  std::map<ComposedAnalysis*, PartEdgePtr> edges;
  
  public:
  
  //IntersectionPartEdge(PartEdgePtr edge, ComposedAnalysis* analysis);
  IntersectionPartEdge(const std::map<ComposedAnalysis*, PartEdgePtr>& edges, PartEdgePtr parent, ComposedAnalysis* analysis);
  
  // Returns the PartEdge associated with this analysis. If the analysis does not implement the partition graph
  // (is not among the keys of parts), returns the parent PartEdge.
  PartEdgePtr getPartEdge(ComposedAnalysis* analysis);
  
  // Return the part that intersects the sources of all the sub-edges of this IntersectionPartEdge
  PartPtr source() const;
  
  // Return the part that intersects the targets of all the sub-edges of this IntersectionPartEdge
  PartPtr target() const;
  
  // Let A={ set of execution prefixes that terminate at the given anchor SgNode }
  // Let O={ set of execution prefixes that terminate at anchor's operand SgNode }
  // Since to reach a given SgNode an execution must first execute all of its operands it must
  //    be true that there is a 1-1 mapping m() : O->A such that o in O is a prefix of m(o).
  // This function is the inverse of m: given the anchor node and operand as well as the
  //    PartEdge that denotes a subset of A (the function is called on this PartEdge), 
  //    it returns a list of PartEdges that partition O.
  std::list<PartEdgePtr> getOperandPartEdge(SgNode* anchor, SgNode* operand);
  
  private:
  // Recursive computation of the cross-product of the getOperandParts of all the sub-part edges of this Intersection part edge.
  // Hierarchically builds a recursion tree that contains more and more combinations of PartEdgePtrs from the results of
  // getOperandPart of different sub-part edges. When the recursion tree reaches its full depth (one level per edge in edges), 
  // it creates an intersection the current combination of edges.
  // edgeI - refers to the current edge in edges
  // accumOperandPartEdges - the list of incoming edgesof the current combination of this IntersectionPartEdges's sub-Edges, 
  //         upto edgeI
  void getOperandPartEdge_rec(SgNode* anchor, SgNode* operand,
                              std::list<PartEdgePtr>::iterator edgeI, std::list<PartEdgePtr> accumOperandPartEdges, 
                              std::list<PartEdgePtr>& allPartEdges);
  
  // If the source Part corresponds to a conditional of some sort (if, switch, while test, etc.)
  // it must evaluate some predicate and depending on its value continue, execution along one of the
  // outgoing edges. The value associated with each outgoing edge is fixed and known statically.
  // getPredicateValue() returns the value associated with this particular edge. Since a single 
  // Part may correspond to multiple CFGNodes getPredicateValue() returns a map from each CFG node
  // within its source part that corresponds to a conditional to the value of its predicate along 
  // this edge.
  std::map<CFGNode, boost::shared_ptr<SgValueExp> > getPredicateValue();  

  public:
  // Two IntersectionPartEdges are equal of all their constituent sub-parts are equal
  bool equal(const PartEdgePtr& o) const;
  
  // Lexicographic ordering: This IntersectionPartEdge is < that IntersectionPartEdge if this has fewer edges than that or
  // there exists an index i in this.edges and that.edges s.t. forall j<i. this.edges[j]==that.edges[j] and 
  // this.edges[i] < that.edges[i].
  bool less(const PartEdgePtr& o) const;
  
  // Recursive function that uses a binary tree to check the equality of the remapping functors.
  // Returns whether the functors between iterators start and end are equal to each other.
  // numElts is the total number of map elements between start and end (not including end)
  bool isEqualRemap(std::map<ComposedAnalysis*, PartEdgePtr>::const_iterator start,
                    std::map<ComposedAnalysis*, PartEdgePtr>::const_iterator end,
                    int numElts) const;
  
  // Remaps the given Lattice as needed to take into account any function call boundaries.
  // Remapping is performed both in the forwards and backwards directions. 
  // Returns the resulting Lattice object, which is freshly allocated.
  // Since the function is called for the scope change across some Part, it needs to account for the fact that
  //    some MemLocs are in scope on one side of Part, while others are in scope on the other side. 
  //    fromPEdge is the edge from which control is passing and the current PartEdge (same as the PartEdge of 
  //    the Lattice) is the one to which control is passing.
  Lattice* forwardRemapML(Lattice* lat, PartEdgePtr fromPEdge);
  Lattice* backwardRemapML(Lattice* lat, PartEdgePtr fromPEdge);
  
  std::string str(std::string indent="");
};

}; // namespace fuse


