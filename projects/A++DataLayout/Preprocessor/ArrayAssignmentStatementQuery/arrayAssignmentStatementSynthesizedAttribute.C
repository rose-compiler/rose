#include "arrayPreprocessor.h"

// generate a display string of the information in the index operand
string ArrayAssignmentStatementQuerySynthesizedAttributeType::displayString() const {
	//string returnString = SynthesizedAttributeBaseClassType::displayString() + "[ Workspace: ";
	string returnString = "[ Workspace: ";
	returnString += workspace;
//     if(isSgExpression(lhsNode))
//    	 returnString += lhsNode->unparseToString();
	returnString += "] ";

	return returnString;
}

void ArrayAssignmentStatementQuerySynthesizedAttributeType::display(const string & label) const {
	printf("In ArrayAssignmentStatementTransformation::display(%s) \n", label.c_str());
	string internalDataString = displayString();
	// printf ("internalDataString = %s \n",(internalDataString.c_str() != NULL) ? internalDataString.c_str() : "NULL STRING");
	printf("internalDataString = %s \n", internalDataString.c_str());
}

#if 1
ArrayAssignmentStatementQuerySynthesizedAttributeType::ArrayAssignmentStatementQuerySynthesizedAttributeType() {
	// default is to assume no loop dependence
	loopDependence = FALSE;
	workspace = "";
}
#endif

ArrayAssignmentStatementQuerySynthesizedAttributeType::ArrayAssignmentStatementQuerySynthesizedAttributeType(
		SgNode* astNode) {
	// default is to assume no loop dependence
	loopDependence = FALSE;
	workspace = "";
}

ArrayAssignmentStatementQuerySynthesizedAttributeType::~ArrayAssignmentStatementQuerySynthesizedAttributeType() {
	printf("Inside of ~ArrayAssignmentStatementQuerySynthesizedAttributeType destructor (this = %p) \n", this);

	loopDependence = FALSE;
	workspace = "";
}

// ##########################################
//        Constructors and Operators
// ##########################################
ArrayAssignmentStatementQuerySynthesizedAttributeType::ArrayAssignmentStatementQuerySynthesizedAttributeType(
		const ArrayAssignmentStatementQuerySynthesizedAttributeType & X)
// : SynthesizedAttributeBaseClassType(X.associated_AST_Node)
		{
	// printf ("In ArrayAssignmentStatementQuerySynthesizedAttributeType copy constructor (this = %p) \n",this);

	// Deep copy semantics
	((ArrayAssignmentStatementQuerySynthesizedAttributeType*) this)->operator=(X);
}

#if 0
// Assignment of the base class data
ArrayAssignmentStatementQuerySynthesizedAttributeType &
ArrayAssignmentStatementQuerySynthesizedAttributeType::
operator= ( const SynthesizedAttributeBaseClassType & X )
{
	printf ("In ArrayAssignmentStatementQuerySynthesizedAttributeType operator=(SynthesizedAttributeBaseClassType) (this = %p) \n",this);

	SynthesizedAttributeBaseClassType::operator= ( X );

	loopDependence = false;
	loopDependenceLhs = "";
	loopDependenceRhs = "";

	workspace = "";

	return *this;
}
#endif

ArrayAssignmentStatementQuerySynthesizedAttributeType &
ArrayAssignmentStatementQuerySynthesizedAttributeType::operator=(
		const ArrayAssignmentStatementQuerySynthesizedAttributeType & X) {
	// printf ("In ArrayAssignmentStatementQuerySynthesizedAttributeType operator=(ArrayAssignmentStatementQuerySynthesizedAttributeType) (this = %p) \n",this);

	//SynthesizedAttributeBaseClassType::operator= ( X );

	loopDependence = X.loopDependence;
	loopDependenceLhs = X.loopDependenceLhs;
	loopDependenceRhs = X.loopDependenceRhs;

	workspace = X.workspace;

	return *this;
}

ArrayAssignmentStatementQuerySynthesizedAttributeType &
ArrayAssignmentStatementQuerySynthesizedAttributeType::operator+=(
		const ArrayAssignmentStatementQuerySynthesizedAttributeType & X) {
	// This function is called by the arrayAssignmentStatementQueryAssemblyFunction to handle the
	// assembly of systhesized attributes for the default case.  It implements the user's policy
	// to handle the workspace string.

	// printf ("In ArrayAssignmentStatementQuerySynthesizedAttributeType operator+=(ArrayAssignmentStatementQuerySynthesizedAttributeType) (this = %p) \n",this);

	//SynthesizedAttributeBaseClassType::operator+= ( X );

	// Or the loop dependence in case we have A = B = A (in case this helps)
	loopDependence = loopDependence || X.loopDependence;

	// Don't overwrite unless we have valid strings (there should only be one synthesized attribute
	// with a valid string)
	loopDependenceLhs = (X.loopDependenceLhs.length() > 0) ? X.loopDependenceLhs : string("");
	loopDependenceRhs = (X.loopDependenceRhs.length() > 0) ? X.loopDependenceRhs : string("");

	// Accumulate the workspace strings (it is up to the derived classes' 
	// operator+=() to implement the handling of the workspace).
	workspace += X.workspace;

	// printf("In ArrayAssignmentStatementQuerySynthesizedAttributeType::operator+=(): variableName = %s \n",variableName.c_str());

	// combine database info (put arg2 into arg1)
	// ArrayOperandDataBase::merge(arrayOperandList,X.arrayOperandList);

	return *this;
}

string ArrayAssignmentStatementQuerySynthesizedAttributeType::getWorkSpace() const {
	// access function for loopDependence variable
	return workspace;
}

void ArrayAssignmentStatementQuerySynthesizedAttributeType::setWorkSpace(string X) {
	// access function for loopDependence variable
	workspace = X;
}

bool ArrayAssignmentStatementQuerySynthesizedAttributeType::getLoopDependence() const {
	// access function for loopDependence variable
	return loopDependence;
}

void ArrayAssignmentStatementQuerySynthesizedAttributeType::setLoopDependence(bool value) {
	// access function for loopDependence variable
	loopDependence = value;
}

string ArrayAssignmentStatementQuerySynthesizedAttributeType::getLoopDependenceLhs() const {
	// access function for loopDependence variable
	return loopDependenceLhs;
}

void ArrayAssignmentStatementQuerySynthesizedAttributeType::setLoopDependenceLhs(string X) {
	// access function for loopDependence variable
	loopDependenceLhs = X;
}

string ArrayAssignmentStatementQuerySynthesizedAttributeType::getLoopDependenceRhs() const {
	// access function for loopDependenceLhs variable
	return loopDependenceRhs;
}

void ArrayAssignmentStatementQuerySynthesizedAttributeType::setLoopDependenceRhs(string X) {
	// access function for loopDependenceRhs variable
	loopDependenceRhs = X;
}

