#include "arrayPreprocessor.h"

#include "arrayAssignmentStatementTransformation.h"
using namespace SageInterface;
using namespace SageBuilder;

// ********************************************************************************************
// Support for Query to build string representing the transformation of an expression statement
// ********************************************************************************************

// Check if this is an A++ Array Reference
bool isAPPArray(SgNode *astNode) {

	SgVarRefExp* varRefExp = isSgVarRefExp(astNode);

	if (varRefExp == NULL)
		return false;

	SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
	ROSE_ASSERT(variableSymbol != NULL);
	SgInitializedName* initializedName = variableSymbol->get_declaration();
	ROSE_ASSERT(initializedName != NULL);
	SgName variableName = initializedName->get_name();

	// Now compute the offset to the index objects (form a special query for this???)

	SgType* type = variableSymbol->get_type();
	ROSE_ASSERT(type != NULL);

	string typeName = TransformationSupport::getTypeName(type);
	ROSE_ASSERT(typeName.c_str() != NULL);

	// Recognize only these types at present
	if (typeName == "intArray" || typeName == "floatArray" || typeName == "doubleArray") {
		return true;
	}

	return false;
}

// Constructs _A_pointer[SC_A(_1,_2)]
SgPntrArrRefExp* buildAPPArrayRef(SgNode* astNode,
		ArrayAssignmentStatementQueryInheritedAttributeType & arrayAssignmentStatementQueryInheritedData,
		OperandDataBaseType & operandDataBase, SgScopeStatement* scope, SgExprListExp* parameterExpList) {
#if DEBUG
	printf("Contructing A++ array reference object \n");
#endif

	string returnString;

	SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
	ROSE_ASSERT(varRefExp != NULL);
	SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
	ROSE_ASSERT(variableSymbol != NULL);
	SgInitializedName* initializedName = variableSymbol->get_declaration();
	ROSE_ASSERT(initializedName != NULL);
	SgName variableName = initializedName->get_name();

	vector<SgExpression*> parameters;

	// Figure out the dimensionality of the statement globally
	int maxNumberOfIndexOffsets = 6; // default value for A++/P++ arrays
	ROSE_ASSERT(arrayAssignmentStatementQueryInheritedData.arrayStatementDimensionDefined == TRUE);
	if (arrayAssignmentStatementQueryInheritedData.arrayStatementDimensionDefined == TRUE) {
		// The the globally computed array dimension from the arrayAssignmentStatementQueryInheritedData
		maxNumberOfIndexOffsets = arrayAssignmentStatementQueryInheritedData.arrayStatementDimension;
	}

	// Then we want the minimum of all the dimensions accesses (or is it the maximum?)
	for (int n = 0; n < maxNumberOfIndexOffsets; n++) {
		parameters.push_back(buildVarRefExp("_" + StringUtility::numberToString(n + 1), scope));
	}

	// Make a reference to the global operand database
	//OperandDataBaseType & operandDataBase = accumulatorValue.operandDataBase;

	SgType* type = variableSymbol->get_type();
	ROSE_ASSERT(type != NULL);

	string typeName = TransformationSupport::getTypeName(type);
	ROSE_ASSERT(typeName.c_str() != NULL);

	// Copy the string from the SgName object to a string object
	string variableNameString = variableName.str();

	// Setup an intry in the synthesized attribute data base for this variable any
	// future results from analysis could be place there at this point as well
	// record the name in the synthesized attribute
	ROSE_ASSERT(operandDataBase.transformationOption > ArrayTransformationSupport::UnknownIndexingAccess);
	ArrayOperandDataBase arrayOperandDB = operandDataBase.setVariableName(variableNameString);

	// We could have specified in the inherited attribute that this array variable was
	// index and if so leave the value of $IDENTIFIER_STRING to be modified later in
	// the assembly of the operator() and if not do the string replacement on
	// $IDENTIFIER_STRING here (right now).

	returnString = string("$IDENTIFIER_STRING") + string("_pointer[SC") + string("$MACRO_NAME_SUBSTRING") + string("(")
			+ string("$OFFSET") + string(")]");

	string functionSuffix = "";
	SgPntrArrRefExp* pntrRefExp;

	cout << " arrayAssignmentStatementQueryInheritedData.getIsIndexedArrayOperand() "
			<< arrayAssignmentStatementQueryInheritedData.getIsIndexedArrayOperand() << endl;
	// The inherited attribute mechanism is not yet implimented
	if (arrayAssignmentStatementQueryInheritedData.getIsIndexedArrayOperand() == FALSE)
	//if(true)
	{
		// do the substitution of $OFFSET here since it our last chance
		// (offsetString is the list of index values "index1,index2,...,indexn")
		//returnString = StringUtility::copyEdit(returnString,"$OFFSET",offsetString);

		string operandIdentifier = arrayOperandDB.generateIdentifierString();
		// do the substitution of $IDENTIFIER_STRING here since it our last chance
		// if variable name is "A", generate: A_pointer[SC_A(index1,...)]
		// returnString = StringUtility::copyEdit (returnString,"$IDENTIFIER_STRING",variableNameString);
		ROSE_ASSERT(arrayOperandDB.indexingAccessCode > ArrayTransformationSupport::UnknownIndexingAccess);

		// Edit into place the name of the data pointer
		returnString = StringUtility::copyEdit(returnString, "$IDENTIFIER_STRING", operandIdentifier);

		// Optimize the case of uniform or unit indexing to generate a single subscript macro definition
		if ((arrayOperandDB.indexingAccessCode == ArrayTransformationSupport::UniformSizeUnitStride)
				|| (arrayOperandDB.indexingAccessCode == ArrayTransformationSupport::UniformSizeUniformStride))
			returnString = StringUtility::copyEdit(returnString, "$MACRO_NAME_SUBSTRING", "");
		else {
			returnString = StringUtility::copyEdit(returnString, "$MACRO_NAME_SUBSTRING", operandIdentifier);
			functionSuffix = operandIdentifier;
		}

		/* 
		 * Create SgPntrArrRefExp lhs is VarRefExp and rhs is SgFunctionCallExp
		 */
		SgVarRefExp* newVarRefExp = buildVarRefExp(operandIdentifier + "_pointer", scope);

		string functionName = "SC" + functionSuffix;

		SgFunctionCallExp* functionCallExp;
		if (parameterExpList == NULL)
			functionCallExp = buildFunctionCallExp(functionName, buildIntType(), buildExprListExp(parameters), scope);
		else
			functionCallExp = buildFunctionCallExp(functionName, buildIntType(), parameterExpList, scope);

		pntrRefExp = buildPntrArrRefExp(newVarRefExp, functionCallExp);

#if DEBUG
		cout << " pntrArrRefExp = " << pntrRefExp->unparseToString() << endl;
#endif 

	}

	return pntrRefExp;
}

/*
 * This method checks if there exist atleast one
 * reference to A++ Array in the exprStatement
 */
bool checkAPPExist(SgExprStatement* exprStatement) {

	bool isAPPExist = false;

	vector<SgFunctionCallExp*> functionCallList = querySubTree<SgFunctionCallExp> (exprStatement, V_SgFunctionCallExp);

	cout << " Function Call size: " << functionCallList.size() << endl;
	// Check to see if there is atleast one function
	// reference to A++ Arrays
	for (vector<SgFunctionCallExp*>::reverse_iterator iter = functionCallList.rbegin(); iter
	//			!= functionCallList.rend() && !isAPPExist; iter++) {
			!= functionCallList.rend(); iter++) {
		SgFunctionCallExp* functionCallExp = (*iter);
		string functionTypeName = TransformationSupport::getFunctionTypeName(functionCallExp);

		cout << " Function name " << TransformationSupport::getFunctionName(functionCallExp) << endl;
		if ((functionTypeName == "doubleArray") || (functionTypeName == "floatArray") || (functionTypeName
				== "intArray")) {
			// Use this query to handle only A++ function call expressions
			// printf ("Break out of overloaded operator processing since type = %s is not to be processed \n",functionTypeName.c_str());
			isAPPExist = true;
		}
	}

	return isAPPExist;
}

bool isFunctionParenthesisOperator(SgFunctionCallExp* functionCallExp) {
	ROSE_ASSERT(functionCallExp != NULL);

	string operatorName = TransformationSupport::getFunctionName(functionCallExp);

	int numberOfParameters = functionCallExp->get_args()->get_expressions().size();

	// Classify the function name (it could be an overloaded function)
	TransformationSupport::operatorCodeType operatorCodeVariant = TransformationSupport::classifyOverloadedOperator(
			operatorName.c_str(), numberOfParameters);

	if (operatorCodeVariant == TransformationSupport::PARENTHESIS_OPERATOR_CODE)
		return true;

	return false;
}

/*
 * Handle the case of parent being parenthesis 
 * while constructing the array reference
 */
bool isParentParenthesisOperator(SgNode* astNode) {

	SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
	ROSE_ASSERT(functionCallExp != NULL);

	while ((functionCallExp = getEnclosingNode<SgFunctionCallExp> (functionCallExp)) != NULL) {
		if (isFunctionParenthesisOperator(functionCallExp))
			return true;
	}

	return false;
}

void substituteIndexes(SgExprListExp* functionCallExprList, SgScopeStatement* scope) {
	int count = 1;
	ROSE_ASSERT(functionCallExprList != NULL);

	SgExpressionPtrList& args = functionCallExprList->get_expressions();

	for (SgExpressionPtrList::iterator expr = args.begin(); expr != args.end(); expr++, count++) {
		cout << " Substitute Indexes : " << (*expr)->unparseToString() << endl;

		vector<SgVarRefExp*> varRefList = querySubTree<SgVarRefExp> (*expr, V_SgVarRefExp);

		if (varRefList.size() == 0)
			continue;

		SgVarRefExp* varRefExp = isSgVarRefExp(*varRefList.begin());
		ROSE_ASSERT(varRefExp != NULL);
		//TODO Check if type is Indexed, Ranged
		SgVarRefExp* replaceVarRefExp = buildVarRefExp("_" + StringUtility::numberToString(count), scope);
		replaceExpression(varRefExp, replaceVarRefExp, false);
	}
}

/*
 * This method creates the appropriate statement
 * and surrounding loop around it
 */
SgExprStatement* ArrayAssignmentStatementTransformation::createSecondLoop(SgExprStatement* exprStatement,
		ArrayAssignmentStatementQueryInheritedAttributeType & arrayAssignmentStatementQueryInheritedData,
		OperandDataBaseType & operandDataBase) {

	bool isAPPExist = checkAPPExist(exprStatement);

	if (!isAPPExist)
		return NULL;

#if DEBUG
	cout << " Creating Second Loop for a dependent statement : " << exprStatement->unparseToString() << endl;
#endif

	vector<SgVarRefExp*> varRefList = querySubTree<SgVarRefExp> (exprStatement, V_SgVarRefExp);

	ROSE_ASSERT(varRefList.size() > 0);
	SgScopeStatement* scope = exprStatement->get_scope();

#if DEBUG
	cout << " LHS Operand is : " << (*varRefList.begin())->unparseToString() << endl;
#endif

	SgVarRefExp* lhsVarRefExp = (*varRefList.begin());

	// Remove it from NodeList since we will copy this node
	// for a new statement and check it later
	vector<int>::iterator dimIter = dimensionList.begin();
	for(vector<SgExpression*>::iterator iter=nodeList.begin(); iter!=nodeList.end(); iter++, dimIter++)
	{
		if((*iter) == lhsVarRefExp)
		{
#if DEBUG
			cout << " Erasing element : " << (*iter)->unparseToString() << endl;
#endif
			nodeList.erase(iter);
			dimensionList.erase(dimIter);
			break;
		}
	}

	SgExpression* replaceExp;
	if(isFunctionParenthesisOperator(isSgFunctionCallExp(lhsVarRefExp->get_parent()->get_parent())))
	{
		replaceExp = isSgFunctionCallExp(lhsVarRefExp->get_parent()->get_parent());
	}
	else
	{
		replaceExp = lhsVarRefExp;
	}

	vector<SgExpression*> parameters;

	// Figure out the dimensionality of the statement globally
	int maxNumberOfIndexOffsets = 6; // default value for A++/P++ arrays
	ROSE_ASSERT(arrayAssignmentStatementQueryInheritedData.arrayStatementDimensionDefined == TRUE);
	if (arrayAssignmentStatementQueryInheritedData.arrayStatementDimensionDefined == TRUE) {
		// The the globally computed array dimension from the arrayAssignmentStatementQueryInheritedData
		maxNumberOfIndexOffsets = arrayAssignmentStatementQueryInheritedData.arrayStatementDimension;
	}

	// Then we want the minimum of all the dimensions accesses (or is it the maximum?)
	for (int n = 0; n < maxNumberOfIndexOffsets; n++) {
		parameters.push_back(buildVarRefExp("_" + StringUtility::numberToString(n + 1), scope));
	}

	// Copy the old replacement expression for the new statement
	SgExpression* copyReplaceExp = copyExpression(replaceExp);
	vector<SgVarRefExp*> copyVarRefList = querySubTree<SgVarRefExp> (copyReplaceExp, V_SgVarRefExp);
	ROSE_ASSERT(copyVarRefList.size() > 0);
#if DEBUG
	cout << " Inserting VarRef into processing list: " << (*copyVarRefList.begin())->unparseToString() << endl;
#endif
	nodeList.push_back((*copyVarRefList.begin()));
	dimensionList.push_back(maxNumberOfIndexOffsets);


	SgVarRefExp* newVarRefExp = buildVarRefExp("__T_pointer", scope);

	string functionName = "SC_"+lhsVarRefExp->get_symbol()->get_declaration()->get_name();
	SgFunctionCallExp* functionCallExp = buildFunctionCallExp(functionName, buildIntType(),
			buildExprListExp(parameters), scope);

	SgPntrArrRefExp* pntrArrRefExp2 = buildPntrArrRefExp(newVarRefExp, functionCallExp);

	SgExprStatement* assignExprStatement = buildAssignStatement( copyReplaceExp , pntrArrRefExp2);

	insertStatementAfter(exprStatement, assignExprStatement);

	return assignExprStatement;
}

/*
 * Create LHS of a loop dependent statement
 */
void createFirstLoop(SgExprStatement* exprStatement,
		ArrayAssignmentStatementQueryInheritedAttributeType & arrayAssignmentStatementQueryInheritedData,
		OperandDataBaseType & operandDataBase) {
	//transformArrayRefAPP(exprStatement, arrayAssignmentStatementQueryInheritedData, operandDataBase);

	// Change the LHS, it will the first PntrArrayRef
	SgScopeStatement* scope = exprStatement->get_scope();

	// Figure out the dimensionality of the statement globally
	vector<SgExpression*> parameters;
	int maxNumberOfIndexOffsets = 6; // default value for A++/P++ arrays
	ROSE_ASSERT(arrayAssignmentStatementQueryInheritedData.arrayStatementDimensionDefined == TRUE);
	if (arrayAssignmentStatementQueryInheritedData.arrayStatementDimensionDefined == TRUE) {
		// The the globally computed array dimension from the arrayAssignmentStatementQueryInheritedData
		maxNumberOfIndexOffsets = arrayAssignmentStatementQueryInheritedData.arrayStatementDimension;
	}

	// Then we want the minimum of all the dimensions accesses (or is it the maximum?)
	for (int n = 0; n < maxNumberOfIndexOffsets; n++) {
		parameters.push_back(buildVarRefExp("_" + StringUtility::numberToString(n + 1), scope));
	}

	vector<SgVarRefExp*> varRefList = querySubTree<SgVarRefExp> (exprStatement, V_SgVarRefExp);

	ROSE_ASSERT(varRefList.size() > 0);

	SgVarRefExp* lhsVarRefExp = (*varRefList.begin()); // LHS should be the first ref

	SgExpression* replaceExp;
	if(isFunctionParenthesisOperator(isSgFunctionCallExp(lhsVarRefExp->get_parent()->get_parent())))
	{
		replaceExp = isSgFunctionCallExp(lhsVarRefExp->get_parent()->get_parent());
	}
	else
	{
		replaceExp = lhsVarRefExp;
	}


	SgVarRefExp* newVarRefExp = buildVarRefExp("__T_pointer", scope);
	string functionName = "SC_"+lhsVarRefExp->get_symbol()->get_declaration()->get_name();
	SgFunctionCallExp* functionCallExp = buildFunctionCallExp(functionName, buildIntType(),
			buildExprListExp(parameters), scope);
	SgPntrArrRefExp* pntrArrRefExp2 = buildPntrArrRefExp(newVarRefExp, functionCallExp);

	ROSE_ASSERT(replaceExp != NULL);
	replaceExpression(replaceExp, pntrArrRefExp2);

	return;
}

// Supporting function (not one of the global functions required for the Query interface)
ArrayAssignmentStatementQuerySynthesizedAttributeType ArrayAssignmentStatementTransformation::expressionStatementTransformation(SgExprStatement* astNode,
		ArrayAssignmentStatementQueryInheritedAttributeType & arrayAssignmentStatementQueryInheritedData,
		const ArrayAssignmentStatementQuerySynthesizedAttributeType & innerLoopTransformation,
		OperandDataBaseType & operandDataBase) {
	// Since we input the operandDataBase we could skip the initial AST queries and query the data base instead.
	// (the operandDataBase has more information than the initial queries represent).

#if DEBUG
	cout << " TOP of expressionStatementTransformation " << astNode->unparseToString() << endl;
#endif

	// declaration of return value
	ArrayAssignmentStatementQuerySynthesizedAttributeType returnSynthesizedAttribute;

	string returnString;

	printf("In expressionStatementTransformation(): operandDataBase.displayString() = %s \n",
			operandDataBase.displayString().c_str());

	// Make sure the data base has been setup properly
	ROSE_ASSERT(operandDataBase.transformationOption > ArrayTransformationSupport::UnknownIndexingAccess);
	ROSE_ASSERT(operandDataBase.dimension > -1);

	// printf ("######################### START OF VARIABLE TYPE NAME QUERY ######################## \n");

	//list<string> operandNameStringList = NameQuery::getVariableNamesWithTypeNameQuery (astNode, "doubleArray" );
	printf(" Kamal - Need to handle NameQuery \n");
	// Commented below two lines Kamal
	//	NameQuerySynthesizedAttributeType operandNameStringList =
	//		NameQuery::querySubTree (
	//                   astNode,
	//                  "doubleArray",
	//                 NameQuery::VariableNamesWithTypeName );

	// printf ("Print out the list of variable names: \n%s \n",StringUtility::listToString(operandNameStringList).c_str());

	// Now use STL to build a list of unique names
	// Commented the two lines below Kamal
	//list<string> uniqueOperandNameStringList = operandNameStringList;
	//uniqueOperandNameStringList.unique();

	// printf ("Print out the list of unique variable names: \n%s \n",StringUtility::listToString(uniqueOperandNameStringList).c_str());

	// printf ("######################### START OF VARIABLE NAME QUERY ######################## \n");

	// Specify optimization assertions defined by user (specified by the user in his application code)
	operandDataBase.setUserOptimizationAssertions(arrayAssignmentStatementQueryInheritedData.transformationOptions);

	// This query gets the indexing code for each operand used in the expression statement
	// (but for now we make them all the same, later we will support the optimization of indexing of individual array operands)
	// Query the operand data base to find out the global properties of the indexing in the array statement
	// int globalIndexingProperties = ArrayTransformationSupport::UniformSizeUniformStride;
	// int globalIndexingProperties = operandDataBase.globalIndexingProperties();

	// printf ("globalIndexingProperties = %d \n",globalIndexingProperties);

#if DEBUG
	cout << " Creating unique operand list " << endl;
#endif

	operandDataBase.removeDups();

	printf("######################### END OF ALL PRELIMINARY QUERIES ######################## \n");

	// Template string into which we substitute the variableData and loopNestString strings
	string transformationTemplateString =
			"\
// Automatically Introduced Transformation (via preprocessor built by ROSE)\n\n";

	attachComment(astNode, transformationTemplateString);

	// #######################################
	//     COMPUTE THE GLOBAL DECLARATIONS
	// #######################################

	// A string to hold the variables required for this transformation
	// Build the variable data into the array statment transformation
	string globalDeclarations = "";
#if OLD
	ArrayTransformationSupport::buildOperandSpecificGlobalDeclarations(
			arrayAssignmentStatementQueryInheritedData, operandDataBase);
#endif

	// printf ("globalDeclarations = \n%s \n",globalDeclarations.c_str());

	// ###############################################
	//     COMPUTE THE LOCAL VARIABLE DECLARATIONS
	// ###############################################

	// A string to hold the variables required for this transformation
	// Build the variable data into the array statment transformation
	// e.g. ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D6(_A);
	ArrayTransformationSupport::buildOperandSpecificVariableDeclarations(astNode,
			arrayAssignmentStatementQueryInheritedData, operandDataBase);

	// ##################################################
	//     COMPUTE THE LOCAL VARIABLE INITIALIZATIONS
	// ##################################################

	// Variable initialization must occur imediately before the transformations using the variables
	// e.g. ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D6(A,_A);
	ArrayTransformationSupport::buildOperandSpecificVariableInitialization(astNode,
			arrayAssignmentStatementQueryInheritedData, operandDataBase);

	// #############################
	//     COMPUTE THE LOOP NEST
	// #############################

	// Get the dimensionality of the array statement
	ROSE_ASSERT(arrayAssignmentStatementQueryInheritedData.arrayStatementDimensionDefined == TRUE);
	int dimensionOfArrayStatement = arrayAssignmentStatementQueryInheritedData.arrayStatementDimension;

	// call function to build loop nest
	string loopNestString;

	cout << " LoopDependence : " << innerLoopTransformation.getLoopDependence() << endl;
	if (innerLoopTransformation.getLoopDependence() == TRUE) {

#if DEBUG
		cout << " In innerLoopTransformation.getLoopDependence() = TRUE " << endl;
#endif

		// Kamal (07/21/2011) Do not remove this now since it may be important in certain loops

		// If a loop dependence was found in the assembly then generate a special form of the
		// transformation to preserve the array semantics. This can be optimized later to block the
		// the computation so that we avoid poor use of cache.  Add an additional loop nest to the
		// loopNestString so that we can transform A = A+A into:
		//      _T.redim(lhs); for (...) {_T = A+A;} for (...) {A = _T};

		// Insert "T.redim(<lhs>);" at front of loopNestString
		string lhsArrayOperandName = operandDataBase.arrayOperandList[0].arrayVariableName;

		cout << "lhsOperandName: " << lhsArrayOperandName << endl;
		loopNestString = "__T_pointer = _T.getDataPointer(); \n _T.redim(" + lhsArrayOperandName + ");\n" + loopNestString;

		string temporaryString = innerLoopTransformation.getLoopDependenceLhs();
		// Use the same indexing with the temporary as is used with the lhs (copy lhs and change the
		// variable name (add to data base so that we generate the correct supporting declarations
		// and initializations)). Since _T is the same size as A we don't have to change the
		// subscript macro.
		temporaryString = StringUtility::copyEdit(temporaryString, lhsArrayOperandName, "_T");
		attachArbitraryText(getEnclosingStatement(astNode), loopNestString, PreprocessingInfo::before);

		// Handle Second Loop creation A = Temp
		SgExprStatement* lhsExprStatement = createSecondLoop(astNode, arrayAssignmentStatementQueryInheritedData,
				operandDataBase);

#if DEBUG
		cout << " Second Loop lhsExprStatement: " << lhsExprStatement->unparseToString() << endl;
#endif

		ArrayTransformationSupport::buildLoopNest(lhsExprStatement, operandDataBase, dimensionOfArrayStatement);

		// call function to build loop nest
		//LHS creation, doesn't create a new node
		createFirstLoop(astNode, arrayAssignmentStatementQueryInheritedData, operandDataBase);
		loopNestString = "";
		ArrayTransformationSupport::buildLoopNest(astNode, operandDataBase, dimensionOfArrayStatement);

#if DEBUG
		cout << " innerLoopTransformation.getLoopDependence() = TRUE LHS: " << innerLoopTransformation.getLoopDependenceLhs() << " RHS:" << innerLoopTransformation.getLoopDependenceRhs();
#endif


#if OLD
		// ***************************************************
		// Setup loop bodies for loop dependent transformation
		// ***************************************************

		// First loop body
		string firstLoopBody = innerLoopTransformation.getLoopDependenceRhs();
		firstLoopBody = temporaryString + firstLoopBody;
		firstLoopBody = StringUtility::copyEdit(firstLoopBody, "$ASSIGNMENT_OPERATOR", " = ");

		// Second loop body
		string secondLoopBody = innerLoopTransformation.getLoopDependenceLhs();
		secondLoopBody = secondLoopBody + string(" = ") + temporaryString;

		// Mark the first "$INNER_LOOP" substring so that we can differentiate between the first and second loops
		loopNestString = StringUtility::copyEdit(loopNestString, "$INNER_LOOP", "$FIRST_INNER_LOOP");
		//loopNestString += ArrayTransformationSupport::buildLoopNest (astNode, operandDataBase, dimensionOfArrayStatement );
		loopNestString = StringUtility::copyEdit(loopNestString, "$INNER_LOOP", "$SECOND_INNER_LOOP");

		loopNestString = StringUtility::copyEdit(loopNestString, "$FIRST_INNER_LOOP", firstLoopBody);
		loopNestString = StringUtility::copyEdit(loopNestString, "$SECOND_INNER_LOOP", secondLoopBody);

#if DEBUG
		printf ("firstLoopBody  = %s \n",firstLoopBody.c_str());
		printf ("secondLoopBody = %s \n",secondLoopBody.c_str());
		printf ("loopNestString = \n%s \n",loopNestString.c_str());
#endif

#endif
		// Add "T" to operand data base for loop dependent array statement
		// operandDataBase.setVariableName( "_T" );
		ArrayOperandDataBase temporaryArrayOperand = operandDataBase.arrayOperandList[0];
		temporaryArrayOperand.arrayVariableName = "_T";
		operandDataBase.arrayOperandList.push_back(temporaryArrayOperand);

	} else {

#if DEBUG
		cout << " In innerLoopTransformation.getLoopDependence() = FALSE " << endl;
#endif
		// call function to build loop nest
		ArrayTransformationSupport::buildLoopNest(astNode, operandDataBase, dimensionOfArrayStatement);

	}

#if DEBUG
	printf ("Before generation of global declarations: loopNestString = \n%s\n",loopNestString.c_str());
#endif

	// Reset the workspace to "" in the synthesized attribute (part of general cleanup, but not required)
	returnSynthesizedAttribute.setWorkSpace("");

	return returnSynthesizedAttribute;
}

SgPntrArrRefExp* buildArrayRefExp(SgVarRefExp* newVarRefExp, int dimension, string functionName, SgScopeStatement* scope)
{
	// Normal Array Access without index
	vector<SgExpression*> parameters;

	// Figure out the dimensionality of the statement globally
	ROSE_ASSERT(dimension > 0);
	int maxNumberOfIndexOffsets = dimension; // default value for A++/P++ arrays

	// Then we want the minimum of all the dimensions accesses (or is it the maximum?)
	for (int n = 0; n < maxNumberOfIndexOffsets; n++) {
		parameters.push_back(buildVarRefExp("_" + StringUtility::numberToString(n + 1), scope));
	}

	SgFunctionCallExp* functionCallExp = buildFunctionCallExp(functionName, buildIntType(),
			buildExprListExp(parameters), scope);

	SgPntrArrRefExp* pntrRefExp = buildPntrArrRefExp(newVarRefExp, functionCallExp);

	return pntrRefExp;
}

void ArrayAssignmentStatementTransformation::processArrayRefExp(SgVarRefExp* varRefExp, int dimension) {

	string variableName = varRefExp->get_symbol()->get_declaration()->get_name().str();
#if DEBUG
	cout << " Variable Name " << variableName << endl;
#endif

	SgScopeStatement* scope = getScope(varRefExp);
	//SgVarRefExp* replaceVarRefExp = buildVarRefExp(variableName, getScope(varRefExp));

	SgVarRefExp* newVarRefExp = buildVarRefExp("_" + variableName + "_pointer", scope);
	ROSE_ASSERT(newVarRefExp != NULL);

	string functionName = "SC_" + variableName;

	if (varRefExp->get_parent() != NULL) {
		SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(varRefExp->get_parent()->get_parent());
		if (functionCallExpression != NULL) {
			string operatorName = TransformationSupport::getFunctionName(functionCallExpression);

#if DEBUG
			cout << " Operator Name: " << operatorName << endl;
#endif

			if (operatorName == "operator()") {

				// Create a copy since the original might be deleted during replacement
				SgExprListExp* functionExprList = isSgExprListExp(copyExpression(functionCallExpression->get_args()));
				substituteIndexes(functionExprList, scope);
				SgFunctionCallExp* functionCallExp = buildFunctionCallExp(functionName, buildIntType(),
						functionExprList, scope);
				SgPntrArrRefExp* pntrArrRefExp = buildPntrArrRefExp(newVarRefExp, functionCallExp);
				cout << " PntrArrayReference replacement: " << pntrArrRefExp->unparseToString() << endl;
				replaceExpression(functionCallExpression, pntrArrRefExp, false);
			} else {

				SgPntrArrRefExp* pntrRefExp = buildArrayRefExp(newVarRefExp, dimension, functionName, scope);
				replaceExpression(varRefExp, pntrRefExp);
			}
		}
		else
		{
			// Added to support simple cases like A = A+B where there is dependence
			SgPntrArrRefExp* pntrRefExp = buildArrayRefExp(newVarRefExp, dimension, functionName, scope);
			replaceExpression(varRefExp, pntrRefExp);
		}
	}

}

// Rename this function to: localArrayAssignmentStatementQueryFunction
ArrayAssignmentStatementQueryInheritedAttributeType ArrayAssignmentStatementTransformation::evaluateInheritedAttribute(
		SgNode* astNode, ArrayAssignmentStatementQueryInheritedAttributeType arrayAssignmentStatementQueryInheritedData) {
	// This function returns a string specific to the current astNode (assembly of strings is a separate function)

	ROSE_ASSERT(astNode != NULL);

#if DEBUG
	printf ("##### TOP of arrayAssignmentStatementAssembly.C::evaluateInheritedAttribute() (sage_class_name = %s) \n",astNode->sage_class_name());
#endif

	// Make a reference to the global operand database
	OperandDataBaseType & operandDataBase = accumulatorValue.operandDataBase;

	// Make sure the data base has been setup properly
	ROSE_ASSERT(operandDataBase.transformationOption > ArrayTransformationSupport::UnknownIndexingAccess);
	ROSE_ASSERT(operandDataBase.dimension > -1);

	// Default value for synthesized attribute we will use as a return value
	// Need to handle all unary and binary operators and variables (but not much else)
	switch (astNode->variant()) {
	case FUNC_CALL: // tag for SgFunctionCallExp class
	{
		// Error checking: Verify that we have a SgFunctionCallExp object
		SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(astNode);
		ROSE_ASSERT(functionCallExpression != NULL);

		string operatorName = TransformationSupport::getFunctionName(functionCallExpression);
		ROSE_ASSERT(operatorName.length() > 0);

		// printf ("overloaded operator is of type = %s \n",functionTypeName.c_str());

		string functionTypeName = TransformationSupport::getFunctionTypeName(functionCallExpression);

		cout << " Function Type Name: " << functionTypeName << endl;

		if ((functionTypeName != "doubleArray") && (functionTypeName != "floatArray") && (functionTypeName
				!= "intArray")) {
			// Use this query to handle only A++ function call expressions
			// printf ("Break out of overloaded operator processing since type = %s is not to be processed \n",functionTypeName.c_str());
			break;
		} else {
			printf("Processing overloaded operator of type = %s \n", functionTypeName.c_str());
		}

		ROSE_ASSERT((functionTypeName == "doubleArray") || (functionTypeName == "floatArray") || (functionTypeName
				== "intArray"));

		printf("CASE FUNC_CALL: Overloaded operator = %s \n", operatorName.c_str());

		TransformationSupport::operatorCodeType operatorCodeVariant =
				TransformationSupport::classifyOverloadedOperator(operatorName.c_str());
		printf("CASE FUNC_CALL: operatorCodeVariant = %d \n", operatorCodeVariant);

		ROSE_ASSERT(operatorName.length() > 0);

		// Moved from local generation function to assembly function

		// Separating this case into additional cases makes up to some
		// extent for using a more specific higher level grammar.
		switch (operatorCodeVariant) {
		case TransformationSupport::PARENTHESIS_OPERATOR_CODE: {
			ROSE_ASSERT(operatorName == "operator()");

			// Record that the array operand is index with InternalIndex objects this info
			// is used later in the assembly phase to defer the specification of return
			// string.
			arrayAssignmentStatementQueryInheritedData.setIsIndexedArrayOperand(TRUE);

			break;
		}

		default: {
			// printf ("default in switch found in variant = %d \n",operatorCodeVariant);
			break;
		}
		}
		break;
	}
	}

	// Build the inherited attribute (using the source position information (required for use of
	// rewrite mechanism)).
	ArrayAssignmentStatementQueryInheritedAttributeType returnAttribute(arrayAssignmentStatementQueryInheritedData);

#if DEBUG
	returnAttribute.display("Called from ArrayAssignmentStatementTransformation::evaluateInheritedAttribute()");
	printf ("##### BOTTOM of evaluateInheritedAttribute (astNode = %s) \n",astNode->sage_class_name());
#endif

	// return arrayAssignmentStatementQueryInheritedData;
	return returnAttribute;
}

// first visits the VarRef and then creates entry in operandDatabase which is
// useful in expressionStatement transformation. Thus, we replace the VarRef
// at the end of the traversal and insert loops during traversal
ArrayAssignmentStatementQuerySynthesizedAttributeType ArrayAssignmentStatementTransformation::evaluateSynthesizedAttribute(
		SgNode* astNode,
		ArrayAssignmentStatementQueryInheritedAttributeType arrayAssignmentStatementQueryInheritedData,
		SubTreeSynthesizedAttributes synthesizedAttributeList) {
	// This function assembles the elements of the input list (a list of char*) to form the output (a single char*)

#if DEBUG
	printf ("\n$$$$$ TOP of evaluateSynthesizedAttribute (astNode = %s) (synthesizedAttributeList.size() = %d) \n",
			astNode->sage_class_name(),synthesizedAttributeList.size());
	//cout << " Ast node string: " << astNode->unparseToString() << endl;
#endif

	// Build the return value for this function
	ArrayAssignmentStatementQuerySynthesizedAttributeType returnSynthesizedAttribute(astNode);

	// Iterator used within several error checking loops (not sure we should declare it here!)
	vector<ArrayAssignmentStatementQuerySynthesizedAttributeType>::iterator i;

	// Make a reference to the global operand database
	OperandDataBaseType & operandDataBase = accumulatorValue.operandDataBase;

	// Make sure the data base has been setup properly
	ROSE_ASSERT(operandDataBase.transformationOption > ArrayTransformationSupport::UnknownIndexingAccess);
	ROSE_ASSERT(operandDataBase.dimension > -1);

	// Build up a return string
	string returnString = "";

	string operatorString;

	// Need to handle all unary and binary operators and variables (but not much else)
	switch (astNode->variant()) {

	case FUNC_CALL: {

		// Error checking: Verify that we have a SgFunctionCallExp object
		SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(astNode);
		ROSE_ASSERT(functionCallExpression != NULL);

		string operatorName = TransformationSupport::getFunctionName(functionCallExpression);
		ROSE_ASSERT(operatorName.c_str() != NULL);

		string functionTypeName = TransformationSupport::getFunctionTypeName(functionCallExpression);

		if ((functionTypeName != "doubleArray") && (functionTypeName != "floatArray") && (functionTypeName
				!= "intArray")) {
			// Use this query to handle only A++ function call expressions
			// printf ("Break out of overloaded operator processing since type = %s is not to be processed \n",functionTypeName.c_str());
			break;
		} else {
			// printf ("Processing overloaded operator of type = %s \n",functionTypeName.c_str());
		}

		ROSE_ASSERT((functionTypeName == "doubleArray") || (functionTypeName == "floatArray") || (functionTypeName
				== "intArray"));

		// printf ("CASE FUNC_CALL: Overloaded operator = %s \n",operatorName.c_str());

		// Get the number of parameters to this function
		SgExprListExp* exprListExp = functionCallExpression->get_args();
		ROSE_ASSERT(exprListExp != NULL);

		SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
		int numberOfParameters = expressionPtrList.size();

		TransformationSupport::operatorCodeType operatorCodeVariant =
				TransformationSupport::classifyOverloadedOperator(operatorName.c_str(), numberOfParameters);
		// printf ("CASE FUNC_CALL: numberOfParameters = %d operatorCodeVariant = %d \n",
		//      numberOfParameters,operatorCodeVariant);

		ROSE_ASSERT(operatorName.length() > 0);

		// Separating this case into additional cases makes up to some
		// extent for using a more specific higher level grammar.
		switch (operatorCodeVariant) {

		case TransformationSupport::ASSIGN_OPERATOR_CODE: {
			vector<ArrayOperandDataBase>::iterator lhs = operandDataBase.arrayOperandList.begin();
			vector<ArrayOperandDataBase>::iterator rhs = lhs;
			rhs++;
			while (rhs != operandDataBase.arrayOperandList.end()) {
				// look at the operands on the rhs for a match with the one on the lhs
				if ((*lhs).arrayVariableName == (*rhs).arrayVariableName) {
					// A loop dependence has been identified

					// Mark the synthesized attribute to record
					// the loop dependence within this statement
					returnSynthesizedAttribute.setLoopDependence(TRUE);
				}

				rhs++;
			}
			break;
		}

		default:
			break;
		}

		break;
	}
	case EXPR_STMT: {
		printf("Found a EXPR STMT expression %s\n", astNode->unparseToString().c_str());

		// The assembly associated with the SgExprStatement is what 
		// triggers the generation of the transformation string
		SgExprStatement* expressionStatement = isSgExprStatement(astNode);
		ROSE_ASSERT(expressionStatement != NULL);

		ArrayAssignmentStatementQuerySynthesizedAttributeType innerLoopTransformation =
				synthesizedAttributeList[SgExprStatement_expression];

		// Call another global support
		// Create appropriate macros, nested loops, etc
		expressionStatementTransformation(expressionStatement, arrayAssignmentStatementQueryInheritedData,
				innerLoopTransformation, operandDataBase);

		break;
	}

		//		case TransformationSupport::PARENTHESIS_OPERATOR_CODE: {
		//			ROSE_ASSERT (operatorName == "operator()");
		//			// printf ("Indexing of InternalIndex objects in not implemented yet! \n");
		//
		//			// Now get the operands out and search for the offsets in the index objects
		//
		//			// We only want to pass on the transformationOptions as inherited attributes
		//			// to the indexOffsetQuery
		//			// list<int> & transformationOptionList = arrayAssignmentStatementQueryInheritedData.getTransformationOptions();
		//
		//			// string offsetString;
		//			string indexOffsetString[6]; // = {NULL,NULL,NULL,NULL,NULL,NULL};
		//
		//			// retrieve the variable name from the data base (so that we can add the associated index object names)
		//			// printf ("WARNING (WHICH OPERAND TO SELECT): operandDataBase.size() = %d \n",operandDataBase.size());
		//			// ROSE_ASSERT (operandDataBase.size() == 1);
		//			// string arrayVariableName = returnSynthesizedAttribute.arrayOperandList[0].arrayVariableName;
		//			int lastOperandInDataBase = operandDataBase.size() - 1;
		//			ArrayOperandDataBase & arrayOperandDB = operandDataBase.arrayOperandList[lastOperandInDataBase];
		//			// string arrayVariableName =
		//			//      operandDataBase.arrayOperandList[operandDataBase.size()-1].arrayVariableName;
		//			string arrayVariableName = arrayOperandDB.arrayVariableName;
		//
		//			string arrayDataPointerNameSubstring = string("_") + arrayVariableName;
		//
		//			// printf ("***** WARNING: Need to get identifier from the database using the ArrayOperandDataBase::generateIdentifierString() function \n");
		//
		//			if (expressionPtrList.size() == 0) {
		//				// Case of A() (index object with no offset integer expression) Nothing to do here (I think???)
		//				printf("Special case of Indexing with no offset! exiting ... \n");
		//				ROSE_ABORT();
		//
		//				returnString = "";
		//			} else {
		//				// Get the value of the offsets (start the search from the functionCallExp)
		//				SgExprListExp* exprListExp = functionCallExpression->get_args();
		//				ROSE_ASSERT (exprListExp != NULL);
		//
		//				SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
		//				SgExpressionPtrList::iterator i = expressionPtrList.begin();
		//
		//				// Case of indexing objects used within operator()
		//				int counter = 0;
		//				while (i != expressionPtrList.end()) {
		//					// printf ("Looking for the offset on #%d of %d (total) \n",counter,expressionPtrList.size());
		//
		//					// Build up the name of the final index variable (or at least give
		//					// it a unique number by dimension)
		//					string counterString = StringUtility::numberToString(counter + 1);
		//
		//					// Call another transformation mechanism to generate string for the index
		//					// expression (since we don't have an unparser mechanism in ROSE yet)
		//					indexOffsetString[counter] = IndexOffsetQuery::transformation(*i);
		//
		//					ROSE_ASSERT (indexOffsetString[counter].c_str() != NULL);
		//					// printf ("indexOffsetString [%d] = %s \n",counter,indexOffsetString[counter].c_str());
		//
		//					// Accumulate a list of all the InternalIndex, Index, and Range objects
		//					printf(" Warning - Need to handle indexNameList from the older code \n");
		//
		//					i++;
		//					counter++;
		//				}

		// Added VAR_REF case (moved from the local function)
	case VAR_REF: {
		// A VAR_REF has to output a string (the variable name)
#if DEBUG
		printf ("Found a variable reference expression \n");
#endif

		// Since we are at a leaf in the traversal of the AST this attribute list should a size of 0.
		ROSE_ASSERT(synthesizedAttributeList.size() == 0);

		SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
		ROSE_ASSERT(varRefExp != NULL);
		SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
		ROSE_ASSERT(variableSymbol != NULL);
		SgInitializedName* initializedName = variableSymbol->get_declaration();
		ROSE_ASSERT(initializedName != NULL);
		SgName variableName = initializedName->get_name();

		string buffer;
		string indexOffsetString;

		// Now compute the offset to the index objects (form a special query for this???)

		SgType* type = variableSymbol->get_type();
		ROSE_ASSERT(type != NULL);

		string typeName = TransformationSupport::getTypeName(type);
		ROSE_ASSERT(typeName.c_str() != NULL);

		// Recognize only these types at present
		if (typeName == "intArray" || typeName == "floatArray" || typeName == "doubleArray") {
			// Only define the variable name if we are using an object of array type
			// Copy the string from the SgName object to a string object
			string variableNameString = variableName.str();

#if DEBUG
			printf("Handle case of A++ array object VariableName: %s \n", variableNameString.c_str());
#endif

			if (arrayAssignmentStatementQueryInheritedData.arrayStatementDimensionDefined == TRUE) {
				cout << " Dim: " << arrayAssignmentStatementQueryInheritedData.arrayStatementDimension << endl;
				// The the globally computed array dimension from the arrayAssignmentStatementQueryInheritedData
				dimensionList.push_back(arrayAssignmentStatementQueryInheritedData.arrayStatementDimension);
			} else {
				dimensionList.push_back(6); // Default dimension for A++/P++
			}

			nodeList.push_back(isSgExpression(varRefExp));
			//processArrayRefExp(varRefExp, arrayAssignmentStatementQueryInheritedData);

			// Setup an intry in the synthesized attribute data base for this variable any
			// future results from analysis could be place there at this point as well
			// record the name in the synthesized attribute
			ROSE_ASSERT(operandDataBase.transformationOption > ArrayTransformationSupport::UnknownIndexingAccess);
			ArrayOperandDataBase arrayOperandDB = operandDataBase.setVariableName(variableNameString);
		}

		break;
	}

	default: {
		break;
	}
	} // End of main switch statement


#if DEBUG
	printf ("$$$$$ BOTTOM of arrayAssignmentStatementAssembly::evaluateSynthesizedAttribute (astNode = %s) \n",astNode->sage_class_name());
	printf ("      BOTTOM: returnString = \n%s \n",returnString.c_str());
#endif

	return returnSynthesizedAttribute;
}

void ArrayAssignmentStatementTransformation::atTraversalStart() {

#if DEBUG
	printf (" ============ ArrayAssignmentStatementTransformation Start ================= \n");
#endif

}

void ArrayAssignmentStatementTransformation::atTraversalEnd() {

	// Replace the VarRefExp at the end of traversal
	ROSE_ASSERT(nodeList.size() == dimensionList.size());
	vector<int>::iterator dimIter = dimensionList.begin();
	for (vector<SgExpression*>::iterator nodeIter = nodeList.begin(); nodeIter != nodeList.end(); nodeIter++, dimIter++) {
#if DEBUG
		cout << " Org Exp:" << (*nodeIter)->unparseToString() << " dim: " << (*dimIter) << endl;
#endif
		processArrayRefExp(isSgVarRefExp(*nodeIter), (*dimIter));
	}

#if DEBUG
	printf (" ============ ArrayAssignmentStatementTransformation End ================= \n");
#endif

}
