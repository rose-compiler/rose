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
// GB : 2011-03-05 (Removing Sign Lattice Dependence)#include "sgnAnalysis.h"
#include "liveDeadVarAnalysis.h"

// top - relations between each pair of variables are unknown or too complex to be representable as affine inequalities (minimal information)
// intermediate - some concrete information is known about some variable pairs
// bottom - impossible situation (maximal information) (bottom flag = true)

// By default the constraint graph is = top. Since this implies a top inequality between every pair, we don't 
// actually maintain such affineInequality objects. Instead, if there is no affineInequality between a pair of
// variables, this itself implies that this affineInequality=top.
class ConstrGraph : public virtual InfiniteLattice//, public virtual LogicalCond
{
public:
	// Possible levels of this constraint graph, defined by their information content in ascending order.
	typedef enum levels {
		// Uninitialized constraint graph. Uninitialized constraint graphs behave
		//    just like regular constraint graphs but they are not equal to any other graph
		//    until they are initialized. Any operation that modifies or reads the state  
		//    of a constraint graph (not including comparisons or other operations that don't
		//    access individual variable mappings) causes it to become initialized (if it 
		//    wasn't already). An uninitialized constraint graph is !=bottom. 
		//    printing a graph's contents does not make it initialized.
		uninitialized = 0,
		// Constraint graph that has no constraints
		bottom,
		// This graph's constraints are defined as a conjunction or disjunction of inequalities.
		// More details are provided in constrType field
		constrKnown,
		// The set of constraints in this graph are too complex to be described as a conjunction of inequalities or
		// a negation of such a conjunction 
		top};
protected:
	levels level;
	
public:
	typedef enum {
		unknown,
		// This graph's constraints are represented as a conjunction of inequalities.
		conj,
		// Constraints are representes as the negation of a conjunction of inequalities. 
		// This is the same as a disjunction of the negations of the same inequalities.
		negConj,
		// This graph's constrants are mutually-inconsistent
		inconsistent
	} constrTypes;
protected:
	constrTypes constrType;
	
	// The function and DataflowNode that this constraint graph corresponds to
	// as well as the node's state
	const Function* func;
	const DataflowNode* n;
	const NodeState* state;
	
	// Represents constrants (x<=y+c). vars2Value[x] maps to a set of constraint::<y, a, b, c>
	//map<varID, map<varID, constraint> > vars2Value;
	map<varID, map<varID, affineInequality> > vars2Value;
	
	// The LiveDeadVarsAnalysis that identifies the live/dead state of all application variables.
	// Needed to create a FiniteVarsExprsProductLattice.
	LiveDeadVarsAnalysis* ldva;
		
	// To allow the user to modify the graph in several spots before calling isSelfConsistent()
	// we allow them to perform their modifications inside a transaction and call isSelfConsistent only
	// at the end of the transaction
	bool inTransaction;
	
	// The divisibility lattices associated with the current CFG node
	// divL is a map from annotations to product lattices. Each product lattice will only be used to
	//    reason about variables that have the same annotation. When a variable has multiple annotations
	//    only one matching product lattice will be used.
	// The annotation ""->NULL matches all variables
	map<pair<string, void*>, FiniteVarsExprsProductLattice*> divL;
	
	// The sign lattices associated with the current CFG node
	// sgnL is a map from annotations to product lattices. Each product lattice will only be used to
	//   reason about variables that have the same annotation. When a variable has multiple annotations
	//   only one matching product lattice will be used.
	// The annotation ""->NULL matches all variables
	// GB : 2011-03-05 (Removing Sign Lattice Dependence) map<pair<string, void*>, FiniteVarsExprsProductLattice*> sgnL;
	
	// Set of live variables that this constraint graph applies to
	set<varID> vars;
	
	// set of variables for which we have divisibility information
	set<varID> divVars;
	
	// Flag indicating whether some of the constraints have changed since the last time
	// this graph was checked for bottom-ness
	bool constrChanged;
	// Set of variables the for which we've added constraints since the last transitive closure
	set<varID> newConstrVars;
	// Set of variables the constraints on which have been modified since the last transitive closure
	set<varID> modifiedVars;
	

	/**** Constructors & Destructors ****/
	//ConstrGraph(bool initialized=true, string indent="");
	
protected:
	ConstrGraph(const Function& func, const DataflowNode& n, const NodeState& state, bool initialized=false, string indent="");
	
public:	
	ConstrGraph(const Function& func, const DataflowNode& n, const NodeState& state, 
	            LiveDeadVarsAnalysis* ldva, FiniteVarsExprsProductLattice* divL, // GB : 2011-03-05 (Removing Sign Lattice Dependence) FiniteVarsExprsProductLattice* sgnL, 
	            bool initialized=true, string indent="");
	ConstrGraph(const Function& func, const DataflowNode& n, const NodeState& state, 
	            LiveDeadVarsAnalysis* ldva, 
	            const map<pair<string, void*>, FiniteVarsExprsProductLattice*>& divL, 
	            // GB : 2011-03-05 (Removing Sign Lattice Dependence)const map<pair<string, void*>, FiniteVarsExprsProductLattice*>& sgnL, 
	            bool initialized=true, string indent="");
	
	//ConstrGraph(const varIDSet& scalars, const varIDSet& arrays, bool initialized=true);
	
	//ConstrGraph(varIDSet& arrays, varIDSet& scalars, FiniteVarsExprsProductLattice* divL, bool initialized=true);
	
	ConstrGraph(ConstrGraph &that, bool initialized=true, string indent="");
	
	ConstrGraph(const ConstrGraph* that, bool initialized=true, string indent="");
	
	// Creates a constraint graph that contains the given set of inequalities, 
	//// which are assumed to correspond to just scalars
	ConstrGraph(const set<varAffineInequality>& ineqs, const Function& func, const DataflowNode& n, const NodeState& state,
	            LiveDeadVarsAnalysis* ldva, FiniteVarsExprsProductLattice* divL, 
	            // GB : 2011-03-05 (Removing Sign Lattice Dependence)FiniteVarsExprsProductLattice* sgnL, 
	            string indent="");
	ConstrGraph(const set<varAffineInequality>& ineqs, const Function& func, const DataflowNode& n, const NodeState& state,
	            LiveDeadVarsAnalysis* ldva, 
	            const map<pair<string, void*>, FiniteVarsExprsProductLattice*>& divL, 
	            // GB : 2011-03-05 (Removing Sign Lattice Dependence)const map<pair<string, void*>, FiniteVarsExprsProductLattice*>& sgnL, 
	            string indent="");
	
	protected:
	// Initialization code that is common to multiple constructors
	void initCG(const Function* func, const DataflowNode* n, const NodeState* state, 
	            bool initialized, string indent="");
	
	public:
	~ConstrGraph ();
	
	// Initializes this Lattice to its default state, if it is not already initialized
	void initialize(string indent="");
	void initialize()
	{ initialize(""); }
	
	// For a given variable returns the corresponding divisibility variable
	static varID getDivVar(const varID& scalar);
	
	// Returns true if the given variable is a divisibility variable and false otherwise
	static bool isDivVar(const varID& scalar);
	
	// Returns a divisibility product lattice that matches the given variable
	FiniteVarsExprsProductLattice* getDivLattice(const varID& var, string indent="");
	
	string DivLattices2Str(string indent="");
	
	// Returns a sign product lattice that matches the given variable
	// GB : 2011-03-05 (Removing Sign Lattice Dependence)
	// FiniteVarsExprsProductLattice* getSgnLattice(const varID& var, string indent="");

	// Adds the given variable to the variables list, returning true if this causes
	// the constraint graph to change and false otherwise.
	bool addVar(const varID& scalar, string indent="");
	
	// Removes the given variable and its divisibility variables (if one exists) from the variables list
	// and removes any constraints that involve them. 
	// Returning true if this causes the constraint graph to change and false otherwise.
	bool removeVar(const varID& scalar, string indent="");
	
	// Returns a reference to the constraint graph's set of variables
	const varIDSet& getVars() const;
	
	// Returns a modifiable reference to the constraint graph's set of variables
	varIDSet& getVarsMod();
	
	/***** Copying *****/
	
	// Overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);

	// Returns a copy of this lattice
	Lattice* copy() const;
	
	// Returns a copy of this LogicalCond object
	//LogicalCond* copy();
	
	// Copies the state of that to this constraint graph
	// Returns true if this causes this constraint graph's state to change
	bool copyFrom(ConstrGraph &that, string indent="");
	
	// Copies the state of That into This constraint graph, but mapping constraints of varFrom to varTo, even
	//    if varFrom is not mapped by This and is only mapped by That. varTo must be mapped by This.
	// Returns true if this causes this constraint graph's state to change.
	bool copyFromReplace(ConstrGraph &that, varID varTo, varID varFrom, string indent="");
	
	// Copies the given var and its associated constrants from that to this.
	// Returns true if this causes this constraint graph's state to change; false otherwise.
	bool copyVar(const ConstrGraph& that, const varID& var);
	
protected:
	// Determines whether constraints in cg are different from
	// the constraints in this
	bool diffConstraints(ConstrGraph &that, string indent="");
	
public:
	// Copies the constraints of cg into this constraint graph.
	// Returns true if this causes this constraint graph's state to change.
	bool copyConstraints(ConstrGraph &that, string indent="");
	
	// Copies the constraints of cg associated with varFrom into this constraint graph, 
	//    but mapping them instead to varTo.
	// Returns true if this causes this constraint graph's state to change.
	bool copyConstraintsReplace(ConstrGraph &that, varID varTo, varID varFrom, string indent="");
	
	/**** Erasing ****/
	// erases all constraints from this constraint graph
	// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
	//              true (=false) or to not bother checking with isBottom (=true)
	void eraseConstraints(bool noBottomCheck=false, string indent="");
	
public:
	// Erases all constraints that relate to variable eraseVar and its corresponding divisibility variable 
	// from this constraint graph
	// Returns true if this causes the constraint graph to change and false otherwise
	// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
	// 							or to not bother checking self-consistency and just return the last-known value (=true)
	bool eraseVarConstr(const varID& eraseVar, bool noConsistencyCheck=false, string indent="");
	
	// Erases all constraints that relate to variable eraseVar but not its divisibility variable from 
	//    this constraint graph
	// Returns true if this causes the constraint graph to change and false otherwise
	// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
	// 							or to not bother checking self-consistency and just return the last-known value (=true)
	bool eraseVarConstrNoDiv(const varID& eraseVar, bool noConsistencyCheck=false, string indent="");
	
	// Erases all constraints between eraseVar and scalars in this constraint graph but leave the constraints 
	//    that relate to its divisibility variable alone
	// Returns true if this causes the constraint graph to change and false otherwise
	// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
	// 							or to not bother checking self-consistency and just return the last-known value (=true)
	bool eraseVarConstrNoDivVars(const varID& eraseVar, bool noConsistencyCheck=false, string indent="");
	
	// Removes any constraints between the given pair of variables
	// Returns true if this causes the constraint graph to change and false otherwise
	//bool disconnectVars(const varID& x, const varID& y);
	
	// Replaces all instances of origVar with newVar. Both are assumed to be scalars.
	// Returns true if this causes the constraint graph to change and false otherwise
	// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
	// 							or to not bother checking self-consistency and just return the last-known value (=true)
	bool replaceVar(const varID& origVar, const varID& newVar, bool noConsistencyCheck=false, string indent="");
	
	protected:
	// Used by copyAnnotVars() and mergeAnnotVars() to identify variables that are interesting
	// from their perspective.
	bool annotInterestingVar(const varID& var, const set<pair<string, void*> >& noCopyAnnots, const set<varID>& noCopyVars,
                            const string& annotName, void* annotVal, string indent="");
	
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
	                   const set<varID>& noCopyVars, string indent="");
	
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
	                    const set<varID>& noCopyVars, string indent);
	
	
	protected:
	// Union the current inequality for y in the given subMap of vars2Value with the given affine inequality
	// Returns true if this causes a change in the subMap, false otherwise.
	bool unionXYsubMap(map<varID, affineInequality>& subMap, const varID& y, const affineInequality& ineq, string indent="");
	
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
	                          const set<varID>& noCopyVars, string indent="");
	
	// Support routine for mergeAnnotVars(). Filters out any rem variables in the given set, replacing
	// them with their corresponding final versions if those final versions are not already in the set
	// Returns true if this causes the set to change, false otherwise.
	bool mergeAnnotVarsSet(set<varID> varsSet, 
	                       string finalAnnotName, void* finalAnnotVal, 
	                       string remAnnotName,   void* remAnnotVal,
	                       const set<pair<string, void*> >& noCopyAnnots,
	                       const set<varID>& noCopyVars, string indent="");
	
	public:
	                    
	// Returns true if the given variable has an annotation in the given set and false otherwise.
	// The variable matches an annotation if its name and value directly match or if the variable
	// has no annotations and the annotation's name is "".
	static bool varHasAnnot(const varID& var, const set<pair<string, void*> >& annots, string indent="");
	
	// Returns true if the given variable has an annotation in the given set and false otherwise.
	// The variable matches an annotation if its name and value directly match or if the variable
	// has no annotations and the annotName=="".
	static bool varHasAnnot(const varID& var, string annotName, void* annotVal, string indent="");
	
	// Returns a constraint graph that only includes the constrains in this constraint graph that involve the
	// variables in focusVars and their respective divisibility variables, if any. 
	// It is assumed that focusVars only contains scalars and not array ranges.
	ConstrGraph* getProjection(const varIDSet& focusVars, string indent="");
	
	// Creates a new constraint graph that is the disjoint union of the two given constraint graphs.
	// The variables in cg1 and cg2 that are not in the noAnnot set, are annotated with cg1Annot and cg2Annot, respectively,
	// under the name annotName.
	// cg1 and cg2 are assumed to have identical constraints between variables in the noAnnotset.
	static ConstrGraph* joinCG(ConstrGraph* cg1, void* cg1Annot, ConstrGraph* cg2, void* cg2Annot, 
	                           string annotName, const varIDSet& noAnnot, string indent="");
	
	protected:
	// Copies the per-variable contents of srcCG to tgtCG, while ensuring that in tgtCG all variables that are not
	// in noAnnot are annotated with the annotName->annot label. For variables in noAnnot, the function ensures
	// that tgtCG does not have inconsistent mappings between such variables.
	static void joinCG_copyState(ConstrGraph* tgtCG, ConstrGraph* srcCG, void* annot, 
	                             string annotName, const varIDSet& noAnnot, string indent="");
	
	public:
	// Replaces all references to variables with the given annotName->annot annotation to 
	// references to variables without the annotation
	// Returns true if this causes the constraint graph to change and false otherwise
	bool removeVarAnnot(string annotName, void* annot, string indent="");
	
	// Replaces all references to variables with the given annotName->annot annotation to 
	// references to variables without the annotation
	// Returns true if this causes the constraint graph to change and false otherwise
	bool replaceVarAnnot(string oldAnnotName, void* oldAnnot,
	                     string newAnnotName, void* newAnnot, string indent="");
	
	// For all variables that have a string (tgtAnnotName -> tgtAnnotVal) annotation 
	//    (or if tgtAnnotName=="" and the variable has no annotation), add the annotation
	//    (newAnnotName -> newAnnotVal).
	// Returns true if this causes the constraint graph to change and false otherwise
	bool addVarAnnot(string tgtAnnotName, void* tgtAnnotVal, string newAnnotName, void* newAnnotVal, string indent="");
	
	// adds a new range into this constraint graph 
	//void addRange(varID rangeVar);
	
public:
	/**** Transfer Function-Related Updates ****/
	// Negates the constraint graph.
	// Returns true if this causes the constraint graph to change and false otherwise
	bool negate(string indent="");
	
	// Updates the constraint graph with the information that x*a = y*b+c
	// Returns true if this causes the constraint graph to change and false otherwise
	bool assign(const varAffineInequality& cond, string indent="");
	bool assign(varID x, varID y, const affineInequality& ineq, string indent="");
	bool assign(varID x, varID y, int a, int b, int c, string indent="");
	
	// Updates the constraint graph to record that there are no constraints in the given variable.
	// Returns true if this causes the constraint graph to change and false otherwise
	bool assignBot(varID var, string indent="");
	
	// Updates the constraint graph to record that the constraints between the given variable and
	//    other variables are Top.
	// Returns true if this causes the constraint graph to change and false otherwise
	bool assignTop(varID var, string indent="");
	
/*	// Undoes the i = j + c assignment for backwards analysis
	void undoAssignment( quad i, quad j, quad c );*/
	
/*	// kills all links from variable x to every other variable
	void killVariable( quad x );
*/
	
	// Add the condition (x*a <= y*b + c) to this constraint graph. The addition is done via a conjunction operator, 
	// meaning that the resulting graph will be left with either (x*a <= y*b + c) or the original condition, whichever is stronger.
	// returns true if this causes the constraint graph to change and false otherwise
	bool assertCond(const varAffineInequality& cond, string indent="");
	
	// add the condition (x*a <= y*b + c) to this constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	bool assertCond(const varID& x, const varID& y, const affineInequality& ineq, string indent="");
	
	// add the condition (x*a <= y*b + c) to this constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	bool assertCond(const varID& x, const varID& y, int a, int b, int c, string indent="");
	
	// add the condition (x*a = y*b + c) to this constraint graph
	// returns true if this causes the constraint graph to change and false otherwise
	bool assertEq(const varAffineInequality& cond, string indent="");
	bool assertEq(varID x, varID y, const affineInequality& ineq, string indent="");
	bool assertEq(const varID& x, const varID& y, int a=1, int b=1, int c=0, string indent="");
		
	/**** Dataflow Functions ****/
	
	// returns the sign of the given variable
	affineInequality::signs getVarSign(const varID& var, string indent="");
		
	// returns true of the given variable is =0 and false otherwise
	bool isEqZero(const varID& var, string indent="");
	
	// Returns true if v1*a = v2*b + c and false otherwise
	bool eqVars(const varID& v1, const varID& v2, int a=1, int b=1, int c=0, string indent="");
	bool eqVars(const varID& v1, const varID& v2, string indent="")
	{ return eqVars(v1, v2, 1, 1, 0, indent); }
	
	// If v1*a = v2*b + c, sets a, b and c appropriately and returns true. 
	// Otherwise, returns false.
	bool isEqVars(const varID& v1, const varID& v2, int& a, int& b, int& c, string indent="");
	
	// Returns a list of variables that are equal to var in this constraint graph as a list of pairs
	// <x, ineq>, where var*ineq.getA() = x*ineq.getB() + ineq.getC()
	map<varID, affineInequality> getEqVars(varID var, string indent="");
	
	// Returns true if v1*a <= v2*b + c and false otherwise
	bool lteVars(const varID& v1, const varID& v2, int a=1, int b=1, int c=0, string indent="");
	
	// Returns true if v1*a < v2*b + c and false otherwise
	bool ltVars(const varID& v1, const varID& v2, int a=1, int b=1, int c=0, string indent="");
	
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
	bool widenUpdate(InfiniteLattice* that, string indent="");
	bool widenUpdate(InfiniteLattice* that) { return widenUpdate(that, ""); }
	
	// Widens this from that and saves the result in this, while ensuring that if a given constraint
	// doesn't exist in that, its counterpart in this is not modified
	// returns true if this causes this to change and false otherwise
	bool widenUpdateLimitToThat(InfiniteLattice* that, string indent="");
	
	// Common code for widenUpdate() and widenUpdateLimitToThat()
	bool widenUpdate_ex(InfiniteLattice* that_arg, bool limitToThat, string indent="");
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	// The meet is the intersection of constraints: the set of constraints 
	//    that is common to both constraint graphs. Thus, the result is the loosest
	//    set of constraints that satisfies both sets and therefore also the information union.
	bool meetUpdate(Lattice* that, string indent="");
	bool meetUpdate(Lattice* that) { return meetUpdate(that, ""); }
	
	// Meet this and that and saves the result in this, while ensuring that if a given constraint
	// doesn't exist in that, its counterpart in this is not modified
	// returns true if this causes this to change and false otherwise
	bool meetUpdateLimitToThat(InfiniteLattice* that, string indent="");
		
	// Common code for meetUpdate() and meetUpdateLimitToThat()
	bool meetUpdate_ex(Lattice* that_arg, bool limitToThat, string indent="");
	
	// <from LogicalCond>
	bool orUpd(LogicalCond& that, string indent="");
	bool orUpd(LogicalCond& that)
	{ return orUpd(that, ""); }

	// <from LogicalCond>
	bool andUpd(LogicalCond& that, string indent="");
	bool andUpd(LogicalCond& that)
	{ return andUpd(that, ""); }
	
	bool andUpd(ConstrGraph* that, string indent="");
	bool andUpd(ConstrGraph* that)
	{ return andUpd(that, ""); }
	
	// Unified function for Or(meet), And and Widening
	// If meet == true, this function computes the meet and if =false, computes the widening.
	// If OR == true, the function computes the OR of each pair of inequalities and otherwise, computes the AND.
	// if limitToThat == true, if a given constraint does not exist in that, this has no effect on the meet/widening
	bool OrAndWidenUpdate(ConstrGraph* that, bool meet, bool OR, bool limitToThat, string indent="");

	
	// Portion of OrAndWidenUpdate that deals with x variables for which there exist x->y mapping 
	// in This but not in That. Increments itThisX and updates modified and modifiedVars in case this 
	// function modifies the constraint graph.
	void OrAndWidenUpdate_XinThisNotThat(
		                            bool OR, bool limitToThat, 
		                            map<varID, map<varID, affineInequality> >::iterator& itThisX, bool& modified,
		                            string indent="");
	
	// Portion of OrAndWidenUpdate that deals with x variables for which there exist x->y mapping 
	// in That but not in This. Increments itThisX and updates modified and modifiedVars in case this 
	// function modifies the constraint graph.
	// additionsToThis - Records the new additions to vars2Value that need to be made after we are done iterating 
	//      over it. It guaranteed that the keys mapped by the first level of additionsToThis are not mapped
	//      at the first level by vals2Value.
	void OrAndWidenUpdate_XinThatNotThis(
		                            bool OR, bool limitToThat, 
		                            ConstrGraph* that,
		                            map<varID, map<varID, affineInequality> >::iterator& itThatX, 
		                            map<varID, map<varID, affineInequality> >& additionsToThis, 
		                            bool& modified, string indent="");
	
	// Portion of OrAndWidenUpdate that deals with x->y pairs for which there exist x->y mapping 
	// in This but not in That. Increments itThisX and updates modified and modifiedVars in case this 
	// function modifies the constraint graph.
	void OrAndWidenUpdate_YinThisNotThat(
		                            bool OR, bool limitToThat, 
		                            map<varID, map<varID, affineInequality> >::iterator& itThisX,
		                            map<varID, affineInequality>::iterator& itThisY, 
		                            bool& modified, string indent="");
	
	// Portion of OrAndWidenUpdate that deals with x->y pairs for which there exist x->y mapping 
	// in That but not in This. Increments itThisX and updates modified and modifiedVars in case this 
	// function modifies the constraint graph.
	// additionsToThis - Records the new additions to vars2Value[itThisX->first] that need to be made after 
	//      we are done iterating over it. It guaranteed that the keys mapped by additionsToThis are not mapped
	//      at the first level by vals2Value[itThisX->first].
	void OrAndWidenUpdate_YinThatNotThis(
		                            bool OR, bool limitToThat, 
		                            map<varID, affineInequality>::iterator& itThisY, 
		                            map<varID, map<varID, affineInequality> >::iterator& itThatX,
		                            map<varID, affineInequality>::iterator& itThatY, 
		                            map<varID, affineInequality>& additionsToThis, 
		                            bool& modified, string indent="");
		
	// Computes the transitive closure of the given constraint graph, and updates the graph to be that transitive closure. 
	// Returns true if this causes the graph to change and false otherwise.
	bool transitiveClosure(string indent="");
	protected:
	bool transitiveClosureDiv(string indent="");
	void transitiveClosureY(const varID& x, const varID& y, bool& modified, int& numSteps, int& numInfers, bool& iterModified, string indent="");
	void transitiveClosureZ(const varID& x, const varID& y, const varID& z, bool& modified, int& numSteps, int& numInfers, bool& iterModified, string indent="");
	
	public:
	// Computes the transitive closure of the given constraint graph,
	// focusing on the constraints of scalars that have divisibility variables
	// we only bother propagating constraints to each such variable through its divisibility variable
	// Returns true if this causes the graph to change and false otherwise.
	bool divVarsClosure(string indent="");
	
	// The portion of divVarsClosure that is called for every y variable. Thus, given x and x' (x's divisibility variable)
	// divVarsClosure_perY() is called for every scalar or array y to infer the x->y connection thru x->x'->y and
	// infer the y->x connection thru x->x'->x
	// Returns true if this causes the graph to change and false otherwise.
	bool divVarsClosure_perY(const varID& x, const varID& divX, const varID& y, 
	                         affineInequality* constrXDivX, affineInequality* constrDivXX/*,
	                         affineInequality::signs xSign, affineInequality::signs ySign*/,
	                         string indent="");
	
	// Computes the transitive closure of this constraint graph while modifying 
	// only the constraints that involve the given variable
	// Returns true if this causes the graph to change and false otherwise.
	bool localTransClosure(const varID& tgtVar, string indent="");
		
protected:
	// Searches this constraint graph for negative cycles, which indicates that the constraints represented
	//    by the graph are not self-consistent (the code region where the graph holds is unreachable). Modifies
	//    the level of this graph as needed.
	// Returns true if this call caused a modification in the graph and false otherwise.
	bool checkSelfConsistency(string indent="");
	
public:
	
	// Creates a divisibility variable for the given variable and adds it to the constraint graph
	// If var = r (mod d), then the relationship between x and x' (the divisibility variable)
	// will be x = x'*d + r
	// returns true if this causes the constraint graph to be modified (it may not if this 
	//    information is already in the graph) and false otherwise
	bool addDivVar(varID var/*, int div, int rem*/, bool killDivVar=false, string indent="");
	
	// Disconnect this variable from all other variables except its divisibility variable. This is done 
	// in order to compute the original variable's relationships while taking its divisibility information 
	// into account.
	// Returns true if this causes the constraint graph to be modified and false otherwise
	bool disconnectDivOrigVar(varID var/*, int div, int rem*/, string indent="");
	
	// Finds the variable within this constraint graph that corresponds to the given divisibility variable.
	//    If such a variable exists, returns the pair <variable, true>.
	//    Otherwise, returns <???, false>.
	pair<varID, bool> divVar2Var(const varID& divVar, string indent="");
	
	// Adds a new divisibility lattice, with the associated anotation
	// Returns true if this causes the constraint graph to be modified and false otherwise
	bool addDivL(FiniteVarsExprsProductLattice* divLattice, string annotName, void* annot, string indent="");
	
	// Adds a new sign lattice, with the associated anotation
	// Returns true if this causes the constraint graph to be modified and false otherwise
	// GB : 2011-03-05 (Removing Sign Lattice Dependence)
	// bool addSgnL(FiniteVarsExprsProductLattice* sgnLattice, string annotName, void* annot, string indent="");
	
	/**** State Accessor Functions *****/
	// Returns true if this constraint graph includes constraints for the given variable
	// and false otherwise
	bool containsVar(const varID& var, string indent="");
	
	// returns the x->y constraint in this constraint graph
	affineInequality* getVal(varID x, varID y, string indent="");
	
	// set the x->y connection in this constraint graph to c
	// return true if this results this ConstrGraph being changed, false otherwise
	// xSign, ySign: the default signs for x and y. If they're set to unknown, setVal computes them on its own using getVarSign.
	//     otherwise, it uses the given signs 
	// GB : 2011-03-05 (Removing Sign Lattice Dependence)
	/*bool setVal(varID x, varID y, int a, int b, int c, 
	            affineInequality::signs xSign=affineInequality::unknownSgn, affineInequality::signs ySign=affineInequality::unknownSgn, 
	            string indent="");*/
	bool setVal(varID x, varID y, int a, int b, int c, 
	            string indent="");
	/*{ return setVal(x, y, a, b, c, 
	                // GB : 2011-03-05 (Removing Sign Lattice Dependence)affineInequality::unknownSgn, affineInequality::unknownSgn, 
	                indent); }*/
	
	bool setVal(varID x, varID y, const affineInequality& ineq, string indent="");
	
	// Sets the state of this constraint graph to Uninitialized, without modifying its contents. Thus, 
	//    the graph will register as uninitalized but when it is next used, its state will already be set up.
	// Returns true if this causes the constraint graph to be modified and false otherwise.
	bool setToUninitialized_KeepState(string indent="");
	
	// Sets the state of this constraint graph to Bottom
	// Returns true if this causes the constraint graph to be modified and false otherwise.
	bool setToBottom(string indent="");
	
	// Sets the state of this constraint graph to constrKnown, with the given constraintType
	// eraseCurConstr - if true, erases the current set of constraints and if false, leaves them alone
	// Returns true if this causes the constraint graph to be modified and false otherwise.
	bool setToConstrKnown(constrTypes ct, bool eraseCurConstr=true, string indent="");
	
	// Sets the state of this constraint graph to Inconsistent
	// noConsistencyCheck - flag indicating whether this function should do nothing if this noConsistencyCheck() returns 
	//              true (=false) or to not bother checking with isBottom (=true)
	// Returns true if this causes the constraint graph to be modified and false otherwise.
	bool setToInconsistent(string indent="");
	
	// Sets the state of this constraint graph to top 
	// If onlyIfNotInit=true, this is only done if the graph is currently uninitialized
	// Returns true if this causes the constraint graph to be modified and false otherwise.
	bool setToTop(bool onlyIfNotInit=false, string indent="");
	
	
	// Returns the level and constraint type of this constraint graph
	// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
	// 							or to not bother checking self-consistency and just return the last-known value (=true)
	pair<levels, constrTypes> getLevel(bool noConsistencyCheck=false, string indent="");
	
	// Returns true if this graph is self-consistent and false otherwise
	// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
	// 							or to not bother checking self-consistency and just return the last-known value (=true)
	bool isSelfConsistent(bool noConsistencyCheck=false, string indent="");
	
	// Returns true if this graph has valid constraints and is self-consistent
	// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
	// 							or to not bother checking self-consistency and just return the last-known value (=true)
	bool hasConsistentConstraints(bool noConsistencyCheck=false, string indent="");

	// Returns true if this constraint graph is maximal in that it can never reach a higher lattice state: it is
	//    either top or inconsistent). Returns false if it not maximal.
	// noConsistencyCheck - flag indicating whether this function should explicitly check the self-consisteny of this graph (=false)
	// 							or to not bother checking self-consistency and just return the last-known value (=true)
	bool isMaximalState(bool noConsistencyCheck=false, string indent="");
	
	/**** String Output *****/
	
	// Returns the string representation of the constraints held by this constraint graph, 
	//    with a line for each pair of variables for which the constraint is < bottom. It also prints
	//    the names of all the arrays that have empty ranges in this constraint graph
	// There is no \n on the last line of output, even if it is a multi-line string
	string str(string indent="");
	void varSetStatusToStream(const set<varID>& vars, ostringstream& outs, bool &needEndl, string indent="");
	
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
	bool mustOutsideRange(varID x, int b, int c, varID y, string indent="");
	
	// returns true if this logical condition must be true and false otherwise
	// <from LogicalCond>
	bool mayTrue(string indent="");
	bool mayTrue() { return mayTrue(""); }
		
/*	// returns true if x+c MUST be inside the range of y and false otherwise
	// If two variables are unrelated, it is assumed that there is no information 
	// about their relationship and mustInsideRange() thus proceeds conservatively.
	bool mustInsideRange(varID x, int b, int c, varID y);*/
	
	/* Transactions */
	void beginTransaction(string indent="");
	void endTransaction(string indent="");
};



#endif

