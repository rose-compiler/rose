#ifndef CONSTANT_PROPAGATION_ANALYSIS_H
#define CONSTANT_PROPAGATION_ANALYSIS_H

#include "compose.h"
namespace fuse
{
    class CPValueObject;
};
#include "VariableStateTransfer.h"
#include "abstract_object_map.h"

namespace fuse
{
/***************************************
 ***** ConstantPropagationAnalysis *****
 ***************************************/

// This is a forward dataflow analysis that implements a simple abstraction of values 
// that consists of the universal set, a single constant value and an empty set. It 
// maintains a map of memory locations to these value abstractions.

extern int constantPropagationAnalysisDebugLevel;

class CPValueObject : public FiniteLattice, public ValueObject
{
  private:
  // the current value of the variable (if known)
  int value;
  
  // bool uninitialized; // Covered by case of bottom.
  
  bool undefined;
  
  private:
  // this object's current level in the lattice: (bottom, valKnown, divKnown, fullSet)
  short level;
  
  public:  // The different levels of this lattice
  // No information is known about the value of the variable (its set of possible values is empty)
  static const short emptySet = 1; 
  
  // Value is known
  static const short constantValue = 2; 
  
  // This variable holds more values than can be represented using a single value 
  static const short fullSet = 3;
  
  public:
  // Do we need a default constructor?
  CPValueObject(PartEdgePtr pedge);
  
  // This constructor builds a constant value lattice.
  CPValueObject(int v, PartEdgePtr pedge);
  
  CPValueObject(short level, int v, PartEdgePtr pedge);
  
  // Do we need th copy constructor?
  CPValueObject(const CPValueObject & X);
  
  // Access functions.
  int getValue() const;
  short getLevel() const;
  
  bool setValue(int x);
  bool setLevel(short x);
  
  // **********************************************
  // Required definition of pure virtual functions.
  // **********************************************
  void initialize();
  
  // returns a copy of this lattice
  Lattice* copy() const;
  
  // overwrites the state of "this" Lattice with "that" Lattice
  void copy(Lattice* that);
  
  bool operator==(Lattice* that) /*const*/;
  
  // computes the meet of this and that and saves the result in this
  // returns true if this causes this to change and false otherwise
  bool meetUpdate(Lattice* that);
  bool meetUpdate(CPValueObject* that);
  
  // Set this Lattice object to represent the set of all possible execution prefixes.
  // Return true if this causes the object to change and false otherwise.
  bool setToFull();
  
  // Set this Lattice object to represent the of no execution prefixes (empty set)
  // Return true if this causes the object to change and false otherwise.
  bool setToEmpty();
  
  // Set all the information associated Lattice object with this MemLocObjectPtr to full.
  // Return true if this causes the object to change and false otherwise.
  bool setMLValueToFull(MemLocObjectPtr ml);
  
  // Returns whether this lattice denotes the set of all possible execution prefixes.
  bool isFull();
  // Returns whether this lattice denotes the empty set.
  bool isEmpty();
  
  // pretty print for the object
  std::string str(std::string indent="") const;
  std::string str(std::string indent="") { return ((const CPValueObject*)this)->str(indent); }
  std::string strp(PartEdgePtr pedge, std::string indent="") const;
    
  bool mayEqualV(ValueObjectPtr o, PartEdgePtr pedge);
  bool mustEqualV(ValueObjectPtr o, PartEdgePtr pedge);
  
  // Returns whether the two abstract objects denote the same set of concrete objects
  bool equalSet(AbstractObjectPtr o, PartEdgePtr pedge);
  
  // Returns whether this abstract object denotes a non-strict subset (the sets may be equal) of the set denoted
  // by the given abstract object.
  bool subSet(AbstractObjectPtr o, PartEdgePtr pedge);
  
  // Computes the meet of this and that and saves the result in this.
  // Returns true if this causes this to change and false otherwise.
  bool meetUpdateV(ValueObjectPtr that, PartEdgePtr pedge);
  
  // Returns whether this AbstractObject denotes the set of all possible execution prefixes.
  bool isFull(PartEdgePtr pedge);
  // Returns whether this AbstractObject denotes the empty set.
  bool isEmpty(PartEdgePtr pedge);
  
  // Allocates a copy of this object and returns a pointer to it
  ValueObjectPtr copyV() const;

  // Returns true if this ValueObject corresponds to a concrete value that is statically-known
  bool isConcrete();
  // Returns the type of the concrete value (if there is one)
  SgType* getConcreteType();
  // Returns the concrete value (if there is one) as an SgValueExp, which allows callers to use
  // the normal ROSE mechanisms to decode it
  set<boost::shared_ptr<SgValueExp> > getConcreteValue();
  
  /* Don't have good idea how to represent a finite number of options 
  virtual bool isFiniteSet()=0;
  virtual set<AbstractObj> getValueSet()=0;*/
  
  //string str(const string& indent);
};
typedef boost::shared_ptr<CPValueObject> CPValueObjectPtr;

class ConstantPropagationAnalysis : virtual public FWDataflow
{
  protected:
  //static std::map<varID, Lattice*> constVars;
  //AbstractObjectMap constVars;
   
  public:
  ConstantPropagationAnalysis();
  
  // Returns a shared pointer to a freshly-allocated copy of this ComposedAnalysis object
  ComposedAnalysisPtr copy() { return boost::make_shared<ConstantPropagationAnalysis>(); }
  
  // Initializes the state of analysis lattices at the given function, part and edge into our out of the part
  // by setting initLattices to refer to freshly-allocated Lattice objects.
  void genInitLattice(PartPtr part, PartEdgePtr pedge, 
                      std::vector<Lattice*>& initLattices);
  
  bool transfer(PartPtr part, CFGNode cn, NodeState& state, std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo);
  
  boost::shared_ptr<DFTransferVisitor> getTransferVisitor(PartPtr part, CFGNode cn, 
                                              NodeState& state, std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo);
  
  boost::shared_ptr<ValueObject> Expr2Val(SgNode* n, PartEdgePtr pedge);
  bool implementsExpr2Val() { return true; }
  
  // pretty print for the object
  std::string str(std::string indent="")
  { return "ConstantPropagationAnalysis"; }
  
  friend class ConstantPropagationAnalysisTransfer;
};

class ConstantPropagationAnalysisTransfer : public VariableStateTransfer<CPValueObject>
{
   private:
   typedef void (ConstantPropagationAnalysisTransfer::*TransferOp)(CPValueObjectPtr, CPValueObjectPtr, CPValueObjectPtr);
   template <typename T> void transferArith(SgBinaryOp *sgn, T transferOp);
   template <class T> void visitIntegerValue(T *sgn);
   
   void transferArith(SgBinaryOp *sgn, TransferOp transferOp);
   
   void transferIncrement(SgUnaryOp *sgn);
   void transferCompoundAdd(SgBinaryOp *sgn);
   void transferAdditive(CPValueObjectPtr arg1Lat, CPValueObjectPtr arg2Lat, CPValueObjectPtr resLat, bool isAddition);
   void transferMultiplicative(CPValueObjectPtr arg1Lat, CPValueObjectPtr arg2Lat, CPValueObjectPtr resLat);
   void transferDivision(CPValueObjectPtr arg1Lat, CPValueObjectPtr arg2Lat, CPValueObjectPtr resLat);
   void transferMod(CPValueObjectPtr arg1Lat, CPValueObjectPtr arg2Lat, CPValueObjectPtr resLat);
   void transferLogical(CPValueObjectPtr arg1Lat, CPValueObjectPtr arg2Lat, 
                        CPValueObjectPtr resLat, SgExpression* expr);
   // Transfer function for logical short-circuit operations: && and ||
   void transferShortCircuitLogical(SgBinaryOp *sgn);

  // Logical operations that include their own control flow due to short-circuit evaluation
  /*void transferControlLogical(CPValueObjectPtr arg1Lat, CPValueObjectPtr arg2Lat,
                              CPValueObjectPtr resLat, SgExpression* expr);*/

  public:
   //  void visit(SgNode *);
   // Values
   void visit(SgLongLongIntVal *sgn);
   void visit(SgLongIntVal *sgn);
   void visit(SgIntVal *sgn);
   void visit(SgShortVal *sgn);
   void visit(SgUnsignedLongLongIntVal *sgn);
   void visit(SgUnsignedLongVal *sgn);
   void visit(SgUnsignedIntVal *sgn);
   void visit(SgUnsignedShortVal *sgn);
   void visit(SgValueExp *sgn);
   // Arithmetic Operations
   void visit(SgPlusAssignOp *sgn);
   void visit(SgMinusAssignOp *sgn);
   void visit(SgMultAssignOp *sgn);
   void visit(SgDivAssignOp *sgn);
   void visit(SgModAssignOp *sgn);
   void visit(SgAddOp *sgn);
   void visit(SgSubtractOp *sgn);
   void visit(SgMultiplyOp *sgn);
   void visit(SgDivideOp *sgn);
   void visit(SgModOp *sgn);
   // Increment Operations
   void visit(SgPlusPlusOp *sgn);
   void visit(SgMinusMinusOp *sgn);
   // Unary Operations
   void visit(SgUnaryAddOp *sgn);
   void visit(SgMinusOp *sgn);
   void visit(SgCastExp *sgn);
   // Logical Operations
   void visit(SgGreaterOrEqualOp *sgn);
   void visit(SgGreaterThanOp *sgn);
   void visit(SgLessOrEqualOp *sgn);
   void visit(SgLessThanOp *sgn);
   void visit(SgEqualityOp *sgn);
   void visit(SgNotEqualOp *sgn);
   void visit(SgAndOp *sgn);
   void visit(SgOrOp *sgn);
   
   bool finish();
   
   ConstantPropagationAnalysisTransfer(PartPtr part, CFGNode cn, NodeState& state, 
                                       std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo, 
                                       Composer* composer, ConstantPropagationAnalysis* analysis);
};

}; //namespace fuse

#endif
