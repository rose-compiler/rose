#ifndef COMPOSE_H
#define COMPOSE_H

#include "analysis.h"
#include "abstract_object.h"
#include "partitions.h"
#include "graphIterator.h"
#include "composed_analysis.h"
/* GB 2012-09-02: DESIGN NOTE
   Analyses and abstract objects written for the compositional framework must support a wide range of functionality,
   both mandatory (e.g. mayEqual/mustEqual in AbstractObjects) and optional (e.g. Expr2MemLoc() in ComposedAnalysis).
   In general optional functionality should go into the composer, which will then find an implementor for this 
 * functionality. Mandatory functionality should be placed as body-less virtual methods that are required for specific
 * instances of AbstractObject and ComposedAnalysis. Note that although we do currently have a way for ComposedAnalyses
 * to provide optional functionality and have the Composer find it, the same is not true for AbstractObjects because
 * thus far we have not found a use-case where we wanted AbstractObject functionality to be optional. This issue
 * may need to be revisited.
*/

// ------------------------------
// ----- Composition Driver -----
// ------------------------------

namespace fuse {
extern int composerDebugLevel;
class Composer;

// Represents the state of our knowledge about some fact
typedef enum {Unknown=-1, False=0, True=1} knowledgeT;

// #####################
// ##### COMPOSERS #####
// #####################

class Composer
{
  public:
    Composer();
    
  // Abstract interpretation functions that return this analysis' abstractions that 
  // represent the outcome of the given SgExpression at the end of all execution prefixes
  // that terminate at PartEdge pedge
  // The objects returned by these functions are expected to be deallocated by their callers.

  // Analyses that are being composed inside a given composer provide a pointer to themselves
  // in the client argument. Code that uses the composer from the outside, does not need to provide
  // a client.
    
  virtual ValueObjectPtr       Expr2Val(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client=NULL)=0;
  // Variant of Expr2Val that inquires about the value of the memory location denoted by the operand of the 
  // given node n, where the part edge denotes the set of execution prefixes that terminate at SgNode n.
  virtual ValueObjectPtr OperandExpr2Val(SgNode* n, SgNode* operand, PartEdgePtr pedge, ComposedAnalysis* client=NULL)=0;

  virtual MemLocObjectPtr  Expr2MemLoc(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client=NULL)=0;
  // Variant of Expr2MemLoc that inquires about the memory location denoted by the operand of the given node n, where
  // the part denotes the set of prefixes that terminate at SgNode n.
  virtual MemLocObjectPtr OperandExpr2MemLoc(SgNode* n, SgNode* operand, PartEdgePtr pedge, ComposedAnalysis* client=NULL)=0;
  // #SA: Variant of Expr2MemLoc for an analysis to call its own Expr2MemLoc method to interpret complex expressions
  // Composer caches memory objects for the analysis
  virtual MemLocObjectPtr Expr2MemLocSelf(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* self)=0;

  virtual CodeLocObjectPtrPair Expr2CodeLoc(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client=NULL)=0;

  // Returns whether the given pair of AbstractObjects are may-equal at the given PartEdge
  // Wrapper for calling type-specific versions of isLive without forcing the caller to care about the type of objects.
  bool mayEqual(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client);

  // Special calls for each type of AbstractObject
  virtual bool mayEqualV (ValueObjectPtr  val1, ValueObjectPtr  val2, PartEdgePtr pedge, ComposedAnalysis* client) { throw NotImplementedException(); }
  virtual bool mayEqualML(MemLocObjectPtr  ml1, MemLocObjectPtr  ml2, PartEdgePtr pedge, ComposedAnalysis* client) { throw NotImplementedException(); }
  virtual bool mayEqualCL(CodeLocObjectPtr cl1, CodeLocObjectPtr cl2, PartEdgePtr pedge, ComposedAnalysis* client) { throw NotImplementedException(); }

  // Returns whether the given pair of AbstractObjects are must-equal at the given PartEdge
  // Wrapper for calling type-specific versions of isLive without forcing the caller to care about the type of object
  bool mustEqual(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client);

  // Special calls for each type of AbstractObject
  virtual bool mustEqualV (ValueObjectPtr  val1, ValueObjectPtr  val2, PartEdgePtr pedge, ComposedAnalysis* client) { throw NotImplementedException(); }
  virtual bool mustEqualML(MemLocObjectPtr  ml1, MemLocObjectPtr  ml2, PartEdgePtr pedge, ComposedAnalysis* client) { throw NotImplementedException(); }
  virtual bool mustEqualCL(CodeLocObjectPtr cl1, CodeLocObjectPtr cl2, PartEdgePtr pedge, ComposedAnalysis* client) { throw NotImplementedException(); }

  // Returns whether the two abstract objects denote the same set of concrete objects
  virtual bool equalSet(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client)=0;
  
  // Returns whether abstract object ao1 denotes a non-strict subset (the sets may be equal) of the set denoted
  // by the abstract object ao2.
  virtual bool subSet(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client)=0;
  
  // Returns whether the given AbstractObject is live at the given PartEdge
  // Wrapper for calling type-specific versions of isLive without forcing the caller to care about the type of object
  bool isLive(AbstractObjectPtr ao, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // Special calls for each type of AbstractObject
  virtual bool isLiveVal    (ValueObjectPtr val,  PartEdgePtr pedge, ComposedAnalysis* client)=0;
  virtual bool isLiveMemLoc (MemLocObjectPtr ml,  PartEdgePtr pedge, ComposedAnalysis* client)=0;
  virtual bool isLiveCodeLoc(CodeLocObjectPtr cl, PartEdgePtr pedge, ComposedAnalysis* client)=0;
  
  virtual bool OperandIsLiveVal    (SgNode* n, SgNode* operand, ValueObjectPtr val,  PartEdgePtr pedge, ComposedAnalysis* client)=0;
  virtual bool OperandIsLiveMemLoc (SgNode* n, SgNode* operand, MemLocObjectPtr ml,  PartEdgePtr pedge, ComposedAnalysis* client)=0;
  virtual bool OperandIsLiveCodeLoc(SgNode* n, SgNode* operand, CodeLocObjectPtr cl, PartEdgePtr pedge, ComposedAnalysis* client)=0;
  
  // Computes the meet of from and to and saves the result in to.
  // Returns true if this causes this to change and false otherwise.
  virtual bool meetUpdateVal    (ValueObjectPtr   to, ValueObjectPtr   from, PartEdgePtr pedge, ComposedAnalysis* analysis)=0;
  virtual bool meetUpdateMemLoc (MemLocObjectPtr  to, MemLocObjectPtr  from, PartEdgePtr pedge, ComposedAnalysis* analysis)=0;
  virtual bool meetUpdateCodeLoc(CodeLocObjectPtr to, CodeLocObjectPtr from, PartEdgePtr pedge, ComposedAnalysis* analysis)=0;

  // Returns whether the given AbstractObject corresponds to the set of all sub-executions or the empty set
  virtual bool isFull (AbstractObjectPtr ao, PartEdgePtr pedge, ComposedAnalysis* analysis)=0;
  virtual bool isEmpty(AbstractObjectPtr ao, PartEdgePtr pedge, ComposedAnalysis* analysis)=0;
  
  // Return the anchor Parts of a given function
  virtual std::set<PartPtr> GetStartAStates(ComposedAnalysis* client)=0;
  // There may be multiple terminal points in the application (multiple calls to exit(), returns from main(), etc.)
  virtual std::set<PartPtr> GetEndAStates(ComposedAnalysis* client)=0;
  
  /*
  // Returns whether dom is a dominator of part
  virtual std::set<PartPtr> isDominator(PartPtr part, PartPtr dom, ComposedAnalysis* client);
  
  // Map that maintains each part's dominator set. When a part is not mapped to any set, it is assumed that it
  // is mapped to the set of all parts
  std::map<PartPtr, std::set<PartPtr> > dominators;
  // The set of all parts
  set<PartPtr> allParts;
  // Flag that indicates whether the dominator-related data structures have been computed
  bool domInit;
  
  // Initializes the dominator-related data structures
  void initializeDominators(ComposedAnalysis* client);*/

  /* // Given a Part that this analysis implements returns the Part from the preceding analysis
  // that this Part corresponds to (we assume that each analysis creates one or more Parts and PartEdges
  // for each Part and PartEdge of its parent analysis)
  virtual PartPtr     sourcePart    (PartPtr part)=0;
  // Given a PartEdge that this analysis implements returns the PartEdge from the preceding analysis
  // that this PartEdge corresponds to (we assume that each analysis creates one or more Parts and PartEdges
  // for each Part and PartEdge of its parent analysis)
  virtual PartEdgePtr sourcePartEdge(PartEdgePtr pedge)=0;*/
};
typedef boost::shared_ptr<Composer> ComposerPtr;

// Classes FuncCaller and FuncCallerArgs wrap the functionality to call functions
// Expr2* and ComposerGetFunction*Part on analyses inside the ChainComposer. FuncCaller
// exposes the () operator that takes FuncCallerArgs as the argument. Specific implementations
// decide what function the () operator actually calls and what the arguments actually are
// but by abstracting these details away we can get a general algorithm for the ChainComposer to 
// choose the analysis that implements a given function.
/*class FuncCallerArgs : public dbglog::printable
{ 
  // Dummy virtual methods to allow dynamic casting on classes derived from FuncCallerArgs
  virtual void dummy() {}
};
*/
template<class RetObject, class ArgsObject>
class FuncCaller
{
  public:
  // Calls the implementation of some API operation inside server analysis on behalf of client analysis. 
  virtual RetObject operator()(const ArgsObject& args, PartEdgePtr pedge, ComposedAnalysis* server, ComposedAnalysis* client)=0;  
  // Returns a string representation of the returned object
  virtual std::string retStr(RetObject ml)=0;
  // Returns the name of the function being called, for debugging purposes
  virtual std::string funcName() const=0;
};

// Simple implementation of a Composer where the analyses form a linear sequence of 
// dependences
class ChainComposer : public Composer, public UndirDataflow
{
  std::list<ComposedAnalysis*> allAnalyses;
  std::list<ComposedAnalysis*> doneAnalyses;
  // The analysis that is currently executing
  ComposedAnalysis* currentAnalysis;
  // The optional pass that tests the results of the other analyses
  ComposedAnalysis* testAnalysis;
  // If true, the debug output of testAnalysis is emitted.
  bool verboseTest;
  
  public:
  // stxAnalysis - Points to the analysis that ChainComposer should run before any other to take application 
  //    information provided in its syntax and represent it using Fuse abstractions. If stxAnalysis==NULL,
  //    SyntacticAnalysis is used.
  ChainComposer(const std::list<ComposedAnalysis*>& analyses, 
                ComposedAnalysis* testAnalysis, bool verboseTest, 
                ComposedAnalysis* stxAnalysis=NULL);
  ChainComposer(const ChainComposer&);
  
  // Returns a shared pointer to a freshly-allocated copy of this ComposedAnalysis object
  ComposedAnalysisPtr copy() 
  { return boost::make_shared<ChainComposer>(*this); }
  
  private:
  // The types of functions we may be interested in calling
  typedef enum {any, memloc, codeloc, val, part} reqType;
  // Maps each client analysis and request type to the cached list of analyses 
  // that separate the client and the server in the composition chain, with the server
  // as the server at the front and client at the back.
  //static std::map<ComposedAnalysis*, std::map<reqType, std::list<ComposedAnalysis*> > > serverCache;
  // Maps each client analysis and request type to the cache number of analyses
  // that separate the client and the server in the composition chain (e.g. if they're
  // adjacent in the chain, the number is 0).
  //static std::map<ComposedAnalysis*, std::map<reqType, int > > serverCache;
  
  // Generic function that looks up the composition chain from the given client 
  // analysis and returns the result produced by the first instance of the function 
  // called by the caller object found along the way.
  template<class RetObject, class ArgsObject>
  RetObject callServerAnalysisFunc(ArgsObject& args, PartEdgePtr pedge, ComposedAnalysis* client, 
                                   FuncCaller<RetObject, ArgsObject>& caller, bool verbose=false);
  
  public:
  // Abstract interpretation functions that return this analysis' abstractions that 
  // represent the outcome of the given SgExpression. 
  // The objects returned by these functions are expected to be deallocated by their callers.
  ValueObjectPtr Expr2Val(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client);
  // Variant of Expr2Val that inquires about the value of the memory location denoted by the operand of the 
  // given node n, where the part denotes the set of prefixes that terminate at SgNode n.
  ValueObjectPtr OperandExpr2Val(SgNode* n, SgNode* operand, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // MemLocObjectPtrPair Expr2MemLoc(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client);
  MemLocObjectPtr Expr2MemLoc(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client);
  
  private:
  // MemLocObjectPtrPair Expr2MemLoc_ex(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client);
  MemLocObjectPtr Expr2MemLoc_ex(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client);
  
  public:
  // Variant of Expr2MemLoc that inquires about the memory location denoted by the operand of the given node n, where
  // the part denotes the set of prefixes that terminate at SgNode n.
  // MemLocObjectPtrPair OperandExpr2MemLoc(SgNode* n, SgNode* operand, PartEdgePtr pedge, ComposedAnalysis* client);
  MemLocObjectPtr OperandExpr2MemLoc(SgNode* n, SgNode* operand, PartEdgePtr pedge, ComposedAnalysis* client);

  // #SA: Variant of Expr2MemLoc called by an analysis to call its own Expr2MemLoc inorder
  // to interpret complex expressions
  MemLocObjectPtr Expr2MemLocSelf(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* self);
  
  CodeLocObjectPtrPair Expr2CodeLoc(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // Returns whether the given pair of AbstractObjects are may-equal at the given PartEdge
  bool mayEqualV (ValueObjectPtr  val1, ValueObjectPtr  val2, PartEdgePtr pedge, ComposedAnalysis* client);
  bool mayEqualML(MemLocObjectPtr  ml1, MemLocObjectPtr  ml2, PartEdgePtr pedge, ComposedAnalysis* client);
  bool mayEqualCL(CodeLocObjectPtr cl1, CodeLocObjectPtr cl2, PartEdgePtr pedge, ComposedAnalysis* client);

  // Returns whether the given pair of AbstractObjects are must-equal at the given PartEdge
  bool mustEqualV (ValueObjectPtr  val1, ValueObjectPtr  val2, PartEdgePtr pedge, ComposedAnalysis* client);
  bool mustEqualML(MemLocObjectPtr  ml1, MemLocObjectPtr  ml2, PartEdgePtr pedge, ComposedAnalysis* client);
  bool mustEqualCL(CodeLocObjectPtr cl1, CodeLocObjectPtr cl2, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // Returns whether the two abstract objects denote the same set of concrete objects
  bool equalSet(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // Returns whether abstract object ao1 denotes a non-strict subset (the sets may be equal) of the set denoted
  // by the abstract object ao2.
  bool subSet(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client);
  
  /*// Calls the isLive() method of the given MemLocObject that denotes an operand of the given SgNode n within
  // the context of its own PartEdges and returns true if it may be live within any of them
  bool OperandIsLive(SgNode* n, SgNode* operand, MemLocObjectPtr ml, PartEdgePtr pedge, ComposedAnalysis* client);*/
  
  // Returns whether the given AbstractObject is live at the given PartEdge
  bool isLiveVal    (ValueObjectPtr val,  PartEdgePtr pedge, ComposedAnalysis* client);
  bool isLiveMemLoc (MemLocObjectPtr ml,  PartEdgePtr pedge, ComposedAnalysis* client);
  bool isLiveCodeLoc(CodeLocObjectPtr cl, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // Calls the isLive() method of the given AbstractObject that denotes an operand of the given SgNode n within
  // the context of its own PartEdges and returns true if it may be live within any of them
  bool OperandIsLiveVal    (SgNode* n, SgNode* operand, ValueObjectPtr val,  PartEdgePtr pedge, ComposedAnalysis* client);
  bool OperandIsLiveMemLoc (SgNode* n, SgNode* operand, MemLocObjectPtr ml,  PartEdgePtr pedge, ComposedAnalysis* client);
  bool OperandIsLiveCodeLoc(SgNode* n, SgNode* operand, CodeLocObjectPtr cl, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // Computes the meet of from and to and saves the result in to.
  // Returns true if this causes this to change and false otherwise.
  bool meetUpdateVal    (ValueObjectPtr   to, ValueObjectPtr   from, PartEdgePtr pedge, ComposedAnalysis* analysis);
  bool meetUpdateMemLoc (MemLocObjectPtr  to, MemLocObjectPtr  from, PartEdgePtr pedge, ComposedAnalysis* analysis);
  bool meetUpdateCodeLoc(CodeLocObjectPtr to, CodeLocObjectPtr from, PartEdgePtr pedge, ComposedAnalysis* analysis);
  
  // Returns whether the given AbstractObject corresponds to the set of all sub-executions or the empty set
  bool isFull (AbstractObjectPtr ao, PartEdgePtr pedge, ComposedAnalysis* analysis);
  bool isEmpty(AbstractObjectPtr ao, PartEdgePtr pedge, ComposedAnalysis* analysis);
  
  // Return the anchor Parts of a given function
  std::set<PartPtr> GetStartAStates(ComposedAnalysis* client);
  std::set<PartPtr> GetEndAStates(ComposedAnalysis* client);
  
  // -----------------------------------------
  // ----- Methods from ComposedAnalysis -----
  // -----------------------------------------

  // Runs the analysis, combining the intra-analysis with the inter-analysis of its choice
  // ChainComposer invokes the runAnalysis methods of all its constituent analyses in sequence
  void runAnalysis();

  // The Expr2* and GetFunction*Part functions are implemented by calling the same functions in each of the
  // constituent analyses and returning an Intersection object that includes their responses

  // Abstract interpretation functions that return this analysis' abstractions that
  // represent the outcome of the given SgExpression. The default implementations of
  // these throw NotImplementedException so that if a derived class does not implement
  // any of these functions, the Composer is informed.
  //
  // The objects returned by these functions are expected to be deallocated by their callers.
  ValueObjectPtr   Expr2Val    (SgNode* n, PartEdgePtr pedge);
  MemLocObjectPtr  Expr2MemLoc (SgNode* n, PartEdgePtr pedge);
  CodeLocObjectPtr Expr2CodeLoc(SgNode* n, PartEdgePtr pedge);

  // Return true if the class implements Expr2* and false otherwise
  bool implementsExpr2Val    ();
  bool implementsExpr2MemLoc ();
  bool implementsExpr2CodeLoc();

  // Return the anchor Parts of a given function
  std::set<PartPtr> GetStartAStates_Spec();
  std::set<PartPtr> GetEndAStates_Spec();

  std::string str(std::string indent="");
};

// Composer that invokes multiple analyses in parallel (they do not interact) and runs them to completion independently.
// It also implements the ComposedAnalysis interface and can be used by another Composer as an analysis. Thus, when this
// Composer's constituent analyses ask a query on the composer, it merely forwards this query to its parent Composer.
// Further, when its parent Composer makes queries of it, this Composer forwards those queries to its constituent 
// analyses and returns an Intersection object that contains their responses.
class LooseParallelComposer : public Composer, public UndirDataflow
{
  std::list<ComposedAnalysis*> allAnalyses;
  
  // Indicates whether at least one sub-analysis implements a partition
  knowledgeT subAnalysesImplementPartitions;
  
  public:
  LooseParallelComposer(const std::list<ComposedAnalysis*>& analyses, knowledgeT subAnalysesImplementPartitions=Unknown);

  // ---------------------------------
  // ----- Methods from Composer -----
  // ---------------------------------
  
  // Returns a shared pointer to a freshly-allocated copy of this ComposedAnalysis object
  ComposedAnalysisPtr copy() { return boost::make_shared<LooseParallelComposer>(allAnalyses, subAnalysesImplementPartitions); }
  
  // The Expr2* and GetFunction*Part functions are implemented by calling the same functions in the parent composer
  
  // Abstract interpretation functions that return this analysis' abstractions that 
  // represent the outcome of the given SgExpression. 
  // The objects returned by these functions are expected to be deallocated by their callers.
  ValueObjectPtr Expr2Val(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client);
  // Variant of Expr2Val that inquires about the value of the memory location denoted by the operand of the 
  // given node n, where the part denotes the set of prefixes that terminate at SgNode n.
  ValueObjectPtr OperandExpr2Val(SgNode* n, SgNode* operand, PartEdgePtr pedge, ComposedAnalysis* client);
  
  //MemLocObjectPtrPair Expr2MemLoc(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client);
  MemLocObjectPtr Expr2MemLoc(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // Variant of Expr2MemLoc that inquires about the memory location denoted by the operand of the given node n, where
  // the part denotes the set of prefixes that terminate at SgNode n.
  //MemLocObjectPtrPair OperandExpr2MemLoc(SgNode* n, SgNode* operand, PartEdgePtr pedge, ComposedAnalysis* client);
  MemLocObjectPtr OperandExpr2MemLoc(SgNode* n, SgNode* operand, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // #SA: Variant of Expr2MemLoc for an analysis to call its own Expr2MemLoc method to interpret complex expressions
  // Composer caches memory objects for the analysis
  MemLocObjectPtr Expr2MemLocSelf(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* self);
  
  CodeLocObjectPtrPair Expr2CodeLoc(SgNode* n, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // Returns whether the given pair of AbstractObjects are may-equal at the given PartEdge
  bool mayEqualV (ValueObjectPtr  val1, ValueObjectPtr  val2, PartEdgePtr pedge, ComposedAnalysis* client);
  bool mayEqualML(MemLocObjectPtr  ml1, MemLocObjectPtr  ml2, PartEdgePtr pedge, ComposedAnalysis* client);
  bool mayEqualCL(CodeLocObjectPtr cl1, CodeLocObjectPtr cl2, PartEdgePtr pedge, ComposedAnalysis* client);

  // Returns whether the given pair of AbstractObjects are must-equal at the given PartEdge
  bool mustEqualV (ValueObjectPtr  val1, ValueObjectPtr  val2, PartEdgePtr pedge, ComposedAnalysis* client);
  bool mustEqualML(MemLocObjectPtr  ml1, MemLocObjectPtr  ml2, PartEdgePtr pedge, ComposedAnalysis* client);
  bool mustEqualCL(CodeLocObjectPtr cl1, CodeLocObjectPtr cl2, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // Returns whether the two abstract objects denote the same set of concrete objects
  bool equalSet(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // Returns whether abstract object ao1 denotes a non-strict subset (the sets may be equal) of the set denoted
  // by the abstract object ao2.
  bool subSet(AbstractObjectPtr ao1, AbstractObjectPtr ao2, PartEdgePtr pedge, ComposedAnalysis* client);
  
  /*// Calls the isLive() method of the given MemLocObject that denotes an operand of the given SgNode n within
  // the context of its own PartEdges and returns true if it may be live within any of them
  bool OperandIsLive(SgNode* n, SgNode* operand, MemLocObjectPtr ml, PartEdgePtr pedge, ComposedAnalysis* client);*/
  
  // Returns whether the given AbstractObject is live at the given PartEdge
  bool isLiveVal    (ValueObjectPtr val,  PartEdgePtr pedge, ComposedAnalysis* client);
  bool isLiveMemLoc (MemLocObjectPtr ml,  PartEdgePtr pedge, ComposedAnalysis* client);
  bool isLiveCodeLoc(CodeLocObjectPtr cl, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // Calls the isLive() method of the given AbstractObject that denotes an operand of the given SgNode n within
  // the context of its own PartEdges and returns true if it may be live within any of them
  bool OperandIsLiveVal    (SgNode* n, SgNode* operand, ValueObjectPtr val,  PartEdgePtr pedge, ComposedAnalysis* client);
  bool OperandIsLiveMemLoc (SgNode* n, SgNode* operand, MemLocObjectPtr ml,  PartEdgePtr pedge, ComposedAnalysis* client);
  bool OperandIsLiveCodeLoc(SgNode* n, SgNode* operand, CodeLocObjectPtr cl, PartEdgePtr pedge, ComposedAnalysis* client);
  
  // Computes the meet of from and to and saves the result in to.
  // Returns true if this causes this to change and false otherwise.
  bool meetUpdateVal    (ValueObjectPtr   to, ValueObjectPtr   from, PartEdgePtr pedge, ComposedAnalysis* analysis);
  bool meetUpdateMemLoc (MemLocObjectPtr  to, MemLocObjectPtr  from, PartEdgePtr pedge, ComposedAnalysis* analysis);
  bool meetUpdateCodeLoc(CodeLocObjectPtr to, CodeLocObjectPtr from, PartEdgePtr pedge, ComposedAnalysis* analysis);
  
  // Returns whether the given AbstractObject corresponds to the set of all sub-executions or the empty set
  bool isFull (AbstractObjectPtr ao, PartEdgePtr pedge, ComposedAnalysis* analysis);
  bool isEmpty(AbstractObjectPtr ao, PartEdgePtr pedge, ComposedAnalysis* analysis);
  
  // Return the anchor Parts of a given function
  std::set<PartPtr> GetStartAStates(ComposedAnalysis* client);
  std::set<PartPtr> GetEndAStates(ComposedAnalysis* client);
  
  // Common functionality for GetStartAStates_Spec() and GetEndAStates_Spec()
  
  // Functors that call either GetStartStates or GetEndStates in GetStartOrEndStates_Spec
  class callStartOrEndAStates {
    public:
    virtual std::set<PartPtr> callGetStartOrEndAStates_ComposedAnalysis(ComposedAnalysis* analysis)=0;
    virtual std::set<PartPtr> callGetStartOrEndAStates_Composer(Composer* composer, ComposedAnalysis* analysis)=0;
  };
  
  std::set<PartPtr> GetStartOrEndAStates_Spec(callStartOrEndAStates& caller, std::string funcName);
  
  // -----------------------------------------
  // ----- Methods from ComposedAnalysis -----
  // -----------------------------------------
  
  // Runs the analysis, combining the intra-analysis with the inter-analysis of its choice
  // LooseParallelComposer invokes the runAnalysis methods of all its constituent analyses in sequence
  void runAnalysis();
  
  // The Expr2* and GetFunction*Part functions are implemented by calling the same functions in each of the 
  // constituent analyses and returning an Intersection object that includes their responses
  
  // Abstract interpretation functions that return this analysis' abstractions that 
  // represent the outcome of the given SgExpression. The default implementations of 
  // these throw NotImplementedException so that if a derived class does not implement 
  // any of these functions, the Composer is informed.
  //
  // The objects returned by these functions are expected to be deallocated by their callers.
  ValueObjectPtr   Expr2Val    (SgNode* n, PartEdgePtr pedge);
  MemLocObjectPtr  Expr2MemLoc (SgNode* n, PartEdgePtr pedge);
  CodeLocObjectPtr Expr2CodeLoc(SgNode* n, PartEdgePtr pedge);
  
  // Return true if the class implements Expr2* and false otherwise
  bool implementsExpr2Val    ();
  bool implementsExpr2MemLoc ();
  bool implementsExpr2CodeLoc();
  
  // Return the anchor Parts of a given function
  std::set<PartPtr> GetStartAStates_Spec();
  std::set<PartPtr> GetEndAStates_Spec();
  
  // When Expr2* is queried for a particular analysis on edge pedge, exported by this LooseParallelComposer 
  // this function translates from the pedge that the LooseParallelComposer::Expr2* is given to the PartEdge 
  // that this particular sub-analysis runs on. If some of the analyses that were composed in parallel with 
  // this analysis (may include this analysis) implement partition graphs, we know that 
  // GetStartAState/GetEndAStates wrapped them in IntersectionPartEdges. In this case this function
  // converts pedge into an IntersectionPartEdge and queries its getPartEdge method. Otherwise, 
  // GetStartAState/GetEndAStates do no wrapping and thus, we can return pedge directly.
  PartEdgePtr getEdgeForAnalysis(PartEdgePtr pedge, ComposedAnalysis* analysis);
  
  std::string str(std::string indent="");
};

}; // namespace fuse

#endif
