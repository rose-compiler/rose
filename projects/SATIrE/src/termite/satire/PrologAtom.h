/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#ifndef PROLOGATOM_H_
#define PROLOGATOM_H_
#include "PrologTerm.h"

/**class representing a prolog atom*/

#if !HAVE_SWI_PROLOG

class PrologAtom : public PrologTerm {
public:
  ///the destructor
  ~PrologAtom() {};	
  ///constructor setting the string
  PrologAtom(std::string name) {
      mName = name;
  };
  ///the arity is always 0
  int getArity() {return 0;};
  ///an atom is always ground
  bool isGround() {return true;};
  ///return the string
  std::string getName() {return mName;};
  /// return the string
  std::string getRepresentation() { return quote(mName); };
private:
  /// the string
  std::string mName;
};


#else


class PrologAtom : public PrologTerm {
public:
  PrologAtom(term_t t) : PrologTerm(t) {};

  ///the destructor
  ~PrologAtom() {
    // Decrement the reference count of the atom.
    //PL_unregister(term);
  }
  ///constructor setting the string
  PrologAtom(std::string name) {
    term = PL_new_term_ref();
    PL_put_atom_chars(term, name.c_str());
#   if DEBUG_TERMITE
      std::cerr<<"PL_new_atom("<<getRepresentation()<<") = "<<term<<std::endl;
#   endif
  }
};

#endif

#endif
