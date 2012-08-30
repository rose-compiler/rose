#ifndef ATYPE_H
#define ATYPE_H

#include <limits.h>
#include <string>
#include <sstream>

using namespace std;

namespace AType {

  class Top {
  };

class Bot {
};

class BoolLattice {
 public:
  enum ValueType { FALSE, TRUE, BOT, TOP};
  BoolLattice();
  BoolLattice(bool val);
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

class ConstIntLattice {
 public:
  enum ValueType { BOT, TOP, CONSTINT, TRUE,FALSE};
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
  ConstIntLattice operator!();
  ConstIntLattice operator-(); // unary minus
  ConstIntLattice operator||(ConstIntLattice other);
  ConstIntLattice operator&&(ConstIntLattice other);
  ConstIntLattice operator==(ConstIntLattice other);
  ConstIntLattice operator!=(ConstIntLattice other);
  //ConstIntLattice operator<(ConstIntLattice other);
  string toString();
  friend ostream& operator<<(ostream& os, const ConstIntLattice& toStream);
  ValueType getValueType();
  int getIntValue() const;
 private:
  int intValue;
  ValueType valueType;
};

 ostream& operator<<(ostream& os, ConstIntLattice& toStream);

} // end of namespace AType

#endif
