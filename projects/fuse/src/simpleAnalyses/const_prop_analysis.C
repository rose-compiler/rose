#include "sage3basic.h"
#include "const_prop_analysis.h"

#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include <boost/make_shared.hpp>
  
using namespace std;
using namespace dbglog;

namespace fuse {
int constantPropagationAnalysisDebugLevel = 0;

// **********************************************************************
//              ConstantPropagationLattice
// **********************************************************************

CPValueObject::CPValueObject(PartEdgePtr pedge) : Lattice(pedge), FiniteLattice(pedge), ValueObject(NULL)
{
  this->value = 0;
  this->level = emptySet;
}

CPValueObject::CPValueObject(int v, PartEdgePtr pedge) : Lattice(pedge), FiniteLattice(pedge), ValueObject(NULL)
{
  this->value = v;
  this->level = constantValue;
}

CPValueObject::CPValueObject(short level, int v, PartEdgePtr pedge) : Lattice(pedge), FiniteLattice(pedge), ValueObject(NULL)
{
  this->value = v;
  this->level = level;
}

// This is the same as the implicit definition, so it might not be required to be defined explicitly.
// I am searching for the minimal example of the use of the data flow classes.
CPValueObject::CPValueObject(const CPValueObject & that) : Lattice(that.latPEdge), FiniteLattice(that.latPEdge), ValueObject(that)
{
  this->value = that.value;
  this->level = that.level;
}

int
CPValueObject::getValue() const
{
   return value;
}

short
CPValueObject::getLevel() const
{
   return level;
}
  
bool
CPValueObject::setValue(int x)
 {
  // These are more than access functions, they return if the state of the lattice has changed.
  bool modified = this->level != constantValue || this->value != value;
  this->value = x;
  level = constantValue;
  return modified;
}

bool
CPValueObject::setLevel(short x)
{
  // These are more than access functions, they return if the state of the lattice has changed.
  bool modified = this->level != x;
  level = x;
  return modified;
}

// **********************************************
// Required definition of pure virtual functions.
// **********************************************
void
CPValueObject::initialize()
{
  // Use the default constructor (implemented above).
  // So nothing to do here.
}


// returns a copy of this lattice
Lattice*
CPValueObject::copy() const
{
  return new CPValueObject(*this);
}


// overwrites the state of "this" Lattice with "that" Lattice
void
CPValueObject::copy(Lattice* X)
{
  Lattice::copy(X);
  CPValueObject* that = dynamic_cast<CPValueObject*>(X);

  this->value = that->value;
  this->level = that->level;
}


bool
CPValueObject::operator==(Lattice* X) /*const*/
{
  // Implementation of equality operator.
  CPValueObject* that = dynamic_cast<CPValueObject*>(X);
  return (value == that->value) && (level == that->level);
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool 
CPValueObject::meetUpdate(Lattice* X)
{
  CPValueObject* that = dynamic_cast<CPValueObject*>(X);
  assert(that);
  return meetUpdate(that);
}

bool
CPValueObject::meetUpdate(CPValueObject* that)
{
  // This is the more technically interesting required function.
  if (this->level == emptySet) {
    if(that->level == emptySet) {
      // leave it and return false
      return false;
    } else if (that->level == constantValue) {
      this->level = constantValue;
      this->value = that->value;
      return true;
    } else if (that->level == fullSet) {
      this->level = fullSet;
      return true;
    }
    assert(0); // We should never get here
  } else if (this->level == constantValue) {
    if(that->level == emptySet) {
      return false;
    } else if (that->level == constantValue) {
      if (this->value == that->value) {
        return false;
      } else {
        this->level = fullSet;
        return true;
      }
    } else if (that->level == fullSet) {
      this->level = fullSet;
      return true;
    }
    assert(0); // We should never get here
  } else if (this->level == fullSet) { 
    // Already at the fullSet. Cannot go up further.
    return false;
  }

  // Make up a return value for now.
  return false;
}

// Set this Lattice object to represent the set of all possible execution prefixes.
// Return true if this causes the object to change and false otherwise.
bool
CPValueObject::setToFull()
{
  // These are more than access functions, they return if the state of the lattice has changed.
  bool modified = this->level != fullSet;
  this->value = 0;
  level = fullSet;
  return modified;
}

// Set this Lattice object to represent the of no execution prefixes (empty set)
// Return true if this causes the object to change and false otherwise.
bool
CPValueObject::setToEmpty()
{
  // These are more than access functions, they return if the state of the lattice has changed.
  bool modified = this->level != emptySet;
  this->value = 0;
  level = emptySet;
  return modified;
}

// Set all the information associated Lattice object with this MemLocObjectPtr to full.
// Return true if this causes the object to change and false otherwise.
bool
CPValueObject::setMLValueToFull(MemLocObjectPtr ml)
{
  // Do nothing since this object does not contain information about MemLocObjects
  return false;
}

// Returns whether this lattice denotes the set of all possible execution prefixes.
bool
CPValueObject::isFull()
{ return level == fullSet; }

// Returns whether this lattice denotes the empty set.
bool
CPValueObject::isEmpty()
{ return level == emptySet; }

string
CPValueObject::str(string indent) const
{
  return strp(latPEdge, indent);
}

string
CPValueObject::strp(PartEdgePtr pedge, string indent) const
{    
  ostringstream outs;
  if(level == emptySet)
    outs << "[emptySet]";
  else if(level == constantValue)
    outs << "[const: "<<value<<"]";
  else if(level == fullSet)
    outs << "[fullSet]";
  else 
    assert(0);
  
  return outs.str();
}


bool CPValueObject::mayEqualV(ValueObjectPtr o, PartEdgePtr pedge)
{
  return mustEqualV(o, pedge);
}

bool CPValueObject::mustEqualV(ValueObjectPtr o, PartEdgePtr pedge)
{
  CPValueObjectPtr that = boost::dynamic_pointer_cast<CPValueObject>(o);
  if(!that) { return false; }
  return (value == that->value) && (level == that->level);
}

// Returns whether the two abstract objects denote the same set of concrete objects
bool CPValueObject::equalSet(AbstractObjectPtr o, PartEdgePtr pedge)
{
  CPValueObjectPtr that = boost::dynamic_pointer_cast<CPValueObject>(o);
  assert(that);
  if(level != that->level) return false;
  else {
    if(level == constantValue)
      return (value == that->value);
    // If the level is not a constant the sets are equal if the levels are the same.
    else
      return true;
  }
}

// Returns whether this abstract object denotes a non-strict subset (the sets may be equal) of the set denoted
// by the given abstract object.
bool CPValueObject::subSet(AbstractObjectPtr o, PartEdgePtr pedge)
{
  return true;
  CPValueObjectPtr that = boost::dynamic_pointer_cast<CPValueObject>(o);
  assert(that);
  /*dbg << "this->level="<<this->level<<" that->level="<<that->level<<endl;
  dbg << "this->value="<<this->value<<" that->value="<<that->value<<endl;*/
  // Smaller lattice values correspond to smaller sets
  if(level < that->level) { return true; }
  else {
    if(level == constantValue)
      return (value == that->value);
    // If the level is not a constant the sets are equal if the levels are the same.
    else
      return level == that->level;
  }
}

// Computes the meet of this and that and saves the result in this.
// Returns true if this causes this to change and false otherwise.
bool CPValueObject::meetUpdateV(ValueObjectPtr that_arg, PartEdgePtr pedge)
{
  CPValueObjectPtr that = boost::dynamic_pointer_cast<CPValueObject>(that_arg);
  assert(that);
  return meetUpdate(that.get());
}

// Returns whether this lattice denotes the set of all possible execution prefixes.
bool
CPValueObject::isFull(PartEdgePtr pedge)
{ return level == fullSet; }

// Returns whether this lattice denotes the empty set.
bool
CPValueObject::isEmpty(PartEdgePtr pedge)
{ return level == emptySet; }


// Allocates a copy of this object and returns a pointer to it
ValueObjectPtr CPValueObject::copyV() const
{
  return boost::make_shared<CPValueObject>(*this);
}

// Returns true if this ValueObject corresponds to a concrete value that is statically-known
bool CPValueObject::isConcrete()
{ return level==constantValue; }

// Returns the type of the concrete value (if there is one)
SgType* CPValueObject::getConcreteType()
{
  assert(isConcrete());
  // GB 2012-09-26: We only support integer types for our constants. This will need to be improved.
  return SageBuilder::buildIntType();
}

// Returns the concrete value (if there is one) as an SgValueExp, which allows callers to use
// the normal ROSE mechanisms to decode it
set<boost::shared_ptr<SgValueExp> > CPValueObject::getConcreteValue()
{
  assert(isConcrete());
  set<boost::shared_ptr<SgValueExp> > concreteVals;
  concreteVals.insert(boost::shared_ptr<SgValueExp>(SageBuilder::buildIntVal(value)));
  return concreteVals;
}

// **********************************************************************
//            ConstantPropagationAnalysisTransfer
// **********************************************************************

template <typename T>
void ConstantPropagationAnalysisTransfer::transferArith(SgBinaryOp *sgn, T transferOp)
   {
   CPValueObjectPtr arg1Lat, arg2Lat, resLat;
   getLattices(sgn, arg1Lat, arg2Lat, resLat);
      {
      transferOp(this, arg1Lat, arg2Lat, resLat);
      setLattice(sgn, resLat);
      if (isSgCompoundAssignOp(sgn))
        setLatticeOperand(sgn, sgn->get_lhs_operand(), resLat);
      }
   }

void
ConstantPropagationAnalysisTransfer::transferArith(SgBinaryOp *sgn, TransferOp transferOp)
   {
   transferArith(sgn, boost::mem_fn(transferOp));
   }

void 
ConstantPropagationAnalysisTransfer::transferIncrement(SgUnaryOp *sgn)
   {
   CPValueObjectPtr arg1Lat, arg2Lat, resLat;
   getLattices(sgn, arg1Lat, arg2Lat, resLat);
   transferAdditive(arg1Lat, arg2Lat, resLat, isSgPlusPlusOp(sgn));
   setLattice(sgn, resLat);
   // GB: shouldn't need to deallocate when we consistently use boost shared pointers
   //delete arg2Lat; // Allocated by getLattices
   setLatticeOperand(sgn, sgn->get_operand(), resLat);
   }

void
ConstantPropagationAnalysisTransfer::transferAdditive(CPValueObjectPtr arg1Lat, CPValueObjectPtr arg2Lat, CPValueObjectPtr resLat, bool isAddition)
   {
   if (arg1Lat->getLevel() == CPValueObject::emptySet || arg2Lat->getLevel() == CPValueObject::emptySet)
      {
      updateModified(resLat->setLevel(CPValueObject::emptySet));
      }
     else 
      {
      // Both knownValue
      if(arg1Lat->getLevel() == CPValueObject::constantValue && arg2Lat->getLevel() == CPValueObject::constantValue) 
         {
         updateModified(resLat->setValue(isAddition ? arg1Lat->getValue() + arg2Lat->getValue() : arg1Lat->getValue() - arg2Lat->getValue()));
         }
        else
         {
         // Else => Top
         updateModified(resLat->setLevel(CPValueObject::fullSet));
         }
      }
   }


void
ConstantPropagationAnalysisTransfer::transferMultiplicative(CPValueObjectPtr arg1Lat, CPValueObjectPtr arg2Lat, CPValueObjectPtr resLat)
   {
   if (arg1Lat->getLevel() == CPValueObject::emptySet || arg2Lat->getLevel() == CPValueObject::emptySet)
      {
      updateModified(resLat->setLevel(CPValueObject::emptySet));
      }
     else 
      {
     // Both knownValue
      if(arg1Lat->getLevel() == CPValueObject::constantValue && arg2Lat->getLevel() == CPValueObject::constantValue) 
         {
         updateModified(resLat->setValue(arg1Lat->getValue() * arg2Lat->getValue()));
         }
        else
         {
        // Else => Top
         updateModified(resLat->setLevel(CPValueObject::fullSet));
         }
      }
   }

void
ConstantPropagationAnalysisTransfer::transferDivision(CPValueObjectPtr arg1Lat, CPValueObjectPtr arg2Lat, CPValueObjectPtr resLat)
   {
   if (arg1Lat->getLevel() == CPValueObject::emptySet || arg2Lat->getLevel() == CPValueObject::emptySet)
      {
      updateModified(resLat->setLevel(CPValueObject::emptySet));
      }
     else 
      {
     // Both knownValue
      if(arg1Lat->getLevel() == CPValueObject::constantValue && arg2Lat->getLevel() == CPValueObject::constantValue) 
         {
         updateModified(resLat->setValue(arg1Lat->getValue() / arg2Lat->getValue()));
         }
        else
         {
        // Else => Top
         updateModified(resLat->setLevel(CPValueObject::fullSet));
         }
      }
   }

void
ConstantPropagationAnalysisTransfer::transferMod(CPValueObjectPtr arg1Lat, CPValueObjectPtr arg2Lat, CPValueObjectPtr resLat)
   {
   if (arg1Lat->getLevel() == CPValueObject::emptySet || arg2Lat->getLevel() == CPValueObject::emptySet)
      {
      updateModified(resLat->setLevel(CPValueObject::emptySet));
      }
     else 
      {
     // Both knownValue
      if(arg1Lat->getLevel() == CPValueObject::constantValue && arg2Lat->getLevel() == CPValueObject::constantValue) 
         {
         updateModified(resLat->setValue(arg1Lat->getValue() % arg2Lat->getValue()));
         }
        else
         {
        // Else => Top
         updateModified(resLat->setLevel(CPValueObject::fullSet));
         }
      }
   }

void
ConstantPropagationAnalysisTransfer::transferLogical(CPValueObjectPtr arg1Lat, CPValueObjectPtr arg2Lat, 
                                                     CPValueObjectPtr resLat, SgExpression* expr)
{
  if (arg1Lat->getLevel() == CPValueObject::emptySet || arg2Lat->getLevel() == CPValueObject::emptySet)
    updateModified(resLat->setLevel(CPValueObject::emptySet));
  else {
    // Both knownValue
    if(arg1Lat->getLevel() == CPValueObject::constantValue && arg2Lat->getLevel() == CPValueObject::constantValue) {
      switch(expr->variantT()) {
        case V_SgGreaterOrEqualOp: updateModified(resLat->setValue(arg1Lat->getValue() >= arg2Lat->getValue())); break;
        case V_SgGreaterThanOp:    updateModified(resLat->setValue(arg1Lat->getValue() >  arg2Lat->getValue())); break;
        case V_SgLessOrEqualOp:    updateModified(resLat->setValue(arg1Lat->getValue() <= arg2Lat->getValue())); break;
        case V_SgLessThanOp:       updateModified(resLat->setValue(arg1Lat->getValue() <  arg2Lat->getValue())); break;
        case V_SgEqualityOp:       updateModified(resLat->setValue(arg1Lat->getValue() == arg2Lat->getValue())); break;
        case V_SgNotEqualOp:       updateModified(resLat->setValue(arg1Lat->getValue() != arg2Lat->getValue())); break;
        
        // For logical operations these two we care about arg1 if index==1 and arg2 if index==2
        case V_SgAndOp: updateModified(resLat->setValue(arg1Lat->getValue() && arg2Lat->getValue())); break;
        case V_SgOrOp:  updateModified(resLat->setValue(arg1Lat->getValue() || arg2Lat->getValue())); break;
        
        default: assert(0);
      }
    } else {
      // Else => Top
      updateModified(resLat->setLevel(CPValueObject::fullSet));
    }
  }
}

// Transfer function for logical short-circuit operations: && and ||
void ConstantPropagationAnalysisTransfer::transferShortCircuitLogical(SgBinaryOp *sgn)
{
  CPValueObjectPtr arg1Lat, arg2Lat, resLat;
  arg1Lat = getLatticeOperand(sgn, sgn->get_lhs_operand());
  resLat  = getLattice(sgn);
  
  // If the first operand of the short-circuit operation has a known value and it is sufficient to determine
  // the operation's outcome
  if(arg1Lat->getLevel() == CPValueObject::constantValue &&
     ((isSgAndOp(sgn) && !arg1Lat->getValue()) ||
      (isSgOrOp(sgn)  &&  arg1Lat->getValue()))) {
    resLat->setValue(arg1Lat->getValue());
  // Otherwise, if the second value needs to be read to determine the operation's outcome
  } else {
    // Case 1: arg1's value was known but not sufficient
    if(arg1Lat->getLevel() == CPValueObject::constantValue) {
      arg2Lat = getLatticeOperand(sgn, sgn->get_rhs_operand());
      if(arg2Lat->getLevel() == CPValueObject::constantValue) {
        resLat->setValue(arg2Lat->getValue());
      // If arg2's value is uninitialized, then any value is valid for the result. Use uninitialized
      } else if(arg2Lat->getLevel() == CPValueObject::emptySet) {
        resLat->setToEmpty();
      // If arg2 may have multiple values, then the result may have multiple values
      } else if(arg2Lat->getLevel() == CPValueObject::fullSet) {
        resLat->setToFull();
      } else assert(0);
    // Else if, arg1's value is uninitialized, then anly value is valid for the result. Use arg2's v
    } else if(arg1Lat->getLevel() == CPValueObject::emptySet) {
      resLat->setToEmpty();
    // If arg1 may have multiple values, then the result may have multiple values
    } else if(arg1Lat->getLevel() == CPValueObject::fullSet) {
      resLat->setToFull();
    } else assert(0);
  }
}

/*
// Logical operations that include their own control flow due to short-circuit evaluation
void
ConstantPropagationAnalysisTransfer::transferControlLogical(CPValueObjectPtr arg1Lat, CPValueObjectPtr arg2Lat, 
                                                     CPValueObjectPtr resLat, SgExpression* expr)
{
  // If this is the first part of a control logical expression, which reads the first operand and decides whether to
  // check the second operand or exit out of the conditional
  if(cn.getIndex()==1) {
    if(arg1Lat->getLevel() == CPValueObject::emptySet)
      updateModified(resLat->setLevel(CPValueObject::emptySet));
    else if(arg1Lat->getLevel() == CPValueObject::constantValue)
      updateModified(resLat->setLevel(arg1->getValue()));
    else if(arg1Lat->getLevel() == CPValueObject::fullSet)
      updateModified(resLat->setLevel(CPValueObject::emptySet));
    assert(0);
  // Else, if this is the second part of a control logical expression, which we reach only if the first operand
  // is not sufficient to evaluate the expression
  } else if(cn.getIndex()==2) {
    if(arg2Lat->getLevel() == CPValueObject::emptySet)
      updateModified(resLat->setLevel(CPValueObject::emptySet));
    else if(arg2Lat->getLevel() == CPValueObject::constantValue)
      updateModified(resLat->setLevel(arg1->getValue()));
    else if(arg1Lat->getLevel() == CPValueObject::fullSet)
      updateModified(resLat->setLevel(CPValueObject::emptySet));
    assert(0);
  }
}*/

// Values
void
ConstantPropagationAnalysisTransfer::visit(SgLongLongIntVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgLongIntVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgIntVal *sgn)
   {
   //scope reg("ConstantPropagationAnalysisTransfer::visit(SgIntVal)", scope::low, constantPropagationAnalysisDebugLevel, 1);
   assert(sgn);
   //CPValueObjectPtr resLat = getLattice(sgn);
   CPValueObjectPtr resLat(new CPValueObject(sgn->get_value(), part->inEdgeFromAny()));
   assert(resLat);
   //resLat->setValue(sgn->get_value());
   //resLat->setLevel(CPValueObject::constantValue);
   setLattice(sgn, resLat);
   }

void
ConstantPropagationAnalysisTransfer::visit(SgShortVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgUnsignedLongLongIntVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgUnsignedLongVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgUnsignedIntVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgUnsignedShortVal *sgn)
   {
   }

void
ConstantPropagationAnalysisTransfer::visit(SgValueExp *sgn)
   {
   }

// Arithmetic Operations
void
ConstantPropagationAnalysisTransfer::visit(SgPlusAssignOp *sgn)
   {
   transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferAdditive, _1, _2, _3, _4, true ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgMinusAssignOp *sgn)
   {
   transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferAdditive, _1, _2, _3, _4, false));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgMultAssignOp *sgn)
   {
   transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferMultiplicative, _1, _2, _3, _4 ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgDivAssignOp *sgn)
   {
   transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferDivision, _1, _2, _3, _4 ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgModAssignOp *sgn)
   {
   transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferMod, _1, _2, _3, _4 ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgAddOp *sgn)
   {
   transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferAdditive, _1, _2, _3, _4, true ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgSubtractOp *sgn)
   {
   transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferAdditive, _1, _2, _3, _4, false));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgMultiplyOp *sgn)
   {
   transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferMultiplicative, _1, _2, _3, _4 ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgDivideOp *sgn)
   {
   transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferDivision, _1, _2, _3, _4 ));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgModOp *sgn)
   {
   transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferMod, _1, _2, _3, _4 ));
   }

// Increment Operations
void
ConstantPropagationAnalysisTransfer::visit(SgPlusPlusOp *sgn)
   {
   transferIncrement(sgn);
   }

void
ConstantPropagationAnalysisTransfer::visit(SgMinusMinusOp *sgn)
   {
   transferIncrement(sgn);
   }

// Unary Operations
void
ConstantPropagationAnalysisTransfer::visit(SgUnaryAddOp *sgn)
   {
   setLattice(sgn, getLatticeOperand(sgn, sgn->get_operand()));
   }

void
ConstantPropagationAnalysisTransfer::visit(SgMinusOp *sgn)
   {
   CPValueObjectPtr operandLat = getLatticeOperand(sgn, sgn->get_operand());

   // This fixes up the value if it is relevant (where level is neither fullSet not emptySet).
   operandLat->setValue(-operandLat->getValue());
   
   setLattice(sgn, operandLat);
   }

void 
ConstantPropagationAnalysisTransfer::visit(SgCastExp *sgn)
   {
   setLattice(sgn, getLatticeOperand(sgn, sgn->get_operand()));
   }

// Logical Operations
void
ConstantPropagationAnalysisTransfer::visit(SgGreaterOrEqualOp *sgn)
{ transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferLogical, _1, _2, _3, _4, sgn)); }

void
ConstantPropagationAnalysisTransfer::visit(SgGreaterThanOp *sgn)
{ transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferLogical, _1, _2, _3, _4, sgn)); }

void
ConstantPropagationAnalysisTransfer::visit(SgLessOrEqualOp *sgn)
{ transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferLogical, _1, _2, _3, _4, sgn)); }

void
ConstantPropagationAnalysisTransfer::visit(SgLessThanOp *sgn)
{ transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferLogical, _1, _2, _3, _4, sgn)); }

void
ConstantPropagationAnalysisTransfer::visit(SgEqualityOp *sgn)
{ transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferLogical, _1, _2, _3, _4, sgn)); }

void
ConstantPropagationAnalysisTransfer::visit(SgNotEqualOp *sgn)
{ transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferLogical, _1, _2, _3, _4, sgn)); }

void
ConstantPropagationAnalysisTransfer::visit(SgAndOp *sgn) { 
  // Only bother to consider operators with short-circuiting a the end of the operator so that
  // all of its operands precede the operator
  if(cn.getIndex()==2) 
    //transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferLogical, _1, _2, _3, _4, sgn));
    transferShortCircuitLogical(sgn);
}

void
ConstantPropagationAnalysisTransfer::visit(SgOrOp *sgn) {
  // Only bother to consider operators with short-circuiting a the end of the operator so that
  // all of its operands precede the operator
  if(cn.getIndex()==2)
    //transferArith(sgn, boost::bind(&ConstantPropagationAnalysisTransfer::transferLogical, _1, _2, _3, _4, sgn));
    transferShortCircuitLogical(sgn);
}

bool
ConstantPropagationAnalysisTransfer::finish()
   {
   return modified;
   }

ConstantPropagationAnalysisTransfer::ConstantPropagationAnalysisTransfer(
          PartPtr part, CFGNode cn, NodeState& state, 
          map<PartEdgePtr, vector<Lattice*> >& dfInfo, 
          Composer* composer, ConstantPropagationAnalysis* analysis)
   : VariableStateTransfer<CPValueObject>(state, dfInfo, boost::make_shared<CPValueObject>(part->inEdgeFromAny()), 
                                          composer, analysis, part, cn, constantPropagationAnalysisDebugLevel)
   {
   }




// **********************************************************************
//             ConstantPropagationAnalysis
// **********************************************************************

// GB: Is this needed for boost shared-pointers?
ConstantPropagationAnalysis::ConstantPropagationAnalysis()
{
}

// Initializes the state of analysis lattices at the given function, part and edge into our out of the part
// by setting initLattices to refer to freshly-allocated Lattice objects.
void ConstantPropagationAnalysis::genInitLattice(PartPtr part, PartEdgePtr pedge, 
                                                 vector<Lattice*>& initLattices)
{
  AbstractObjectMap* l = new AbstractObjectMap(boost::make_shared<CPValueObject>(pedge/*part->inEdgeFromAny()*/),
                                               pedge,
                                               getComposer(), this);
  /*dbg << "ConstantPropagationAnalysis::initializeState, analysis="<<returning l="<<l<<" n=<"<<escape(p.getNode()->unparseToString())<<" | "<<p.getNode()->class_name()<<" | "<<p.getIndex()<<">\n";
  dbg << "    l="<<l->str("    ")<<endl;*/
  initLattices.push_back(l);

  // GB: WE NEED TO INITIALIZE THIS LATTICE WITH THE CURRENTLY LIVE VARIABLES. E.G. AS INITIALIZATION-TIME
}
  
bool
ConstantPropagationAnalysis::transfer(PartPtr p, CFGNode cn, NodeState& state, 
                                      map<PartEdgePtr, vector<Lattice*> >& dfInfo)
   {
   assert(0); 
   return false;
   }

boost::shared_ptr<DFTransferVisitor>
ConstantPropagationAnalysis::getTransferVisitor(PartPtr part, CFGNode cn, NodeState& state, 
                                                map<PartEdgePtr, vector<Lattice*> >& dfInfo)
   {
  // Why is the boost shared pointer used here?
   ConstantPropagationAnalysisTransfer* t = new ConstantPropagationAnalysisTransfer(part, cn, state, dfInfo, getComposer(), this);
   return boost::shared_ptr<DFTransferVisitor>(t);
   }

ValueObjectPtr ConstantPropagationAnalysis::Expr2Val(SgNode* n, PartEdgePtr pedge)
{
  if(constantPropagationAnalysisDebugLevel>=1) dbg << "ConstantPropagationAnalysis::Expr2Val(n="<<SgNode2Str(n)<<", pedge="<<pedge->str()<<")"<<endl;
  // If pedge doesn't have wildcards
  if(pedge->source() && pedge->target()) {
    NodeState* state = NodeState::getNodeState(this, pedge->source());
    //dbg << "state="<<state->str(this)<<endl;
    AbstractObjectMap* cpMap = dynamic_cast<AbstractObjectMap*>(state->getLatticeBelow(this, pedge, 0));
    if(cpMap == NULL) {
      Lattice* l = state->getLatticeBelow(this, pedge, 0);
      dbg << "l="<<l->str()<<endl;
    }
    assert(cpMap);
    
    // MemLocObjectPtrPair p = composer->Expr2MemLoc(n, pedge, this);
    MemLocObjectPtr p = composer->Expr2MemLoc(n, pedge, this);
    if(constantPropagationAnalysisDebugLevel>=2) {
      indent ind;
      dbg << "&nbsp;&nbsp;&nbsp;&nbsp;p="<<p->str()<<endl;
      dbg << "cpMap Below="<<cpMap<<"="<<cpMap->str()<<endl;
      
      dbg << "nodeState = "<<state->str()<<endl;
    }

    // Return the lattice associated with n's expression since that is likely to be more precise
    // but if it is not available, used the memory object
    // return (p.expr ? boost::dynamic_pointer_cast<ValueObject>(cpMap->get(p.expr)) :
    //                  boost::dynamic_pointer_cast<ValueObject>(cpMap->get(p.mem)));
    return (boost::dynamic_pointer_cast<ValueObject>(cpMap->get(p)));
  // If the target of this edge is a wildcard
  } else if(pedge->source()) {
    NodeState* state = NodeState::getNodeState(this, pedge->source());
    //dbg << "state="<<state->str(this)<<endl;
    
    // Merge the lattices along all the outgoing edges
    map<PartEdgePtr, vector<Lattice*> >& e2lats = state->getLatticeBelowAllMod(this);
    assert(e2lats.size()>=1);
    boost::shared_ptr<CPValueObject> mergedVal = boost::make_shared<CPValueObject>(pedge);
    for(map<PartEdgePtr, vector<Lattice*> >::iterator lats=e2lats.begin(); lats!=e2lats.end(); lats++) {
      PartEdge* edgePtr = lats->first.get();
      assert(edgePtr->source() == pedge.get()->source());
      
      AbstractObjectMap* cpMap = dynamic_cast<AbstractObjectMap*>(state->getLatticeBelow(this, lats->first, 0));
      assert(cpMap);
      
      // MemLocObjectPtrPair p = composer->Expr2MemLoc(n, pedge, this);
      MemLocObjectPtr p = composer->Expr2MemLoc(n, pedge, this);
      if(constantPropagationAnalysisDebugLevel>=2) {
        indent ind;
        dbg << "&nbsp;&nbsp;&nbsp;&nbsp;p="<<p->str()<<endl;
        dbg << "cpMap="<<cpMap<<"="<<cpMap->str()<<endl;
      }
      
      // boost::shared_ptr<CPValueObject> val = 
      //         boost::dynamic_pointer_cast<CPValueObject>
      //           (p.expr ? boost::dynamic_pointer_cast<ValueObject>(cpMap->get(p.expr)) :
      //                     boost::dynamic_pointer_cast<ValueObject>(cpMap->get(p.mem)));
      boost::shared_ptr<CPValueObject> val = 
        boost::dynamic_pointer_cast<CPValueObject> (boost::dynamic_pointer_cast<ValueObject>(cpMap->get(p)));

      mergedVal->meetUpdate(val.get());
    }
    return mergedVal;
  // If the source of this edge is a wildcard
  } else if(pedge->target()) {
    NodeState* state = NodeState::getNodeState(this, pedge->target());
    if(constantPropagationAnalysisDebugLevel>=2) dbg << "state="<<state->str()<<endl;
    AbstractObjectMap* cpMap = dynamic_cast<AbstractObjectMap*>(state->getLatticeAbove(this, NULLPartEdge, 0));
    assert(cpMap);
    
    // MemLocObjectPtrPair p = composer->Expr2MemLoc(n, pedge, this);
    MemLocObjectPtr p = composer->Expr2MemLoc(n, pedge, this);
    if(constantPropagationAnalysisDebugLevel>=2) {
      indent ind;
      dbg << "&nbsp;&nbsp;&nbsp;&nbsp;p="<<p->str()<<endl;
      dbg << "cpMap="<<cpMap<<"="<<cpMap->str()<<endl;
    }

    // Return the lattice associated with n's expression since that is likely to be more precise
    // but if it is not available, used the memory object
    // return (p.expr ? boost::dynamic_pointer_cast<ValueObject>(cpMap->get(p.expr)) :
    //                  boost::dynamic_pointer_cast<ValueObject>(cpMap->get(p.mem)));
    return boost::dynamic_pointer_cast<ValueObject>(cpMap->get(p));
  }
  assert(0);
}

}; // namespace fuse;
