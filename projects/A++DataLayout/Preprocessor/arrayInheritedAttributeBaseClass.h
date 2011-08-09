#ifndef ROSE_ARRAY_INHERITED_ATTRIBUTE_BASE_CLASS_H
#define ROSE_ARRAY_INHERITED_ATTRIBUTE_BASE_CLASS_H

#include <list>
using namespace std;

class ArrayStatementQueryInheritedAttributeType
{
	// Define an inherited attribute that is common to all array statement transformations. It is
	// derived from an inherited attribute base class common to all transformations in ROSE (this is
	// not currently required).

	// It seems that all the array statement transformation share a common need for:
	// 1) The options specified by the user to control the preprocessor generation of transformations
	// 2) information about the dimension of the array operands associated with the query (or transformation).

	//private:
public:
	// enum values from user-defined enum declaration
	list<int> transformationOptions;

	// list of array dimensions associated with each array operand
	list<int> arrayDimensions;

	// The dimension of an array statement might not be well defined if it mixes different 
	bool arrayStatementDimensionDefined;
	int arrayStatementDimension;

	~ArrayStatementQueryInheritedAttributeType ();

	//private:
	// Make the default constructor unavailable to derived classes
	ArrayStatementQueryInheritedAttributeType ();

	//public:
	ArrayStatementQueryInheritedAttributeType ( SgNode* astNode );

	ArrayStatementQueryInheritedAttributeType ( const ArrayStatementQueryInheritedAttributeType & X );
	ArrayStatementQueryInheritedAttributeType (
			const ArrayStatementQueryInheritedAttributeType & X, SgNode* astNode );
	ArrayStatementQueryInheritedAttributeType & operator= (
			const ArrayStatementQueryInheritedAttributeType & X );

	bool isAssertedByUser ( int option ) const;
};

// endif matching define ROSE_ARRAY_INHERITED_ATTRIBUTE_BASE_CLASS_H
#endif

