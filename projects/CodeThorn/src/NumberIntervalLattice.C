#include "NumberIntervalLattice.h"
#include <iostream>

using namespace SPRAY;

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
void Number::operator+=(const Number& other) {
  _num+=other._num;
}
void Number::operator-=(const Number& other) {
  _num-=other._num;
}
void Number::operator*=(const Number& other) {
  _num*=other._num;
}
void Number::operator/=(const Number& other) {
  _num/=other._num;
}
void Number::operator%=(const Number& other) {
  _num%=other._num;
}
void Number::operator<<=(const Number& other) {
  _num<<=other._num;
}
void Number::operator>>=(const Number& other) {
  _num>>=other._num;
}
std::string Number::toString() const {
  std::stringstream ss;
  ss<<_num;
    return ss.str();
}

Number SPRAY::operator+(const Number& n1, const Number& n2) {
  Number n=n1;
    n+=n2;
    return n;
}
Number SPRAY::operator-(const Number& n1, const Number& n2) {
  Number n=n1;
  n-=n2;
  return n;
}
Number SPRAY::operator*(const Number& n1, const Number& n2) {
  Number n=n1;
  n*=n2;
  return n;
}
Number SPRAY::operator/(const Number& n1, const Number& n2) {
  Number n=n1;
  n/=n2;
  return n;
}
Number SPRAY::operator%(const Number& n1, const Number& n2) {
  Number n=n1;
  n%=n2;
  return n;
}
Number SPRAY::operator<<(const Number& n1, const Number& n2) {
    Number n=n1;
    n<<=n2;
    return n;
}
Number SPRAY::operator>>(const Number& n1, const Number& n2) {
  Number n=n1;
  n>>=n2;
  return n;
}

std::ostream& SPRAY::operator<<(std::ostream& out, Number& number) {
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
