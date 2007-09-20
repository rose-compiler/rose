/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#ifndef PROLOGCOMPTERM_H_
#define PROLOGCOMPTERM_H_
#include "PrologTerm.h"
using namespace std;
/// Representation of a composite prolog term .
class PrologCompTerm : public PrologTerm {
 public:
  ~PrologCompTerm() {};
		/// Creates a composite term with the given name. no subterms added yet.
		
  PrologCompTerm(std::string name) : mName(name) {};
		int getArity();
		bool isGround();
		std::string getName() {return mName;};
		std::string getRepresentation();
		/// Add a subterm at the last position
		void addSubterm(PrologTerm* t) {mSubterms.push_back(t);};
		/// the i-th subterm
		PrologTerm* at(int i) {return mSubterms.at(i);};
	private:
		/// the name of the terms
		std::string mName;
		/// list of the subterms
		std::vector<PrologTerm *> mSubterms;
};
#endif
