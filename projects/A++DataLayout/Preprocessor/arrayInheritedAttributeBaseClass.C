// This is the one header file which all source code requires
#include "arrayPreprocessor.h"

// #include "algorithm.h"
#include "algo.h"

ArrayStatementQueryInheritedAttributeType::~ArrayStatementQueryInheritedAttributeType() {
	// Nothing to do here
	arrayStatementDimensionDefined = false;
	arrayStatementDimension = -1;
}

ArrayStatementQueryInheritedAttributeType::ArrayStatementQueryInheritedAttributeType() {
	// This constructor should never be called
	printf(
			"In ArrayStatementQueryInheritedAttributeType: Base class initialized with NULL pointer \n");

	arrayStatementDimensionDefined = false;
	arrayStatementDimension = -1;
}

ArrayStatementQueryInheritedAttributeType::ArrayStatementQueryInheritedAttributeType(
		SgNode* astNode) {
	arrayStatementDimensionDefined = false;
	arrayStatementDimension = -1;
}

ArrayStatementQueryInheritedAttributeType::ArrayStatementQueryInheritedAttributeType(
		const ArrayStatementQueryInheritedAttributeType & X) {
	// Notice that because of the base class, it is best to not use the operator= to implement the
	// copy constructor.
	// operator=(X);

	//printf(" ArrayStatementQueryInheritedAttributeType: Copy Constructor called \n");

	// Copy the local data members
	transformationOptions = X.transformationOptions;
	arrayDimensions = X.arrayDimensions;
	arrayStatementDimensionDefined = X.arrayStatementDimensionDefined;
	arrayStatementDimension = X.arrayStatementDimension;
}

ArrayStatementQueryInheritedAttributeType::ArrayStatementQueryInheritedAttributeType(
		const ArrayStatementQueryInheritedAttributeType & X, SgNode* astNode) {
	// Notice that because of the base class, it is best to not use the operator= to implement the
	// copy constructor.
	// operator=(X);

	//printf(" ArrayStatementQueryInheritedAttributeType: Copy Constructor with astNode called \n");

	// Copy the local data members
	transformationOptions = X.transformationOptions;
	arrayDimensions = X.arrayDimensions;
	arrayStatementDimensionDefined = X.arrayStatementDimensionDefined;
	arrayStatementDimension = X.arrayStatementDimension;
}

ArrayStatementQueryInheritedAttributeType &
ArrayStatementQueryInheritedAttributeType::operator=(
		const ArrayStatementQueryInheritedAttributeType & X) {
	// Call the base class operator=
	// InheritedAttributeBaseClassType::operator=(X);

	// Copy the local data members
	transformationOptions = X.transformationOptions;
	arrayDimensions = X.arrayDimensions;
	arrayStatementDimensionDefined = X.arrayStatementDimensionDefined;
	arrayStatementDimension = X.arrayStatementDimension;

	return *this;
}

bool ArrayStatementQueryInheritedAttributeType::isAssertedByUser(int option) const {
	// Return true if the option is found in the list of options

	printf(
			"Inside of ArrayStatementQueryInheritedAttributeType::isAssertedByUser() option = %d transformationOptions.size() = %d \n",
			option, transformationOptions.size());

	list<int>::const_iterator i;
	int counter = 0;
	for (i = transformationOptions.begin(); i != transformationOptions.end(); i++) {
		printf("List element #%d = %d \n", counter++, *i);
	}

	bool returnValue = false;
	list<int>::const_iterator location;
	location = find(transformationOptions.begin(), transformationOptions.end(), option);

	// If the element is not in the list (or the list is zero length) then "find()" returns the end iterator.
	returnValue = (location == transformationOptions.end()) ? false : true;

	printf("In ArrayStatementQueryInheritedAttributeType::isAssertedByUser() returnValue = %s \n",
			returnValue ? "true" : "false");

	return returnValue;

}

