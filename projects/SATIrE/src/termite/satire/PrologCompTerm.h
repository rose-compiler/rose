/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
     2007-2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#ifndef PROLOGCOMPTERM_H_
#define PROLOGCOMPTERM_H_
#include "PrologTerm.h"
#include <assert.h>
#include <stdarg.h>

#if !HAVE_SWI_PROLOG

/// Representation of a compound prolog term .
class PrologCompTerm : public PrologTerm {
public:
  ~PrologCompTerm() {};
  /// Creates a compound term with the given name. no subterms added yet.
  
  PrologCompTerm(std::string name = "#ERROR") : mName(name) {};

  PrologCompTerm(std::string name, size_t n, ...) : mName(name) {
    if(n > 0) {
      va_list params;
      va_start(params, n);
      for (size_t i=0; i < n; i++)
        addSubterm(va_arg(params, PrologTerm *));
      va_end(params);
    }
  }
  
  int getArity() { return mSubterms.size(); };
  bool isGround() {
    bool ground = true;
    std::vector<PrologTerm*>::iterator it;
    it = mSubterms.begin();
    while (it != mSubterms.end()) {
      ground = ground && (*it)->isGround();
      it++;
    }
    return ground;
  };

  /// Get the Functor
  std::string getName() { return mName; };
  std::string getRepresentation() {
    /*Pattern: name(...all subterms separated by commas..) */
    std::string rep = getName();
    rep += "(";
    std::vector<PrologTerm*>::iterator it;
    it = mSubterms.begin();
    // append the representation of all subterms
    while (it != mSubterms.end()) {
      rep = rep + (*it)->getRepresentation();
      // all but the last subterm are followed by a comma
      if(++it != mSubterms.end()) rep += ",";
    }
    rep += ")";
    return rep;
  };

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
  /// Creates a compound term with the given name. no subterms added yet.
  PrologInfixOperator(std::string name) : PrologCompTerm(name) {};
  std::string getRepresentation() {
    assert(mSubterms.size() == 2);
    return mSubterms[0]->getRepresentation() + getName() + mSubterms[1]->getRepresentation();
  }
};

#else

// The SWI Prolog implementation generates the term_t delayed, on the
// first read. Afterwards addSubTerms will fail!
class PrologCompTerm : public PrologTerm {
public:
  ~PrologCompTerm() {
    //PL_unregister_atom(term);
  }

  PrologCompTerm(term_t t) : PrologTerm(t) {
  };

  PrologCompTerm(std::string name, size_t n, ...) {
    // FIXME.. this should be done directly!!
    term = PL_new_term_ref();
    PL_put_atom_chars(term, name.c_str());
    if(n > 0) {
      va_list params;
      va_start(params, n);
      for (size_t i=0; i < n; i++)
        addSubterm(va_arg(params, PrologTerm *));
      va_end(params);
    }
  }

  /// Creates a compound term with the given name. no subterms added yet.
  PrologCompTerm(std::string functor = "#ERROR") {
    term = PL_new_term_ref();
    PL_put_atom_chars(term, functor.c_str());
#   if DEBUG_TERMITE
      std::cerr<<"CompTerm: PL_new_atom('"<<functor<<"') ="<<term<<std::endl;
#   endif
  }

  /// return a list of successors
  std::vector<PrologTerm*> getSubTerms()  {
    std::vector<PrologTerm *> mSubterms;
    int arity = getArity();
    for(int n = 1; n <= arity; n++) { 
      term_t arg = PL_new_term_ref();
      PL_get_arg(n, term, arg);
      mSubterms.push_back(newPrologTerm(arg));
    }
    return mSubterms;
  }

  /// Add a subterm at the first position
  void addFirstSubTerm(PrologTerm* t) {
#   if DEBUG_TERMITE
      std::cerr<<display(term)<<" . addFirstSubTerm("
	       <<t->getRepresentation()<<");"<<std::endl;
#   endif

    term_t old_term = term;
    int arity;
    atom_t name;
    if (PL_get_atom(old_term, &name)) { // still arity 0
      arity = 0;
    } else {
	PL_get_name_arity(old_term, &name, &arity);
    }

    // Construct a new, bigger term
    term_t args = PL_new_term_refs(arity+1);
    PL_put_variable(args);
    for(int n = 1; n <= arity; n++)
      assert(PL_get_arg(n, old_term, args+n));

    term = PL_new_term_ref();
    PL_cons_functor_v(term, PL_new_functor(name, arity+1), args); 
    assert(PL_unify_arg(1, term, t->getTerm()));

#   if DEBUG_TERMITE
      std::cerr<<" --> "<<display(term)<<" !"<<std::endl;
#   endif
  }

  /// Add a subterm at the last position
  void addSubterm(PrologTerm* t) {
#   if DEBUG_TERMITE
      std::cerr<<display(term)<<"  addSubterm("<<t->getRepresentation()<<");"
	       <<std::endl;
#   endif

    term_t old_term = term;
    int arity;
    term_t name;
    if (PL_get_atom(old_term, &name)) { // still arity 0
      arity = 0;
    } else {
      PL_get_name_arity(old_term, &name, &arity);
    }

    // Construct a new, bigger term
    term_t args = PL_new_term_refs(arity+1);
    for(int n = 0; n < arity; n++)
      assert(PL_get_arg(n+1, old_term, args+n));
    PL_put_variable(args+arity);

    term = PL_new_term_ref();
    PL_cons_functor_v(term, PL_new_functor(name, arity+1), args); 

    assert(PL_unify_arg(arity+1, term, t->getTerm()));

#   if DEBUG_TERMITE
      std::cerr<<" --> "<<display(term)<<" !"<<std::endl;
#   endif
  }

  /// the i-th subterm
  PrologTerm* at(int i) {      
    term_t arg = PL_new_term_ref();
    PL_get_arg(i+1, term, arg);
    return newPrologTerm(arg);
  }
};

class PrologInfixOperator : public PrologCompTerm {
public:
  PrologInfixOperator(std::string name) : PrologCompTerm(name) {};
};


#endif

#endif
