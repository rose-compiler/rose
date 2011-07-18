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
#include "../transformationWorklist.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

interleaveAcrossArraysTraversal::interleaveAcrossArraysTraversal ( Transformation* t )
{
	//fileNames = _fileNames;
	ROSE_ASSERT( t->getOption() == TransformationOptions::InterleaveAcrossArrays);
	transformation = t;
	ROSE_ASSERT ( t->getOutput().size() == 1);
	outputName = t->getOutput().at(0);
}

void
interleaveAcrossArraysTraversal::processArrayDeclarations()
{
	// Loop through the variable Decl List
	for(vector<SgVariableDeclaration*>::iterator iter = varDeclList.begin(); iter != varDeclList.end(); iter++)
	{
		SgVariableDeclaration* varDecl = *iter;

		if(iter != varDeclList.begin())
		{
			#if DEBUG
				cout << " Removing Declaration : " << varDecl->unparseToString() << endl;
			#endif
			removeStatement(varDecl);
			continue;
		}

		// Create replacement Declaration
		SgInitializedNamePtrList & varList = varDecl->get_variables();
		ROSE_ASSERT(varList.size() == 1);
		SgInitializedName* var = *varList.begin();
		SgConstructorInitializer* constInit = isSgConstructorInitializer(var->get_initializer());
		SgConstructorInitializer* replaceConstInit = isSgConstructorInitializer(copyExpression(constInit));
		ROSE_ASSERT (replaceConstInit != NULL);
		SgExprListExp* exprListExp = replaceConstInit->get_args();
		exprListExp->prepend_expression(buildIntVal(transformation->getInput().size())); // prepend the dimension

		replaceVarDecl = buildVariableDeclaration( outputName, var->get_type(), replaceConstInit , getScope(varDecl) );
		insertStatementBefore(varDecl, replaceVarDecl, true);
		removeStatement(varDecl);

		#if DEBUG
			cout << " Replacement Variable Declaration : " << replaceVarDecl->unparseToString() << endl;
		#endif
	}

}

void
interleaveAcrossArraysTraversal::processArrayRefExp()
{
	bool isAllAccess = false;

	// Loop through the variable Decl List
	for(vector<SgVarRefExp*>::iterator iter = varRefExpList.begin(); iter != varRefExpList.end(); iter++)
	{
		SgVarRefExp* varRefExp = *iter;
		string variableName = varRefExp->get_symbol()->get_declaration()->get_name().str();
		#if DEBUG
			cout << " Variable Name " << variableName << endl;
		#endif
		int index = transformation->getInputIndex(variableName);
		SgVarRefExp* replaceVarRefExp = buildVarRefExp(outputName, getScope(varRefExp));


		// Handle Index Case ()
		if(varRefExp->get_parent() != NULL)
		{
			SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(varRefExp->get_parent()->get_parent());
			if(functionCallExpression!=NULL)
			{
				string operatorName = TransformationSupport::getFunctionName ( functionCallExpression );

				#if DEBUG
				cout << " Operator Name: " << operatorName << endl;
				#endif

				if(operatorName == "operator()")
				{
					replaceExpression(varRefExp, replaceVarRefExp);
					SgExprListExp* exprListExp = functionCallExpression->get_args();
					exprListExp->prepend_expression(buildIntVal(index));
				}
				else
				{
					// Normal Array Access without index
					if(!isAllAccess)
					{
						// Insert All Index declaration
						ROSE_ASSERT( replaceVarDecl != NULL );
						SgClassDeclaration* classDecl = buildClassDeclaration("Index", getScope(replaceVarDecl) );
						SgClassType* classType = SgClassType::createType(classDecl, NULL);
						SgVariableDeclaration* indexDecl = buildVariableDeclaration( "_all", classType , NULL, getScope(replaceVarDecl));
						insertStatementBefore( replaceVarDecl, indexDecl, true);
						#if DEBUG
							cout << " Inserting Index Statement : " << indexDecl->unparseToString()  << endl;
						#endif
						isAllAccess = true;
					}

					// Create functionCall Exp C(1, _all)
					SgExprListExp* parameterList = new SgExprListExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
					parameterList->append_expression(buildIntVal(index));
					parameterList->append_expression(buildVarRefExp("_all", getScope(replaceVarDecl)));
					SgFunctionCallExp* replaceFunctionCallExp = buildFunctionCallExp(outputName, varRefExp->get_symbol()->get_type() , parameterList, getScope(varRefExp));
					replaceExpression(varRefExp, replaceFunctionCallExp);
				}
			}
		}

	}

}

SynthesizedAttribute
interleaveAcrossArraysTraversal::evaluateSynthesizedAttribute ( SgNode* n, SynthesizedAttributesList childAttributes )
   {

	//cout << " Node: " << n->unparseToString() << endl;

  // Fold up the list of child attributes using logical or, i.e. the local
  // result will be true iff one of the child attributes is true.
     SynthesizedAttribute localResult;

     if(isSgVarRefExp(n))
     {
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
		if(typeName !="doubleArray" && typeName !="floatArray" && typeName !="intArray")
			return true;

		// Check if variableName matches the input list
		if(!transformation->containsInput(variableName))
			return true;

		#if DEBUG
			cout << " Var Name: " << variableName << " Type: " << typeName << endl;
		#endif

		varRefExpList.push_back(varRefExp);

     }
     else if(isSgVariableDeclaration(n))
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

     			if(type!="doubleArray" && type !="floatArray" && type!="intArray")
     				return true;

     			#if DEBUG
     				cout << " Processing Declaration Var Name: " << variableName << " Type: " << type << endl;
     			#endif

				// Check if variable is present in transformation
				if(!transformation->containsInput(variableName))
					return true;

				varDeclList.push_back(varDecl);
     		}
     	}



     return localResult;
   }

void interleaveAcrossArraysTraversal::atTraversalStart()
   {

	#if DEBUG
		 printf (" ============ interleaveAcrossArraysTraversal Start ================= \n");
	#endif

   }

void interleaveAcrossArraysTraversal::atTraversalEnd()
   {

	processArrayDeclarations();
	processArrayRefExp();

	#if DEBUG
		 printf (" ============ interleaveAcrossArraysTraversal End ================= \n");
	#endif

   }
