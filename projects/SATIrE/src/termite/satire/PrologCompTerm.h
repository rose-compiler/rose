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
      for (size_t i=0; i < n; i++) {
	PrologTerm* t = va_arg(params, PrologTerm *);
	assert(t != NULL);
        addSubterm(t);
      }
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
    term_t name;
    int arity;
    PL_get_name_arity(t, &name, &arity);
    while (arity --> 0)
      subterms.push_back(NULL);
  };

#if 0
  // AP (2009-11-10) Deprecated. Use the somewhat less flexible versions below.
  PrologCompTerm(std::string name, size_t n, ...) : PrologTerm()
  {
 // GB (2009-11-03): More sophisticated construction of composite terms. The
 // "term" member variable is initialized by the default constructor invoked
 // above. We copy the argument terms to a new argument list, create a
 // functor, and hope that everything works out well.
    atom_t functorAtom = PL_new_atom(name.c_str());
    if (n > 0) {
      functor_t functor = PL_new_functor(functorAtom, n);
      term_t args = PL_new_term_refs(n);
      PL_cons_functor_v(term, functor, args);
      va_list params;
      va_start(params, n);
      for (size_t i = 0; i < n; i++) {
        PrologTerm *arg_i = va_arg(params, PrologTerm *);
	assert(arg_i != NULL);
        PL_unify_arg(i+1, term, arg_i->getTerm());
        subterms.push_back(arg_i);
      }
      va_end(params);
    } else {
      term = functorAtom;
    }
  }
#endif

# define COMPTERM_CONSTRUCTOR(N)		      \
  atom_t functorAtom = PL_new_atom(name.c_str());     \
  functor_t functor = PL_new_functor(functorAtom, N); \
  term_t args = PL_new_term_refs(N);		      \
  PL_cons_functor_v(term, functor, args)

  PrologCompTerm(std::string name, PrologTerm* t1) : PrologTerm()
  {
    COMPTERM_CONSTRUCTOR(1);
    PL_unify_arg(1, term, t1->getTerm());
    subterms.push_back(t1);
  }

  PrologCompTerm(std::string name, PrologTerm* t1, PrologTerm* t2) 
  : PrologTerm()
  {
    COMPTERM_CONSTRUCTOR(2);
    PL_unify_arg(1, term, t1->getTerm());
    PL_unify_arg(2, term, t2->getTerm());
    subterms.push_back(t1);
    subterms.push_back(t2);
  }

  PrologCompTerm(std::string name, 
		 PrologTerm* t1, PrologTerm* t2, PrologTerm* t3) 
  : PrologTerm()
  {
    COMPTERM_CONSTRUCTOR(3);
    PL_unify_arg(1, term, t1->getTerm());
    PL_unify_arg(2, term, t2->getTerm());
    PL_unify_arg(3, term, t3->getTerm());
    subterms.push_back(t1);
    subterms.push_back(t2);
    subterms.push_back(t3);
  }

  PrologCompTerm(std::string name, 
		 PrologTerm* t1, PrologTerm* t2, PrologTerm* t3, 
		 PrologTerm* t4) 
  : PrologTerm()
  {
    COMPTERM_CONSTRUCTOR(4);
    PL_unify_arg(1, term, t1->getTerm());
    PL_unify_arg(2, term, t2->getTerm());
    PL_unify_arg(3, term, t3->getTerm());
    PL_unify_arg(4, term, t4->getTerm());
    subterms.push_back(t1);
    subterms.push_back(t2);
    subterms.push_back(t3);
    subterms.push_back(t4);
  }

  PrologCompTerm(std::string name, 
		 PrologTerm* t1, PrologTerm* t2, PrologTerm* t3, 
		 PrologTerm* t4, PrologTerm* t5) 
  : PrologTerm()
  {
    COMPTERM_CONSTRUCTOR(5);
    PL_unify_arg(1, term, t1->getTerm());
    PL_unify_arg(2, term, t2->getTerm());
    PL_unify_arg(3, term, t3->getTerm());
    PL_unify_arg(4, term, t4->getTerm());
    PL_unify_arg(5, term, t5->getTerm());
    subterms.push_back(t1);
    subterms.push_back(t2);
    subterms.push_back(t3);
    subterms.push_back(t4);
    subterms.push_back(t5);
  }

  PrologCompTerm(std::string name, 
		 PrologTerm* t1, PrologTerm* t2, PrologTerm* t3,
 		 PrologTerm* t4, PrologTerm* t5, PrologTerm* t6) 
  : PrologTerm()
  {
    COMPTERM_CONSTRUCTOR(6);
    PL_unify_arg(1, term, t1->getTerm());
    PL_unify_arg(2, term, t2->getTerm());
    PL_unify_arg(3, term, t3->getTerm());
    PL_unify_arg(4, term, t4->getTerm());
    PL_unify_arg(5, term, t5->getTerm());
    PL_unify_arg(6, term, t6->getTerm());
    subterms.push_back(t1);
    subterms.push_back(t2);
    subterms.push_back(t3);
    subterms.push_back(t4);
    subterms.push_back(t5);
    subterms.push_back(t6);
  }

  PrologCompTerm(std::string name, 
		 PrologTerm* t1, PrologTerm* t2, PrologTerm* t3, 
		 PrologTerm* t4, PrologTerm* t5, PrologTerm* t6, PrologTerm* t7)
  : PrologTerm()
  {
    COMPTERM_CONSTRUCTOR(7);
    PL_unify_arg(1, term, t1->getTerm());
    PL_unify_arg(2, term, t2->getTerm());
    PL_unify_arg(3, term, t3->getTerm());
    PL_unify_arg(4, term, t4->getTerm());
    PL_unify_arg(5, term, t5->getTerm());
    PL_unify_arg(6, term, t6->getTerm());
    PL_unify_arg(7, term, t7->getTerm());
    subterms.push_back(t1);
    subterms.push_back(t2);
    subterms.push_back(t3);
    subterms.push_back(t4);
    subterms.push_back(t5);
    subterms.push_back(t6);
    subterms.push_back(t7);
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
    assert(0 && "this function does not exist");

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

    subterms.push_back(t);

#   if DEBUG_TERMITE
      std::cerr<<" --> "<<display(term)<<" !"<<std::endl;
#   endif
  }

  /// the i-th subterm
  PrologTerm* at(int i) {
    if (subterms[i] != NULL)
      return subterms[i];
    else {
      term_t arg = PL_new_term_ref();
      PL_get_arg(i+1, term, arg);
      subterms[i] = newPrologTerm(arg);
      return subterms[i];
    }
  }

private:
  std::vector<PrologTerm *> subterms;
};

class PrologInfixOperator : public PrologCompTerm {
public:
  PrologInfixOperator(std::string name) : PrologCompTerm(name) {};
};


#endif

#endif
