#include "NumberIntervalLattice.h"
#include <iostream>

Number::Number():_num(0){}
Number::Number(int num):_num(num) {}
int Number::getInt() { return _num; }
bool Number::operator==(const Number& other) const {
  return _num==other._num;
}
bool Number::operator!=(const Number& other) const {
  return !(_num==other._num);
}
bool Number::operator<(const Number& other) const {
  return _num<other._num;
}
bool Number::operator<=(const Number& other) const {
  return _num<other._num||_num==other._num;
}
bool Number::operator>(const Number& other) const {
  return !(_num<=other._num);
}
bool Number::operator>=(const Number& other) const {
  return !(_num<other._num);
}
std::string Number::toString() const {
  std::stringstream ss;
  ss<<_num;
    return ss.str();
}

std::ostream& operator<<(std::ostream& out, Number& number) {
  out<<number.toString();
  return out;
}

NumberIntervalLattice::NumberIntervalLattice(){}

NumberIntervalLattice::NumberIntervalLattice(Number left, Number right) {
  setLow(left);
  setHigh(right);
}

NumberIntervalLattice::NumberIntervalLattice(GenericIntervalLattice<Number> interval) {
  if(interval.isLowInf()) {
    setLowInf();
  } else {
    setLow(interval.getLow());
  }
  if(interval.isHighInf()) {
    setHighInf();
  } else {
    setHigh(interval.getHigh());
  }
}
