#ifndef ATYPE_H
#define ATYPE_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <limits.h>
#include <string>
#include <sstream>

using namespace std;

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
  \author Markus Schordan
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
  bool isTop();
  bool isTrue();
  bool isFalse();
  bool isBot();
  BoolLattice operator!();
  BoolLattice operator||(BoolLattice other);
  BoolLattice operator&&(BoolLattice other);
  // operator= : C++ default used
  // operator== : C++ default used
  string toString();
  friend ostream& operator<<(ostream& os, const BoolLattice& toStream);
 private:
  ValueType value;
};

 ostream& operator<<(ostream& os, BoolLattice& toStream);

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
  ConstIntLattice(Top e);
  // type conversion
  ConstIntLattice(Bot e);
  // type conversion
  ConstIntLattice(int x);
  bool isTop() const;
  bool isTrue() const;
  bool isFalse() const;
  bool isBot() const;
  bool isConstInt() const;
  //  bool strictWeakOrderingIsSmaller(const ConstIntLattice& c2) const;
  //bool strictWeakOrderingIsEqual(const ConstIntLattice& c2) const;
  ConstIntLattice operator!();
  ConstIntLattice operator-(); // unary minus
  ConstIntLattice operator||(ConstIntLattice other);
  ConstIntLattice operator&&(ConstIntLattice other);
  ConstIntLattice operator==(ConstIntLattice other) const;
  ConstIntLattice operator!=(ConstIntLattice other);
  //ConstIntLattice operator<(ConstIntLattice other);
  string toString() const;
  friend ostream& operator<<(ostream& os, const ConstIntLattice& toStream);
  ValueType getValueType() const;
  int getIntValue() const;
 private:
  int intValue;
  ValueType valueType;
};

 ostream& operator<<(ostream& os, ConstIntLattice& toStream);

  /*! \brief Comparison class, allowing to use ConstIntLattice in ordered containers (e.g. set)
	\date 2012
	\author Markus Schordan
  */
  struct ConstIntLatticeCmp {
	bool operator()(const AType::ConstIntLattice& c1, const AType::ConstIntLattice& c2) const;
  };


} // end of namespace AType

#endif
