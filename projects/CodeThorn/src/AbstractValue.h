#ifndef ABSTRACT_VALUE_H
#define ABSTRACT_VALUE_H

/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <climits>
#include <string>
#include <sstream>
#include "BoolLattice.h"
#include "VariableIdMapping.h"
#include <cstdint>
#include "VariableIdMappingExtended.h"
#include "TypeSizeMapping.h"
#include "Labeler.h"

using std::string;
using std::istream;
using std::ostream;

namespace CodeThorn {
  
  class AbstractValue;
  class AbstractValueSet;
  
  bool strictWeakOrderingIsSmaller(const AbstractValue& c1, const AbstractValue& c2);
  bool strictWeakOrderingIsEqual(const AbstractValue& c1, const AbstractValue& c2);

/*!
  \brief Implements semantic functions of an integer lattice.
  \date 2012
  \author Markus Schordan
  \details All lattice domain operators are implemented as overloaded functions.
 */
class AbstractValue {
 public:
  friend bool strictWeakOrderingIsSmaller(const AbstractValue& c1, const AbstractValue& c2);
  friend bool strictWeakOrderingIsEqual(const AbstractValue& c1, const AbstractValue& c2);
  AbstractValue();
  AbstractValue(bool val);
  // type conversion
  AbstractValue(CodeThorn::Top e);
  // type conversion
  AbstractValue(CodeThorn::Bot e);
  // type conversion
  AbstractValue(Label lab); // for modelling function addresses
  // copy constructor
  AbstractValue(const AbstractValue& other);
  // assignment op
  AbstractValue& operator=(AbstractValue other);
 
  AbstractValue(signed char x);
  AbstractValue(unsigned char x);
  AbstractValue(short int x);
  AbstractValue(unsigned short int x);
  AbstractValue(int x);
  AbstractValue(unsigned int x);
  AbstractValue(long int x);
  AbstractValue(unsigned long int x);
  AbstractValue(long long int x);
  AbstractValue(unsigned long long int x);
  AbstractValue(float x);
  AbstractValue(double x);
  //using in a union causes gcc warning because of backward incompatibility with gcc 4.4 (in 7.4)
  // -Wno-psabi allows to turn this off
  //AbstractValue(long double x);
  AbstractValue(CodeThorn::VariableId varId); // allows implicit type conversion
  ~AbstractValue(); // also deallocates extensions
  void initInteger(CodeThorn::BuiltInType btype, long int ival);
  void initFloat(CodeThorn::BuiltInType btype, float fval);
  void initDouble(CodeThorn::BuiltInType btype, double fval);
  static AbstractValue createIntegerValue(CodeThorn::BuiltInType btype, long long int ival);
  CodeThorn::TypeSize calculateTypeSize(CodeThorn::BuiltInType btype);
  // currently this maps to isTop() - in preparation to handle
  // uninitilized explicitly. A declaration (without initializer)
  // should use this function to model the semantics of an undefined value.
  bool isUndefined() const; 
  bool isTop() const;
  bool isTrue() const;
  bool isFalse() const;
  bool isBot() const;
  // determines whether the value is known and constant. Otherwise it can be bot or top.
  bool isConstInt() const;
  bool isConstFloat() const;
  bool isConstDouble() const;
  // currently identical to isPtr() but already used where one unique value is required
  bool isConstPtr() const;
  bool isPtr() const;
  // deprecated
  bool isPtrSet() const;
  bool isAVSet() const;
  bool isFunctionPtr() const;
  bool isRef() const;
  bool isNullPtr() const;
  // if the type is AV_SET it determines whether the set contains a null pointer (zero)
  bool ptrSetContainsNullPtr() const;
  // deprecated
  size_t getPtrSetSize() const;
  size_t getAVSetSize() const;
  enum Operator { UnaryMinus, Not, BitwiseComplement, Add, Sub, Mul, Div, Mod, Or, And, Eq, NotEq, Less, LessOrEq, More, MoreOrEq, BitwiseAnd, BitwiseOr, BitwiseXor, BitwiseShiftLeft, BitwiseShiftRight};

  //static int32_t arrayAbstractionIndex;
  static AbstractValue applyOperator(AbstractValue::Operator op, AbstractValue& v1, AbstractValue& v2);

  static AbstractValue operatorAdd(AbstractValue& a,AbstractValue& b, AbstractValue elemSize);
  static AbstractValue operatorAdd(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorSub(AbstractValue& a,AbstractValue& b, AbstractValue elemSize);
  static AbstractValue operatorSub(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorMul(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorDiv(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorMod(AbstractValue& a,AbstractValue& b);

  static AbstractValue operatorOr(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorAnd(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorEq(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorNotEq(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorLess(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorLessOrEq(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorMoreOrEq(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorMore(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorBitwiseAnd(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorBitwiseOr(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorBitwiseXor(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorBitwiseShiftLeft(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorBitwiseShiftRight(AbstractValue& a,AbstractValue& b);

  static AbstractValue operatorNot(AbstractValue& a);
  static AbstractValue operatorUnaryMinus(AbstractValue& a);
  static AbstractValue operatorBitwiseComplement(AbstractValue& a);

  AbstractValue operatorNot();
  AbstractValue operatorUnaryMinus(); // unary minus
  AbstractValue operatorBitwiseComplement() const;

  AbstractValue operatorOr(AbstractValue other) const;
  AbstractValue operatorAnd(AbstractValue other) const;
  AbstractValue operatorEq(AbstractValue other) const;
  AbstractValue operatorNotEq(AbstractValue other) const;
  AbstractValue operatorLess(AbstractValue other) const;
  AbstractValue operatorLessOrEq(AbstractValue other) const;
  AbstractValue operatorMoreOrEq(AbstractValue other) const;
  AbstractValue operatorMore(AbstractValue other) const;

  AbstractValue operatorBitwiseAnd(AbstractValue other) const;
  AbstractValue operatorBitwiseOr(AbstractValue other) const;
  AbstractValue operatorBitwiseXor(AbstractValue other) const;

  AbstractValue operatorBitwiseShiftLeft(AbstractValue other) const;
  AbstractValue operatorBitwiseShiftRight(AbstractValue other) const;

  static AbstractValue createAddressOfVariable(CodeThorn::VariableId varId);
  static AbstractValue createAddressOfArray(CodeThorn::VariableId arrayVariableId);
  static AbstractValue createAddressOfArrayElement(CodeThorn::VariableId arrayVariableId, AbstractValue Index);
  static AbstractValue createAddressOfArrayElement(CodeThorn::VariableId arrayVariableId, AbstractValue Index, AbstractValue elementSize);
  static AbstractValue createAddressOfFunction(CodeThorn::Label lab);
  static AbstractValue createNullPtr();
  static AbstractValue createUndefined(); // used to model values of uninitialized variables/memory locations
  static AbstractValue createTop();
  static AbstractValue createBot();
  static AbstractValue createAbstractValuePtrSet(AbstractValueSet*);
  void setAbstractValueSetPtr(AbstractValueSet* avPtr);
  AbstractValueSet* getAbstractValueSet() const;

  // strict weak ordering (required for sorted STL data structures if
  // no comparator is provided)
  //  bool operator==(AbstractValue other) const;
  //bool operator!=(AbstractValue other) const;
  bool operator==(const AbstractValue other) const;
  bool operator!=(const AbstractValue other) const;
  bool operator<(AbstractValue other) const;

  bool isReferenceVariableAddress();

  string toString() const;
  string toString(CodeThorn::VariableIdMapping* vim) const;
  string toLhsString(CodeThorn::VariableIdMapping* vim) const;
  string toRhsString(CodeThorn::VariableIdMapping* vim) const;
  string arrayVariableNameToString(CodeThorn::VariableIdMapping* vim) const;
  
  friend ostream& operator<<(ostream& os, const AbstractValue& value);
  friend istream& operator>>(istream& os, AbstractValue& value);
  void fromStream(istream& is);

    int getIntValue() const;
  long int getLongIntValue() const;
  float getFloatValue() const;
  double getDoubleValue() const;
  //long double getLongDoubleValue() const;
  std::string getFloatValueString() const;

  // returns index value if it is an integer
  int getIndexIntValue() const;
  // returns index value (can be top)
  AbstractValue getIndexValue() const;
  CodeThorn::VariableId getVariableId() const;
  // sets value according to type size (truncates if necessary)
  void setValue(long int ival);
  void setValue(float fval);
  void setValue(double fval);
  Label getLabel() const;
  size_t hash() const;
  std::string valueTypeToString() const;

  //CodeThorn::TypeSize getTypeSize() const;
  //void setTypeSize(CodeThorn::TypeSize valueSize);
  //CodeThorn::TypeSize getElementTypeSize() const;
  //void setElementTypeSize(CodeThorn::TypeSize valueSize);

  static void setVariableIdMapping(CodeThorn::VariableIdMappingExtended* varIdMapping);
  static CodeThorn::VariableIdMappingExtended* getVariableIdMapping();
  static bool approximatedBy(AbstractValue val1, AbstractValue val2);
  static AbstractValue combine(AbstractValue val1, AbstractValue val2);
  static bool strictChecking; // if turned off, some error conditions are not active, but the result remains sound.
  static AbstractValue convertPtrToPtrSet(AbstractValue val); // requires val to be AV_PTR
  static AbstractValue conditionallyApplyArrayAbstraction(AbstractValue val);
  bool isAbstract() const;
  // forces abstract value to be handled as a summary
  void setAbstractFlag(bool flag); 

private:

  /* the following are extensions that allocate more memory than a single abstract value
     they can only be created through merging abstract value
     - AV_SET allows to represent a set of values pointers.
     - INTERVAL represents an interval of abstract number values // TODO
     - INDEX_RANGE represents a range of a consecutive memory region (e.g. array) // TODO
  */
  enum ValueType { AV_BOT, AV_TOP, AV_UNDEFINED, AV_INTEGER, AV_FP_SINGLE_PRECISION, AV_FP_DOUBLE_PRECISION, AV_PTR, AV_REF, AV_FUN_PTR, AV_SET, /*INTERVAL, INDEX_RANGE*/ };
  ValueType getValueType() const;

  // functions used for (de)allocating additional memory for some abstractions
  AbstractValueSet* abstractValueSetCopy() const;
  void copy(const AbstractValue& other);
  void allocateExtension(ValueType);
  void deallocateExtension();
  void addSetElement(AbstractValue av); // requires this to be AV_SET, adds av to AV_SET
  void setExtension(void*);
  AbstractValue topOrError(std::string) const;
  ValueType valueType;
  CodeThorn::VariableId variableId;
  union {
    long intValue;
    float floatValue;
    double doubleValue;
    void* extension=nullptr; // used for AVSet
  };
  Label label;
  bool _abstractionFlag=false;
public:
  static CodeThorn::VariableIdMappingExtended* _variableIdMapping;
  static bool byteMode; // computes byte offset for array and struct elements
  static bool pointerSetsEnabled;
};

 ostream& operator<<(ostream& os, const AbstractValue& value);
 istream& operator>>(istream& is, AbstractValue& value);

  /*! \brief Comparison class, allowing to use AbstractValue in ordered containers (e.g. set)
    \date 2012
    \author Markus Schordan
  */
  struct AbstractValueCmp {
    bool operator()(const AbstractValue& c1, const AbstractValue& c2) const;
  };

  //typedef std::set<AbstractValue> AbstractValueSet;
  class AbstractValueSet : public std::set<AbstractValue> {
  public:
    bool isEqual(AbstractValueSet& other) const;
    std::string toString(VariableIdMapping* vim) const;
  };
  AbstractValueSet& operator+=(AbstractValueSet& s1, AbstractValueSet& s2);

}

#endif
