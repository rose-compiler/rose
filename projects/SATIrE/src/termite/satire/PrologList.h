/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#ifndef PROLOGLIST_H_
#define PROLOGLIST_H_
#include "PrologTerm.h"
#include <deque>

#if !HAVE_SWI_PROLOG

/**
 * class representing a prolog list*/
class PrologList : public PrologTerm {
 public:
  /**default constructor*/
  PrologList() {}
  /** construct from vector*/
  PrologList(std::deque<PrologTerm*> v) : mTerms(v) {}
  PrologList(std::vector<PrologTerm*> v) :mTerms(v.begin(), v.end()) {}
  
  /// return size of the list
  int getArity() {return mTerms.size();}
  /// are all the members ground?
  bool isGround() {
    bool ground = true;
    //List is ground if all elements are.
    std::deque<PrologTerm*>::iterator it = mTerms.begin();
    while(it != mTerms.end()) {
      ground = ground && (*it)->isGround();
      it++;
    }
    return ground;
  }

  /// the predicate name of a list in prolog is .
  std::string getName() {return ".";}
  /// output the representation
  std::string getRepresentation() {
    /*Pattern: name(...all subterms separated by commas..) */
    std::string rep = "[";
    std::deque<PrologTerm*>::iterator it;
    it = mTerms.begin();
    // append the representation of all subterms
    while (it != mTerms.end()) {
      rep = rep + (*it)->getRepresentation();
      // all but the last subterm are followed by a comma
      if(++it != mTerms.end()) rep += ",";
    }
    rep += "]";
    return rep;
  }

  /// add a list element
  void addElement(PrologTerm* t) {mTerms.push_back(t);}

  /// add a list element at the beginning
  void addFirstElement(PrologTerm* t) {mTerms.push_front(t);}

  /// get the i-th element
  PrologTerm* at(int i) {return mTerms.at(i);}
  /// return a list of successors
  std::deque<PrologTerm*>* getSuccs() {return &mTerms;}
 private:
  /// the successors
  std::deque<PrologTerm*> mTerms;
};

# else


/**
 * class representing a prolog list*/
class PrologList : public PrologTerm {
 public:
  /** empty list */
  PrologList() {
    term = PL_new_term_ref();
    PL_put_nil(term);
#   if DEBUG_TERMITE
      std::cerr<<"new PrologList: "<<display(term)<<std::endl;
#   endif
  }

  PrologList(term_t t) : PrologTerm(t) {}

  /** construct from vector*/
  PrologList(std::vector<PrologTerm*> v) {
    term = PL_new_term_ref();
    PL_put_nil(term);
    for (std::vector<PrologTerm*>::reverse_iterator i = v.rbegin();
	 i != v.rend(); ++i) { 
      PL_cons_list(term, (*i)->getTerm(), term);
    }
#   if DEBUG_TERMITE
      std::cerr<<"new PrologList: "<<display(term)<<std::endl;
#   endif
  }

  PrologList(std::deque<PrologTerm*> v) {
    term = PL_new_term_ref();
    PL_put_nil(term);
    for (std::deque<PrologTerm*>::reverse_iterator i = v.rbegin();
	 i != v.rend(); ++i) { 
      PL_cons_list(term, (*i)->getTerm(), term);
    }
#   if DEBUG_TERMITE
      std::cerr<<"new PrologList: "<<display(term)<<std::endl;
#   endif
  }
  
  /// return size of the list
  int getArity() { 
    int l = 0;
    fid_t fid = PL_open_foreign_frame();
    // FIXME: cache predicate
    term_t a0 = PL_new_term_refs(3);
    assert(PL_unify(a0, term));
    PL_put_variable(a0+2);
    qid_t qid = PL_open_query(NULL, PL_Q_NORMAL, 
			      PL_predicate("length", 2, ""), a0);
    assert(PL_next_solution(qid) && 
	   PL_get_integer(a0+1, &l));
    //PL_close_query(qid);
    PL_cut_query(qid);
    PL_discard_foreign_frame(fid);
    return l;
  }

  /// add a list element
  void addElement(PrologTerm* t) {
#   if DEBUG_TERMITE
      std::cerr<<"  addElement("<<display(term)<<" + " 
	       <<t->getRepresentation()<<")"<<std::endl;
#   endif

    term_t a0 = PL_new_term_refs(3);
    assert(PL_unify(a0, term));
    PL_put_nil(a0+1);
    PL_cons_list(a0+1, t->getTerm(), a0+1);
    PL_put_variable(a0+2);
    // TODO: cache the predicates
    assert(PL_call_predicate(NULL, PL_Q_NORMAL, 
			     PL_predicate("append", 3, "library(lists)"), a0));
    term = a0 + 2;

#   if DEBUG_TERMITE
      for (int i = 0; i < 3; ++i)
	std::cerr<<display(a0+i)<<std::endl;
      std::cerr<<"-> "<<display(term)<<std::endl;
#   endif
  }

  /// add the first list element
  void addFirstElement(PrologTerm* t) {
    PL_cons_list(term, t->getTerm(), term);
  }

  /// get the i-th element
  PrologTerm* at(int i) {
    assert(i >= 0);
    if ((unsigned) i < mTerms.size())
      return mTerms[i];

    term_t t = PL_copy_term_ref(term);

    for (int c = 0; c < i; c++)
      assert(PL_get_tail(t, t));
    PL_get_head(t, t);
    return newPrologTerm(t);
  }

  /// return a list of successors
  std::deque<PrologTerm*>* getSuccs()  {
    if (mTerms.size() == 0) {
      term_t tail = PL_copy_term_ref(term);
      term_t head = PL_new_term_ref();
      while (PL_get_list(tail, head, tail))
	mTerms.push_back(newPrologTerm(head));
    }

    return &mTerms;
  }

 private:
  /// the successors
  std::deque<PrologTerm*> mTerms;
};


#endif

#endif
