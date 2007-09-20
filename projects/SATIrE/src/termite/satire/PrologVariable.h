/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#ifndef PROLOGVARIABLE_H_
#define PROLOGVARIABLE_H_
#include "PrologTerm.h"
using namespace std;
///class representing a prolog variable
class PrologVariable : public PrologTerm {
	public:
		///constructor setting the name
		PrologVariable(std::string name) : mName(name) {};
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
