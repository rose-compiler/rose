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

istream& AType::operator>>(istream& is, AbstractValue& value) {
  value.fromStream(is);
  return is;
}


// default constructor
AType::AbstractValue::AbstractValue():valueType(AType::AbstractValue::BOT),intValue(0) {}

// type conversion
AType::AbstractValue::AbstractValue(bool val) {
  if(val) {
    valueType=AType::AbstractValue::CONSTINT;
    intValue=1;
  } else {
    valueType=AType::AbstractValue::CONSTINT;
    intValue=0;
  }
}

// type conversion
AType::AbstractValue::AbstractValue(Top e) {valueType=AType::AbstractValue::TOP;intValue=0;}
// type conversion
AType::AbstractValue::AbstractValue(Bot e) {valueType=AType::AbstractValue::BOT;intValue=0;}
// type conversion
AType::AbstractValue::AbstractValue(unsigned char x) {valueType=AType::AbstractValue::CONSTINT;intValue=(int)x;}
AType::AbstractValue::AbstractValue(signed char x) {valueType=AType::AbstractValue::CONSTINT;intValue=(int)x;}
AType::AbstractValue::AbstractValue(short x) {valueType=AType::AbstractValue::CONSTINT;intValue=(int)x;}
AType::AbstractValue::AbstractValue(int x) {valueType=AType::AbstractValue::CONSTINT;intValue=x;}
AType::AbstractValue::AbstractValue(long int x) {
  if((x<INT_MIN || x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
   valueType=AType::AbstractValue::CONSTINT;intValue=(int)x;
}
AType::AbstractValue::AbstractValue(long long int x) {
  if((x<INT_MIN || x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AType::AbstractValue::CONSTINT;intValue=(int)x;
}
AType::AbstractValue::AbstractValue(unsigned short int x) {
  if((x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AType::AbstractValue::CONSTINT;intValue=(int)x;
}
AType::AbstractValue::AbstractValue(unsigned int x) {
  if((x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AType::AbstractValue::CONSTINT;intValue=(int)x;
}
AType::AbstractValue::AbstractValue(unsigned long int x) {
  if((x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AType::AbstractValue::CONSTINT;intValue=(int)x;
}
AType::AbstractValue::AbstractValue(unsigned long long int x) {
  if((x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AType::AbstractValue::CONSTINT;intValue=(int)x;
}

AType::AbstractValue 
AType::AbstractValue::createAddressOfArray(SPRAY::VariableId arrayVarId) {
  return AType::AbstractValue::createAddressOfArrayElement(arrayVarId,AType::AbstractValue(0));
}
AType::AbstractValue 
AType::AbstractValue::createAddressOfArrayElement(SPRAY::VariableId arrayVariableId, 
                                             AType::AbstractValue index) {
  AType::AbstractValue val;
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

std::string AType::AbstractValue::valueTypeToString() const {
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

int AType::AbstractValue::intLength() { return sizeof(int); }

bool AType::AbstractValue::isTop() const {return valueType==AType::AbstractValue::TOP;}
bool AType::AbstractValue::isTrue() const {return valueType==AType::AbstractValue::CONSTINT && intValue!=0;}
bool AType::AbstractValue::isFalse() const {return valueType==AType::AbstractValue::CONSTINT && intValue==0;}
bool AType::AbstractValue::isBot() const {return valueType==AType::AbstractValue::BOT;}
bool AType::AbstractValue::isConstInt() const {return valueType==AType::AbstractValue::CONSTINT;}
bool AType::AbstractValue::isPtr() const {return valueType==AType::AbstractValue::PTR;}

long AType::AbstractValue::hash() const {
  if(isTop()) return LONG_MAX;
  else if(isBot()) return LONG_MIN;
  else if(isConstInt()) return getIntValue();
    else if(isPtr()) return getVariableId().getIdCode()+getIntValue();
  else throw CodeThorn::Exception("Error: AbstractValue hash: unknown value.");
}

AType::AbstractValue AType::AbstractValue::operatorNot() {
  AType::AbstractValue tmp;
  switch(valueType) {
  case AType::AbstractValue::CONSTINT: 
    tmp.valueType=valueType;
    if(intValue==0) {
      tmp.intValue=1;
    } else {
      tmp.intValue=0;
    }
    break;
  case AType::AbstractValue::TOP: tmp=Top();break;
  case AType::AbstractValue::BOT: tmp=Bot();break;
  default:
    throw CodeThorn::Exception("Error: AbstractValue operation '!' failed.");
  }
  return tmp;
}

AType::AbstractValue AType::AbstractValue::operatorOr(AbstractValue other) {
  AType::AbstractValue tmp;
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
  throw CodeThorn::Exception("Error: AbstractValue operation|| failed.");
}

AType::AbstractValue AType::AbstractValue::operatorAnd(AbstractValue other) {
  AType::AbstractValue tmp;
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
  throw CodeThorn::Exception("Error: AbstractValue operation&& failed.");
}
 
bool AType::strictWeakOrderingIsSmaller(const AType::AbstractValue& c1, const AType::AbstractValue& c2) {
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
      throw CodeThorn::Exception("Error: AbstractValue::strictWeakOrderingIsSmaller: unknown equal values.");
    }
  }
}

bool AType::strictWeakOrderingIsEqual(const AType::AbstractValue& c1, const AType::AbstractValue& c2) {
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

bool AType::ConstIntLatticeCmp::operator()(const AType::AbstractValue& c1, const AType::AbstractValue& c2) const {
  return AType::strictWeakOrderingIsSmaller(c1,c2);
}

bool AType::AbstractValue::operator==(AType::AbstractValue other) const {
  return AType::strictWeakOrderingIsEqual(*this,other);
}
bool AType::AbstractValue::operator<(AType::AbstractValue other) const {
  return AType::strictWeakOrderingIsSmaller(*this,other);
}

// TODO: comparison with nullptr
AType::AbstractValue AType::AbstractValue::operatorEq(AbstractValue other) const {
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
    return AbstractValue(variableId==other.variableId && intValue==other.intValue);
  } else if(isConstInt() && other.isConstInt()) {
    return AbstractValue(intValue==other.intValue);
  } else {
    return AbstractValue(Top()); // all other cases can be true or false
  }
}

AType::AbstractValue AType::AbstractValue::operatorNotEq(AbstractValue other) const {
  return ((*this).operatorEq(other)).operatorNot();
}

AType::AbstractValue AType::AbstractValue::operatorLess(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()<other.getIntValue();
}

AType::AbstractValue AType::AbstractValue::operatorLessOrEq(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()<=other.getIntValue();
}

AType::AbstractValue AType::AbstractValue::operatorMoreOrEq(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()>=other.getIntValue();
}

AType::AbstractValue AType::AbstractValue::operatorMore(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()>other.getIntValue();
}

AType::AbstractValue AType::AbstractValue::operatorBitwiseOr(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()|other.getIntValue();
}

AType::AbstractValue AType::AbstractValue::operatorBitwiseXor(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()^other.getIntValue();
}

AType::AbstractValue AType::AbstractValue::operatorBitwiseAnd(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()&other.getIntValue();
}

AType::AbstractValue AType::AbstractValue::operatorBitwiseComplement() const {
  if(isTop()||isBot())
    return *this;
  assert(isConstInt());
  return ~getIntValue();
}

AType::AbstractValue AType::AbstractValue::operatorBitwiseShiftLeft(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return *this;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()<<other.getIntValue();
}

AType::AbstractValue AType::AbstractValue::operatorBitwiseShiftRight(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return *this;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()>>other.getIntValue();
}

string AType::AbstractValue::toString(SPRAY::VariableIdMapping* vim) const {
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
    throw CodeThorn::Exception("Error: AbstractValue::toString operation failed. Unknown abstraction type.");
  }
}

string AType::AbstractValue::toString() const {
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
    throw CodeThorn::Exception("Error: AbstractValue::toString operation failed. Unknown abstraction type.");
  }
}

void AType::AbstractValue::fromStream(istream& is) {
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

AType::AbstractValue::ValueType AType::AbstractValue::getValueType() const {
  return valueType;
}

int AType::AbstractValue::getIndexIntValue() const { 
  if(valueType!=PTR) {
    cerr << "AbstractValue: valueType="<<valueTypeToString()<<endl;
    throw CodeThorn::Exception("Error: AbstractValue::getIndexIntValue operation failed.");
  }
  else 
    return intValue;
}

int AType::AbstractValue::getIntValue() const { 
  // PTR will be removed once all ptrs are adapted to getIndexIntValue
  if(valueType!=CONSTINT && valueType!=PTR) {
    cerr << "AbstractValue: valueType="<<valueTypeToString()<<endl;
    throw CodeThorn::Exception("Error: AbstractValue::getIntValue operation failed.");
  }
  else 
    return intValue;
}

 SPRAY::VariableId AType::AbstractValue::getVariableId() const { 
   if(valueType!=PTR) {
     cerr << "AbstractValue: valueType="<<valueTypeToString()<<endl;
     throw CodeThorn::Exception("Error: AbstractValue::getVariableId operation failed.");
  }
  else 
    return variableId;
}

// arithmetic operators
AType::AbstractValue AType::AbstractValue::operatorUnaryMinus() {
  AType::AbstractValue tmp;
  switch(valueType) {
  case AType::AbstractValue::CONSTINT: 
    tmp.valueType=AType::AbstractValue::CONSTINT;
    tmp.intValue=-intValue; // unary minus
    break;
  case AType::AbstractValue::TOP: tmp=Top();break;
  case AType::AbstractValue::BOT: tmp=Bot();break;
  case AType::AbstractValue::PTR:
    throw CodeThorn::Exception("Error: AbstractValue operator unary minus on pointer value.");
  default:
    throw CodeThorn::Exception("Error: AbstractValue operation unaryMinus failed.");
  }
  return tmp;
}

AType::AbstractValue AType::AbstractValue::operatorAdd(AType::AbstractValue& a,AType::AbstractValue& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  if(a.isPtr() && b.isConstInt()) {
    AType::AbstractValue val=a;
    val.intValue+=b.intValue;
    return val;
  } else if(a.isConstInt() && b.isPtr()) {
    AType::AbstractValue val=b;
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
AType::AbstractValue AType::AbstractValue::operatorSub(AType::AbstractValue& a,AType::AbstractValue& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  if(a.isPtr() && b.isPtr()) {
    if(a.getVariableId()==b.getVariableId()) {
      AType::AbstractValue val;
      val.intValue=a.intValue-b.intValue;
      val.valueType=CONSTINT;
      val.variableId=a.variableId; // same as b.variableId
      return val;
    } else {
      return Top(); // subtraction of incompatible pointers gives arbitrary value
    }
  } else if(a.isPtr() && b.isConstInt()) {
    AType::AbstractValue val=a;
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
AType::AbstractValue AType::AbstractValue::operatorMul(AType::AbstractValue& a,AType::AbstractValue& b) {
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
AType::AbstractValue AType::AbstractValue::operatorDiv(AType::AbstractValue& a,AType::AbstractValue& b) {
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
AType::AbstractValue AType::AbstractValue::operatorMod(AType::AbstractValue& a,AType::AbstractValue& b) {
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

AType::AbstractValue AType::operator+(AType::AbstractValue& a,AType::AbstractValue& b) {
  return AbstractValue::operatorAdd(a,b);
}
AType::AbstractValue AType::operator-(AType::AbstractValue& a,AType::AbstractValue& b) {
  return AbstractValue::operatorSub(a,b);
}
AType::AbstractValue AType::operator*(AType::AbstractValue& a,AType::AbstractValue& b) {
  return AbstractValue::operatorMul(a,b);
}
AType::AbstractValue AType::operator/(AType::AbstractValue& a,AType::AbstractValue& b) {
  return AbstractValue::operatorDiv(a,b);
}
AType::AbstractValue AType::operator%(AType::AbstractValue& a,AType::AbstractValue& b) {
  return AbstractValue::operatorMod(a,b);
}
