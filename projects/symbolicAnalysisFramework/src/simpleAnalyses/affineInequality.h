#ifndef AFFINE_INEQUALITY_H
#define AFFINE_INEQUALITY_H

#include <sstream>
#include <iostream>
#include <string>
#include <functional>
#include <queue>

using namespace std;

#include "common.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysis.h"
#include "divAnalysis.h"
#include "printAnalysisStates.h"
#include "logical.h"

// represents the constraint on variables x and y: x*a <= y*b + c
class affineInequality: public printable // : public LogicalCond
{
	friend class varAffineInequality;
	
	public:
	// The different signs that a variable may have
	typedef enum{
		// This variable's state is unknown
		unknownSgn, 
		// This variable is =zero
		eqZero, 
		// This variable is positive or =zero
		posZero, 
		// This variable is negative or =zero
		negZero} signs;
	
	protected:
	//varID y;
	int a;
	int b;
	int c;
	
	// flags that indicate whether the x or y variables are = zeroVar
	bool xZero;
	bool yZero;
	
	// indicate the sign of x and y
	signs xSign;
	signs ySign;
	
	public:
	// Possible levels of this constraint. Above bottom the levels are ordered according to their 
	//    information content. False has the most content (admits fewest points), then a specific
	//    affine inequality and finally, top, which represents a state where the true inequality is
	//    too complex to be represented as an affine inequality, meaning that the only affine inequality
	//    that can do the job is x <= y + infinity.
	// no constraint known since current constraints are not representable using a single affine 
	// inequality (i.e. x <= y + infinity)
	static const short top=3;
	// some affine inequality constraint is known
	static const short constrKnown=2;
	// it is known that the affine constraints are inconsistent: x<=y-infinity
	//static const short falseConstr=1;
	// the values of the variables are not constrained relative to each other (effectively, bottom == uninitialized)
	// (this object would not even be created in this case)
	static const short bottom=0;
	
	protected:
	// the current level in this constraint's lattice
	short level;
	
	public:
	
	affineInequality();
	
	affineInequality(const affineInequality& that);
	
	affineInequality(int a, int b, int c, bool xZero, bool yZero, signs xSign, signs ySign);
	
	// given a constraint on x, z and a constraint on z, y, infers the corresponding constraint on x, y 
	// and sets this constraint to it
	affineInequality(const affineInequality& xz, const affineInequality& zy/*, bool xZero, bool yZero/*, DivLattice* divX, DivLattice* divY, varID z*/);
	
	bool operator=(const affineInequality& that);
	
	bool operator==(const affineInequality& that) const;
	
	bool operator!=(const affineInequality& that) const;
	
	// lexicographic ordering (total order)
	bool operator<(const affineInequality& that) const;
	
	// semantic affineInequality ordering (partial order)
	// returns true if this affineInequality represents less or more information (less information is 
	// top, more information is bottom) than that for all values of x and y and false otherwise
	//bool operator<<(const affineInequality& that) const;
	bool semLessThan(const affineInequality& that, bool xEqZero, bool yEqZero) const;
	
	bool set(const affineInequality& that);
	
	bool set(int a, int b, int c);
	bool set(int a, int b, int c, bool xZero, bool yZero, signs xSign, signs ySign);
	
	bool setA(int a);
	
	bool setB(int b);
	
	bool setC(int c);
	
	// sets this constraint object to bottom
	bool setToBottom();
	
	// sets this constraint object to false
	//bool setToFalse();
	
	// sets this constraint object to top
	bool setToTop();
	
	// returns y, a, b or c
	
	int getA() const;
	
	int getB() const;
	
	int getC() const;
	
	short getLevel() const;
	
	bool isXZero() const;
	bool isYZero() const;
	
	signs getXSign() const;
	signs getYSign() const;
	
	protected:
	// divide out from a, b and c any common factors, reducing the triple to its normal form
	// return true if this modifies this constraint and false otherwise
	bool normalize();
	
	public:
	// INTERSECT this with that, saving the result in this
	// Intersection = the affine inequality that contains the constraint information of both
	//         this AND that (i.e. the line that is lower than both lines)
	// (moves this/that upward in the lattice)
	void operator*=(const affineInequality& that);
	
	// Just like *=, except intersectUpd() returns true if the operation causes this
	// affineInequality to change and false otherwise.
	bool intersectUpd(const affineInequality& that);

	// UNION this with that, saving the result in this
	// Union = the affine inequality that contains no more information than either
	//         this OR that (i.e. the line that is higher than both lines)
	// (moves this/that downward in the lattice)
	void operator+=(const affineInequality& that);
	
	// Just like +=, except unionUpd() returns true if the operation causes this
	// affineInequality to change and false otherwise.
	bool unionUpd(const affineInequality& that);
	
	// WIDEN this with that, saving the result in this
	//void operator^=(const affineInequality& that);
	
	// returns true if the x-y constraint constrXY is consistent with the y-x constraint constrYX for 
	// some values of x and y and false otherwise. Since affineInequalities are conservative in the
	// sense that they don't contain any more information than is definitely true, it may be
	// that the true constraints are in fact inconsistent but we do not have enough information to
	// prove this.
	static bool mayConsistent(const affineInequality& constrXY, const affineInequality& constrYX);
	
	static string signToString(signs sign);
	
	string str(string indent="");
	string str(string indent="") const;
	
	string str(varID x, varID y, string indent="") const;
	
	public:
	// the basic logical operations that must be supported by any implementation of 
	// a logical condition: NOT, AND and OR
	/*void notUpd();
	void andUpd(LogicalCond& that);
	void orUpd(LogicalCond& that);*/
};

// represents a full affine inequality, including the variables involved in the inequality
class varAffineInequality : public printable //: public LogicalCond
{
	protected:
	varID x;
	varID y;
	affineInequality ineq;
	
	public:
	varAffineInequality(const varAffineInequality& that);
	
	varAffineInequality(const varID& x, const varID& y, const affineInequality& ineq);
	
	varAffineInequality(const varID& x, const varID& y, int a, int b, int c, bool xZero, bool yZero);
	
	// get methods
	/*varID& getX() const;
	
	varID& getY() const;
	
	affineInequality& getIneq() const;*/
	
	const varID& getX() const;
	
	const varID& getY() const;
	
	int getA() const;
	
	int getB() const;
	
	int getC() const;
	
	
	const affineInequality& getIneq() const;
	
	// set methods, return true if this object changes
	const bool setX(const varID& x);
	
	const bool setY(const varID& y);
	
	const bool setA(int a);
	
	const bool setB(int b);
	
	const bool setC(int c);
	
	const bool setIneq(affineInequality& ineq);
	
	// comparison methods
	bool operator==(const varAffineInequality& that) const;
	
	bool operator<(const varAffineInequality& that) const;
	
	string str(string indent="");

	string str(string indent="") const;
	
	// Parses expr and returns the corresponding varAffineInequality if expr can be represented
	// as such and NULL otherwise.
	//static varAffineInequality* parseIneq(SgExpression* expr);
	
	public:
	// the basic logical operations that must be supported by any implementation of 
	// a logical condition: NOT, AND and OR
	/*void notUpd();
	void andUpd(LogicalCond& that);
	void orUpd(LogicalCond& that);
	
	// returns a copy of this LogicalCond object
	LogicalCond* copy();*/
};

/****************************
 *** affineInequalityFact ***
 ****************************/

class affineInequalityFact : public NodeFact
{
	public:
	set<varAffineInequality> ineqs;
	
	affineInequalityFact()
	{}
	
	affineInequalityFact(const affineInequalityFact& that)
	{
		this->ineqs = that.ineqs;
	}
	
	NodeFact* copy() const;
	
	string str(string indent="");
	string str(string indent="") const;
};


/********************************
 *** affineInequalitiesPlacer ***
 ********************************/
class affineInequalitiesPlacer : public UnstructuredPassIntraAnalysis
{
	public:
	void visit(const Function& func, const DataflowNode& n, NodeState& state);
	
	protected:
		// points trueIneqFact and falseIneqFact to freshly allocated objects that represent the true and false
	// branches of the control flow guarded by the given expression. They are set to NULL if our representation
	// cannot represent one of the expressions.
	// doFalseBranch - if =true, falseIneqFact is set to the correct false-branch condition and to NULL otherwise
	static void setTrueFalseIneq(SgExpression* expr, 
	                             affineInequalityFact **trueIneqFact, affineInequalityFact **falseIneqFact, 
	                             bool doFalseBranch);
};

/*// Looks over all the conditional statements in the application and associates appropriate 
//    affine inequalities with the SgNodes that depend on these statements. Each inequality
//    is a must: it must be true of the node that it is associated with.
void initAffineIneqs(SgProject* project);

// return the set of varAffineInequalities associated with this node or NULL if none are available
set<varAffineInequality>* getAffineIneq(SgNode* n);*/

/*class printAffineInequalities : public UnstructuredPassIntraAnalysis
{
	affineInequalitiesPlacer* placer;
	
	public:
	printAffineInequalities(affineInequalitiesPlacer *placer);
		
	void visit(const Function& func, const DataflowNode& n, NodeState& state);
};*/

// prints the inequality facts set by the given affineInequalityPlacer
void printAffineInequalities(affineInequalitiesPlacer* aip, string indent="");

// Runs the Affine Inequality Placer analysis
void runAffineIneqPlacer(bool printStates=false);

// returns the set of inequalities known to be true at the given DataflowNode
const set<varAffineInequality>& getAffineIneq(const DataflowNode& n);

#endif
