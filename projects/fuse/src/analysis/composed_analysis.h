#ifndef DATAFLOW_H
#define DATAFLOW_H

#include "nodeState.h"
#include "analysis.h"
#include "lattice.h"
#include "abstract_object.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <set>
#include <map>
#include <string>

namespace fuse {

class NotImplementedException
{};
  
class ComposedAnalysis;
typedef boost::shared_ptr<ComposedAnalysis> ComposedAnalysisPtr;

class ComposedAnalysis : public virtual Dataflow, public dbglog::printable
{
  public:
  Composer* composer;
  
  // Informs this analysis about the identity of the Composer object that composes
  // this analysis with others
  void setComposer(Composer* composer)
  {
    this->composer = composer;
  }
  
  Composer* getComposer()
  {
    return composer;
  }
  
  public:
  
  // Returns a shared pointer to a freshly-allocated copy of this ComposedAnalysis object
  virtual ComposedAnalysisPtr copy()=0;
  
  // Abstract interpretation functions that return this analysis' abstractions that 
  // represent the outcome of the given SgExpression. The default implementations of 
  // these throw NotImplementedException so that if a derived class does not implement 
  // any of these functions, the Composer is informed.
  //
  // The objects returned by these functions are expected to be deallocated by their callers.
  virtual ValueObjectPtr   Expr2Val    (SgNode* n, PartEdgePtr pedge) { throw NotImplementedException(); }
  virtual MemLocObjectPtr  Expr2MemLoc (SgNode* n, PartEdgePtr pedge) { throw NotImplementedException(); }
  virtual CodeLocObjectPtr Expr2CodeLoc(SgNode* n, PartEdgePtr pedge) { throw NotImplementedException(); }
  
  // Return true if the class implements Expr2* and false otherwise
  virtual bool implementsExpr2Val    () { return false; }
  virtual bool implementsExpr2MemLoc () { return false; }
  virtual bool implementsExpr2CodeLoc() { return false; }
  
  /*
  // <<<<<<<<<<
  // The following set of calls are just wrappers that call the corresponding
  // functions on their operand AbstractObjects. Implementations of Composed analyses may want to
  // provide their own implementations of these functions if they implement a partition graph
  // and need to convert the pedge from case they provide this support
  // inside analysis-specific functions rather than inside AbstractObject.
  
  // Returns whether the given pair of AbstractObjects are may-equal at the given PartEdge
  // Wrapper for calling type-specific versions of mayEqual without forcing the caller to care about the type of object
  bool mayEqual(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge);
  
  // Special calls for each type of AbstractObject
  virtual bool mayEqualV (ValueObjectPtr  val1, ValueObjectPtr  val2, PartEdgePtr pedge);
  virtual bool mayEqualML(MemLocObjectPtr  ml1, MemLocObjectPtr  ml2, PartEdgePtr pedge);
  virtual bool mayEqualCL(CodeLocObjectPtr cl1, CodeLocObjectPtr cl2, PartEdgePtr pedge);
  
  // Returns whether the given pair of AbstractObjects are must-equal at the given PartEdge
  // Wrapper for calling type-specific versions of mustEqual without forcing the caller to care about the type of object
  bool mustEqual(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge);
  
  // Special calls for each type of AbstractObject
  virtual bool mustEqualV (ValueObjectPtr  val1, ValueObjectPtr  val2, PartEdgePtr pedge);
  virtual bool mustEqualML(MemLocObjectPtr  ml1, MemLocObjectPtr  ml2, PartEdgePtr pedge);
  virtual bool mustEqualCL(CodeLocObjectPtr cl1, CodeLocObjectPtr cl2, PartEdgePtr pedge);
  
  // Returns whether the given AbstractObject is live at the given PartEdge
  // Wrapper for calling type-specific versions of isLive without forcing the caller to care about the type of object
  bool isLive       (AbstractObjectPtr ao, PartEdgePtr pedge);
  // Special calls for each type of AbstractObject
  virtual bool isLiveVal    (ValueObjectPtr val,   PartEdgePtr pedge);
  virtual bool isLiveMemLoc (MemLocObjectPtr ml,   PartEdgePtr pedge);
  virtual bool isLiveCodeLoc(CodeLocObjectPtr cl,  PartEdgePtr pedge);
  */
  
  // Returns whether the given pair of AbstractObjects are equal at the given PartEdge
  
  private:
  // Cached copies of the results of GetStartAState and GetEndAState
  std::set<PartPtr> StartAStates;
  std::set<PartPtr> EndAStates;
  std::map<PartEdgePtr, PartEdgePtr> new2oldPEdge;  
  
  public:
   
  // Returns true if this ComposedAnalysis implements the partition graph and false otherwise
  virtual bool implementsPartGraph() { return false; }
    
  // Return the anchor Parts of a given function, caching the results if possible
  std::set<PartPtr> GetStartAStates();
  std::set<PartPtr> GetEndAStates();
  
  // Specific Composers implement these two functions
  virtual std::set<PartPtr> GetStartAStates_Spec() { throw NotImplementedException(); }
  virtual std::set<PartPtr> GetEndAStates_Spec()   { throw NotImplementedException(); }
  
  // Given a PartEdge pedge implemented by this ComposedAnalysis, returns the part from its predecessor
  // from which pedge was derived. This function caches the results if possible.
  PartEdgePtr convertPEdge(PartEdgePtr pedge);
  
  // Specific Composers implement this function
  //virtual PartEdgePtr convertPEdge_Spec(PartEdgePtr pedge) { throw NotImplementedException(); }
  
/*  // Given a Part that this analysis implements returns the Part from the preceding analysis
  // that this Part corresponds to (we assume that each analysis creates one or more Parts and PartEdges
  // for each Part and PartEdge of its parent analysis)
  virtual PartPtr     sourcePart    (PartPtr part)      { throw NotImplementedException(); }
  // Given a PartEdge that this analysis implements returns the PartEdge from the preceding analysis
  // that this PartEdge corresponds to (we assume that each analysis creates one or more Parts and PartEdges
  // for each Part and PartEdge of its parent analysis)
  virtual PartEdgePtr sourcePartEdge(PartEdgePtr pedge) { throw NotImplementedException(); }*/
  
  // In the long term we will want analyses to return their own implementations of 
  // maps and sets. This is not strictly required to produce correct code and is 
  // therefore not supported.
  // Maps and Sets 
  /*virtual ValueSet* NewValueSet()  { throw NotImplementedException; }
  virtual ValueMap* NewValueMap()  { throw NotImplementedException; }
  
  virtual MemLocSet* NewMemLocSet() { throw NotImplementedException; }
  virtual MemLocMap* NewMemLocMap() { throw NotImplementedException; }
  
  virtual CodeLocSet* NewCodeLocSet() { throw NotImplementedException; }
  virtual CodeLocMap* NewCodeLocMap() { throw NotImplementedException; }*/

  public:
  typedef enum {fw=0, bw=1, none=2} direction;
  
  // Runs the intra-procedural analysis on the given function and returns true if
  /* // the function's NodeState gets modified as a result and false otherwise
  // state - the function's NodeState
  // analyzeFromDirectionStart - If true the function should be analyzed from its starting point from the analysis' 
  //    perspective (fw: entry point, bw: exit point)*/
  void runAnalysis();
  
  // Execute the analysis transfer function, updating its dataflow info.
  // The final state of dfInfo will map a Lattice* object to each outgoing or incoming PartEdge.
  // Returns true if the Lattices in dfInfo are modified and false otherwise.
  bool transferDFState(PartPtr part, CFGNode cn, SgNode* sgn, NodeState& state, std::map<PartEdgePtr, 
                       std::vector<Lattice*> >& dfInfo, const std::set<PartPtr>& ultimateParts);
  
  // Propagates the Lattice* mapped to different PartEdges in dfInfo along these PartEdges
  void propagateDF2Desc(PartPtr part, 
                        bool modified, 
                        // Set of all the Parts that have already been visited by the analysis
                        std::set<PartPtr>& visited,
                        // Set of all the Parts that have been initialized
                        std::set<PartPtr>& initialized,
                        // The dataflow iterator that identifies the state of the iteration
                        dataflowPartEdgeIterator* curNodeIt,
                        std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo, 
                        // anchor that denotes the current abstract state in the debug output
                        anchor curPartAnchor,
                        // graph widget that visualizes the flow of the worklist algorithm
                        graph& worklistGraph,
                        // Maps each Abstract State to the anchors of outgoing links that target it from the last visit to its predecessors
                        std::map<PartPtr, std::set<anchor> >& toAnchors,
                        // Maps each Abstract state to the anchors of the AStates that lead to it, as well as the AStates themselves
                        std::map<PartPtr, std::set<std::pair<anchor, PartPtr> > >& fromAnchors);
  
  // Generates the initial lattice state for the given dataflow node. Implementations 
  // fill in the lattices above and below this part, as well as the facts, as needed. Since in many cases
  // the lattices above and below each node are the same, implementors can alternately implement the 
  // genInitLattice and genInitFact functions, which are called by the default implementation of initializeState.
  virtual void initializeState(PartPtr part, NodeState& state);
  
  // Initializes the state of analysis lattices at the given function, part and edge into our out of the part
  // by setting initLattices to refer to freshly-allocated Lattice objects.
  virtual void genInitLattice(PartPtr part, PartEdgePtr pedge, 
                             std::vector<Lattice*>& initLattices) {}
  
  // Initializes the state of analysis facts at the given function and part by setting initFacts to 
  // freshly-allocated Fact objects.
  virtual void genInitFact(PartPtr part, std::vector<NodeFact*>& initFacts) {}
  
  // propagates the dataflow info from the current node's NodeState (curNodeState) to the next node's
  // NodeState (nextNodeState)
  bool propagateStateToNextNode(
              std::map<PartEdgePtr, std::vector<Lattice*> >& curNodeState, PartPtr curDFNode,
              std::map<PartEdgePtr, std::vector<Lattice*> >& nextNodeState, PartPtr nextDFNode);

  //virtual NodeState*initializeFunctionNodeState(const Function &func, NodeState *fState) = 0;
  virtual std::set<PartPtr> getInitialWorklist() = 0;
  virtual std::map<PartEdgePtr, std::vector<Lattice*> >& getLatticeAnte(NodeState *state) = 0;
  virtual std::map<PartEdgePtr, std::vector<Lattice*> >& getLatticePost(NodeState *state) = 0;
  virtual void setLatticeAnte(NodeState *state, std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo, bool overwrite) = 0;
  virtual void setLatticePost(NodeState *state, std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo, bool overwrite) = 0;

  // If we're currently at a function call, use the associated inter-procedural
  // analysis to determine the effect of this function call on the dataflow state.
  //virtual void transferFunctionCall(const Function &caller, PartPtr callPart, CFGNode callCFG, NodeState *state) = 0;

  virtual std::list<PartPtr> getDescendants(PartPtr p) = 0;
  virtual std::list<PartEdgePtr> getEdgesToDescendants(PartPtr part) = 0;
  virtual std::set<PartPtr> getUltimate() = 0;
  virtual dataflowPartEdgeIterator* getIterator() = 0;
  
  // Remaps the given Lattice across the scope transition (if any) of the given edge, updating the lat vector
  // with pointers to the updated Lattice objects and deleting old Lattice objects as needed.
  virtual void remapML(PartEdgePtr fromPEdge, std::vector<Lattice*>& lat)=0;
  
  virtual direction getDirection() = 0;
};

/* Forward Dataflow Analysis */
class FWDataflow  : public ComposedAnalysis
{
  public:
  
  FWDataflow()
  {}
  
  //NodeState* initializeFunctionNodeState(const Function &func, NodeState *fState);
  std::set<PartPtr> getInitialWorklist();
  std::map<PartEdgePtr, std::vector<Lattice*> >& getLatticeAnte(NodeState *state);
  std::map<PartEdgePtr, std::vector<Lattice*> >& getLatticePost(NodeState *state);
  void setLatticeAnte(NodeState *state, std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo, bool overwrite);
  void setLatticePost(NodeState *state, std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo, bool overwrite);
  
  //void transferFunctionCall(const Function &func, PartPtr callPart, CFGNode callCFG, NodeState *state);
  std::list<PartPtr> getDescendants(PartPtr p);
  std::list<PartEdgePtr> getEdgesToDescendants(PartPtr part);
  std::set<PartPtr> getUltimate();
  dataflowPartEdgeIterator* getIterator();
  
  // Remaps the given Lattice across the scope transition (if any) of the given edge, updating the lat vector
  // with pointers to the updated Lattice objects and deleting old Lattice objects as needed.
  void remapML(PartEdgePtr fromPEdge, std::vector<Lattice*>& lat);
  
  direction getDirection() { return fw; }
};

/* Backward Dataflow Analysis */
class BWDataflow  : public ComposedAnalysis
{
  public:
  
  BWDataflow()
  {}
  
  //NodeState* initializeFunctionNodeState(const Function &func, NodeState *fState);
  std::set<PartPtr> getInitialWorklist();
  std::map<PartEdgePtr, std::vector<Lattice*> >& getLatticeAnte(NodeState *state);
  std::map<PartEdgePtr, std::vector<Lattice*> >& getLatticePost(NodeState *state);
  void setLatticeAnte(NodeState *state, std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo, bool overwrite);
  void setLatticePost(NodeState *state, std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo, bool overwrite);
  //void transferFunctionCall(const Function &func, PartPtr callPart, CFGNode callCFG, NodeState *state);
  std::list<PartPtr> getDescendants(PartPtr p);
  std::list<PartEdgePtr> getEdgesToDescendants(PartPtr part);
  std::set<PartPtr> getUltimate();
  dataflowPartEdgeIterator* getIterator();
  
  // Remaps the given Lattice across the scope transition (if any) of the given edge, updating the lat vector
  // with pointers to the updated Lattice objects and deleting old Lattice objects as needed.
  void remapML(PartEdgePtr fromPEdge, std::vector<Lattice*>& lat);
  
  direction getDirection() { return bw; }
};


/* Dataflow Analysis that doesn't have a direction but is still a compositional
   analysis (e.g. Syntactic analysis or orthogonal array analysis)*/
class UndirDataflow  : public ComposedAnalysis
{
  public:
  
  UndirDataflow()
  {}
  
  //NodeState* initializeFunctionNodeState(const Function &func, NodeState *fState) { return NULL; }
  std::set<PartPtr> getInitialWorklist() { return std::set<PartPtr>(); }
  static std::map<PartEdgePtr, std::vector<Lattice*> > emptyMap;
  std::map<PartEdgePtr, std::vector<Lattice*> >& getLatticeAnte(NodeState *state) { return emptyMap; }
  std::map<PartEdgePtr, std::vector<Lattice*> >& getLatticePost(NodeState *state) { return emptyMap; }
  void setLatticeAnte(NodeState *state, std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo, bool overwrite) { }
  void setLatticePost(NodeState *state, std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo, bool overwrite) { }
  //void transferFunctionCall(const Function &func, PartPtr callPart, CFGNode callCFG, NodeState *state) {};
  std::list<PartPtr> getDescendants(PartPtr p) { std::list<PartPtr> empty; return empty; }
  std::list<PartEdgePtr> getEdgesToDescendants(PartPtr part) { std::list<PartEdgePtr> empty; return empty; }
  std::set<PartPtr> getUltimate() { return std::set<PartPtr>(); } 
  dataflowPartEdgeIterator* getIterator() { return NULL; }
  
  // Remaps the given Lattice across the scope transition (if any) of the given edge, updating the lat vector
  // with pointers to the updated Lattice objects and deleting old Lattice objects as needed.
  void remapML(PartEdgePtr fromPEdge, std::vector<Lattice*>& lat) { }
  
  direction getDirection() { return none; }
  
  // Dummy transfer function since undirected analyses does not propagate flow information
  bool transfer(PartPtr p, CFGNode cn, NodeState& state, std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo) {
    return true;
  }
};


// #######################################
// ##### UTILITY PASSES and ANALYSES #####
// #######################################

/******************************************************
 ***            printDataflowInfoPass               ***
 *** Prints out the dataflow information associated ***
 *** with a given analysis for every CFG node a     ***
 *** function.                                      ***
 ******************************************************/
class printDataflowInfoPass : public FWDataflow
{
  Analysis* analysis;

  public:
  printDataflowInfoPass(Analysis *analysis)
  {
          this->analysis = analysis;
  }

  // Initializes the state of analysis lattices, for analyses that produce the same lattices above and below each node
  void genInitLattice(PartPtr part, PartEdgePtr pedge, std::vector<Lattice*>& initLattices);

  bool transfer(PartPtr p, CFGNode cn, NodeState& state, 
                std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo);

  // pretty print for the object
  std::string str(std::string indent="")
  { return "printDataflowInfoPass"; }
};

/***************************************************
 ***            checkDataflowInfoPass            ***
 *** Checks the results of the composed analysis ***
 *** chain at special assert calls.              ***
 ***************************************************/
class checkDataflowInfoPass : public FWDataflow
{
  private:
  int numErrors;

  public:
  checkDataflowInfoPass() : numErrors(0) { }
  checkDataflowInfoPass(int numErrors): numErrors(numErrors) { }
  
  // Returns a shared pointer to a freshly-allocated copy of this ComposedAnalysis object
  ComposedAnalysisPtr copy() { return boost::make_shared<checkDataflowInfoPass>(numErrors); }

  int getNumErrors() const { return numErrors; }

  // Initializes the state of analysis lattices, for analyses that produce the same lattices above and below each node
  void genInitLattice(PartPtr part, PartEdgePtr pedge, std::vector<Lattice*>& initLattices);

  bool transfer(PartPtr p, CFGNode cn, NodeState& state, 
                std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo);

  // pretty print for the object
  std::string str(std::string indent="")
  { return "checkDataflowInfoPass"; }
};


class InitDataflowState : public UnstructuredPassAnalysis
{
  ComposedAnalysis::direction dir;
  
  public:
  InitDataflowState(ComposedAnalysis* analysis, ComposedAnalysis::direction dir) : UnstructuredPassAnalysis(analysis), dir(dir)
  { }

  void visit(PartPtr p, NodeState& state);
};

} // namespace fuse;
#endif
