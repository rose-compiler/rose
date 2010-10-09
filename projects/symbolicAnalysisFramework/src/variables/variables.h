#ifndef VARIABLES_H
#define VARIABLES_H

#include "common.h"

class varID;

/* #########################
   ###### SHARED DATA ######
   ######################### */


extern map<SgFunctionDefinition*, set<varID> > allVars;
extern map<SgFunctionDefinition*, set<varID> > activeVars;

extern varID zeroVar;
extern varID oneVar;
extern varID allVar;

/*// = true only after the variables module has been initialized
extern bool variables_module_initialized;

void initVariables(SgProject* project);*/

/* ##############################
   ###### TYPE DEFINITIONS ######
   ############################## */

// virtual base class for other variable expressions
class variable
{
	//const char* str();
	virtual string str() const = 0;
	
	virtual bool operator == (const variable &that) const = 0;
	virtual bool operator <  (const variable &that) const = 0;
	
	// returns the scope in which this array variable was declared
	virtual SgScopeStatement* getScope() const=0;
	
	// returns a SgExpression that corresponds to an access of this variable
	virtual SgExpression* toSgExpression() const=0;
	
	// returns true if this variable is global and false otherwise
	virtual bool isGlobal() const=0;
};

// type of variable ids, their sets and maps of sets
class varID : public variable
{
	public:
	vector<SgInitializedName *> components;
	// Annotations associated with this variable
	map<string, void*> annotations;
	SgType* varType;
	// special variables do not have associated SgInitializedName nodes and thus, we just give them a name
	string name;
	
	varID(){ }
	
	varID(string name)
	{
		this->name = name;
		varType = NULL;
		genID();
	}
	
	// creates a varID from an SgNode s.t. isValidVarExp(n) is true
	varID(SgNode *n)
	{
		bool ret = init(n);
		ROSE_ASSERT(ret);
		genID();
	}
	
	varID(SgInitializedName *name)
	{
		bool ret = init(name);
		ROSE_ASSERT(ret);
		genID();
	}
	
	// pre-condition: isValidVarExp(refExp) evaluates to true
	varID(SgExpression *exp)
	{
		bool ret=init(exp);
		ROSE_ASSERT(ret);
		genID();
	}
	
	varID(const varID& that)
	{
		init(that);
	}
	
	// initializes this object from the given varID
	bool init(const varID& that);
	
	// initializes this object from the given expression (assumed that isValidVarExp(n) is true)
	// returns true on success, false on failure
	bool init(SgNode *n);
	
	// initializes this object from the given expression (assumed that isValidVarExp(exp) is true)
	// returns true on success, false on failure
	bool init(SgExpression *exp);
	
	// initializes this object from the given SgInitializedName (assumed that isValidVarExp(name) is true)
	bool init(SgInitializedName* name);
	
	bool operator = (const variable &that);
	
	// recursive function that pulls the SgInitializedNames of all the SgVarRefExps inside this SgDotExp
	// returns true on success, false on failure
	bool collectDotComponents(SgDotExp* dotExp);
	
	// returns the scope in which this variable was declared
	// for compound variables (i.e. those composed of dot expressions), it is the scope of the leftmost name
	SgScopeStatement* getScope() const;
	
	// returns a SgExpression that corresponds to an access of this variable
	SgExpression* toSgExpression() const;
	
protected:
	// returns a SgExpression that corresponds to an access of this variable, including only the components
	// at or after the iterator rest into the components vector
	SgExpression* toSgExpression_rec(vector<SgInitializedName *>::const_iterator rest) const;
	
public:
	// returns true if the given expression is one that can be represented as a variable in our representation
	static bool isValidVarExp(SgNode* exp);
	static bool isValidVarExp(SgExpression* exp);
	static bool isValidVarExp(SgInitializedName* exp);
	
protected:
	static bool isValidVarExp_rec(SgExpression* exp);
	
public:
	void add(SgInitializedName *name);
	
	// Adds the given annotation to this variable. Returns true if this causes the variable's
	// annotation state to change and false otherwise.
	bool addAnnotation(const string& aName, void* annot);
	
	// Remove the given annotation from this variable. Returns true if this variable
	// previously had this annotation and false otherwise.
	bool remAnnotation(const string& aName);
	
	// Remove the given annotation from this variable. Returns true if this variable
	// previously had this annotation and false otherwise.
	bool remAllAnnotations();
	
	// Swap this variables annotations, removing [fromAnnotName -> fromAnnotVal] and replacing
	// it with [toAnnotName -> toAnnotVal]. Returns true if this was successful (i.e. the variable
	// does have the [fromAnnotName -> fromAnnotVal] annotation) and false otherwise.
	// If the replacement occurs and this variable already has an annotation named
	//    toAnnotName, this annotation's value is replaced by toAnnotVal.
	bool swapAnnotations(const string& fromAnnotName, void* fromAnnotVal,
	                     const string& toAnnotName, void* toAnnotVal);
	
	// If this variable has the annotation with the given name, returns it. Otherwise, returns NULL.
	void* getAnnotation(const string& aName) const;
	
	// If this variable has the annotation with the given name, returns true. Otherwise, returns false.
	bool hasAnnotation(const string& aName) const;
	
	// If this variable has the annotation with ANY name in the given set, returns true. Otherwise, returns false.
	bool hasAnyAnnotation(const set<string>& aNames) const;
	
	// If this variable has the annotation with EACH name in the given set, returns true. Otherwise, returns false.
	bool hasAllAnnotations(const set<string>& aNames) const;
	
	// Returns the total number of annotations associated with this variable
	int numAnnotations() const;
	
	// Returns the full map of all the annotations associated with this variable
	const map<string, void*>& getAnnotations() const;

	/******************
	 *** COMPARISON ***
	 ******************/

public:	
	bool equal(const varID& two) const;
	bool lessThan(const varID& two) const;
	
	bool operator == (const variable &that) const;
	bool operator != (const varID &that) const;
	bool operator <  (const variable &that) const;
	bool operator >  (const varID &that) const;
	bool operator <= (const varID &that) const;
	bool operator >= (const varID &that) const;
	
	/**************
	 *** OUTPUT ***
	 **************/
	
	// string representation of the variable reference
	//const char* str();
	string str() const;
	
	// string representation of the variable reference, with variable/field names augmented with 
	// the line numbers where they were defined. File names are omitted for brevity	
	//const char* str_linenum();
	string str_linenum() const;
	
	// string representation of the variable reference, with the variable names replaced 
	// with the pointers to their declarations
	//const char* str_ptr();
	string str_ptr() const;
	
	/**********************
	 *** SEMANTINC INFO ***
	 **********************/
	
	// returns true if the last field in this variable has an array or pointer type
	bool isArrayType() const;
	
	// returns true if any of its constituent SgInitializedNames are compiler-generated
	bool isCompilerGenerated() const;
	
	// returns true if this variable is global and false otherwise
	bool isGlobal() const;
	
	/*const bool isReferenceType()
	{
		return isSgReferenceType(components[components.size()-1]->get_typeptr());
	}*/
	
	private:
	// Unique ID generation and access functionality
	// the maximum ID that has been generated for any variable
	static long globalMaxID;
	// the unique ID of this variable
	long ID;
	// generates a new ID for this variable and stores it in ID
	void genID();
	
	public:
	// returns this variable's ID
	long getID() const;
};

//bool operator == ( const varID &one, const varID &two);

// Variables are ordered in lexicographic order, with element-wise comparisons 
// performed using the basic < operator for pointers.
//bool operator < ( const varID &one, const varID &two);
//bool operator > ( const varID &one, const varID &two);

bool operator == (const varID &var, SgInitializedName* iName);
bool operator != (const varID &var, SgInitializedName* iName);
bool operator == (SgInitializedName* iName, const varID &var);
bool operator != (SgInitializedName* iName, const varID &var);

bool operator == (const varID &var, SgExpression* expr);
bool operator != (const varID &var, SgExpression* expr);
bool operator == (SgExpression* expr, const varID &var);
bool operator != (SgExpression* expr, const varID &var);

typedef set<varID, less<varID> > varIDSet;
typedef map<varID, varIDSet *>        m_varID2setPtr;
typedef map<varID, quad>              m_varID2quad;
typedef map<varID, string>            m_varID2str;
typedef map<varID, bool>              m_varID2bool;
typedef pair<varID, varID>            varIDpair;
typedef list<varID>                   varIDlist;
typedef map<varID, m_varID2quad>      m_varID2varID2quad;

// type of number sets and maps of their sets
typedef set<quad, std::less<quad> > setQuad;
typedef map<quad, setQuad *> m_quad2setPtr;

/* #################################
   ###### FUNCTION PROTOTYPES ######
   ################################# */

// convert from variable id to variable name
std::string getVariableName( varID var );

// translate from an expression that uses a variable to that variable's unique id
// currently supported: a (SgVarRefExp), a.i (SgDotExp), 
/*varID getVarReference( SgVarRefExp *exp );
varID getVarReference( SgDotExp * );
varID getVarReference( SgInitializedName * );*/

// add the special variable Zero to the given set of variables
void addPredefinedVars(varIDSet &vars);

// returns whether the variable with the given id exists in our set of interest
bool existsVariable( varID x, m_varID2str &vars2Name );
// returns whether the variable in the given reference expression exists in our 
// set of interest
bool existsVariable( SgVarRefExp *x, m_varID2str &vars2Name );

// returns whether this is a variable reference or dot expression (field reference)
bool isTypeConsidered( SgNode * exp);

// returns the id in a variable reference or dot expression (field reference)
//varID getRefOfTypeConsidered( SgNode *exp );

/*// mapping from variable declaration node pointer to string variable name for
// all the variables being used in the analysis of the current array
// (i.e. any variables involved in operations with the array, as
//   defined in determineInterestSet()
m_varID2str vars2Name;

// set of all 0, 1, -1, all constants being used in the program +/-1 and the sums of all the above
// may be used to make the widening operator more permissive
varIDSet allConstants;
*/

// returns a set of varIDs that correspond to all the variables (of SgDotExp/SgVarRefExp form)
// that were referenced in the given subtree
varIDSet getVarRefsInSubtree(SgNode* root);

// returns a set of varIDs that correspond to all the variables (of SgDotExp/SgVarRefExp form)
// that were read from in the given subtree
varIDSet getReadVarRefsInSubtree(SgNode* root);

// returns a set of varIDs that correspond to all the variables (of SgDotExp/SgVarRefExp form)
// that were written to in the given subtree
varIDSet getWriteVarRefsInSubtree(SgNode* root);

// returns a set of varIDs that correspond to all the variables (of SgDotExp/SgVarRefExp form)
// that were referenced as arrays (i.e. var[i]) in the given subtree
varIDSet getArrayVarRefsInSubtree(SgNode* root);


class arrayElt : public variable
{
	varID arrayVar;
	list<SgExpression*>* indexExprs;
	
	public:
	arrayElt(SgNode* expr);
	
	arrayElt(SgExpression* expr);
	
	string str() const;
	
	bool operator == (const variable &that_arg) const;
	
	bool operator <  (const variable &that) const;
	
	// returns true if the given expression is one that can be represented as a variable in our representation
	static bool isValidVarExp(SgExpression* exp);
	
	// returns the scope in which this array variable was declared
	// for compound variables (i.e. those composed of dot expressions), it is the scope of the leftmost name
	SgScopeStatement* getScope() const;
	
	// returns a reference to the array variable, without any indexes
	const varID& getArrayVar();
	
	// returns a reference to the index expressions
	list<SgExpression*>* getIndexExprs();
	
	// returns a SgExpression that corresponds to an access of this variable
	SgExpression* toSgExpression() const;
	
	protected:
	// returns the SgPntrArrRefExp that corresponds to the index expressions in indexExprs that start with itIndexes
	// precondition : itIndexes!=index.end()
	SgPntrArrRefExp* toSgExpression_rec(list<SgExpression*>::reverse_iterator itIndexes) const;
		
	public:
	// returns true if this variable is global and false otherwise
	bool isGlobal() const;
};

// returns a set of arrayElts that correspond to all the arrays (of SgDotExp/SgVarRefExp[SgExpression][][][]... form)
// that were referenced in the given subtree
set<arrayElt> getArrayRefsInSubtree(SgNode* root);
#endif
