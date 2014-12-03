#ifndef NUMBERINTERVALLATTICE_H
#define NUMBERINTERVALLATTICE_H

#include <string>
#include <sstream>
#include <iostream>
#include "GenericIntervalLattice.h"

class Number {
 public:
  Number();
  // TODO: extend for other number types
  Number(int num);
  int getInt();
  bool operator==(const Number& other) const;
  bool operator!=(const Number& other) const;
  bool operator>(const Number& other) const;
  bool operator>=(const Number& other) const;
  bool operator<(const Number& other) const;
  bool operator<=(const Number& other) const;
  std::string toString() const;
  // TODO: artithmetic operators
 private:
  int _num;
};

std::ostream& operator<< (std::ostream &out, Number &number);

class NumberIntervalLattice : public GenericIntervalLattice<Number> {
 public:
  NumberIntervalLattice();
  NumberIntervalLattice(Number left, Number right);
  // required for the conversion of the return-type of some GenericIntervalLattice functions
  NumberIntervalLattice(GenericIntervalLattice<Number>);
 private:
};

#endif
