/* -*- C++ -*-
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
          2008 Adrian Prantl (adrian@complang.tuwien.ac.at)
see LICENSE in the root folder of this project
*/
#ifndef PROLOGATOM_H_
#define PROLOGATOM_H_
#include "PrologTerm.h"

/**class representing a prolog atom*/
class PrologAtom : public PrologTerm {
public:
  ///the destructor
  ~PrologAtom() {};	
  ///constructor setting the string
  PrologAtom(std::string name) {
#if 0
    // FIXME: qualified names should be deactivated anyway
    // toProlog (ok) vs. main.C (err)
    if ((name.length() > 2) &&
	(name[0] == name[1] == ':'))
      mName = name.substr(2, name.length());
    else 
#endif
      mName = name;
  };
  ///the arity is always 0
  int getArity() {return 0;};
  ///an atom is always ground
  bool isGround() {return true;};
  ///return the string
  std::string getName() {return mName;};
  /// return the string
  std::string getRepresentation() { return quote(mName);
  };
private:
  /// the string
  std::string mName;
};
#endif
