/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#ifndef PROLOGVARIABLE_H_
#define PROLOGVARIABLE_H_
#include "PrologTerm.h"

///class representing a prolog variable
class PrologVariable : public PrologTerm {
	public:
		///constructor setting the name
  PrologVariable(std::string name) : mName(name) { assert(false); };
		/// arity is always zero
		int getArity() {return 0;};
		/// a variable isn't ground
		bool isGround() {return false;};
		/// return the name
		std::string getName() {return mName;};
		/// output the name
		std::string getRepresentation() {return mName;};
		
	private:
		/// the name
		std::string mName;
};

#endif
