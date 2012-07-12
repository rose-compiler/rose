/*
 * interleaveAcrossArraysTransformation.C
 *
 *  Created on: Jul 12, 2011
 *      Author: sharma9
 */

#include "rose.h"
#include <string>
#include <iostream>
#include <vector>

#include "interleaveAcrossArraysTransformation.h"
#include "transformationWorklist.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

interleaveAcrossArraysTraversal::interleaveAcrossArraysTraversal(Transformation* t) {
	ROSE_ASSERT( t->getOption() == LayoutOptions::InterleaveAcrossArrays);
	transformation = t;
	ROSE_ASSERT ( t->getOutput().size() == 1);
	outputName = t->getOutput().at(0);
}

void interleaveAcrossArraysTraversal::processArrayDeclarations() {

#if DEBUG
	cout << " Start of interleaveAcrossArraysTraversal::processArrayDeclarations() " << endl;
#endif

	// Loop through the variable Decl List
	for (vector<SgVariableDeclaration*>::iterator iter = varDeclList.begin(); iter != varDeclList.end(); iter++) {
		SgVariableDeclaration* varDecl = *iter;

		if (iter != (varDeclList.end()-1)) {
#if DEBUG
			cout << " Removing Declaration : " << varDecl->unparseToString() << endl;
#endif
			//removeStatement(varDecl);
			continue;
		}

		// Create replacement Declaration
		SgInitializedNamePtrList & varList = varDecl->get_variables();
		ROSE_ASSERT(varList.size() == 1);
		SgInitializedName* var = *varList.begin();
		SgConstructorInitializer* constInit = isSgConstructorInitializer(var->get_initializer());
		ROSE_ASSERT (constInit != NULL);
		SgConstructorInitializer* replaceConstInit = isSgConstructorInitializer(copyExpression(constInit));
		ROSE_ASSERT (replaceConstInit != NULL);
		SgExprListExp* exprListExp = replaceConstInit->get_args();
		exprListExp->prepend_expression(buildIntVal(transformation->getInput().size())); // prepend the dimension

		// Build the multiplication string
		SgExpressionPtrList & exps = constInit->get_args()->get_expressions();
		SgExpression *multOp = buildIntVal(transformation->getInput().size());
		SgExpression *finalMultOp;
		for (int i = 0; i < exps.size(); i++) {
			finalMultOp = buildMultiplyOp(multOp, exps[i]);
			multOp = finalMultOp;
		}

		SgExprListExp *multExprListExp = buildExprListExp(finalMultOp);
		replaceConstInit->set_args(multExprListExp);

#if DEBUG
		cout << " Mult String:  " << finalMultOp->unparseToString() << endl;
#endif

		replaceVarDecl = buildVariableDeclaration(outputName, var->get_type(), replaceConstInit, getScope(varDecl));
		insertStatementAfter(varDecl, replaceVarDecl);
		//removeStatement(varDecl);

		// double *_D_pointer = 0;
		SgAssignInitializer* tempInit = buildAssignInitializer(buildIntVal(0));
		string replaceDoublePntrString = "_" + outputName + "_pointer";
		SgVariableDeclaration* replaceDoublePntr = buildVariableDeclaration(replaceDoublePntrString,
				buildPointerType(buildDoubleType()), tempInit, getScope(varDecl));
		insertStatementAfter(replaceVarDecl, replaceDoublePntr);

		//TODO: Replace This later
		string dataPointerString = replaceDoublePntrString + " = " + outputName + ".getDataPointer();";
		attachArbitraryText(replaceDoublePntr, dataPointerString, PreprocessingInfo::after);

#if DEBUG
		cout << " Replacement Variable Declaration : " << replaceVarDecl->unparseToString() << endl;
#endif
	}

}

// This code was created to directly translate A++ code for data layout
// However, due to design changes A++ => C++ => Data layout transformation
// the code below is kept for reference purpose
#if 0
void interleaveAcrossArraysTraversal::processArrayRefExp() {
	bool isAllAccess = false;

	// Loop through the variable Decl List
	for (vector<SgVarRefExp*>::iterator iter = varRefExpList.begin(); iter != varRefExpList.end(); iter++) {
		SgVarRefExp* varRefExp = *iter;
		string variableName = varRefExp->get_symbol()->get_declaration()->get_name().str();
#if DEBUG
		cout << " Variable Name " << variableName << endl;
#endif
		int index = transformation->getInputIndex(variableName);
		SgVarRefExp* replaceVarRefExp = buildVarRefExp(outputName, getScope(varRefExp));

		// Handle Index Case ()
		if (varRefExp->get_parent() != NULL)
		{
			SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(varRefExp->get_parent()->get_parent());
			if (functionCallExpression != NULL)
			{
				string operatorName = TransformationSupport::getFunctionName(functionCallExpression);

#if DEBUG
				cout << " Operator Name: " << operatorName << endl;
#endif

				if (operatorName == "operator()") {
					replaceExpression(varRefExp, replaceVarRefExp);
					SgExprListExp* exprListExp = functionCallExpression->get_args();
					exprListExp->prepend_expression(buildIntVal(index));
				} else {
					// Normal Array Access without index
					if (!isAllAccess) {
						// Insert All Index declaration
						ROSE_ASSERT( replaceVarDecl != NULL );
						SgClassDeclaration* classDecl = buildClassDeclaration("Index", getScope(replaceVarDecl));
						SgClassType* classType = SgClassType::createType(classDecl, NULL);
						SgVariableDeclaration* indexDecl = buildVariableDeclaration("_all", classType, NULL,
								getScope(replaceVarDecl));
						insertStatementBefore(replaceVarDecl, indexDecl);
#if DEBUG
						cout << " Inserting Index Statement : " << indexDecl->unparseToString() << endl;
#endif
						isAllAccess = true;
					}

					// Create functionCall Exp C(1, _all)
					SgExprListExp* parameterList = new SgExprListExp(
							Sg_File_Info::generateDefaultFileInfoForTransformationNode());
					parameterList->append_expression(buildIntVal(index));
					parameterList->append_expression(buildVarRefExp("_all", getScope(replaceVarDecl)));
					SgFunctionCallExp* replaceFunctionCallExp = buildFunctionCallExp(outputName,
							varRefExp->get_symbol()->get_type(), parameterList, getScope(varRefExp));
					replaceExpression(varRefExp, replaceFunctionCallExp);
				}
			}
		}

	}

}
#endif

void interleaveAcrossArraysTraversal::processPntrArrRefExp()
{
	vector<string> inputVec = transformation->getInput();
	ROSE_ASSERT(inputVec.size() > 0);
	// Loop through the variable Decl List
	for (vector<SgPntrArrRefExp*>::iterator iter = pntrArrRefExpList.begin(); iter != pntrArrRefExpList.end(); iter++) {
		SgVarRefExp* varRefExp = isSgVarRefExp((*iter)->get_lhs_operand());
		string variableName = varRefExp->get_symbol()->get_declaration()->get_name().str();
		ROSE_ASSERT(variableName.length() > 0);
		// get Index
		// This will be replaced by SageAtrrib check
		vector<string> inputVec = transformation->getInput();
		int index = 0;
		for(vector<string>::iterator inputIter=inputVec.begin(); inputIter!=inputVec.end(); inputIter++, index++)
		{
			string inputVarName = *inputIter;
			if(variableName.compare("_"+inputVarName+"_pointer") == 0)
			{
				break;
			}
		}

		SgScopeStatement* scope = getEnclosingStatement(varRefExp)->get_scope();
		// _A_pointer => _D_pointer
		SgVarRefExp* newVarRefExp = buildVarRefExp("_" + outputName + "_pointer", scope);
		replaceExpression(varRefExp,newVarRefExp);

		// SC_A(...) => No. of Interleave Arrays * SC_A(...) + Offset
		SgFunctionCallExp* rhsFunctionCallExp = isSgFunctionCallExp((*iter)->get_rhs_operand());
		ROSE_ASSERT(rhsFunctionCallExp!=NULL);

		SgExpression* multOp = buildMultiplyOp( buildIntVal(inputVec.size()) , rhsFunctionCallExp);
		ROSE_ASSERT((index+1) > 0);
		SgExpression* addOp = buildAddOp(multOp, buildIntVal(index));

		cout << " AddOp: " << addOp->unparseToString() << endl;
		(*iter)->set_rhs_operand(addOp);
	}
}

void interleaveAcrossArraysTraversal::storePntrArrRefExp(SgPntrArrRefExp* pntrArrRefExp)
{

	SgExpression* lhsExp = pntrArrRefExp->get_lhs_operand();

	// TODO: Later add support for SageAttribute
	// Other pointer type
	if(!isSgVarRefExp(lhsExp))
		return;

	SgVarRefExp* varRefExp = isSgVarRefExp(lhsExp);
	ROSE_ASSERT(varRefExp!=NULL);
	string variableName = varRefExp->get_symbol()->get_name();
	ROSE_ASSERT(variableName.length() > 0);

	// This will be replaced by SageAtrrib check
	vector<string> inputVec = transformation->getInput();
	bool isFound = false; int index = 0;
	for(vector<string>::iterator iter=inputVec.begin(); iter!=inputVec.end(); iter++, index++)
	{
		string inputVarName = *iter;
		if(variableName.compare("_"+inputVarName+"_pointer") == 0)
		{
			isFound = true;
			break;
		}

	}

	// Variable Not found
	if(!isFound)
		return;

	pntrArrRefExpList.push_back(pntrArrRefExp);

}

SynthesizedAttribute interleaveAcrossArraysTraversal::evaluateSynthesizedAttribute(SgNode* n,
		SynthesizedAttributesList childAttributes) {

	//cout << " Node: " << n->unparseToString() << endl;

	// Fold up the list of child attributes using logical or, i.e. the local
	// result will be true iff one of the child attributes is true.
	SynthesizedAttribute localResult;

	if (isSgVarRefExp(n)) {
//		SgVarRefExp* varRefExp = isSgVarRefExp(n);
//		ROSE_ASSERT (varRefExp != NULL);
//		SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
//		ROSE_ASSERT (variableSymbol != NULL);
//		SgInitializedName* initializedName = variableSymbol->get_declaration();
//		ROSE_ASSERT (initializedName != NULL);
//		string variableName = initializedName->get_name().str();
//		SgType* type = variableSymbol->get_type();
//		ROSE_ASSERT (type != NULL);
//		string typeName = TransformationSupport::getTypeName(type);
//
//		// A++ Supported Arrays
//		if (typeName != "doubleArray" && typeName != "floatArray" && typeName != "intArray")
//			return true;
//
//		// Check if variableName matches the input list
//		if (!transformation->containsInput(variableName))
//			return true;
//
//#if DEBUG
//		cout << " Var Name: " << variableName << " Type: " << typeName << endl;
//#endif
//
//		varRefExpList.push_back(varRefExp);

	} else if (isSgVariableDeclaration(n)) {
		SgVariableDeclaration* varDecl = isSgVariableDeclaration(n);
		SgInitializedNamePtrList & varList = varDecl->get_variables();

		for (SgInitializedNamePtrList::iterator initIter = varList.begin(); initIter != varList.end(); initIter++) {
			SgInitializedName* var = *initIter;
			ROSE_ASSERT(var!=NULL);
			SgType *variableType = var->get_type();
			ROSE_ASSERT (variableType != NULL);
			string type = TransformationSupport::getTypeName(variableType);
			string variableName = var->get_name().str();

			if (type != "doubleArray" && type != "floatArray" && type != "intArray")
				return true;

#if DEBUG
			cout << " Processing Declaration Var Name: " << variableName << " Type: " << type << endl;
#endif

			// Check if variable is present in transformation
			if (!transformation->containsInput(variableName))
				return true;

			varDeclList.push_back(varDecl);
		}
	} else if (isSgPntrArrRefExp(n)) {
		#if DEBUG
			cout << " Pointer Array Ref Exp: " << n->unparseToString() << endl;
		#endif
		storePntrArrRefExp(isSgPntrArrRefExp(n));
	}

	return localResult;
}

void interleaveAcrossArraysTraversal::atTraversalStart() {

#if DEBUG
	printf (" ============ interleaveAcrossArraysTraversal Start ================= \n");
#endif

}

void interleaveAcrossArraysTraversal::atTraversalEnd() {

	processArrayDeclarations();
	//processArrayRefExp();
	processPntrArrRefExp();

#if DEBUG
	printf (" ============ interleaveAcrossArraysTraversal End ================= \n");
#endif

}
