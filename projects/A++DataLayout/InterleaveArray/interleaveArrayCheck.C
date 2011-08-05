/*
 * interleaveAcrossArraysCheck.C
 *
 *  Created on: Jul 12, 2011
 *      Author: sharma9
 */

/*
 * This file contains a preprocessing step
 * to ensure that interleave across arrays
 * will be done safely. If there is an odd
 * case it will print out an exception to
 * the users.
 */
#include <string>
#include <iostream>
#include <set>

#include "rose.h"
#include "interleaveAcrossArraysCheck.h"
#include "transformationSupport.h"
#include "../transformationWorklist.h"
#include "../utility.h"

using namespace std;

/*
 * Case 1. Arrays must not be used as function parameters
 * Case 2. Multiple declarations of same array
 */

interleaveArrayCheck::interleaveArrayCheck(Transformation* t) {
	isArrayRefInFunctionCall = false;
	ROSE_ASSERT( t->getOption() == TransformationOptions::InterleaveArray);
	transformation = t;
}

void interleaveArrayCheck::storeArrayReference(SgInitializedName* var, string variableName,
		string type) {
	// Check if variable is present in transformation
	if (!transformation->containsInput(variableName))
		return;

	SgConstructorInitializer* constInit = isSgConstructorInitializer(var->get_initializer());
	ROSE_ASSERT (constInit != NULL);
	string dimensions = constInit->get_args()->unparseToString();
	ArrayRef arrayRef(variableName, type, dimensions);

	for (vector<ArrayRef>::iterator iter = arrayRefList.begin(); iter != arrayRefList.end();
			iter++) {
		ArrayRef curArrayRef = *iter;

		// Case 2
		if (curArrayRef.name == variableName) {
			cout << " ERROR: Variable Name: " << variableName << " Type: " << type
					<< " duplicate entry found " << endl;
			ROSE_ABORT();
		}

	}

	arrayRefList.push_back(arrayRef);

	return;
}

InterleaveAcrossArraysCheckSynthesizedAttributeType interleaveArrayCheck::evaluateSynthesizedAttribute(
		SgNode* n, SynthesizedAttributesList childAttributes) {
	cout << " Node: " << n->unparseToString() << endl;

	InterleaveAcrossArraysCheckSynthesizedAttributeType localResult;

	for (SynthesizedAttributesList::reverse_iterator child = childAttributes.rbegin();
			child != childAttributes.rend(); child++) {
		InterleaveAcrossArraysCheckSynthesizedAttributeType childResult = *child;
		localResult.isArrayRef |= childResult.isArrayRef;
		localResult.isFunctionRefExp |= childResult.isFunctionRefExp;
	}

	if (isSgVariableDeclaration(n)) {
		SgVariableDeclaration* varDecl = isSgVariableDeclaration(n);
		SgInitializedNamePtrList & varList = varDecl->get_variables();

		for (SgInitializedNamePtrList::iterator initIter = varList.begin();
				initIter != varList.end(); initIter++) {
			SgInitializedName* var = *initIter;
			ROSE_ASSERT(var!=NULL);
			SgType *variableType = var->get_type();
			ROSE_ASSERT (variableType != NULL);
			string type = TransformationSupport::getTypeName(variableType);
			string variableName = var->get_name().str();

			if (type != "doubleArray" && type != "floatArray" && type != "intArray")
				return localResult;

#if DEBUG
			cout << " Var Name: " << variableName << " Type: " << type << endl;
#endif

			storeArrayReference(var, variableName, type);
		}
	} else if (isSgVarRefExp(n)) {
		SgVarRefExp* varRefExp = isSgVarRefExp(n);
		ROSE_ASSERT (varRefExp != NULL);
		SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
		ROSE_ASSERT (variableSymbol != NULL);
		SgInitializedName* initializedName = variableSymbol->get_declaration();
		ROSE_ASSERT (initializedName != NULL);
		string variableName = initializedName->get_name().str();
		SgType* type = variableSymbol->get_type();
		ROSE_ASSERT (type != NULL);
		string typeName = TransformationSupport::getTypeName(type);

		// A++ Supported Arrays
		if (typeName != "doubleArray" && typeName != "floatArray" && typeName != "intArray")
			return localResult;

		// Check if variableName matches the input list
		if (transformation->containsInput(variableName))
			localResult.isArrayRef = true;
	} else if (isSgFunctionCallExp(n)) {
		// Case 1
		// Check for array being present in function Call
		if (localResult.isFunctionRefExp && localResult.isArrayRef) {
			cout << " ERROR: Array Reference present in a function call " << endl;
			ROSE_ABORT();
		}
	} else if (isSgFunctionRefExp(n)) {
		localResult.isFunctionRefExp = true;
	}

	return localResult;
}

void interleaveArrayCheck::atTraversalStart() {

#if DEBUG
	printf (" ============ interleaveArrayCheck Start ================= \n");
#endif

}

void interleaveArrayCheck::atTraversalEnd() {

#if DEBUG
	printf (" ============ interleaveArrayCheck End ================= \n");
#endif

}
