/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"

#include "ConstraintRepresentation.h"
#include <algorithm>
#include <iostream>
#include "CollectionOperators.h"
#include "CodeThornException.h"

using namespace std;
using namespace CodeThorn;

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void Constraint::negate() {
  switch(op()) {
  case EQ_VAR_CONST: _op=NEQ_VAR_CONST;break;
  case NEQ_VAR_CONST: _op=EQ_VAR_CONST;break;
  case EQ_VAR_VAR: _op=NEQ_VAR_VAR;break;
  case NEQ_VAR_VAR: _op=EQ_VAR_VAR;break;
  case DEQ: break;
  default:
    cerr<< "Error: unknown constraint operator."<<endl;
    exit(1);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
Constraint::ConstraintOp Constraint::op() const { return _op; }
AbstractValue Constraint::lhsVar() const {
  return _lhsVar; 
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
AbstractValue Constraint::rhsVar() const {
  if(isVarVarOp())
    return _rhsVar;
  else 
    throw CodeThorn::Exception( "Error: Constraint::rhsVar failed  (not a var-var operator).");
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
AbstractValue Constraint::rhsVal() const {
  if(isVarValOp()) {
    return _rhsVar;
  } else {
    throw CodeThorn::Exception( "Error: Constraint::rhsVal failed (not a var-val operator).");
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool Constraint::isVarVarOp() const {
    return (_op==EQ_VAR_VAR || _op==NEQ_VAR_VAR);
  }
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool Constraint::isVarValOp() const {
  bool iseq=(_op==EQ_VAR_CONST || _op==NEQ_VAR_CONST);
  return iseq;
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool Constraint::isEquation() const {
  return (_op==EQ_VAR_VAR || _op==EQ_VAR_CONST);
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool Constraint::isInequation() const {
  return (_op==NEQ_VAR_VAR || _op==NEQ_VAR_CONST);
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool Constraint::isDisequation() const {
  return ( _op==DEQ);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator<(const Constraint& c1, const Constraint& c2) {
  if(c1.lhsVar()!=c2.lhsVar())
    return c1.lhsVar()<c2.lhsVar();
  if(c1.op()!=c2.op())
    return c1.op()<c2.op();
  if(c1.op()==c2.op()) {
    switch(c1.op()) {
    case Constraint::DEQ:
      return false; // bugfix MS:4/26/17
    case Constraint::EQ_VAR_CONST:
    case Constraint::NEQ_VAR_CONST:
      return (c1.rhsVal()<c2.rhsVal());
    case Constraint::EQ_VAR_VAR:
    case Constraint::NEQ_VAR_VAR:
      return (c1.rhsVar()<c2.rhsVar());
    default:
      throw CodeThorn::Exception( "Error: Constraint::operator< unknown operator in constraint.");
    }
  } else {
    return false;
  }
  throw CodeThorn::Exception( "Error: Constraint::operator< failed.");
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator==(const Constraint& c1, const Constraint& c2) {
  if(c1.isDisequation()&&c2.isDisequation()) {
    return true;
  } else if( (c1.isDisequation()&&!c2.isDisequation()) || (!c1.isDisequation()&&c2.isDisequation()) ) {
    return false;
  } else {
    return (c1.lhsVar()==c2.lhsVar() && c1.op()==c2.op() 
            && ((c1.isVarValOp() && (c1.rhsVal()==c2.rhsVal()))
                || 
                (c1.isVarVarOp() && (c1.rhsVar()==c2.rhsVar()))
                )
            );
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator!=(const Constraint& c1, const Constraint& c2) {
  return !(c1==c2);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
Constraint::Constraint() {
  initialize();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
Constraint::Constraint(ConstraintOp op0,AbstractValue lhs, AbstractValue rhs):_op(op0),_lhsVar(lhs),_rhsVar(rhs) {
  //cout<<"DEBUG: Constraint::Constraint("<<op0<<","<<lhs.toString()<<","<<rhs.toString()<<endl;
  switch(op0) {
  case EQ_VAR_CONST:
  case NEQ_VAR_CONST:
    ROSE_ASSERT(_lhsVar.isPtr()&&_rhsVar.isConstInt());
    return;
  case EQ_VAR_VAR:
  case NEQ_VAR_VAR:
    ROSE_ASSERT(_lhsVar.isPtr()&&_rhsVar.isPtr());
    return;
  case DEQ:
    ROSE_ASSERT(_lhsVar.isBot()&&_rhsVar.isConstInt()); // see macro DISEQUALITYCONSTRAINT
    return;
  default:
    cerr<<"Error: Constraint constructor var-val operator: "<<op0<<". Wrong operator."<<endl;
    exit(1);
  }
} 

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string Constraint::toString() const {
  stringstream ss;
  if(isDisequation())
    return "V0##0";
  if(isVarVarOp()) {
    ss<<lhsVar().toString()<<(*this).opToString()<<rhsVar().toString();
  } else if(isVarValOp()) {
      ss<<lhsVar().toString()<<(*this).opToString()<<rhsVal().toString();
  } else {
      throw CodeThorn::Exception( "Error: Constraint::toString: unknown operator.");
  }
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string Constraint::toString(VariableIdMapping* variableIdMapping) const {
  stringstream ss;
  if(isDisequation())
    return "V0##0";
  if(isVarVarOp()) {
    ss<<lhsVar().toLhsString(variableIdMapping)<<(*this).opToString()<<rhsVar().toLhsString(variableIdMapping);
  } else if (isVarValOp()) {
    ss<<lhsVar().toLhsString(variableIdMapping)<<(*this).opToString()<<rhsVal().toLhsString(variableIdMapping);
  } else {
    throw CodeThorn::Exception( "Error: Constraint::toString: unknown operator.");
  }
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string Constraint::toAssertionString(VariableIdMapping* variableIdMapping) const {
  stringstream ss;
  if(isDisequation())
    return "false";
  if(isVarVarOp()) {
    ss<<lhsVar().toString(variableIdMapping)<<(*this).opToString()<<rhsVar().toString(variableIdMapping);
  } else if(isVarValOp()) {
    ss<<lhsVar().toString(variableIdMapping)<<(*this).opToString()<<rhsVal().toString(variableIdMapping);
  } else {
    throw CodeThorn::Exception( "Error: Constraint::toAssertionString: unknown operator.");
  }
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void Constraint::toStreamAsTuple(ostream& os) {
  os<<"(";
  os<<_op;
  os<<",";
  switch(_op) {
  case EQ_VAR_VAR:
  case NEQ_VAR_VAR:
  case DEQ:
    os<<_lhsVar.toString();
    os<<",";
    os<<_rhsVar.toString();;
    break;
  case EQ_VAR_CONST:
  case NEQ_VAR_CONST:
    os<<_lhsVar.toString();
    os<<",";
    os<<_rhsVar.getIntValue(); // TODO? toString()
    break;
  default:
    throw CodeThorn::Exception( "Constraint::toStream: unknown operator.");
  }
  os<<")";
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void Constraint::toStream(ostream& os) {
  os<<toString();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string Constraint::operatorStringFromStream(istream& is) {
  stringstream opss;
  string op;
  char c;
  while(is.peek()=='!'||is.peek()=='='||is.peek()=='#') {
    is>>c;
    opss<<c;
  }
  op=opss.str();
  if(op=="!="||op=="=="||op=="##")
    return op;
  else {
    throw CodeThorn::Exception( "Error: unknown operator in parsing constraint.");
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void Constraint::initialize() {
  _op=Constraint::UNDEFINED;
  AbstractValue varId;
  _lhsVar=varId;
  _rhsVar=varId;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string Constraint::opToString() const {
  switch(op()) {
  case EQ_VAR_VAR:
  case EQ_VAR_CONST: return "==";
  case NEQ_VAR_VAR:
  case NEQ_VAR_CONST: return "!=";
  case DEQ: return "##";
  default:
    cerr << "Error: Constraint: unknown operator: "<<op()<<endl;
    exit(1);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void Constraint::swapVars() {
  if(!isVarVarOp()) {
    throw CodeThorn::Exception( "Error: Constraint::swapVars on non var-var constraint.");
  }
  AbstractValue tmp=_lhsVar;
  _lhsVar=_rhsVar;
  _rhsVar=tmp;
}

ConstraintSet ConstraintSet::constraintsOfVariable(AbstractValue varId) const {
  ConstraintSet cset;
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
    Constraint c=*i;
    AbstractValue lhsVar=c.lhsVar();
    if(lhsVar==varId) {
      cset.insert(*i);
    }
    if(c.isVarVarOp()) {
      if(varId==c.rhsVar()) {
        cset.insert(*i);
      }
    }
  }
  return cset;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void ConstraintSet::addAssignEqVarVar(AbstractValue lhsVar, AbstractValue rhsVar) {
  // move removes all constraints on target var. arg1rhsVar->arg2lhsVar
  if(lhsVar==rhsVar)
    return;
  removeAllConstraintsOfVar(lhsVar);
  addEqVarVar(lhsVar,rhsVar);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void ConstraintSet::addEqVarVar(AbstractValue lhsVar, AbstractValue rhsVar) {
  // move removes all constraints on target var. arg1rhsVar->arg2lhsVar
  if(lhsVar==rhsVar)
    return;
  addConstraint(Constraint(Constraint::EQ_VAR_VAR,lhsVar,rhsVar));
}

#if 0
ConstraintSet ConstraintSet::invertedConstraints() {
  cerr << "WARNING: inverting constraints is not finished yet."<<endl;
  ConstraintSet result;
  // TODO: ensure that only a single equality constraint is inverted, otherwise this would be not correct.
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
    Constraint c=*i;
    switch(c.op()) {
    case Constraint::EQ_VAR_CONST:
      result.addConstraint(Constraint(Constraint::NEQ_VAR_CONST,c.lhsVar(),c.rhsVal()));
      break;
    case Constraint::NEQ_VAR_CONST:
      result.addConstraint(Constraint(Constraint::EQ_VAR_CONST,c.lhsVar(),c.rhsVal()));
      break;
    case Constraint::DEQ:
      result.addDisequality();
      break;
    default:
      throw CodeThorn::Exception( "Error: ConstraintSet::invertedConstraints: unknown or unsupported operator.");
    }
  }
  return result;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void ConstraintSet::invertConstraints() {
  cerr << "WARNING: inverting constraints is not finished yet."<<endl;
  for(ConstraintSet::iterator i=begin();i!=end();) {
    Constraint c=*i;
    ++i;
    switch(c.op()) {
    case Constraint::EQ_VAR_CONST:
      // c ist const because it is an element in a sorted set
      eraseConstraint(c); // we remove c from the set (but c remains unchanged and available)
      insertConstraint(Constraint(Constraint::NEQ_VAR_CONST,c.lhsVar(),c.rhsVal()));
      break;
    case Constraint::NEQ_VAR_CONST:
      // c ist const because it is an element in a sorted set
      eraseConstraint(c); // we remove c from the set (but c remains unchanged and available)
      insertConstraint(Constraint(Constraint::EQ_VAR_CONST,c.lhsVar(),c.rhsVal()));
      break;
    case Constraint::DEQ:
      // remains unchanged
      break;
    default:
      throw CodeThorn::Exception( "Error: ConstraintSet::invertedConstraints: unknown or unsupported operator.");
    }
  }
}
#endif 

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void  ConstraintSet::moveConstConstraints(AbstractValue fromVarId, AbstractValue toVarId) {
  if(fromVarId==toVarId)
    return; // duplication not necessary
  ConstraintSet::iterator i=begin();
  while(i!=end()) {
    Constraint c=*i++; // MS: increment MUST be done here
    if(c.isVarValOp() && c.lhsVar()==fromVarId) {
      addConstraint(Constraint(c.op(),toVarId,c.rhsVal()));
    }
  }
  eraseConstConstraints(fromVarId);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void ConstraintSet::eraseConstConstraints(AbstractValue varId)  {
  ConstraintSet::iterator i=begin();
  while(i!=end()) {
    if((*i).isVarValOp() && (*i).lhsVar()==varId)
      eraseConstraint(i++);
    else
      ++i;
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void  ConstraintSet::deleteAndMoveConstConstraints(AbstractValue fromVarId, AbstractValue toVarId) {
  if(fromVarId==toVarId)
    return; // duplication not necessary
  removeAllConstraintsOfVar(toVarId);
  moveConstConstraints(fromVarId,toVarId);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void  ConstraintSet::duplicateConstConstraints(AbstractValue varId1, AbstractValue varId2) {
  if(varId1==varId2)
    return; // duplication not necessary
  // we duplicate constraints for both variables
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
    Constraint c=*i;
    if(c.isVarValOp()) {
      if(c.lhsVar()==varId1) {
        addConstraint(Constraint(c.op(),varId2,c.rhsVal()));
      }
      if(c.lhsVar()==varId2) {
        addConstraint(Constraint(c.op(),varId1,c.rhsVal()));
      }
    }
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool ConstraintSet::disequalityExists() const {
  // disequality is normalized and always represented as 
  // a set with one element of type DIS.
  bool found=false;
  if(size()==1) {
    ConstraintSet::iterator i=begin();
    found=(*i).isDisequation();
  }
  return found;
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void ConstraintSet::addDisequality() {
  addConstraint(DISEQUALITYCONSTRAINT);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void ConstraintSet::addConstraint(Constraint c) {
  // we have to look which version of constraint already exists. Three cases are possible:
  // a) at most one const-equality constraint for the dedicated variable
  // b) arbitrary many inequality constraints for the dedicated variable
  // c) disequality (== constraint set represents information x==k && x!=k)
  if(disequalityExists())
      return;
  if(c.isVarVarOp()) {
    if(c.lhsVar()==c.rhsVar()) {
      // we do not add an x=x constraint
      return;
    }
#if 1
    if(c.lhsVar()<c.rhsVar()) {
      // ordering is OK, nothing to do
    } else {
      // normalization: ordering is not OK, swap (y=x ==> x=y)
      c.swapVars();
    }
#endif
  }
  AbstractValue dedicatedLhsVar=equalityMaintainer.determineDedicatedElement(c.lhsVar());
  switch(c.op()) {
    // attempt to insert x==k
  case Constraint::EQ_VAR_CONST: {
    // if x!=k exists ==> ##
    AbstractValue tmprhsVal=c.rhsVal();
    bool tmp=constraintExists(Constraint::NEQ_VAR_CONST,dedicatedLhsVar,tmprhsVal);
    if(tmp) {
      addDisequality();
      return;
    }
    // check if a different equality already exists
    ListOfAValue lst=getEqVarConst(dedicatedLhsVar);
    assert(lst.size()<=1);
    if(lst.size()==1) {
      AbstractValue val=*lst.begin();
      if(!(c.rhsVal()==val))
        addDisequality();
      return;
    }
    // all const-constraints can be removed (equality is most precise and is added)
    eraseConstConstraints(dedicatedLhsVar);
    set<Constraint>::insert(Constraint(Constraint::EQ_VAR_CONST,dedicatedLhsVar,c.rhsVal()));
    return;
  }
  case Constraint::NEQ_VAR_CONST: {
    // we attempt to insert x!=k

    // check if x==k exists. If yes, introduce x##k
    if(constraintExists(Constraint::EQ_VAR_CONST,dedicatedLhsVar,c.rhsVal())) {
      addDisequality();
      return;
    }
    // search for some x==m (where m must be different to k (otherwise we would have found it above))
    ConstraintSet::iterator i=findSpecific(Constraint::EQ_VAR_CONST,dedicatedLhsVar);
    if(i!=end()) {
      ROSE_ASSERT(!((*i).rhsVal() == c.rhsVal()));
      // we have found an equation x==m with m!=k ==> do not insert x!=k constraint (do nothing)
      return;
    } else {
      insertConstraint(Constraint(Constraint::NEQ_VAR_CONST,dedicatedLhsVar,c.rhsVal()));
      return;
    }
    break;
  }
  case Constraint::DEQ:
    // erase all existing constraints
    set<Constraint>::clear();
    set<Constraint>::insert(DISEQUALITYCONSTRAINT);
    return;

  case Constraint::EQ_VAR_VAR: {
    // TODO: maintain consistency wenn x=y is inserted but constraints of x and y are in conflict
    // currently we only add x=y for a fresh variable y on parameter passing and therefore this is OK.
    AbstractValue v1=dedicatedLhsVar;
    AbstractValue v2=equalityMaintainer.determineDedicatedElement(c.rhsVar());
    if(v1==v2) {
      // same dedicated variable => equality is already represented => nothing to do
      return;
    } else {
      // we need to move const-constraints from one variable to the new dedicated variable
      equalityMaintainer.addEquality(v1,v2);
      AbstractValue new_v1=equalityMaintainer.determineDedicatedElement(v1);
      AbstractValue new_v2=equalityMaintainer.determineDedicatedElement(v2);
      if(new_v1!=v1) {
        moveConstConstraints(v1,new_v1);
        if(!disequalityExists()) {
          eraseEqWithLhsVar(v1);
          insertConstraint(Constraint(Constraint::EQ_VAR_VAR,new_v1,v1));
        }
      } else if(new_v2!=v2) {
        moveConstConstraints(v2,new_v2);
        if(!disequalityExists()) {
          eraseEqWithLhsVar(v2);
          insertConstraint(Constraint(Constraint::EQ_VAR_VAR,new_v2,v2));
        }
      } else {
        cerr << "Internal error: AddConstraint:"<<endl;
        cerr << "Added x==y, but the two vars are mapped to different dedicated variables."<<endl;
        cerr << "This cannot be the case => programmatic error."<<endl;
        exit(1);
      }
    }
    return;
  }
  default:
    throw CodeThorn::Exception( "Internal error: ConstraintSet::insert: unknown operator.");
  }
  // all other cases (no constraint already in the set is effected)
  throw CodeThorn::Exception( "Internal error: ConstraintSet::insert: unknown combination of constraints.");
  //set<Constraint>::insert(c);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void ConstraintSet::eraseConstraint(set<Constraint>::iterator i) {
  assert(i!=end());
  set<Constraint>::erase(i);
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void ConstraintSet::eraseConstraint(Constraint c) {
  set<Constraint>::erase(c);
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void ConstraintSet::insertConstraint(Constraint c) {
  set<Constraint>::insert(c);
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void ConstraintSet::eraseEqWithLhsVar(AbstractValue v) {
  set<Constraint>::iterator i=begin();
  while(i!=end()) {
    if((*i).op()==Constraint::EQ_VAR_VAR && (*i).lhsVar()==v)
      eraseConstraint(i++);
    else
      ++i;
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
long ConstraintSet::numberOfConstConstraints(AbstractValue var) {
  long cnt=0;
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
    if((*i).isVarValOp() && (*i).lhsVar()==var) cnt++;
  }
  return cnt;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
#if 1
ConstraintSet CodeThorn::operator+(ConstraintSet s1, ConstraintSet s2) {
  ConstraintSet result=s1;
  for(ConstraintSet::iterator i2=s2.begin();i2!=s2.end();++i2)
    result.addConstraint(*i2);
  return result;
}
#else
ConstraintSet ConstraintSet::operator+(ConstraintSet& s2) {
  ConstraintSet result;
  result=*this;
  for(ConstraintSet::iterator i2=s2.begin();i2!=s2.end();++i2)
    result.addConstraint(*i2);
  return result;
}
#endif

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
ConstraintSet& ConstraintSet::operator+=(ConstraintSet& s2) {
  for(ConstraintSet::iterator i2=s2.begin();i2!=s2.end();++i2)
    addConstraint(*i2);
  return *this;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool ConstraintSet::constraintExists(Constraint::ConstraintOp op) const { 
  return constraintsWithOp(op).size()>0;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
ConstraintSet ConstraintSet::constraintsWithOp(Constraint::ConstraintOp op) const { 
  ConstraintSet cs;
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
    if((*i).op()==op)
      cs.addConstraint(*i);
  }
  return cs;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool ConstraintSet::constraintExists(Constraint::ConstraintOp op, AbstractValue varId, AbstractValue intVal) const { 
  Constraint tmp(op,varId,intVal);
  return constraintExists(tmp);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool ConstraintSet::constraintExists(const Constraint& tmp) const { 
  Constraint tmp2=tmp;
  tmp2.setLhsVar(equalityMaintainer.determineDedicatedElement(tmp.lhsVar()));
  ConstraintSet::const_iterator foundElemIter=find(tmp2);
  return foundElemIter!=end();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
ConstraintSet ConstraintSet::findSpecificSet(Constraint::ConstraintOp op, AbstractValue varId) const {
  ConstraintSet cs;
  // find op-constraint for variable varname
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
    if((*i).lhsVar()==varId && (*i).op()==op) {
      cs.addConstraint(Constraint((*i).op(),(*i).lhsVar(),(*i).rhsVal()));
    }
  }
  return cs;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
ConstraintSet::iterator ConstraintSet::findSpecific(Constraint::ConstraintOp op, AbstractValue varId) const {
  // find op-constraint for variable varname
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
    if((*i).lhsVar()==varId && (*i).op()==op)
      return i;
  }
  return end();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
AbstractValue ConstraintSet::varAbstractValue(const AbstractValue varId) const {
  AbstractValue c;
  AbstractValue dedicatedVarId=equalityMaintainer.determineDedicatedElement(varId);
  ConstraintSet::iterator i=findSpecific(Constraint::EQ_VAR_CONST,dedicatedVarId);
  if(i==end()) {
    // no EQ_VAR_CONST constraint for this variable
    return AbstractValue(CodeThorn::Top());
  } else {
    return AbstractValue((*i).rhsVal());
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SetOfVariableId ConstraintSet::getEqVars(const AbstractValue varId) const {
  return equalityMaintainer.equalElements(varId);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
ListOfAValue ConstraintSet::getEqVarConst(const AbstractValue varId) const {
  ListOfAValue s;
  AbstractValue dedicatedVarId=equalityMaintainer.determineDedicatedElement(varId);
  ConstraintSet::iterator i=findSpecific(Constraint::EQ_VAR_CONST,dedicatedVarId);
  if(i!=end()) {
    // there can exist at most one EQ_VAR_CONST for a variable
    s.push_back((*i).rhsVal());
  }
  return s;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
ListOfAValue ConstraintSet::getNeqVarConst(const AbstractValue varId) const {
  ListOfAValue s;
  AbstractValue dedicatedVarId=equalityMaintainer.determineDedicatedElement(varId);
  ConstraintSet cset=findSpecificSet(Constraint::NEQ_VAR_CONST,dedicatedVarId);
  for(ConstraintSet::iterator i=cset.begin();i!=cset.end();++i) {
    s.push_back((*i).rhsVal());
  }
  return s;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void ConstraintSet::removeAllConstraintsOfVar(AbstractValue varId) {
  // case 1: variable is a dedicated variable in a set with #set>=2
  // => find new dedicated variable and move all information and remove all
  // otherwise
  // => simply remove all (we do not loose information)

  bool isDedicatedVar=(equalityMaintainer.determineDedicatedElement(varId)==varId);
  AbstractValueSet equalVars=equalityMaintainer.equalElements(varId);
  if(isDedicatedVar && (equalVars.size()>=2)) {
    // find an element different to the dedicated var (the 2nd element must be correct)
    AbstractValueSet::iterator i=equalVars.begin();
    ++i;
    assert(i!=equalVars.end());
    AbstractValue someOtherVar=*i;
    assert(someOtherVar!=varId);
    equalityMaintainer.removeEqualities(varId);
    AbstractValue newDedicatedVar=equalityMaintainer.determineDedicatedElement(someOtherVar);
    moveConstConstraints(varId,newDedicatedVar);
    // the constraint set CANNOT become inconsistent
    assert(!disequalityExists());
    // we now continue and can remove all constraints of varId
  }
  // remove all constraints of varId
  equalityMaintainer.removeEqualities(varId);
  ConstraintSet::iterator i=begin();
  while(i!=end()) {
    if((*i).isVarValOp() && (*i).lhsVar()==varId)
      eraseConstraint(i++);
    else if((*i).isVarVarOp() && ((*i).lhsVar()==varId||(*i).rhsVar()==varId))
      eraseConstraint(i++);
    else
      ++i;
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string ConstraintSet::toString() const {
  stringstream ss;
  ss<<"{";
  for(set<Constraint>::iterator i=begin();i!=end();++i) {
    if(i!=begin()) 
      ss<<",";
    ss<<(*i).toString();
  }
  ss<<"}";
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */

string ConstraintSet::toStringWithoutBraces(VariableIdMapping* vim) const {
  stringstream ss;
  for(set<Constraint>::iterator i=begin();i!=end();++i) {
    if(i!=begin()) 
      ss<<",";
    ss<<(*i).toString(vim);
  }
  return ss.str();
}

string ConstraintSet::toString(VariableIdMapping* vim) const {
  stringstream ss;
  ss<<"{";
  ss<<toStringWithoutBraces(vim);
  ss<<"}";
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string ConstraintSet::toAssertionString(VariableIdMapping* vim) const {
  stringstream ss;
  for(set<Constraint>::iterator i=begin();i!=end();++i) {
    if(i!=begin()) 
      ss<<" && ";
    ss<<(*i).toAssertionString(vim);
  }
  return ss.str();
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
long ConstraintSet::memorySize() const {
  long mem=0;
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
    mem+=sizeof(*i);
  }
  return mem+sizeof(*this);
}

/*! 
  * \author MarkusSchordan
  * \date 2012.
 */
// strict weak ordering on two sets
bool CodeThorn::operator<(const ConstraintSet& s1, const ConstraintSet& s2) {
  if(s1.size()!=s2.size())
    return s1.size()<s2.size();
  ConstraintSet::iterator i=s1.begin();
  ConstraintSet::iterator j=s2.begin();
  while(i!=s1.end() && j!=s2.end()) {
    if(*i!=*j) {
      return *i<*j;
    } else {
      ++i;++j;
    }
  }
  assert(i==s1.end() && j==s2.end());
  return false; // both are equal
}

#if 0
bool operator==(const ConstraintSet& s1, const ConstraintSet& s2) {
  if(s1.size()==s2.size()) {
    ConstraintSet::iterator i=s1.begin(),j=s2.begin();
    while(i!=s1.end()) {
      if(*i!=*j)
        return false;
      i++;j++;
    }
    assert(i==s1.end() && j==s2.end()); // must hold for sets of equal size
    return true;
  } else {
    return false;
  }
}
bool operator!=(const ConstraintSet& s1, const ConstraintSet& s2) {
  return !(s1==s2);
}
#endif

string ConstraintSetMaintainer::toString() {
  stringstream ss;
  for(ConstraintSetMaintainer::iterator i=this->begin();
      i!=this->end();
      ++i) {
    ss<<"CS@:"<<*i<<":"<<(*i)->toString()<<endl;
  }
  return ss.str();
}
