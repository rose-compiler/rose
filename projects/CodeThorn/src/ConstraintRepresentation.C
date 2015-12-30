/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"

#include "ConstraintRepresentation.h"
#include <algorithm>
#include <iostream>
#include "CollectionOperators.h"


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
    cerr<< "Error: unkown constraint operator."<<endl;
    exit(1);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
Constraint::ConstraintOp Constraint::op() const { return _op; }
VariableId Constraint::lhsVar() const {
  return _lhsVar; 
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
VariableId Constraint::rhsVar() const {
  if(isVarVarOp())
    return _rhsVar;
  else
    throw "Error: Constraint::rhsVar failed.";
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
AValue Constraint::rhsVal() const {
  if(isVarValOp())
    return _intVal; 
  else
    throw "Error: Constraint::rhsVal failed.";
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
//CppCapsuleAValue Constraint::rhsValCppCapsule() const { return _intVal; }

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
    case Constraint::EQ_VAR_CONST:
    case Constraint::NEQ_VAR_CONST:
    case Constraint::DEQ:
      return (c1.rhsVal()<c2.rhsVal());
    case Constraint::EQ_VAR_VAR:
    case Constraint::NEQ_VAR_VAR:
      return (c1.rhsVar()<c2.rhsVar());
    default:
      throw "Error: Constraint::operator< unknown operator in constraint.";
    }
  } else {
    return false;
  }
  throw "Error: Constraint::operator< failed.";
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator==(const Constraint& c1, const Constraint& c2) {
  return 
    c1.lhsVar()==c2.lhsVar() && c1.op()==c2.op() 
    && ((c1.isVarValOp() && (c1.rhsVal()==c2.rhsVal()))
        || 
        (c1.isVarVarOp() && (c1.rhsVar()==c2.rhsVar()))
       )
    ;
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
Constraint::Constraint(ConstraintOp op0,VariableId lhs, AValue rhs):_op(op0),_lhsVar(lhs),_rhsVar(VariableId()),_intVal(rhs) {
  switch(op0) {
  case EQ_VAR_CONST:
  case NEQ_VAR_CONST:
  case DEQ:
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
#if 0
Constraint::Constraint(ConstraintOp op0,VariableId lhs, CppCapsuleAValue rhs):_op(op0),_lhsVar(lhs),_rhsVar(VariableId()),_intVal(rhs) {
  switch(op0) {
  case EQ_VAR_CONST:
  case NEQ_VAR_CONST:
  case DEQ:
    return;
  default:
    cerr<<"Error: Constraint constructor var-caps(val) operator: "<<op0<<". Wrong operator."<<endl;
    exit(1);
  } 
} 
#endif
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
Constraint::Constraint(ConstraintOp op0,VariableId lhs, VariableId rhs):_op(op0),_lhsVar(lhs),_rhsVar(rhs),_intVal(0) {
  switch(op0) {
  case EQ_VAR_VAR:
  case NEQ_VAR_VAR:
  case DEQ:
    return;
  default:
    cerr<<"Error: Constraint constructor var-var operator: "<<op0<<". Wrong operator."<<endl;
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
  if(isVarVarOp())
    ss<<lhsVar().toString()<<(*this).opToString()<<rhsVar().toString();
  else {
    if(isVarValOp())
      ss<<lhsVar().toString()<<(*this).opToString()<<rhsVal().toString();
    else
      throw "Error: Constraint::toString: unknown operator.";
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
    return "__##__";
  if(isVarVarOp())
    ss<<variableIdMapping->uniqueLongVariableName(lhsVar())<<(*this).opToString()<<variableIdMapping->uniqueLongVariableName(rhsVar());
  else {
    if(isVarValOp())
      ss<<variableIdMapping->uniqueLongVariableName(lhsVar())<<(*this).opToString()<<rhsVal().toString();
    else
      throw "Error: Constraint::toString: unknown operator.";
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
  if(isVarVarOp())
    ss<<variableIdMapping->uniqueLongVariableName(lhsVar())<<(*this).opToString()<<variableIdMapping->uniqueLongVariableName(rhsVar());
  else {
    if(isVarValOp())
      ss<<variableIdMapping->uniqueLongVariableName(lhsVar())<<(*this).opToString()<<rhsVal().toString();
    else
      throw "Error: Constraint::toString: unknown operator.";
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
    os<<(_intVal.getIntValue());
    break;
  default:
    throw "Constraint::toStream: unknown operator.";
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
    throw "Error: unknown operator in parsing constraint.";
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void Constraint::initialize() {
  _op=Constraint::UNDEFINED;
  VariableId varId;
  _lhsVar=varId;
  _rhsVar=varId;
  _intVal=AValue(0);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void Constraint::fromStream(istream& is) {
  char c;
  string s;
  int __varIdCode=-1; 
  VariableId __varId; 
  AValue __varAValue; 
  is>>c;
  if(c!='V') throw "Error: Syntax error Constraint. Expected VariableId.";
  is>>__varIdCode;
  assert(__varIdCode>=0);
  __varId.setIdCode(__varIdCode);
  _lhsVar=__varId;
  string op=operatorStringFromStream(is);
  if(is.peek()=='V') {
    // case: varop var
    is>>c; // consume 'V'
    is>>__varIdCode;
    assert(__varIdCode>=0);
    __varId.setIdCode(__varIdCode);
    _rhsVar=__varId;
    if(op=="==") _op=EQ_VAR_VAR;
    if(op=="!=") _op=NEQ_VAR_VAR;
    if(op=="##") _op=DEQ; // special case
  } else {
    // case: var op const
    is>>__varAValue;
    _intVal=__varAValue;
    if(op=="==") _op=EQ_VAR_CONST;
    if(op=="!=") _op=NEQ_VAR_CONST;
  }
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
    throw "Error: Constraint::swapVars on non var-var constraint.";
  }
  VariableId tmp=_lhsVar;
  _lhsVar=_rhsVar;
  _rhsVar=tmp;
}

ConstraintSet ConstraintSet::constraintsOfVariable(VariableId varId) const {
  ConstraintSet cset;
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
    Constraint c=*i;
    VariableId lhsVar=c.lhsVar();
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
void ConstraintSet::addAssignEqVarVar(VariableId lhsVar, VariableId rhsVar) {
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
void ConstraintSet::addEqVarVar(VariableId lhsVar, VariableId rhsVar) {
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
      throw "Error: ConstraintSet::invertedConstraints: unknown or unsupported operator.";
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
      throw "Error: ConstraintSet::invertedConstraints: unknown or unsupported operator.";
    }
  }
}
#endif 

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void  ConstraintSet::moveConstConstraints(VariableId fromVarId, VariableId toVarId) {
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
void ConstraintSet::eraseConstConstraints(VariableId varId)  {
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
void  ConstraintSet::deleteAndMoveConstConstraints(VariableId fromVarId, VariableId toVarId) {
  if(fromVarId==toVarId)
    return; // duplication not necessary
  removeAllConstraintsOfVar(toVarId);
  moveConstConstraints(fromVarId,toVarId);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void  ConstraintSet::duplicateConstConstraints(VariableId varId1, VariableId varId2) {
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
  VariableId dedicatedLhsVar=equalityMaintainer.determineDedicatedElement(c.lhsVar());
  switch(c.op()) {
    // attempt to insert x==k
  case Constraint::EQ_VAR_CONST: {
    // if x!=k exists ==> ##
    AValue tmprhsVal=c.rhsVal();
    bool tmp=constraintExists(Constraint::NEQ_VAR_CONST,dedicatedLhsVar,tmprhsVal);
    if(tmp) {
      addDisequality();
      return;
    }
    // check if a different equality already exists
    ListOfAValue lst=getEqVarConst(dedicatedLhsVar);
    assert(lst.size()<=1);
    if(lst.size()==1) {
      AValue val=*lst.begin();
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
      assert(!((*i).rhsVal() == c.rhsVal()));
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
    VariableId v1=dedicatedLhsVar;
    VariableId v2=equalityMaintainer.determineDedicatedElement(c.rhsVar());
    if(v1==v2) {
      // same dedicated variable => equality is already represented => nothing to do
      return;
    } else {
      // we need to move const-constraints from one variable to the new dedicated variable
      equalityMaintainer.addEquality(v1,v2);
      VariableId new_v1=equalityMaintainer.determineDedicatedElement(v1);
      VariableId new_v2=equalityMaintainer.determineDedicatedElement(v2);
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
    throw "Internal error: ConstraintSet::insert: unknown operator.";
  }
  // all other cases (no constraint already in the set is effected)
  throw "Internal error: ConstraintSet::insert: unknown combination of constraints.";
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
void ConstraintSet::eraseEqWithLhsVar(VariableId v) {
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
long ConstraintSet::numberOfConstConstraints(VariableId var) {
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
#if 0
bool ConstraintSet::constraintExists(Constraint::ConstraintOp op, VariableId varId, CppCapsuleAValue intVal) const { 
  Constraint tmp(op,varId,intVal);
  return constraintExists(tmp);
}
#endif

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool ConstraintSet::constraintExists(Constraint::ConstraintOp op, VariableId varId, AValue intVal) const { 
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
ConstraintSet ConstraintSet::findSpecificSet(Constraint::ConstraintOp op, VariableId varId) const {
  ConstraintSet cs;
  // find op-constraint for variable varname
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
    if((*i).lhsVar()==varId && (*i).op()==op)
      cs.addConstraint(Constraint((*i).op(),(*i).lhsVar(),(*i).rhsVal()));
  }
  return cs;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
ConstraintSet::iterator ConstraintSet::findSpecific(Constraint::ConstraintOp op, VariableId varId) const {
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
AType::ConstIntLattice ConstraintSet::varConstIntLatticeValue(const VariableId varId) const {
  AType::ConstIntLattice c;
  VariableId dedicatedVarId=equalityMaintainer.determineDedicatedElement(varId);
  ConstraintSet::iterator i=findSpecific(Constraint::EQ_VAR_CONST,dedicatedVarId);
  if(i==end()) {
    // no EQ_VAR_CONST constraint for this variable
    return AType::ConstIntLattice(AType::Top());
  } else {
    return AType::ConstIntLattice((*i).rhsVal());
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SetOfVariableId ConstraintSet::getEqVars(const VariableId varId) const {
  return equalityMaintainer.equalElements(varId);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
ListOfAValue ConstraintSet::getEqVarConst(const VariableId varId) const {
  ListOfAValue s;
  VariableId dedicatedVarId=equalityMaintainer.determineDedicatedElement(varId);
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
ListOfAValue ConstraintSet::getNeqVarConst(const VariableId varId) const {
  ListOfAValue s;
  VariableId dedicatedVarId=equalityMaintainer.determineDedicatedElement(varId);
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
void ConstraintSet::removeAllConstraintsOfVar(VariableId varId) {
  // case 1: variable is a dedicated variable in a set with #set>=2
  // => find new dedicated variable and move all information and remove all
  // otherwise
  // => simply remove all (we do not loose information)

  bool isDedicatedVar=(equalityMaintainer.determineDedicatedElement(varId)==varId);
  VariableIdMapping::VariableIdSet equalVars=equalityMaintainer.equalElements(varId);
  if(isDedicatedVar && (equalVars.size()>=2)) {
    // find an element different to the dedicated var (the 2nd element must be correct)
    VariableIdMapping::VariableIdSet::iterator i=equalVars.begin();
    ++i;
    assert(i!=equalVars.end());
    VariableId someOtherVar=*i;
    assert(someOtherVar!=varId);
    equalityMaintainer.removeEqualities(varId);
    VariableId newDedicatedVar=equalityMaintainer.determineDedicatedElement(someOtherVar);
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
