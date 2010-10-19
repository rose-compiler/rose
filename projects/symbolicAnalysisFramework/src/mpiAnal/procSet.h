#ifndef PROC_SET_H
#define PROC_SET_H

#include "common.h"
#include "cfgUtils.h"
#include "variables.h"
#include "ConstrGraph.h"
#include <string>

class procSet : public NodeFact//, public printable
{
	public:
		
	static const int infinity = -1;
	
	// Returns a heap-allocated reference to the intersection of this and that
	virtual procSet& intersect(const procSet& that) const=0;
	
	// Updates this with the intersection of this and that
	// returns true if this causes the procSet to change, false otherwise
	virtual bool intersectUpd(const procSet& that)=0;

	// Returns a heap-allocated reference to this - that
	virtual procSet& rem(const procSet& that) const=0;
	
	// Updates this with the result of this - that
	// returns true if this causes the procSet to change, false otherwise
	virtual bool remUpd(const procSet& that)=0;
	
	// Returns true if this set is empty, false otherwise
	virtual bool emptySet() const=0;
	
	// Returns true if this is a representable set, false otherwise
	virtual bool validSet() const=0;
	
	// The size of this process set, either a specific integer or infinity
	virtual int size() const=0;
	
	// Assigns this to that
	//virtual procSet& operator=(const procSet& that)=0;
	
	// Comparison
	virtual bool operator==(const procSet& that) const=0;
	bool operator!=(const procSet& that) const
	{ return !(*this == that); }
	
	virtual bool operator<(const procSet& that) const=0;
	
	// Returns a string representation of this set
	virtual string str(string indent="") const=0;
	
	// NodeFact methods

	// returns a copy of this node fact
	//NodeFact* copy() const;
};

class emptyProcSet : public virtual procSet
{
	public:
	
	// Returns a heap-allocated reference to the intersection of this and that
	procSet& intersect(const procSet& that) const
	{ return *(new emptyProcSet()); }
	
	// Updates this with the intersection of this and that
	// returns true if this causes the procSet to change, false otherwise
	bool intersectUpd(const procSet& that)
	{ return false; }
	
	// Returns a heap-allocated reference to this - that
	procSet& rem(const procSet& that) const
	{ return *(new emptyProcSet()); }
	
	// Updates this with the result of this - that
	// returns true if this causes the procSet to change, false otherwise
	bool remUpd(const procSet& that)
	{ return false; }
	
	// Returns true if this set is empty, false otherwise
	bool emptySet() const
	{ return true; }
	
	// Returns true if this is a representable set, false otherwise
	bool validSet() const
	{ return true; }
	
	// The size of this process set, either a specific integer or infinity
	int size() const
	{ return 0; }
	
	// Assigns this to that
	procSet& operator=(const procSet& that)
	{ ROSE_ASSERT(0); }
	
	// Comparison
	bool operator==(const procSet& that) const
	{ ROSE_ASSERT(0); }
	
	bool operator<(const procSet& that) const
	{ return false; }
	
	// Returns a string representation of this set
	string str(string indent)
	{ 
		ostringstream outs;
		outs << indent << "[emptyProcSet]";
		return outs.str();
	}
	
	string str(string indent) const
	{
		ostringstream outs;
		outs << indent << "[emptyProcSet]";
		return outs.str();
	}
	
	// NodeFact methods

	// returns a copy of this node fact
	NodeFact* copy() const
	{ return new emptyProcSet(); }
};

class invalidProcSet : public virtual procSet
{
	public:
	// Returns a heap-allocated reference to the intersection of this and that
	procSet& intersect(const procSet& that) const
	{ return *(new invalidProcSet()); }
	
	// Updates this with the intersection of this and that
	// returns true if this causes the procSet to change, false otherwise
	bool intersectUpd(const procSet& that)
	{ return false; }
	
	// Returns a heap-allocated reference to this - that
	procSet& rem(const procSet& that) const
	{ return *(new invalidProcSet()); }
	
	// Updates this with the result of this - that
	// returns true if this causes the procSet to change, false otherwise
	bool remUpd(const procSet& that)
	{ return false; }
	
	// Returns true if this set is empty, false otherwise
	bool emptySet() const
	{ return false; }
	
	// Returns true if this is a representable set, false otherwise
	bool validSet() const
	{ return false; }
	
	// The size of this process set, either a specific integer or infinity
	int size() const
	{ return 0; }
	
	// Assigns this to that
	procSet& operator=(const procSet& that)
	{ ROSE_ASSERT(0); }
	
	// Comparison
	bool operator==(const procSet& that) const
	{ ROSE_ASSERT(0); }
	
	bool operator<(const procSet& that) const
	{ return false; }
	
	// Returns a string representation of this set
	string str(string indent) //const
	{ 
		ostringstream outs;
		outs << indent << "[invalidProcSet]";
		return outs.str();
	}
	
	string str(string indent) const
	{ 
		ostringstream outs;
		outs << indent << "[invalidProcSet]";
		return outs.str();
	}

	// NodeFact methods

	// returns a copy of this node fact
	NodeFact* copy() const
	{ return new invalidProcSet(); }
};

// process sets that can be represented as contiguous ranges of ranks
class contRangeProcSet : public virtual procSet
{
	// The variables that represent the lower and upper bounds of the range of ranks
	// The range is [lb ... ub], inclusive.
	varID lb;
	varID ub;
	// nodeConstant constraints on lb and ub
	ConstrGraph* cg;
	// true if this represents an empty range 
	bool emptyRange;
	// true if this is a representable set, false otherwise
	bool valid;
	
	// The number of variables that have been generated as range lower and upper bounds
	static int varCount;
	
	varID genFreshVar();
	
	public:
	// Creates an empty set
	contRangeProcSet(bool emptyRange=true);
		
	contRangeProcSet(varID lb, varID ub, ConstrGraph* cg);
	
	// this->lb*lbA = lb*lbB + lbC
	// this->ub*ubA = ub*ubB + ubC
	contRangeProcSet(varID lb, int lbA, int lbB, int lbC,
	                 varID ub, int ubA, int ubB, int ubC,
	                 ConstrGraph* cg);
	
	// if freshenVars==true, calls genFreshBounds() to make this set use different lower and upper
	// bound variables from that set, while ensuring that the bound variables themselves are 
	// equal to each other in cg
	contRangeProcSet(const contRangeProcSet& that, bool freshenVars=false);
	
	//~contRangeProcSet();
	
	// copies that to this, returning true if this is not changed and false otherwise
	bool copy(const contRangeProcSet& that);
	
	// Return the current value of lb, ub and cg
	const varID& getLB() const;
	const varID& getUB() const;
	ConstrGraph* getConstr() const;
	
	// Set lb, ub and cg, returning true if this causes this process set to change
	// and false otherwise
	bool setLB(const varID& lb);
	bool setUB(const varID& ub);
	bool setConstr(ConstrGraph* cg);
	
	// Do not modify the upper or lower bound but add the constraint that it is equal lb/ub:
	// lb*a = newLB*b + c, ub*a = newUB*b + c
	bool assignLB(const varID& lb, int a=1, int b=1, int c=0);
	bool assignUB(const varID& ub, int a=1, int b=1, int c=0);
	
	// Asserts all the set-specific invariants in this set's constraint graph
	void refreshInvariants();
	
	// Generates new lower and upper bound variables that are set to be equal to the original
	// lower and upper bound variables in this procSet's constraint graph as:
	// newLB*lbA = oldLB*lbB + lbC and newUB*ubA = oldUB*ubB + ubC
	void genFreshBounds(int lbA=1, int lbB=1, int lbC=0,
	                    int ubA=1, int ubB=1, int ubC=0);
	
	// Transition from using the current constraint graph to using newCG, while annotating
	// the lower and upper bound variables with the given annotation annotName->annot.
	// Return true if this causes this set to change, false otherwise.
	bool setConstrAnnot(ConstrGraph* newCG, string annotName, void* annot);
	
	// Add the given annotation to the lower and upper bound variables
	// Return true if this causes this set to change, false otherwise.
	bool addAnnotation(string annotName, void* annot);
	
	// Remove the given annotation from this set's lower and upper bound variables
	// Return true if this causes this set to change, false otherwise.
	bool removeVarAnnot(string annotName);
	
	// Make this set invalid, return true if this causes the set to change, false otherwise
	bool invalidate();
	
	// Make this set valid, return true if this causes the set to change, false otherwise
	bool makeValid();
	
	// Make this set empty, return true if this causes the set to change, false otherwise.
	// Also, remove the set's lower and upper bounds from its associated constraint graph (if any).
	bool emptify();
	
	// Make this set non-empty, return true if this causes the set to change, false otherwise
	bool makeNonEmpty();
	
	// Returns true if this and that represent the equal ranges
	bool rangeEq(const contRangeProcSet& that) const;
	
	// Returns true if that is at the top of but not equal to this's range
	bool rangeTop(const contRangeProcSet& that) const;
	
	// Returns true if that is at the bottom of but not equal to this's range
	bool rangeBottom(const contRangeProcSet& that) const;
	
	// Returns true if the ranges of this and that must be disjoint
	bool rangeDisjoint(const contRangeProcSet& that) const;
	
	// Returns true if this must overlap, with this Below that: lb <= that.lb <= ub <= that.ub
	bool overlapAbove(const contRangeProcSet& that) const;
	
	// Returns true if this must overlap, with this Above that: that.lb <= lb <= that.ub <= ub
	bool overlapBelow(const contRangeProcSet& that) const;
	
	// Copies the given variable's constraints from srcCG to cg.
	// !!! May want to label the inserted variables with this procSet object to avoid collisions
	// !!! with other procSet objects.
	void copyVar(ConstrGraph* srcCG, const varID& var);
	
	// returns a heap-allocated reference to the intersection of this and that
	// if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
	procSet& intersect(const procSet& that) const;
	procSet& intersect(const contRangeProcSet& that, bool transClosure=false) const;
	
	// updates this with the intersection of this and that
	// returns true if this causes the procSet to change, false otherwise
	// if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
	bool intersectUpd(const procSet& that);
	bool intersectUpd(const contRangeProcSet& that, bool transClosure=false);
	
	// Returns a heap-allocated reference to this - that
	// if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
	procSet& rem(const procSet& that) const;
	procSet& rem(const contRangeProcSet& that , bool transClosure=false) const;
	
	// Updates this with the result of this - that
	// returns true if this causes the procSet to change, false otherwise
	// if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
	bool remUpd(const procSet& that);
	bool remUpd(const contRangeProcSet& that, bool transClosure=false);

	// Returns true if this set is empty, false otherwise
	bool emptySet() const;
	
	// Returns true if this is a representable set, false otherwise
	bool validSet() const;
	
	// The size of this process set, either a specific integer or infinity
	int size() const;
	
	// Assigns this to that
	procSet& operator=(const procSet& that);
	
	// Comparison
	bool operator==(const procSet& that) const;
	
	bool operator<(const procSet& that) const;
	
	// Returns a string representation of this set
	string str(string indent="") /*const*/;
	string str(string indent="") const;
	
	// NodeFact methods

	// returns a copy of this node fact
	NodeFact* copy() const;
	
	// Removes the upper and lower bounds of this set from its associated constraint graph
	// or the provided constraint graph (if it is not ==NULL) and sets cg to NULL if tgtCG==NULL.
	// Returns true if this causes the constraint graph to change, false otherwise.
	bool cgDisconnect();
	bool cgDisconnect(ConstrGraph* tgtCG) const;
};

#endif
