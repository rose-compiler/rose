/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "ConstraintRepresentation.h"
#include <algorithm>
#include <iostream>
#include "CollectionOperators.h"


using namespace std;

void Constraint::negate() {
  switch(op()) {
  case EQ_VAR_CONST: _op=NEQ_VAR_CONST;break;
  case NEQ_VAR_CONST: _op=EQ_VAR_CONST;break;
  case DEQ_VAR_CONST: break;
  case EQ_VAR_VAR: _op=NEQ_VAR_VAR;break;
  case NEQ_VAR_VAR: _op=EQ_VAR_VAR;break;
  case DEQ_VAR_VAR: break;
  default:
	cerr<< "Error: unkown constraint operator."<<endl;
	exit(1);
  }
}

Constraint::ConstraintOp Constraint::op() const { return _op; }
VariableId Constraint::lhsVar() const { return _lhsVar; }
VariableId Constraint::rhsVar() const { return _rhsVar; }
AValue Constraint::rhsVal() const { return _intVal.getValue(); }
CppCapsuleAValue Constraint::rhsValCppCapsule() const { return _intVal; }

bool Constraint::isVarVarOp() const {
	return (_op==EQ_VAR_VAR || _op==NEQ_VAR_VAR || _op==DEQ_VAR_VAR);
  }
bool Constraint::isVarValOp() const {
  return (_op==EQ_VAR_CONST || _op==NEQ_VAR_CONST || _op==DEQ_VAR_CONST);
}
bool Constraint::isEquation() const {
  return (_op==EQ_VAR_VAR || _op==EQ_VAR_CONST);
}
bool Constraint::isInequation() const {
  return (_op==NEQ_VAR_VAR || _op==NEQ_VAR_CONST);
}
bool Constraint::isDisequation() const {
  return (_op==DEQ_VAR_VAR || _op==DEQ_VAR_CONST);
}

bool operator<(const Constraint& c1, const Constraint& c2) {
  if(c1.lhsVar()!=c2.lhsVar())
	return c1.lhsVar()<c2.lhsVar();
  if(c1.op()!=c2.op())
	return c1.op()<c2.op();
  if(c1.op()==c2.op()) {
	switch(c1.op()) {
	case Constraint::EQ_VAR_CONST:
	case Constraint::NEQ_VAR_CONST:
	case Constraint::DEQ_VAR_CONST:
	  return (c1.rhsValCppCapsule()<c2.rhsValCppCapsule());
	case Constraint::EQ_VAR_VAR:
	case Constraint::NEQ_VAR_VAR:
	case Constraint::DEQ_VAR_VAR:
	  return (c1.rhsVar()<c2.rhsVar());
	default:
	  throw "Error: Constraint::operator< unknown operator in constraint.";
	}
  } else {
	return false;
  }
  throw "Error: Constraint::operator< failed.";
}

bool operator==(const Constraint& c1, const Constraint& c2) {
  return 
	c1.lhsVar()==c2.lhsVar() && c1.op()==c2.op() 
	&& ((c1.isVarValOp() && (c1.rhsValCppCapsule()==c2.rhsValCppCapsule()))
		|| 
		(c1.isVarVarOp() && (c1.rhsVar()==c2.rhsVar()))
	   )
	;
}

bool operator!=(const Constraint& c1, const Constraint& c2) {
  return !(c1==c2);
}

Constraint::Constraint(ConstraintOp op0,VariableId lhs, AValue rhs):_op(op0),_lhsVar(lhs),_intVal(CppCapsuleAValue(rhs)) {
} 
Constraint::Constraint(ConstraintOp op0,VariableId lhs, CppCapsuleAValue rhs):_op(op0),_lhsVar(lhs),_intVal(rhs) {
} 
Constraint::Constraint(ConstraintOp op0,VariableId lhs, VariableId rhs):_op(op0),_lhsVar(lhs),_rhsVar(rhs) {
} 

string Constraint::toString() const {
  stringstream ss;
  if(isDisequation())
	return "$##0";
  if(isVarVarOp())
	ss<<lhsVar().longVariableName()<<(*this).opToString()<<rhsVar().longVariableName();
  else {
	if(isVarValOp())
	  ss<<lhsVar().longVariableName()<<(*this).opToString()<<rhsVal().toString();
	else
	  throw "Error: Constraint::toString: unknown operator.";
  }
  return ss.str();
}

string Constraint::opToString() const {
  switch(op()) {
  case EQ_VAR_VAR:
  case EQ_VAR_CONST: return "==";
  case NEQ_VAR_CONST: return "!=";
  case DEQ_VAR_CONST: return "##";
  default:
	cerr << "Error: Constraint: unknown operator"<<endl;
	exit(1);
  }
}

void Constraint::swapVars() {
  if(!isVarVarOp()) {
	throw "Error: Constraint::swapVars on non var-var constraint.";
  }
  VariableId tmp=_lhsVar;
  _lhsVar=_rhsVar;
  _rhsVar=tmp;
}

bool ConstraintSet::deqConstraintExists() const {
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if((*i).op()==Constraint::DEQ_VAR_CONST)
	  return true;
  }
  return false;
}

void ConstraintSet::addAssignEqVarVar(VariableId lhsVar, VariableId rhsVar) {
  //TODO: check for const-sets
  moveConstConstraints(lhsVar, rhsVar); // duplication direction from right to left (as in an assignment)
  addConstraint(Constraint(Constraint::EQ_VAR_VAR,lhsVar,rhsVar));
}
void ConstraintSet::removeEqVarVar(VariableId lhsVar, VariableId rhsVar) {
  //TODO: move const-constraints if necessary
  removeConstraint(Constraint(Constraint::EQ_VAR_VAR,lhsVar,rhsVar));
}

ConstraintSet ConstraintSet::invertedConstraints() {
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
	case Constraint::DEQ_VAR_CONST:
	  result.addDisequality();
	  break;
	  // we do not extract VAR_VAR constraints from expressions. We only invert constraint sets which have been extracted from expressions.
	  // therefore VAR_VAR constraints are not supposed to be inverted.
#if 0
	case Constraint::EQ_VAR_VAR:
	  result.addConstraint(Constraint(Constraint::NEQ_VAR_VAR,c.lhsVar(),c.rhsVar()));
	  break;
	case Constraint::NEQ_VAR_VAR:
	  result.addConstraint(Constraint(Constraint::EQ_VAR_VAR,c.lhsVar(),c.rhsVar()));
	  break;
	case Constraint::DEQ_VAR_VAR:
	  result.addDisequality();
	  break;
#endif
	default:
	  throw "Error: ConstraintSet::invertedConstraints: unknown or unsupported operator.";
	}
  }
  return result;
}

void ConstraintSet::invertConstraints() {
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	Constraint c=*i;
	switch(c.op()) {
	case Constraint::EQ_VAR_CONST:
	  // c ist const because it is an element in a sorted set
	  removeConstraint(c); // we remove c from the set (but c remains unchanged and available)
	  addConstraint(Constraint(Constraint::NEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	  break;
	case Constraint::NEQ_VAR_CONST:
	  // c ist const because it is an element in a sorted set
	  removeConstraint(c); // we remove c from the set (but c remains unchanged and available)
	  addConstraint(Constraint(Constraint::EQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	  break;
	case Constraint::DEQ_VAR_CONST:
	  // remains unchanged
	  break;
	default:
	  throw "Error: ConstraintSet::invertedConstraints: unknown or unsupported operator.";
	}
  }
}

void  ConstraintSet::moveConstConstraints(VariableId lhsVarId, VariableId rhsVarId) {
  if(lhsVarId==rhsVarId)
	return; // duplication not necessary
  deleteConstraints(lhsVarId);
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	Constraint c=*i;
	if(c.isVarValOp() && c.lhsVar()==rhsVarId) {
	  addConstraint(Constraint(c.op(),lhsVarId,c.rhsValCppCapsule()));
	}
  }
}

void  ConstraintSet::duplicateConstConstraints(VariableId lhsVarId, VariableId rhsVarId) {
  if(lhsVarId==rhsVarId)
	return; // duplication not necessary
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	Constraint c=*i;
	if(c.isVarValOp() && c.lhsVar()==rhsVarId) {
	  addConstraint(Constraint(c.op(),lhsVarId,c.rhsValCppCapsule()));
	}
  }
}

bool ConstraintSet::disequalityExists() const {
  ConstraintSet::iterator i=find(DISEQUALITYCONSTRAINT);
  if(i!=end())
	return true;
  else
	return false;
}
void ConstraintSet::addDisequality() {
  addConstraint(DISEQUALITYCONSTRAINT);
}

void ConstraintSet::addConstraint(Constraint c) {
  // we have to look which version of constraint already exists (it can only be 
  // a) at most one equality constraint or b) arbitrary many inequality constraints or c) one disequality)
  // we do not check for x=y constraints
  if(disequalityExists())
	  return;
  // TODO1-2: currently not necessary as we propagate all constriants when a x=y is added to a constraint set.
  //   TODO1: do not add x!=k if {y!=k, x==y}
  //   TODO1a: compute set of inequality-constants for given variable and DO consider all equations
  //   TODO1b: compute set of equality-constants for given variable and DO consider all equations
  //   TODO2: if {x==k, x==y} then y!=k ==> x##y}
  // TODO3: introduce generic disequality: x##0 (0 is dummy) for any disequality which is created
  if(c.isVarVarOp()) {
	if(c.lhsVar()==c.rhsVar()) {
	  // we do not add an x=x constraint
	  return;
	}
#if 0
	if(c.lhsVar()<c.rhsVar()) {
	  // ordering is OK, nothing to do
	} else {
	  // ordering is not OK, swap (y=x ==> x=y)
	  c.swapVars();
	}
#endif
  }
  switch(c.op()) {
	// attempt to insert x==k
  case Constraint::EQ_VAR_CONST: {
	if(disequalityExists())
	  return;
	// if x!=k exists ==> ##
	if(constraintExists(Constraint::NEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule())) {
	  removeConstraint(Constraint(Constraint::NEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	  addDisequality();
	  return;
	}
	// search for x==? (there can only be at most one equation on x)
	ConstraintSet::iterator i=findSpecific(Constraint::EQ_VAR_CONST,c.lhsVar());
	if(i!=end()) {
	  if(!((*i).rhsValCppCapsule()==c.rhsValCppCapsule())) {
		// other x==num exists with num!=c.num ==> DEQ
		removeConstraint(*i);
		addDisequality();
		return;
	  } else {
		// nothing todo (same constraint already exists)
		return;
	  }
	}
	// all remaining const-constraints can be removed (can only be inequalities)
	deleteConstConstraints(c.lhsVar());
	set<Constraint>::insert(Constraint(Constraint::EQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	return;
  }
  case Constraint::NEQ_VAR_CONST: {
	// we attempt to insert x!=k

	// check for existing disequality first
	if(disequalityExists())
	  return;
	// check if x==k exists. If yes, introduce x##k
	if(constraintExists(Constraint::EQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule())) {
	  removeConstraint(Constraint(Constraint::EQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	  addDisequality();
	  return;
	}
	// search for some x==m (where m must be different to k (otherwise we would have found it above))
	ConstraintSet::iterator i=findSpecific(Constraint::EQ_VAR_CONST,c.lhsVar());
	if(i!=end()) {
	  assert(!((*i).rhsValCppCapsule() == c.rhsValCppCapsule()));
	  // we have found an equation x==m with m!=k ==> do not insert x!=k constraint (do nothing)
	  return;
	}
	break;
  }
  case Constraint::DEQ_VAR_CONST:
	//removeConstraint(Constraint(Constraint::EQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	//removeConstraint(Constraint(Constraint::NEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	//set<Constraint>::insert(Constraint(Constraint::DEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	set<Constraint>::insert(c);
	return;
  case Constraint::EQ_VAR_VAR:
	// TODO: maintain consistency wenn x=y is inserted but constraints of x and y are in conflict
	// currently we only add x=y for a fresh variable y on parameter passing and therefore this is OK.
	set<Constraint>::insert(Constraint(Constraint::EQ_VAR_VAR,c.lhsVar(),c.rhsVar()));
	return;
  default:
	throw "INTERNAL ERROR: ConstraintSet::insert: unknown operator.";
  }
  // all other cases (no constraint already in the set is effected)
  set<Constraint>::insert(c);
}

void ConstraintSet::eraseConstraint(Constraint c) {
  erase(c);
}

void ConstraintSet::removeConstraint(Constraint c) {
  if(c.op()==Constraint::EQ_VAR_VAR) {
	// ensure we do not loose information because of dropping x=y
	// example: x!=1,y!=2,x=y => x!=1,x!=2,y!=1,y!=2
	duplicateConstConstraints(c.lhsVar(), c.rhsVar()); // duplication direction from right to left (as in an assignment)
	duplicateConstConstraints(c.rhsVar(), c.lhsVar()); // duplication direction from right to left (as in an assignment)
	eraseConstraint(c);
  } else {
	// all other cases
	eraseConstraint(c);
  }
}

void ConstraintSet::removeConstraint(ConstraintSet::iterator i) {
  removeConstraint(*i);
}

#if 1
ConstraintSet operator+(ConstraintSet s1, ConstraintSet s2) {
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

ConstraintSet& ConstraintSet::operator+=(ConstraintSet& s2) {
  for(ConstraintSet::iterator i2=s2.begin();i2!=s2.end();++i2)
	addConstraint(*i2);
  return *this;
}

bool ConstraintSet::constraintExists(Constraint::ConstraintOp op) const { 
  return constraintsWithOp(op).size()>0;
}

ConstraintSet ConstraintSet::constraintsWithOp(Constraint::ConstraintOp op) const { 
  ConstraintSet cs;
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if((*i).op()==op)
	  cs.addConstraint(*i);
  }
  return cs;
}

bool ConstraintSet::constraintExists(Constraint::ConstraintOp op, VariableId varId, CppCapsuleAValue intVal) const { 
  Constraint tmp(op,varId,intVal);
  return constraintExists(tmp);
}

bool ConstraintSet::constraintExists(Constraint::ConstraintOp op, VariableId varId, AValue intVal) const { 
  return constraintExists(op,varId,CppCapsuleAValue(intVal));
}

bool ConstraintSet::constraintExists(const Constraint& tmp) const { 
  ConstraintSet::const_iterator foundElemIter=find(tmp);
  return foundElemIter!=end();
}

ConstraintSet ConstraintSet::findSpecificSet(Constraint::ConstraintOp op, VariableId varId) const {
  ConstraintSet cs;
  // find op-constraint for variable varname
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if((*i).lhsVar()==varId && (*i).op()==op)
	  cs.addConstraint(Constraint((*i).op(),(*i).lhsVar(),(*i).rhsValCppCapsule()));
  }
  return cs;
}

ConstraintSet::iterator ConstraintSet::findSpecific(Constraint::ConstraintOp op, VariableId varId) const {
  // find op-constraint for variable varname
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if((*i).lhsVar()==varId && (*i).op()==op)
	  return i;
  }
  return end();
}

AType::ConstIntLattice ConstraintSet::varConstIntLatticeValue(const VariableId varId) const {
  AType::ConstIntLattice c;
  ConstraintSet::iterator i=findSpecific(Constraint::EQ_VAR_CONST,varId);
  if(i==end()) {
	// no EQ_VAR_CONST constraint for this variable
	return AType::ConstIntLattice(AType::Top());
  } else {
	return AType::ConstIntLattice((*i).rhsVal());
  }
}


ConstraintSet ConstraintSet::deleteVarConstraints(VariableId varId) {
  deleteConstraints(varId);
  return *this;
}

void ConstraintSet::deleteConstConstraints(VariableId varId) {
  // now, remove all other constraints with variable varId
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if((*i).lhsVar()==varId && (*i).isVarValOp())
	  removeConstraint(i);
  }
}

void ConstraintSet::deleteConstraints(VariableId varId) {
  // ensure we delete equalities before all other constraints (removing equalities keeps information alive)
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if(((*i).op()==Constraint::EQ_VAR_VAR)) {
	  if(((*i).lhsVar()==varId)) 
		removeConstraint(i);
	  else {
		if(((*i).rhsVar()==varId)) 
		  removeConstraint(i);
	  }
	}
  }
  // now, remove all other constraints with variable varId
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if((*i).lhsVar()==varId)
	  removeConstraint(i);
  }
}

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

long ConstraintSet::memorySize() const {
  long mem=0;
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	mem+=sizeof(*i);
  }
  return mem+sizeof(*this);
}

// strict weak ordering on two sets
bool operator<(const ConstraintSet& s1, const ConstraintSet& s2) {
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
