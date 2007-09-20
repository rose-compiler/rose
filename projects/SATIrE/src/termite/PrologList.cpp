/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#include "PrologList.h"

bool PrologList::isGround() {
  bool ground = true;
  //List is ground if all elements are.
  vector<PrologTerm*>::iterator it = mTerms.begin();
  while(it != mTerms.end()) {
    ground = ground && (*it)->isGround();
    it++;
  }
  return ground;
}


std::string PrologList::getRepresentation() {
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
}
