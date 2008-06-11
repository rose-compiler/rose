/* -*- cplusplus -*-
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
          2008 Adrian Prantl
see LICENSE in the root folder of this project
*/
#ifndef PROLOGLIST_H_
#define PROLOGLIST_H_
#include "PrologTerm.h"
#include <deque>

/**
 * class representing a prolog list*/
class PrologList : public PrologTerm {
 public:
  /**default constructor*/
  PrologList() {};
  /** construct from vector*/
  PrologList(std::vector<PrologTerm*> v) : mTerms(v) {};
  PrologList(std::deque<PrologTerm*> v) :mTerms(v.begin(), v.end()) {};
  
  /// return size of the list
  int getArity() {return mTerms.size();};
  /// are all the members ground?
  bool isGround() {
    bool ground = true;
    //List is ground if all elements are.
    vector<PrologTerm*>::iterator it = mTerms.begin();
    while(it != mTerms.end()) {
      ground = ground && (*it)->isGround();
      it++;
    }
    return ground;
  };

  /// the predicate name of a list in prolog is .
  string getName() {return ".";};
  /// output the representation
  std::string getRepresentation() {
    /*Pattern: name(...all subterms separated by commas..) */
    string rep = "[";
    std::vector<PrologTerm*>::iterator it;
    it = mTerms.begin();
    // append the representation of all subterms
    while (it != mTerms.end()) {
      rep = rep + (*it)->getRepresentation();
      // all but the last subterm are followed by a comma
      if(++it != mTerms.end()) rep += ",";
    }
    rep += "]";
    return rep;
  };

  /// add a list element
  void addElement(PrologTerm* t) {mTerms.push_back(t);};
  /// get the i-th element
  PrologTerm* at(int i) {return mTerms.at(i);};
  /// return a list of successors
  vector<PrologTerm*>* getSuccs() {return &mTerms;};
 private:
  /// the successors
  vector<PrologTerm*> mTerms;
};
#endif
