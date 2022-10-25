#ifndef NUMBERINTERVALLATTICE_H
#define NUMBERINTERVALLATTICE_H

#include <string>
#include <sstream>
#include <iostream>
#include "sage3basic.h"
#include "GenericIntervalLattice.h"

namespace CodeThorn {

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
    void operator+=(const Number& other);
    void operator-=(const Number& other);
    void operator*=(const Number& other);
    void operator/=(const Number& other);
    void operator%=(const Number& other);
    void operator<<=(const Number& other);
    void operator>>=(const Number& other);
    std::string toString() const;
    // TODO: artithmetic operators
  private:
    int _num;
  };
  
  Number operator+(const Number& n1, const Number& n2);
  Number operator-(const Number& n1, const Number& n2);
  Number operator*(const Number& n1, const Number& n2);
  Number operator/(const Number& n1, const Number& n2);
  Number operator%(const Number& n1, const Number& n2);
  Number operator<<(const Number& n1, const Number& n2);
  Number operator>>(const Number& n1, const Number& n2);

  std::ostream& operator<< (std::ostream &out, Number &number);
 
  class NumberIntervalLattice : public CodeThorn::GenericIntervalLattice<Number> {
  public:
    NumberIntervalLattice();
    NumberIntervalLattice(Number value);
    NumberIntervalLattice(Number left, Number right);
    // required for the conversion of the return-type of some GenericIntervalLattice functions
    NumberIntervalLattice(GenericIntervalLattice<Number>);
  private:
  };
  
}

#endif
