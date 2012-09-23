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
  if(c1.lhsVar()<c2.lhsVar())
	return true;
  if(c1.lhsVar()==c2.lhsVar() && c1.op()<c2.op())
	return true;
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
	}
  }
  return false;
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
	  result.addConstraint(Constraint(Constraint::DEQ_VAR_CONST,c.lhsVar(),c.rhsVal())); // unchanged
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
	  result.addConstraint(Constraint(Constraint::DEQ_VAR_VAR,c.lhsVar(),c.rhsVar())); // unchanged
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


void ConstraintSet::addConstraint(Constraint c) {
  // we have to look which version of constraint already exists (it can only be at most one)
  switch(c.op()) {
  case Constraint::EQ_VAR_CONST: {
	if(constraintExists(Constraint::DEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()))
	  return;
	if(constraintExists(Constraint::NEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule())) {
	  removeConstraint(Constraint(Constraint::NEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	  set<Constraint>::insert(Constraint(Constraint::DEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	  return;
	}
	ConstraintSet::iterator i=findSpecific(Constraint::EQ_VAR_CONST,c.lhsVar());
	if(i!=end()) {
	  if(!((*i).rhsValCppCapsule()==c.rhsValCppCapsule())) {
		// other x==num exists with num!=c.num ==> DEQ
		removeConstraint(*i);
		set<Constraint>::insert(Constraint(Constraint::DEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
		return;
	  } else {
		// nothing todo (same constraint already exists)
		return;
	  }
	}
	// all remaining constraints can be removed (can only be inqualities)
	deleteConstConstraints(c.lhsVar());
	set<Constraint>::insert(Constraint(Constraint::EQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	return;
  }
  case Constraint::NEQ_VAR_CONST: {
	if(constraintExists(Constraint::DEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()))
	  return;
	if(constraintExists(Constraint::EQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule())) {
	  removeConstraint(Constraint(Constraint::EQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	  set<Constraint>::insert(Constraint(Constraint::DEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	  return;
	}
	break;
  }
  case Constraint::DEQ_VAR_CONST:
	removeConstraint(Constraint(Constraint::EQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	removeConstraint(Constraint(Constraint::NEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	set<Constraint>::insert(Constraint(Constraint::DEQ_VAR_CONST,c.lhsVar(),c.rhsValCppCapsule()));
	return;
  case Constraint::EQ_VAR_VAR:
	// TODO: maintain consistency wenn x=y is inserted but constraints of x and y are in conflict
	// currently we only add x=y for a fresh variable y on parameter passing
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
#if 1
  // we use this as the find algorithm does not properly work yet (TODO: investigate)
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if(*i==tmp)
	  return true;
  }
  return false;
#else
  ConstraintSet::const_iterator foundElemIter=find(tmp);
  return foundElemIter!=end();
#endif
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

bool ConstraintSetMaintainer::constraintSetExists(ConstraintSet& s) {
  return constraintSetPtr(s)!=0;
}

const ConstraintSet* ConstraintSetMaintainer::constraintSetPtr(ConstraintSet& s) {
  // we use this as the find algorithm cannot be used for this data structure yet.
  for(ConstraintSetMaintainer::iterator i=begin();i!=end();++i) {
	if(*i==s)
	  return &*i;
  }
  return 0;
}

const ConstraintSet* ConstraintSetMaintainer::processNewConstraintSet(ConstraintSet& s) {
  ProcessingResult res=processConstraintSet(s);
  assert(res.first==false);
  return res.second;
}

const ConstraintSet* ConstraintSetMaintainer::processNewOrExistingConstraintSet(ConstraintSet& s) {
  ProcessingResult res=processConstraintSet(s);
  return res.second;
}

ConstraintSetMaintainer::ProcessingResult ConstraintSetMaintainer::processConstraintSet(ConstraintSet s) {
  if(const ConstraintSet* existingConstraintSetPtr=constraintSetPtr(s)) {
	return make_pair(true,existingConstraintSetPtr);
  } else {
	push_back(s);
	const ConstraintSet* existingConstraintSetPtr=constraintSetPtr(s);
	assert(existingConstraintSetPtr);
	return make_pair(false,existingConstraintSetPtr);
  }
  assert(0);
}

long ConstraintSet::memorySize() const {
  long mem=0;
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	mem+=sizeof(*i);
  }
  return mem+sizeof(*this);
}
long ConstraintSetMaintainer::memorySize() const {
  long mem=0;
  for(ConstraintSetMaintainer::const_iterator i=begin();
	  i!=end();
	  ++i) {
	mem+=(*i).memorySize();
  }
  return mem+sizeof(*this);
}
