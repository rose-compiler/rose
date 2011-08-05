// We need this header file so that we can use the mechanisms within ROSE to build a preprocessor
#include "arrayPreprocessor.h"

// ************************************************
// Function definitions for ArrayStatementTraversal
// ************************************************

ArrayStatementTraversal::ArrayStatementTraversal() {
	// Nothing to do here
}

ProgramTransformationInheritedAttributeType ArrayStatementTraversal::evaluateInheritedAttribute(
		SgNode* astNode, ProgramTransformationInheritedAttributeType inheritedAttribute) {
	// Call the copy constructor to build the new inherited attribute that knows about the
	// current location (astNode) or call a member function to update the current copy of
	// the inherited attribute if making a copy is too expensive.
	ProgramTransformationInheritedAttributeType locatedInheritedAttribute(inheritedAttribute,
			astNode);

#if DEBUG
	printf("In ArrayStatementTraversal::evaluateInheritedAttribute \n");
#endif

	if (astNode->variantT() == V_SgExprStatement) {
			expList.push_back(isSgExprStatement(astNode));
			inheritedAttributeList.push_back(inheritedAttribute);
		}

	return inheritedAttribute;
}


void ArrayStatementTraversal::atTraversalStart() {

#if DEBUG
	cout << " ============= Start of ArrayStatementTraversal Called =========" << endl;
#endif

}

void ArrayStatementTraversal::atTraversalEnd() {

#if DEBUG
	cout << " ============= End of ArrayStatementTraversal Called =========" << endl;
#endif

	// Perform the transformation at the end of traversal
	for (int i = 0; i < expList.size(); i++) {
		SgExprStatement* exprStatement = expList[i];
		ProgramTransformationInheritedAttributeType inheritedAttribute = inheritedAttributeList[i];

		// Create variable declarations only once
		if (i == 0)
			ArrayAssignmentStatementTransformation::createVariableDeclarations(exprStatement, 6);

		arrayAssignmentTransformation(inheritedAttribute, exprStatement);
	}
}

bool ArrayStatementTraversal::arrayAssignmentTransformation(
		const ProgramTransformationInheritedAttributeType & inheritedAttribute, SgNode* astNode) {
	// The purpose of this transformation is to insert the array transformation in place of each array
	// statement.  This is a function defined in the class derived from the TransformationSpecificationType.
	// This function is a pure virtual function within the TransformationSpecificationType base class.

	// This function requires that the inherited attribute be passed so that we can pass it along to
	// the nested transformation.

#if DEBUG
	printf(" In ArrayStatementTraversal::arrayAssignmentTransformation \n");
#endif

	ROSE_ASSERT (astNode != NULL);
	ROSE_ASSERT (isSgExprStatement(astNode) != NULL);

	ArrayAssignmentStatementQuerySynthesizedAttributeType gatheredInfo;
	if (ArrayAssignmentStatementTransformation::targetForTransformation(astNode) == true) {

#if DEBUG
		printf ("Calling ArrayAssignmentStatementTransformation::transformation() \n");
#endif
		gatheredInfo = ArrayAssignmentStatementTransformation::transformation(inheritedAttribute,
				astNode);

	}

	// This copies and returns the SynthesizedAttributeBaseClassType information
	//return gatheredInfo;
	return true;
}

