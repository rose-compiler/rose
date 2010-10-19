#ifndef LATTICE_H
#define LATTICE_H

#include <string>
#include <map>
#include "CallGraphTraverse.h"
#include "variables.h"
using namespace std;

class Lattice : public printable
{
	public:
	// initializes this Lattice to its default state, if it is not already initialized
	virtual void initialize()=0;
	// returns a copy of this lattice
	virtual Lattice* copy() const=0;
	// overwrites the state of this Lattice with that of that Lattice
	virtual void copy(Lattice* that)=0;
	
	// Called by analyses to create a copy of this lattice. However, if this lattice maintains any 
	//    information on a per-variable basis, these per-variable mappings must be converted from 
	//    the current set of variables to another set. This may be needed during function calls, 
	//    when dataflow information from the caller/callee needs to be transferred to the callee/calleer.
	// We do not force child classes to define their own versions of this function since not all
	//    Lattices have per-variable information.
	// varNameMap - maps all variable names that have changed, in each mapping pair, pair->first is the 
	//              old variable and pair->second is the new variable
	// func - the function that the copy Lattice will now be associated with
	virtual /*Lattice**/void remapVars(const map<varID, varID>& varNameMap, const Function& newFunc) {} 
	
	// Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
	// that contains data for a set of variables and incorporateVars must overwrite the state of just
	// those variables, while leaving its state for other variables alone.
	// We do not force child classes to define their own versions of this function since not all
	//    Lattices have per-variable information.
	virtual void incorporateVars(Lattice* that) {}
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	virtual bool meetUpdate(Lattice* that)=0;
	// computes the meet of this and that and returns the result
	//virtual Lattice* meet(Lattice* that)=0;
	virtual bool finiteLattice()=0;
	
	virtual bool operator==(Lattice* that) /*const*/=0;
		
	// The string that represents this object
	// If indent!="", every line of this string must be prefixed by indent
	// The last character of the returned string should not be '\n', even if it is a multi-line string.
	//virtual string str(string indent="") /*const*/=0;
};


class FiniteLattice : public virtual Lattice
{
	public:
	bool finiteLattice()
	{ return true;	}
};

class InfiniteLattice : public virtual Lattice
{
	public:
	bool finiteLattice()
	{ return false; }
	
	// widens this from that and saves the result in this
	// returns true if this causes this to change and false otherwise
	virtual bool widenUpdate(InfiniteLattice* that)=0;
};

#endif
