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
using namespace CodeThorn;
using namespace CodeThorn;

VariableIdMappingExtended* AbstractValue::_variableIdMapping=nullptr;

istream& CodeThorn::operator>>(istream& is, AbstractValue& value) {
  value.fromStream(is);
  return is;
}


// default constructor
AbstractValue::AbstractValue():valueType(AbstractValue::BOT),intValue(0) {}

// type conversion
// TODO: represent value 'undefined' here
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

void AbstractValue::setVariableIdMapping(VariableIdMappingExtended* varIdMapping) {
  AbstractValue::_variableIdMapping=varIdMapping;
}

AbstractValue::TypeSize AbstractValue::calculateTypeSize(CodeThorn::BuiltInType btype) {
  ROSE_ASSERT(AbstractValue::_variableIdMapping);
  return AbstractValue::_variableIdMapping->getTypeSize(btype);
}

void AbstractValue::setValue(long long int val) {
  ROSE_ASSERT(typeSize!=0);
  // TODO: truncate here if necessary
  intValue=val;
}

void AbstractValue::setValue(long double fval) {
  ROSE_ASSERT(typeSize!=0);
  // TODO: adapt here if necessary
  floatValue=fval;
}

AbstractValue AbstractValue::createIntegerValue(CodeThorn::BuiltInType btype, long long int ival) {
  AbstractValue aval;
  aval.initInteger(btype,ival);
  return aval;
}

void AbstractValue::initInteger(CodeThorn::BuiltInType btype, long long int ival) {
  valueType=AbstractValue::INTEGER;
  setTypeSize(calculateTypeSize(btype));
  setValue(ival);
}

void AbstractValue::initFloat(CodeThorn::BuiltInType btype, long double fval) {
  valueType=AbstractValue::FLOAT;
  setTypeSize(calculateTypeSize(btype));
  setValue(fval);
}

// type conversion
AbstractValue::AbstractValue(Top e) {valueType=AbstractValue::TOP;intValue=0;} // intValue=0 superfluous
// type conversion
AbstractValue::AbstractValue(Bot e) {valueType=AbstractValue::BOT;intValue=0;} // intValue=0 superfluous

AbstractValue::AbstractValue(unsigned char x) {
  initInteger(BITYPE_UCHAR,x);
}
AbstractValue::AbstractValue(signed char x) {
  initInteger(BITYPE_SCHAR,x);
}
AbstractValue::AbstractValue(short x) {
  initInteger(BITYPE_SSHORT,x);
}
AbstractValue::AbstractValue(int x) {
  initInteger(BITYPE_SINT,x);
}
AbstractValue::AbstractValue(long int x) {
  initInteger(BITYPE_SLONG,x);
}
AbstractValue::AbstractValue(long long int x) {
  initInteger(BITYPE_SLONG_LONG,x);
}

AbstractValue::AbstractValue(unsigned short int x) {
  initInteger(BITYPE_USHORT,x);
}
AbstractValue::AbstractValue(unsigned int x) {
  initInteger(BITYPE_UINT,x);
}
AbstractValue::AbstractValue(unsigned long int x) {
  initInteger(BITYPE_ULONG,x);
}
AbstractValue::AbstractValue(unsigned long long int x) {
  initInteger(BITYPE_ULONG_LONG,x);
}
AbstractValue::AbstractValue(float x) {
  initFloat(BITYPE_FLOAT,x);
}
AbstractValue::AbstractValue(double x) {
  initFloat(BITYPE_DOUBLE,x);
}
AbstractValue::AbstractValue(long double x) {
  initFloat(BITYPE_LONG_DOUBLE,x);
}

AbstractValue AbstractValue::createNullPtr() {
  AbstractValue aval(0);
  // create an integer 0, not marked as pointer value.
  return aval;
}

AbstractValue 
AbstractValue::createAddressOfVariable(CodeThorn::VariableId varId) {
  return AbstractValue::createAddressOfArray(varId);
}
AbstractValue 
AbstractValue::createAddressOfArray(CodeThorn::VariableId arrayVarId) {
  return AbstractValue::createAddressOfArrayElement(arrayVarId,AbstractValue(0));
}
AbstractValue 
AbstractValue::createAddressOfArrayElement(CodeThorn::VariableId arrayVariableId, 
                                             AbstractValue index) {
  AbstractValue val;
  if(index.isTop()) {
    return Top();
  } else if(index.isBot()) {
    return Bot();
  } else if(index.isConstInt()) {
    val.valueType=PTR;
    val.variableId=arrayVariableId;
    ROSE_ASSERT(index.isConstInt());
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
  case UNDEFINED: return "undefined";
  case INTEGER: return "constint";
  case FLOAT: return "float";
  case PTR: return "ptr";
  case REF: return "ref";
  case BOT: return "bot";
  default:
    return "unknown";
  }
}

// currently maps to isTop(); in preparation for explicit handling of
// undefined values.
bool AbstractValue::isUndefined() const {return valueType==AbstractValue::UNDEFINED;}
bool AbstractValue::isTop() const {return valueType==AbstractValue::TOP||isUndefined();}
bool AbstractValue::isTrue() const {return valueType==AbstractValue::INTEGER && intValue!=0;}
bool AbstractValue::isFalse() const {return valueType==AbstractValue::INTEGER && intValue==0;}
bool AbstractValue::isBot() const {return valueType==AbstractValue::BOT;}
bool AbstractValue::isConstInt() const {return valueType==AbstractValue::INTEGER;}
bool AbstractValue::isConstPtr() const {return (valueType==AbstractValue::PTR);}
bool AbstractValue::isPtr() const {return (valueType==AbstractValue::PTR);}
bool AbstractValue::isNullPtr() const {return valueType==AbstractValue::INTEGER && intValue==0;}

long AbstractValue::hash() const {
  if(isTop()) return LONG_MAX;
  else if(isBot()) return LONG_MIN;
  else if(isConstInt()) return getIntValue();
  else if(isPtr()) {
    VariableId varId=getVariableId();
    ROSE_ASSERT(varId.isValid());
    return varId.getIdCode()+getIntValue();
  }
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
  case AbstractValue::UNDEFINED: tmp=*this;break;
  default:
    // other cases should not appear because there must be a proper cast
    // TODO: logger[WARN]<<"AbstractValue::operatorNot: unhandled abstract value "<<tmp.toString()<<". Assuming any value as result."<<endl;
    tmp=Top();
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
        if(c1.getIntValue()!=c2.getIntValue()) {
          return c1.getIntValue()<c2.getIntValue();
        } else {
          return c1.getTypeSize()<c2.getTypeSize();
        }
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
      return c1.getIntValue()==c2.getIntValue() && c1.getTypeSize()==c2.getTypeSize();
    else if(c1.isPtr() && c2.isPtr()) {
      return c1.getVariableId()==c2.getVariableId() && c1.getIntValue()==c2.getIntValue() && c1.getTypeSize()==c2.getTypeSize();
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
    return AbstractValue(variableId==other.variableId && intValue==other.intValue && getTypeSize()==other.getTypeSize());
  } else if(isConstInt() && other.isConstInt()) {
    return AbstractValue(intValue==other.intValue && getTypeSize()==other.getTypeSize());
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
  if(isPtr() && other.isPtr()) {
    // check is same memory region
    if(variableId==other.variableId) {
      return AbstractValue(intValue<other.intValue && getTypeSize()==other.getTypeSize());
    } else {
      // incompatible pointer comparison (can be true or false)
      return AbstractValue::createTop();
    }
  }
  if(!(isConstInt()&&other.isConstInt())) {
    cerr<<"WARNING: operatorLess: "<<toString()<<" < "<<other.toString()<<" - assuming arbitrary result."<<endl;
    return AbstractValue::createTop();
  }
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

string AbstractValue::toLhsString(CodeThorn::VariableIdMapping* vim) const {
  switch(valueType) {
  case TOP: return "top";
  case BOT: return "bot";
  case UNDEFINED: return "undefined";
  case INTEGER: {
    stringstream ss;
    ss<<getIntValue();
    return ss.str();
  }
  case PTR: {
    stringstream ss;
    if(vim->getNumberOfElements(variableId)==1) {
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

string AbstractValue::toRhsString(CodeThorn::VariableIdMapping* vim) const {
  switch(valueType) {
  case TOP: return "top";
  case BOT: return "bot";
  case UNDEFINED: return "undefined";
  case INTEGER: {
    stringstream ss;
    ss<<getIntValue();
    return ss.str();
  }
  case PTR: {
    stringstream ss;
    ss<<"&"; // on the rhs an abstract pointer is always a pointer value of some abstract value
    if(vim->getNumberOfElements(variableId)==1) {
      ss<<variableId.toUniqueString(vim); // variables are arrays of size 1
    } else {
      ss<<variableId.toUniqueString(vim)<<"["<<getIntValue()<<"]";
    }
    return ss.str();
  }
  default:
    throw CodeThorn::Exception("Error: AbstractValue::toRhsString operation failed. Unknown abstraction type.");
  }
}

string AbstractValue::arrayVariableNameToString(CodeThorn::VariableIdMapping* vim) const {
  switch(valueType) {
  case PTR: {
    stringstream ss;
    ss<<variableId.toString(vim);
    return ss.str();
  }
  default:
    throw CodeThorn::Exception("Error: AbstractValue::arrayVariableNameToString operation failed. Unknown abstraction type.");
  }
}

string AbstractValue::toString(CodeThorn::VariableIdMapping* vim) const {
  switch(valueType) {
  case TOP: return "top";
  case BOT: return "bot";
  case UNDEFINED: return "undefined";
  case INTEGER: {
    stringstream ss;
    ss<<getIntValue();
    return ss.str();
  }
  case FLOAT: {
    return getFloatValueString();
  }
  case PTR: {
    //    if(vim->hasArrayType(variableId)||vim->hasClassType(variableId)||vim->hasReferenceType(variableId)||vim->isHeapMemoryRegionId(variableId)) {
      stringstream ss;
      ss<<"("
        <<variableId.toUniqueString(vim)
        <<","
        <<getIntValue()
        <<")";
      return ss.str();
      //    } else {
      //      return variableId.toString(vim);
      //    }
  }
  default:
    throw CodeThorn::Exception("Error: AbstractValue::toString operation failed. Unknown abstraction type.");
  }
}

string AbstractValue::toString() const {
  switch(valueType) {
  case TOP: return "top";
  case BOT: return "bot";
  case UNDEFINED: return "undefined";
  case INTEGER: {
    stringstream ss;
    ss<<getIntValue();
    return ss.str();
  }
  case FLOAT: {
    return getFloatValueString();
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
  int tmpintValue=0;
  if(CodeThorn::Parse::checkWord("top",is)) {
    valueType=TOP;
    intValue=0;
  } else if(CodeThorn::Parse::checkWord("bot",is)) {
    valueType=BOT;
    intValue=0;
  } else if(CodeThorn::Parse::integer(is,tmpintValue)) {
    valueType=INTEGER;
    intValue=tmpintValue;
  } else {
    throw CodeThorn::Exception("Error: ConstIntLattic::fromStream failed.");
  }
}

AbstractValue::ValueType AbstractValue::getValueType() const {
  return valueType;
}

AbstractValue::TypeSize AbstractValue::getTypeSize() const {
  return typeSize;
}

void AbstractValue::setTypeSize(AbstractValue::TypeSize typeSize) {
  this->typeSize=typeSize;
}

AbstractValue AbstractValue::getIndexValue() const { 
  if(isTop()||isBot()) {
    return *this;
  } else {
    return AbstractValue(getIndexIntValue());
  }
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
  // TODO: PTR will be removed once all ptrs are adapted to getIndexIntValue
  if(valueType!=INTEGER && valueType!=PTR) {
    cerr << "AbstractValue: valueType="<<valueTypeToString()<<endl;
    throw CodeThorn::Exception("Error: AbstractValue::getIntValue operation failed.");
  }
  else 
    return intValue;
}

std::string AbstractValue::getFloatValueString() const { 
   if(valueType!=FLOAT) {
     cerr << "AbstractValue: valueType="<<valueTypeToString()<<endl;
     throw CodeThorn::Exception("Error: AbstractValue::getFloatValueString operation failed.");
   } else {
     stringstream ss;
     ss<<floatValue;
     return ss.str();
   }
}

CodeThorn::VariableId AbstractValue::getVariableId() const { 
  if(valueType!=PTR && valueType!=REF) {
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
  case AbstractValue::FLOAT: 
    tmp.valueType=AbstractValue::FLOAT;
    tmp.floatValue=-floatValue; // unary minus
    break;
  case AbstractValue::TOP:
    tmp=Top();break;
  case AbstractValue::UNDEFINED:
    tmp=*this;break; // keep information that it is undefined
  case AbstractValue::BOT: tmp=Bot();break;
  case AbstractValue::PTR:
    throw CodeThorn::Exception("Error: AbstractValue operator unary minus on pointer value.");
  case AbstractValue::REF:
    throw CodeThorn::Exception("Error: AbstractValue operator unary minus on reference value.");
    //  default case intentionally not present to force all values to be handled explicitly
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
    throw CodeThorn::Exception("Error: invalid operands of type pointer to binary ‘operator+’"+a.toString()+"+"+b.toString());
  } else if(a.isConstInt() && b.isConstInt()) {
    return a.getIntValue()+b.getIntValue();
  } else {
    throw CodeThorn::Exception("Error: undefined behavior in '+' operation: "+a.toString()+","+b.toString());
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
    throw CodeThorn::Exception("Error: undefined behavior in binary '-' operation.");
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

// static function, two arguments
bool AbstractValue::approximatedBy(AbstractValue val1, AbstractValue val2) {
  if(val1.isBot()||val2.isTop()) {
    // bot <= x, x <= top
    return true;
  } else if(val1.isTop() && val2.isTop()) {
    // this case is necessary because TOP and UNDEFINED need to be treated the same
    // and isTop also includes isUndefined (in its definition).
    return true;
  } else if(val1.valueType==val2.valueType) {
    switch(val1.valueType) {
    case BOT: return true;
    case INTEGER: return (val1.intValue==val2.intValue);
    case FLOAT: return (val1.floatValue==val2.floatValue);
    case PTR: return (val1.getVariableId()==val2.getVariableId());
    case REF: return (val1.getVariableId()==val2.getVariableId());
    case TOP:
    case UNDEFINED:
      // special cases of above if-conditions
      // TODO: enfore non-reachable here
      return true;
    }
  }
  return false;
}

// static function, two arguments
AbstractValue AbstractValue::combine(AbstractValue val1, AbstractValue val2) {
  if(val1.isUndefined()||val2.isUndefined()) {
    // keep it undefined if at least one of the two is undefined (taint analysis)
    // any undefined value is treated like top everywhere else
    if(val1.isUndefined())
      return val1;
    else
      return val2;
  } else if(val1.isTop()||val2.isTop()) {
    return createTop(); // create top when any of the two is top (all undefined-cases already handled above)
  } else if(val1.isBot()) {
    return val2;
  } else if(val2.isBot()) {
    return val1;
  } else if(val1.valueType==val2.valueType) {
    switch(val1.valueType) {
    case BOT: return val2;
    case TOP: return val1; // special case of above if-conds (TODO: enforce not reachable)
    case UNDEFINED: return val1; // special case of above if-cond (TODO: enforce not reachable)
    case INTEGER: {
      if(val1.intValue==val2.intValue) {
        return val1;
      } else {
        return createTop();
      }
    }
    case FLOAT: {
      if(val1.floatValue==val2.floatValue) {
        return val1;
      } else {
        return createTop();
      }
    }
    case PTR: 
    case REF: {
      if(val1.getVariableId()==val2.getVariableId()) {
        return val1;
      } else {
        return createTop();
      }
    }
    }
  }
  return createTop();
}

AbstractValue AbstractValue::createTop() {
  CodeThorn::Top top;
  return AbstractValue(top);
}
AbstractValue AbstractValue::createUndefined() {
  AbstractValue newValue;
  newValue.valueType=AbstractValue::UNDEFINED;
  return newValue;
}
AbstractValue AbstractValue::createBot() {
  CodeThorn::Bot bot;
  return AbstractValue(bot);
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

