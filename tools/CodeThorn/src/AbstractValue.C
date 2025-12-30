#include "sage3basic.h"
#include "AbstractValue.h"
#include "assert.h"
#include "CommandLineOptions.h"
#include <iostream>
#include <limits>
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "CodeThornException.h"
#include "VariableIdMapping.h"
#include "CodeThornLib.h"
#include "ROSE_FALLTHROUGH.h"

using namespace std;
using namespace CodeThorn;
using namespace Sawyer::Message; // required for logger

//int32_t CodeThorn::AbstractValue::arrayAbstractionIndex=-1; // -1 is used for turning array abstraction off (which is different to a max value)

VariableIdMappingExtended* AbstractValue::_variableIdMapping=nullptr;
bool AbstractValue::strictChecking=false;
bool AbstractValue::byteMode=false;
bool AbstractValue::pointerSetsEnabled=false;
int AbstractValue::domainAbstractionVariant=0; // default
AbstractValue AbstractValue::_pointerToArbitraryMemory=AbstractValue::createBot(); // default bottom element (feature is activated by set-function)

using CodeThorn::logger;

istream& CodeThorn::operator>>(istream& is, AbstractValue& value) {
  value.fromStream(is);
  return is;
}

// default constructor (AV_UNDEFINED instead of AV_BOT)
AbstractValue::AbstractValue():valueType(AbstractValue::AV_UNDEFINED),extension(0) {}

// type conversion
// TODO: represent value 'undefined' here
AbstractValue::AbstractValue(VariableId varId):valueType(AbstractValue::AV_PTR),variableId(varId),intValue(0) {
  if(byteMode) {
    // also set element type size
    ROSE_ASSERT(_variableIdMapping);
  }
}

AbstractValue::AbstractValue(Label lab):valueType(AbstractValue::AV_FUN_PTR),label(lab) {}

AbstractValue::~AbstractValue() {
  switch(valueType) {
  case AV_BOT:
  case AV_INTEGER:
  case AV_FP_SINGLE_PRECISION:
  case AV_FP_DOUBLE_PRECISION:
  case AV_PTR:
  case AV_REF:
  case AV_FUN_PTR:
  case AV_TOP:
  case AV_UNDEFINED:
    // nothing to do
    break;
  case AV_SET:
    deallocateExtension();
    break;
    // intentionally no default case to get compiler warning if one is missing
  }
}

AbstractValueSet* AbstractValue::abstractValueSetCopy() const {
  AbstractValueSet* avs=new AbstractValueSet();
  AbstractValueSet* current=getAbstractValueSet();
  for(AbstractValueSet::iterator i=current->begin();i!=current->end();++i) {
    avs->insert(*i);
  }
  return avs;
}

void AbstractValue::copy(const AbstractValue& other) {
  valueType = other.valueType;
  variableId = other.variableId;
  label= other.label;
  _abstractionFlag=other._abstractionFlag;
  switch(other.valueType) {
  case AV_BOT:
  case AV_TOP:
  case AV_UNDEFINED:
  case AV_FUN_PTR: // uses label
    break;
  case AV_PTR:
  case AV_REF:
  case AV_INTEGER: intValue=other.intValue;
    break;
  case AV_FP_SINGLE_PRECISION: floatValue=other.floatValue;
    break;
  case AV_FP_DOUBLE_PRECISION: doubleValue=other.doubleValue;
    break;
  case AV_SET: extension=other.abstractValueSetCopy();
    break;
  }
}

AbstractValue::AbstractValue(const AbstractValue& other) {
  copy(other);
}

AbstractValue& AbstractValue::operator=(AbstractValue other) {
  copy(other);
  return *this;
}
// type conversion
AbstractValue::AbstractValue(bool val) {
  if(val) {
    valueType=AbstractValue::AV_INTEGER;
    intValue=1;
  } else {
    valueType=AbstractValue::AV_INTEGER;
    intValue=0;
  }
}

void AbstractValue::setPointerToArbitraryMemory(AbstractValue ptr) {
  ROSE_ASSERT(ptr.isPtr());
  AbstractValue::_pointerToArbitraryMemory=ptr;
}

AbstractValue AbstractValue::getPointerToArbitraryMemory() {
  // by default bot
  return AbstractValue::_pointerToArbitraryMemory;
}

bool AbstractValue::isPointerToArbitraryMemory() const {
  return *this==getPointerToArbitraryMemory();
}

void AbstractValue::setVariableIdMapping(VariableIdMappingExtended* varIdMapping) {
  AbstractValue::_variableIdMapping=varIdMapping;
}

CodeThorn::VariableIdMappingExtended* AbstractValue::getVariableIdMapping() {
  return AbstractValue::_variableIdMapping;
}

CodeThorn::TypeSize AbstractValue::calculateTypeSize(CodeThorn::BuiltInType btype) {
  ROSE_ASSERT(AbstractValue::_variableIdMapping);
  return AbstractValue::_variableIdMapping->getBuiltInTypeSize(btype);
}

void AbstractValue::setValue(long int val) {
  // TODO: truncate here if necessary
  intValue=val;
}

void AbstractValue::setValue(float fval) {
  floatValue=fval;
}
void AbstractValue::setValue(double fval) {
  doubleValue=fval;
}

AbstractValue AbstractValue::createIntegerValue(CodeThorn::BuiltInType btype, long long int ival) {
  AbstractValue aval;
  aval.initInteger(btype,ival);
  return aval;
}

void AbstractValue::initInteger(CodeThorn::BuiltInType /*btype*/, long int ival) {
  valueType=AbstractValue::AV_INTEGER;
  setValue(ival);
}

void AbstractValue::initFloat(CodeThorn::BuiltInType btype, float fval) {
  ROSE_ASSERT(btype==BITYPE_FLOAT);
  valueType=AbstractValue::AV_FP_SINGLE_PRECISION;
  setValue(fval);
}

void AbstractValue::initDouble(CodeThorn::BuiltInType btype, double dval) {
  ROSE_ASSERT(btype==BITYPE_DOUBLE);
  valueType=AbstractValue::AV_FP_DOUBLE_PRECISION;
  setValue(dval);
}

// type conversion
AbstractValue::AbstractValue(Top /*e*/) {valueType=AbstractValue::AV_TOP;intValue=0;} // intValue=0 superfluous
// type conversion
AbstractValue::AbstractValue(Bot /*e*/) {valueType=AbstractValue::AV_BOT;intValue=0;} // intValue=0 superfluous

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
  initDouble(BITYPE_DOUBLE,x);
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
  ROSE_ASSERT(arrayVariableId.isValid());
  return createAddressOfArrayElement(arrayVariableId, index, AbstractValue(1));
}

AbstractValue
AbstractValue::createAddressOfArrayElement(CodeThorn::VariableId arrayVariableId, AbstractValue index, AbstractValue elementSize) {
  ROSE_ASSERT(arrayVariableId.isValid());
  if(index.isTop()||elementSize.isTop()) {
    return Top();
  } else if(index.isBot()) {
    return Bot();
  } else if(index.isConstInt()) {
    // create address of array element 0 and add index by multiplying it with element size
    AbstractValue addr0;
    addr0.valueType=AV_PTR;
    addr0.variableId=arrayVariableId;
    addr0.intValue=0;
    AbstractValue arrayElemAddress=operatorAdd(addr0,index,elementSize);
    if(pointerSetsEnabled) {
      return convertPtrToPtrSet(arrayElemAddress);
    } else {
      return arrayElemAddress;
    }
  } else {
    cerr<<"Error: createAddressOfArray: non-const index and not top or bot either."<<endl;
    exit(1);
  }
}

AbstractValue
AbstractValue::createAddressOfFunction(CodeThorn::Label lab) {
  return AbstractValue(lab);
}

AbstractValue AbstractValue::convertPtrToPtrSet(AbstractValue val) {
  ROSE_ASSERT(val.isPtr());
  AbstractValue newVal;
  newVal.allocateExtension(AV_SET);
  ROSE_ASSERT(newVal.isPtrSet());
  newVal.addSetElement(val);
  return newVal;
}

AbstractValue AbstractValue::conditionallyApplyArrayAbstraction(AbstractValue val) {
  //cout<<"DEBUG: AbstractValue::condapply: "<<val.toString(getVariableIdMapping())<<endl;
  int arrayAbstractionIndex=getVariableIdMapping()->getArrayAbstractionIndex();
  if(arrayAbstractionIndex>=0) {
    //cout<<"DEBUG: array abstraction active starting at index: "<<arrayAbstractionIndex<<endl;
    if(val.isPtr()&&!val.isNullPtr()) {
      VariableId memLocId=val.getVariableId();
      if(getVariableIdMapping()->isOfArrayType(memLocId)) {
        AbstractValue index=val.getIndexValue();
        if(!index.isTop()&&!index.isBot()) {
          int offset=val.getIndexIntValue();
          auto remappingEntry=getVariableIdMapping()->getOffsetAbstractionMappingEntry(memLocId,offset);
          if(remappingEntry.getIndexRemappingType()==VariableIdMappingExtended::IndexRemappingEnum::IDX_REMAPPED) {
            SAWYER_MESG(logger[TRACE])<<"remapping index "<<offset<<" -> "<<remappingEntry.getRemappedOffset()<<endl;
            val.setValue(remappingEntry.getRemappedOffset());
            val.setAbstractFlag(true);
          }
        }
      }
    }
  }
  return val;
}

std::string AbstractValue::valueTypeToString() const {
  switch(valueType) {
  case AV_TOP: return "top";
  case AV_UNDEFINED: return "uninit";
  case AV_INTEGER: return "constint";
  case AV_FP_SINGLE_PRECISION: return "float";
  case AV_PTR: return "ptr";
  case AV_FUN_PTR: return "funptr";
  case AV_REF: return "ref";
  case AV_BOT: return "bot";
  case AV_SET: return "ptrset";
  default:
    return "unknown";
  }
}

// currently maps to isTop(); in preparation for explicit handling of
// undefined values.
bool AbstractValue::isUndefined() const {return valueType==AbstractValue::AV_UNDEFINED;}
bool AbstractValue::isTop() const {return valueType==AbstractValue::AV_TOP||isUndefined();}
//bool AbstractValue::isTop() const {return valueType==AbstractValue::AV_TOP;}
bool AbstractValue::isTopOrUndefinedOrArbitraryMemPtrOrAbstract() const {return isTop()||isUndefined()||isPointerToArbitraryMemory()||isAbstract();}
bool AbstractValue::isTrue() const {return valueType==AbstractValue::AV_INTEGER && intValue!=0;}
bool AbstractValue::isFalse() const {return valueType==AbstractValue::AV_INTEGER && intValue==0;}
bool AbstractValue::isBot() const {return valueType==AbstractValue::AV_BOT;}
bool AbstractValue::isConstInt() const {return valueType==AbstractValue::AV_INTEGER;}
bool AbstractValue::isConstFloat() const {return valueType==AbstractValue::AV_FP_SINGLE_PRECISION;}
bool AbstractValue::isConstDouble() const {return valueType==AbstractValue::AV_FP_DOUBLE_PRECISION;}
bool AbstractValue::isConstPtr() const {return (valueType==AbstractValue::AV_PTR);}
bool AbstractValue::isPtr() const {return (valueType==AbstractValue::AV_PTR);}
// deprecated (use isAVSet instead)
bool AbstractValue::isPtrSet() const {return (isAVSet());}
bool AbstractValue::isAVSet() const {return (valueType==AbstractValue::AV_SET);}
bool AbstractValue::isFunctionPtr() const {return (valueType==AbstractValue::AV_FUN_PTR);}
bool AbstractValue::isRef() const {return (valueType==AbstractValue::AV_REF);}
bool AbstractValue::isNullPtr() const {return (valueType==AbstractValue::AV_INTEGER && intValue==0 && !isAbstract() && !isPointerToArbitraryMemory());}

bool AbstractValue::isAbstract() const {
  if(isTop()||_abstractionFlag)
     return true;
  if(isAVSet()) {
    if(getAVSetSize() > 1) {
      // if more than one value it is always considered to be a summary
      return true;
    } else if(getAVSetSize()==1) {
      // special case of one AV element only, can be both abstract or concrete
      AbstractValueSet* avSet=getAbstractValueSet();
      AbstractValue av=*avSet->begin();
      return av.isAbstract();
    }
    // an empty AV set is not a summary
  }
  return false;
}

void AbstractValue::setAbstractFlag(bool flag) {
  _abstractionFlag=flag;
}
bool AbstractValue::getAbstractFlag() {
  return _abstractionFlag;
}

size_t AbstractValue::hash() const {
  if(isTop()) return LONG_MAX;
  else if(isBot()) return LONG_MIN;
  else if(isConstInt()) return getIntValue();
  else if(isConstFloat()) return getFloatValue();
  else if(isConstDouble()) return getDoubleValue();
  else if(isPtr()||isRef()) {
    VariableId varId=getVariableId();
    ROSE_ASSERT(varId.isValid());
    return varId.getIdCode()+getIntValue();
  } else if(isFunctionPtr()) {
    return (size_t)getLabel().getId();
  } else {
    if(strictChecking)
      throw CodeThorn::Exception("Error: AbstractValue hash: unknown value.");
    return LONG_MAX<<1;
  }
}

bool AbstractValue::ptrSetContainsNullPtr() const {
  ROSE_ASSERT(valueType==AV_SET);
  return getAbstractValueSet()->find(createNullPtr())!=getAbstractValueSet()->end();
}

// deprecated
size_t AbstractValue::getPtrSetSize() const {
  return getAVSetSize();
}

size_t AbstractValue::getAVSetSize() const {
  ROSE_ASSERT(valueType==AV_SET);
  return getAbstractValueSet()->size();
}

AbstractValue AbstractValue::operatorNot() {
  AbstractValue tmp;
  switch(valueType) {
  case AbstractValue::AV_INTEGER:
    tmp.valueType=valueType;
    if(intValue==0) {
      tmp.intValue=1;
    } else {
      tmp.intValue=0;
    }
    break;
  case AbstractValue::AV_TOP: tmp=Top();break;
  case AbstractValue::AV_BOT: tmp=Bot();break;
  case AbstractValue::AV_UNDEFINED: tmp=*this;break;
  case AbstractValue::AV_SET:
    if(AbstractValue::ptrSetContainsNullPtr() && getPtrSetSize()>1) {
    tmp=Top();
  } else if(AbstractValue::ptrSetContainsNullPtr() && getPtrSetSize()==1) {
    tmp.intValue=1;
  } else if(!AbstractValue::ptrSetContainsNullPtr()) {
    tmp.intValue=0;
  } else {
    SAWYER_MESG(logger[ERROR])<<"Error: unhandled case in AbstractValue::operatorNot() (AV_SET)."<<endl;
    exit(1);
  }
  break; // PP (20/11/24)
  default:
    // other cases should not appear because there must be a proper cast
    SAWYER_MESG(logger[WARN])<<"AbstractValue::operatorNot: unhandled abstract value "<<tmp.toString()<<". Assuming any value as result."<<endl;
    tmp=Top();
  }
  return tmp;
}

// the following wrapper functions can be eliminated if all called functions are rewritten for 2 parameters
AbstractValue AbstractValue::operatorOr(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorOr(v2);
}
AbstractValue AbstractValue::operatorAnd(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorAnd(v2);
}
AbstractValue AbstractValue::operatorEq(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorEq(v2);
}
AbstractValue AbstractValue::operatorNotEq(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorNotEq(v2);
}
AbstractValue AbstractValue::operatorLess(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorLess(v2);
}
AbstractValue AbstractValue::operatorLessOrEq(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorLessOrEq(v2);
}
AbstractValue AbstractValue::operatorMoreOrEq(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorMoreOrEq(v2);
}
AbstractValue AbstractValue::operatorMore(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorMore(v2);
}
AbstractValue AbstractValue::operatorBitwiseAnd(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorBitwiseAnd(v2);
}
AbstractValue AbstractValue::operatorBitwiseOr(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorBitwiseOr(v2);
}
AbstractValue AbstractValue::operatorBitwiseXor(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorBitwiseXor(v2);
}
AbstractValue AbstractValue::operatorBitwiseShiftLeft(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorBitwiseShiftLeft(v2);
}
AbstractValue AbstractValue::operatorBitwiseShiftRight(AbstractValue& v1, AbstractValue& v2) {
  return v1.operatorBitwiseShiftRight(v2);
}

AbstractValue AbstractValue::operatorBitwiseComplement(AbstractValue& v1) {
  return v1.operatorBitwiseComplement();
}
AbstractValue AbstractValue::operatorNot(AbstractValue& v1) {
  return v1.operatorNot();
}
AbstractValue AbstractValue::operatorUnaryMinus(AbstractValue& v1) {
  return v1.operatorUnaryMinus();
}

AbstractValue AbstractValue::operatorOr(AbstractValue other) const {
  AbstractValue tmp;
  // all AV_TOP cases
  if(isTop()   && other.isTop())   return Top();
  if(isTop()   && other.isTrue())  return true;
  if(isTrue()  && other.isTop())   return true;
  if(isTop()   && other.isFalse()) return Top();
  if(isFalse() && other.isTop())   return Top();
  // all AV_BOT cases
  if(valueType==AV_BOT) {
    tmp.valueType=other.valueType;
    tmp.intValue=other.intValue;
    return tmp;
  }
  if(other.valueType==AV_BOT) {
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

AbstractValue AbstractValue::operatorAnd(AbstractValue other) const {
  AbstractValue tmp;
  // all AV_TOP cases
  if(isTop()   && other.isTop())   return Top();
  if(isTop()   && other.isTrue())  return Top();
  if(isTrue()  && other.isTop())   return Top();
  if(isTop()   && other.isFalse()) return false;
  if(isFalse() && other.isTop())   return false;
  // all AV_BOT cases
  if(valueType==AV_BOT) {
    tmp.valueType=other.valueType;
    tmp.intValue=other.intValue;
    return tmp;
  }
  if(other.valueType==AV_BOT) {
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
    } else if(c1.isConstFloat() && c2.isConstFloat()) {
      return c1.getFloatValue()<c2.getFloatValue();
    } else if(c1.isConstDouble() && c2.isConstDouble()) {
      return c1.getDoubleValue()<c2.getDoubleValue();
    } else if(c1.isPtr() && c2.isPtr()) {
      if(c1.getVariableId()!=c2.getVariableId()) {
        return c1.getVariableId()<c2.getVariableId();
      } else {
        //if(c1.getIntValue()!=c2.getIntValue()) {
          return c1.getIntValue()<c2.getIntValue();
    //}
  /*else {
          if(c1.getTypeSize()!=c2.getTypeSize())
            return c1.getTypeSize()<c2.getTypeSize();
          else
            return c1.getElementTypeSize()<c2.getElementTypeSize();
        }
  */
      }
    } else if(c1.isPtrSet() && c2.isPtrSet()) {
      if(c1.getPtrSetSize()!=c2.getPtrSetSize()) {
        return c1.getPtrSetSize()<c2.getPtrSetSize();
      } else {
        // since the set is an ordered set, there exists a weak ordering
        // [1,2,3]<[1,2,4]; [1,2,3] >= [1,2,3]; [1,4,5] > [1,3,6]
        AbstractValueSet& s1=*c1.getAbstractValueSet();
        AbstractValueSet& s2=*c2.getAbstractValueSet();
        for(auto e1 : s1) {
          size_t eqCnt=0;
          for(auto e2 : s2) {
            if(e1!=e2) {
              if(!(e1<e2))
                return false;
            } else {
              eqCnt++;
            }
            if(eqCnt==s2.size()) {
              return false; // case ==
            }
          }
        }
        return true;
      }
    } else if(c1.isAbstract()!=c2.isAbstract()) {
      return c1.isAbstract()==false && c2.isAbstract()==true;
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
    if(c1.isConstFloat() && c2.isConstFloat())
      return c1.getFloatValue()==c2.getFloatValue();
    if(c1.isConstDouble() && c2.isConstDouble())
      return c1.getDoubleValue()==c2.getDoubleValue();
    else if(c1.isPtr() && c2.isPtr()) {
      return c1.getVariableId()==c2.getVariableId() && c1.getIntValue()==c2.getIntValue();
    } else if(c1.isFunctionPtr() && c2.isFunctionPtr()) {
      return c1.getLabel()==c2.getLabel();
    } else if(c1.isPtrSet() && c2.isPtrSet()) {
      return c1.getAbstractValueSet()->isEqual(*c2.getAbstractValueSet());
    } else if(c1.isAbstract()!=c2.isAbstract()) {
      return false;
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
  // all AV_TOP cases
  if(isTopOrUndefinedOrArbitraryMemPtrOrAbstract() || other.isTopOrUndefinedOrArbitraryMemPtrOrAbstract()) {
    return CodeThorn::Top();
  }
  // all AV_BOT cases
  if(valueType==AV_BOT) {
    return other;
  } else if(other.valueType==AV_BOT) {
    return *this;
  } else if(isPtr() && other.isPtr()) {
    // if any of the two is an abitrary mem loc, it's unknown whether they are equal
    if(isPointerToArbitraryMemory()||other.isPointerToArbitraryMemory()) {
      return CodeThorn::Top();
    }
    // if they refer to different memory regions they are definitely different
    if(variableId!=other.variableId) {
      return AbstractValue(0);
    }
    if(!isAbstract()&&!other.isAbstract()) {
      return AbstractValue(variableId==other.variableId && intValue==other.intValue /*&& getTypeSize()==other.getTypeSize()*/);
    }
    return CodeThorn::Top();
  } else if(isConstInt() && other.isConstInt()) {
    // includes case for two null pointer values
    return AbstractValue(intValue==other.intValue /*&& getTypeSize()==other.getTypeSize()*/);
  } else if((!isNullPtr() && other.isNullPtr()) || (isNullPtr() && !other.isNullPtr()) ) {
    return AbstractValue(0);
  } else {
    return AbstractValue(Top()); // all other cases can be true or false
  }
}

AbstractValue AbstractValue::operatorNotEq(AbstractValue other) const {
  return ((*this).operatorEq(other)).operatorNot();
}

AbstractValue AbstractValue::operatorLess(AbstractValue other) const {
  if(isTopOrUndefinedOrArbitraryMemPtrOrAbstract()||other.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  if(isPtr() && other.isPtr()) {
    // check is same memory region
    if(variableId==other.variableId) {
      return AbstractValue(intValue<other.intValue /*&& getTypeSize()==other.getTypeSize()*/);
    } else {
      // incompatible pointer comparison (can be true or false)
      return AbstractValue::createTop();
    }
  }
  if(isConstInt()&&other.isConstInt()) {
    return getIntValue()<other.getIntValue();
  }
  return AbstractValue::createTop();
}

AbstractValue AbstractValue::operatorLessOrEq(AbstractValue other) const {
  if(isTopOrUndefinedOrArbitraryMemPtrOrAbstract()||other.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  if(isConstInt()&&other.isConstInt()) {
    return getIntValue()<=other.getIntValue();
  }
  return AbstractValue::createTop();
}

AbstractValue AbstractValue::operatorMoreOrEq(AbstractValue other) const {
  if(isTopOrUndefinedOrArbitraryMemPtrOrAbstract()||other.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  if(isConstInt()&&other.isConstInt())
    return getIntValue()>=other.getIntValue();
  return AbstractValue::createTop();
}

AbstractValue AbstractValue::operatorMore(AbstractValue other) const {
  if(isTopOrUndefinedOrArbitraryMemPtrOrAbstract()||other.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  if(isConstInt()&&other.isConstInt()) {
    return getIntValue()>other.getIntValue();
  }
  return AbstractValue::createTop();
}

AbstractValue AbstractValue::operatorBitwiseOr(AbstractValue other) const {
  if(isTopOrUndefinedOrArbitraryMemPtrOrAbstract()||other.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  if(isConstInt()&&other.isConstInt())
    return getIntValue()|other.getIntValue();
  return AbstractValue::createTop();
}

AbstractValue AbstractValue::operatorBitwiseXor(AbstractValue other) const {
  if(isTopOrUndefinedOrArbitraryMemPtrOrAbstract()||other.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  if(isConstInt()&&other.isConstInt())
    return getIntValue()^other.getIntValue();
  return AbstractValue::createTop();
}

AbstractValue AbstractValue::operatorBitwiseAnd(AbstractValue other) const {
  if(isTopOrUndefinedOrArbitraryMemPtrOrAbstract()||other.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
    return Top();
  if(isBot())
    return other;
  if(other.isBot())
    return *this;
  if(isConstInt()&&other.isConstInt())
    return getIntValue()&other.getIntValue();
  return AbstractValue::createTop();
}

AbstractValue AbstractValue::operatorBitwiseComplement() const {
  if(isTopOrUndefinedOrArbitraryMemPtrOrAbstract()||isBot())
    return *this;
  if(isConstInt())
    return ~getIntValue();
  return AbstractValue::createTop();
}

AbstractValue AbstractValue::operatorBitwiseShiftLeft(AbstractValue other) const {
  if(isTopOrUndefinedOrArbitraryMemPtrOrAbstract()||other.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
    return Top();
  if(isBot())
    return *this;
  if(other.isBot())
    return *this;
  if(isConstInt()&&other.isConstInt())
    return getIntValue()<<other.getIntValue();
  return AbstractValue::createTop();
}

AbstractValue AbstractValue::operatorBitwiseShiftRight(AbstractValue other) const {
  if(isTopOrUndefinedOrArbitraryMemPtrOrAbstract()||other.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
    return Top();
  if(isBot())
    return *this;
  if(other.isBot())
    return *this;
  if(isConstInt()&&other.isConstInt())
    return getIntValue()>>other.getIntValue();
  return AbstractValue::createTop();
}

string AbstractValue::toLhsString(CodeThorn::VariableIdMapping* vim) const {
  switch(valueType) {
  case AV_TOP: return "top";
  case AV_BOT: return "bot";
  case AV_UNDEFINED: return "undefined(lhs)";
  case AV_INTEGER: {
    stringstream ss;
    ss<<getIntValue();
    return ss.str();
  }
  case AV_SET: {
    // print set of abstract values
    AbstractValueSet& avSet=*static_cast<AbstractValueSet*>(extension);
    stringstream ss;
    ss<<"{";
    for (auto el:avSet) {
      el.toString(vim);
      ss<<" ";
    }
    ss<<"}";
    return ss.str(); // PP (20/11/24)
  }
  case AV_PTR: {
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
      stringstream ss;ss<<valueType;
      return "<<ERROR0-"+ss.str()+">>";
    }
  }
}

string AbstractValue::toRhsString(CodeThorn::VariableIdMapping* vim) const {
  switch(valueType) {
  case AV_TOP: return "top";
  case AV_BOT: return "bot";
  case AV_UNDEFINED: return "uninit(rhs)";
  case AV_INTEGER: {
    stringstream ss;
    ss<<getIntValue();
    return ss.str();
  }
  case AV_PTR: {
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
      return "<<ERROR1>>";
    }
  }
}

string AbstractValue::arrayVariableNameToString(CodeThorn::VariableIdMapping* vim) const {
  switch(valueType) {
  case AV_PTR: {
    stringstream ss;
    ss<<variableId.toString(vim);
    return ss.str();
  }
  default:
    if(strictChecking) {
      throw CodeThorn::Exception("Error: AbstractValue::arrayVariableNameToString operation failed. Unknown abstraction type.");
    } else {
      return "<<ERROR2>>";
    }
  }
}

string AbstractValue::abstractToString() const {
  return isAbstract()?"*":"";
}

string AbstractValue::toString(CodeThorn::VariableIdMapping* vim) const {
  switch(valueType) {
  case AV_TOP: return "top";
  case AV_BOT: return "bot";
  case AV_UNDEFINED: return "uninit"+abstractToString();
  case AV_INTEGER: {
    stringstream ss;
    ss<<getIntValue()<<abstractToString();
    return ss.str();
  }
  case AV_FP_SINGLE_PRECISION:
  case AV_FP_DOUBLE_PRECISION: {
    return getFloatValueString()+abstractToString();
  }
  case AV_PTR: {
    //    if(vim->isOfArrayType(variableId)||vim->isOfClassType(variableId)||vim->isOfReferenceType(variableId)||vim->isHeapMemoryRegionId(variableId)) {
      stringstream ss;
      ss<<"("
        <<variableId.toString(vim) // toUniqueString: problem in regression tests, because IDs can be different due to different number of globals in different OS versions.
        <<","
        <<getIntValue()
        //<<","
        //<<getElementTypeSize()
        <<")"
        <<abstractToString()
        ;
      return ss.str();
      //    } else {
      //      return variableId.toString(vim);
      //    }
  }
  case AV_SET: {
    // print set of abstract values
    AbstractValueSet& avSet=*static_cast<AbstractValueSet*>(extension);
    stringstream ss;
    ss<<"{";
    for (auto el:avSet) {
      ss<<el.toString(vim);
      ss<<" ";
    }
    ss<<"}";
    return ss.str();
  }
  case AV_FUN_PTR: {
    return "fptr:"+label.toString();
  }
  default:
    if(strictChecking) {
      throw CodeThorn::Exception("Error: AbstractValue::toString operation failed. Unknown abstraction type.");
    } else {
      return "<<ERROR3>>";
    }
  }
}

string AbstractValue::toString() const {
  if(VariableIdMapping* vim=getVariableIdMapping()) {
    return toString(vim);
  }
  switch(valueType) {
  case AV_TOP: return "top";
  case AV_BOT: return "bot";
  case AV_UNDEFINED: return "uninit";
  case AV_INTEGER: {
    stringstream ss;
    ss<<getIntValue();
    return ss.str();
  }
  case AV_FP_SINGLE_PRECISION:
  case AV_FP_DOUBLE_PRECISION: {
    return getFloatValueString();
  }
  case AV_PTR: {
    stringstream ss;
    ss<<"("<<variableId.toString()<<","<<getIntValue()<<","<<isAbstract()<<")";
    return ss.str();
  }
  case AV_FUN_PTR: {
    return "fptr:"+label.toString();
  }
  case AV_SET: {
    // print set of abstract values
    AbstractValueSet& avSet=*static_cast<AbstractValueSet*>(extension);
    stringstream ss;
    ss<<"{";
    for (auto el:avSet) {
      ss<<el.toString();
      ss<<" ";
    }
    ss<<"}";
    return ss.str();
  }
  default:
    if(strictChecking) {
      throw CodeThorn::Exception("Error: AbstractValue::toString operation failed. Unknown abstraction type.");
    } else {
      return "<<ERROR4>>";
    }
  }
}

void AbstractValue::fromStream(istream& is) {
  int tmpintValue=0;
  if(CodeThorn::Parse::checkWord("top",is)) {
    valueType=AV_TOP;
    intValue=0;
  } else if(CodeThorn::Parse::checkWord("bot",is)) {
    valueType=AV_BOT;
    intValue=0;
  } else if(CodeThorn::Parse::integer(is,tmpintValue)) {
    valueType=AV_INTEGER;
    intValue=tmpintValue;
  } else {
    throw CodeThorn::Exception("Error: ConstIntLattic::fromStream failed.");
  }
}

AbstractValue::ValueType AbstractValue::getValueType() const {
  return valueType;
}

/*
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
*/

AbstractValue AbstractValue::getIndexValue() const {
  if(isTop()||isBot()) {
    return *this;
  } else {
    return AbstractValue(getIndexIntValue());
  }
}

int AbstractValue::getIndexIntValue() const {
  if(valueType!=AV_PTR) {
    cerr << "AbstractValue::getIndexIntValue:  valueType="<<valueTypeToString()<<endl;
    throw CodeThorn::Exception("Error: AbstractValue::getIndexIntValue operation failed.");
  }
  else
    return (int)intValue;
}

int AbstractValue::getIntValue() const {
  // TODO: AV_PTR will be removed once all ptrs are adapted to getIndexIntValue
  if(valueType!=AV_INTEGER && valueType!=AV_PTR) {
    cerr << "AbstractValue::getIntValue:  valueType="<<valueTypeToString()<<endl;
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
  return (double)doubleValue;
}
/*
long double AbstractValue::getLongDoubleValue() const {
  return (long double)floatValue;
}
*/
std::string AbstractValue::getFloatValueString() const {
   if(valueType!=AV_FP_SINGLE_PRECISION && valueType!=AV_FP_DOUBLE_PRECISION) {
     cerr << "AbstractValue::getFloatValueString: valueType="<<valueTypeToString()<<endl;
     throw CodeThorn::Exception("Error: AbstractValue::getFloatValueString operation failed.");
   } else {
     stringstream ss;
     // emulate printf output
     if(valueType==AV_FP_SINGLE_PRECISION) {
       ss<<std::fixed<<std::setprecision(6)<<floatValue;
     } else {
       ss<<std::fixed<<std::setprecision(6)<<doubleValue;
     }
     return ss.str();
   }
}

CodeThorn::VariableId AbstractValue::getVariableId() const {
  if(valueType!=AV_PTR && valueType!=AV_REF) {
    cerr << "AbstractValue::getVariableId() valueType="<<valueTypeToString()<<endl;
    cerr << "AbstractValue: value:"<<toString()<<endl;
    int *x=0;
    *x=1; // trigger stack trace
    throw CodeThorn::Exception("Error: AbstractValue::getVariableId operation failed.");
  }
  else
    return variableId;
}

// arithmetic operators
AbstractValue AbstractValue::operatorUnaryMinus() {
  AbstractValue tmp;
  switch(valueType) {
  case AbstractValue::AV_INTEGER:
    tmp.valueType=AbstractValue::AV_INTEGER;
    tmp.intValue=-intValue; // unary minus
    break;
  case AbstractValue::AV_FP_SINGLE_PRECISION:
    tmp.valueType=AbstractValue::AV_FP_SINGLE_PRECISION;
    tmp.floatValue=-floatValue; // unary minus
    break;
  case AbstractValue::AV_FP_DOUBLE_PRECISION:
    tmp.valueType=AbstractValue::AV_FP_DOUBLE_PRECISION;
    tmp.doubleValue=-doubleValue; // unary minus
    break;
  case AbstractValue::AV_TOP:
    tmp=Top();break;
  case AbstractValue::AV_UNDEFINED:
    tmp=*this;break; // keep information that it is undefined
  case AbstractValue::AV_BOT: tmp=Bot();break;
  case AbstractValue::AV_PTR:
  case AbstractValue::AV_FUN_PTR:
    return topOrError("Error: AbstractValue operator unary minus on pointer value.");
  case AbstractValue::AV_REF:
    return topOrError("Error: AbstractValue operator unary minus on reference value.");
    //  default case intentionally not present to force all values to be handled explicitly
  case AbstractValue::AV_SET:
    return topOrError("Error: AbstractValue operator unary minus on ptrset value.");
  }
  return tmp;
}

AbstractValue AbstractValue::operatorAdd(AbstractValue& a,AbstractValue& b, AbstractValue elementSize) {
  if(a.isPtr()) {
    AbstractValue offset=operatorMul(b,elementSize);
    return operatorAdd(a,offset);
  } else if(b.isPtr()) {
    AbstractValue offset=operatorMul(a,elementSize);
    return operatorAdd(b,offset);
  } else {
    return operatorAdd(a,b);
  }
}

AbstractValue AbstractValue::operatorSub(AbstractValue& a,AbstractValue& b, AbstractValue elementSize) {
  if(a.isPtr() && b.isPtr()) {
    AbstractValue diffVal=operatorSub(a,b);
    return operatorMul(diffVal,elementSize);
  } else if(a.isPtr()) {
    AbstractValue offset=operatorMul(b,elementSize);
    return operatorSub(a,offset);
  } else if(b.isPtr()) {
    AbstractValue offset=operatorMul(a,elementSize);
    return operatorSub(b,offset);
  } else {
    return operatorSub(a,b);
  }
}

AbstractValue AbstractValue::operatorAdd(AbstractValue& a,AbstractValue& b) {
  if(a.isPointerToArbitraryMemory())
    return a;
  if(b.isPointerToArbitraryMemory())
    return b;
  if((a.isPtr()&&b.isTop())||(b.isPtr()&&a.isTop())) {
    auto amp=AbstractValue::getPointerToArbitraryMemory();
    if(!amp.isBot()) {
      return amp;
    }
  }
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
    if(strictChecking)
      throw CodeThorn::Exception("Error: invalid operands of type pointer to binary ‘operator+’"+a.toString()+"+"+b.toString());
    return createTop();
  } else if(a.isConstInt() && b.isConstInt()) {
    return a.getIntValue()+b.getIntValue();
  } else if(a.isConstFloat() && b.isConstFloat()) {
    return a.getFloatValue()+b.getFloatValue();
  } else if(a.isConstDouble() && b.isConstDouble()) {
    return a.getDoubleValue()+b.getDoubleValue();
  } else {
    if(strictChecking)
      throw CodeThorn::Exception("Error: undefined behavior in '+' operation: "+a.toString()+","+b.toString());
    return createTop();
  }
}
AbstractValue AbstractValue::operatorSub(AbstractValue& a,AbstractValue& b) {
  if(a.isTopOrUndefinedOrArbitraryMemPtrOrAbstract() || b.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
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
          SAWYER_MESG(logger[WARN])<<"Byte pointer subtraction gives value non-divisible by element size. Using top as result:"<<a.toString(_variableIdMapping)<<"-"<<b.toString(_variableIdMapping)<<endl;
          return Top();
        } else {
          val.intValue=(a.intValue-b.intValue)/pointerElementSize;
          val.valueType=AV_INTEGER;
          val.variableId=a.variableId; // same as b.variableId
        }
      } else {
        val.intValue=a.intValue-b.intValue;
        val.valueType=AV_INTEGER;
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
  } else if(a.isConstFloat() && b.isConstFloat()) {
    return a.getFloatValue()-b.getFloatValue();
  } else if(a.isConstDouble() && b.isConstDouble()) {
    return a.getDoubleValue()-b.getDoubleValue();
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
  if(a.isTopOrUndefinedOrArbitraryMemPtrOrAbstract() || b.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  else if(a.isConstInt() && b.isConstInt()) {
    return a.getIntValue()*b.getIntValue();
  } else if(a.isConstFloat() && b.isConstFloat()) {
    return a.getFloatValue()*b.getFloatValue();
  } else if(a.isConstDouble() && b.isConstDouble()) {
    return a.getDoubleValue()*b.getDoubleValue();
  }
  return createTop();
}
AbstractValue AbstractValue::operatorDiv(AbstractValue& a,AbstractValue& b) {
  if(a.isTopOrUndefinedOrArbitraryMemPtrOrAbstract() || b.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  else if(a.isConstInt() && b.isConstInt()) {
    return a.getIntValue()/b.getIntValue();
  } else if(a.isConstFloat() && b.isConstFloat()) {
    return a.getFloatValue()/b.getFloatValue();
  } else if(a.isConstDouble() && b.isConstDouble()) {
    return a.getDoubleValue()/b.getDoubleValue();
  }
  return createTop();
}

AbstractValue AbstractValue::operatorMod(AbstractValue& a,AbstractValue& b) {
  if(a.isTopOrUndefinedOrArbitraryMemPtrOrAbstract() || b.isTopOrUndefinedOrArbitraryMemPtrOrAbstract())
    return Top();
  if(a.isBot())
    return b;
  if(b.isBot())
    return a;
  // TODO modulo of pointer values
  assert(a.isConstInt() && b.isConstInt());
  return a.getIntValue()%b.getIntValue();
}

AbstractValueSet* AbstractValue::getAbstractValueSet() const {
  ROSE_ASSERT(valueType==AV_SET);
  return static_cast<AbstractValueSet*>(extension);
}

// static function, two arguments
bool AbstractValue::approximatedBy(AbstractValue val1, AbstractValue val2) {
  if(val1.isBot()||val2.isTop()) {
    // bot <= x, x <= top
    return true;
  } else if(val1.isTop() && val2.isTop()) {
    // this case is necessary because AV_TOP and AV_UNDEFINED need to be treated the same
    // and isTop also includes isUndefined (in its definition).
    return true;
  } else if(val1.isPointerToArbitraryMemory() && val2.isTopOrUndefinedOrArbitraryMemPtrOrAbstract()) {
    return true;
  } else if(val1.valueType==val2.valueType) {
    switch(val1.valueType) {
    case AV_BOT: return true;
    case AV_INTEGER: return (val1.intValue==val2.intValue);
    case AV_FP_SINGLE_PRECISION: return (val1.floatValue==val2.floatValue);
    case AV_FP_DOUBLE_PRECISION: return (val1.doubleValue==val2.doubleValue);
    case AV_PTR:
    case AV_REF: return (val1.getVariableId()==val2.getVariableId()&&val1.intValue==val2.intValue);
    case AV_FUN_PTR: return (val1.label==val2.label);
    case AV_TOP:
    case AV_UNDEFINED:
      // should be unreachable because of 2nd if-condition above
      // TODO: enforce non-reachable here
      return true;
    case AV_SET: {
      AbstractValueSet* set1=val1.getAbstractValueSet();
      AbstractValueSet* set2=val2.getAbstractValueSet();
      // (val1 approximatedBy val2) iff (val1 subsetOf val2) iff (val2 includes val1)
      return std::includes(set2->begin(),set2->end(),
                           set1->begin(),set1->end());
    }
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
    case AV_BOT: return val2;
    case AV_TOP: return val1; // special case of above if-conds (TODO: enforce not reachable)
    case AV_UNDEFINED: return val1; // special case of above if-cond (TODO: enforce not reachable)
    case AV_INTEGER: {
      if(val1.intValue==val2.intValue) {
        return val1;
      } else {
        return createTop();
      }
    }
    case AV_FP_SINGLE_PRECISION: {
      if(val1.floatValue==val2.floatValue) {
        return val1;
      } else {
        return createTop();
      }
    }
    case AV_FP_DOUBLE_PRECISION: {
      if(val1.doubleValue==val2.doubleValue) {
        return val1;
      } else {
        return createTop();
      }
    }
    case AV_PTR:
    case AV_REF: {
      if(val1.getVariableId()==val2.getVariableId()
         &&val1.getIntValue()==val2.getIntValue()) {
        return val1;
      } else if(!val1.isNullPtr()&&!val2.isNullPtr()&&val1.isPtr()&&val2.isPtr()) {
        return AbstractValue::getPointerToArbitraryMemory(); // TODO100
        //return AbstractValue::createTop();
      } else {
        if(AbstractValue::pointerSetsEnabled) {
          // promote to ptr set in case the values are not equal (handled above)
          //cout<<"DEBUG: promoto to pointer set"<<endl;
          AbstractValueSet* resultSet=new AbstractValueSet();
          //cout<<"DEBUG: result set:"<<resultSet<<endl;
          resultSet->insert(val1);
          resultSet->insert(val2);
          AbstractValue av=createAbstractValuePtrSet(resultSet);
          //cout<<"DEBUG: promot to potiner set, result set:"<<resultSet<<" elements:"<<resultSet->size()
          //    <<" av:"<<av.toString()<<endl;
          //cout<<"DEBUG: pointer set number of elems:"<<av.getAbstractValueSet()->size()<<endl;
          return av;
        } else {
          return createTop();
        }
      }
    }
    case AV_FUN_PTR: {
      if(val1.label==val2.label) {
        return val1;
      } else {
        return createTop();
      }
    }
    case AV_SET: {
      // set union
      AbstractValueSet* set1=val1.getAbstractValueSet();
      AbstractValueSet* set2=val2.getAbstractValueSet();
      AbstractValueSet* resultSet=new AbstractValueSet();
      //cout<<"DEBUG: combine: set union: new set:"<<resultSet<<endl;
#if 1
      for(AbstractValueSet::iterator i=set1->begin();i!=set1->end();++i) {
        resultSet->insert(*i);
      }
      for(AbstractValueSet::iterator i=set2->begin();i!=set2->end();++i) {
        resultSet->insert(*i);
      }
#else
      std::set_union(set1->begin(),set1->end(),set2->begin(),set2->end(),resultSet->begin());
#endif
      return createAbstractValuePtrSet(resultSet);
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
  newValue.valueType=AbstractValue::AV_UNDEFINED;
  return newValue;
}
AbstractValue AbstractValue::createBot() {
  CodeThorn::Bot bot;
  return AbstractValue(bot);
}

AbstractValue AbstractValue::createAbstractValuePtrSet(AbstractValueSet* set) {
  AbstractValue val;
  val.setAbstractValueSetPtr(set);
  return val;
}

void AbstractValue::setAbstractValueSetPtr(AbstractValueSet* avPtr) {
  //cout<<"DEBUG: setAbstractValueSetPtr:"<<avPtr<<endl;
  valueType=AV_SET;
  extension=avPtr;
  //cout<<"DEBUG: toString:"<<this->toString()<<endl;
}

void AbstractValue::addSetElement(AbstractValue av) {
  ROSE_ASSERT(valueType==AV_SET);
  AbstractValueSet* avs=getAbstractValueSet();
  ROSE_ASSERT(avs);
  avs->insert(av);
}

bool AbstractValue::isReferenceVariableAddress() {
  // TODO: remove this test, once null pointers are no longer represented as zero integers, it will then be covered by one of the other two predicates
  if(isNullPtr()) {
    return true;
  }
  if(isPtr()||isRef()) {
    return getVariableIdMapping()->isOfReferenceType(getVariableId());
  }
  return false;
}

void AbstractValue::allocateExtension(ValueType valueTypeParam) {
  valueType=valueTypeParam;
  switch(valueType) {
  case AV_SET:
    extension=new AbstractValueSet();
    //cout<<"DEBUG: ALLOC:"<<extension<<endl;
    break;
  default:
    cerr<<"Error: unsupported value type extension:"<<valueType<<endl;
    exit(1);
  }
}

void AbstractValue::deallocateExtension() {
  ROSE_ASSERT(valueType==AV_SET);
  AbstractValueSet* avs=getAbstractValueSet();
  //cout<<"DELETE: "<<this<<":"<<avs<<":"<<this->toString()<<endl;
  if(avs)
    delete avs;
}

AbstractValue AbstractValue::applyOperator(AbstractValue::Operator op, AbstractValue& v1, AbstractValue& v2) {
  switch(op) {
  case AbstractValue::Operator::Add: return operatorAdd(v1,v2);
  case AbstractValue::Operator::Sub: return operatorSub(v1,v2);
  case AbstractValue::Operator::Mul: return operatorMul(v1,v2);
  case AbstractValue::Operator::Div: return operatorDiv(v1,v2);
  case AbstractValue::Operator::Mod: return operatorMod(v1,v2);
  case AbstractValue::Operator::Or: return operatorOr(v1,v2);
  case AbstractValue::Operator::And: return operatorAnd(v1,v2);
  case AbstractValue::Operator::Eq: return operatorEq(v1,v2);
  case AbstractValue::Operator::NotEq: return operatorNotEq(v1,v2);
  case AbstractValue::Operator::Less: return operatorLess(v1,v2);
  case AbstractValue::Operator::LessOrEq: return operatorLessOrEq(v1,v2);
  case AbstractValue::Operator::MoreOrEq: return operatorMoreOrEq(v1,v2);
  case AbstractValue::Operator::More: return operatorMore(v1,v2);
  case AbstractValue::Operator::BitwiseAnd: return operatorBitwiseAnd(v1,v2);
  case AbstractValue::Operator::BitwiseOr: return operatorBitwiseOr(v1,v2);
  case AbstractValue::Operator::BitwiseXor: return operatorBitwiseXor(v1,v2);
  case AbstractValue::Operator::BitwiseShiftLeft: return operatorBitwiseShiftLeft(v1,v2);
  case AbstractValue::Operator::BitwiseShiftRight: return operatorBitwiseShiftRight(v1,v2);

    // operators with wrong arity explicitely listed
  case AbstractValue::Operator::BitwiseComplement:
  case AbstractValue::Operator::UnaryMinus:
  case AbstractValue::Operator::Not:
    throw CodeThorn::Exception("Error: AbstractValue::operator arity error.");

    // intentionally no 'default'
  };
  ROSE_ASSERT(false); // unreachable
}


bool AbstractValueSet::isEqual(AbstractValueSet& other) const {
  if(this->size()==other.size()) {
    for(auto e2 : other) {
      if(this->find(e2)==this->end())
        return false;
    }
    return true;
  } else {
    return false;
  }
}

std::string AbstractValueSet::toString(VariableIdMapping* vim) const {
  stringstream ss;
  ss<<"{";
  for(auto i=this->begin();i!=this->end();++i) {
    ss<<(*i).toString(vim)+" ";
  }
  ss<<"}";
  return ss.str();
}

AbstractValueSet& CodeThorn::operator+=(AbstractValueSet& s1, AbstractValueSet& s2) {
  for(AbstractValueSet::iterator i=s2.begin();i!=s2.end();++i) {
    s1.insert(*i);
  }
  return s1;
}
