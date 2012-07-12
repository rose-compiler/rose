#ifndef ROSE_PROGRAM_TRANSFORMATION_H
#define ROSE_PROGRAM_TRANSFORMATION_H

// This header file defines the outermost use of the tree traversal
// mechanism to traverse the AST and calls nested traversals specific
// to array statement transformation (only array expression statements
// are targeted for transformation).  The transformation can be any of
// several different types and are handled through nested tree
// traversals.

typedef ArrayStatementQueryInheritedAttributeType ProgramTransformationInheritedAttributeType;

class ArrayStatementTraversal
: public SgTopDownProcessing<ProgramTransformationInheritedAttributeType>
{
public:
	ArrayStatementTraversal ();

	// Functions required by the AST Rewrite Tree Traversal mechanism
	ProgramTransformationInheritedAttributeType
	evaluateInheritedAttribute (
			SgNode* astNode,
			ProgramTransformationInheritedAttributeType inheritedValue );


	// Simple array statement transformations
	bool
	arrayAssignmentTransformation (
			const ProgramTransformationInheritedAttributeType & inheritedAttribute,
			SgNode* astNode );

	// Transformations on scalar indexing expressions in statements
	bool
	arrayScalarIndexingTransformation (
			const ProgramTransformationInheritedAttributeType & inheritedAttribute,
			SgNode* astNode );

	void atTraversalStart();
	void atTraversalEnd();
private:
	vector<SgExprStatement*> expList;
	vector<ProgramTransformationInheritedAttributeType> inheritedAttributeList;
};

// endif for ROSE_PROGRAM_TRANSFORMATION_H
#endif

