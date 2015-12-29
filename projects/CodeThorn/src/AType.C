// -*- mode: C++; coding: utf-8; -*-
/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "AType.h"
#include "assert.h"
#include "CommandLineOptions.h"
#include <iostream>
#include <climits>
#include "Miscellaneous.h"
#include "Miscellaneous2.h"

using namespace std;

using namespace CodeThorn;

bool AType::ConstIntLattice::arithTop=false;

ostream& AType::operator<<(ostream& os, const BoolLattice& value) {
  os << value.toString();
  return os;
}

ostream& AType::operator<<(ostream& os, const ConstIntLattice& value) {
  os << value.toString();
  return os;
}

istream& AType::operator>>(istream& is, ConstIntLattice& value) {
  value.fromStream(is);
  return is;
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

/**
 * CAVEAT:    We define !bot := bot
 * RATIONALE: bot means "not yet analyzed" / "no meaningful result"
 *          top means "could be any value"
 */
AType::BoolLattice AType::BoolLattice::operator!() {
  AType::BoolLattice tmp;
  switch(value) {
  case FALSE: tmp.value=TRUE;break;
  case TRUE: tmp.value=FALSE;break;
  case TOP: tmp.value=TOP;break;
  case BOT: tmp.value=BOT;break;
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

bool AType::BoolLattice::operator<(BoolLattice other) const {
  if (isBot()) {
    if (other.isBot()) return false;
    return true;
  }
  if (isFalse()) {
    if (other.isBot())   return false;
    if (other.isFalse()) return false;
    return true;
  }
  if (isTrue()) {
    if (other.isBot())   return false;
    if (other.isFalse()) return false;
    if (other.isTrue())  return false;
    return true;
  }
  if (isTop()) {
    return false;
  }
  throw "Error: BoolLattice operation< failed.";
}

AType::BoolLattice AType::BoolLattice::operator||(AType::BoolLattice other) {
  AType::BoolLattice tmp;
  // all TOP cases
  if(isTop()   && other.isTop())   return Top();
  if(isTop()   && other.isTrue())  return true;
  if(isTrue()  && other.isTop())   return true;
  if(isTop()   && other.isFalse()) return Top();
  if(isFalse() && other.isTop())   return Top();
  // all BOT cases
  if(value==BOT)                   return other;
  if(other.value==BOT)             return *this;
  // usual bool cases
  if(isTrue()  && other.isTrue())  return true;
  if(isTrue()  && other.isFalse()) return true;
  if(isFalse() && other.isTrue())  return true;
  if(isFalse() && other.isFalse()) return false;
  throw "Error: BoolLattice operation|| failed.";
}

AType::BoolLattice AType::BoolLattice::operator&&(AType::BoolLattice other) {
  // all TOP cases
  if(isTop()   && other.isTop())   return Top();
  if(isTop()   && other.isTrue())  return Top();
  if(isTrue()  && other.isTop())   return Top();
  if(isTop()   && other.isFalse()) return false;
  if(isFalse() && other.isTop())   return false;
  // all BOT cases
  if(value==BOT)                   return other;
  if(other.value==BOT)             return *this;
  // usual bool cases
  if(isTrue()  && other.isTrue())  return true;
  if(isTrue()  && other.isFalse()) return false;
  if(isFalse() && other.isTrue())  return false;
  if(isFalse() && other.isFalse()) return false;
  throw "Error: BoolLattice operation&& failed.";
}

AType::BoolLattice AType::BoolLattice::lub(AType::BoolLattice other) {
  // all TOP cases
  if(isTop()   || other.isTop())   return Top();
  // all BOT cases
  if(value==BOT)                   return other;
  if(other.value==BOT)             return *this;
  // usual bool cases
  if(isTrue()  && other.isTrue())  return true;
  if(isTrue()  && other.isFalse()) return Top();
  if(isFalse() && other.isTrue())  return Top();
  if(isFalse() && other.isFalse()) return false;
  throw "Error: BoolLattice lub failed.";
}

AType::BoolLattice AType::BoolLattice::glb(AType::BoolLattice other) {
  // all BOT cases
  if(isBot()   || other.isBot())   return Bot();
  // all BOT cases
  if(value==TOP)                   return other;
  if(other.value==TOP)             return *this;
  // usual bool cases
  if(isTrue()  && other.isTrue())  return true;
  if(isTrue()  && other.isFalse()) return Bot();
  if(isFalse() && other.isTrue())  return Bot();
  if(isFalse() && other.isFalse()) return false;
  throw "Error: BoolLattice glb failed.";
}

// operator= : C++ default used
// operator== : C++ default used
string AType::BoolLattice::toString() const {
  switch(value) {
  case TOP: return "⊤" /*"top"*/; /* AP: not sure how portable this is */
  case BOT: return "⊥" /*"bot"*/;
  case TRUE: return "true";
  case FALSE: return "false";
  default:
    cerr<<"VALUE = "<<value<<endl;
    throw "Error: BoolLattice::toString operation failed.";
  }
}

// default constructor
AType::ConstIntLattice::ConstIntLattice():valueType(AType::ConstIntLattice::BOT),intValue(0) {}

// type conversion
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
AType::ConstIntLattice::ConstIntLattice(Top e) {valueType=AType::ConstIntLattice::TOP;intValue=0;}
// type conversion
AType::ConstIntLattice::ConstIntLattice(Bot e) {valueType=AType::ConstIntLattice::BOT;intValue=0;}
// type conversion
AType::ConstIntLattice::ConstIntLattice(unsigned char x) {valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;}
AType::ConstIntLattice::ConstIntLattice(signed char x) {valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;}
AType::ConstIntLattice::ConstIntLattice(short x) {valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;}
AType::ConstIntLattice::ConstIntLattice(int x) {valueType=AType::ConstIntLattice::CONSTINT;intValue=x;}
AType::ConstIntLattice::ConstIntLattice(long int x) {
  if((x<INT_MIN || x>INT_MAX)) throw "Error: numbers outside 'signed int' range not supported.";
  valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;
}
AType::ConstIntLattice::ConstIntLattice(long long int x) {
  if((x<INT_MIN || x>INT_MAX)) throw "Error: numbers outside 'signed int' range not supported.";
  valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;
}
AType::ConstIntLattice::ConstIntLattice(unsigned short int x) {
  if((x>INT_MAX)) throw "Error: numbers outside 'signed int' range not supported.";
  valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;
}
AType::ConstIntLattice::ConstIntLattice(unsigned int x) {
  if((x>INT_MAX)) throw "Error: numbers outside 'signed int' range not supported.";
  valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;
}
AType::ConstIntLattice::ConstIntLattice(unsigned long int x) {
  if((x>INT_MAX)) throw "Error: numbers outside 'signed int' range not supported.";
  valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;
}
AType::ConstIntLattice::ConstIntLattice(unsigned long long int x) {
  if((x>INT_MAX)) throw "Error: numbers outside 'signed int' range not supported.";
  valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;
}
int AType::ConstIntLattice::intLength() { return sizeof(int); }

bool AType::ConstIntLattice::isTop() const {return valueType==AType::ConstIntLattice::TOP;}
bool AType::ConstIntLattice::isTrue() const {return valueType==AType::ConstIntLattice::CONSTINT && intValue!=0;}
bool AType::ConstIntLattice::isFalse() const {return valueType==AType::ConstIntLattice::CONSTINT && intValue==0;}
bool AType::ConstIntLattice::isBot() const {return valueType==AType::ConstIntLattice::BOT;}
bool AType::ConstIntLattice::isConstInt() const {return valueType==AType::ConstIntLattice::CONSTINT;}

long AType::ConstIntLattice::hash() const {
  if(isTop()) return LONG_MAX;
  if(isBot()) return LONG_MIN;
  if(isConstInt()) return getIntValue();
  throw "Error: ConstIntLattice hash: unknown value.";
}

AType::ConstIntLattice AType::ConstIntLattice::operatorNot() {
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
  case AType::ConstIntLattice::TOP: tmp=Top();break;
  case AType::ConstIntLattice::BOT: tmp=Bot();break;
  default:
    throw "Error: ConstIntLattice operation '!' failed.";
  }
  return tmp;
}

AType::ConstIntLattice AType::ConstIntLattice::operatorOr(ConstIntLattice other) {
  AType::ConstIntLattice tmp;
  // all TOP cases
  if(isTop()   && other.isTop())   return Top();
  if(isTop()   && other.isTrue())  return true;
  if(isTrue()  && other.isTop())   return true;
  if(isTop()   && other.isFalse()) return Top();
  if(isFalse() && other.isTop())   return Top();
  // all BOT cases
  if(valueType==BOT) {
    tmp.valueType=other.valueType; 
    tmp.intValue=other.intValue;
    return tmp;
  }
  if(other.valueType==BOT) {
    tmp.valueType=valueType; 
    tmp.intValue=intValue;
    return tmp;
  }
  // otherwise, usual bool cases:
  assert(isConstInt()&&other.isConstInt());
  if(isTrue()  && other.isTrue())  return true;
  if(isTrue()  && other.isFalse()) return true;
  if(isFalse() && other.isTrue())  return true;
  if(isFalse() && other.isFalse()) return false;
  throw "Error: ConstIntLattice operation|| failed.";
}

AType::ConstIntLattice AType::ConstIntLattice::operatorAnd(ConstIntLattice other) {
  AType::ConstIntLattice tmp;
  // all TOP cases
  if(isTop()   && other.isTop())   return Top();
  if(isTop()   && other.isTrue())  return Top();
  if(isTrue()  && other.isTop())   return Top();
  if(isTop()   && other.isFalse()) return false;
  if(isFalse() && other.isTop())   return false;
  // all BOT cases
  if(valueType==BOT) {
    tmp.valueType=other.valueType;
    tmp.intValue=other.intValue;
    return tmp;
  }
  if(other.valueType==BOT) {
    tmp.valueType=valueType; 
    tmp.intValue=intValue;
    return tmp;
  }
  // otherwise, usual bool cases:
  assert(isConstInt()&&other.isConstInt());
  if(isTrue() && other.isTrue())   return true;
  if(isTrue() && other.isFalse())  return false;
  if(isFalse() && other.isTrue())  return false;
  if(isFalse() && other.isFalse()) return false;
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
  return AType::strictWeakOrderingIsSmaller(c1,c2);
}

bool AType::ConstIntLattice::operator==(AType::ConstIntLattice other) const {
  return AType::strictWeakOrderingIsEqual(*this,other);
}
bool AType::ConstIntLattice::operator<(AType::ConstIntLattice other) const {
  return AType::strictWeakOrderingIsSmaller(*this,other);
}

#if 1
bool AType::CppCapsuleConstIntLatticeLessComparator::operator()(const AType::CppCapsuleConstIntLattice& c1, const AType::CppCapsuleConstIntLattice& c2) const {
  return AType::strictWeakOrderingIsSmaller(c1.getValue(),c2.getValue());
}

bool AType::CppCapsuleConstIntLattice::operator==(AType::CppCapsuleConstIntLattice other) const {
  return AType::strictWeakOrderingIsEqual(getValue(),other.getValue());
}
bool AType::CppCapsuleConstIntLattice::operator<(AType::CppCapsuleConstIntLattice other) const {
  return AType::strictWeakOrderingIsSmaller(getValue(),other.getValue());
}
#endif

AType::ConstIntLattice AType::ConstIntLattice::operatorEq(ConstIntLattice other) const {
  // all TOP cases
  if(valueType==TOP || other.valueType==TOP) { 
    return AType::Top();
  }
  // all BOT cases
  if(valueType==BOT)
    return other;
  if(other.valueType==BOT) { 
    return *this;
  }
  // otherwise usual bool cases
  if(intValue==other.intValue) 
    return ConstIntLattice(true);
  else
    return ConstIntLattice(false);
}

AType::ConstIntLattice AType::ConstIntLattice::operatorNotEq(ConstIntLattice other) const {
  return ((*this).operatorEq(other)).operatorNot();
}

AType::ConstIntLattice AType::ConstIntLattice::operatorLess(ConstIntLattice other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()<other.getIntValue();
}

AType::ConstIntLattice AType::ConstIntLattice::operatorLessOrEq(ConstIntLattice other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()<=other.getIntValue();
}

AType::ConstIntLattice AType::ConstIntLattice::operatorMoreOrEq(ConstIntLattice other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()>=other.getIntValue();
}

AType::ConstIntLattice AType::ConstIntLattice::operatorMore(ConstIntLattice other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()>other.getIntValue();
}

string AType::ConstIntLattice::toString() const {
  switch(valueType) {
  case TOP: return "top";
  case BOT: return "bot";
  case CONSTINT: {
    stringstream ss;
    ss<<getIntValue();
    return ss.str();
  }
  default:
    throw "Error: ConstIntLattice::toString operation failed.";
  }
}

void AType::ConstIntLattice::fromStream(istream& is) {
  if(SPRAY::Parse::checkWord("top",is)) {
    valueType=TOP;
    intValue=0;
  } else if(SPRAY::Parse::checkWord("bot",is)) {
    valueType=BOT;
    intValue=0;
  } else if(SPRAY::Parse::integer(is,intValue)) {
    valueType=CONSTINT;
  } else {
    throw "Error: ConstIntLattic::fromStream failed.";
  }
}

AType::ConstIntLattice::ValueType AType::ConstIntLattice::getValueType() const {
  return valueType;
}

int AType::ConstIntLattice::getIntValue() const { 
  if(valueType!=CONSTINT) {
    cerr << "ConstIntLattice: valueType="<<valueType<<endl;
    throw "Error: ConstIntLattice::getIntValue operation failed.";
  }
  else 
    return intValue;
}

// arithmetic operators
AType::ConstIntLattice AType::ConstIntLattice::operatorUnaryMinus() {
  AType::ConstIntLattice tmp;
  switch(valueType) {
  case AType::ConstIntLattice::CONSTINT: 
    tmp.valueType=AType::ConstIntLattice::CONSTINT;
    tmp.intValue=-intValue; // unary minus
    break;
  case AType::ConstIntLattice::TOP: tmp=Top();break;
  case AType::ConstIntLattice::BOT: tmp=Bot();break;
  default:
    throw "Error: ConstIntLattice operation unaryMinus failed.";
  }
  return tmp;
}

AType::ConstIntLattice AType::operator+(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  assert(a.isConstInt() && b.isConstInt());
  if(ConstIntLattice::arithTop)
    return AType::Top();
  else
    return a.getIntValue()+b.getIntValue();
}
AType::ConstIntLattice AType::operator-(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  assert(a.isConstInt() && b.isConstInt());
  if(ConstIntLattice::arithTop)
    return AType::Top();
  else
    return a.getIntValue()-b.getIntValue();
}
AType::ConstIntLattice AType::operator*(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  assert(a.isConstInt() && b.isConstInt());
  if(ConstIntLattice::arithTop)
    return AType::Top();
  else
    return a.getIntValue()*b.getIntValue();
}
AType::ConstIntLattice AType::operator/(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  assert(a.isConstInt() && b.isConstInt());
  if(ConstIntLattice::arithTop)
    return AType::Top();
  else
    return a.getIntValue()/b.getIntValue();
}
AType::ConstIntLattice AType::operator%(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  assert(a.isConstInt() && b.isConstInt());
  if(ConstIntLattice::arithTop)
    return AType::Top();
  else
    return a.getIntValue()%b.getIntValue();
}
