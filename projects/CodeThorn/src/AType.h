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

using std::string;
using std::istream;
using std::ostream;

namespace CodeThorn {

/*!
  \brief Several abstract types implementing abstract semantics.
  \details These types are used by the analyzer for computing abstract values according the semantics of the respective domain.
  \date 2012
  \author Markus Schordan
 */

namespace AType {

  /*!
    \brief Serves as top value in analysis.
    \date 2012
    \author Markus Schordan
  */
  class Top {
  };

/*!
  \brief Serves as bot value in analysis.
  \date 2012
  \author Markus Schordan
 */
  class Bot {
  };

/*!
  \brief Implements semantic functions of a boolean lattice.
  \date 2012
  \author Markus Schordan, Adrian Prantl
  \details All lattice domain operators are implemented as overloaded functions.
 */
class BoolLattice {
 public:
  enum ValueType { FALSE, TRUE, BOT, TOP};
  BoolLattice();
  BoolLattice(bool val); // type conversion
  BoolLattice(Top e); // type conversion
  BoolLattice(Bot e); // type conversion
  BoolLattice(int x); // type conversion
  bool isTop() const;
  bool isTrue() const;
  bool isFalse() const;
  bool isBot() const;
  BoolLattice operator!();
  BoolLattice operator||(BoolLattice other);
  BoolLattice operator&&(BoolLattice other);
  /// least upper bound
  BoolLattice lub(BoolLattice other);
  /// greatest lower bound
  BoolLattice glb(BoolLattice other);
  /// operator= : C++ default used
  bool operator==(BoolLattice other) const;
  bool operator!=(BoolLattice other) const;
  /// for sorting only!
  bool operator<(BoolLattice other) const;
  string toString() const;
  inline ValueType val() const { return value; }
  friend ostream& operator<<(ostream& os, const BoolLattice& value);
 private:
  ValueType value;
};

 ostream& operator<<(ostream& os, const BoolLattice& value);

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
  enum ValueType { BOT, CONSTINT, TOP};
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
  ConstIntLattice(int x);
  ConstIntLattice(long int x);
  ConstIntLattice(long long int x);
  ConstIntLattice(unsigned short int x);
  ConstIntLattice(unsigned int x);
  ConstIntLattice(unsigned long int x);
  ConstIntLattice(unsigned long long int x);
  bool isTop() const;
  bool isTrue() const;
  bool isFalse() const;
  bool isBot() const;
  // determines whether the value is known and constant. Otherwise it can be bot or top.
  bool isConstInt() const;
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
  long hash() const;
  static bool arithTop;
  // returns length of integer dependent on valueType
  int intLength();
 private:
  ValueType valueType;
  int intValue;
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

#if 1
 class CppCapsuleConstIntLattice {
 public:
   CppCapsuleConstIntLattice(){}
   CppCapsuleConstIntLattice(ConstIntLattice val):value(val){}
   CppCapsuleConstIntLattice(Top top){value=ConstIntLattice(top);}
   bool operator==(CppCapsuleConstIntLattice other) const;
   bool operator<(CppCapsuleConstIntLattice other) const;
   string toString() const { return value.toString(); }
   ConstIntLattice getValue() const { return value; }
   void setValue(ConstIntLattice val) { value=val; }
 private:
   ConstIntLattice value;
 };
  struct CppCapsuleConstIntLatticeLessComparator {
    bool operator()(const AType::CppCapsuleConstIntLattice& c1, const AType::CppCapsuleConstIntLattice& c2) const;
  };
#endif

} // end of namespace AType

typedef AType::ConstIntLattice AValue; 
typedef AType::ConstIntLatticeCmp AValueCmp; 

typedef AType::CppCapsuleConstIntLattice CppCapsuleAValue; 
typedef AType::CppCapsuleConstIntLatticeLessComparator CppCapsuleAValueLessComp; 


}

#endif
