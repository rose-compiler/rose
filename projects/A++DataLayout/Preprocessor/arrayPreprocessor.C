// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it show the desing of a transformation to be used with an AST Restructuring Tool
// built by ROSETTA.

// Main header file for array preprocessor
#include "arrayPreprocessor.h"


void ArrayPreprocessor(SgProject* project) {

	// Call the preprocessor
	ArrayStatementTraversal treeTraversal;

	// Ignore the return value since we don't need it
	ProgramTransformationInheritedAttributeType inheritedAttribute(project);
	treeTraversal.traverseInputFiles(project, inheritedAttribute);

	return;
}

