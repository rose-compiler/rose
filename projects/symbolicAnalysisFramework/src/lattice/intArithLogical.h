#ifndef INT_ARITH_LOGICAL
#define INT_ARITH_LOGICAL

#include "common.h"
#include "cfgUtils.h"
#include "lattice.h"
#include "logical.h"
#include "nodeState.h"
#include "variables.h"
#include "spearWrap.h"
#include "printAnalysisStates.h"

// Represents an expression that uses arithmetic and logical operators
// on a set of integral variables.
class IntArithLogical : public FiniteLattice, public LogicalCond
{
public:
	// the information content of this expression/level in the lattice
	typedef enum
	{
		// this object is uninitialized
		uninitialized, 
		// no information is known about the value of the variable
		bottom,
		// we have a specific formula for this variable (may be True = no information, or False = full information)
		known/*, 
		// this variable can be either positive or negative
		top*/
	} infContent;
	
	infContent level;

	
	// The possible comparison operations
	typedef enum {eq=SpearOp::Equal, le=SpearOp::SgnLTE} cmpOps;
	
	// The possible logical operations
	typedef enum {andOp=SpearOp::AndOp, orOp=SpearOp::OrOp, notOp=SpearOp::NotOp} logOps;
	
	class exprLeafOrNode: public virtual SpearExpr
	{
		public:
		typedef enum {eLeaf, lNode} elt;
		virtual elt elType()=0;
		
		typedef enum {isTrue, exprKnown, isFalse} infContent;
		virtual infContent getLevel()=0;
		
		// Sets this expression to True, returning true if this causes
		// the expression to be modified and false otherwise.
		virtual bool setToTrue()=0;
		
		// Sets this expression to False, returning true if this causes
		// the expression to be modified and false otherwise.
		virtual bool setToFalse()=0;
		
		// Negate this logical expression.
		// Returns true if this causes this logicNode to change, false otherwise.
		virtual bool notUpd()=0;
		
		virtual bool operator==(exprLeafOrNode& that)=0;
		bool operator!=(exprLeafOrNode& that) { return !(*this == that); }
		
		// Information order on affine relationships with the least information (True) being highest
		// and most information (False) being lowest. Some expressions are not comparable since
		// their sets of accepted points are not strictly related to each other.
		virtual bool operator<=(exprLeafOrNode& that)=0;
		bool operator<(exprLeafOrNode& that) { return (*this != that) && (*this <= that); }
		bool operator>=(exprLeafOrNode& that) { return (*this == that) && !(*this <= that); }
		bool operator>(exprLeafOrNode& that) { return (*this != that) && (*this >= that); }
		
		// Returns a human-readable string representation of this expression.
		// Every line of this string must be prefixed by indent.
		// The last character of the returned string should not be '\n', even if it is a multi-line string.
		virtual string str(string indent="")=0;
	};
	
	class logicNode;
	
	// A leaf expression: a*x cmpOp b*y + c, where x and y are variables and a,b and c are integers 
	// These expressions are used conservatively: they must contain no more information than is actually 
	// known to be true. In other words, given a set R of points (x,y) that satisfy the real condition,
	// and the set A of points that satisfy the approximation stored in exprLeaf. R must be a subset of A.
	// Lattice: isTrue     (all points x,y are accepted when some should not be: zero knowledge)
	//         exprKnown   (exactly correct set of points x,y are accepted: partial knowledge)
	//          isFalse    (no points x,y are accepted: perfect knowledge)
	class exprLeaf: public virtual exprLeafOrNode
	{
		friend class logicNode;
		
		protected:
		SpearOp cmp;
		int a, b, c;
		varID x, y;
		
		exprLeafOrNode::infContent level;
		
		list<SpearAbstractVar*> vars;
		SpearAbstractVar* outVar;
		string logExpr;
		bool varsExprInitialized;
		
		exprLeaf(SpearOp cmp, int a, varID x, int b, varID y, int c);
		
		public:
		exprLeaf(cmpOps cmp, int a, varID x, int b, varID y, int c);
		
		exprLeaf(const exprLeaf& that);
		
		protected:
		// divide out from a, b and c any common factors, reducing the triple to its normal form
		// return true if this modifies this constraint and false otherwise
		bool normalize();
		
		// Sets this expression to True, returning true if this causes
		// the expression to be modified and false otherwise.
		bool setToTrue();
		
		// Sets this expression to False, returning true if this causes
		// the expression to be modified and false otherwise.
		bool setToFalse();
		
		infContent getLevel();
		
		// computes vas and logExpr
		void computeVarsExpr();
		
		public:
			
		// returns a list of the names of all the variables needed to represent the logical 
		// expression in this object
		const list<SpearAbstractVar*>& getVars();
		
		// returns the variable that represents the result of this expression
		SpearAbstractVar* getOutVar();
		
		// returns the SPEAR Format expression that represents the constraint in this object
		const string& getExpr();
		
		// Returns a human-readable string representation of this expression.
		// Every line of this string must be prefixed by indent.
		// The last character of the returned string should not be '\n', even if it is a multi-line string.
		string str(string indent="");
		string str(string indent="") const;
		string genStr(string indent="") const;
		
		elt elType(){ return exprLeafOrNode::eLeaf; }
		
		SpearExpr* copy();
		
		// Negate this logical expression.
		// Returns true if this causes this logicNode to change, false otherwise.
		bool notUpd();
		
		// And-s this expression with the given expression, if possible
		// Return true if the conjunction is possible and this now contains the conjunction expression and
		//        false if it is not possible.
		// If the conjunction is possible, modified is set to true if the conjunction causes
		//    this to change and false otherwise.
		bool andExprs(const exprLeaf& that, bool &modified);
		// returns the same thing as orExprs but doesn't actually perform the conjunction
		bool andExprsTest(const exprLeaf& that);
		
		// Or-s this expression with the given expression, if possible
		// Return true if the disjunction is possible and this now contains the disjunction expression and
		//        false if it is not possible.
		// If the conjunction is possible, modified is set to true if the conjunction causes
		//    this to change and false otherwise.
		bool orExprs(const exprLeaf& that, bool &modified);
		// returns the same thing as andExprs but doesn't actually perform the disjunction
		bool orExprsTest(const exprLeaf& that);
		
		bool operator==(exprLeafOrNode& that);
		
		// Information order on affine relationships with the least information (True) being highest
		// and most information (False) being lowest. Some expressions are not comparable since
		// their sets of accepted points are not strictly related to each other.
		bool operator<=(exprLeafOrNode& that);
	};
	
	// An internal node that represents a logical connective between the affine relations
	class logicNode: public virtual exprLeafOrNode
	{
		list<exprLeafOrNode*> children;
		SpearOp logOp;
		exprLeafOrNode::infContent level;
		
		bool varsExprInitialized;
		list<SpearAbstractVar*> vars;
		SpearAbstractVar*       outVar;
		string logExpr;
				
		public:
		logicNode(logOps logOp);
		
		// constructor for logOp==notOp, which has a single child
		logicNode(logOps logOp, exprLeafOrNode* child);
		
		// constructor for logOp==andOp, orOp and xorOp, which have 2+ children
		logicNode(logOps logOp, exprLeafOrNode* childA, exprLeafOrNode* childB);
		
		// constructor for logOp==andOp, orOp and xorOp, which have 2+ children
		logicNode(logOps logOp, const list<exprLeafOrNode*>& children);
		
		logicNode(const logicNode& that);
		
		~logicNode();
		
		// add a new child to this node
		void addChild(exprLeafOrNode* child);
		
		protected:
		// propagates all the true and false sub-terms upwards through the tree, pruning the appropriate portions
		// returns two if this causes the logicNode to change and false otherwise
		bool normalize();
		
		public:
		// Sets this expression to True, returning true if this causes
		// the expression to be modified and false otherwise.
		bool setToTrue();
		
		// Sets this expression to False, returning true if this causes
		// the expression to be modified and false otherwise.
		bool setToFalse();
		
		infContent getLevel();
		
		// returns a list of the names of all the variables needed to represent the logical 
		// expression in this object
		const list<SpearAbstractVar*>& getVars();
		
		// returns the variable that represents the result of this expression
		SpearAbstractVar* getOutVar();
		
		// returns the SPEAR Format expression that represents the constraint in this object
		const string& getExpr();
		
		// Returns a human-readable string representation of this expression.
		// Every line of this string must be prefixed by indent.
		// The last character of the returned string should not be '\n', even if it is a multi-line string.
		string str(string indent="");
		string str(string indent="") const;
		string genStr(string indent="") const;
		
		protected:
		void computeVarsExpr();
		
		public:
		elt elType(){ return exprLeafOrNode::lNode; }
		
		SpearExpr* copy();
		
		// Negate this logical expression.
		// Returns true if this causes this logicNode to change, false otherwise.
		bool notUpd();
		
		// given a 2-level expression tree, collects all the permutations of grandchildren
		// from each child branch into conjunct logicNodes and saves them in newChildren.
		// Thus, (A^B^C)v(D^E^F) would become the list (A^D),(A^E),(A^F),(B^D),(B^E),(B^F),(C^D),(C^E),(C^F).
		// The function works recursively, one child at a time.
		// newChildren: The list where we'll save the new conjuncts
		// newConjOrig: Accumulated stack of grandchildren that will be combined to form each conjunct
		//              Thus, for input (A^B)v(C^D)v(D^E)v(F^G)v(H^I), the contents of newConjOrig
		//              may be A,C,E,F, with H or I to be added when the function recurses again.
		// curChild: Iterator that refers to the current child we're processing. The conjuncts
		//           are formed whenever curChild reaches the end of children.
		void genChildrenConj(list<exprLeafOrNode*>& newChildren, list<exprLeafOrNode*> newConjOrig, 
		                     list<exprLeafOrNode*>::const_iterator curChild);
		
		// ANDs this and that, storing the result in this.
		// Returns true if this causes this logicNode to change, false otherwise.
		bool andUpd(logicNode& that);

		// OR this and that, storing the result in this.
		// Returns true if this causes this logicNode to change, false otherwise.
		bool orUpd(logicNode& that);
		
		// Removes all instances of p*var with (q*var + r) and adjusts all relations that involve var
		// accordingly.
		// Returns true if this causes the logicNode object to change and false otherwise.
		bool replaceVar(varID var, int p, int q, int r);
		
		// Removes all facts that relate to the given variable, possibly replacing them 
		// with other facts that do not involve the variable but could be inferred through
		// the removed facts. (i.e. if we have x<y ^ y<z and wish to remove y, the removed 
		// expressions may be replaced with x<z or just True)
		// Returns true if this causes the logicNode object to change and false otherwise.
		bool removeVar(varID var);
		
		protected:
		// generic function used by orUpd and andUpd to insert new term into a disjunction
		// newChildren - The new list of children that will eventually replace children. We will be trying to
		//               insert newTerm into newChildren.
		void insertNewChildOr(list<exprLeafOrNode*>& newChildren, exprLeafOrNode* newTerm);
		
		// compares two different children lists, returning true if they're equal and false otherwise
		bool eqChildren(list<exprLeafOrNode*>& one, list<exprLeafOrNode*>& two);
		
		public:
		bool operator==(exprLeafOrNode& that);
		
		// Information order on affine relationships with the least information (True) being highest
		// and most information (False) being lowest. Some expressions are not comparable since
		// their sets of accepted points are not strictly related to each other.
		bool operator<=(exprLeafOrNode& that);
	};
	
	/*class DNFOrNode : logicNode
	{
		// Negate this logical expression 
		void notUpd();
		
		// given a 2-level expression tree, collects all the permutations of grandchildren
		// from each child branch into conjunct logicNodes and saves them in newChildren.
		// Thus, (A^B^C)v(D^E^F) would become the list (A^D),(A^E),(A^F),(B^D),(B^E),(B^F),(C^D),(C^E),(C^F).
		// The function works recursively, one child at a time.
		// newChildren: The list where we'll save the new conjuncts
		// newConjOrig: Accumulated stack of grandchildren that will be combined to form each conjunct
		//              Thus, for input (A^B)v(C^D)v(D^E)v(F^G)v(H^I), the contents of newConjOrig
		//              may be A,C,E,F, with H or I to be added when the function recurses again.
		// curChild: Iterator that refers to the current child we're processing. The conjuncts
		//           are formed whenever curChild reaches the end of children.
		void genChildrenConj(list<logicNode*>& newChildren, list<exprLeaf*> newConjOrig, 
		                     list<exprLeafOrNode*>::const_iterator curChild);
	};
	
	class DNFAndNode : logicNode
	{
		// Negate this logical expression 
		void notUpd();
	};*/
	
	// The expression tree itself
	logicNode* expr;
	
	public:
	// Creates an uninitialized logical expression
	IntArithLogical();
	
	// Creates a logical expression that is either True or False, depending on the value argument
	IntArithLogical(bool value);
	
	// Create an IntArithLogical that corresponds to a single affine relationship
	IntArithLogical(cmpOps cmp, int a, varID x, int b, varID y, int c);
	
	IntArithLogical(const IntArithLogical& that);
		
	// initializes this Lattice to its default bottom state
	// if the given new level is higher than bottom, expr is also initialized to a new object
	//void initialize(IntArithLogical::infContent newLevel=bottom);
	void initialize();
	
	// initializes this Lattice to its default state, with a specific value (true or false)
	void initialize(bool value);
	
	// returns a copy of this lattice
	Lattice* copy() const;
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
	
	// the basic logical operations that must be supported by any implementation of 
	// a logical condition: NOT, AND and OR
	// Return true if this causes the IntArithLogical object to change and false otherwise.
	bool notUpd();
	bool andUpd(LogicalCond& that);
	bool orUpd(LogicalCond& that);
	
	// Sets this expression to True, returning true if this causes
	// the expression to be modified and false otherwise.
	////// If onlyIfNotInit=true, this is only done if the expression is currently uninitialized
	bool setToTrue(/*bool onlyIfNotInit=false*/);
	
	// Sets this expression to False, returning true if this causes
	// the expression to be modified and false otherwise.
	////// If onlyIfNotInit=true, this is only done if the expression is currently uninitialized
	bool setToFalse(/*bool onlyIfNotInit=false*/);
	
	// Removes all facts that relate to the given variable, possibly replacing them 
	// with other facts that do not involve the variable but could be inferred through
	// the removed facts. (i.e. if we have x<y ^ y<z and wish to remove y, the removed 
	// expressions may be replaced with x<z or just True)
	// Returns true if this causes the IntArithLogical object to change and false otherwise.
	bool removeVar(varID var);
	
	// returns a copy of this LogicalCond object
	LogicalCond* copy();
	
	protected:
	// Writes the full expression that corresponds to this object, including any required
	// declarations and range constraints to os. Returns that variable that summarizes this expression.
	// otherVars - list of variables that also need to be declared and ranged
	// createProposition - if true, outputSpearExpr() creates a self-contained proposition. If false, no 
	//       proposition is created; it is presumed that the caller will be using the expression as part 
	//       of a larger proposition.
	SpearVar outputSpearExpr(exprLeaf* otherExpr, ofstream &os, bool createProposition);
	
	// Runs Spear on the given input file. Returns true if the file's conditions are satisfiable and false otherwise.
	static bool runSpear(string inputFile);
	
	public:
	// Queries whether the given affine relation is implied by this arithmetic/logical constrains.
	// Returns true if yes and false otherwise
	bool isImplied(cmpOps cmp, int a, varID x, int b, varID y, int c);
	
	// returns true if this logical condition must be true and false otherwise
	bool mayTrue();
	
	// Queries whether the arithmetic/logical constrains may be consistent with the given affine relation.
	// Returns true if yes and false otherwise
	bool mayConsistent(cmpOps cmp, int a, varID x, int b, varID y, int c);
	
	// Updates the expression with the information that x*a has been assigned to y*b+c
	// returns true if this causes the expression to change and false otherwise
	bool assign(int a, varID x, int b, varID y, int c);
};

/****************************
 *** affineInequalityFact ***
 ****************************/

class IntArithLogicalFact : public NodeFact
{
	public:
	IntArithLogical expr;
	
	IntArithLogicalFact()
	{}
	
	IntArithLogicalFact(const IntArithLogical& expr): expr(expr)
	{ }
	
	IntArithLogicalFact(const IntArithLogicalFact& that)
	{
		this->expr = that.expr;
	}
	
	NodeFact* copy() const;
	
	string str(string indent="");
};

/*****************************
 *** IntArithLogicalPlacer ***
 *****************************/

class IntArithLogicalPlacer : public UnstructuredPassIntraAnalysis
{
	public:
	void visit(const Function& func, const DataflowNode& n, NodeState& state);
	
	protected:
	// points trueIneqFact and falseIneqFact to freshly allocated objects that represent the true and false
	// branches of the control flow guarded by the given expression. They are set to NULL if our representation
	// cannot represent one of the expressions.
	// doFalseBranch - if =true, falseIneqFact is set to the correct false-branch condition and to NULL otherwise
	static void setTrueFalseBranches(SgExpression* expr, 
	                             IntArithLogicalFact **trueIneqFact, IntArithLogicalFact **falseIneqFact, 
	                             bool doFalseBranch);
};

// prints the inequality facts set by the given affineInequalityPlacer
void printIntArithLogicals(IntArithLogicalPlacer* aip, string indent="");

// Runs the Affine Inequality Placer analysis
void runIntArithLogicalPlacer(bool printStates);

// returns the set of IntArithLogical expressions known to be true at the given DataflowNode
const IntArithLogical& getIntArithLogical(const DataflowNode& n);

#endif
