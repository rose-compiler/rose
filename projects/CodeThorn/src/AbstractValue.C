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
#include "VariableIdMapping.h"

using namespace std;
using namespace SPRAY;
using namespace CodeThorn;

istream& CodeThorn::operator>>(istream& is, AbstractValue& value) {
  value.fromStream(is);
  return is;
}


// default constructor
AbstractValue::AbstractValue():valueType(AbstractValue::BOT),intValue(0) {}

// type conversion
// TODO: represent value 'unitialized' here
AbstractValue::AbstractValue(VariableId varId):valueType(AbstractValue::PTR),variableId(varId),intValue(0) {
}

// type conversion
AbstractValue::AbstractValue(bool val) {
  if(val) {
    valueType=AbstractValue::INTEGER;
    intValue=1;
  } else {
    valueType=AbstractValue::INTEGER;
    intValue=0;
  }
}

// type conversion
AbstractValue::AbstractValue(Top e) {valueType=AbstractValue::TOP;intValue=0;}
// type conversion
AbstractValue::AbstractValue(Bot e) {valueType=AbstractValue::BOT;intValue=0;}
// type conversion
AbstractValue::AbstractValue(unsigned char x) {valueType=AbstractValue::INTEGER;intValue=(int)x;}
AbstractValue::AbstractValue(signed char x) {valueType=AbstractValue::INTEGER;intValue=(int)x;}
AbstractValue::AbstractValue(short x) {valueType=AbstractValue::INTEGER;intValue=(int)x;}
AbstractValue::AbstractValue(int x) {valueType=AbstractValue::INTEGER;intValue=x;}
AbstractValue::AbstractValue(long int x) {
  if((x<INT_MIN || x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
   valueType=AbstractValue::INTEGER;intValue=(int)x;
}
AbstractValue::AbstractValue(long long int x) {
  if((x<INT_MIN || x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AbstractValue::INTEGER;intValue=(int)x;
}
AbstractValue::AbstractValue(unsigned short int x) {
  if((x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AbstractValue::INTEGER;intValue=(int)x;
}
AbstractValue::AbstractValue(unsigned int x) {
  if((x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AbstractValue::INTEGER;intValue=(int)x;
}
AbstractValue::AbstractValue(unsigned long int x) {
  if((x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AbstractValue::INTEGER;intValue=(int)x;
}
AbstractValue::AbstractValue(unsigned long long int x) {
  if((x>INT_MAX)) throw CodeThorn::Exception("Error: numbers outside 'signed int' range not supported.");
  valueType=AbstractValue::INTEGER;intValue=(int)x;
}

AbstractValue 
AbstractValue::createAddressOfVariable(SPRAY::VariableId varId) {
  return AbstractValue::createAddressOfArray(varId);
}
AbstractValue 
AbstractValue::createAddressOfArray(SPRAY::VariableId arrayVarId) {
  return AbstractValue::createAddressOfArrayElement(arrayVarId,AbstractValue(0));
}
AbstractValue 
AbstractValue::createAddressOfArrayElement(SPRAY::VariableId arrayVariableId, 
                                             AbstractValue index) {
  AbstractValue val;
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

std::string AbstractValue::valueTypeToString() const {
  switch(valueType) {
  case TOP: return "top";
  case INTEGER: return "constint";
  case FLOAT: return "float";
  case PTR: return "ptr";
  case REF: return "ref";
  case BOT: return "bot";
  default:
    return "unknown";
  }
}

bool AbstractValue::isTop() const {return valueType==AbstractValue::TOP;}
bool AbstractValue::isTrue() const {return valueType==AbstractValue::INTEGER && intValue!=0;}
bool AbstractValue::isFalse() const {return valueType==AbstractValue::INTEGER && intValue==0;}
bool AbstractValue::isBot() const {return valueType==AbstractValue::BOT;}
bool AbstractValue::isConstInt() const {return valueType==AbstractValue::INTEGER;}
bool AbstractValue::isPtr() const {return valueType==AbstractValue::PTR;}

long AbstractValue::hash() const {
  if(isTop()) return LONG_MAX;
  else if(isBot()) return LONG_MIN;
  else if(isConstInt()) return getIntValue();
    else if(isPtr()) return getVariableId().getIdCode()+getIntValue();
  else throw CodeThorn::Exception("Error: AbstractValue hash: unknown value.");
}

AbstractValue AbstractValue::operatorNot() {
  AbstractValue tmp;
  switch(valueType) {
  case AbstractValue::INTEGER: 
    tmp.valueType=valueType;
    if(intValue==0) {
      tmp.intValue=1;
    } else {
      tmp.intValue=0;
    }
    break;
  case AbstractValue::TOP: tmp=Top();break;
  case AbstractValue::BOT: tmp=Bot();break;
  default:
    throw CodeThorn::Exception("Error: AbstractValue operation '!' failed.");
  }
  return tmp;
}

AbstractValue AbstractValue::operatorOr(AbstractValue other) {
  AbstractValue tmp;
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

AbstractValue AbstractValue::operatorAnd(AbstractValue other) {
  AbstractValue tmp;
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
 
bool CodeThorn::strictWeakOrderingIsSmaller(const AbstractValue& c1, const AbstractValue& c2) {
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

bool CodeThorn::strictWeakOrderingIsEqual(const AbstractValue& c1, const AbstractValue& c2) {
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

bool CodeThorn::AbstractValueCmp::operator()(const AbstractValue& c1, const AbstractValue& c2) const {
  return CodeThorn::strictWeakOrderingIsSmaller(c1,c2);
}

//bool AbstractValue::operator==(AbstractValue other) const {
//  return CodeThorn::strictWeakOrderingIsEqual(*this,other);
//}
//bool AbstractValue::operator!=(AbstractValue other) const {
//  return !CodeThorn::strictWeakOrderingIsEqual(*this,other);
//}
bool AbstractValue::operator==(const AbstractValue other) const {
  return CodeThorn::strictWeakOrderingIsEqual(*this,other);
}
bool AbstractValue::operator!=(const AbstractValue other) const {
  return !CodeThorn::strictWeakOrderingIsEqual(*this,other);
}
bool AbstractValue::operator<(AbstractValue other) const {
  return CodeThorn::strictWeakOrderingIsSmaller(*this,other);
}

// TODO: comparison with nullptr
AbstractValue AbstractValue::operatorEq(AbstractValue other) const {
  // all TOP cases
  if(valueType==TOP || other.valueType==TOP) { 
    return CodeThorn::Top();
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

AbstractValue AbstractValue::operatorNotEq(AbstractValue other) const {
  return ((*this).operatorEq(other)).operatorNot();
}

AbstractValue AbstractValue::operatorLess(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()<other.getIntValue();
}

AbstractValue AbstractValue::operatorLessOrEq(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()<=other.getIntValue();
}

AbstractValue AbstractValue::operatorMoreOrEq(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()>=other.getIntValue();
}

AbstractValue AbstractValue::operatorMore(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()>other.getIntValue();
}

AbstractValue AbstractValue::operatorBitwiseOr(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()|other.getIntValue();
}

AbstractValue AbstractValue::operatorBitwiseXor(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()^other.getIntValue();
}

AbstractValue AbstractValue::operatorBitwiseAnd(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()&other.getIntValue();
}

AbstractValue AbstractValue::operatorBitwiseComplement() const {
  if(isTop()||isBot())
    return *this;
  assert(isConstInt());
  return ~getIntValue();
}

AbstractValue AbstractValue::operatorBitwiseShiftLeft(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return *this;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()<<other.getIntValue();
}

AbstractValue AbstractValue::operatorBitwiseShiftRight(AbstractValue other) const {
  if(isTop()||other.isTop())
    return Top();
  if(isBot())
    return *this;
  if(other.isBot())
    return *this;
  assert(isConstInt()&&other.isConstInt());
  return getIntValue()>>other.getIntValue();
}

string AbstractValue::toLhsString(SPRAY::VariableIdMapping* vim) const {
  switch(valueType) {
  case TOP: return "top";
  case BOT: return "bot";
  case INTEGER: {
    stringstream ss;
    ss<<getIntValue();
    return ss.str();
  }
  case PTR: {
    stringstream ss;
    if(vim->getSize(variableId)==1) {
      ss<<variableId.toString(vim); // variables are arrays of size 1
    } else {
      ss<<variableId.toString(vim)<<"["<<getIntValue()<<"]";
    }
    return ss.str();
  }
  default:
    throw CodeThorn::Exception("Error: AbstractValue::toLhsString operation failed. Unknown abstraction type.");
  }
}

string AbstractValue::toRhsString(SPRAY::VariableIdMapping* vim) const {
  switch(valueType) {
  case TOP: return "top";
  case BOT: return "bot";
  case INTEGER: {
    stringstream ss;
    ss<<getIntValue();
    return ss.str();
  }
  case PTR: {
    stringstream ss;
    ss<<"&"; // on the rhs an abstract pointer is always a pointer value of some abstract value
    if(vim->getSize(variableId)==1) {
      ss<<variableId.toString(vim); // variables are arrays of size 1
    } else {
      ss<<variableId.toString(vim)<<"["<<getIntValue()<<"]";
    }
    return ss.str();
  }
  default:
    throw CodeThorn::Exception("Error: AbstractValue::toRhsString operation failed. Unknown abstraction type.");
  }
}

string AbstractValue::toString(SPRAY::VariableIdMapping* vim) const {
  switch(valueType) {
  case TOP: return "top";
  case BOT: return "bot";
  case INTEGER: {
    stringstream ss;
    ss<<getIntValue();
    return ss.str();
  }
  case PTR: {
    stringstream ss;
    ss<<"(";
    ss<<variableId.toString(vim);
    ss<<","<<getIntValue()<<")";
    return ss.str();
  }
  default:
    throw CodeThorn::Exception("Error: AbstractValue::toString operation failed. Unknown abstraction type.");
  }
}

string AbstractValue::toString() const {
  switch(valueType) {
  case TOP: return "top";
  case BOT: return "bot";
  case INTEGER: {
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

void AbstractValue::fromStream(istream& is) {
  if(SPRAY::Parse::checkWord("top",is)) {
    valueType=TOP;
    intValue=0;
  } else if(SPRAY::Parse::checkWord("bot",is)) {
    valueType=BOT;
    intValue=0;
  } else if(SPRAY::Parse::integer(is,intValue)) {
    valueType=INTEGER;
  } else {
    throw CodeThorn::Exception("Error: ConstIntLattic::fromStream failed.");
  }
}

AbstractValue::ValueType AbstractValue::getValueType() const {
  return valueType;
}

uint8_t AbstractValue::getValueSize() const {
  return valueSize;
}

void AbstractValue::setValueSize(uint8_t valueSize) {
  this->valueSize=valueSize;
}

int AbstractValue::getIndexIntValue() const { 
  if(valueType!=PTR) {
    cerr << "AbstractValue: valueType="<<valueTypeToString()<<endl;
    throw CodeThorn::Exception("Error: AbstractValue::getIndexIntValue operation failed.");
  }
  else 
    return intValue;
}

int AbstractValue::getIntValue() const { 
  // PTR will be removed once all ptrs are adapted to getIndexIntValue
  if(valueType!=INTEGER && valueType!=PTR) {
    cerr << "AbstractValue: valueType="<<valueTypeToString()<<endl;
    throw CodeThorn::Exception("Error: AbstractValue::getIntValue operation failed.");
  }
  else 
    return intValue;
}

 SPRAY::VariableId AbstractValue::getVariableId() const { 
   if(valueType!=PTR) {
     cerr << "AbstractValue: valueType="<<valueTypeToString()<<endl;
     throw CodeThorn::Exception("Error: AbstractValue::getVariableId operation failed.");
  }
  else 
    return variableId;
}

// arithmetic operators
AbstractValue AbstractValue::operatorUnaryMinus() {
  AbstractValue tmp;
  switch(valueType) {
  case AbstractValue::INTEGER: 
    tmp.valueType=AbstractValue::INTEGER;
    tmp.intValue=-intValue; // unary minus
    break;
  case AbstractValue::TOP: tmp=Top();break;
  case AbstractValue::BOT: tmp=Bot();break;
  case AbstractValue::PTR:
    throw CodeThorn::Exception("Error: AbstractValue operator unary minus on pointer value.");
  default:
    throw CodeThorn::Exception("Error: AbstractValue operation unaryMinus failed.");
  }
  return tmp;
}

AbstractValue AbstractValue::operatorAdd(AbstractValue& a,AbstractValue& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  if(a.isPtr() && b.isConstInt()) {
    AbstractValue val=a;
    val.intValue+=b.intValue;
    return val;
  } else if(a.isConstInt() && b.isPtr()) {
    AbstractValue val=b;
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
AbstractValue AbstractValue::operatorSub(AbstractValue& a,AbstractValue& b) {
  if(a.isTop() || b.isTop())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  if(a.isPtr() && b.isPtr()) {
    if(a.getVariableId()==b.getVariableId()) {
      AbstractValue val;
      val.intValue=a.intValue-b.intValue;
      val.valueType=INTEGER;
      val.variableId=a.variableId; // same as b.variableId
      return val;
    } else {
      return Top(); // subtraction of incompatible pointers gives arbitrary value
    }
  } else if(a.isPtr() && b.isConstInt()) {
    AbstractValue val=a;
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
AbstractValue AbstractValue::operatorMul(AbstractValue& a,AbstractValue& b) {
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
AbstractValue AbstractValue::operatorDiv(AbstractValue& a,AbstractValue& b) {
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
AbstractValue AbstractValue::operatorMod(AbstractValue& a,AbstractValue& b) {
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

AbstractValue CodeThorn::operator+(AbstractValue& a,AbstractValue& b) {
  return AbstractValue::operatorAdd(a,b);
}
AbstractValue CodeThorn::operator-(AbstractValue& a,AbstractValue& b) {
  return AbstractValue::operatorSub(a,b);
}
AbstractValue CodeThorn::operator*(AbstractValue& a,AbstractValue& b) {
  return AbstractValue::operatorMul(a,b);
}
AbstractValue CodeThorn::operator/(AbstractValue& a,AbstractValue& b) {
  return AbstractValue::operatorDiv(a,b);
}
AbstractValue CodeThorn::operator%(AbstractValue& a,AbstractValue& b) {
  return AbstractValue::operatorMod(a,b);
}

AbstractValueSet& CodeThorn::operator+=(AbstractValueSet& s1, AbstractValueSet& s2) {
  for(AbstractValueSet::iterator i=s2.begin();i!=s2.end();++i) {
    s1.insert(*i);
  }
  return s1;
}
