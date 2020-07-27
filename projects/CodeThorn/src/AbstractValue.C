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
#include "CodeThornLib.h"

using namespace std;
using namespace CodeThorn;
using namespace Sawyer::Message; // required for logger[WARN]

VariableIdMappingExtended* AbstractValue::_variableIdMapping=nullptr;
bool AbstractValue::strictChecking=false;
bool AbstractValue::byteMode=false;

istream& CodeThorn::operator>>(istream& is, AbstractValue& value) {
  value.fromStream(is);
  return is;
}


// default constructor
AbstractValue::AbstractValue():valueType(AbstractValue::BOT),intValue(0) {}

// type conversion
// TODO: represent value 'undefined' here
AbstractValue::AbstractValue(VariableId varId):valueType(AbstractValue::PTR),variableId(varId),intValue(0) {
  if(byteMode) {
    // also set element type size
    ROSE_ASSERT(_variableIdMapping);
    size_t elemSize=_variableIdMapping->getElementSize(varId);
    setElementTypeSize(elemSize);
  }
}

AbstractValue::AbstractValue(Label lab):valueType(AbstractValue::FUN_PTR),label(lab) {}

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

CodeThorn::VariableIdMappingExtended* AbstractValue::getVariableIdMapping() {
  return AbstractValue::_variableIdMapping;
}

CodeThorn::TypeSize AbstractValue::calculateTypeSize(CodeThorn::BuiltInType btype) {
  ROSE_ASSERT(AbstractValue::_variableIdMapping);
  return AbstractValue::_variableIdMapping->getTypeSize(btype);
}

void AbstractValue::setValue(long int val) {
  ROSE_ASSERT(getTypeSize()!=0);
  // TODO: truncate here if necessary
  intValue=val;
}

void AbstractValue::setValue(double fval) {
  ROSE_ASSERT(getTypeSize()!=0);
  // TODO: adapt here if necessary
  floatValue=fval;
}

AbstractValue AbstractValue::createIntegerValue(CodeThorn::BuiltInType btype, long long int ival) {
  AbstractValue aval;
  aval.initInteger(btype,ival);
  return aval;
}

void AbstractValue::initInteger(CodeThorn::BuiltInType btype, long int ival) {
  valueType=AbstractValue::INTEGER;
  setTypeSize(calculateTypeSize(btype));
  setValue(ival);
}

void AbstractValue::initFloat(CodeThorn::BuiltInType btype, double fval) {
  valueType=AbstractValue::TOP;intValue=0;
  /* implementation of algebraic operators for floats not implemented yet
  using top instead
  valueType=AbstractValue::FLOAT;
  setTypeSize(calculateTypeSize(btype));
  setValue(fval);
  */
}

// type conversion
AbstractValue::AbstractValue(Top e) {valueType=AbstractValue::TOP;intValue=0;} // intValue=0 superfluous
// type conversion
AbstractValue::AbstractValue(Bot e) {valueType=AbstractValue::BOT;intValue=0;} // intValue=0 superfluous

AbstractValue::AbstractValue(unsigned char x) {
  initInteger(BITYPE_CHAR,x);
}
AbstractValue::AbstractValue(signed char x) {
  initInteger(BITYPE_CHAR,x);
}
AbstractValue::AbstractValue(short x) {
  initInteger(BITYPE_SHORT,x);
}
AbstractValue::AbstractValue(int x) {
  initInteger(BITYPE_INT,x);
}
AbstractValue::AbstractValue(long int x) {
  initInteger(BITYPE_LONG,x);
}
AbstractValue::AbstractValue(long long int x) {
  initInteger(BITYPE_LONG_LONG,x);
}

AbstractValue::AbstractValue(unsigned short int x) {
  initInteger(BITYPE_SHORT,x);
}
AbstractValue::AbstractValue(unsigned int x) {
  initInteger(BITYPE_INT,x);
}
AbstractValue::AbstractValue(unsigned long int x) {
  initInteger(BITYPE_LONG,x);
}
AbstractValue::AbstractValue(unsigned long long int x) {
  initInteger(BITYPE_LONG_LONG,x);
}
AbstractValue::AbstractValue(float x) {
  initFloat(BITYPE_FLOAT,x);
}
AbstractValue::AbstractValue(double x) {
  initFloat(BITYPE_DOUBLE,x);
}
/*
AbstractValue::AbstractValue(long double x) {
  initFloat(BITYPE_LONG_DOUBLE,x);
}
*/
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
#if 0
  cout<<"AbstractValue::createAddressOfArrayElement:arrayVariable:"<<arrayVariableId.toString(_variableIdMapping)
      <<" index:"<<index.toString(_variableIdMapping)
      <<" #"<<_variableIdMapping->getNumberOfElements(arrayVariableId)
      <<" elemsize:"<<_variableIdMapping->getElementSize(arrayVariableId)
      <<endl;
#endif
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
    if(AbstractValue::byteMode) {
      // multiple with type size
      size_t elemSize=_variableIdMapping->getElementSize(arrayVariableId);
      val.intValue=index.getIntValue()*elemSize;
      val.setElementTypeSize(elemSize);
    }
    return val;
  } else {
    cerr<<"Error: createAddressOfArray: unknown index type."<<endl;
    exit(1);
  }
}

AbstractValue 
AbstractValue::createAddressOfFunction(CodeThorn::Label lab) {
  return AbstractValue(lab);
}

std::string AbstractValue::valueTypeToString() const {
  switch(valueType) {
  case TOP: return "top";
  case UNDEFINED: return "undefined";
  case INTEGER: return "constint";
  case FLOAT: return "float";
  case PTR: return "ptr";
  case FUN_PTR: return "funptr";
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
bool AbstractValue::isConstFloat() const {return valueType==AbstractValue::FLOAT;}
bool AbstractValue::isConstPtr() const {return (valueType==AbstractValue::PTR);}
bool AbstractValue::isPtr() const {return (valueType==AbstractValue::PTR);}
bool AbstractValue::isFunctionPtr() const {return (valueType==AbstractValue::FUN_PTR);}
bool AbstractValue::isRef() const {return (valueType==AbstractValue::REF);}
bool AbstractValue::isNullPtr() const {return valueType==AbstractValue::INTEGER && intValue==0;}

long AbstractValue::hash() const {
  if(isTop()) return LONG_MAX;
  else if(isBot()) return LONG_MIN;
  else if(isConstInt()) return getIntValue();
  else if(isConstFloat()) return getFloatValue();
  else if(isPtr()||isRef()) {
    VariableId varId=getVariableId();
    ROSE_ASSERT(varId.isValid());
    return varId.getIdCode()+getIntValue();
  } else if(isFunctionPtr()) {
    return (long)getLabel().getId();
  } else {
    if(strictChecking)
      throw CodeThorn::Exception("Error: AbstractValue hash: unknown value.");
    return LONG_MAX<<1;
  }
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
    SAWYER_MESG(logger[WARN])<<"AbstractValue::operatorNot: unhandled abstract value "<<tmp.toString()<<". Assuming any value as result."<<endl;
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
          if(c1.getTypeSize()!=c2.getTypeSize())
            return c1.getTypeSize()<c2.getTypeSize();
          else
            return c1.getElementTypeSize()<c2.getElementTypeSize();
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
      return c1.getVariableId()==c2.getVariableId() && c1.getIntValue()==c2.getIntValue() && c1.getTypeSize()==c2.getTypeSize() && c1.getElementTypeSize()==c2.getElementTypeSize();
    } else if(c1.isFunctionPtr() && c2.isFunctionPtr()) {
      return c1.getLabel()==c2.getLabel();
    } else {
      ROSE_ASSERT((c1.isTop()&&c2.isTop()) || (c1.isBot()&&c2.isBot()));
      return true;
    }
  } else {
    // different value types
    return false;
  }
}

Label CodeThorn::AbstractValue::getLabel() const {
  return label;
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
    // element type size is not relevant in byteMode when comparing pointers
    return AbstractValue(variableId==other.variableId && intValue==other.intValue && getTypeSize()==other.getTypeSize());
  } else if(isConstInt() && other.isConstInt()) {
    // includes case for two null pointer values
    return AbstractValue(intValue==other.intValue && getTypeSize()==other.getTypeSize());
  } else if((isPtr() && other.isNullPtr()) || (isNullPtr() && other.isPtr()) ) {
    return AbstractValue(0);
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
    SAWYER_MESG(logger[WARN])<<"operatorLess: "<<toString()<<" < "<<other.toString()<<" - assuming arbitrary result."<<endl;
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
    if(strictChecking) {
      throw CodeThorn::Exception("Error: AbstractValue::toLhsString operation failed. Unknown abstraction type.");
    } else {
      return "<<ERROR>>";
    }
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
    if(strictChecking) {
      throw CodeThorn::Exception("Error: AbstractValue::toRhsString operation failed. Unknown abstraction type.");
    } else {
      return "<<ERROR>>";
    }
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
    if(strictChecking) {
      throw CodeThorn::Exception("Error: AbstractValue::arrayVariableNameToString operation failed. Unknown abstraction type.");
    } else {
      return "<<ERROR>>";
    }
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
        <<","
        <<getElementTypeSize()
        <<")";
      return ss.str();
      //    } else {
      //      return variableId.toString(vim);
      //    }
  }
  case FUN_PTR: {
    return label.toString();
  }
  default:
    if(strictChecking) {
      throw CodeThorn::Exception("Error: AbstractValue::toString operation failed. Unknown abstraction type.");
    } else {
      return "<<ERROR>>";
    }
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
    if(strictChecking) {
      throw CodeThorn::Exception("Error: AbstractValue::toString operation failed. Unknown abstraction type.");
    } else {
      return "<<ERROR>>";
    }
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

CodeThorn::TypeSize AbstractValue::getTypeSize() const {
  if(typeSize==0)
    return getElementTypeSize();
  else
    return typeSize;
}

void AbstractValue::setTypeSize(CodeThorn::TypeSize typeSize) {
  this->typeSize=typeSize;
}

CodeThorn::TypeSize AbstractValue::getElementTypeSize() const {
  return elementTypeSize;
}

void AbstractValue::setElementTypeSize(CodeThorn::TypeSize typeSize) {
  this->elementTypeSize=typeSize;
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
    return (int)intValue;
}

int AbstractValue::getIntValue() const { 
  // TODO: PTR will be removed once all ptrs are adapted to getIndexIntValue
  if(valueType!=INTEGER && valueType!=PTR) {
    cerr << "AbstractValue: valueType="<<valueTypeToString()<<endl;
    throw CodeThorn::Exception("Error: AbstractValue::getIntValue operation failed.");
  }
  else 
    return (int)intValue;
}
long int AbstractValue::getLongIntValue() const { 
  return (long int)intValue;
}
float AbstractValue::getFloatValue() const {
  return (float)floatValue;
}
double AbstractValue::getDoubleValue() const {
  return (double)floatValue;
}
/*
long double AbstractValue::getLongDoubleValue() const {
  return (long double)floatValue;
}
*/
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
  case AbstractValue::FUN_PTR:
    return topOrError("Error: AbstractValue operator unary minus on pointer value.");
  case AbstractValue::REF:
    return topOrError("Error: AbstractValue operator unary minus on reference value.");
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
    if(byteMode) {
      int pointerElementSize=0;
      if(a.getElementTypeSize()>0) {
        pointerElementSize=a.getElementTypeSize();
      } else {
        pointerElementSize=_variableIdMapping->getElementSize(a.getVariableId());
      }
      val.intValue+=b.intValue*pointerElementSize;
    } else {
      val.intValue+=b.intValue;
    }
    return val;
  } else if(a.isConstInt() && b.isPtr()) {
    AbstractValue val=b;
    if(byteMode) {
      int pointerElementSize=0;
      if(b.getElementTypeSize()>0) {
        pointerElementSize=b.getElementTypeSize();
      } else {
        pointerElementSize=_variableIdMapping->getElementSize(b.getVariableId());
      }
      val.intValue+=a.intValue*pointerElementSize;
    } else {
      val.intValue+=a.intValue;
    }
    return val;
  } else if(a.isPtr() && b.isPtr()) {
    if(strictChecking)
      throw CodeThorn::Exception("Error: invalid operands of type pointer to binary ‘operator+’"+a.toString()+"+"+b.toString());
    return createTop();
  } else if(a.isConstInt() && b.isConstInt()) {
    return a.getIntValue()+b.getIntValue();
  } else {
    if(strictChecking)
      throw CodeThorn::Exception("Error: undefined behavior in '+' operation: "+a.toString()+","+b.toString());
    return createTop();
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
      if(byteMode) {
        int pointerElementSize=_variableIdMapping->getElementSize(a.getVariableId());
        if((a.intValue-b.intValue)%pointerElementSize!=0) {
          SAWYER_MESG(CodeThorn::logger[WARN])<<"Byte pointer subtraction gives value non-divisible by element size. Using top as result:"<<a.toString(_variableIdMapping)<<"-"<<b.toString(_variableIdMapping)<<endl;
          return Top();
        } else {
          val.intValue=(a.intValue-b.intValue)/pointerElementSize;
          val.valueType=INTEGER;
          val.variableId=a.variableId; // same as b.variableId
        }
      } else {
        val.intValue=a.intValue-b.intValue;
        val.valueType=INTEGER;
        val.variableId=a.variableId; // same as b.variableId
      }
      return val;
    } else {
      return Top(); // subtraction of incompatible pointers gives arbitrary value
    }
  } else if(a.isPtr() && b.isConstInt()) {
    AbstractValue val=a;
    if(byteMode) {
      int pointerElementSize=_variableIdMapping->getElementSize(a.getVariableId());
      val.intValue-=a.intValue*pointerElementSize;
    } else {
      val.intValue-=b.intValue;
    }
    return val;
  } else if(a.isConstInt() && b.isPtr()) {
    if(strictChecking)
      throw CodeThorn::Exception("Error: forbidden operation in '-' operation. Attempt to subtract pointer from integer.");
    return createTop();
  } else if(a.isConstInt() && b.isConstInt()) {
    return a.getIntValue()-b.getIntValue();
  } else {
    if(strictChecking)
      throw CodeThorn::Exception("Error: undefined behavior in binary '-' operation.");
    return createTop();
  }
}

AbstractValue AbstractValue::topOrError(std::string errorMsg) const {
  if(strictChecking) {
    throw CodeThorn::Exception(errorMsg);
  } else {
    return createTop();
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
    case PTR:
    case REF: return (val1.getVariableId()==val2.getVariableId()&&val1.intValue==val2.intValue);
    case FUN_PTR: return (val1.label==val2.label);
    case TOP:
    case UNDEFINED:
      // should be unreachable because of 2nd if-condition above
      // TODO: enforce non-reachable here
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
      if(val1.getVariableId()==val2.getVariableId()
         &&val1.getIntValue()==val2.getIntValue()) {
        return val1;
      } else {
        return createTop();
      }
    }
    case FUN_PTR: {
      if(val1.label==val2.label) {
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

bool AbstractValue::isReferenceVariableAddress() {
  if(isPtr()||isNullPtr()||isRef()) {
    return getVariableIdMapping()->hasReferenceType(getVariableId());
  }
  return false;
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

CodeThorn::AlignedMemLoc AbstractValue::alignedMemLoc() {
  AbstractValue arbitraryMemLoc=*this;
  VariableId varId=arbitraryMemLoc.getVariableId();
  long int offset=arbitraryMemLoc.getIndexIntValue();
  ROSE_ASSERT(AbstractValue::_variableIdMapping);
  long int pointerValueElemSize=arbitraryMemLoc.getElementTypeSize();
  long int inStateElemSize=(long int)AbstractValue::_variableIdMapping->getElementSize(varId);
  arbitraryMemLoc.setElementTypeSize(inStateElemSize); // adapt element size when storing in state
  if(pointerValueElemSize!=inStateElemSize) {
    if(inStateElemSize!=0) {
      long int withinElementOffset=offset%inStateElemSize;
      if(withinElementOffset!=0) {
        // TODO: access within element with mod as byte offset
        // need to know the element size of the pointer to mask it properly
        arbitraryMemLoc.setValue(offset-withinElementOffset); // adjustment to element-aligned offset
      }
    }
  }
  return AlignedMemLoc(arbitraryMemLoc,offset);
}

AlignedMemLoc::AlignedMemLoc(AbstractValue av,int offset) {
  memLoc=av;
  this->offset=offset;
}
