/*
 * specificationTraversal.C
 *
 *  Created on: Jul 11, 2011
 *      Author: sharma9
 */

#include "rose.h"
#include <string>
#include <iostream>

#include "transformationWorklist.h"
#include "layoutOptions.h"
using namespace std;

/*
 * This file contains a simple traversal mechanism to
 * extract the input specification as given by the
 * user. The traversal tries to lookup function names
 * with standard transformations.
 */

class specificationTraversal: public AstSimpleProcessing {
public:
	specificationTraversal();
	virtual void visit(SgNode* n);
	virtual void atTraversalStart();
	virtual void atTraversalEnd();
	TransformationWorklist getTransformationWorkList();

private:
	void handleInterLeaveAcrossArrays(SgFunctionCallExp* funcCallExp);
	TransformationWorklist transformationWorklist;
};

specificationTraversal::specificationTraversal() {
}

TransformationWorklist specificationTraversal::getTransformationWorkList() {
	return transformationWorklist;
}

/*
 * Case to handle interleaveArrayOption
 * C = interleaveAcrossArrays(A,B);
 * i/p = A,B ; o/p = C ; operation = interleaveAcrossArrays
 */
void specificationTraversal::handleInterLeaveAcrossArrays(SgFunctionCallExp* funcCallExp) {
	ROSE_ASSERT(isSgFunctionCallExp(funcCallExp) != NULL);

	SgExpressionPtrList& args = funcCallExp->get_args()->get_expressions();

	vector < string > inputArgs;
	vector < string > outputArgs;

	// Extract the argument and put it into the input list
	for (SgExpressionPtrList::iterator expr = args.begin(); expr != args.end(); expr++) {
		SgVarRefExp* varRefExp = isSgVarRefExp(*expr);
		ROSE_ASSERT(varRefExp != NULL);

		string argName = varRefExp->get_symbol()->get_declaration()->get_name().getString();
		inputArgs.push_back(argName);

#if DEBUG
		cout << " Input Arg: " << argName << endl;
#endif
	}

	// Extract the output
	SgAssignOp* assignOp = isSgAssignOp(funcCallExp->get_parent());
	SgVarRefExp* outputVarRefExp = isSgVarRefExp(assignOp->get_lhs_operand());
	ROSE_ASSERT(outputVarRefExp != NULL);
	string outputName = outputVarRefExp->get_symbol()->get_declaration()->get_name().getString();
	outputArgs.push_back(outputName);

#if DEBUG
	cout << " Output Arg: " << outputName << endl;
#endif

	// Add to worklist
	transformationWorklist.addToWorklist(LayoutOptions::InterleaveAcrossArrays, inputArgs, outputArgs);
}

void specificationTraversal::visit(SgNode* n) {
	if (isSgFunctionCallExp(n) != NULL)
	{

		SgFunctionCallExp* funcCallExp = isSgFunctionCallExp(n);
		SgFunctionRefExp* funcRefExp = isSgFunctionRefExp(funcCallExp->get_function());
		string functionName = funcRefExp->get_symbol()->get_declaration()->get_name().getString();

#if DEBUG
		cout << " Found Function Name: " << functionName << endl;
#endif

		LayoutOptions::LayoutType type = LayoutOptions::getLayoutType(functionName);

		switch (type) {
		case LayoutOptions::InterleaveAcrossArrays:
			handleInterLeaveAcrossArrays(funcCallExp);
			break;

		case LayoutOptions::UnknownTransformation:
			break;

		default:
			cout << " Unknown layout type option. " << endl;
			ROSE_ABORT();
		}

	}
}

void specificationTraversal::atTraversalStart() {

#if DEBUG
	printf (" ============ Specification Traversal Start ================= \n");
#endif

}

void specificationTraversal::atTraversalEnd() {

#if DEBUG
	printf (" ============ Specification Traversal End ================= \n");
#endif

}

