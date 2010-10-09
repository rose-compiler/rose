#ifndef CONSTR_GRAPH_H
#define CONSTR_GRAPH_H

#include <sstream>
#include <iostream>
#include <string>
#include <functional>
#include <queue>

#include "common.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "affineInequality.h"
#include "divAnalysis.h"
#include "sgnAnalysis.h"


// bottom - relations between each pair of variarbles are unknown or too complex to be representable as affine inequalities (minimal information)
// intermediate - some concrete information is known about some variable pairs
// bottom - impossible situation (maximal information) (bottom flag = true)

// By default the constraint graph is = top. Since this implies a top inequality between every pair, we don't 
// actually maintain such affineInequality objects. Instead, if there is no affineInequality between a pair of
// variables, this itself implies that this affineInequality=top.
class ConstrGraph : public virtual InfiniteLattice, public virtual LogicalCond
{
protected:
	// flag indicating whether some of the constraints have changed since the last time
	// this graph was checked for bottom-ness
	bool constrChanged;
	
/*	// indicates whether this constraint graph represents a dead array 
	// (i.e. it contains a negative cycle that goes through $)
	bool dead;
*/
	// indicates whether this constraint graph represents an impossible situation
	// (i.e. it contains a negative cycle that doesn't go through $)
	bool bottom;

	// Represents constrants (x<=y+c). vars2Value[x] maps to a set of constraint::<y, a, b, c>
	//map<varID, map<varID, constraint> > vars2Value;
	map<varID, map<varID, affineInequality> > vars2Value;
	
	// Set of arrays whose ranges are being tracked by this constraint graph
	varIDSet arrays;
	// Set of scalars that may be relevant for the tracked arrays
	varIDSet scalars;
	
	// Set of external versions of variables. Useful for keeping different constraint graphs synchronized
	// with each other's invariants while only performing periodic updates.
	varIDSet externalVars;
	
	/*// set of scalar divisibility variables. For each scalar x there exist x' s.t. if x = r (mod d) 
	// (according to DivAnalysis), the constraint graph will contain the constraint x'*d+r = x
	varIDSet& divScalars;*/
	
	// mapping from arrays to boolean flags. Each flag indicates whether its respective
	// array's range is empty (true) or not (false)
	map<varID, bool> emptyRange;
	
	// flag indicating whether this constraint graph should be considered initialized
	//    (the constraints it holds are real) or uninitialized (the constraints 
	//    do not correspond to a real state). Uninitialized constraint graphs behave
	//    just like regular constraint graphs but they are not equal to any other graph
	//    until they are initialized. Any operation that modifies or reads the state  
	//    of a constraint graph (not including comparisons or other operations that don't
	//    access individual variable mappings) causes it to become initialized (if it 
	//    wasn't already). An uninitialized constraint graph is !=bottom. 
	//    printing a graph's contents does not make it initialized
	bool initialized;
	
	// In order to allow the user to modify the graph in several spots before calling isFeasible()
	// we allow them to perform their modifications inside a transaction and call isFeasible only
	// at the end of the transaction
	bool inTransaction;
	
	// The divisibility lattices associated with the current CFG node
	// divL is a map from annotations to product lattices. Each product lattice will only be used to
	//    reason about variables that have the same annotation. When a variable has multiple annotations
	//    only one matching product lattice will be used.
	// The annotation ""->NULL matches all variables
	map<pair<string, void*>, FiniteVariablesProductLattice*> divL;
	
	// The sign lattices associated with the current CFG node
	// sgnL is a map from annotations to product lattices. Each product lattice will only be used to
	//   reason about variables that have the same annotation. When a variable has multiple annotations
	//   only one matching product lattice will be used.
	// The annotation ""->NULL matches all variables
	map<pair<string, void*>, FiniteVariablesProductLattice*> sgnL;
	
	// the function that this constraint graph belongs to
	Function func;
	
	// set of variables for which we have divisibility information
	set<varID> divVariables;
	// maps divisibility variables to their original variables
	//map<varID, varID> divVar2OrigVar;
	
	// Set of variables the for which we've added constraints since the last transitive closure
	set<varID> newConstrVars;
	// Set of variables the constraints on which have been modified since the last transitive closure
	set<varID> modifiedVars;
	
public:
	/**** Constructors & Destructors ****/
	ConstrGraph(bool initialized=true);
	
	ConstrGraph(Function func, FiniteVariablesProductLattice* divL, FiniteVariablesProductLattice* sgnL, bool initialized=true);
	ConstrGraph(Function func, const map<pair<string, void*>, FiniteVariablesProductLattice*>& divL, 
	            const map<pair<string, void*>, FiniteVariablesProductLattice*>& sgnL, bool initialized=true);
	
	ConstrGraph(const varIDSet& scalars, const varIDSet& arrays, bool initialized=true);
	
	//ConstrGraph(varIDSet& arrays, varIDSet& scalars, FiniteVariablesProductLattice* divL, bool initialized=true);
	
	ConstrGraph(ConstrGraph &that, bool initialized=true);
	
	ConstrGraph(const ConstrGraph* that, bool initialized=true);
	
	// Creates a constraint graph that contains the given set of inequalities, 
	//// which are assumed to correspond to just scalars
	ConstrGraph(const set<varAffineInequality>& ineqs, Function func, FiniteVariablesProductLattice* divL, FiniteVariablesProductLattice* sgnL);
	ConstrGraph(const set<varAffineInequality>& ineqs, Function func,
	            const map<pair<string, void*>, FiniteVariablesProductLattice*>& divL, 
	            const map<pair<string, void*>, FiniteVariablesProductLattice*>& sgnL);
	
	~ConstrGraph ();
	
	// initializes this Lattice to its default state, if it is not already initialized
	void initialize();
	
	/***** The sets of arrays and scalars visible in the given function *****/
	static map<Function, varIDSet> funcVisibleArrays;
	static map<Function, varIDSet> funcVisibleScalars;
	
	// returns the set of arrays visible in this function
	static varIDSet& getVisibleArrays(Function func);
	
	// returns the set of scalarsvisible in this function
	static varIDSet& getVisibleScalars(Function func);
	
	// for a given scalar returns the corresponding divisibility scalar
	static varID getDivScalar(const varID& scalar);
	
	// returns true if the given variable is a divisibility scalar and false otherwise
	static bool isDivScalar(const varID& scalar);
	
	// Returns a divisibility product lattice that matches the given variable
	FiniteVariablesProductLattice* getDivLattice(const varID& var);
	
	// Returns a sign product lattice that matches the given variable
	FiniteVariablesProductLattice* getSgnLattice(const varID& var);

	
	// returns whether the given variable is known in this constraint graph to be an array
	bool isArray(const varID& array) const;
	
	// Adds the given variable to the scalars list, returning true if this causes
	// the constraint graph to change and false otherwise.
	bool addScalar(const varID& scalar);
	
	// Removes the given variable and its divisibility scalar (if one exists) from the scalars list
	// and removes any constraints that involve them. 
	// Returning true if this causes the constraint graph to change and false otherwise.
	bool removeScalar(const varID& scalar);
	
	// Returns a reference to the constraint graph's set of scalars
	const varIDSet& getScalars() const;
	
	// Returns a modifiable reference to the constraint graph's set of scalars
	varIDSet& getScalarsMod();
	
	// For each scalar variable not in noExternal create another variable that is identical except that it has a special
	//    annotation that identifies it as the external view onto the value of this variable. The newly 
	//    generated external variables will be set to be equal to their original counterparts
	//    and will default to have no relations with any other variables, even through transitive closures. 
	//    These variables are used to transfer information about variable state from one constraint graph
	//    to another. Specially annotated cersions of the external variables will exist in multiple constraint 
	//    graphs and will periodically be updated in other graphs from this graph based on how the relationships 
	//    between the external variables and their regular counterparts change in this graph.
	// If the external variables already exist, their relationships relative to their original counterparts are reset.
	// Returns true if this causes the constraint graph to change, false otherwise.
	bool addScalarExternals(varIDSet noExternal);
	
	// Looks over all the external versions of all scalars in this constraint graph and looks for all the same variables
	//    in the tgtCFG constraint graph that also have the given annotation. Then, updates the variables in that 
	//    from the current relationships in this between the original scalars and their external versions. 
	//    Thus, if blah <= var' in that and var' = var + 5 in this (var' is the annotated copy of var) then we'll update 
	//    to blah <= var' - 5 so that now blah is related to var's current value in this constraint graph. 
	// Returns true if this causes the tgtCG constraint graph to change, false otherwise.
	bool updateFromScalarExternals(ConstrGraph* tgtCG, string annotName, void* annot);
	
	// Returns a reference to the constraint graph's set of externals
	const varIDSet& getExternals() const;
	
	/***** Copying *****/
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);

	// returns a copy of this lattice
	Lattice* copy() const;
	
	// returns a copy of this LogicalCond object
	LogicalCond* copy();
	
	// Copies the state of cg to this constraint graph
	// Returns true if this causes this constraint graph's state to change
	bool copyFrom(ConstrGraph &that);
	
	// Update the state of this constraint graph from that constraint graph, leaving 
	//    this graph's original contents alone and only modifying the pairs that are in that.
	// Returns true if this causes this constraint graph's state to change
	bool updateFrom(ConstrGraph &that);
	
	// Copies the given var and its associated constrants from that to this.
	// Returns true if this causes this constraint graph's state to change; false otherwise.
	bool copyVar(const ConstrGraph& that, const varID& var);
	
protected:
	/*// returns true if this and cg map the same sets of arrays and false otherwise
	bool mapsSameArrays(ConstrGraph *cg);

	// determines whether cg1->arrays contains cg2->arrays
	static bool containsArraySet(ConstrGraph *cg1, ConstrGraph *cg2);
	
	// determines whether cg->arrays and cg->emptyRange are different 
	// from arrays and emptyRange
	bool diffArrays(ConstrGraph *cg);*/
	
	// copies the data of cg->emptyRange to emptyRange and 
	// returns whether this caused emptyRange to change
	bool copyArrays(const ConstrGraph &that);
	
	// updates the data of cg->emptyRange to emptyRange, leaving 
	//    this graph's original emptyRange alone and only modifying the entries that are in that.
	// returns whether this caused emptyRange to change
	bool updateArrays(const ConstrGraph &that);
	
	// determines whether constraints in cg are different from
	// the constraints in this
	bool diffConstraints(ConstrGraph &that);
	
	// copies the constraints of cg into this constraint graph
	// returns true if this causes this constraint graph's state to change
	bool copyConstraints(ConstrGraph &that);
	
	/***** Array Range Management *****/
	// sets the ranges of all arrays in this constraint graph to not be empty
	void unEmptyArrayRanges();
	
	// sets the ranges of all arrays in this constraint graph to empty
	// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
	//              true (=false) or to not bother checking with isBottom (=true)
	void emptyArrayRanges(bool noBottomCheck=false);
	
	/**** Erasing ****/
	// erases all constraints from this constraint graph
	// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
	//              true (=false) or to not bother checking with isBottom (=true)
	void eraseConstraints(bool noBottomCheck=false);
	
public:
	// erases all constraints that relate to variable eraseVar and its corresponding divisibility variable 
	// from this constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
	//              true (=false) or to not bother checking with isBottom (=true)
	bool eraseVarConstr(const varID& eraseVar, bool noBottomCheck=false);
	
	// erases all constraints that relate to variable eraseVar but not its divisibility variable from 
	// this constraint graph
	// Returns true if this causes the constraint graph to change and false otherwise
	// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
	//              true (=false) or to not bother checking with isBottom (=true)
	bool eraseVarConstrNoDiv(const varID& eraseVar, bool noBottomCheck=false);
	
	// erases all constraints between eraseVar and scalars this constraint graph but leave the constraints 
	// that relate to its divisibility variable alone
	// Returns true if this causes the constraint graph to change and false otherwise
	// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
	//              true (=false) or to not bother checking with isBottom (=true)
	bool eraseVarConstrNoDivScalars(const varID& eraseVar, bool noBottomCheck=false);
	
	// erases the ranges of all array variables
	void eraseAllArrayRanges();
	
	// Removes any constraints between the given pair of variables
	// Returns true if this causes the constraint graph to change and false otherwise
	bool disconnectVars(const varID& x, const varID& y);
	
	// Replaces all instances of origVar with newVar
	// Returns true if this causes the constraint graph to change and false otherwise
	// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
	//              true (=false) or to not bother checking with isBottom (=true)
	bool replaceVar(const varID& origVar, const varID& newVar, bool noBottomCheck=false);
	
	protected:
	// Used by copyAnnotVars() and mergeAnnotVars() to identify variables that are interesting
	// from their perspective.
	bool annotInterestingVar(const varID& var, const set<pair<string, void*> >& noCopyAnnots, const set<varID>& noCopyVars,
                            const string& annotName, void* annotVal);
	
	public: 
	// Copies the constrains on all the variables that have the given annotation (srcAnnotName -> srcAnnotVal).
	// For each such variable we create a copy variable that is identical except that the
	//    (srcAnnotName -> srcAnnotVal) annotation is replaced with the (tgtAnnotName -> tgtAnnotVal) annotation.
	// If two variables match the (srcAnnotName -> srcAnnotVal) annotation and the constraint graph has a relation
	//    between them, their copies will have the same relation between each other but will have no relation to the
	//    original variables. If two variables have a relation and only one is copied, then the copy maintains the 
	//    original relation to the non-copied variable.
	// A variable matches the given (srcAnnotName -> srcAnnotVal) annotation if this is one of the variable's annotations
	//    or if srcAnnotName=="" and the variable has no annotations.
	// Avoids copying variables with annotations in the noCopyAnnots set and variables in the noCopyVars set.
	// Returns true if this causes the constraint graph to change and false otherwise.
	bool copyAnnotVars(string srcAnnotName, void* srcAnnotVal, 
	                   string tgtAnnotName, void* tgtAnnotVal,
	                   const set<pair<string, void*> >& noCopyAnnots,
	                   const set<varID>& noCopyVars);
	
	// Merges the state of the variables in the constraint graph with the [finalAnnotName -> finalAnnotVal] annotation
	//    with the state of the variables with the [remAnnotName -> remAnnotVal]. Each constraint that involves a variable
	//    with the former annotation and the same variable with the latter annotation is replaced with the union of the 
	//    two constraints and will only involve the variable with the [finalAnnotName -> finalAnnotVal] (latter) annotation.
	// All variables with the [remAnnotName -> remAnnotVal] annotation are removed from the constraint graph.
	// A variable matches the given (srcAnnotName -> srcAnnotVal) annotation if this is one of the variable's annotations
	//    or if srcAnnotName=="" and the variable has no annotations.
	// Avoids merging variables with annotations in the noCopyAnnots set and variables in the noCopyVars set.
	// Returns true if this causes the constraint graph to change and false otherwise.
	// It is assumed that variables that match [finalAnnotName -> finalAnnotVal] differ from variables that match
	//    [remAnnotName -> remAnnotVal] in only that annotation.
	bool mergeAnnotVars(const string& finalAnnotName, void* finalAnnotVal, 
	                    const string& remAnnotName,   void* remAnnotVal,
	                    const set<pair<string, void*> >& noCopyAnnots,
	                    const set<varID>& noCopyVars);
	
	
	protected:
	// Union the current inequality for y in the given subMap of vars2Value with the given affine inequality
	// Returns true if this causes a change in the subMap, false otherwise.
	bool unionXYsubMap(map<varID, affineInequality>& subMap, const varID& y, const affineInequality& ineq);
	
	// Merges the given sub-map of var2Vals, just like mergeAnnotVars. Specifically, for every variable in the subMap
	// that has a [remAnnotName -> remAnnotVal] annotation,
	// If there exists a corresponding variable that has the [finalAnnotName -> finalAnnotVal] annotation, 
	//    their respective inequalities are unioned. This union is left with the latter variable and the former
	//    variable's entry in subMap is removed
	// If one does not exist, we simply replace the variable's record with an identical one that now belongs
	//    to its counterpart with the [finalAnnotName -> finalAnnotVal] annotation.
	// Other entries are left alone.
	// Returns true if this causes the subMap to change, false otherwise.
	bool mergeAnnotVarsSubMap(map<varID, affineInequality>& subMap, 
	                          string finalAnnotName, void* finalAnnotVal, 
	                          string remAnnotName,   void* remAnnotVal,
	                          const set<pair<string, void*> >& noCopyAnnots,
	                          const set<varID>& noCopyVars);
	
	// Support routine for mergeAnnotVars(). Filters out any rem variables in the given set, replacing
	// them with their corresponding final versions if those final versions are not already in the set
	// Returns true if this causes the set to change, false otherwise.
	bool mergeAnnotVarsSet(set<varID> varsSet, 
	                       string finalAnnotName, void* finalAnnotVal, 
	                       string remAnnotName,   void* remAnnotVal,
	                       const set<pair<string, void*> >& noCopyAnnots,
	                       const set<varID>& noCopyVars);
	
	public:
	                    
	// Returns true if the given variable has an annotation in the given set and false otherwise.
	// The variable matches an annotation if its name and value directly match or if the variable
	// has no annotations and the annotation's name is "".
	static bool varHasAnnot(const varID& var, const set<pair<string, void*> >& annots);
	
	// Returns true if the given variable has an annotation in the given set and false otherwise.
	// The variable matches an annotation if its name and value directly match or if the variable
	// has no annotations and the annotName=="".
	static bool varHasAnnot(const varID& var, string annotName, void* annotVal);
	
	// Returns a constraint graph that only includes the constrains in this constraint graph that involve the
	// variables in focusVars and their respective divisibility variables, if any. 
	// It is assumed that focusVars only contains scalars and not array ranges.
	ConstrGraph* getProjection(const varIDSet& focusVars);
	
	// Creates a new constraint graph that is the disjoint union of the two given constraint graphs.
	// The variables in cg1 and cg2 that are not in the noAnnot set, are annotated with cg1Annot and cg2Annot, respectively,
	// under the name annotName.
	// cg1 and cg2 are assumed to have identical constraints between variables in the noAnnotset.
	static ConstrGraph* joinCG(ConstrGraph* cg1, void* cg1Annot, ConstrGraph* cg2, void* cg2Annot, string annotName, const varIDSet& noAnnot);
	
	protected:
	// Copies the per-variable contents of srcCG to tgtCG, while ensuring that in tgtCG all variables that are not
	// in noAnnot are annotated with the annotName->annot label. For variables in noAnnot, the function ensures
	// that tgtCG does not have inconsistent mappings between such variables.
	static void joinCG_copyState(ConstrGraph* tgtCG, ConstrGraph* srcCG, void* annot, string annotName, const varIDSet& noAnnot);
	
	public:
	// Replaces all references to variables with the given annotName->annot annotation to 
	// references to variables without the annotation
	// Returns true if this causes the constraint graph to change and false otherwise
	bool removeVarAnnot(string annotName, void* annot);
	
	// Replaces all references to variables with the given annotName->annot annotation to 
	// references to variables without the annotation
	// Returns true if this causes the constraint graph to change and false otherwise
	bool replaceVarAnnot(string oldAnnotName, void* oldAnnot,
	                     string newAnnotName, void* newAnnot);
	
	// For all variables that have a string (tgtAnnotName -> tgtAnnotVal) annotation 
	//    (or if tgtAnnotName=="" and the variable has no annotation), add the annotation
	//    (newAnnotName -> newAnnotVal).
	// Returns true if this causes the constraint graph to change and false otherwise
	bool addVarAnnot(string tgtAnnotName, void* tgtAnnotVal, string newAnnotName, void* newAnnotVal);
	
	// adds a new range into this constraint graph 
	//void addRange(varID rangeVar);
	
public:
	/**** Transfer Function-Related Updates ****/
	// updates the constraint graph with the information that x*a = y*b+c
	// returns true if this causes the constraint graph to change and false otherwise
	bool assign(const varAffineInequality& cond);
	bool assign(varID x, varID y, const affineInequality& ineq);
	bool assign(varID x, varID y, int a, int b, int c);
	
/*	// Undoes the i = j + c assignment for backwards analysis
	void undoAssignment( quad i, quad j, quad c );*/
	
/*	// kills all links from variable x to every other variable
	void killVariable( quad x );
*/
	
	// Add the condition (x*a <= y*b + c) to this constraint graph. The addition is done via a conjunction operator, 
	// meaning that the resulting graph will be left with either (x*a <= y*b + c) or the original condition, whichever is stronger.
	// returns true if this causes the constraint graph to change and false otherwise
	bool assertCond(const varAffineInequality& cond);
	
	// add the condition (x*a <= y*b + c) to this constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	bool assertCond(const varID& x, const varID& y, const affineInequality& ineq);
	
	// add the condition (x*a <= y*b + c) to this constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	bool assertCond(const varID& x, const varID& y, int a, int b, int c);
	
	// add the condition (x*a = y*b + c) to this constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	bool assertEq(const varAffineInequality& cond);
	bool assertEq(varID x, varID y, const affineInequality& ineq);
	bool assertEq(const varID& x, const varID& y, int a=1, int b=1, int c=0);
	
	// include information from prior passes (initAffineIneqs or DivAnalysis) into this constraint graph
	//void includePriorPassInfo(DataflowNode n);
	
	// Cuts i*b+c from the given array's range if i*b+c is either at the very bottom
	//    or very bottom of its range. In other words, if (i*b+c)'s range overlaps the  
	//    array's range on one of its edges, the array's range is reduced by 1 on 
	//    that edge
	// returns true if this causes the constraint graph to change and false otherwise
	bool shortenArrayRange(varID array, varID i, int b, int c);
	
	/**** Dataflow Functions ****/
	
	// returns the sign of the given variable
	affineInequality::signs getVarSign(const varID& var);
		
	// returns true of the given variable is =0 and false otherwise
	bool isEqZero(const varID& var);
	
	// Returns true if v1*a = v2*b + c and false otherwise
	bool eqVars(const varID& v1, const varID& v2, int a=1, int b=1, int c=0);
	
	// If v1*a = v2*b + c, sets a, b and c appropriately and returns true. 
	// Otherwise, returns false.
	bool isEqVars(const varID& v1, const varID& v2, int& a, int& b, int& c);
	
	// Returns a list of variables that are equal to var in this constraint graph as a list of pairs
	// <x, ineq>, where var*ineq.getA() = x*ineq.getB() + ineq.getC()
	map<varID, affineInequality> getEqVars(varID var);
	
	// Returns true if v1*a <= v2*b + c and false otherwise
	bool lteVars(const varID& v1, const varID& v2, int a=1, int b=1, int c=0);
	
	// Returns true if v1*a < v2*b + c and false otherwise
	bool ltVars(const varID& v1, const varID& v2, int a=1, int b=1, int c=0);
	
	// Class used to iterate over all the constraints x*a <= y*b + c for a given variable x
	class leIterator
	{
		varID x;
		const ConstrGraph* parent;
		map<varID, map<varID, affineInequality> >::const_iterator curX;
		map<varID, affineInequality>::const_iterator curY;

		public:
		leIterator(const ConstrGraph* parent,
		           const map<varID, map<varID, affineInequality> >::iterator& curX);
			
		leIterator(const ConstrGraph* parent,
		           const varID& x);
		
		bool isDone() const;
		
		varAffineInequality operator*() const ;
		
		void operator ++ ();
		void operator ++ (int);
		
		bool operator==(const leIterator& otherIt) const;
		bool operator!=(const leIterator& otherIt) const;
	};
	// Beginning and end points of the iteration over all constraints x*a <= y*b + c for a 
	// given variable x.
	leIterator leBegin(const varID& y);
	leIterator leEnd();
	
	// Class used to iterate over all the constraints x*a <= y*b + c for a given variable y
	class geIterator
	{
		bool isEnd; // true if this is the end iterator
		map<varID, map<varID, affineInequality> >::const_iterator curX;
		map<varID, affineInequality>::const_iterator curY;
		const ConstrGraph* parent;
		const varID y;
		
		public:
		geIterator();
		
		geIterator(const ConstrGraph* parent, const varID& y);
		
		geIterator(const ConstrGraph* parent, const varID& y,
		           const map<varID, map<varID, affineInequality> >::iterator& curX,
		           const map<varID, affineInequality>::iterator& curY);
		
		// Advances curX and curY by one step. Returns false if curX/curY is already at the
		// end of parent.vars2Value and true otherwise (i.e. successful step).
		bool step();
		
		// Move curX/curY to the next x/y pair with a matching y (may leave curX/curY already satisfy this).
		// Returns true if there are no more such pairs.
		bool advance();
		
		bool isDone() const;
		
		const varID& getX() const;
		
		varAffineInequality operator*() const ;
		
		void operator ++ ();
		void operator ++ (int);
		
		bool operator==(const geIterator& otherIt) const;
		bool operator!=(const geIterator& otherIt) const;
	};
	// Beginning and End points of the iteration over all constraints x*a <= y*b + c for a 
	// given variable y.
	geIterator geBegin(const varID& y);
	geIterator geEnd();
	
	// widens this from that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool widenUpdate(InfiniteLattice* that);
	
	// Widens this from that and saves the result in this, while ensuring that if a given constraint
	// doesn't exist in that, its counterpart in this is not modified
	// returns true if this causes this to change and false otherwise
	bool widenUpdateLimitToThat(InfiniteLattice* that);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	// The meet is the intersection of constraints: the set of constraints 
	//    that is common to both constraint graphs. Thus, the result is the loosest
	//    set of constraints that satisfies both sets and therefore also the information union.
	bool meetUpdate(Lattice* that);
	
	// Meet this and that and saves the result in this, while ensuring that if a given constraint
	// doesn't exist in that, its counterpart in this is not modified
	// returns true if this causes this to change and false otherwise
	bool meetUpdateLimitToThat(InfiniteLattice* that);
		
	// Unified function for meet and widening
	// if meet == true, this function computes the meet and if =false, computes the widening
	// if limitToThat == true, if a given constraint does not exist in that, this has no effect on the meet/widening
	bool meetwidenUpdate(ConstrGraph* that, bool meet, bool limitToThat);
	
	// <from LogicalCond>
	bool andUpd(LogicalCond& that);
	
	bool andUpd(ConstrGraph* that);
	
	// <from LogicalCond>
	bool orUpd(LogicalCond& that);
	
	
	// computes the transitive closure of this constraint graph, 
	// returns 0 if the resulting graph is Bottom
	// returns -1 if the resulting graph is Dead
	void transitiveClosure();
	
	// computes the transitive closure of the given constraint graph,
	// focusing on the constraints of scalars that have divisibility variables
	// we only bother propagating constraints to each such variable through its divisibility variable
	void divVarsClosure();
	
	// The portion of divVarsClosure that is called for every y variable. Thus, given x and x' (x's divisibility variable)
	// divVarsClosure_perY() is called for every scalar or array y to infer the x->y connection thru x->x'->y and
	// infer the y->x connection thru x->x'->x
	bool divVarsClosure_perY(const varID& x, const varID& divX, const varID& y, 
	                         affineInequality* constrXDivX, affineInequality* constrDivXX/*,
	                         affineInequality::signs xSign, affineInequality::signs ySign*/);
	
	// computes the transitive closure of this constraint graph while modifying 
	// only the constraints that involve the given variable
	void localTransClosure(const varID& tgtVar);
		
protected:
	// searches this constraint graph for cycles. 
	// If it finds a negative cycle that does not go through an array variable, it records this 
	//    fact and returns 0 (Bottom).
	// Otherwise, it returns 1.
	// If isFeasible() finds any negative cycles, it updates the state of this constraint graph
	//    accordingly, either setting it to bottom or recording that the range of an array is empty.
	// !checking for feasibility does makes an uninitialized graph into an initialized one!
	int isFeasible();
	
public:
	
	// Creates a divisibility variable for the given variable and adds it to the constraint graph
	// If var = r (mod d), then the relationship between x and x' (the divisibility variable)
	// will be x = x'*d + r
	// returns true if this causes the constraint graph to be modified (it may not if this 
	//    information is already in the graph) and false otherwise
	bool addDivVar(varID var/*, int div, int rem*/, bool killDivVar=false);
	
	// Disconnect this variable from all other variables except its divisibility variable. This is done 
	// in order to compute the original variable's relationships while taking its divisibility information 
	// into account.
	// Returns true if this causes the constraint graph to be modified and false otherwise
	bool disconnectDivOrigVar(varID var/*, int div, int rem*/);
	
	// Adds a new divisibility lattice, with the associated anotation
	// Returns true if this causes the constraint graph to be modified and false otherwise
	bool addDivL(FiniteVariablesProductLattice* divLattice, string annotName, void* annot);
	
	// Adds a new sign lattice, with the associated anotation
	// Returns true if this causes the constraint graph to be modified and false otherwise
	bool addSgnL(FiniteVariablesProductLattice* sgnLattice, string annotName, void* annot);
	
	/**** State Accessor Functions *****/
	// Returns true if this constraint graph includes constraints for the given variable
	// and false otherwise
	bool containsVar(const varID& var);
	
	// returns the x->y constraint in this constraint graph
	affineInequality* getVal(varID x, varID y);
	
	// set the x->y connection in this constraint graph to c
	// return true if this results this ConstrGraph being changed, false otherwise
	// xSign, ySign: the default signs for x and y. If they're set to unknown, setVal computes them on its own using getVarSign.
	//     otherwise, it uses the given signs 
	bool setVal(varID x, varID y, int a, int b, int c, 
	            affineInequality::signs xSign=affineInequality::unknownSgn, affineInequality::signs ySign=affineInequality::unknownSgn);
	
	bool setVal(varID x, varID y, const affineInequality& ineq);
	
	// Sets the state of this constraint graph to Uninitialized, without modifying its contents. Thus, 
	//    the graph will register as uninitalized but when it is next used, its state will already be set up.
	// Returns true if this causes the constraint graph to be modified and false otherwise.
	bool setToUninitialized();
	
	// sets the state of this constraint graph to Bottom
	// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
	//              true (=false) or to not bother checking with isBottom (=true)
	void setToBottom(bool noBottomCheck=false);
	
	// Sets the state of this constraint graph to top 
	// If onlyIfNotInit=true, this is only done if the graph is currently uninitialized
	void setToTop(bool onlyIfNotInit=false);
	
	// returns whether the range of the given array is empty
	bool isEmptyRange(varID array);
	
	// returns whether this constraint graph is bottom
	bool isBottom();
	
	/**** String Output *****/
	
	// Returns the string representation of the constraints held by this constraint graph, 
	//    with a line for each pair of variables for which the constraint is < bottom. It also prints
	//    the names of all the arrays that have empty ranges in this constraint graph
	// There is no \n on the last line of output, even if it is a multi-line string
	string str(string indent="");
	
//protected:
	// Returns the string representation of the constraints held by this constraint graph, 
	//    with a line for each pair of variables for which the constraint is < bottom. It also prints
	//    the names of all the arrays that have empty ranges in this constraint graph
	// There is no \n on the last line of output, even if it is a multi-line string
	// If useIsBottom=true, isBottom() is used to determine whether the graph is =bottom.
	// Otherwise, the bottom variable is checked.
	// If useIsBottom=true, isBottom() is used to determine whether the graph is =bottom.
	// Otherwise, the bottom variable is checked.
	string str(string indent, bool useIsBottom);
	
public:
	/**** Comparison Functions ****/	
	bool operator != (ConstrGraph &that);
	bool operator == (ConstrGraph &that);
	bool operator == (Lattice* that);
	bool operator <<= (ConstrGraph &that);

	
	// Returns true if x*b+c MUST be outside the range of y and false otherwise. 
	// If two variables are unrelated, it is assumed that there is no information 
	// about their relationship and mustOutsideRange() thus proceeds conservatively (returns true).
	bool mustOutsideRange(varID x, int b, int c, varID y);
	
	// returns true if this logical condition must be true and false otherwise
	// <from LogicalCond>
	bool mayTrue();
		
/*	// returns true if x+c MUST be inside the range of y and false otherwise
	// If two variables are unrelated, it is assumed that there is no information 
	// about their relationship and mustInsideRange() thus proceeds conservatively.
	bool mustInsideRange(varID x, int b, int c, varID y);*/
	
	/* Transactions */
	void beginTransaction();
	void endTransaction();
};



#endif

