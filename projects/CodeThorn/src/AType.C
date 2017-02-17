// -*- mode: C++; coding: utf-8; -*-
/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"
#include "AType.h"
#include "assert.h"
#include "CommandLineOptions.h"
#include <iostream>
#include <climits>
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "CodeThornException.h"

using namespace std;

using namespace CodeThorn;

istream& AType::operator>>(istream& is, ConstIntLattice& value) {
  value.fromStream(is);
  return is;
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
  if((x<INT_MIN || x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
   valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;
}
AType::ConstIntLattice::ConstIntLattice(long long int x) {
  if((x<INT_MIN || x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;
}
AType::ConstIntLattice::ConstIntLattice(unsigned short int x) {
  if((x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;
}
AType::ConstIntLattice::ConstIntLattice(unsigned int x) {
  if((x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;
}
AType::ConstIntLattice::ConstIntLattice(unsigned long int x) {
  if((x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;
}
AType::ConstIntLattice::ConstIntLattice(unsigned long long int x) {
  if((x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AType::ConstIntLattice::CONSTINT;intValue=(int)x;
}
int AType::ConstIntLattice::intLength() { return sizeof(int); }

bool AType::ConstIntLattice::isTop() const {return valueType==AType::ConstIntLattice::TOP;}
bool AType::ConstIntLattice::isTrue() const {return valueType==AType::ConstIntLattice::CONSTINT && intValue!=0;}
bool AType::ConstIntLattice::isFalse() const {return valueType==AType::ConstIntLattice::CONSTINT && intValue==0;}
bool AType::ConstIntLattice::isBot() const {return valueType==AType::ConstIntLattice::BOT;}
bool AType::ConstIntLattice::isConstInt() const {return valueType==AType::ConstIntLattice::CONSTINT;}
bool AType::ConstIntLattice::isPtr() const {return valueType==AType::ConstIntLattice::PTR;}

long AType::ConstIntLattice::hash() const {
  if(isTop()) return LONG_MAX;
  if(isBot()) return LONG_MIN;
  if(isConstInt()) return getIntValue();
  throw CodeThorn::Exception("Error: ConstIntLattice hash: unknown value.");
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
    throw CodeThorn::Exception("Error: ConstIntLattice operation '!' failed.");
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
  throw CodeThorn::Exception("Error: ConstIntLattice operation|| failed.");
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
  throw CodeThorn::Exception("Error: ConstIntLattice operation&& failed.");
}
 
bool AType::strictWeakOrderingIsSmaller(const AType::ConstIntLattice& c1, const AType::ConstIntLattice& c2) {
  if (c1.getValueType()!=c2.getValueType()) {
    return c1.getValueType()<c2.getValueType();
  } else {
    ROSE_ASSERT(c1.getValueType()==c2.getValueType()); 
    if(c1.isConstInt() && c2.isConstInt()) {
      return c1.getIntValue()<c2.getIntValue();
    } else if(c1.isPtr() && c2.isPtr()) {
      if(c1.getVariableId()!=c2.getVariableId()) {
        return c1.getVariableId()<c2.getVariableId();
      } else {
        return c1.getIntValue()<c2.getIntValue();
      }
    } else if (c1.isBot()==c2.isBot()) {
      return false;
    } else if (c1.isTop()==c2.isTop()) {
      return false;
    } else {
      throw CodeThorn::Exception("Error: ConstIntLattice::strictWeakOrderingIsSmaller: unknown equal values.");
    }
  }
}

bool AType::strictWeakOrderingIsEqual(const AType::ConstIntLattice& c1, const AType::ConstIntLattice& c2) {
  if(c1.getValueType()==c2.getValueType()) {
    if(c1.isConstInt() && c2.isConstInt())
      return c1.getIntValue()==c2.getIntValue();
    else if(c1.isPtr() && c2.isPtr()) {
      return c1.getVariableId()==c2.getVariableId() && c1.getIntValue()==c2.getIntValue();
    } else {
      ROSE_ASSERT((c1.isTop()&&c2.isTop()) || (c1.isBot()&&c2.isBot()));
      return true;
    }
  } else {
    // different value types
    return false;
  }
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

AType::ConstIntLattice AType::ConstIntLattice::operatorBitwiseOr(ConstIntLattice other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()|other.getIntValue();
}

AType::ConstIntLattice AType::ConstIntLattice::operatorBitwiseXor(ConstIntLattice other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()^other.getIntValue();
}

AType::ConstIntLattice AType::ConstIntLattice::operatorBitwiseAnd(ConstIntLattice other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()&other.getIntValue();
}

AType::ConstIntLattice AType::ConstIntLattice::operatorBitwiseComplement() const {
  if(isTop()||isBot())
    return *this;
  assert(isConstInt());
  return ~getIntValue();
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
    throw CodeThorn::Exception("Error: ConstIntLattice::toString operation failed.");
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
    throw CodeThorn::Exception("Error: ConstIntLattic::fromStream failed.");
  }
}

AType::ConstIntLattice::ValueType AType::ConstIntLattice::getValueType() const {
  return valueType;
}

int AType::ConstIntLattice::getIntValue() const { 
  if(valueType!=CONSTINT) {
    cerr << "ConstIntLattice: valueType="<<valueType<<endl;
    throw CodeThorn::Exception("Error: ConstIntLattice::getIntValue operation failed.");
  }
  else 
    return intValue;
}

 SPRAY::VariableId AType::ConstIntLattice::getVariableId() const { 
   if(valueType!=PTR) {
     cerr << "ConstIntLattice: valueType="<<valueType<<endl;
     throw CodeThorn::Exception("Error: ConstIntLattice::getVariableId operation failed.");
  }
  else 
    return variableId;
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
    throw CodeThorn::Exception("Error: ConstIntLattice operation unaryMinus failed.");
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
  return a.getIntValue()%b.getIntValue();
}
