/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#include "PrologCompTerm.h"

int PrologCompTerm::getArity() {
	return mSubterms.size(); 
}

bool PrologCompTerm::isGround() {
	bool ground = true;
	std::vector<PrologTerm*>::iterator it;
	it = mSubterms.begin();
	while (it != mSubterms.end()) {
		ground = ground && (*it)->isGround();
		it++;
	}
	return ground;
}

std::string PrologCompTerm::getRepresentation() {
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
}


