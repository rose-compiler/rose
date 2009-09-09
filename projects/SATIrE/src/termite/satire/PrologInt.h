/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#ifndef PROLOGINT_H_
#define PROLOGINT_H_
#include "PrologTerm.h"

#include <sstream>

#if !HAVE_SWI_PROLOG

///class representing a prolog integer
class PrologInt : public PrologTerm {
public:
  /// constructor sets the value
  PrologInt(int value) : mValue(value) {}
  /// default constructor to make this constructible from templates
  PrologInt() : mValue(0) {}
  ///the arity is 0
  int getArity() {return 0;}
  /// an integer is always ground
  bool isGround() {return true;}
  /// return "Integer"
  std::string getName() {return "Integer";}

  /// return string representation of integer
  std::string getRepresentation() {
    std::stringstream ss;
    std::string s;
    ss << mValue;
    ss >> s;
    return s;
  }

  /// return value
  int getValue() {return mValue;}
 private:
  /// the value
  int mValue;
};

///class representing a prolog integer
class PrologFloat : public PrologTerm {
public:
  /// constructor sets the value
  PrologFloat(double value) : mValue(value) {}
  ///the arity is 0
  int getArity() {return 0;}
  /// an integer is always ground
  bool isGround() {return true;}
  /// return "Integer"
  std::string getName() {return "Float";}

  /// return string representation of integer
  std::string getRepresentation() {
    std::stringstream ss;
    std::string s;
    ss << mValue;
    ss >> s;
    return s;
  }

  /// return value
  double getValue() {return mValue;}
 private:
  /// the value
  double mValue;
};

#else

///class representing a prolog integer
class PrologInt : public PrologTerm {
public:
  // CAREFUL: we can't use the default "copy constructor" 
  // because term_t and int64_t are equivalent
  PrologInt() : PrologTerm() {};
  void createFromTerm(term_t t) { term = PL_copy_term_ref(t); }

  /// constructor sets the value
  PrologInt(int64_t value) { 
    //std::stringstream s;
    //s << value;
    //PL_put_atom_chars(term, s.str().c_str());
    //#   if DEBUG_TERMITE
    //  std::cerr<<"PrologInt: PL_new_atom("<<s.str()<<") = "<<term<<std::endl;
    //#   endif

    term = PL_new_term_ref();
    PL_put_int64(term, value);

#   if DEBUG_TERMITE
      std::cerr<<"PrologInt: "<<value<<") = "<<term<<std::endl;
#   endif
  }
  ///the arity is 0
  int getArity() { return 0; }
  /// an integer is always ground
  bool isGround() { return true; }
  /// return "Integer"
  std::string getName() { return "Integer"; }

  /// return value
  int64_t getValue() {
    int64_t i;
    assert(PL_get_int64(term, &i));
    return i;
  }
};

///class representing a prolog float
class PrologFloat : public PrologTerm {
public:
  // CAREFUL: we can't use the default "copy constructor" 
  // because term_t and int64_t are equivalent
  PrologFloat() : PrologTerm() {};
  void createFromTerm(term_t t) { term = PL_copy_term_ref(t); }

  /// constructor sets the value
  PrologFloat(double value) { 
    //std::stringstream s;
    //s << value;
    //PL_put_atom_chars(term, s.str().c_str());
    //#   if DEBUG_TERMITE
    //  std::cerr<<"PrologInt: PL_new_atom("<<s.str()<<") = "<<term<<std::endl;
    //#   endif

    term = PL_new_term_ref();
    PL_put_float(term, value);

#   if DEBUG_TERMITE
      std::cerr<<"PrologFloat: "<<value<<") = "<<term<<std::endl;
#   endif
  }
  ///the arity is 0
  int getArity() { return 0; }
  /// an integer is always ground
  bool isGround() { return true; }
  /// return "Integer"
  std::string getName() { return "Float"; }

  /// return value
  double getValue() {
    double i;
    assert(PL_get_float(term, &i));
    return i;
  }
};

#endif

#endif
