/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#ifndef PROLOGTERM_H_
#define PROLOGTERM_H_
#include <vector>
#include <string>
#include <iostream>
#include <ctype.h>
using namespace std;
/// Representation of a prolog term
class PrologTerm {
 public:
  virtual ~PrologTerm() {};
  /// returns the arity of the term
  virtual int getArity() = 0;
  /// returns wether or not the term is a ground term, i.e. contains no variables.
  /** Note that this is the case iff all the subterms are ground. */
  virtual bool isGround() = 0;
  /// Gets the name (functor/variable/constant) of the term. 
  /* numbers are represented as strings and therefore returned as such */
  virtual std::string getName() = 0;
  /// the actual prolog term that is represented by this object
  virtual std::string getRepresentation() = 0;

  /// Properly quote an atom if necessary
  static std::string quote(const std::string atom) {
    if (atom.length() == 0) return "''";
    if (((atom.length() > 0) && (!islower(atom[0])) && (!isdigit(atom[0])))
	|| (atom.find(' ') < atom.length())) {
      std::string s;
      s = "'" + atom  + "'";
      return s;
    }
    return atom;
  }
};


#endif
