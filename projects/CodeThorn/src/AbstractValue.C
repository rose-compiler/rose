// -*- mode: C++; coding: utf-8; -*-
/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"
#include "AbstractValue.h"
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

AType::ConstIntLattice 
AType::ConstIntLattice::createAddressOfArray(SPRAY::VariableId arrayVarId) {
  return AType::ConstIntLattice::createAddressOfArrayElement(arrayVarId,AType::ConstIntLattice(0));
}
AType::ConstIntLattice 
AType::ConstIntLattice::createAddressOfArrayElement(SPRAY::VariableId arrayVariableId, 
                                             AType::ConstIntLattice index) {
  AType::ConstIntLattice val;
  if(index.isTop()) {
    return Top();
  } else if(index.isBot()) {
    return Bot();
  } else if(index.isConstInt()) {
    val.valueType=PTR;
    val.variableId=arrayVariableId;
    val.intValue=index.getIntValue();
    return val;
  } else {
    cerr<<"Error: createAddressOfArray: unknown index type."<<endl;
    exit(1);
  }
}

std::string AType::ConstIntLattice::valueTypeToString() const {
  switch(valueType) {
  case TOP: return "top";
  case CONSTINT: return "constint";
  case PTR: return "ptr";
  case RAW_PTR: return "rawptr";
  case BOT: return "bot";
  default:
    return "unknown";
  }
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
  else if(isBot()) return LONG_MIN;
  else if(isConstInt()) return getIntValue();
    else if(isPtr()) return getVariableId().getIdCode()+getIntValue();
  else throw CodeThorn::Exception("Error: ConstIntLattice hash: unknown value.");
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

// TODO: comparison with nullptr
AType::ConstIntLattice AType::ConstIntLattice::operatorEq(ConstIntLattice other) const {
  // all TOP cases
  if(valueType==TOP || other.valueType==TOP) { 
    return AType::Top();
  }
  // all BOT cases
  if(valueType==BOT) {
    return other;
  } else if(other.valueType==BOT) { 
    return *this;
  } else if(isPtr() && other.isPtr()) {
    return ConstIntLattice(variableId==other.variableId && intValue==other.intValue);
  } else if(isConstInt() && other.isConstInt()) {
    return ConstIntLattice(intValue==other.intValue);
  } else {
    return ConstIntLattice(Top()); // all other cases can be true or false
  }
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

AType::ConstIntLattice AType::ConstIntLattice::operatorBitwiseShiftLeft(ConstIntLattice other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return *this;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()<<other.getIntValue();
}

AType::ConstIntLattice AType::ConstIntLattice::operatorBitwiseShiftRight(ConstIntLattice other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return *this;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()>>other.getIntValue();
}

string AType::ConstIntLattice::toString(SPRAY::VariableIdMapping* vim) const {
  switch(valueType) {
  case TOP: return "top";
  case BOT: return "bot";
  case CONSTINT: {
    stringstream ss;
    ss<<getIntValue();
    return ss.str();
  }
  case PTR: {
    stringstream ss;
    ss<<"("<<variableId.toString(vim)<<","<<getIntValue()<<")";
    return ss.str();
  }
  default:
    throw CodeThorn::Exception("Error: ConstIntLattice::toString operation failed. Unknown abstraction type.");
  }
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
  case PTR: {
    stringstream ss;
    ss<<"("<<variableId.toString()<<","<<getIntValue()<<")";
    return ss.str();
  }
  default:
    throw CodeThorn::Exception("Error: ConstIntLattice::toString operation failed. Unknown abstraction type.");
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

int AType::ConstIntLattice::getIndexIntValue() const { 
  if(valueType!=PTR) {
    cerr << "ConstIntLattice: valueType="<<valueTypeToString()<<endl;
    throw CodeThorn::Exception("Error: ConstIntLattice::getIndexIntValue operation failed.");
  }
  else 
    return intValue;
}

int AType::ConstIntLattice::getIntValue() const { 
  // PTR will be removed once all ptrs are adapted to getIndexIntValue
  if(valueType!=CONSTINT && valueType!=PTR) {
    cerr << "ConstIntLattice: valueType="<<valueTypeToString()<<endl;
    throw CodeThorn::Exception("Error: ConstIntLattice::getIntValue operation failed.");
  }
  else 
    return intValue;
}

 SPRAY::VariableId AType::ConstIntLattice::getVariableId() const { 
   if(valueType!=PTR) {
     cerr << "ConstIntLattice: valueType="<<valueTypeToString()<<endl;
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
  case AType::ConstIntLattice::PTR:
    throw CodeThorn::Exception("Error: ConstIntLattice operator unary minus on pointer value.");
  default:
    throw CodeThorn::Exception("Error: ConstIntLattice operation unaryMinus failed.");
  }
  return tmp;
}

AType::ConstIntLattice AType::ConstIntLattice::operatorAdd(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  if(a.isPtr() && b.isConstInt()) {
    AType::ConstIntLattice val=a;
    val.intValue+=b.intValue;
    return val;
  } else if(a.isConstInt() && b.isPtr()) {
    AType::ConstIntLattice val=b;
    val.intValue+=a.intValue;
    return val;
  } else if(a.isPtr() && b.isPtr()) {
    throw CodeThorn::Exception("Error: invalid operands of type pointer to binary ‘operator+’.");
  } else if(a.isConstInt() && b.isConstInt()) {
    return a.getIntValue()+b.getIntValue();
  } else {
    throw CodeThorn::Exception("Error: undefined behavior in '+' operation.");
  }
}
AType::ConstIntLattice AType::ConstIntLattice::operatorSub(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  if(a.isPtr() && b.isPtr()) {
    if(a.getVariableId()==b.getVariableId()) {
      AType::ConstIntLattice val;
      val.intValue=a.intValue-b.intValue;
      val.valueType=CONSTINT;
      val.variableId=a.variableId; // same as b.variableId
      return val;
    } else {
      return Top(); // subtraction of incompatible pointers gives arbitrary value
    }
  } else if(a.isPtr() && b.isConstInt()) {
    AType::ConstIntLattice val=a;
    val.intValue-=b.intValue;
    return val;
  } else if(a.isConstInt() && b.isPtr()) {
    throw CodeThorn::Exception("Error: forbidden operation in '-' operation. Attempt to subtract pointer from integer.");
  } else if(a.isConstInt() && b.isConstInt()) {
    return a.getIntValue()-b.getIntValue();
  } else {
    throw CodeThorn::Exception("Error: undefined behavior in '-' operation.");
  }
}
AType::ConstIntLattice AType::ConstIntLattice::operatorMul(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  // TODO multiplication of pointer values
  assert(a.isConstInt() && b.isConstInt());
  return a.getIntValue()*b.getIntValue();
}
AType::ConstIntLattice AType::ConstIntLattice::operatorDiv(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  // TODO division of pointer values
  assert(a.isConstInt() && b.isConstInt());
  return a.getIntValue()/b.getIntValue();

}
AType::ConstIntLattice AType::ConstIntLattice::operatorMod(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  // TODO modulo of pointer values
  assert(a.isConstInt() && b.isConstInt());
  return a.getIntValue()%b.getIntValue();
}

AType::ConstIntLattice AType::operator+(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  return ConstIntLattice::operatorAdd(a,b);
}
AType::ConstIntLattice AType::operator-(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  return ConstIntLattice::operatorSub(a,b);
}
AType::ConstIntLattice AType::operator*(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  return ConstIntLattice::operatorMul(a,b);
}
AType::ConstIntLattice AType::operator/(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  return ConstIntLattice::operatorDiv(a,b);
}
AType::ConstIntLattice AType::operator%(AType::ConstIntLattice& a,AType::ConstIntLattice& b) {
  return ConstIntLattice::operatorMod(a,b);
}
