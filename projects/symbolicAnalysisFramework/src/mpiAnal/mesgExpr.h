#ifndef MESG_EXPR_H
#define MESG_EXPR_H

#include "procSet.h"
#include "common.h"
#include "cfgUtils.h"
#include "variables.h"

class mesgExpr
{
	public:
	virtual mesgExpr& copy()=0;
	
	// Returns the subset of domain on which the message expression recv o send is the identity function
	// or an invalid subdomain is no such subdomain exists (object is heap-allocated)
	virtual procSet& getIdentityDomain(const mesgExpr& recv, const mesgExpr& send, 
	                                  const procSet& domain) const=0;
	
	// Maps the given domain to its corresponding range, returning the range process set (object is heap-allocated)
	virtual procSet& getImage(const procSet& domain) const=0;
	
	// Removes the portion of the message expression that maps the domain to the range,
	// as defined by cg (nodeConstant).
	// Returns true if successful, false if not.
	virtual bool removeMap(const procSet& domain, const procSet& remR)=0;
	
	// Returns true if this message expression can be merged with that message expression
	// with NO loss of information, in all possible executions (i.e. its a must-statement,
	// not a may-statement)
	virtual bool mustMergeable(const mesgExpr& that) const=0;
	
	// Merges this and that and updates this with the result.
	// Returns true if this causes this message expression to change, false otherwise.
	virtual bool mergeUpd(const mesgExpr& that)=0;
	
	// The number of different communication operations represented by this message expression
	virtual int numCommOps() const=0;
	
	// Returns true if this is a null message expression (performs no matches) and false otherwise
	virtual bool isNULL() const=0;
	
	// Assigns this to that
	virtual mesgExpr& operator=(const mesgExpr& that)=0;
	
	// Comparison
	virtual bool operator==(const mesgExpr& that) const=0;
	bool operator!=(const mesgExpr& that) const
	{ return !(*this == that); }
	
	// Returns a string representation of this set
	virtual string str(string indent="") const=0;
};

class nullMesgExpr : virtual public mesgExpr
{
	public:
	nullMesgExpr(){}
	
	mesgExpr& copy()
	{ return *(new nullMesgExpr()); }
	
	// Returns the subset of domain on which the message expression recv o send is the identity function
	// or an invalid subdomain is no such subdomain exists (object is heap-allocated)
	procSet& getIdentityDomain(const mesgExpr& recv, const mesgExpr& send, 
	                                  const procSet& domain) const
	{
		emptyProcSet* e = new emptyProcSet();
		return *e;
	}
	
	// Maps the given domain to its corresponding range, returning the range process set (object is heap-allocated)
	procSet& getImage(const procSet& domain) const
	{
		emptyProcSet* e = new emptyProcSet();
		return *e;
	}
	
	// Removes the portion of the message expression that maps the domain to the range,
	// as defined by cg (nodeConstant).
	// Returns true if successful, false if not.
	bool removeMap(const procSet& domain, const procSet& remR)
	{ return false; }
	
	// Returns true if this message expression can be merged with that message expression
	// with NO loss of information, in all possible executions (i.e. its a must-statement,
	// not a may-statement)
	bool mustMergeable(const mesgExpr& that) const
	{ return false; }
	
	// Merges this and that and updates this with the result.
	// Returns true if this causes this message expression to change, false otherwise.
	bool mergeUpd(const mesgExpr& that)
	{ return false; }
	
	// The number of different communication operations represented by this message expression
	int numCommOps() const
	{ return 0; }
	
	// Returns true if this is a null message expression (performs no matches) and false otherwise
	bool isNULL() const
	{ return true; }
	
	// Assigns this to that
	mesgExpr& operator=(const mesgExpr& that)
	{ ROSE_ASSERT(0); }
	
	// Comparison
	bool operator==(const mesgExpr& that) const
	{ return false; }
		
	// Returns a string representation of this set
	string str(string indent="") const
	{ 
		ostringstream outs;
		outs << indent << "[nullMesgExpr]";
		return outs.str();
	}
};

class realMesgExpr : virtual public mesgExpr
{
	public:
	// Returns true if this is a null message expression (performs no matches) and false otherwise
	bool isNULL() const
	{ return false; }	
};

class OneDmesgExpr : virtual public realMesgExpr
{
	/*varID rangeLB, rangeUB;
	// a (possibly) non-nodeConstant constraint graph that contains the range variable
	ConstrGraph* rangeCG;*/
	// The set of processes that this message expression maps all input domains to.
	// mRange.getConstr() may include non-nodeConstant variables
public:
	contRangeProcSet mRange;
	
	public:

	OneDmesgExpr()
	{}
	
	OneDmesgExpr(const contRangeProcSet& mRange) : mRange(mRange)
	{}
	
	OneDmesgExpr(const OneDmesgExpr& that) : mRange(that.mRange)
	{}
	
	// Creates a message expression from the given SgExpression
	OneDmesgExpr(SgExpression* expr, ConstrGraph* cg);
	
	// Creates a message expression from the given SgExpression, annotating the message expression
	// bounds with the given process set.
	OneDmesgExpr(SgExpression* expr, ConstrGraph* cg, int pSet);
	
	//~OneDmesgExpr();
	
	mesgExpr& copy()
	{ return *(new OneDmesgExpr(*this)); }
	
	// Returns the subset of domain on which the message expression recv o send is the identity function
	// or an invalid subdomain is no such subdomain exists (object is heap-allocated)
	procSet& getIdentityDomain(const mesgExpr& recv, const mesgExpr& send, 
	                           const procSet& domain) const;
	
	// Maps the given domain to its corresponding range, returning the range process set (object is heap-allocated)
	procSet& getImage(const procSet& domain) const;
	
	contRangeProcSet& getImage(const contRangeProcSet& domain) const;
	
	// Removes the portion of the message expression that maps the domain to the range,
	// as defined by cg (nodeConstant).
	// Returns true if successful, false if not.
	bool removeMap(const procSet& domain, const procSet& remR);
	
	// Returns true if this message expression can be merged with that message expression
	// with NO loss of information, in all possible executions (i.e. its a must-statement,
	// not a may-statement)
	bool mustMergeable(const mesgExpr& that) const;
	
	// Merges this and that and updates this with the result.
	// Returns true if this causes this message expression to change, false otherwise.
	bool mergeUpd(const mesgExpr& that);
	
	// The number of different communication operations represented by this message expression
	int numCommOps() const;
	
	// Transition from using the current constraint graph to using newCG, while annotating
	// the lower and upper bound variables of the range with the given annotation annotName->annot.
	// Return true if this causes this set to change, false otherwise.
	bool setConstrAnnot(ConstrGraph* newCG, string annotName, void* annot);
	
	// Returns the constraint graph used by the mRange process set
	ConstrGraph* getConstr() const;
	
	// Sets the constraint graph used by the mRange process set
	// Returns true if this causes the message expression to change, false otherwise
	bool setConstr(ConstrGraph* cg);
	
	// Assigns this to that
	mesgExpr& operator=(const mesgExpr& that);
	mesgExpr& operator=(const OneDmesgExpr& that);
	
	// Comparison
	bool operator==(const mesgExpr& that) const;
	
	void transClosure();
	
	// Returns a string representation of this set
	string str(string indent="") const;
};

#endif
