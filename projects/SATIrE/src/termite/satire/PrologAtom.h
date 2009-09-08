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
  PrologAtom(std::string name = "#ERROR", bool escapedRepresentation = true) {
      mName = name;
      mEscapedRepresentation = escapedRepresentation;
  };
  ///the arity is always 0
  int getArity() {return 0;};
  ///an atom is always ground
  bool isGround() {return true;};
  ///return the string
  std::string getName() {return mName;};
  /// return the string
  std::string getRepresentation() {
    if (mEscapedRepresentation)
      return quote(mName);
    else
   // do not escape characters, but quote the whole string
      return "'" + mName + "'";
  }
private:
  /// the string
  std::string mName;
  /// flag indicating whether to quote the string when its representation is
  /// accessed -- in rare cases (preprocessing info) we do not want quoting
  bool mEscapedRepresentation;
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
  PrologAtom(std::string name = "#ERROR", bool escapedRepresentation = true) {
    term = PL_new_term_ref();
    PL_put_atom_chars(term, name.c_str());
#   if DEBUG_TERMITE
      std::cerr<<"PL_new_atom("<<getRepresentation()<<") = "<<term<<std::endl;
#   endif
    (void) escapedRepresentation;  // unused
  }

  ///return the string
  std::string getName() {
    char* name;
    PL_get_atom_chars(term, &name);
    return std::string(name);
  }
};

#endif

#endif
