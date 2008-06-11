/* -*- C++ -*-
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
          2008 Adrian Prantl (adrian@complang.tuwien.ac.at)
see LICENSE in the root folder of this project
*/
#ifndef PROLOGINT_H_
#define PROLOGINT_H_
#include "PrologTerm.h"

#include <sstream>

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
  std::string getRepresentation() {
    std::stringstream ss;
    std::string s;
    ss << mValue;
    ss >> s;
    return s;
  };

  /// return value
  int getValue() {return mValue;};
 private:
  /// the value
  int mValue;
};

class PrologUnsignedInt : public PrologTerm {
public:
  /// constructor sets the value
  PrologUnsignedInt(unsigned int value) : mValue(value) {};
  ///the arity is 0
  int getArity() {return 0;};
  /// an integer is always ground
  bool isGround() {return true;};
  /// return "Integer"
  std::string getName() {return "Integer";};

  /// return string representation of integer
  std::string getRepresentation() {
    std::stringstream ss;
    std::string s;
    ss << mValue;
    ss >> s;
    return s;
  };

  /// return value
  int getValue() {return mValue;};
private:
  /// the value
  unsigned mValue;
};

#endif
