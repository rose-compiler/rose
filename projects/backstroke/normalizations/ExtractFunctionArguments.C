#include "ExtractFunctionArguments.h"
#include <sstream>
#include <stdio.h>

using namespace std;


/** Perform the function argument extraction on all function calls in the given subtree of the AST. */
void ExtractFunctionArguments::NormalizeTree(SgNode* tree)
{
	//First, preprocess all for loops
	Rose_STL_Container<SgNode*> forLoops = NodeQuery::querySubTree(tree, V_SgForStatement);

	for (Rose_STL_Container<SgNode*>::const_iterator iter = forLoops.begin(); iter != forLoops.end(); iter++)
	{
		SgForStatement* forStatement = isSgForStatement(*iter);
		ROSE_ASSERT(forStatement != NULL);

		//Check if the test expression will have to be rewritten. If so, we need
		//to move it out of the for loop. This means we also have to move the initialize out
		//of the for loop
		if (SubtreeNeedsNormalization(forStatement->get_test()))
		{
			SgStatementPtrList initStatementList = forStatement->get_init_stmt();

			for (SgStatementPtrList::iterator initStatementIter = initStatementList.begin();
					initStatementIter != initStatementList.end(); initStatementIter++)
			{
				SgStatement* initStatement = *initStatementIter;

				//The only way to have multiple variables in the init statement is if they are all variable declarations
				if (initStatementList.size() > 1)
				{
					ROSE_ASSERT(isSgVariableDeclaration(initStatement));
				}

				//Hoist this statement outside the for loop
				HoistStatementOutsideOfForLoop(forStatement, initStatement);
			}
		}

		//If the increment expression needs normalization, move it to the bottom of the loop.
		//This means we can't have any continue statements in the loop!
		SageInterface::moveForStatementIncrementIntoBody(forStatement);
		/*if (SubtreeNeedsNormalization(forStatement->get_increment()))
		{
			//The function SageInterface::moveForStatementIncrementIntoBody is broken as of right now
			SgExpression* incrementExpression = forStatement->get_increment();
			SageInterface::replaceExpression(incrementExpression, SageBuilder::buildNullExpression(), true);

			SgBasicBlock* loopBody = isSgBasicBlock(forStatement->get_loop_body());
			ROSE_ASSERT(loopBody != NULL);

			SgStatement* incrementStatement = SageBuilder::buildExprStatement(incrementExpression);
			SageInterface::appendStatement(incrementStatement, loopBody);

			//Check that there are no continue statements in the code and report an error if there are
			Rose_STL_Container<SgNode*> continueStatements = NodeQuery::querySubTree(forStatement, V_SgContinueStmt);
			if (continueStatements.size() > 0)
			{
				fprintf(stderr, "Function argument evaluation does not currently support 'continue' inside loops. Exiting");
				exit(1);
			}
		}*/


		//Ensure the for statement has a basic block
		SageInterface::ensureBasicBlockAsBodyOfFor(forStatement);
	}

	//Get all functions in function evaluation order
	vector<FunctionCallInfo> functionCalls = FunctionEvaluationOrderTraversal::GetFunctionCalls(tree);

	//Go through all the function calls and replace the arguments of each one
	for (vector<FunctionCallInfo>::const_iterator fnCallIter = functionCalls.begin(); fnCallIter != functionCalls.end(); fnCallIter++)
	{
		const FunctionCallInfo& functionCallInfo = *fnCallIter;
		RewriteFunctionCallArguments(functionCallInfo);
	}
}


/** Given the information about a function call (obtained through a traversal), extract its arguments
 * into temporary variables where it is necessary. */
void ExtractFunctionArguments::RewriteFunctionCallArguments(const FunctionCallInfo& functionCallInfo)
{
	SgFunctionCallExp* functionCall = functionCallInfo.functionCall;

	SgFunctionDeclaration* functionDeclaration = functionCall->getAssociatedFunctionDeclaration();
	SgExprListExp* functionArgs = functionCall->get_args();
	ROSE_ASSERT(functionDeclaration != NULL && functionArgs != NULL);

	SgExpressionPtrList& argumentList = functionArgs->get_expressions();

	printf("\nExtracting arguments from function call to function %s, location %d:%d\n", functionDeclaration->get_name().str(),
			functionCall->get_file_info()->get_line(), functionCall->get_file_info()->get_col());

	//Go over all the function arguments, pull them out
	for (SgExpressionPtrList::const_iterator argIter = argumentList.begin(); argIter != argumentList.end(); argIter++)
	{
		SgExpression* arg = *argIter;

		//No need to pull out parameters that are not complex expressions and
		//thus don't have side effects
		if (!FunctionArgumentNeedsNormalization(arg))
			continue;

		printf("Found %s, of return type %s.\n", arg->class_name().c_str(), arg->get_type()->unparseToCompleteString().c_str());

		//Build a declaration for the temporary variable
		SgScopeStatement* scope = functionCallInfo.tempVarDeclarationLocation->get_scope();
		SgVariableDeclaration* tempVarDeclaration = CreateTempVariableForExpression(arg, scope, functionCallInfo.initializeTempVarAtDeclaration);

		//Insert the temporary variable declaration
		InsertStatement(tempVarDeclaration, functionCallInfo.tempVarDeclarationLocation, functionCallInfo.tempVarDeclarationInsertionMode);

		//Insert an evaluation for the temporary variable (if required)
		if (functionCallInfo.tempVarEvaluationLocation != NULL)
		{
			SgVarRefExp* tempVarReference = SageBuilder::buildVarRefExp(tempVarDeclaration);
			SgTreeCopy copyHelp;
			SgExpression* argCopy = isSgExpression(arg->copy(copyHelp));
			ROSE_ASSERT(argCopy != NULL);
			SgAssignOp* assignment = SageBuilder::buildAssignOp(tempVarReference, argCopy);
			SgStatement* assignmentStatement = SageBuilder::buildExprStatement(assignment);

			InsertStatement(assignmentStatement, functionCallInfo.tempVarEvaluationLocation, functionCallInfo.tempVarEvaluationInsertionMode);
		}

		//Replace the argument with the new temporary variable
		SageInterface::replaceExpression(arg, SageBuilder::buildVarRefExp(tempVarDeclaration));
	}
}


/** Given the expression which is the argument to a function call, returns true if that
 * expression should be pulled out into a temporary variable on a separate line.
 * E.g. if the expression contains a function call, it needs to be normalized, while if it
 * is a constant, there is no need to change it. */
bool ExtractFunctionArguments::FunctionArgumentNeedsNormalization(SgExpression* argument)
{
	//For right now, move everything but a constant value or an explicit variable access
	if (isSgVarRefExp(argument) || isSgValueExp(argument))
		return false;

	return true;
}


/** Returns true if any of the arguments of the given function call will need to
 * be extracted. */
bool ExtractFunctionArguments::FunctionCallNeedsNormalization(SgFunctionCallExp* functionCall)
{
	ROSE_ASSERT(functionCall != NULL);
	SgExprListExp* functionArgs = functionCall->get_args();
	ROSE_ASSERT(functionArgs != NULL);
	SgExpressionPtrList& argumentList = functionArgs->get_expressions();

	for (SgExpressionPtrList::const_iterator argIter = argumentList.begin();
			argIter != argumentList.end(); argIter++)
	{
		if (FunctionArgumentNeedsNormalization(*argIter))
			return true;
	}
	return false;
}


/** Returns true if any function calls in the given subtree will need to be
 * instrumented. (to extract function arguments). */
bool ExtractFunctionArguments::SubtreeNeedsNormalization(SgNode* top)
{
	ROSE_ASSERT(top != NULL);
	Rose_STL_Container<SgNode*> functionCalls = NodeQuery::querySubTree(top, V_SgFunctionCallExp);
	for (Rose_STL_Container<SgNode*>::const_iterator iter = functionCalls.begin(); iter != functionCalls.end(); iter++)
	{
		SgFunctionCallExp* functionCall = isSgFunctionCallExp(*iter);
		ROSE_ASSERT(functionCall != NULL);
		if (FunctionArgumentNeedsNormalization(functionCall))
			return true;
	}

	return false;
}


/** Given an expression, generates a temporary variable whose initializer optionally evaluates
 * that expression. The temporary variable created is a const reference to avoid potentially calling
 * extra copy constructors.
 * @param expression Expression which will be replaced by a variable
 * @param scope scope in which the temporary variable will be generated
 * @return declaration of the temporary variable. Its initializer evaluates the original expression. */
SgVariableDeclaration* ExtractFunctionArguments::CreateTempVariableForExpression(SgExpression* expression, SgScopeStatement* scope, bool initializeInDeclaration)
{
	SgType *expressionType = expression->get_type();

	//Generate a unique variable name
	SgName name = GenerateUniqueVariableName(scope).c_str();

	SgAssignInitializer* initializer = NULL;

	//Copy the expression and convert it to an intialization for the temporary variable
	if (initializeInDeclaration)
	{
		SgTreeCopy copyHelp;
		SgExpression* expressionCopy = isSgExpression(expression->copy(copyHelp));
		ROSE_ASSERT(expressionCopy != NULL);
		initializer = new SgAssignInitializer(expressionCopy, expressionType);
	}

	SgVariableDeclaration* tempVarDeclaration = SageBuilder::buildVariableDeclaration(name, expressionType, initializer);
	ROSE_ASSERT(tempVarDeclaration != NULL);

	return tempVarDeclaration;
}


/** Take a statement that is located somewhere inside the for loop and move it right before the
 * for looop. If the statement is a variable declaration, the declaration is left in its original
 * location to preserve its scope, and a new temporary variable is introduced. */
void ExtractFunctionArguments::HoistStatementOutsideOfForLoop(SgForStatement* forLoop, SgStatement* statement)
{
	//Let's handle the easy case first. Just move the whole thing outside the loop
	if (!isSgVariableDeclaration(statement))
	{
		SgTreeCopy treeCopy;
		SgStatement* statementCopy = isSgStatement(statement->copy(treeCopy));
		ROSE_ASSERT(statementCopy != NULL);
		SageInterface::removeStatement(statement);
		SageInterface::insertStatement(forLoop, statementCopy, true);
		return;
	}

	//We have a declaration. We want to preserve its scope but still hoist its initialization statement if necessary
	SgVariableDeclaration* declaration = isSgVariableDeclaration(statement);
	SgInitializedNamePtrList& variableInitializedNames = declaration->get_variables();
	SgInitializedNamePtrList::const_iterator variableInitalizedNameIter = variableInitializedNames.begin();
	for (; variableInitalizedNameIter != variableInitializedNames.end(); variableInitalizedNameIter++)
	{
		SgInitializedName* initializedName = *variableInitalizedNameIter;
		ROSE_ASSERT(initializedName != NULL);

		SgInitializer* initializer = initializedName->get_initializer();

		//If the initializer is NULL, there is nothing to hoist
		if (initializer == NULL)
			continue;

		bool isAggregateInitializer = isSgAggregateInitializer(initializer);

		SgTreeCopy treeCopy;
		SgType* tempVarType = initializer->get_type();

		//Create a new temporary variable that has the same initializer
		SgInitializer* initializerCopy = isSgInitializer(initializer->copy(treeCopy));
		SgName varName = GenerateUniqueVariableName(forLoop->get_scope());
		SgVariableDeclaration* tempVarDeclaration = SageBuilder::buildVariableDeclaration(varName, tempVarType, initializerCopy);
		SageInterface::insertStatementBefore(forLoop, tempVarDeclaration);

		//Change this variable to be a copy of the temporary
		SgAssignInitializer* assignFromTempVar = SageBuilder::buildAssignInitializer(SageBuilder::buildVarRefExp(tempVarDeclaration),
				tempVarType);
		SageInterface::replaceExpression(initializer, assignFromTempVar);

		//This is an initialize of the form int x[] = {2, 3}
		//We need to keep the temporary variable as an array, but change the type of x to be a pointer
		if (isAggregateInitializer)
		{
			SgType* originalType = initializedName->get_type();
			ROSE_ASSERT(isSgArrayType(originalType));
			SgType* baseType = isSgArrayType(tempVarType)->get_base_type();
			SgType* pointerizedArrayType = SageBuilder::buildPointerType(baseType);
			initializedName->set_type(pointerizedArrayType);
			delete originalType;
		}
	}
}


/** Generate a name that is unique in the current scope and any parent and children scopes.
 * @param baseName the word to be included in the variable names. */
string ExtractFunctionArguments::GenerateUniqueVariableName(SgScopeStatement* scope, std::string baseName)
{
	//TODO: This implementation is incomplete; it does not check for collsions
	static int counter = 0;
	counter++;

	ostringstream name;
	name << "__" << baseName << counter << "__";
	return name.str();
}


/** Insert a new statement in the specified location. The actual insertion can occur either before or after the location
 * depending on the insertion mode. */
void ExtractFunctionArguments::InsertStatement(SgStatement* newStatement, SgStatement* location, FunctionCallInfo::InsertionMode insertionMode)
{
	switch (insertionMode)
	{
		case FunctionCallInfo::INSERT_BEFORE:
			SageInterface::insertStatementBefore(location, newStatement);
			break;
		case FunctionCallInfo::APPEND_SCOPE:
		{
			SgScopeStatement* scopeStatement = isSgScopeStatement(location);
			ROSE_ASSERT(scopeStatement != NULL);
			SageInterface::appendStatement(newStatement, scopeStatement);
			break;
		}
		case FunctionCallInfo::INVALID:
		default:
			ROSE_ASSERT(false);
	}
}

/** Traverses the subtree of the given AST node and finds all function calls in
 * function-evaluation order. */


		  /*static*/std::vector<FunctionCallInfo> FunctionEvaluationOrderTraversal::GetFunctionCalls(SgNode* root)
{
	FunctionEvaluationOrderTraversal t;
	FunctionCallInheritedAttribute rootAttribute;
	t.traverse(root, rootAttribute);

	return t.functionCalls;
}


/** Visits AST nodes in pre-order */
FunctionCallInheritedAttribute FunctionEvaluationOrderTraversal::evaluateInheritedAttribute(SgNode* astNode, FunctionCallInheritedAttribute parentAttribute)
{
	FunctionCallInheritedAttribute result = parentAttribute;
	SgForStatement* parentForLoop = isSgForStatement(parentAttribute.currentLoop);

	if (isSgForStatement(astNode))
		result.currentLoop = isSgForStatement(astNode);
	else if (isSgWhileStmt(astNode))
		result.currentLoop = isSgWhileStmt(astNode);
	else if (isSgDoWhileStmt(astNode))
		result.currentLoop = isSgDoWhileStmt(astNode);
	else if (isSgForInitStatement(astNode))
	{
		ROSE_ASSERT(result.forLoopStatus == FunctionCallInheritedAttribute::NOT_IN_FOR);
		result.forLoopStatus = FunctionCallInheritedAttribute::INSIDE_FOR_INIT;
		ROSE_ASSERT(isSgForStatement(result.currentLoop));
	}
	else if (parentForLoop != NULL && parentForLoop->get_test() == astNode)
	{
		ROSE_ASSERT(result.forLoopStatus == FunctionCallInheritedAttribute::NOT_IN_FOR);
		result.forLoopStatus = FunctionCallInheritedAttribute::INSIDE_FOR_TEST;
	}
	else if (parentForLoop != NULL && parentForLoop->get_increment() == astNode)
	{
		ROSE_ASSERT(result.forLoopStatus == FunctionCallInheritedAttribute::NOT_IN_FOR);
		result.forLoopStatus = FunctionCallInheritedAttribute::INSIDE_FOR_INCREMENT;
	}


	if (isSgStatement(astNode))
		result.lastStatement = isSgStatement(astNode);

	return result;
}


/** Visits AST nodes in post-order. This is function-evaluation order. */
SynthetizedAttribute FunctionEvaluationOrderTraversal::evaluateSynthesizedAttribute(SgNode* astNode, FunctionCallInheritedAttribute parentAttribute, SynthesizedAttributesList)
{
	SgFunctionCallExp* functionCall = isSgFunctionCallExp(astNode);
	if (functionCall == NULL)
		return false; //dummy return value

	FunctionCallInfo functionCallInfo(functionCall);

	//Handle for loops (being inside the body of a for loop doesn't need special handling)
	SgForStatement* forLoop = isSgForStatement(parentAttribute.currentLoop);
	if (parentAttribute.forLoopStatus == FunctionCallInheritedAttribute::INSIDE_FOR_INIT)
	{
		//The variables needed for the initialization of a for loop should be declared and
		//evaluated outside the for loop
		ROSE_ASSERT(forLoop != NULL);
		functionCallInfo.tempVarDeclarationLocation = forLoop;
		functionCallInfo.initializeTempVarAtDeclaration = true;
		functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
	}
	else if (parentAttribute.forLoopStatus == FunctionCallInheritedAttribute::INSIDE_FOR_TEST)
	{
		//The variables for the test expression of a for loop need to be declared and initialized outside the for loop,
		//because the test is evaluated before the for loop is entered. They also need to be reassigned at the bottom
		//Of the for loop
		ROSE_ASSERT(forLoop != NULL);
		functionCallInfo.tempVarDeclarationLocation = forLoop;
		functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
		functionCallInfo.initializeTempVarAtDeclaration = true;

		SgBasicBlock* loopBody = isSgBasicBlock(forLoop->get_loop_body());
		ROSE_ASSERT(loopBody != NULL); //At this point, the for loop should have been preprocessed to have a basic block
		functionCallInfo.tempVarEvaluationLocation = loopBody;
		functionCallInfo.tempVarEvaluationInsertionMode = FunctionCallInfo::APPEND_SCOPE;
	}
	else if (parentAttribute.forLoopStatus == FunctionCallInheritedAttribute::INSIDE_FOR_INCREMENT)
	{
		//The variables for an increment expression need to be declared outside the for loop, but initialized
		//at the bottom of the loop body
		ROSE_ASSERT(forLoop != NULL);
		functionCallInfo.tempVarDeclarationLocation = forLoop;
		functionCallInfo.initializeTempVarAtDeclaration = false;

		SgBasicBlock* loopBody = isSgBasicBlock(forLoop->get_loop_body());
		ROSE_ASSERT(loopBody != NULL);
		functionCallInfo.tempVarEvaluationInsertionMode = FunctionCallInfo::APPEND_SCOPE;
	}


	else
	{
		//Assume we're in a basic block. Then just insert right before the current statement
		functionCallInfo.tempVarDeclarationLocation = parentAttribute.lastStatement;
		functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
		functionCallInfo.initializeTempVarAtDeclaration = true;
	}

	functionCalls.push_back(functionCallInfo);
	return false; //dummy return value
}