#ifndef LIVEDEADMEMANALYSIS_H
#define LIVEDEADMEMANALYSIS_H

#include "compose.h"
#include "abstract_object_set.h"

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>

/* Limitations
   ===========
 * - Variable expressions get declared dead at SgInitializedName nodes as do the expressions for arrays.
 *   However, objects for array[i] are not declared dead at array's SgInitializedName.
 * - Some SgNodes such as SgCast are omitted from the CFGNode traversal. However, since they may appear
 *   as operands, ExprObjs for them do get introduced but never get killed since their SgNodes are never visited.
 */

namespace fuse {
extern int liveDeadAnalysisDebugLevel;

// Virtual class that allows users of the LiveDeadVarsAnalysis to mark certain variables as 
// being used inside a function call if the function's body is not available.
// NOTE: not sure where the definition exists for this class
class funcSideEffectUses
{
public:
    // Returns the set of variables that are used in a call to the given function for which a body has not been provided.
    // The function is also provided with the Part where the function was called, as well as its state.
    virtual std::set<MemLocObjectPtr> usedVarsInFunc(const Function& func, PartPtr part, NodeState& state)=0;
};

class LiveDeadMemAnalysis;

class LiveDeadMemTransfer : public DFTransferVisitor
{
    AbstractObjectSet* liveLat;
    LiveDeadMemAnalysis* ldma;
    //ComposerExpr2MemLocPtr ceml;
    Composer* composer;

    bool modified;
    /*// Expressions that are assigned by the current operation
    std::set<SgExpression*>   assignedExprs;
    // Variables that are assigned by the current operation
    std::set<MemLocObjectPtr> assignedMem;
    // Variables that are used/read by the current operation
    std::set<MemLocObjectPtr> usedMem;*/
    AbstractObjectSet assigned;
    AbstractObjectSet used;
    
    PartPtr part;

    funcSideEffectUses *fseu;

    friend class LDMAExpressionTransfer;

    // Note that the variable corresponding to this expression is assigned
    void assign(SgNode* sgn, SgExpression *operand);
    //void assign(AbstractObjectPtr mem);
    
    // Note that the variable corresponding to this expression is used
    void use(SgNode *sgn, SgExpression* operand);
    //void use(AbstractObjectPtr mem);
    // Note that the memory location denoted by the corresponding SgInitializedName is used
    void useMem(SgInitializedName* name);
    // Note that the memory location denoted by the corresponding SgVarRefExp is used
    void useMem(SgVarRefExp* e);
    // Note that the memory location denoted by the corresponding SgPntrArrRefExp is used
    void useMem(SgPntrArrRefExp* e);
    
    // Returns true if the given expression is currently live and false otherwise
    bool isMemLocLive(SgExpression* sgn);
    bool isMemLocLive(SgExpression* sgn, SgExpression* operand);

public:
LiveDeadMemTransfer(PartPtr part, CFGNode cn, NodeState &s, 
                    std::map<PartEdgePtr, std::vector<Lattice*> > &dfInfo, 
                    LiveDeadMemAnalysis* ldma,
                    /*ComposerExpr2MemLocPtr ceml, */Composer* composer, funcSideEffectUses *fseu);

    bool finish();

    void visit(SgExpression *);
    void visit(SgInitializedName *);
    void visit(SgReturnStmt *);
    void visit(SgExprStatement *);
    void visit(SgCaseOptionStmt *);
    void visit(SgSwitchStatement *);
    void visit(SgIfStmt *);
    void visit(SgForStatement *);
    void visit(SgWhileStmt *);
    void visit(SgDoWhileStmt *);
    void visit(SgFunctionDefinition* def);
};

class LDMemLocObject;

class LiveDeadMemAnalysis : public BWDataflow
{
protected:
    funcSideEffectUses* fseu;
    
public:
    LiveDeadMemAnalysis(funcSideEffectUses* fseu=NULL);
    
    // Returns a shared pointer to a freshly-allocated copy of this ComposedAnalysis object
    ComposedAnalysisPtr copy() { return boost::make_shared<LiveDeadMemAnalysis>(fseu); }
    
    // Initializes the state of analysis lattices at the given function, part and edge into our out of the part
    // by setting initLattices to refer to freshly-allocated Lattice objects.
    void genInitLattice(PartPtr part, PartEdgePtr pedge, 
                        std::vector<Lattice*>& initLattices);
        
    boost::shared_ptr<DFTransferVisitor> getTransferVisitor(
                                                      PartPtr part, CFGNode cn,
                                                      NodeState& state, 
                                                      std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo)
    { 
      // We're currently computing information for all these edges coming into part. Update all the Lattices 
      // in dfInfo to be associated with these edges to makes sure that if there were any prior liveness 
      // analyses, we rely on their opinions of what is live before this part, rather than after this part
      // (the edge associated with dfInfo's Lattices at the time of the call).
      for(std::map<PartEdgePtr, std::vector<Lattice*> >::iterator df=dfInfo.begin(); df!=dfInfo.end(); df++)
        for(std::vector<Lattice*>::iterator l=df->second.begin(); l!=df->second.end(); l++)
          (*l)->setPartEdge(part->inEdgeFromAny());
      
      return boost::shared_ptr<DFTransferVisitor>(
                new LiveDeadMemTransfer(part, cn, state, dfInfo, 
                                        this, getComposer(), //ComposerExpr2MemLocPtr(new ComposerExpr2MemLoc(*getComposer(), part->inEdgeFromAny(), *((ComposedAnalysis*)this))),
                                        fseu));
    }

    bool transfer(PartPtr part, CFGNode cn, NodeState& state, 
                  std::map<PartEdgePtr, std::vector<Lattice*> >& dfInfo) { assert(0); return false; }
    
    // Maps the given SgNode to an implementation of the MemLocObject abstraction.
    MemLocObjectPtr Expr2MemLoc(SgNode* n, PartEdgePtr pedge);
    bool implementsExpr2MemLoc() { return true; }
    
    // pretty print for the object
    std::string str(std::string indent="")
    { return "LiveDeadMemAnalysis"; }
};

class LDMemLocObject : public virtual MemLocObject
{
  protected:
  MemLocObjectPtr parent;
  LiveDeadMemAnalysis* ldma;

  public:
  LDMemLocObject(SgNode* n, MemLocObjectPtr parent_, LiveDeadMemAnalysis* ldma);
  LDMemLocObject(const LDMemLocObject& that);

  bool mayEqualML(MemLocObjectPtr o, PartEdgePtr pedge);
  bool mustEqualML(MemLocObjectPtr o, PartEdgePtr pedge);

  // Returns whether the two abstract objects denote the same set of concrete objects
  bool equalSet(AbstractObjectPtr o, PartEdgePtr pedge);
  
  // Returns whether this abstract object denotes a non-strict subset (the sets may be equal) of the set denoted
  // by the given abstract object.
  bool subSet(AbstractObjectPtr o, PartEdgePtr pedge);
  
  // Returns true if this object is live at the given part and false otherwise
  bool isLiveML(PartEdgePtr pedge);
  
  // Computes the meet of this and that and saves the result in this
  // returns true if this causes this to change and false otherwise
  bool meetUpdateML(MemLocObjectPtr that, PartEdgePtr pedge);
  
  // Returns whether this AbstractObject denotes the set of all possible execution prefixes.
  bool isFull(PartEdgePtr pedge);
  // Returns whether this AbstractObject denotes the empty set.
  bool isEmpty(PartEdgePtr pedge);
  
  // pretty print for the object
  std::string str(std::string indent="") const;
  std::string str(std::string indent="") { return ((const LDMemLocObject*)this)->str(indent); }
  std::string strp(PartEdgePtr pedge, std::string indent="");
  
  // Allocates a copy of this object and returns a pointer to it
  MemLocObjectPtr copyML() const;
  
  private:
  // Dummy Virtual method to make it possible to dynamic cast from LDMemLocObjectPtr to MemLocObjectPtr
  virtual void foo() {}
};
typedef boost::shared_ptr<LDMemLocObject> LDMemLocObjectPtr;

// Creates an instance of an LDMemLocObject that belongs to one of the MemLocObject categories
// (LDMemLocObject sub-classes): LDScalar, LDFunctionMemLoc, LDLabeledAggregate, LDArray or LDPointer.
LDMemLocObjectPtr createLDMemLocObjectCategory(SgNode* n, MemLocObjectPtr parent, LiveDeadMemAnalysis* ldma);

/*
//memory object that has no internal structure
class LDScalar : virtual public LDMemLocObject, virtual public Scalar
{
 public:
   LDScalar(SgNode* n, MemLocObjectPtr parent, LiveDeadMemAnalysis* ldma);
   
  // Implement the required functions by calling the real copies in LDMemLocObject
  bool mayEqual(MemLocObjectPtr o, PartEdgePtr pedge)  { return LDMemLocObject::mayEqualML(o, pedge); }
  bool mustEqual(MemLocObjectPtr o, PartEdgePtr pedge) { return LDMemLocObject::mayEqualML(o, pedge); }
  std::string str(std::string indent="") const { return LDMemLocObject::str(indent); }
  std::string str(std::string indent="") { return LDMemLocObject::str(indent); }
  std::string strp(PartEdgePtr pedge, std::string indent="") { return LDMemLocObject::strp(pedge, indent); }
  MemLocObjectPtr copyML() const { return LDMemLocObject::copyML(); }
};
typedef boost::shared_ptr<LDScalar> LDScalarPtr;

// memory object to model function objects in memory
class LDFunctionMemLoc: virtual public LDMemLocObject, virtual public FunctionMemLoc
{
public:  
  LDFunctionMemLoc(SgNode* n, MemLocObjectPtr parent, LiveDeadMemAnalysis* ldma);
  
  // Implement the required functions by calling the real copies in LDMemLocObject
  bool mayEqual(MemLocObjectPtr o, PartEdgePtr pedge)  { return LDMemLocObject::mayEqualML(o, pedge); }
  bool mustEqual(MemLocObjectPtr o, PartEdgePtr pedge) { return LDMemLocObject::mayEqualML(o, pedge); }
  std::string str(std::string indent="") const { return LDMemLocObject::str(indent); }
  std::string str(std::string indent="") { return LDMemLocObject::str(indent); }
  std::string strp(PartEdgePtr pedge, std::string indent="") { return LDMemLocObject::strp(pedge, indent); }
  MemLocObjectPtr copyML() const { return LDMemLocObject::copyML(); }
};
typedef boost::shared_ptr<LDFunctionMemLoc> LDFunctionMemLocPtr;

// a memory object that contains a finite number of explicitly labeled memory objects, such as structs, classes and bitfields
class LDLabeledAggregate: virtual public LDMemLocObject, virtual public LabeledAggregate
{
 public:
   LDLabeledAggregate(SgNode* n, MemLocObjectPtr parent, LiveDeadMemAnalysis* ldma);
   
   // number of fields
   size_t fieldCount(PartEdgePtr pedge);

   // Returns a list of field
   std::list<LabeledAggregateFieldPtr > getElements(PartEdgePtr pedge) const; 
   
  // Implement the required functions by calling the real copies in LDMemLocObject
  bool mayEqual(MemLocObjectPtr o, PartEdgePtr pedge)  { return LDMemLocObject::mayEqualML(o, pedge); }
  bool mustEqual(MemLocObjectPtr o, PartEdgePtr pedge) { return LDMemLocObject::mayEqualML(o, pedge); }
  std::string str(std::string indent="") const { return LDMemLocObject::str(indent); }
  std::string str(std::string indent="") { return LDMemLocObject::str(indent); }
  std::string strp(PartEdgePtr pedge, std::string indent="") { return LDMemLocObject::strp(pedge, indent); }
  MemLocObjectPtr copyML() const { return LDMemLocObject::copyML(); }
};
typedef boost::shared_ptr<LDLabeledAggregate> LDLabeledAggregatePtr;

// Some programming languages don't have the concept of pointers. We provide explicit support for Array
class LDArray: virtual public LDMemLocObject, virtual public Array
{
 public:
   LDArray(SgNode* n, MemLocObjectPtr parent, LiveDeadMemAnalysis* ldma);
   
   // Returns a memory object that corresponds to all the elements in the given array
   MemLocObjectPtr getElements(PartEdgePtr pedge);
   // Returns the memory object that corresponds to the elements described by the given abstract index, 
   // which represents one or more indexes within the array
   MemLocObjectPtr getElements(IndexVectorPtr ai, PartEdgePtr pedge);

   // number of dimensions of the array
   size_t getNumDims(PartEdgePtr pedge);

   //--- pointer like semantics
   // support dereference of array object, similar to the dereference of pointer
   // Return the element object: array[0]
   MemLocObjectPtr getDereference(PartEdgePtr pedge);
   
  // Implement the required functions by calling the real copies in LDMemLocObject
  bool mayEqual(MemLocObjectPtr o, PartEdgePtr pedge)  { return LDMemLocObject::mayEqualML(o, pedge); }
  bool mustEqual(MemLocObjectPtr o, PartEdgePtr pedge) { return LDMemLocObject::mayEqualML(o, pedge); }
  std::string str(std::string indent="") const { return LDMemLocObject::str(indent); }
  std::string str(std::string indent="") { return LDMemLocObject::str(indent); }
  std::string strp(PartEdgePtr pedge, std::string indent="") { return LDMemLocObject::strp(pedge, indent); }
  MemLocObjectPtr copyML() const { return LDMemLocObject::copyML(); }
};
typedef boost::shared_ptr<LDArray> LDArrayPtr;

class LDPointer: virtual public LDMemLocObject, virtual public Pointer
{
 public:
   LDPointer(SgNode* n, MemLocObjectPtr parent, LiveDeadMemAnalysis* ldma);
   
   MemLocObjectPtr getDereference(PartEdgePtr pedge);
   
  // Implement the required functions by calling the real copies in LDMemLocObject
  bool mayEqual(MemLocObjectPtr o, PartEdgePtr pedge)  { return LDMemLocObject::mayEqualML(o, pedge); }
  bool mustEqual(MemLocObjectPtr o, PartEdgePtr pedge) { return LDMemLocObject::mayEqualML(o, pedge); }
  std::string str(std::string indent="") const { return LDMemLocObject::str(indent); }
  std::string str(std::string indent="") { return LDMemLocObject::str(indent); }
  std::string strp(PartEdgePtr pedge, std::string indent="") { return LDMemLocObject::strp(pedge, indent); }
  MemLocObjectPtr copyML() const { return LDMemLocObject::copyML(); }
};
typedef boost::shared_ptr<LDPointer> LDPointerPtr;*/

// Initialize vars to hold all the variables and expressions that are live at PartEdgePtr pedge
void getAllLiveMemAt(LiveDeadMemAnalysis* ldma, PartEdgePtr pedge, const NodeState& state, std::set<AbstractObjectPtr>& vars, std::string indent="");

// Returns the set of variables and expressions that are live at PartEdgePtr pedge
std::set<AbstractObjectPtr> getAllLiveMemAt(LiveDeadMemAnalysis* ldma, PartEdgePtr pedge, const NodeState& state, std::string indent="");

// Returns true if the given MemLocObject must be live at the given Part
bool isLiveMust(MemLocObjectPtr mem, LiveDeadMemAnalysis* ldma, PartEdgePtr pedge, std::string indent);

// Returns true if the given MemLocObject may be live at the given Part
bool isLiveMay(MemLocObjectPtr mem, LiveDeadMemAnalysis* ldma, PartEdgePtr pedge, std::string indent);

}; // namespace fuse

#endif
