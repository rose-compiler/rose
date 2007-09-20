/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#ifndef PROLOGSTRING_H_
#define PROLOGSTRING_H_
#include "PrologTerm.h"
using namespace std;
/**
 * class representing a prolog string
 */
class PrologString : public PrologTerm {
 public:
		///constructor setting the string
  		PrologString(std::string name) : mName(name) {};
		///arity is 0
		int getArity() {return 0;};
		///a string is always ground
		bool isGround() {return true;};
		///return the string
		std::string getName() {return mName;};
		///return the string in double qoutes
		std::string getRepresentation() {return "\"" + mName + "\"";};
	private:
		///the string
		std::string mName;
};
#endif
