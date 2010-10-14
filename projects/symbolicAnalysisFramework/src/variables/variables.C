#include "variables.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "arrIndexLabeler.h"
#include "rwAccessLabeler.h"
#include <set>

using namespace std;
using namespace arrIndexLabeler;

map<SgFunctionDefinition*, set<varID> > allVars;
map<SgFunctionDefinition*, set<varID> > activeVars;

#define SgDefaultFile Sg_File_Info::generateDefaultFileInfoForTransformationNode()
	
/*// = true only after the variables module has been initialized
bool variables_module_initialized=false;

void initVariables(SgProject* project)
{
	addArrayIndexAnnotations(project);	
	variables_module_initialized=true;
}
*/
/******************************
 ********* varID CODE *********
 ******************************/
 
bool varID::init(const varID& that)
{
	components.clear();
	components = that.components;
	annotations = that.annotations;
	varType = that.varType;
/*	cout << "that.name = "<<that.name<<"\n";
	cout << "name = "<<name<<"\n";*/
	name = that.name;
}

// initializes this object from the given expression (assumed that isValidVarExp(n) is true)
// returns true on success, false on failure
bool varID::init(SgNode *n)
{
	ROSE_ASSERT(isValidVarExp(n));
	if(isSgExpression(n))
		init(isSgExpression(n));
	else if(isSgInitializedName(n))
		init(isSgInitializedName(n));
}
 
// initializes this object from the given expression (assumed that isValidVarExp(exp) is true)
// returns true on success, false on failure
bool varID::init(SgExpression *exp)
{
	exp = cfgUtils::unwrapCasts(exp);
	if(isValidVarExp(exp))
	{
		// if this is an array reference expression, the variable in question is the array itself
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// !!! NOTE: IN THE FUTURE WE MAY WANT TO BE MORE PRECISE ABOUT THIS BUT WE'LL NEED TO ADD THE NOTION OF ARRAYS TO OUR varIDs !!!
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		/*if(isTopArrayRefExp(exp))
			exp = getArrayNameExp(exp);*/
		
		SgVarRefExp* refExp;
		SgDotExp* dotExp;
		if(refExp = isSgVarRefExp(exp))
		{
			components.clear();
			components.push_back(refExp->get_symbol()->get_declaration());
			return true;
		}
		else if(dotExp = isSgDotExp(exp))
		{
			components.clear();
			return collectDotComponents(dotExp);
		}
	}
	varType = exp->get_type();
	return false;
}

// initializes this object from the given SgInitializedName (assumed that isValidVarExp(name) is true)
bool varID::init(SgInitializedName* name)
{
	components.clear();
	components.push_back(name);
	varType = name->get_type();
	return true;
}

bool varID::operator = (const variable &that_arg)
{
	const varID& that = (const varID&)that_arg;
	components  = that.components;
	annotations = that.annotations;
	varType     = that.varType;
	name        = name;
}

// recursive function that pulls the SgInitializedNames of all the SgVarRefExps inside this SgDotExp
// returns true on success, false on failure
bool varID::collectDotComponents(SgDotExp* dotExp)
{
//		printf("isSgVarRefExp(dotExp->get_lhs_operand())=%d isSgDotExp(dotExp->get_lhs_operand())=%d\n", isSgVarRefExp(dotExp->get_lhs_operand()), isSgDotExp(dotExp->get_lhs_operand()));
//		printf("isSgVarRefExp(dotExp->get_rhs_operand())=%d isSgDotExp(dotExp->get_rhs_operand())=%d\n", isSgVarRefExp(dotExp->get_rhs_operand()), isSgDotExp(dotExp->get_rhs_operand()));

	SgExpression* lhs = dotExp->get_lhs_operand();
	ROSE_ASSERT(lhs);
	if(isSgVarRefExp(lhs))
	{
//			printf("pushing lhs %s\n", ((SgVarRefExp*)lhs)->get_symbol()->get_declaration()->get_name().str());
		components.push_back(((SgVarRefExp*)lhs)->get_symbol()->get_declaration());
	}
	else if(isSgDotExp(lhs))
		collectDotComponents((SgDotExp*)lhs);
	else
		return false;

	SgVarRefExp* rhs = isSgVarRefExp(dotExp->get_rhs_operand());
	ROSE_ASSERT(rhs);
//		printf("pushing rhs %s\n", rhs->get_symbol()->get_declaration()->get_name().str());
	components.push_back(rhs->get_symbol()->get_declaration());
	return true;
}

// returns the scope in which this variable was declared
// for compound variables (i.e. those composed of dot expressions), it is the scope of the leftmost name
SgScopeStatement* varID::getScope() const
{
	ROSE_ASSERT(components.size()>0);
	return components[0]->get_scope();
}	

// returns a SgExpression that corresponds to an access of this variable
SgExpression* varID::toSgExpression() const
{
	ROSE_ASSERT(components.size()>0 && varType!=NULL);
	return toSgExpression_rec(components.begin());
}

// returns a SgExpression that corresponds to an access of this variable, including only the components
// at or after the iterator rest into the components vector
SgExpression* varID::toSgExpression_rec(vector<SgInitializedName *>::const_iterator rest) const
{
	SgInitializedName *curName = (*rest);
	
	rest++;
	// if this is the last component
	if(rest==components.end())
	{
		return varRefFromInitName(curName);
	}
	else
	{
		SgVarRefExp* lhs = varRefFromInitName(curName);
		SgExpression* rhs = toSgExpression_rec(rest);
		SgDotExp* de = new SgDotExp(SgDefaultFile, lhs, rhs, varType);
		lhs->set_parent(de);
		rhs->set_parent(de);
		return de;
	}
}

bool varID::isValidVarExp(SgNode* n)
{
	if(isSgExpression(n))
		return isValidVarExp(isSgExpression(n));
	else if(isSgInitializedName(n))
		return isValidVarExp(isSgInitializedName(n));
	else
		return false;
}		

// returns true if the given expression is one that can be represented as a variable in our representation
bool varID::isValidVarExp(SgExpression* exp)
{
	ROSE_ASSERT(exp->get_parent());
	
	exp = cfgUtils::unwrapCasts(exp);
		
	// if this is an array reference expression, the variable in question is the array itself
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!! NOTE: IN THE FUTURE WE MAY WANT TO BE MORE PRECISE ABOUT THIS BUT WE'LL NEED TO ADD THE NOTION OF ARRAYS TO OUR varIDs !!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	/*if(isTopArrayRefExp(exp))
		exp = getArrayNameExp(exp);*/
	//printf("isValidVarExp: exp->class_name()=%s  \"%s\"\n", exp->class_name().c_str(), exp->unparseToString().c_str());
	
/*		printf("isValidVarExp() isSgVarRefExp(exp)=%d  isSgDotExp(exp)=%d\n", isSgVarRefExp(exp), isSgDotExp(exp));
	if(isSgVarRefExp(exp))
	{ printf("   name=%s\n", ((SgVarRefExp*)exp)->get_symbol()->get_declaration()->get_name().str()); }*/
	
	return // exp is valid if it is not part of a larger dot expression
	       (!isSgDotExp(exp->get_parent())) &&
			 // exp is a variable reference expression OR
	       ((isSgVarRefExp(exp) && !isSgDotExp(exp->get_parent())) ||
	       // exp is a dot expression that is a valid expression (evaluated recursively using isValidVarExp_rec())
	         (isSgDotExp(exp) && isValidVarExp_rec(exp)));
}

bool varID::isValidVarExp(SgInitializedName* exp)
{
	return true;
}

bool varID::isValidVarExp_rec(SgExpression* exp)
{
	ROSE_ASSERT(exp->get_parent());
	
	//printf("isValidVarExp_rec: exp->class_name()=%s  \"%s\"\n", exp->class_name().c_str(), exp->unparseToString().c_str());
	
	return // exp is a variable reference expression
	       isSgVarRefExp(exp) ||
	       // exp is a dot expression with an lhs that is a reference expression and an rhs that is a valid expression
	       (isSgDotExp(exp) && isValidVarExp_rec(((SgDotExp*)exp)->get_lhs_operand()) &&
	                           isSgVarRefExp(((SgDotExp*)exp)->get_rhs_operand()));
}

void varID::add(SgInitializedName *name)
{
	components.push_back(name);
}

// Adds the given annotation to this variable. Returns true if this causes the variable's
// annotation state to change and false otherwise.
bool varID::addAnnotation(const string& aName, void* annot)
{
	if(annotations.find(aName) != annotations.end() && 
	   annotations[aName] == annot)
	   return false;
	
	annotations[aName] = annot;
	return true;
}

// Remove the given annotation from this variable. Returns true if this variable
// previously had this annotation and false otherwise.
bool varID::remAnnotation(const string& aName)
{
	return annotations.erase(aName)>0;
}

// Remove the given annotation from this variable. Returns true if this variable
// previously had this annotation and false otherwise.
bool varID::remAllAnnotations()
{
	bool modified = annotations.size()>0;
	annotations.clear();
	return modified;
}

// Swap this variables annotations, removing [fromAnnotName -> fromAnnotVal] and replacing
// it with [toAnnotName -> toAnnotVal]. Returns true if this was successful (i.e. the variable
// does have the [fromAnnotName -> fromAnnotVal] annotation) and false otherwise.
// If the replacement occurs and this variable already has an annotation named
//    toAnnotName, this annotation's value is replaced by toAnnotVal.
bool varID::swapAnnotations(const string& fromAnnotName, void* fromAnnotVal,
                            const string& toAnnotName, void* toAnnotVal)
{
/*cout << "swapAnnotations("<<fromAnnotName<<", "<<toAnnotName<<") Before: ";
for(map<string, void*>::iterator it=annotations.begin(); it!=annotations.end(); it++)
	cout << it->first<<"->"<<it->second<<" "; 
cout << "\n";*/

	map<string, void*>::iterator locFrom = annotations.find(fromAnnotName);
	if(locFrom == annotations.end())
		return false;
	
	map<string, void*>::iterator locTo = annotations.find(toAnnotName);
	// If the to annotation already exists, replace its value with toAnnotVal
	if(locTo != annotations.end())
	{
		locTo->second = toAnnotVal;
	}
	else
	{
		annotations[toAnnotName] = toAnnotVal;
	}
	annotations.erase(locFrom);
	
/*cout << "swapAnnotations("<<fromAnnotName<<", "<<toAnnotName<<") After: ";
for(map<string, void*>::iterator it=annotations.begin(); it!=annotations.end(); it++)
	cout << it->first<<"->"<<it->second<<" "; 
cout << "\n";*/

	
	return true;
}

// If this variable has the annotation with the given name, returns it. Otherwise, returns NULL.
void* varID::getAnnotation(const string& aName) const
{
	map<string, void*>::const_iterator loc = annotations.find(aName);
	if(loc == annotations.end())
		return NULL;
	
	return loc->second;
}

// If this variable has the annotation with the given name, returns true. Otherwise, returns false.
bool varID::hasAnnotation(const string& aName) const
{
	map<string, void*>::const_iterator loc = annotations.find(aName);
	if(loc == annotations.end())
		return false;
	
	return true;
}

// If this variable has the annotation with ANY name in the given set, returns true. Otherwise, returns false.
bool varID::hasAnyAnnotation(const set<string>& aNames) const
{
	for(set<string>::const_iterator it=aNames.begin(); it!=aNames.end(); it++)
		// if the current annotation in aNames was found in annotations
		if(annotations.find(*it) != annotations.end())
			return true;
	
	return false;
}

// If this variable has the annotation with EACH name in the given set, returns true. Otherwise, returns false.
bool varID::hasAllAnnotations(const set<string>& aNames) const
{
	for(set<string>::const_iterator it=aNames.begin(); it!=aNames.end(); it++)
		// if the current annotation in aNames was NOT found in annotations
		if(annotations.find(*it) == annotations.end())
			return false;
	
	return true;
}

// Returns the total number of annotations associated with this variable
int varID::numAnnotations() const
{
	return annotations.size();
}

// Returns the full map of all the annotations associated with this variable
const map<string, void*>& varID::getAnnotations() const
{
	return annotations;
}

/******************
 *** COMPARISON ***
 ******************/

bool varID::equal(const varID& two) const
{
	/*if(components.size() != two.components.size())
		return false;
	if(annotations.size() != two.annotations.size())
		return false;
	
	{
		vector<SgInitializedName *>::const_iterator it_one, it_two;
		for(it_one=components.begin(), it_two=two.components.begin(); 
		    it_one!=components.end(); it_one++, it_two++)
		{
			if((*it_one)!=(*it_two))
				return false;
		}
	}
	
	{
		map<string, void*>::const_iterator it_one, it_two;
		for(it_one=annotations.begin(); it_one!=annotations.end(); it_one++)
		{
			
		}
		
		for(it_one=annotations.begin(), it_two=two.annotations.begin(); 
		    it_one!=annotations.end(); it_one++, it_two++)
		{
			if(it_one->first!=it_two->first && it_one->second!=it_two->second)
				return false;
		}
	}*/
	
	return components == two.components   &&
	       annotations == two.annotations &&
	       name == two.name;
}

bool varID::lessThan(const varID& two) const
{
/*	// if these two variables have no components, just compare their names
	if(components.size()==0 && two.components.size()==0)
		return name < two.name;

	int min_size = min(components.size(), two.components.size());
	
	vector<SgInitializedName *>::const_iterator it_one;
	vector<SgInitializedName *>::const_iterator it_two;
	int i=0;
	for(it_one=components.begin(), it_two=two.components.begin(); 
	    it_one!=components.end() && i<min_size; 
	    it_one++, it_two++)
	{
		if((*it_one)<(*it_two))
			return true;
		else if((*it_one)>(*it_two))
			return false;
		i++;
	}
	
	// we've iterated to the end of one of the lists and thus far they're equal
	
	// if the lists have the same lengths, then they actually are equal
	if(components.size() == two.components.size())
		return false;
	// else, whichever list is shorter is considered to be smaller
	else
		return components.size() < two.components.size();*/
	if(components < two.components) return true;
	else if(components == two.components)
	{
		if(annotations < two.annotations) return true;
		else if(annotations == two.annotations) return name < two.name;
	}
	return false;
}

bool varID::operator == ( const variable &that_arg ) const
{
	const varID &that = (const varID&) that_arg;
	return equal(that);
}
/*bool operator == ( const varID &one, const varID &two )
{
	return ((varID)one).equal((varID)two);
}*/

// two variables are equal if they're not unequal
bool varID::operator != ( const varID &that ) const
{
	return !(*this == that);
}

// ORDERING OPERATORS
// Variables are ordered in lexicographic order, with element-wise comparisons 
// performed using the basic < operator for pointers.
bool varID::operator < ( const variable &that_arg) const
{
	const varID &that = (const varID&) that_arg;
	return lessThan(that);
}

/*bool operator < ( const varID &one, const varID &two)
{
	return ((varID)one).lessThan((varID)two);
}*/


bool varID::operator > ( const varID &that ) const
{
	return !(*this < that) && !(*this == that);
}
/*bool operator > ( const varID &one, const varID &two)
{
	return !(one < two) && !(one == two);
}*/


bool varID::operator <= ( const varID &that ) const
{
	return *this == that || *this < that;
}

bool varID::operator >= ( const varID &that ) const
{
	return *this == that || *this > that;
}

bool operator == (const varID &var, SgInitializedName* iName)
{
	return (var.components.size() == 1 && var.components[0] == iName);
}
bool operator != (const varID &var, SgInitializedName* iName)
{ return !(var == iName); }
bool operator == (SgInitializedName* iName, const varID &var)
{ return (var == iName); }
bool operator != (SgInitializedName* iName, const varID &var)
{ return !(var == iName); }

bool operator == (const varID &var, SgExpression* expr)
{
	varID exprVar(expr);
	return var == exprVar;
}
bool operator != (const varID &var, SgExpression* expr)
{ return !(var == expr); }
bool operator == (SgExpression* expr, const varID &var)
{ return var == expr; }
bool operator != (SgExpression* expr, const varID &var)
{ return !(var == expr); }

	
/**************
 *** OUTPUT ***
 **************/

// string representation of the variable reference
/*const char* varID::str()
{
	return getString().c_str();
}*/

string varID::str() const
{
//	printf("components.size()=%d\n", components.size());
	ostringstream outs;
	
	// if this is a regular variable
	if(components.size()>0)
	{
		for(vector<SgInitializedName *>::const_iterator it = components.begin();
			 it!=components.end(); )
		{
			outs << (*it)->get_name().getString();
//printf("varID()::getString)() this=0x%x *it=0x%x name=%s\n", this, *it, (*it)->get_name().getString().c_str());
//			printf("(*it)->get_name().str()=%s\n", (*it)->get_name().getString().c_str());
			
			it++;
			if(it!=components.end())
				outs << ".";
		}
	}
	// else, if this is a special variable
	else
		outs << name;
	
	for(map<string, void*>::const_iterator it=annotations.begin(); it!=annotations.end(); it++)
	{
		outs << "|" << it->first << "->" << it->second;
	}
	
	return outs.str();
}

// string representation of the variable reference, with variable/field names augmented with 
// the line numbers where they were defined. File names are omitted for brevity	
/*const char* varID::str_linenum()
{
	return getString_linenum().c_str();
}*/

string varID::str_linenum() const
{
//		printf("components.size()=%d\n", components.size());
	string out="";
	// if this is a regular variable
	if(components.size()>0)
	{
		for(vector<SgInitializedName *>::const_iterator it = components.begin();
			 it!=components.end(); )
		{
			stringstream ConvertStream;
			ConvertStream << "["<<(*it)->get_name().getString()<<"|"<<(*it)->get_file_info()->get_line()<<"]";
			ConvertStream >> out;
//			printf("(*it)->get_name().str()=%s\n", (*it)->get_name().getString().c_str());
			
			it++;
			if(it!=components.end())
				out+=".";
		}
	}
	// else, if this is a special variable
	else
		out = name;
			
	return out;
}

// string representation of the variable reference, with the variable names replaced 
// with the pointers to their declarations
/*const char* varID::str_ptr()
{
	return getString_ptr().c_str();
}*/

string varID::str_ptr() const
{
	string out="";
	// if this is a regular variable
	if(components.size()>0)
	{
		int i;
		for(vector<SgInitializedName *>::const_iterator it = components.begin();
			 it!=components.end(); )
		{
			// enough space for '0x' + 8 hex digits + NULL terminator 
			char txt[11];
			snprintf(txt, 11, "0x%x", *it);
			out += txt;
//			printf("(*it)->get_name().str()=%s\n", (*it)->get_name().getString().c_str());
			
			it++;
			if(it!=components.end())
				out+=".";
			i++;
		}
	}
	// else, if this is a special variable
	else
		out = name;
	return out;
}

// returns true if the last field in this variable has an array or pointer type
bool varID::isArrayType() const
{
	// if this is a regular variable
	if(components.size()>0)
		return (isSgArrayType(components[components.size()-1]->get_typeptr())) ||
		       (isSgPointerType(components[components.size()-1]->get_typeptr()));
	else
		return false;
}

// returns true if any of its constituent SgInitializedNames are compiler-generated
bool varID::isCompilerGenerated() const
{
	for(vector<SgInitializedName *>::const_iterator it = components.begin();
		 it!=components.end(); it++)
	{
		if((*it)->get_file_info()->isCompilerGenerated()) return true;
	}
	return false;
}

// returns true if this variable is global and false otherwise
bool varID::isGlobal() const
{
	if(components.size()==0) return false;
	
	SgScopeStatement* nameScope = components[0]->get_scope();
	
	// this variable is global if...
	// components[0] refers to a variable declaration with a globally visible scope if ...
	return 	
		// initName refers to a variable rather than a function AND
		!isSgFunctionType(components[0]->get_type()) &&
		// this SgInitializedName is part of a variable declaration rather than a function argument list AND
		isSgVariableDeclaration(components[0]->get_parent()) &&
		// and it has file-scope
		(isSgGlobal(nameScope) || isSgNamespaceDefinitionStatement(nameScope));
}

// Unique ID generation and access functionality
// the maximum ID that has been generated for any variable
long varID::globalMaxID=0;
	
// generates a new ID for this variable and stores it in ID
void varID::genID()
{
	ID = globalMaxID;
	globalMaxID++;
}

// returns this variable's ID
long varID::getID() const
{
	return ID;
}

// translate from an expression that uses a variable to that variable's unique id
// currently supported: a (SgVarRefExp), a.i (SgDotExp), 
/*varID
getVarReference( SgVarRefExp *exp )
{
	ROSE_ASSERT ( exp );
	varID x = (varID)(exp->get_symbol()->get_declaration());
	return x;
}

varID
getVarReference( SgDotExp *exp )
{
	ROSE_ASSERT ( exp );
	SgVarRefExp *l = isSgVarRefExp( exp->get_lhs_operand() ),
		*r = isSgVarRefExp( exp->get_rhs_operand() );
	if ( r && l )
		{
			varID x = (( (varID)(l->get_symbol()->get_declaration()) ) << 32) ^
				(varID)( r->get_symbol()->get_declaration() );
			return x;
		}
	else
		{
			cout << exp->unparseToString() << "\thas issues\n";
			assert (0);
		}
}

// converts from a pointer to SgInitializedName (32-bits) to a full 64-bit unique id
varID
getVarReference( SgInitializedName *iN )
{
	ROSE_ASSERT ( iN );
	return (varID)iN;
}*/

varID zeroVar("ZERO");
varID oneVar("ONE");
varID allVar("ALL");

// add the (id->name) mappings for the two special variables: Zero and One
void addPredefinedVars(varIDSet &vars)
{
	vars.insert(zeroVar);
}


// returns whether the variable with the given id exists in our set of interest
bool existsVariable(SgFunctionDefinition* func, varID x )
{
	return ( activeVars[func].find( x ) != activeVars[func].end() );
}

// returns whether the variable in the given reference expression exists in our 
// set of interest
/*bool existsVariable( SgVarRefExp *var, m_varID2str &vars2Name )
{
	ROSE_ASSERT ( var );
	SgInitializedName *initName = var->get_symbol()->get_declaration();
	ROSE_ASSERT ( initName );

	return ( vars2Name.find( (varID)initName ) != vars2Name.end() );
}*/


// returns whether this is a variable reference or dot expression (field reference)
/*bool isTypeConsidered( SgNode *exp )
{
	return isSgVarRefExp( exp ) || isSgDotExp( exp );
}*/

// returns the id in a variable reference or dot expression (field reference)
/*varID getRefOfTypeConsidered( SgNode *exp )
{
	if ( isSgVarRefExp( exp ) )
		return getVarReference( isSgVarRefExp( exp ) );
	else
		if ( isSgDotExp( exp ) )
			return getVarReference( isSgDotExp( exp ) );
		else
			assert (0);
}
*/

// inherited attribute that allows us to tell whether a given SgVarRefExp is independent
// or part of some SgDotExp
/*class varStatus
{
	public;
	bool independent;
	
	varStatus()
	{
		independent=true;
	}
	
	varStatus(varStatus &s)
	{
		independent = s.independent;
	}
}*/

class varIDCollector : public AstBottomUpProcessing<varIDSet>
{
	set<rwAccessLabeler::accessType> access;
	
	public:
	varIDCollector()
	{
		// by default collect all access types
		access.insert(rwAccessLabeler::readAccess);
		access.insert(rwAccessLabeler::writeAccess);
		access.insert(rwAccessLabeler::rwAccess);
	}
	
	varIDCollector(rwAccessLabeler::accessType access)
	{ 
		this->access.insert(access);
	}
	
	varIDCollector(set<rwAccessLabeler::accessType>& access)
	{ 
		this->access = access;
	}
		
	varIDSet evaluateSynthesizedAttribute(SgNode* n, SynthesizedAttributesList children )
	{
		// list of variables inside this subtree
		varIDSet res;
		
		//printf("varIDCollector : n = <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str());
		
		SgExpression* expr;
		if((expr = isSgExpression(n)) && varID::isValidVarExp(expr))
		{
			// if we're capturing accesses of the type being performed at node n
			if(access.find(rwAccessLabeler::getAccessType(n)) != access.end())
			{
				// add this variable to the list
				varID newVar(expr);
				//printf(" adding variable %s\n", newVar.str().c_str());
				res.insert(newVar);
			}
		}
		// if n is not a variable node, res = union of children lists
		else
		{
			for(SynthesizedAttributesList::iterator it=children.begin();
			    it!=children.end(); it++)
			{
				for(varIDSet::iterator it2=(*it).begin();
			       it2!=(*it).end(); it2++)
			   {
			   	res.insert(*it2);
			   }
			}
		}
		
		return res;
	}
};

// returns a set of varIDs that correspond to all the variables (of SgDotExp/SgVarRefExp form)
// that were referenced in the given subtree
varIDSet getVarRefsInSubtree(SgNode* root)
{
	varIDCollector c;
	return c.traverse(root);
}

// returns a set of varIDs that correspond to all the variables (of SgDotExp/SgVarRefExp form)
// that were read from in the given subtree
varIDSet getReadVarRefsInSubtree(SgNode* root)
{
	set<rwAccessLabeler::accessType> access;
	access.insert(rwAccessLabeler::readAccess);
	access.insert(rwAccessLabeler::rwAccess);
	varIDCollector c(access);
	return c.traverse(root);
}

// returns a set of varIDs that correspond to all the variables (of SgDotExp/SgVarRefExp form)
// that were written to in the given subtree
varIDSet getWriteVarRefsInSubtree(SgNode* root)
{
	set<rwAccessLabeler::accessType> access;
	access.insert(rwAccessLabeler::writeAccess);
	access.insert(rwAccessLabeler::rwAccess);
	varIDCollector c(access);
	return c.traverse(root);
}

class arrayVarIDCollector : public AstBottomUpProcessing<varIDSet>
{
	public:
	varIDSet evaluateSynthesizedAttribute(SgNode* n, SynthesizedAttributesList children )
	{
		// list of variables inside this subtree
		varIDSet res;
		
		//printf("varIDCollector : n = <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str());
		
		SgExpression* expr;
		if(expr = getArrayNameExp(n))
		{
			// add this variable to the list
			varID newVar(expr);
			//printf(" adding variable %s\n", newVar.str().c_str());
			res.insert(newVar);
		}
		// if n is not a variable node, res = union of children lists
		else
		{
			for(SynthesizedAttributesList::iterator it=children.begin();
			    it!=children.end(); it++)
			{
				for(varIDSet::iterator it2=(*it).begin();
			       it2!=(*it).end(); it2++)
			   {
			   	res.insert(*it2);
			   }
			}
		}
		
		return res;
	}
};

// returns a set of varIDs that correspond to all the variables (of SgDotExp/SgVarRefExp form)
// that were referenced as arrays (i.e. var[i]) in the given subtree
varIDSet getArrayVarRefsInSubtree(SgNode* root)
{
	// make sure that all the array-index annotations have been placed
	addArrayIndexAnnotations(cfgUtils::project);
	arrayVarIDCollector c;
	return c.traverse(root);
}


/*********************************
 ********* arrayElt CODE *********
 *********************************/

arrayElt::arrayElt(SgNode* n)
{
	arrIndexLabeler::addArrayIndexAnnotations(cfgUtils::getProject());
	
	if(arrIndexLabeler::isTopArrayRefExp(n))
	{
		SgPntrArrRefExp* rExpr;
		ROSE_ASSERT(rExpr = isSgPntrArrRefExp(n));
		arrayVar.init(arrIndexLabeler::getArrayNameExp(rExpr));
		indexExprs = &arrIndexLabeler::getArrayIndexExprs(rExpr);
	}
	else
		indexExprs = NULL;
} 

arrayElt::arrayElt(SgExpression* expr)
{
	arrIndexLabeler::addArrayIndexAnnotations(cfgUtils::getProject());
	
	if(arrIndexLabeler::isTopArrayRefExp(expr))
	{
		SgPntrArrRefExp* rExpr;
		ROSE_ASSERT(rExpr = isSgPntrArrRefExp(expr));
		arrayVar.init(arrIndexLabeler::getArrayNameExp(rExpr));
		indexExprs = &arrIndexLabeler::getArrayIndexExprs(rExpr);
	}
	else
		indexExprs = NULL;
}

string arrayElt::str() const
{
	ostringstream outs;
	
	outs << arrayVar.str();
	for(list<SgExpression*>::const_iterator it = indexExprs->begin(); it!=indexExprs->end(); it++)
		outs << "[" << (*it)->unparseToString() << "]";
	
	return outs.str();
}

bool arrayElt::operator == (const variable &that_arg) const
{
	const arrayElt& that = (const arrayElt&)that_arg;
	
	if(arrayVar != that.arrayVar) return false;
	if(indexExprs != that.indexExprs) return false;

	return true;		
}

bool arrayElt::operator <  (const variable &that_arg) const
{
	const arrayElt& that = (const arrayElt&)that_arg;
	if(arrayVar < that.arrayVar) return true;
	if(arrayVar > that.arrayVar) return false;
	if(indexExprs->size() < that.indexExprs->size()) return true;
	if(indexExprs->size() > that.indexExprs->size()) return false;
		
	list<SgExpression*>::const_iterator itThis, itThat;
	for(itThis = indexExprs->begin(), itThat = that.indexExprs->begin();
	    itThis!=indexExprs->end(), itThat!=indexExprs->end();
	    itThis++, itThat++)
	{
		if(*itThis < *itThat) return true;
		if(*itThis > *itThat) return false;
	}

	// this == that
	return false;
}

// returns true if the given expression is one that can be represented as a variable in our representation
bool arrayElt::isValidVarExp(SgExpression* exp)
{
	arrIndexLabeler::addArrayIndexAnnotations(cfgUtils::getProject());
	return isTopArrayRefExp(exp);
}

// returns the scope in which this array variable was declared
// for compound variables (i.e. those composed of dot expressions), it is the scope of the leftmost name
SgScopeStatement* arrayElt::getScope() const
{
	return arrayVar.getScope();
}

// returns a referenec to the array variable, without any indexes
const varID& arrayElt::getArrayVar()
{ return arrayVar; }

// returns a reference to the index expressions
list<SgExpression*>* arrayElt::getIndexExprs()
{ return indexExprs; }

// returns a SgExpression that corresponds to an access of this variable
SgExpression* arrayElt::toSgExpression() const
{
	SgExpression* indexesSubtree;
	if(indexExprs->size()==0)
		return arrayVar.toSgExpression();
	else
		return toSgExpression_rec(indexExprs->rbegin());
}

// returns the SgPntrArrRefExp that corresponds to the index expressions in indexExprs that start with itIndexes
// precondition : itIndexes!=index.end()
SgPntrArrRefExp* arrayElt::toSgExpression_rec(list<SgExpression*>::reverse_iterator itIndexes) const
{
	SgTreeCopy tc;
	SgExpression* curIndexExpr = *itIndexes;
	itIndexes++;
	if(itIndexes==indexExprs->rend())
		return new SgPntrArrRefExp(SgDefaultFile, arrayVar.toSgExpression(), isSgExpression(curIndexExpr->copy(tc)));
	else
		return new SgPntrArrRefExp(SgDefaultFile, toSgExpression_rec(itIndexes), isSgExpression(curIndexExpr->copy(tc)));
}

// returns true if this variable is global and false otherwise
bool arrayElt::isGlobal() const
{
	return arrayVar.isGlobal();
}

class ArrayEltCollector : public AstBottomUpProcessing<set<arrayElt> >
{
	public:
	set<arrayElt> evaluateSynthesizedAttribute(SgNode* n, SynthesizedAttributesList children )
	{
		// list of variables inside this subtree
		set<arrayElt> res;
		
		//printf("varIDCollector : n = <%s | %s>\n", n->unparseToString().c_str(), n->class_name().c_str());
		
		SgExpression* expr;
		if((expr = isSgExpression(n)) && arrayElt::isValidVarExp(expr))
		{
			// add this variable to the list
			arrayElt newVar(expr);
			//printf(" adding variable %s\n", newVar.str().c_str());
			res.insert(newVar);
		}
		// if n is not a variable node, res = union of children lists
		else
		{
			for(SynthesizedAttributesList::iterator it=children.begin();
			    it!=children.end(); it++)
			{
				for(set<arrayElt>::iterator it2=(*it).begin(); it2!=(*it).end(); it2++)
			   {
			   	res.insert(*it2);
			   }
			}
		}
		
		return res;
	}
};

// returns a set of arrayElts that correspond to all the arrays (of SgDotExp/SgVarRefExp[SgExpression][][][]... form)
// that were referenced in the given subtree
set<arrayElt> getArrayRefsInSubtree(SgNode* root)
{
	ArrayEltCollector c;
	return c.traverse(root);	
}
