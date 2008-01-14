/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#ifndef PROLOGATOM_H_
#define PROLOGATOM_H_
#include "PrologTerm.h"
#include <ctype.h>

/**class representing a prolog atom*/
class PrologAtom : public PrologTerm {
 public:
	 ///the destructor
  ~PrologAtom() {};	
  ///constructor setting the string
  PrologAtom(std::string name) : mName(name) {};
  		///the arity is always 0
		int getArity() {return 0;};
		///an atom is always ground
		bool isGround() {return true;};
		///return the string
		std::string getName() {return mName;};
		/// return the string
		std::string getRepresentation() {
		  if ((mName.length() > 0) && isupper(mName[0])) {
		      std::string s;
		      s = "'" + mName  + "'";
		      return s;
		    }
		    return mName;
		};
	private:
		/// the string
		std::string mName;
};
#endif
