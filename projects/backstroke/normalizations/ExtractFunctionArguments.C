#include "ExtractFunctionArguments.h"
#include <sstream>
#include <stdio.h>
#include "Utilities.h"

using namespace std;
using namespace boost;

/** Perform the function argument extraction on all function calls in the given subtree of the AST. */
void ExtractFunctionArguments::NormalizeTree(SgNode* tree)
{
	//Check that there are no continue statements in the code and report an error if there are
	Rose_STL_Container<SgNode*> continueStatements = NodeQuery::querySubTree(tree, V_SgContinueStmt);
	if (continueStatements.size() > 0)
	{
		fprintf(stderr, "Function argument evaluation does not currently support 'continue' inside loops. Exiting");
		exit(1);
	}

	//First, preprocess all for loops
	Rose_STL_Container<SgNode*> forLoops = NodeQuery::querySubTree(tree, V_SgForStatement);

	for (Rose_STL_Container<SgNode*>::const_iterator iter = forLoops.begin(); iter != forLoops.end(); iter++)
	{
		SgForStatement* forStatement = isSgForStatement(*iter);
		ROSE_ASSERT(forStatement != NULL);

		//Check if the test expression will have to be rewritten. If so, we need
		//to move it out of the for loop. This means we also have to move the initializer out
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
		//We should use SageInterface::moveForStatementIncrementIntoBody(forStatement) if we want
		//to support continue statements
		if (SubtreeNeedsNormalization(forStatement->get_increment()))
		{
			//The function SageInterface::moveForStatementIncrementIntoBody is broken as of right now
			SgExpression* incrementExpression = forStatement->get_increment();
			SageInterface::replaceExpression(incrementExpression, SageBuilder::buildNullExpression(), true);

			SgBasicBlock* loopBody = isSgBasicBlock(forStatement->get_loop_body());
			ROSE_ASSERT(loopBody != NULL);

			SgStatement* incrementStatement = SageBuilder::buildExprStatement(incrementExpression);
			SageInterface::appendStatement(incrementStatement, loopBody);
		}
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
		tuple<SgVariableDeclaration*, SgAssignOp*, SgExpression*> tempVarInfo = CreateTempVariableForExpression(arg, scope, functionCallInfo.initializeTempVarAtDeclaration);
		SgVariableDeclaration* tempVarDeclaration = tempVarInfo.get<0>();
		SgAssignOp* tempVarAssignment = tempVarInfo.get<1>();
		SgExpression* tempVarReference = tempVarInfo.get<2>();

		//Insert the temporary variable declaration
		InsertStatement(tempVarDeclaration, functionCallInfo.tempVarDeclarationLocation, functionCallInfo.tempVarDeclarationInsertionMode);

		//Insert an evaluation for the temporary variable (if required)
		if (functionCallInfo.tempVarEvaluationLocation != NULL)
		{
			SgStatement* assignmentStatement = SageBuilder::buildExprStatement(tempVarAssignment);
			InsertStatement(assignmentStatement, functionCallInfo.tempVarEvaluationLocation, functionCallInfo.tempVarEvaluationInsertionMode);
		}
		else
		{
			delete tempVarAssignment;
		}

		//Replace the argument with the new temporary variable
		SageInterface::replaceExpression(arg, tempVarReference);
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
  * that expression. Then, the var reference expression returned can be used instead of the original
  * expression. The temporary variable created can be reassigned to the expression by the returned SgAssignOp;
  * this can be used when the expression the variable represents needs to be evaluated. NOTE: This handles
  * reference types correctly by using pointer types for the temporary.
  * @param expression Expression which will be replaced by a variable
  * @param scope scope in which the temporary variable will be generated
  * @return declaration of the temporary variable, an assignment op to
  *			reevaluate the expression, and a a variable reference expression to use instead of
  *         the original expression. Delete the results that you don't need! */
boost::tuple<SgVariableDeclaration*, SgAssignOp*, SgExpression*> ExtractFunctionArguments::CreateTempVariableForExpression(SgExpression* expression, SgScopeStatement* scope, bool initializeInDeclaration)
{
	SgTreeCopy copyHelp;
	SgType* expressionType = expression->get_type();
	SgType* variableType = expressionType;

	//If the expression has a reference type, we need to use a pointer type for the temporary variable.
	//Else, re-assigning the variable is not possible
	bool isReferenceType = isSgReferenceType(expressionType);
	if (isReferenceType)
	{
		SgType* expressionBaseType = isSgReferenceType(expressionType)->get_base_type();
		variableType = SageBuilder::buildPointerType(expressionBaseType);
	}

	//Generate a unique variable name
	string name = backstroke_util::GenerateUniqueVariableName(scope);

	//Initialize the temporary variable to an evaluation of the expression
	SgAssignInitializer* initializer = NULL;
	SgExpression* tempVarInitExpression = isSgExpression(expression->copy(copyHelp));
	ROSE_ASSERT(tempVarInitExpression != NULL);
	if (isReferenceType)
	{
		//FIXME: the next line is hiding a bug in ROSE. Remove this line and talk to Dan about the resulting assert
		tempVarInitExpression->set_lvalue(false);

		tempVarInitExpression = SageBuilder::buildAddressOfOp(tempVarInitExpression);
	}

	//Optionally initialize the variable in its declaration
	if (initializeInDeclaration)
	{
		SgExpression* initExpressionCopy = isSgExpression(tempVarInitExpression->copy(copyHelp));
		initializer = SageBuilder::buildAssignInitializer(initExpressionCopy);
	}

	SgVariableDeclaration* tempVarDeclaration = SageBuilder::buildVariableDeclaration(name, variableType, initializer, scope);
	ROSE_ASSERT(tempVarDeclaration != NULL);

	//Now create the assignment op for reevaluating the expression
	SgVarRefExp* tempVarReference = SageBuilder::buildVarRefExp(tempVarDeclaration);
	SgAssignOp* assignment = SageBuilder::buildAssignOp(tempVarReference, tempVarInitExpression);

	//Build the variable reference expression that can be used in place of the original expresion
	SgExpression* varRefExpression = SageBuilder::buildVarRefExp(tempVarDeclaration);
	if (isReferenceType)
	{
		//The temp variable is a pointer type, so dereference it before usint it
		varRefExpression = SageBuilder::buildPointerDerefExp(varRefExpression);
	}

	return make_tuple(tempVarDeclaration, assignment, varRefExpression);
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
		SgName varName = backstroke_util::GenerateUniqueVariableName(forLoop->get_scope());
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

			//We could insert an explicit cast here to get rid of a ROSE warning during unparsing, but it's not necessary
		}
	}
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
			if (scopeStatement == NULL)
			{
				scopeStatement = isSgScopeStatement(SageInterface::ensureBasicBlockAsParent(location));
			}
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
	SgWhileStmt* parentWhileLoop = isSgWhileStmt(parentAttribute.currentLoop);
	SgDoWhileStmt* parentDoWhileLoop = isSgDoWhileStmt(parentAttribute.currentLoop);

	if (isSgForStatement(astNode))
		result.currentLoop = isSgForStatement(astNode);
	else if (isSgWhileStmt(astNode))
		result.currentLoop = isSgWhileStmt(astNode);
	else if (isSgDoWhileStmt(astNode))
		result.currentLoop = isSgDoWhileStmt(astNode);
	else if (isSgForInitStatement(astNode))
	{
		ROSE_ASSERT(result.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP);
		result.loopStatus = FunctionCallInheritedAttribute::INSIDE_FOR_INIT;
		ROSE_ASSERT(isSgForStatement(result.currentLoop));
	}
	else if (parentForLoop != NULL && parentForLoop->get_test() == astNode)
	{
		ROSE_ASSERT(result.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP);
		result.loopStatus = FunctionCallInheritedAttribute::INSIDE_FOR_TEST;
	}
	else if (parentForLoop != NULL && parentForLoop->get_increment() == astNode)
	{
		ROSE_ASSERT(result.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP);
		result.loopStatus = FunctionCallInheritedAttribute::INSIDE_FOR_INCREMENT;
	}
	else if (parentWhileLoop != NULL && parentWhileLoop->get_condition() == astNode)
	{
		ROSE_ASSERT(result.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP);
		result.loopStatus = FunctionCallInheritedAttribute::INSIDE_WHILE_CONDITION;
	}
	else if (parentDoWhileLoop != NULL && parentDoWhileLoop->get_condition() == astNode)
	{
		ROSE_ASSERT(result.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP);
		result.loopStatus = FunctionCallInheritedAttribute::INSIDE_DO_WHILE_CONDITION;
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
	if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_FOR_INIT)
	{
		//The variables needed for the initialization of a for loop should be declared and
		//evaluated outside the for loop
		ROSE_ASSERT(forLoop != NULL);
		functionCallInfo.tempVarDeclarationLocation = forLoop;
		functionCallInfo.initializeTempVarAtDeclaration = true;
		functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
	}
	else if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_FOR_TEST)
	{
		//The variables for the test expression of a for loop need to be declared and initialized outside the for loop,
		//because the test is evaluated before the for loop is entered. They also need to be reassigned at the bottom
		//Of the for loop
		ROSE_ASSERT(forLoop != NULL);
		functionCallInfo.tempVarDeclarationLocation = forLoop;
		functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
		functionCallInfo.initializeTempVarAtDeclaration = true;

		functionCallInfo.tempVarEvaluationLocation = forLoop->get_loop_body();
		functionCallInfo.tempVarEvaluationInsertionMode = FunctionCallInfo::APPEND_SCOPE;
	}
	else if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_FOR_INCREMENT)
	{
		//This should have been preprocessed to be at the bottom of the loop
		ROSE_ASSERT(false);
	}
	else if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_WHILE_CONDITION)
	{
		SgWhileStmt* whileLoop = isSgWhileStmt(parentAttribute.currentLoop);
		ROSE_ASSERT(whileLoop != NULL);
		//The variables for the test expression of a while-loop need to be declared and initialized outside the for loop,
		//because the test is evaluated before the while-loop is entered. They also need to be reassigned at the bottom
		//Of the loop
		functionCallInfo.tempVarDeclarationLocation = whileLoop;
		functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
		functionCallInfo.initializeTempVarAtDeclaration = true;

		functionCallInfo.tempVarEvaluationLocation = whileLoop->get_body();
		functionCallInfo.tempVarEvaluationInsertionMode = FunctionCallInfo::APPEND_SCOPE;
	}
	else if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_DO_WHILE_CONDITION)
	{
		SgDoWhileStmt* doWhileLoop = isSgDoWhileStmt(parentAttribute.currentLoop);
		ROSE_ASSERT(doWhileLoop);
		//Temprary variables should be declared before the loop, but initialized at the botom of the loop body
		functionCallInfo.tempVarDeclarationLocation = doWhileLoop;
		functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
		functionCallInfo.initializeTempVarAtDeclaration = false;

		functionCallInfo.tempVarEvaluationLocation = doWhileLoop->get_body();
		functionCallInfo.tempVarEvaluationInsertionMode = FunctionCallInfo::APPEND_SCOPE;
	}

	else if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP)
	{
		//Assume we're in a basic block. Then just insert right before the current statement
		ROSE_ASSERT(parentAttribute.loopStatus = FunctionCallInheritedAttribute::NOT_IN_LOOP);
		functionCallInfo.tempVarDeclarationLocation = parentAttribute.lastStatement;
		functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
		functionCallInfo.initializeTempVarAtDeclaration = true;
	}

	else
	{
		//Unhandled condition?!
		ROSE_ASSERT(false);
	}

	functionCalls.push_back(functionCallInfo);
	return false; //dummy return value
}