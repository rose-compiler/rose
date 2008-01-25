/* -*- c++ -*-
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
          2007 Adrian Prantl
see LICENSE in the root folder of this project
*/
#ifndef PROLOGCOMPTERM_H_
#define PROLOGCOMPTERM_H_
#include "PrologTerm.h"
#include <assert.h>

/// Representation of a composite prolog term .
class PrologCompTerm : public PrologTerm {
public:
  ~PrologCompTerm() {};
  /// Creates a composite term with the given name. no subterms added yet.
  
  PrologCompTerm(std::string name) : mName(name) {};
  int getArity();
  bool isGround();
  std::string getName() { return mName; };
  std::string getRepresentation();
  std::vector<PrologTerm *> getSubTerms() { return mSubterms; };


  /// Add a subterm at the first position
  void addFirstSubTerm(PrologTerm* t) {mSubterms.insert(mSubterms.begin(),t);};

  /// Add a subterm at the last position
  void addSubterm(PrologTerm* t) {mSubterms.push_back(t);};

  /// the i-th subterm
  PrologTerm* at(int i) {return mSubterms.at(i);};
protected:
  /// the name of the terms
  std::string mName;
  /// list of the subterms
  std::vector<PrologTerm *> mSubterms;
};


class PrologInfixOperator : public PrologCompTerm {
public:
  ~PrologInfixOperator() {};
  /// Creates a composite term with the given name. no subterms added yet.
  PrologInfixOperator(std::string name) : PrologCompTerm(name) {};
  std::string getRepresentation() {
    assert(mSubterms.size() == 2);
    return mSubterms[0]->getRepresentation() + getName() + mSubterms[1]->getRepresentation();
  }
};

#endif
