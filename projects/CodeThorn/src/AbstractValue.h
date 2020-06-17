#ifndef ABSTRACT_VALUE_H
#define ABSTRACT_VALUE_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
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
  enum ValueType { BOT, INTEGER, FLOAT, PTR, REF, FUN_PTR, TOP, UNDEFINED };
  AbstractValue();
  AbstractValue(bool val);
  // type conversion
  AbstractValue(CodeThorn::Top e);
  // type conversion
  AbstractValue(CodeThorn::Bot e);
  // type conversion
  AbstractValue(Label lab); // for modelling function addresses
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
  void initInteger(CodeThorn::BuiltInType btype, long long int ival);
  void initFloat(CodeThorn::BuiltInType btype, double fval);
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
  // currently identical to isPtr() but already used where one unique value is required
  bool isConstPtr() const;
  bool isPtr() const;
  bool isFunctionPtr() const;
  bool isRef() const;
  bool isNullPtr() const;
  AbstractValue operatorNot();
  AbstractValue operatorUnaryMinus(); // unary minus
  AbstractValue operatorOr(AbstractValue other);
  AbstractValue operatorAnd(AbstractValue other);
  AbstractValue operatorEq(AbstractValue other) const;
  AbstractValue operatorNotEq(AbstractValue other) const;
  AbstractValue operatorLess(AbstractValue other) const;
  AbstractValue operatorLessOrEq(AbstractValue other) const;
  AbstractValue operatorMoreOrEq(AbstractValue other) const;
  AbstractValue operatorMore(AbstractValue other) const;

  AbstractValue operatorBitwiseAnd(AbstractValue other) const;
  AbstractValue operatorBitwiseOr(AbstractValue other) const;
  AbstractValue operatorBitwiseXor(AbstractValue other) const;
  AbstractValue operatorBitwiseComplement() const;

  AbstractValue operatorBitwiseShiftLeft(AbstractValue other) const;
  AbstractValue operatorBitwiseShiftRight(AbstractValue other) const;

  static AbstractValue operatorAdd(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorSub(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorMul(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorDiv(AbstractValue& a,AbstractValue& b);
  static AbstractValue operatorMod(AbstractValue& a,AbstractValue& b);

  static AbstractValue createAddressOfVariable(CodeThorn::VariableId varId);
  static AbstractValue createAddressOfArray(CodeThorn::VariableId arrayVariableId);
  static AbstractValue createAddressOfArrayElement(CodeThorn::VariableId arrayVariableId, AbstractValue Index);
  static AbstractValue createAddressOfFunction(CodeThorn::Label lab);
  static AbstractValue createNullPtr();
  static AbstractValue createUndefined(); // used to model values of uninitialized variables/memory locations
  static AbstractValue createTop();
  static AbstractValue createBot();
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

  ValueType getValueType() const;
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
  void setValue(long long int ival);
  void setValue(double fval);
  Label getLabel() const;
  long hash() const;
  std::string valueTypeToString() const;

  CodeThorn::TypeSize getTypeSize() const;
  void setTypeSize(CodeThorn::TypeSize valueSize);
  static void setVariableIdMapping(CodeThorn::VariableIdMappingExtended* varIdMapping);
  static CodeThorn::VariableIdMappingExtended* getVariableIdMapping();
  static bool approximatedBy(AbstractValue val1, AbstractValue val2);
  static AbstractValue combine(AbstractValue val1, AbstractValue val2);
  static bool strictChecking; // if turned off, some error conditions are not active, but the result remains sound.
 private:
  AbstractValue topOrError(std::string) const;
  ValueType valueType;
  CodeThorn::VariableId variableId;
  union {
    long long int intValue=0;
    double floatValue;
  };
  Label label;
  CodeThorn::TypeSize typeSize=0;
  static CodeThorn::VariableIdMappingExtended* _variableIdMapping;
};

// arithmetic operators
 AbstractValue operator+(AbstractValue& a,AbstractValue& b);
 AbstractValue operator-(AbstractValue& a,AbstractValue& b);
 AbstractValue operator*(AbstractValue& a,AbstractValue& b);
 AbstractValue operator/(AbstractValue& a,AbstractValue& b);
 AbstractValue operator%(AbstractValue& a,AbstractValue& b);

 ostream& operator<<(ostream& os, const AbstractValue& value);
 istream& operator>>(istream& is, AbstractValue& value);

  /*! \brief Comparison class, allowing to use AbstractValue in ordered containers (e.g. set)
    \date 2012
    \author Markus Schordan
  */
  struct AbstractValueCmp {
    bool operator()(const AbstractValue& c1, const AbstractValue& c2) const;
  };

  typedef std::set<AbstractValue> AbstractValueSet;
  AbstractValueSet& operator+=(AbstractValueSet& s1, AbstractValueSet& s2);
}

#endif
