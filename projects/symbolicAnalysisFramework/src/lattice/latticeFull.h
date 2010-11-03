#ifndef LATTICE_EXTRA_H
#define LATTICE_EXTRA_H

#include <string>
#include "cfgUtils.h"
#include "variables.h"
#include "nodeState.h"
#include "lattice.h"
using namespace std;


/******************************
 *** Commonly used lattices ***
 ******************************/
 
/* Generic lattice wrapper that adds an uninitialized state below all the regular
   lattice states and assumes a single bottom state that corresponds to no information. * /
class uninitBotLattice : public Lattice
{
	public:
	typedef enum infContent {uninitalized, bottom, value};
	
	private:
	infContent level;
	
	public:
	
	uninitBotLattice();
	
	uninitBotLattice(const uninitLattice &that);
	
	// initializes this Lattice to its bottom state, if it it not already initialized
	void initialize();
	
	// called by derived classes to indicate that this lattice has been set to a specific value
	void setToVal();
	
	// Returns this lattice's level in the uninitialized <= bottom <= value hierarchy
	infContent getUninitBotLevel();
	
	// returns a copy of this lattice
	//Lattice* copy();
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// Called by analyses to create a copy of this lattice. However, if this lattice maintains any 
	//    information on a per-variable basis, these per-variable mappings must be converted from 
	//    the current set of variables to another set. This may be needed during function calls, 
	//    when dataflow information from the caller/callee needs to be transferred to the callee/calleer.
	// We do not force child classes to define their own versions of this function since not all
	//    Lattices have per-variable information.
	// varNameMap - maps all variable names that have changed, in each mapping pair, pair->first is the 
	//              old variable and pair->second is the new variable
	// func - the function that the copy Lattice will now be associated with
	void remapVars(const map<varID, varID>& varNameMap, const Function& newFunc);
	
	// Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
	// that contains data for a set of variables and incorporateVars must overwrite the state of just
	// those variables, while leaving its state for other variables alone.
	// We do not force child classes to define their own versions of this function since not all
	//    Lattices have per-variable information.
	void incorporateVars(Lattice* that);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that);
	
	virtual bool operator==(Lattice* that);
		
	// The string that represents this object
	// If indent!="", every line of this string must be prefixed by indent
	// The last character of the returned string should not be '\n', even if it is a multi-line string.
	string str(string indent="");
};*/
 
class BoolAndLattice : public FiniteLattice
{
	// state can be:
	// -1 : unset (default value)
	// 0 : false
	// 1 : true
	int state;
	
	public:
	BoolAndLattice()
	{ state = -1; }
	
	private:
	BoolAndLattice(int state)
	{ this->state = state; }
	
	public:
	BoolAndLattice(bool state)
	{ this->state = state; }
	
	// initializes this Lattice to its default state
	void initialize()
	{ state = -1; }
	
	// returns a copy of this lattice
	Lattice* copy() const;
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that);
	
	// computes the meet of this and that and returns the result
	//Lattice* meet(Lattice* that);
		
	bool operator==(Lattice* that);
	
	// returns the current state of this object
	bool get() const;
	
	// sets the state of this BoolAndLattice to the given value
	// returns true if this causes the BoolAndLattice state to change, false otherwise
	bool set(bool state);
	
	// sets the state of this lattice to the conjunction of the BoolAndLattice's current state and the given value
	// returns true if this causes the BoolAndLattice state to change, false otherwise
	bool andUpd(bool state);
	
	string str(string indent="");
};

class IntMaxLattice : public InfiniteLattice
{
	int state;
	
	public:
	static const int infinity;// = 32768;
	
	IntMaxLattice()
	{
		state = -1;
	}
	
	IntMaxLattice(int state)
	{
		this->state = state;
	}
	
	// initializes this Lattice to its default state
	void initialize()
	{
		state = -1;
	}
	
	// returns a copy of this lattice
	Lattice* copy() const;
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that);
	
	// computes the meet of this and that and returns the result
	//Lattice* meet(Lattice* that);
	
	bool operator==(Lattice* that);
	
	// widens this from that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool widenUpdate(InfiniteLattice* that);
	
	// returns the current state of this object
	int get() const;
	
	// sets the state of this lattice to the given value
	// returns true if this causes the lattice's state to change, false otherwise
	bool set(int state);
	
	// increments the state of this lattice by the given value
	// returns true if this causes the lattice's state to change, false otherwise
	bool incr(int increment);
	
	// computes the maximum of the given value and the state of this lattice and saves 
	//    the result in this lattice
	// returns true if this causes the lattice's state to change, false otherwise
	bool maximum(int value);
	
	string str(string indent="");
};

/*########################
  ### Utility lattices ###
  ########################*/
 
class ProductLattice : public virtual Lattice
{
	protected:
	vector<Lattice*> lattices;
	
	public:
	ProductLattice();
	ProductLattice(const vector<Lattice*>& lattices);
	
	void init(const vector<Lattice*>& lattices);
	
	// initializes this Lattice to its default state
	void initialize();
	
	const vector<Lattice*>& getLattices();
	
	// initializes the given vector with a copy of the lattices vector
	void copy_lattices(vector<Lattice*>& newLattices) const;
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that);
	
	bool operator==(Lattice* that);
	
	// The string that represents this object
	// If indent!="", every line of this string must be prefixed by indent
	// The last character of the returned string should not be '\n', even if it is a multi-line string.
	string str(string indent="");
};

class FiniteProductLattice : public virtual ProductLattice, public virtual FiniteLattice
{
	public:
	FiniteProductLattice() : ProductLattice(), FiniteLattice()
	{}
	
	FiniteProductLattice(const vector<Lattice*>& lattices) : ProductLattice(lattices), FiniteLattice()
	{
		verifyFinite();
	}
	
	FiniteProductLattice(const FiniteProductLattice& that) : ProductLattice(that.lattices), FiniteLattice()
	{
		verifyFinite();
	}
	
	void verifyFinite()
	{
		for(vector<Lattice*>::iterator it = lattices.begin(); it!=lattices.end(); it++)
				ROSE_ASSERT((*it)->finiteLattice());
	}
	
	// returns a copy of this lattice
	Lattice* copy() const
	{
		return new FiniteProductLattice(*this);
	}
};

class InfiniteProductLattice : public virtual ProductLattice, public virtual InfiniteLattice
{
	public:
	InfiniteProductLattice() : ProductLattice(), InfiniteLattice()
	{}
	
	InfiniteProductLattice(const vector<Lattice*>& lattices) : ProductLattice(lattices), InfiniteLattice()
	{}
	
	InfiniteProductLattice(const InfiniteProductLattice& that) : ProductLattice(that.lattices), InfiniteLattice()
	{}
	
	// returns a copy of this lattice
	Lattice* copy() const
	{
		return new InfiniteProductLattice(*this);
	}
	
	// widens this from that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool widenUpdate(InfiniteLattice* that)
	{
		bool modified=false;
		vector<Lattice*>::iterator it, itThat;
		for(it = lattices.begin(), itThat = (dynamic_cast<InfiniteProductLattice*>(that))->lattices.begin(); 
		    it!=lattices.end() && itThat!=(dynamic_cast<InfiniteProductLattice*>(that))->lattices.end(); 
		    it++, itThat++)
			modified = (dynamic_cast<InfiniteProductLattice*>(*it))->widenUpdate(dynamic_cast<InfiniteProductLattice*>(*itThat)) || modified;
          return modified;
	}
};


class VariablesProductLattice : public virtual ProductLattice
{
	protected:
	// if =true, a lattice is created for each scalar variable
	bool includeScalars;
	// if =true, a lattice is created for each array variable 
	bool includeArrays;
	// the function that this lattice is associated with
	Function func;
	// map of lattices that correspond to constant variables
	map<varID, Lattice*> constVarLattices;
	// lattice that corresponds to allVar;
	Lattice* allVarLattice;
	
	// sample lattice that will be initially associated with every variable (before the analysis)
	Lattice* perVarLattice;
	
	// maps variables in a given function to the index of their respective Lattice objects in 
	// the ProductLattice::lattice[] array
	static map<Function, map<varID, int> > varLatticeIndex;
	
	public:
	// creates a new VariablesProductLattice
	// includeScalars - if =true, a lattice is created for each scalar variable
	// includeArrays - if =true, a lattice is created for each array variable 
	// perVarLattice - sample lattice that will be associated with every variable in scope at node n
	//     it should be assumed that the object pointed to by perVarLattice will be either
	//     used internally by this VariablesProductLattice object or deallocated
	// constVarLattices - map of additional variables and their associated lattices, that will be 
	//     incorporated into this VariablesProductLattice in addition to any other lattices for 
	//     currently live variables (these correspond to various useful constant variables like zeroVar)
	// allVarLattice - the lattice associated with allVar (the variable that represents all of memory)
	//     if allVarLattice==NULL, no support is provided for allVar
	// func - the current function
	// n - the dataflow node that this lattice will be associated with
	// state - the NodeState at this dataflow node
	VariablesProductLattice(bool includeScalars, bool includeArrays, Lattice* perVarLattice, 
	                        const map<varID, Lattice*>& constVarLattices, Lattice* allVarLattice,
	                        const Function& func, const DataflowNode& n, const NodeState& state);
	                        
	// copy constructor
	VariablesProductLattice(const VariablesProductLattice& that);
	
	public:
	
	Lattice* getVarLattice(const Function& func, const varID& var);
	
	protected:
	// sets up the varLatticeIndex map, if necessary
	void setUpVarLatticeIndex();
	
	// returns the index of var among the variables associated with func
	// or -1 otherwise
	int getVarIndex(const Function& func, const varID& var);
	
	public:
	// returns the set of global variables(scalars and/or arrays)
	varIDSet& getGlobalVars() const;
	static varIDSet& getGlobalVars(bool includeScalars, bool includeArrays);
	
	// returns the set of variables(scalars and/or arrays) declared in this function
	varIDSet& getLocalVars(Function func) const;
	
	// returns the set of variables(scalars and/or arrays) referenced in this function
	varIDSet& getRefVars(Function func) const;

	// returns the set of variables(scalars and/or arrays) visible in this function
	varIDSet getVisibleVars(Function func) const;
	
	// returns a copy of this lattice
	//Lattice* copy() const;
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// Called by analyses to create a copy of this lattice. However, if this lattice maintains any 
	//    information on a per-variable basis, these per-variable mappings must be converted from 
	//    the current set of variables to another set. This may be needed during function calls, 
	//    when dataflow information from the caller/callee needs to be transferred to the callee/calleer.
	// We do not force child classes to define their own versions of this function since not all
	//    Lattices have per-variable information.
	// varNameMap - maps all variable names that have changed, in each mapping pair, pair->first is the 
	//              old variable and pair->second is the new variable
	// func - the function that the copy Lattice will now be associated with
	/*Lattice**/void remapVars(const map<varID, varID>& varNameMap, const Function& newFunc);
	
	// Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
	// that contains data for a set of variables and incorporateVars must overwrite the state of just
	// those variables, while leaving its state for other variables alone.
	// We do not force child classes to define their own versions of this function since not all
	//    Lattices have per-variable information.
	void incorporateVars(Lattice* that);
	
	// The string that represents this object
	// If indent!="", every line of this string must be prefixed by indent
	// The last character of the returned string should not be '\n', even if it is a multi-line string.
	string str(string indent="");
};

class FiniteVariablesProductLattice : public virtual VariablesProductLattice, public virtual FiniteProductLattice
{
	public:
	// creates a new VariablesProductLattice
	// perVarLattice - sample lattice that will be associated with every variable in scope at node n
	//     it should be assumed that the object pointed to by perVarLattice will be either
	//     used internally by this VariablesProductLattice object or deallocated
	// constVarLattices - map of additional variables and their associated lattices, that will be 
	//     incorporated into this VariablesProductLattice in addition to any other lattices for 
	//     currently live variables (these correspond to various useful constant variables like zeroVar)
	// allVarLattice - the lattice associated with allVar (the variable that represents all of memory)
	//     if allVarLattice==NULL, no support is provided for allVar
	// func - the current function
	// n - the dataflow node that this lattice will be associated with
	// state - the NodeState at this dataflow node
	FiniteVariablesProductLattice(bool includeScalars, bool includeArrays, 
	                              Lattice* perVarLattice, const map<varID, Lattice*>& constVarLattices, Lattice* allVarLattice,
	                        		const Function& func, const DataflowNode& n, const NodeState& state) : 
	    VariablesProductLattice(includeScalars, includeArrays, perVarLattice, constVarLattices, allVarLattice, func, n, state), 
	    FiniteProductLattice()
	{
		verifyFinite();
	}
	
	FiniteVariablesProductLattice(const FiniteVariablesProductLattice& that) : 
		VariablesProductLattice(that), FiniteProductLattice()
	{
		verifyFinite();
	}
	
	// returns a copy of this lattice
	Lattice* copy() const
	{
		return new FiniteVariablesProductLattice(*this);
	}
};

class InfiniteVariablesProductLattice : public virtual VariablesProductLattice, public virtual InfiniteProductLattice
{
	public:
	// creates a new VariablesProductLattice
	// perVarLattice - sample lattice that will be associated with every variable in scope at node n
	//     it should be assumed that the object pointed to by perVarLattice will be either
	//     used internally by this VariablesProductLattice object or deallocated
	// constVarLattices - map of additional variables and their associated lattices, that will be 
	//     incorporated into this VariablesProductLattice in addition to any other lattices for 
	//     currently live variables (these correspond to various useful constant variables like zeroVar)
	// allVarLattice - the lattice associated with allVar (the variable that represents all of memory)
	//     if allVarLattice==NULL, no support is provided for allVar
	// func - the current function
	// n - the dataflow node that this lattice will be associated with
	// state - the NodeState at this dataflow node
	InfiniteVariablesProductLattice(bool includeScalars, bool includeArrays, 
	                                Lattice* perVarLattice, map<varID, Lattice*> constVarLattices, Lattice* allVarLattice,
	                        		  const Function& func, const DataflowNode& n, const NodeState& state) : 
	    VariablesProductLattice(includeScalars, includeArrays, perVarLattice, constVarLattices, allVarLattice, func, n, state), 
	    InfiniteProductLattice()
	{
	}
	
	InfiniteVariablesProductLattice(const FiniteVariablesProductLattice& that) : 
		VariablesProductLattice(that), InfiniteProductLattice()
	{
	}
	
	// returns a copy of this lattice
	Lattice* copy() const
	{
		return new InfiniteVariablesProductLattice(*this);
	}
};


#endif
