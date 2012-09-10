// -*- mode: C++; coding: utf-8; -*-
/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "AType.h"

ostream& AType::operator<<(ostream& os, BoolLattice& toStream) {
  os << toStream.toString();
  return os;
}

ostream& AType::operator<<(ostream& os, ConstIntLattice& toStream) {
  os << toStream.toString();
  return os;
}

AType::BoolLattice::BoolLattice():value(AType::BoolLattice::BOT) {}

AType::BoolLattice::BoolLattice(bool val) {if(val) value=AType::BoolLattice::TRUE; else value=AType::BoolLattice::FALSE;}

// type conversion
AType::BoolLattice::BoolLattice(Top e) {value=AType::BoolLattice::TOP;}

// type conversion
AType::BoolLattice::BoolLattice(Bot e) {value=AType::BoolLattice::BOT;}

// type conversion
AType::BoolLattice::BoolLattice(int x) {if(x==0) value=AType::BoolLattice::FALSE; else value=AType::BoolLattice::TRUE;}

bool AType::BoolLattice::isTop()   const {return value==AType::BoolLattice::TOP;}
bool AType::BoolLattice::isTrue()  const {return value==AType::BoolLattice::TRUE;}
bool AType::BoolLattice::isFalse() const {return value==AType::BoolLattice::FALSE;}
bool AType::BoolLattice::isBot()   const {return value==AType::BoolLattice::BOT;}

AType::BoolLattice AType::BoolLattice::operator!() {
  AType::BoolLattice tmp;
  switch(value) {
  case FALSE: tmp.value=TRUE;break;
  case TRUE: tmp.value=FALSE;break;
  case TOP: tmp.value=TOP;break;
  case BOT: tmp.value=TOP;break;
  default:
	throw "Error: BoolLattice operation '!' failed.";
  }
  return tmp;
}

bool AType::BoolLattice::operator==(AType::BoolLattice other) const {
  return other.value == value;
}

bool AType::BoolLattice::operator!=(AType::BoolLattice other) const {
  return other.value != value;
}

AType::BoolLattice AType::BoolLattice::operator||(AType::BoolLattice other) {
  AType::BoolLattice tmp;
  // all TOP cases
  if(value==TOP || other.value==TOP) { tmp.value=TOP; return tmp; }
  // all BOT cases
  if(value==BOT) { tmp.value=other.value; return tmp; }
  // usual bool cases
  if(value==TRUE) { tmp.value=TRUE; return tmp; }
  if(other.value==TRUE) { tmp.value=TRUE; return tmp; }
  if(value==FALSE || other.value==FALSE) { tmp.value=FALSE; return tmp; }
  throw "Error: BoolLattice operation|| failed.";
}

AType::BoolLattice AType::BoolLattice::operator&&(AType::BoolLattice other) {
  AType::BoolLattice tmp;
  // all TOP cases
  if(value==TOP || other.value==TOP) { tmp.value=TOP; return tmp; }
  // all BOT cases
  if(value==BOT) { tmp.value=other.value; return tmp; }
  if(other.value==BOT) { tmp.value=value; return tmp; }
  // usual bool cases
  if(value==FALSE) { tmp.value=FALSE; return tmp; }
  if(value==TRUE && other.value==TRUE) { tmp.value=TRUE; return tmp; }
  if(value==TRUE && other.value==FALSE) { tmp.value=FALSE; return tmp; }
  throw "Error: BoolLattice operation&& failed.";
}
// operator= : C++ default used
// operator== : C++ default used
string AType::BoolLattice::toString() const {
  switch(value) {
  case TOP: return "⊤" /*"top"*/; /* AP: not shure how legal this is */
  case BOT: return "⊥" /*"bot"*/;
  case TRUE: return "true";
  case FALSE: return "false";
  default:
	throw "Error: BoolLattice::toString operation failed.";
  }
}

AType::ConstIntLattice::ConstIntLattice():valueType(AType::ConstIntLattice::BOT) {}
AType::ConstIntLattice::ConstIntLattice(bool val) {
  if(val) {
	valueType=AType::ConstIntLattice::CONSTINT;
	intValue=1;
  } else {
	valueType=AType::ConstIntLattice::CONSTINT;
	intValue=0;
  }
}
// type conversion
AType::ConstIntLattice::ConstIntLattice(Top e) {valueType=AType::ConstIntLattice::TOP;}
// type conversion
AType::ConstIntLattice::ConstIntLattice(Bot e) {valueType=AType::ConstIntLattice::BOT;}
// type conversion
AType::ConstIntLattice::ConstIntLattice(int x) {valueType=AType::ConstIntLattice::CONSTINT;intValue=x;}
bool AType::ConstIntLattice::isTop() const {return valueType==AType::ConstIntLattice::TOP;}
bool AType::ConstIntLattice::isTrue() const {return valueType==AType::ConstIntLattice::CONSTINT && intValue!=0;}
bool AType::ConstIntLattice::isFalse() const {return valueType==AType::ConstIntLattice::CONSTINT && intValue==0;}
bool AType::ConstIntLattice::isBot() const {return valueType==AType::ConstIntLattice::BOT;}
bool AType::ConstIntLattice::isConstInt() const {return valueType==AType::ConstIntLattice::CONSTINT;}

AType::ConstIntLattice AType::ConstIntLattice::operator!() {
  AType::ConstIntLattice tmp;
  switch(valueType) {
  case AType::ConstIntLattice::CONSTINT: 
	tmp.valueType=valueType;
	if(intValue==0) {
	  tmp.intValue=1;
	} else {
	  tmp.intValue=0;
	}
	break;
  case AType::ConstIntLattice::TOP: tmp.valueType=AType::ConstIntLattice::TOP;break;
  case AType::ConstIntLattice::BOT: tmp.valueType=AType::ConstIntLattice::TOP;break;
  default:
	throw "Error: ConstIntLattice operation '!' failed.";
  }
  return tmp;
}

AType::ConstIntLattice AType::ConstIntLattice::operator-() {
  AType::ConstIntLattice tmp;
  switch(valueType) {
  case AType::ConstIntLattice::CONSTINT: 
	tmp.valueType=AType::ConstIntLattice::CONSTINT;
	tmp.intValue=-intValue; // unary minus
	break;
  case AType::ConstIntLattice::TOP: tmp.valueType=AType::ConstIntLattice::TOP;break;
  case AType::ConstIntLattice::BOT: tmp.valueType=AType::ConstIntLattice::TOP;break;
  default:
	throw "Error: ConstIntLattice operation '!' failed.";
  }
  return tmp;
}


AType::ConstIntLattice AType::ConstIntLattice::operator||(ConstIntLattice other) {
  AType::ConstIntLattice tmp;
  // all TOP cases
  if(valueType==AType::ConstIntLattice::TOP || other.valueType==AType::ConstIntLattice::TOP) {
	tmp.valueType=AType::ConstIntLattice::TOP; return tmp;
  }
  // all BOT cases
  if(valueType==AType::ConstIntLattice::BOT) {
	tmp.valueType=other.valueType; 
	tmp.intValue=other.intValue;
	return tmp;
  }
  // otherwise, usual bool cases:
  tmp.valueType=AType::ConstIntLattice::CONSTINT;
  if(isTrue()) { tmp.intValue=1; return tmp; }
  if(other.isTrue()) { tmp.intValue=1; return tmp; }
  if(isFalse() || other.isFalse()) { tmp.intValue=0; return tmp; }
  throw "Error: ConstIntLattice operation|| failed.";
}

AType::ConstIntLattice AType::ConstIntLattice::operator&&(ConstIntLattice other) {
  AType::ConstIntLattice tmp;
  // all TOP cases
  if(valueType==AType::ConstIntLattice::TOP || other.valueType==AType::ConstIntLattice::TOP) { 
	tmp.valueType=AType::ConstIntLattice::TOP; return tmp;
  }
  // all BOT cases
  if(valueType==AType::ConstIntLattice::BOT) {
	tmp.valueType=other.valueType;
	tmp.intValue=other.intValue;
	return tmp;
  }
  // otherwise, usual bool cases:
  tmp.valueType=AType::ConstIntLattice::CONSTINT;
  if(isFalse()) { tmp.intValue=0; return tmp; }
  if(isTrue() && other.isTrue()) { tmp.intValue=1; return tmp; }
  if(isTrue() && other.isFalse()) { tmp.intValue=0; return tmp; }
  throw "Error: ConstIntLattice operation&& failed.";
}

bool AType::strictWeakOrderingIsSmaller(const AType::ConstIntLattice& c1, const AType::ConstIntLattice& c2) {
  if(c1.isConstInt() && c2.isConstInt())
	return c1.getIntValue()<c2.getIntValue();
  return (c1.getValueType()<c2.getValueType());
}
bool AType::strictWeakOrderingIsEqual(const AType::ConstIntLattice& c1, const AType::ConstIntLattice& c2) {
  if(c1.isConstInt() && c2.isConstInt())
	return c1.getIntValue()==c2.getIntValue();
  return c1.getValueType()==c2.getValueType();
}
bool AType::ConstIntLatticeCmp::operator()(const AType::ConstIntLattice& c1, const AType::ConstIntLattice& c2) const {
  AType::strictWeakOrderingIsSmaller(c1,c2);
}

bool AType::CppCapsuleConstIntLatticeLessComparator::operator()(const AType::CppCapsuleConstIntLattice& c1, const AType::CppCapsuleConstIntLattice& c2) const {
  return AType::strictWeakOrderingIsSmaller(c1.getValue(),c2.getValue());
}

bool AType::CppCapsuleConstIntLattice::operator==(AType::CppCapsuleConstIntLattice other) const {
  return AType::strictWeakOrderingIsEqual(getValue(),other.getValue());
}
bool AType::CppCapsuleConstIntLattice::operator<(AType::CppCapsuleConstIntLattice other) const {
  return AType::strictWeakOrderingIsSmaller(getValue(),other.getValue());
}


AType::ConstIntLattice AType::ConstIntLattice::operator==(ConstIntLattice other) const {
  // all TOP cases
  if(valueType==AType::ConstIntLattice::TOP || other.valueType==AType::ConstIntLattice::TOP) { return AType::Top(); }
  // all BOT cases
  if(valueType==AType::ConstIntLattice::BOT || other.valueType==AType::ConstIntLattice::BOT) { 
	return AType::Top();
  }
  // otherwise usual bool cases
  if(intValue==other.intValue) 
	return ConstIntLattice(true);
  else
	return ConstIntLattice(false);
}

AType::ConstIntLattice AType::ConstIntLattice::operator!=(ConstIntLattice other) {
  return !(*this==other);
}

#if 0
AType::ConstIntLattice AType::ConstIntLattice::operator<(ConstIntLattice other) {
  // if any argument is bot or top : result is top
  // otherwise: compare integer values
  if(isTop()||other.isTop())
	return Top();
  if(isBot()||other.isBot())
	return Top();
  return getIntValue()<other.getIntValue();
}
#endif

string AType::ConstIntLattice::toString() const {
  switch(valueType) {
  case AType::ConstIntLattice::TOP: return "top";
  case AType::ConstIntLattice::BOT: return "bot";
  case AType::ConstIntLattice::CONSTINT: {
	stringstream ss;
	ss<<getIntValue();
	return ss.str();
  }
  default:
	throw "Error: ConstIntLattice::toString operation failed.";
  }
}

AType::ConstIntLattice::ValueType AType::ConstIntLattice::getValueType() const {
  return valueType;
}

int AType::ConstIntLattice::getIntValue() const { 
  return intValue;
}
