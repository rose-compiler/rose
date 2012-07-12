#ifndef ROSE_ARRAY_ASSIGNMENT_STATEMENT_QUERY_INHERITED_ATTRIBUTE_H
#define ROSE_ARRAY_ASSIGNMENT_STATEMENT_QUERY_INHERITED_ATTRIBUTE_H

// Base class common to all inherited attributes used for array statement queries (transformations)
// #include "arrayInheritedAttributeBaseClass.h"

class ArrayAssignmentStatementQueryInheritedAttributeType
: public ArrayStatementQueryInheritedAttributeType
{
private:
	// Hide the default constructor (since the base class requires a SgNode pointer)
	ArrayAssignmentStatementQueryInheritedAttributeType();

public:
	bool skipSubstitutionOfSubscriptComputationMacro;
	bool usingIndexObjectsInSubscriptComputation;

	// Make the attribute as being indexed so that in the assembly we can generate the correct
	// transformation
	bool isIndexedArrayOperand;

	// Destructors and Constructors
	~ArrayAssignmentStatementQueryInheritedAttributeType();

	ArrayAssignmentStatementQueryInheritedAttributeType ( SgNode* astNode );

	// Copy constructor and assignment operator
	ArrayAssignmentStatementQueryInheritedAttributeType
	( const ArrayAssignmentStatementQueryInheritedAttributeType & X );
	ArrayAssignmentStatementQueryInheritedAttributeType
	( const ArrayStatementQueryInheritedAttributeType & X,
			SgNode* astNode );
//        ArrayAssignmentStatementQueryInheritedAttributeType
//             ( const ArrayAssignmentStatementQueryInheritedAttributeType & X,
//               SgNode* astNode );

	ArrayAssignmentStatementQueryInheritedAttributeType & operator=
	( const ArrayAssignmentStatementQueryInheritedAttributeType & X );

	// access functions
	list<int> & getTransformationOptions () const;
	void setTransformationOptions ( const list<int> & X );

	bool getSkipSubstitutionOfSubscriptComputationMacro () const;
	void setSkipSubstitutionOfSubscriptComputationMacro ( bool newValue );

	bool getUsingIndexObjectsInSubscriptComputation () const;
	void setUsingIndexObjectsInSubscriptComputation ( bool newValue );

	// Access functions for isIndexedArrayOperand variable
	bool getIsIndexedArrayOperand ();
	void setIsIndexedArrayOperand ( bool value );

	// output class data for debugging
	void display ( const char* label = "" ) const;
};

// endif for ROSE_ARRAY_ASSIGNMENT_STATEMENT_QUERY_INHERITED_ATTRIBUTE_H
#endif
