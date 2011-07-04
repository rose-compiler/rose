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
	
	// Returns a Lattice that describes the information known within this lattice
	// about the given expression. By default this could be the entire lattice or any portion of it.
	// For example, a lattice that maintains lattices for different known variables and expression will 
	// return a lattice for the given expression. Similarly, a lattice that keeps track of constraints
	// on values of variables and expressions will return the portion of the lattice that relates to
	// the given expression. 
	// It it legal for this function to return NULL if no information is available.
	// The function's caller is responsible for deallocating the returned object
	virtual Lattice* project(SgExpression* expr) { return copy(); }
	
	// The inverse of project(). The call is provided with an expression and a Lattice that describes
	// the dataflow state that relates to expression. This Lattice must be of the same type as the lattice
	// returned by project(). unProject() must incorporate this dataflow state into the overall state it holds.
	// Call must make an internal copy of the passed-in lattice and the caller is responsible for deallocating it.
	// Returns true if this causes this to change and false otherwise.
	virtual bool unProject(SgExpression* expr, Lattice* exprState) { return meetUpdate(exprState); }
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	virtual bool meetUpdate(Lattice* that)=0;
	// computes the meet of this and that and returns the result
	//virtual Lattice* meet(Lattice* that)=0;
	virtual bool finiteLattice()=0;
	
	virtual bool operator==(Lattice* that) /*const*/=0;
	bool operator!=(Lattice* that) {
		return !(*this == that);
	}
	bool operator==(Lattice& that) {
		return *this == &that;
	}
	bool operator!=(Lattice& that) {
		return !(*this == that);
	}
	
	// Functions used to inform this lattice that a given variable is now in use (e.g. a variable has entered 
	//    scope or an expression is being analyzed) or is no longer in use (e.g. a variable has exited scope or
	//    an expression or variable is dead).
	// It is assumed that a newly-added variable has not been added before and that a variable that is being
	//    removed was previously added
	/*virtual void addVar(varID var)=0;
	virtual void remVar(varID var)=0;*/
			
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
