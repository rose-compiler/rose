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
#include "transformationWorklist.h"

using namespace std;

/*
 * Case 1. Arrays must not be used as function parameters
 * Case 2. Arrays must be declared contiguously i.e. no
 * 			statements should exist between them - Done
 * Case 3. Dimensions of Array should match - Done
 * Case 4. Type of Arrays should match - Done
 * Case 5. Multiple declarations of same array - Done
 * Case 6. Substituted Array should not exist in original program
 */

//interleaveAcrossArraysCheck::interleaveAcrossArraysCheck()
//{
//	ROSE_ABORT(" interleaveAcrossArraysCheck constructor called without arguments ");
//}

interleaveAcrossArraysCheck::interleaveAcrossArraysCheck(Transformation* t)
{
	//fileNames = _fileNames;
	isContigousDecl = false;
	isArrayRefInFunctionCall = false;
	ROSE_ASSERT( t->getOption() == LayoutOptions::InterleaveAcrossArrays);
	transformation = t;
	ROSE_ASSERT ( t->getOutput().size() == 1);
	outputName = t->getOutput().at(0);
}

void
interleaveAcrossArraysCheck::storeArrayReference(SgInitializedName* var, string variableName, string type)
{
	// Check if variable is present in transformation
	if(!transformation->containsInput(variableName))
		return;

	SgConstructorInitializer* constInit = isSgConstructorInitializer(var->get_initializer());
	ROSE_ASSERT (constInit != NULL);
	string dimensions = constInit->get_args()->unparseToString();
	ArrayRef arrayRef(variableName, type, dimensions);

	for(vector<ArrayRef>::iterator iter = arrayRefList.begin(); iter!=arrayRefList.end() ; iter ++)
	{
		ArrayRef curArrayRef = *iter;

		// Case 3
		if( curArrayRef.dimensions != dimensions )
		{
			cout << " ERROR: Variable Dimensions: " << dimensions << " for variable: " << variableName << " does not match with other dimensions: " << curArrayRef.dimensions << endl;
			ROSE_ABORT();
		}
		// Case 4
		if( curArrayRef.type != type )
		{
			cout << " ERROR: Variable Type: " << type << " for variable: " << variableName << " does not match with other types: " << curArrayRef.type << endl;
			ROSE_ABORT();
		}
		// Case 5
		if(curArrayRef.name == variableName && curArrayRef.type == type)
		{
			cout << " ERROR: Variable Name: " << variableName << " Type: " << type << " duplicate entry found " << endl;
			ROSE_ABORT();
		}

	}

	arrayRefList.push_back(arrayRef);

	// Set at the start of first declaration and until last declaration
	if(!isContigousDecl && arrayRefList.size() != transformation->getInput().size())
		isContigousDecl = true;
	if(arrayRefList.size() == transformation->getInput().size())
		isContigousDecl = false;

	return;
}

InterleaveAcrossArraysCheckSynthesizedAttributeType
interleaveAcrossArraysCheck::evaluateSynthesizedAttribute ( SgNode* n, SynthesizedAttributesList childAttributes )
{
	//cout << " Node: " << n->unparseToString() << endl;

	InterleaveAcrossArraysCheckSynthesizedAttributeType localResult;

	for (SynthesizedAttributesList::reverse_iterator child = childAttributes.rbegin(); child != childAttributes.rend(); child++)
	{
		InterleaveAcrossArraysCheckSynthesizedAttributeType childResult = *child;
		localResult.isArrayRef |= childResult.isArrayRef;
		localResult.isFunctionRefExp |= childResult.isFunctionRefExp;
	}


	if(isSgVariableDeclaration(n))
	{
		SgVariableDeclaration* varDecl = isSgVariableDeclaration(n);
		SgInitializedNamePtrList & varList = varDecl->get_variables();

		for(SgInitializedNamePtrList::iterator initIter = varList.begin(); initIter!=varList.end() ; initIter++)
		{
			SgInitializedName* var = *initIter;
			ROSE_ASSERT(var!=NULL);
			SgType *variableType = var->get_type();
			ROSE_ASSERT (variableType != NULL);
			string type = TransformationSupport::getTypeName(variableType);
			string variableName = var->get_name().str();

			//Case 6
			if(outputName == variableName)
			{
				cout << " ERROR: Substituting Array " << outputName << " already declared in the file." << endl;
				ROSE_ABORT();
			}

			if(type!="doubleArray" && type !="floatArray" && type!="intArray")
				return localResult;

			#if DEBUG
				cout << " Var Name: " << variableName << " Type: " << type << endl;
			#endif

			storeArrayReference(var, variableName, type );
		}
	}
	else if(isSgPntrArrRefExp(n))
	{
		SgVarRefExp* varRefExp = isSgVarRefExp(isSgPntrArrRefExp(n)->get_lhs_operand());

		if(varRefExp != NULL)
		{
			SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
			ROSE_ASSERT (variableSymbol != NULL);
			SgInitializedName* initializedName = variableSymbol->get_declaration();
			ROSE_ASSERT (initializedName != NULL);
			string variableName = initializedName->get_name().str();
			SgType* type = variableSymbol->get_type();
			ROSE_ASSERT (type != NULL);
			string typeName = TransformationSupport::getTypeName(type);

			// A++ Supported Arrays
			if(typeName !="doubleArray" && typeName !="floatArray" && typeName !="intArray")
				return localResult;

			// Check if variableName matches the input list
			if(transformation->containsInput(variableName))
				localResult.isArrayRef = true;
		}
	}
	else if(isSgFunctionCallExp(n))
	{
		// Case 1
		// Check for array being present in function Call
		if(localResult.isFunctionRefExp && localResult.isArrayRef)
		{
			cout << " ERROR: Array Reference present in a function call " << endl;
			ROSE_ABORT();
		}
	}
	else if(isSgFunctionRefExp(n))
	{
		localResult.isFunctionRefExp = true;
	}
	else if(isSgStatement(n))
	{
		//Case 2
		if(isContigousDecl)
		{
			cout << " ERROR: Array Declaration are not contigous. " << endl;
			ROSE_ABORT();
		}
	}
	return localResult;
}

void interleaveAcrossArraysCheck::atTraversalStart()
   {

	#if DEBUG
		 printf (" ============ interleaveAcrossArraysCheck Start ================= \n");
	#endif

   }

void interleaveAcrossArraysCheck::atTraversalEnd()
   {

	#if DEBUG
		 printf (" ============ interleaveAcrossArraysCheck End ================= \n");
	#endif

   }
