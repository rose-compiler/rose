#ifndef STX_ANALYSIS_H
#define STX_ANALYSIS_H

#include "partitions.h"
#include "abstract_object.h"
#include "compose.h"
#include "CallGraphTraverse.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>

namespace fuse {
  
extern int stxAnalysisDebugLevel;
/*****************************
 ***** SyntacticAnalysis *****
 *****************************/

// This is a non-dataflow analysis that considers just the syntactic information 
// available about the memory locations, values and control locations at at CFG node. 
// It can precisely interpret static variables and lexical values but provides no 
// information that cannot be directly derived from the text of the code such as 
// the referents of pointers.

/********************
 ***** ANALYSIS *****
 ********************/

class SyntacticAnalysis : virtual public UndirDataflow
{
  private:
  static boost::shared_ptr<SyntacticAnalysis> _instance;

  public:
  SyntacticAnalysis() {}
  static SyntacticAnalysis* instance();
  
  // Returns a shared pointer to a freshly-allocated copy of this ComposedAnalysis object
  ComposedAnalysisPtr copy();

  void runAnalysis(const Function&  func, NodeState* state, bool, std::set<Function>) { }
  
  // The genInitLattice, genInitFact and transfer functions are not implemented since this 
  // is not a dataflow analysis.
  
  // Maps the given SgNode to an implementation of the ValueObject abstraction.
  ValueObjectPtr   Expr2Val(SgNode* e, PartEdgePtr pedge);
  static ValueObjectPtr   Expr2ValStatic(SgNode* e, PartEdgePtr pedge);
  bool implementsExpr2Val() { return true; }
  
  // Maps the given SgNode to an implementation of the MemLocObject abstraction.
  MemLocObjectPtr Expr2MemLoc(SgNode* e, PartEdgePtr pedge);
  static MemLocObjectPtr Expr2MemLocStatic(SgNode* e, PartEdgePtr pedge);
  bool implementsExpr2MemLoc() { return true; }
  
  // Maps the given SgNode to an implementation of the Expr2CodeLoc abstraction.
  CodeLocObjectPtr Expr2CodeLoc(SgNode* e, PartEdgePtr pedge);
  static CodeLocObjectPtr Expr2CodeLocStatic(SgNode* e, PartEdgePtr pedge);
  bool implementsExpr2CodeLoc() { return true; }
  
  // Return the anchor Parts of a given function
  std::set<PartPtr> GetStartAStates_Spec();
  std::set<PartPtr> GetEndAStates_Spec();
  
  // pretty print for the object
  std::string str(std::string indent="")
  { return "SyntacticAnalysis"; }
};

/**********************
 ***** PARTITIONS *****
 **********************/

class StxPart;
class StxPartEdge;

// A NULL CFGNode that is used as a wild-card for termination points of edges to/from anywhere
extern CFGNode NULLCFGNode;

//typedef boost::shared_ptr<StxPart> StxPartPtr;
//typedef boost::shared_ptr<StxPartEdge> StxPartEdgePtr;
typedef CompSharedPtr<StxPart> StxPartPtr;
typedef CompSharedPtr<StxPartEdge> StxPartEdgePtr;

// Returns the set of all the function calls that may call the given function
const std::set<SgFunctionCallExp*>& func2Calls(Function func);

class StxFuncContext: public PartContext
{
  Function func;
  CFGNode n;
  public:
  //StxFuncContext(Function func);
  StxFuncContext(CFGNode n);
    
  // Returns a list of PartContextPtr objects that denote more detailed context information about
  // this PartContext's internal contexts. If there aren't any, the function may just return a list containing
  // this PartContext itself.
  std::list<PartContextPtr> getSubPartContexts() const;
  
  bool operator==(const PartContextPtr& that) const;
  bool operator< (const PartContextPtr& that) const;
  
  std::string str(std::string indent="");
};
typedef CompSharedPtr<StxFuncContext> StxFuncContextPtr;

class StxPart : public Part
{
  CFGNode n;
  bool (*filter) (CFGNode cfgn); // a filter function to decide which raw CFG node to show (if return true) or hide (otherwise)
  
  friend class StxPartEdge;
  
  public:
  StxPart(CFGNode n, ComposedAnalysis* analysis, bool (*f) (CFGNode) = defaultFilter): 
    Part(analysis, NULLPart, makePtr<StxFuncContext>(n)), n(n), filter(f) {}
  StxPart(const StxPart& part):    Part((const Part&)part), n(part.n), filter(part.filter) {} 
  StxPart(const StxPartPtr& part): Part((const Part&)part), n(part->n), filter(part->filter) {} 
  StxPart(const StxPart& part,    bool (*f) (CFGNode) = defaultFilter): Part((const Part&)part), n(part.n), filter (f) {}
  StxPart(const StxPartPtr& part, bool (*f) (CFGNode) = defaultFilter): Part((const Part&)part), n(part->n), filter (f) {}
  
  private:
  
  std::map<StxPartEdgePtr, bool> getOutEdges();
  
  public:
  std::list<PartEdgePtr>    outEdges();
  std::list<StxPartEdgePtr> outStxEdges();
  
  private:
  std::map<StxPartEdgePtr, bool> getInEdges();
  
  public:
  std::list<PartEdgePtr>    inEdges();
  std::list<StxPartEdgePtr> inStxEdges();
  std::set<CFGNode>  CFGNodes() const;
  
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
  std::list<PartPtr> getOperandPart(SgNode* anchor, SgNode* operand);*/
  
  // Returns a PartEdgePtr, where the source is a wild-card part (NULLPart) and the target is this Part
  PartEdgePtr inEdgeFromAny();
  // Returns a PartEdgePtr, where the target is a wild-card part (NULLPart) and the source is this Part
  PartEdgePtr outEdgeToAny();
  
  bool equal(const PartPtr& o) const;
  bool less (const PartPtr& o) const;
  
  std::string str(std::string indent="");
};
extern StxPartPtr NULLStxPart;

class StxPartEdge : public PartEdge
{
  CFGPath p;
  bool (*filter) (CFGNode cfgn);

  public:
  StxPartEdge(CFGNode src, CFGNode tgt, ComposedAnalysis* analysis, bool (*f) (CFGNode) = defaultFilter):
      PartEdge(analysis, NULLPartEdge), p(CFGEdge(src, tgt)), filter(f) {}
  StxPartEdge(CFGPath p, ComposedAnalysis* analysis, bool (*f) (CFGNode) = defaultFilter):
      PartEdge(analysis, NULLPartEdge), p(p), filter(f) {}
  StxPartEdge(const StxPartEdge& dfe): PartEdge((const PartEdge&)dfe), p(dfe.p), filter(dfe.filter) {}
  
  PartPtr source() const;
  StxPartPtr stxSource() const;
  PartPtr target() const;
  StxPartPtr stxTarget() const;
  
  CFGPath getPath() const { return p; }
  
  // Let A={ set of execution prefixes that terminate at the given anchor SgNode }
  // Let O={ set of execution prefixes that terminate at anchor's operand SgNode }
  // Since to reach a given SgNode an execution must first execute all of its operands it must
  //    be true that there is a 1-1 mapping m() : O->A such that o in O is a prefix of m(o).
  // This function is the inverse of m: given the anchor node and operand as well as the
  //    PartEdge that denotes a subset of A (the function is called on this PartEdge), 
  //    it returns a list of PartEdges that partition O.
  std::list<PartEdgePtr> getOperandPartEdge(SgNode* anchor, SgNode* operand);
  
  // If the source Part corresponds to a conditional of some sort (if, switch, while test, etc.)
  // it must evaluate some predicate and depending on its value continue, execution along one of the
  // outgoing edges. The value associated with each outgoing edge is fixed and known statically.
  // getPredicateValue() returns the value associated with this particular edge. Since a single 
  // Part may correspond to multiple CFGNodes getPredicateValue() returns a map from each CFG node
  // within its source part that corresponds to a conditional to the value of its predicate along 
  // this edge.
  std::map<CFGNode, boost::shared_ptr<SgValueExp> > getPredicateValue();
  
  bool equal(const PartEdgePtr& o) const;
  bool less (const PartEdgePtr& o) const;
  
  std::string str(std::string indent="");
};
extern StxPartEdgePtr NULLStxPartEdge;

/****************************
 ***** ABSTRACT OBJECTS *****
 ****************************/

class StxValueObject : public ValueObject
{
  public:
  SgValueExp* val;
  
  StxValueObject(SgNode* n);
  StxValueObject(const StxValueObject& that);
  
  bool mayEqualV(ValueObjectPtr o, PartEdgePtr pedge);
  bool mustEqualV(ValueObjectPtr o, PartEdgePtr pedge);
  
  // Returns whether the two abstract objects denote the same set of concrete objects
  bool equalSet(AbstractObjectPtr o, PartEdgePtr pedge);
  
  // Returns whether this abstract object denotes a non-strict subset (the sets may be equal) of the set denoted
  // by the given abstract object.
  bool subSet(AbstractObjectPtr o, PartEdgePtr pedge);
  
  // Returns true if this object is live at the given part and false otherwise
  /*bool isLive(PartEdgePtr pedge) const
  { return true; }*/
  
  // Computes the meet of this and that and saves the result in this.
  // Returns true if this causes this to change and false otherwise.
  bool meetUpdateV(ValueObjectPtr that, PartEdgePtr pedge);

  bool isFull(PartEdgePtr pedge);
  bool isEmpty(PartEdgePtr pedge);
  
  // Returns true if the given pair of SgValueExps represent the same value and false otherwise
  static bool equalValExp(SgValueExp* a, SgValueExp* b);

  // Returns true if this ValueObject corresponds to a concrete value that is statically-known
  bool isConcrete();
  // Returns the type of the concrete value (if there is one)
  SgType* getConcreteType();
  // Returns the concrete value (if there is one) as an SgValueExp, which allows callers to use
  // the normal ROSE mechanisms to decode it
  std::set<boost::shared_ptr<SgValueExp> > getConcreteValue();
  
  std::string str(std::string indent); // pretty print for the object
    
  // Allocates a copy of this object and returns a pointer to it
  ValueObjectPtr copyV() const;
};
typedef boost::shared_ptr<StxValueObject> StxValueObjectPtr;

class StxCodeLocObject : public CodeLocObject
{
  public:
  PartEdgePtr pedge;
  SgExpression* code;
  
  StxCodeLocObject(SgNode* n, PartEdgePtr pedge);
  StxCodeLocObject(const StxCodeLocObject& that);
  
  bool mayEqualCL(CodeLocObjectPtr o, PartEdgePtr pedge);
  bool mustEqualCL(CodeLocObjectPtr o, PartEdgePtr pedge);
  
  // Returns whether the two abstract objects denote the same set of concrete objects
  bool equalSet(AbstractObjectPtr o, PartEdgePtr pedge);
  
  // Returns whether this abstract object denotes a non-strict subset (the sets may be equal) of the set denoted
  // by the given abstract object.
  bool subSet(AbstractObjectPtr o, PartEdgePtr pedge);
  
  // Returns true if this object is live at the given part and false otherwise
  // We don't currently support scope analysis for CodeLocObjects, so we default to them all being live.
  bool isLiveCL(PartEdgePtr pedge)
  { return true; }
  
  // Computes the meet of this and that and saves the result in this
  // returns true if this causes this to change and false otherwise
  bool meetUpdateCL(CodeLocObjectPtr that, PartEdgePtr pedge);

  bool isFull(PartEdgePtr pedge);
  bool isEmpty(PartEdgePtr pedge);
  
  /* Don't have good idea how to represent a finite number of options 
  bool isFiniteSet();
  std::set<AbstractObj> getValueSet();*/
   
  std::string str(std::string indent); // pretty print for the object
  
  // Allocates a copy of this object and returns a pointer to it
  CodeLocObjectPtr copyCL() const;
};
typedef boost::shared_ptr<StxCodeLocObject> StxCodeLocObjectPtr;

/******************************************
 ***** ABSTRACT OBJECT IMPLEMENTATION *****
 ******************************************/

  // One simple implementation for abstract memory object
  // This implementation is tied to the ROSE AST (another set of base classes such as expression,
  // named, and aliased objects)
  //
  // Big picture of this implementation
  // How the multiple inheritance results in the classes here
  //
  //                      Scalar            LabeledAggregate        Array       Pointer  Function
  // -----------------------------------------------------------------------------
  // Expression Objects | ScalarExp          LabAggreExp 
  // Named objects      | ScalarNamedObj    
  // Aliased Objects    | ScalarAliasedObj  LabeledAggregateAliasedObj ArrayAliasedObj  PointerAliasedObj

  // Parent class of sub-types of StxMemLocObjects: NamedObj, ExprObj and AliasedObj
  class NamedObj;
  class ExprObj;
  class AliasedObj;
  class StxMemLocObjectKind;
  typedef boost::shared_ptr<StxMemLocObjectKind> StxMemLocObjectKindPtr;
  class StxMemLocObjectKind: public virtual MemLocObject
  {
  public:
    StxMemLocObjectKind(SgNode* n): MemLocObject(n) {} 
    StxMemLocObjectKind(const StxMemLocObjectKind& that): MemLocObject((const MemLocObject&)that) {} 
    // Casts this object to a NamedObj, ExprObj or AliasedObj, returning 
    // the pointer if it is one of these kinds or NULL otherwise
    NamedObj*   isNamedObj();
    ExprObj*    isExprObj();
    AliasedObj* isAliasedObj();
    
    /*// Returns whether this object may/must be equal to o within the given Part p
    // These methods are called by composers and should not be called by analyses.
    bool mayEqualML (StxMemLocObjectPtr that, PartEdgePtr pedge);
    bool mustEqualML(StxMemLocObjectPtr that, PartEdgePtr pedge);
    
    // Returns true if this object is live at the given part and false otherwise.
    // This method is called by composers and should not be called by analyses.
    virtualbool isLiveML(PartEdgePtr pedge);*/
    
    // Allocates a copy of this object and returns a pointer to it
    MemLocObjectPtr copyML() const { return copyMLK(); }
    
    // Allocates a copy of this object and returns a pointer to it
    virtual StxMemLocObjectKindPtr copyMLK() const=0;
    
    // Computes the meet of this and that and saves the result in this
    // returns true if this causes this to change and false otherwise
    bool meetUpdateML(MemLocObjectPtr that, PartEdgePtr pedge) { assert(0); return false; }
    
    // Returns the type of the MemLoc this object denotes
    SgType* getType() const;
    
    //std::string strp(PartEdgePtr pedge, std::string indent="");
    
    // A dummy virtual destructor to enable dynamic casting
    virtual ~StxMemLocObjectKind() {}
  };
  
  
  class StxMemLocObject;
  typedef boost::shared_ptr<StxMemLocObject> StxMemLocObjectPtr;
  class StxMemLocObject: public virtual MemLocObject
  {
    protected:
    SgType* type;
    StxMemLocObjectKindPtr kind;
    
    public:
    StxMemLocObject(SgNode* n, SgType* t, StxMemLocObjectKindPtr kind);
    
    // equal() should be called by mayEqualML and mustEqualML of any derived classes
    // to ensure that the in-scope or out-of-scope issues are taken into account. 
    // If equal() returns defEqual or disjSet then mayEqualML and mustEqualML should 
    // return true and false, respectively. If equal() returns mayOverlap, mayEqualML and 
    // mustEqualML should continue more refined processing.
    typedef enum {defEqual, disjSet, mayOverlap} eqType;
    eqType equal(StxMemLocObjectPtr that_arg, PartEdgePtr pedge);
    
    // Returns whether this object may/must be equal to o within the given Part p
    // These methods are called by composers and should not be called by analyses.
    bool mayEqualML(MemLocObjectPtr o, PartEdgePtr pedge);
    bool mustEqualML(MemLocObjectPtr o, PartEdgePtr pedge);
    
    // Returns whether the two abstract objects denote the same set of concrete objects
    bool equalSet(AbstractObjectPtr o, PartEdgePtr pedge);
    
    // Returns whether this abstract object denotes a non-strict subset (the sets may be equal) of the set denoted
    // by the given abstract object.
    bool subSet(AbstractObjectPtr o, PartEdgePtr pedge);
    
    // Returns true if this object is live at the given part and false otherwise.
    // This method is called by composers and should not be called by analyses.
    bool isLiveML(PartEdgePtr pedge);
    
    // Computes the meet of this and that and saves the result in this
    // returns true if this causes this to change and false otherwise
    bool meetUpdateML(MemLocObjectPtr that, PartEdgePtr pedge);
    
    // Returns whether this AbstractObject denotes the set of all possible execution prefixes.
    bool isFull(PartEdgePtr pedge);
    // Returns whether this AbstractObject denotes the empty set.
    bool isEmpty(PartEdgePtr pedge);
    
    // Returns true if this MemLocObject is in-scope at the given part and false otherwise
    //virtual bool isLive(PartEdgePtr pedge) const=0;
    
    // Allocates a copy of this object and returns a pointer to it
    MemLocObjectPtr copyML() const;
    
    SgType* getType() const {return type;}
    /*std::set<SgType*> getType() const
    {
      std::set<SgType*> rt;
      rt.insert(type);
      return rt;
    }*/
    //PartPtr getPart()    const {return part;}
    
    std::string str(std::string indent); // pretty print for the object
    std::string strp(PartEdgePtr pedge, std::string indent); // pretty print for the object
  };

  // A simple implementation of the abstract memory object interface
  // Four categories: scalar, labeled aggregate, array, and pointer
  class Scalar_Impl : public Scalar/*, public StxMemLocObject*/
  {
    public:
      Scalar_Impl(SgNode* n) : MemLocObject(n), Scalar(n) {}
      Scalar_Impl(const Scalar_Impl& that) : MemLocObject((const MemLocObject&)that), Scalar(that) {}
      // We implement operator < () at this level
      bool operator < (const MemLocObject& other) const ;
  };

  class Function_Impl : public FunctionMemLoc/*, public StxMemLocObject*/
  {
    public:
      Function_Impl(SgNode* n) : MemLocObject(n), FunctionMemLoc(n) {}
      Function_Impl(const Scalar_Impl& that) : MemLocObject((const MemLocObject&)that), FunctionMemLoc((const FunctionMemLoc&)that) {}
      // We implement operator < () at this level
      bool operator < (const MemLocObject& other) const ;
  };


  // #SA 10/15/12
  // Deriving from enable_shared_from_this provides shared_from_this() to create a shared_ptr using 'this'
  //  
  class LabeledAggregate_Impl : public LabeledAggregate
  {
    public:
      LabeledAggregate_Impl(SgNode* n) : MemLocObject(n), LabeledAggregate(n) {}
      LabeledAggregate_Impl(const Scalar_Impl& that) : MemLocObject((const MemLocObject&)that), LabeledAggregate((const LabeledAggregate&)that) {}
      bool operator < (const MemLocObject& other) const ;

      size_t fieldCount(PartEdgePtr pedge) const {return elements.size(); };
      // Returns a list of fields
      std::list<LabeledAggregateFieldPtr >  getElements(PartEdgePtr pedge) const {return elements;};
      std::list<LabeledAggregateFieldPtr >& getElements(PartEdgePtr pedge) {return elements;};
    protected:
      std::list<LabeledAggregateFieldPtr > elements; 
  };

  class Array_Impl : public Array/*, public StxMemLocObject*/
  {
    public:   
      Array_Impl(SgNode* n) : MemLocObject(n), Array(n) {}
      Array_Impl(const Scalar_Impl& that) : MemLocObject((const MemLocObject &)that), Array((const Array&)that) {}
      bool operator < (const MemLocObject& other) const ;
  };

  class Pointer_Impl: public Pointer/*, public StxMemLocObject*/
  {
    public: 
      Pointer_Impl(SgNode* n) : MemLocObject(n), Pointer(n) {}
      Pointer_Impl(const Scalar_Impl& that) : MemLocObject((const MemLocObject&)that), Pointer((const Pointer&)that) {}
      bool operator < (const MemLocObject& other) const ;
  };
  
  // The most intuitive implementation of array index vector
  class IndexVector_Impl : public IndexVector
  {
    public:
      size_t getSize(PartEdgePtr pedge) const {  return index_vector.size(); };
      /*GB: Deprecating IndexSets and replacing them with ValueObjects.
      std::vector<IndexSet *> index_vector; // a vector of memory objects of named objects or temp expression objects */
      std::vector<ValueObjectPtr> index_vector; // a vector of memory objects of named objects or temp expression objects
      std::string str(std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      IndexVectorPtr copyIV() const;
      
      /* GB: Deprecating the == operator. Now that some objects can contain AbstractObjects any equality test must take the current part as input.
      bool operator== (const IndexVector & other) const; */
      bool mayEqual  (IndexVectorPtr other, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
      bool mustEqual (IndexVectorPtr other, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
      bool equalSet  (IndexVectorPtr other, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
      bool subSet    (IndexVectorPtr other, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
      bool meetUpdate(IndexVectorPtr other, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
      bool isFull    (PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
      bool isEmpty   (PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
  };
  typedef boost::shared_ptr<IndexVector_Impl> IndexVector_ImplPtr;

  class NamedObj; 
  class LabeledAggregateField_Impl : public LabeledAggregateField
  {
    public:
      LabeledAggregateField_Impl(MemLocObjectPtr f, LabeledAggregatePtr p): field (f), parent(p) {}
      std::string getName(PartEdgePtr pedge); // field name
      size_t getIndex(PartEdgePtr pedge); // The field's index within its parent object. The first field has index 0.

      MemLocObjectPtr getField(PartEdgePtr pedge) { return field;}; // Pointer to an abstract description of the field
      void setField(MemLocObjectPtr f) {field = f;}; // Pointer to an abstract description of the field

      LabeledAggregatePtr getParent(PartEdgePtr pedge) {return parent;};
      void setParent(LabeledAggregatePtr p) {parent = p; };

      std::string str(std::string indent); // pretty print for the object
    private:
      MemLocObjectPtr field; // this should be a named obj 
      LabeledAggregatePtr parent; 
  };

  // The connection to the ROSE AST, all concrete type, size , etc. information come from this side
  // -----------------------------------------------------------------------------------------------
  //
  // Three kinds of memory objects in ROSE AST: each of them can be one of the four categories above.
  // 1) SgExpression temporary variables: each SgExpression which is not named memory objects. 
  //                         They can be seen as compiler-generated discrete temp variables
  // 2) named memory objects : one object for each named variable 
  // 3) aliased memory objects: one object for each type, used for a vague memory object
  //                             representing a set of aliased objects. All aliased objects of the
  //                             same type are represented by a single aliased memory object

  class ExprObj;
  typedef boost::shared_ptr<ExprObj> ExprObjPtr;
  class ExprObj: public StxMemLocObjectKind // one object for each SgExpression which does not have a corresponding symbol
    // They are similar to compiler-generated temporaries for three operand AST format
  { 
    public:
      SgExpression* anchor_exp; 
      
      ExprObj(SgExpression* a): MemLocObject(a), StxMemLocObjectKind(a), anchor_exp(a) {}
      ExprObj(const ExprObj& that) : MemLocObject((const MemLocObject &)that), StxMemLocObjectKind((const StxMemLocObjectKind&)that), anchor_exp(that.anchor_exp) {}
      
      bool mayEqualML (MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool isConstant() {return isSgValueExp(anchor_exp); } ; // if the expression object represent a constant value (SgValueExp)
      
      // Returns whether the two abstract objects denote the same set of concrete objects
      bool equalSet(AbstractObjectPtr o, PartEdgePtr pedge);
      
      // Returns whether this abstract object denotes a non-strict subset (the sets may be equal) of the set denoted
      // by the given abstract object.
      bool subSet(AbstractObjectPtr o, PartEdgePtr pedge);
      
      // Returns true if this MemLocObject is in-scope at the given part and false otherwise
      bool isLiveML(PartEdgePtr pedge);
    
      bool isFull(PartEdgePtr pedge);
      bool isEmpty(PartEdgePtr pedge);
      
      // Returns the type of the MemLoc this object denotes
      SgType* getType() const;
      
      virtual std::string str(std::string indent); // pretty print for the object
      virtual std::string strp(PartEdgePtr pedge, std::string indent); // pretty print for the object
  };

  // Correspond to variables that are explicit named in the source code
  // Including: local, global, and static variables, as well as their fields
  // Named objects may not directly alias each other since they must be stored disjointly
  // In most cases, two named objects are equal to each other if they correspond to the same entry in the application's symbol table. 
  //
  // However, for a symbol representing a field of a structure, it can be shared by many instances
  // of the structure. In this case, a parent MemLocObject is needed to distinguish between them 
  class NamedObj;
  typedef boost::shared_ptr<NamedObj> NamedObjPtr;
  class NamedObj: public StxMemLocObjectKind  // one object for each named variable with a symbol
  { 
    public:
      SgSymbol* anchor_symbol; // could be a symbol for variable, function, class/structure, etc.
      SgType* type;
      // NOTE: in most cases, the type field should be anchor_symbol->get_type(). But array element's type can be different from its array type
      MemLocObjectPtr parent; //exists for 1) compound variables like a.b, where a is b's parent, 2) also for array element, where array is the parent
      IndexVectorPtr  array_index_vector; // exists for array element: the index vector of an array element. Ideally this data member could be reused for index of field of structure/class

      //Is this always true that the parent of a named object must be an expr object?
      NamedObj (SgNode* n, SgSymbol* a, SgType* type, MemLocObjectPtr p, IndexVectorPtr iv): 
          MemLocObject(n), StxMemLocObjectKind(n), anchor_symbol(a), type(type), parent(p), array_index_vector (iv){}
      NamedObj (const NamedObj& that) : MemLocObject((const MemLocObject &)that), StxMemLocObjectKind((const StxMemLocObjectKind&)that),
                                        anchor_symbol(that.anchor_symbol), type(that.type), 
                                        parent(that.parent), array_index_vector(that.array_index_vector) {}
      //SgType* getType() const {return type;}
      MemLocObjectPtr getParent() {return parent; } 
      SgSymbol* getSymbol() {return anchor_symbol;}

      std::string getName() {return anchor_symbol->get_name().getString(); }

      virtual std::string str(std::string indent=""); // pretty print for the object
      virtual std::string strp(PartEdgePtr pedge, std::string indent=""); // pretty print for the object

      bool mayEqualML (NamedObjPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(NamedObjPtr o2, PartEdgePtr pedge); 

      bool mayEqualML (MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      
      // Returns whether the two abstract objects denote the same set of concrete objects
      bool equalSet(AbstractObjectPtr o, PartEdgePtr pedge);
      
      // Returns whether this abstract object denotes a non-strict subset (the sets may be equal) of the set denoted
      // by the given abstract object.
      bool subSet(AbstractObjectPtr o, PartEdgePtr pedge);
      
      // Returns true if this MemLocObject is in-scope at the given part and false otherwise
      bool isLiveML(PartEdgePtr pedge);
      
      bool isFull(PartEdgePtr pedge);
      bool isEmpty(PartEdgePtr pedge);
      
      // Returns the type of the MemLoc this object denotes
      SgType* getType() const;
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  //  Memory regions that may be accessible via a pointer, such as heap memory
  //  This implementation does not track accurate aliases, an aliased memory object and 
  //  an aliased or named object may be equal if they have the same typ
  class AliasedObj;
  typedef boost::shared_ptr<AliasedObj> AliasedObjPtr;
  class AliasedObj: public StxMemLocObjectKind
  {  // One object for each type
    public: 
      //#SA
      // StxMemLocObject 'type' is sufficient
      SgType* type; 
      AliasedObj(SgNode* n, SgType* type): MemLocObject(n), StxMemLocObjectKind(n), type(type) {}
      AliasedObj(const AliasedObj& that): MemLocObject((const MemLocObject &)that), StxMemLocObjectKind((const StxMemLocObjectKind&)that), type(that.type) {}
      
      virtual std::string str(std::string indent); // pretty print for the object

      bool mayEqualML (AliasedObjPtr o2, PartEdgePtr pedge);
      bool mustEqualML(AliasedObjPtr o2, PartEdgePtr pedge);

      bool mayEqualML (MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge);
      
      // Returns whether the two abstract objects denote the same set of concrete objects
      bool equalSet(AbstractObjectPtr o, PartEdgePtr pedge);
      
      // Returns whether this abstract object denotes a non-strict subset (the sets may be equal) of the set denoted
      // by the given abstract object.
      bool subSet(AbstractObjectPtr o, PartEdgePtr pedge);
      
      // Returns true if this MemLocObject is in-scope at the given part and false otherwise
      bool isLiveML(PartEdgePtr pedge);
      
      bool isFull(PartEdgePtr pedge);
      bool isEmpty(PartEdgePtr pedge);
      
      // Returns the type of the MemLoc this object denotes
      SgType* getType() const;
  };

  // -----------------------------
  // ----- Expression object -----
  // -----------------------------
  class ScalarExprObj: public Scalar_Impl, public ExprObj
  {
    public:
      ScalarExprObj(SgExpression* e, PartEdgePtr pedge): MemLocObject(e), Scalar_Impl(e), ExprObj(e) {}
      ScalarExprObj(const ScalarExprObj& that): MemLocObject((const MemLocObject &)that), Scalar_Impl((const Scalar_Impl&) that), ExprObj(that.anchor_exp) {}
      //std::set<SgType*> getType() const;
      
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge);*/
      
      std::string str(std::string indent); // pretty print for the object
      std::string strp(PartEdgePtr pedge, std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  // Does it make sense to have expressions which are of a function type?
  //  I guess so, like function passed as parameter, or a  pointer to a function?
  class FunctionExprObj: public Function_Impl, public ExprObj
  {
    public:
      FunctionExprObj(SgExpression* e, PartEdgePtr pedge): MemLocObject(e), Function_Impl(e), ExprObj(e) {}
      FunctionExprObj(const FunctionExprObj& that): MemLocObject((const MemLocObject &)that), Function_Impl((const Function_Impl&) that), ExprObj(that.anchor_exp) {}
      //std::set<SgType*> getType() const;
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); */

      std::string str(std::string indent); // pretty print for the object
      std::string strp(PartEdgePtr pedge, std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  class LabeledAggregateExprObj: public LabeledAggregate_Impl, public ExprObj
  {
    public:
      LabeledAggregateExprObj(SgExpression* s, PartEdgePtr pedge);
      LabeledAggregateExprObj(const LabeledAggregateExprObj& that);
      void init(SgExpression* e, PartEdgePtr pedge);
      //std::set<SgType*> getType();

      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); */
      
      std::string str(std::string indent); // pretty print for the object
      std::string strp(PartEdgePtr pedge, std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  class ArrayExprObj: public Array_Impl, public ExprObj
  {
    public:
      ArrayExprObj(SgExpression* e, PartEdgePtr pedge): MemLocObject(e), Array_Impl(e), ExprObj (e) {}
      ArrayExprObj(const ArrayExprObj& that): MemLocObject((const MemLocObject &)that), Array_Impl((const Array_Impl&) that), ExprObj(that.anchor_exp) {}
      //std::set<SgType*> getType();
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); */
      
      // GB: 2012-08-27: should be implementing the following functions here:
      //                 Array::getElements(), getElements(IndexVectorPtr ai), getNumDims(), getDereference()
      MemLocObjectPtr getElements(PartEdgePtr pedge);
      MemLocObjectPtr getElements(IndexVectorPtr ai, PartEdgePtr pedge);
      size_t getNumDims(PartEdgePtr pedge);
      boost::shared_ptr<MemLocObject> getDereference(PartEdgePtr pedge);
   
      std::string str(std::string indent); // pretty print for the object
      std::string strp(PartEdgePtr pedge, std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  class PointerExprObj: public Pointer_Impl, public ExprObj
  {
    public:
      PointerExprObj(SgExpression* e, PartEdgePtr pedge): MemLocObject(e), Pointer_Impl(e), ExprObj (e) {}
      PointerExprObj(const PointerExprObj& that): MemLocObject((const MemLocObject &)that), Pointer_Impl((const Pointer_Impl&) that), ExprObj(that.anchor_exp) {}
      //std::set<SgType*> getType();
      // used for a pointer to non-array
      MemLocObjectPtr getDereference(PartEdgePtr pedge);
      // used for a pointer to an array
      MemLocObjectPtr getElements(PartEdgePtr pedge);

      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); */
      
      std::string str(std::string indent); // pretty print for the object
      std::string strp(PartEdgePtr pedge, std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  // ------------------------
  // ----- Named object -----
  // ------------------------
  class ScalarNamedObj: public Scalar_Impl, public NamedObj 
  {
    public:
      ScalarNamedObj(SgNode* n, SgSymbol* s, MemLocObjectPtr p, IndexVectorPtr iv, PartEdgePtr pedge): 
        MemLocObject(n), Scalar_Impl(n), NamedObj (n, s, s->get_type(), p, iv) {}
      ScalarNamedObj(const ScalarNamedObj& that): MemLocObject((const MemLocObject &)that), Scalar_Impl(that), NamedObj((const NamedObj&)that) {}
      //std::set<SgType*> getType();
      
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); */
      
      std::string str(std::string indent); // pretty print for the object
      std::string strp(PartEdgePtr pedge, std::string indent); // pretty print for the object

      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  class FunctionNamedObj: public Function_Impl, public NamedObj 
  {
    public:

      // simple constructor, a function symbol is enough
      FunctionNamedObj(SgNode* n, SgSymbol* s, PartEdgePtr pedge): 
          MemLocObject(n), Function_Impl(n), NamedObj(n, s, s->get_type(), MemLocObjectPtr(), IndexVectorPtr()) {}
      FunctionNamedObj(const FunctionNamedObj& that): 
          MemLocObject((const MemLocObject &)that), Function_Impl(that), NamedObj((const NamedObj&)that) {}
      // I am not sure when a function can be used as a child and an array element. But this is
      // provided just in case
      FunctionNamedObj (SgNode* n, SgSymbol* s, PartEdgePtr pedge, MemLocObjectPtr p, IndexVectorPtr iv): 
          MemLocObject(n), Function_Impl(n), NamedObj (n, s, s->get_type(), p, iv) {}
      //std::set<SgType*> getType();
      
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); */
      
      std::string str(std::string indent); // pretty print for the object
      std::string strp(PartEdgePtr pedge, std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  class LabeledAggregateNamedObj: public LabeledAggregate_Impl, public NamedObj
  {
    public:
      LabeledAggregateNamedObj(SgNode* n, SgSymbol* s, MemLocObjectPtr p, IndexVectorPtr iv, PartEdgePtr pedge);
      LabeledAggregateNamedObj(const LabeledAggregateNamedObj& that);
      void init(SgSymbol* s, MemLocObjectPtr p, IndexVectorPtr iv, PartEdgePtr pedge);
      //std::set<SgType*> getType();

      // Returns true if this object and that object may/must refer to the same labeledAggregate memory object.
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); */
      
      std::string str(std::string indent); // pretty print for the object
      std::string strp(PartEdgePtr pedge, std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  class ArrayNamedObj;
  class ArrayNamedObj: public Array_Impl, public NamedObj//, public boost::enable_shared_from_this<ArrayNamedObj>
  {
    public:
      ArrayNamedObj(SgNode* n, SgSymbol* s, MemLocObjectPtr p, IndexVectorPtr iv, PartEdgePtr pedge);
      ArrayNamedObj(const ArrayNamedObj& that);
      void init(SgSymbol* s, MemLocObjectPtr p, IndexVectorPtr iv);
      //std::set <SgType*> getType();
      std::string toString();

      // Returns a memory object that corresponds to all the elements in the given array
      // GB 2012-08-27: This doesn't look right. The contents of an array have one less level of indirection than 
      //                the array itself. You shouldn't be able to call getElements on the contents of an int array.
      MemLocObjectPtr getElements(PartEdgePtr pedge) { return MemLocObjectPtr(this); } ; 
      // Returns the memory object that corresponds to the elements described by the given abstract index, 
      MemLocObjectPtr getElements(IndexVectorPtr ai, PartEdgePtr pedge);

      // number of dimensions of the array
      virtual size_t getNumDims(PartEdgePtr pedge) const;

      // rare case that an array is dereferenced, treated as array[0]
      MemLocObjectPtr getDereference(PartEdgePtr pedge);
      
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge);*/
      
      std::string str(std::string indent); // pretty print for the object
      std::string strp(PartEdgePtr pedge, std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  class PointerNamedObj: public Pointer_Impl, public NamedObj
  {
    public:
      PointerNamedObj(SgNode*n, SgSymbol* s, MemLocObjectPtr p, IndexVectorPtr iv, PartEdgePtr pedge): 
          MemLocObject(n), Pointer_Impl(n), NamedObj (n, s, s->get_type(), p, iv) {}
      PointerNamedObj(const PointerNamedObj& that): 
          MemLocObject((const MemLocObject &)that), Pointer_Impl((const Pointer_Impl&)that), NamedObj((const NamedObj&)that) {}
      //std::set<SgType*> getType() const;
      // used for a pointer to non-array
      MemLocObjectPtr getDereference(PartEdgePtr pedge);
      // used for a pointer to an array
      MemLocObjectPtr getElements(PartEdgePtr pedge);
      // Returns true if this object and that object may/must refer to the same pointer memory object.
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); */
      
      std::string str(std::string indent); // pretty print for the object
      std::string strp(PartEdgePtr pedge, std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  // --------------------------
  // ----- Aliased object -----
  // --------------------------
  class ScalarAliasedObj: public Scalar_Impl, public AliasedObj
  {
    public:
      ScalarAliasedObj(SgNode* n, SgType* t, PartEdgePtr pedge): MemLocObject(n), Scalar_Impl(n), AliasedObj(n, t) {}
      ScalarAliasedObj(const ScalarAliasedObj& that): MemLocObject((const MemLocObject&)that), Scalar_Impl((const Scalar_Impl&)that), AliasedObj((const AliasedObj&)that) {}
      //std::set<SgType*> getType();
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge);*/
      
      std::string str(std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  class FunctionAliasedObj: public Function_Impl, public AliasedObj
  {
    public:
      FunctionAliasedObj(SgNode* n, SgType* t, PartEdgePtr pedge): MemLocObject(n), Function_Impl(n), AliasedObj(n, t){}
      FunctionAliasedObj(const FunctionAliasedObj& that):  MemLocObject((const MemLocObject&)that), Function_Impl((const Function_Impl&)that), AliasedObj((const AliasedObj&)that) {}
      //std::set<SgType*> getType();
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); */
      
      std::string str(std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };
  
  class LabeledAggregateAliasedObj : public  LabeledAggregate_Impl, public AliasedObj
  {
    public:
      LabeledAggregateAliasedObj(SgNode* n, SgType* t, PartEdgePtr pedge): MemLocObject(n), LabeledAggregate_Impl(n), AliasedObj(n, t) {}
      LabeledAggregateAliasedObj(const LabeledAggregateAliasedObj& that): 
           MemLocObject((const MemLocObject&)that), LabeledAggregate_Impl((const LabeledAggregate_Impl&)that), AliasedObj((const AliasedObj&)that) {}
      //std::set<SgType*> getType();
      //TODO
      // size_t fieldCount();
      // std::list<LabeledAggregateField*> getElements() const;
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); */
      
      std::string str(std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  class ArrayAliasedObj: public Array_Impl, public AliasedObj
  {
    public:
      ArrayAliasedObj(SgNode* n, SgType* t, PartEdgePtr pedge): MemLocObject(n), Array_Impl(n), AliasedObj(n, t){}
      ArrayAliasedObj(const ArrayAliasedObj& that): 
           MemLocObject((const MemLocObject&)that), Array_Impl((const Array_Impl&)that), AliasedObj((const AliasedObj&)that) {}
      //std::set<SgType*> getType();

      //TODO
      // MemLocObject* getElements();
      //  MemLocObject* getElements(AbstractIndex* ai);
      //  getNumDims();
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); */
      
      // GB: 2012-08-27: should be implementing the following functions here:
      //                 Array::getElements(), getElements(IndexVectorPtr ai), getNumDims(), getDereference()
      MemLocObjectPtr getElements(PartEdgePtr pedge);
      MemLocObjectPtr getElements(IndexVectorPtr ai, PartEdgePtr pedge);
      size_t getNumDims(PartEdgePtr pedge);
      boost::shared_ptr<MemLocObject> getDereference(PartEdgePtr pedge);
      
      std::string str(std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  class PointerAliasedObj: public Pointer_Impl, public AliasedObj
  {
    public:
      PointerAliasedObj(SgNode* n, SgType* t, PartEdgePtr pedge): MemLocObject(n), Pointer_Impl(n), AliasedObj(n, t) {}
      PointerAliasedObj(const PointerAliasedObj& that):  MemLocObject((const MemLocObject&)that), Pointer_Impl((const Pointer_Impl&)that), AliasedObj((const AliasedObj&)that) {}
      MemLocObjectPtr getDereference(PartEdgePtr pedge);
      // MemLocObject * getElements() const;
      //std::set<SgType*> getType();
      /*bool mayEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); 
      bool mustEqualML(MemLocObjectPtr o2, PartEdgePtr pedge); */
      
      std::string str(std::string indent); // pretty print for the object
      
      // Allocates a copy of this object and returns a pointer to it
      StxMemLocObjectKindPtr copyMLK() const;
  };

  // lower level, internal  builder for different objects -----------------------
  // Users should use MemLocObject* ObjSetFactory::createObjSet (SgNode* n) instead
  
  // Creates an AliasedMemLocObject and an StxMemLocObject that contains it. It can return NULL since not all types are supported.
  // One object per type, Type based alias analysis. A type of the object pointed to by a pointer.
  MemLocObjectPtr createAliasedMemLocObject(SgNode* n, SgType* t, PartEdgePtr pedge);
  
  // Creates an AliasedMemLocObject, which is a MemLocObjectKind. It can return NULL since not all types are supported.
  // One object per type, Type based alias analysis. A type of the object pointed to by a pointer.
  StxMemLocObjectKindPtr createAliasedMemLocObjectKind(SgNode* n, SgType* t, PartEdgePtr pedge);
  
  MemLocObjectPtr createNamedMemLocObject(SgNode* n, SgSymbol* anchor_symbol, SgType* t, PartEdgePtr pedge, MemLocObjectPtr parent, IndexVectorPtr iv); // any 
  
  // Create a NamedMemLocObject from a static variable reference of form a and a.b.c where a is not a reference type
  MemLocObjectPtr createNamedMemLocObject_DirectVarRef(SgNode* n, SgExpression* ref, PartEdgePtr pedge);
  
  // Create a NamedMemLocObject from a static variable reference of form a and a.b.c where a is not a reference type
  MemLocObjectPtr createNamedMemLocObject_DirectVarRefDotExp(SgNode* n, SgExpression* ref, PartEdgePtr pedge);
  
  MemLocObjectPtr createNamedMemLocObject_PntrArrRef(SgNode* n, SgPntrArrRefExp* r, PartEdgePtr pedge);
  //MemLocObjectPtr createNamedMemLocObject(SgNode* n, SgVarRefExp* r, PartEdgePtr pedge); // create NamedMemLocObject or AliasedMemLocObject (for pointer type) from a variable reference   
  
  //MemLocObjectPtr createNamedMemLocObject(SgNode* n, SgPntrArrRefExp* r, PartEdgePtr pedge); // create NamedMemLocObject from an array element access
  MemLocObjectPtr createExpressionMemLocObject(SgExpression* anchor_exp, PartEdgePtr pedge); 
  // Return true if op is an operand of the given SgNode n and false otherwise.
  bool isOperand(SgNode* n, SgExpression* op);
  // MemLocObject* createMemLocObject(SgNode*); // top level catch all case, declared in memory_object.h

  // Helper functions 
  // --------------------------------------
  // debugging
  void dump_aliased_objset_map (); 

  // A helper function to decide if two types are aliased
  // two cases: 1 they are the same type
  //            2 they have overlap (one type is a subtype of the other)
  bool isAliased (const SgType * t1, const SgType * t2 ); 

  // Returns true is type t1 is a sub-type (derived from) of type t2.
  bool isSubType(const SgType* t1, const SgType* t2);
  // !!! FOR NOW WE JUST CHECK IF THE TYPES ARE ALIASED BUT WE NEED PROPER CODE FOR THIS
  
  // If a symbol corresponds to a member variable declaration within SgClassDefinition, returns a pointer
  // to the SgClassDefinition. Otherwise, returns NULL.
  SgClassDefinition* isMemberVariableDeclarationSymbol(SgSymbol * s);

  // a helper function to fill up elements of MemLocObject p from a class/structure type
  // #SA 10/15/12 - modified first parameter to LabeledAggregatePtr
  // Currently only labeled aggregate objects use this function
  void fillUpElements (MemLocObjectPtr p, std::list<boost::shared_ptr<LabeledAggregateField> > & elements, SgClassType* c_t, PartEdgePtr part);

  // convert std::vector<SgExpression*>* subscripts to IndexVectorPtr  array_index_vector
  IndexVectorPtr generateIndexVector (std::vector<SgExpression*>& subscripts);
}; //namespace fuse

#endif
