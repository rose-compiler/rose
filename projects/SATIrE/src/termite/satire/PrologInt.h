/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#ifndef PROLOGINT_H_
#define PROLOGINT_H_
#include "PrologTerm.h"
using namespace std;
///class representing a prolog integer
class PrologInt : public PrologTerm {
 public:
	 /// constructor sets the value
  PrologInt(int value) : mValue(value) {};
  ///the arity is 0
  int getArity() {return 0;};
  /// an integer is always ground
  bool isGround() {return true;};
  /// return "Integer"
  std::string getName() {return "Integer";};
  /// return string representation of integer
  std::string getRepresentation();
  /// return value
  int getValue() {return mValue;};
 private:
  /// the value
  int mValue;
};
#endif
