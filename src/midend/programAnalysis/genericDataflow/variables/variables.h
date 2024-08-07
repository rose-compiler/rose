#include <featureTests.h>
#ifdef ROSE_ENABLE_SOURCE_ANALYSIS

#ifndef VARIABLES_H
#define VARIABLES_H

#include "genericDataflowCommon.h"
#include <map>
#include <vector>
#include <list>
#include <utility>
#include <set>
#include <string>
#include <iosfwd>

class varID;

/* #########################
   ###### SHARED DATA ######
   ######################### */


extern std::map<SgFunctionDefinition*, std::set<varID> > allVars;
extern std::map<SgFunctionDefinition*, std::set<varID> > activeVars;

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
        public:
        //const char* str();
        virtual std::string str() const = 0;
        
        virtual bool operator == (const variable &that) const = 0;
        virtual bool operator <  (const variable &that) const = 0;
        
        // returns the scope in which this array variable was declared
        virtual SgScopeStatement* getScope() const=0;
        
        // returns a SgExpression that corresponds to an access of this variable
        virtual SgExpression* toSgExpression() const=0;
        
        // returns true if this variable is global and false otherwise
        virtual bool isGlobal() const=0;
        
        virtual ~variable() {}
};

// type of variable ids, their sets and maps of sets
class varID : public variable
{
        public:
        std::vector<SgInitializedName *> components;
        // Annotations associated with this variable
        std::map<std::string, void*> annotations;
        SgType* varType;
        // special variables do not have associated SgInitializedName nodes and thus, we just give them a name
        std::string name;
        
        varID() = default;
        
        varID(std::string name)
        {
                this->name = name;
                varType = nullptr;
                genID();
        }
        
        // creates a varID from an SgNode s.t. isValidVarExp(n) is true
        varID(SgNode *n)
        {
                bool ret = init(n);
                ASSERT_require(ret);
                genID();
        }
        
        varID(SgInitializedName *name)
        {
                bool ret = init(name);
                ROSE_ASSERT(ret);
                genID();
        }
        
        // pre-condition: isValidVarExp(refExp) evaluates to true
        varID(const SgExpression *exp)
        {
                bool ret = init(exp);
                ASSERT_require(ret);
                genID();
        }
        
        varID& operator=(const varID &) = default; // removes warning message regarding copy constructor
        varID(const varID& that) : variable()
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
        bool init(const SgExpression *exp);
        
        // initializes this object from the given SgInitializedName (assumed that isValidVarExp(name) is true)
        bool init(SgInitializedName* name);
        
        void operator = (const variable &that);
        
        // recursive function that pulls the SgInitializedNames of all the SgVarRefExps inside this SgDotExp
        // returns true on success, false on failure
        bool collectDotComponents(const SgDotExp* dotExp);
        
        // returns the scope in which this variable was declared
        // for compound variables (i.e. those composed of dot expressions), it is the scope of the leftmost name
        SgScopeStatement* getScope() const;
        
        // returns a SgExpression that corresponds to an access of this variable
        SgExpression* toSgExpression() const;
        
protected:
        // returns a SgExpression that corresponds to an access of this variable, including only the components
        // at or after the iterator rest into the components vector
        SgExpression* toSgExpression_rec(std::vector<SgInitializedName *>::const_iterator rest) const;
        
public:
        // returns true if the given expression is one that can be represented as a variable in our representation
        static bool isValidVarExp(const SgNode* exp);
        static bool isValidVarExp(const SgExpression* exp);
        static bool isValidVarExp(const SgInitializedName* exp);
        
protected:
        static bool isValidVarExp_rec(const SgExpression* exp);
        
public:
        void add(SgInitializedName *name);
        
        // Adds the given annotation to this variable. Returns true if this causes the variable's
        // annotation state to change and false otherwise.
        bool addAnnotation(const std::string& aName, void* annot);
        
        // Remove the given annotation from this variable. Returns true if this variable
        // previously had this annotation and false otherwise.
        bool remAnnotation(const std::string& aName);
        
        // Remove the given annotation from this variable. Returns true if this variable
        // previously had this annotation and false otherwise.
        bool remAllAnnotations();
        
        // Swap this variables annotations, removing [fromAnnotName -> fromAnnotVal] and replacing
        // it with [toAnnotName -> toAnnotVal]. Returns true if this was successful (i.e. the variable
        // does have the [fromAnnotName -> fromAnnotVal] annotation) and false otherwise.
        // If the replacement occurs and this variable already has an annotation named
        //    toAnnotName, this annotation's value is replaced by toAnnotVal.
        bool swapAnnotations(const std::string& fromAnnotName, void* fromAnnotVal,
                             const std::string& toAnnotName, void* toAnnotVal);
        
        // If this variable has the annotation with the given name, returns it. Otherwise, returns NULL.
        void* getAnnotation(const std::string& aName) const;
        
        // If this variable has the annotation with the given name, returns true. Otherwise, returns false.
        bool hasAnnotation(const std::string& aName) const;
        
        // If this variable has the annotation with ANY name in the given set, returns true. Otherwise, returns false.
        bool hasAnyAnnotation(const std::set<std::string>& aNames) const;
        
        // If this variable has the annotation with EACH name in the given set, returns true. Otherwise, returns false.
        bool hasAllAnnotations(const std::set<std::string>& aNames) const;
        
        // Returns the total number of annotations associated with this variable
        int numAnnotations() const;
        
        // Returns the full map of all the annotations associated with this variable
        const std::map<std::string, void*>& getAnnotations() const;

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
        
        // string representation of the variable reference.
        // If noAnnot is true, excludes annotations from the name.
        //const char* str();
        std::string str() const;
        std::string str(bool noAnnot) const;
        
        // string representation of the variable reference, with variable/field names augmented with 
        // the line numbers where they were defined. File names are omitted for brevity 
        //const char* str_linenum();
        std::string str_linenum() const;
        
        // string representation of the variable reference, with the variable names replaced 
        // with the pointers to their declarations
        //const char* str_ptr();
        std::string str_ptr() const;
                
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

std::ostream &operator<<(std::ostream &stream, varID v);
std::ostream &operator<<(std::ostream &stream, const std::set<varID>::iterator& v);
//ostream &operator<<(ostream &stream, const std::set<varID>::const_iterator& v);

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

typedef std::set<varID, std::less<varID> > varIDSet;
typedef std::map<varID, varIDSet *>        m_varID2setPtr;
typedef std::map<varID, quad>              m_varID2quad;
typedef std::map<varID, std::string>            m_varID2str;
typedef std::map<varID, bool>              m_varID2bool;
typedef std::pair<varID, varID>            varIDpair;
typedef std::list<varID>                   varIDlist;
typedef std::map<varID, m_varID2quad>      m_varID2varID2quad;

// type of number sets and maps of their sets
typedef std::set<quad, std::less<quad> > setQuad;
typedef std::map<quad, setQuad *> m_quad2setPtr;

/* #################################
   ###### FUNCTION PROTOTYPES ######
   ################################# */

// Returns the varID that corresponds to the given SgExpression
varID SgExpr2Var(const SgExpression* expr);

// Returns true if the given expression can be interepreted as a concrete variable
bool isVarExpr(SgExpression* expr);

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
        std::list<SgExpression*>* indexExprs;
        
        public:
        arrayElt(SgNode* expr);
        
        arrayElt(SgExpression* expr);
        
        std::string str() const;
        
        bool operator == (const variable &that_arg) const;
        
        bool operator <  (const variable &that) const;
        
        // returns true if the given expression is one that can be represented as a variable in our representation
        static bool isValidVarExp(const SgExpression* exp);
        
        // returns the scope in which this array variable was declared
        // for compound variables (i.e. those composed of dot expressions), it is the scope of the leftmost name
        SgScopeStatement* getScope() const;
        
        // returns a reference to the array variable, without any indexes
        const varID& getArrayVar();
        
        // returns a reference to the index expressions
        std::list<SgExpression*>* getIndexExprs();
        
        // returns a SgExpression that corresponds to an access of this variable
        SgExpression* toSgExpression() const;
        
        protected:
        // returns the SgPntrArrRefExp that corresponds to the index expressions in indexExprs that start with itIndexes
        // precondition : itIndexes!=index.end()
        SgPntrArrRefExp* toSgExpression_rec(std::list<SgExpression*>::reverse_iterator itIndexes) const;
                
        public:
        // returns true if this variable is global and false otherwise
        bool isGlobal() const;
};

// returns a set of arrayElts that correspond to all the arrays (of SgDotExp/SgVarRefExp[SgExpression][][][]... form)
// that were referenced in the given subtree
std::set<arrayElt> getArrayRefsInSubtree(SgNode* root);
#endif
#endif
