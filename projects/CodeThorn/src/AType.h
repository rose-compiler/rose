#ifndef ATYPE_H
#define ATYPE_H

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

using std::string;
using std::istream;
using std::ostream;

namespace CodeThorn {

namespace AType {
 class ConstIntLattice;

 bool strictWeakOrderingIsSmaller(const ConstIntLattice& c1, const ConstIntLattice& c2);
 bool strictWeakOrderingIsEqual(const ConstIntLattice& c1, const ConstIntLattice& c2);

/*!
  \brief Implements semantic functions of an integer lattice.
  \date 2012
  \author Markus Schordan
  \details All lattice domain operators are implemented as overloaded functions.
 */
class ConstIntLattice {
 public:
  friend bool strictWeakOrderingIsSmaller(const ConstIntLattice& c1, const ConstIntLattice& c2);
  friend bool strictWeakOrderingIsEqual(const ConstIntLattice& c1, const ConstIntLattice& c2);
  enum ValueType { BOT, CONSTINT, PTR, PATHEXPR, TOP};
  ConstIntLattice();
  ConstIntLattice(bool val);
  // type conversion
  ConstIntLattice(AType::Top e);
  // type conversion
  ConstIntLattice(AType::Bot e);
  // type conversion
  ConstIntLattice(signed char x);
  ConstIntLattice(unsigned char x);
  ConstIntLattice(short int x);
  ConstIntLattice(unsigned short int x);
  ConstIntLattice(int x);
  ConstIntLattice(unsigned int x);
  ConstIntLattice(long int x);
  ConstIntLattice(unsigned long int x);
  ConstIntLattice(long long int x);
  ConstIntLattice(unsigned long long int x);
  // constructor for pointer value
  ConstIntLattice(SPRAY::VariableId,ConstIntLattice);
  bool isTop() const;
  bool isTrue() const;
  bool isFalse() const;
  bool isBot() const;
  // determines whether the value is known and constant. Otherwise it can be bot or top.
  bool isConstInt() const;
  bool isPtr() const;
  ConstIntLattice operatorNot();
  ConstIntLattice operatorUnaryMinus(); // unary minus
  ConstIntLattice operatorOr(ConstIntLattice other);
  ConstIntLattice operatorAnd(ConstIntLattice other);
  ConstIntLattice operatorEq(ConstIntLattice other) const;
  ConstIntLattice operatorNotEq(ConstIntLattice other) const;
  ConstIntLattice operatorLess(ConstIntLattice other) const;
  ConstIntLattice operatorLessOrEq(ConstIntLattice other) const;
  ConstIntLattice operatorMoreOrEq(ConstIntLattice other) const;
  ConstIntLattice operatorMore(ConstIntLattice other) const;

  ConstIntLattice operatorBitwiseAnd(ConstIntLattice other) const;
  ConstIntLattice operatorBitwiseOr(ConstIntLattice other) const;
  ConstIntLattice operatorBitwiseXor(ConstIntLattice other) const;
  ConstIntLattice operatorBitwiseComplement() const;

  static ConstIntLattice operatorAdd(ConstIntLattice& a,ConstIntLattice& b);
  static ConstIntLattice operatorSub(ConstIntLattice& a,ConstIntLattice& b);
  static ConstIntLattice operatorMul(ConstIntLattice& a,ConstIntLattice& b);
  static ConstIntLattice operatorDiv(ConstIntLattice& a,ConstIntLattice& b);
  static ConstIntLattice operatorMod(ConstIntLattice& a,ConstIntLattice& b);

  ConstIntLattice createAddressOfArray(SPRAY::VariableId arrayVariableId, ConstIntLattice Index);
  // strict weak ordering (required for sorted STL data structures if
  // no comparator is provided)
  bool operator==(AType::ConstIntLattice other) const;
  bool operator<(AType::ConstIntLattice other) const;

  string toString() const;
  friend ostream& operator<<(ostream& os, const ConstIntLattice& value);
  friend istream& operator>>(istream& os, ConstIntLattice& value);
  void fromStream(istream& is);

  ValueType getValueType() const;
  int getIntValue() const;
  SPRAY::VariableId getVariableId() const;

  int intLength();   // returns length of integer dependent on valueType
  long hash() const;
 private:
  ValueType valueType;
  int intValue;
  SPRAY::VariableId variableId;
};

// arithmetic operators
 AType::ConstIntLattice operator+(AType::ConstIntLattice& a,AType::ConstIntLattice& b);
 AType::ConstIntLattice operator-(AType::ConstIntLattice& a,AType::ConstIntLattice& b);
 AType::ConstIntLattice operator*(AType::ConstIntLattice& a,AType::ConstIntLattice& b);
 AType::ConstIntLattice operator/(AType::ConstIntLattice& a,AType::ConstIntLattice& b);
 AType::ConstIntLattice operator%(AType::ConstIntLattice& a,AType::ConstIntLattice& b);

 ostream& operator<<(ostream& os, const ConstIntLattice& value);
 istream& operator>>(istream& is, ConstIntLattice& value);

  /*! \brief Comparison class, allowing to use ConstIntLattice in ordered containers (e.g. set)
    \date 2012
    \author Markus Schordan
  */
  struct ConstIntLatticeCmp {
    bool operator()(const AType::ConstIntLattice& c1, const AType::ConstIntLattice& c2) const;
  };

} // end of namespace AType

typedef AType::ConstIntLattice AValue; 
typedef AType::ConstIntLatticeCmp AValueCmp; 

}

#endif
