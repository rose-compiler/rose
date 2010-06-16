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
		if (SubtreeNeedsNormalization(forStatement->get_increment()))
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
		}
	}

	StatementInheritedAttribute scope(NULL);
	traverse(tree, scope);
}


StatementInheritedAttribute ExtractFunctionArguments::evaluateInheritedAttribute(SgNode* astNode, StatementInheritedAttribute parentStatementAttribute)
{
	printf("Inherited Attribute Visiting Node Type %s: %s\n", astNode->class_name().c_str(), astNode->unparseToString().c_str());
	StatementInheritedAttribute newAttribute = parentStatementAttribute;

	//return parentStatementAttribute;
	//Detect if we're inside the initializer of a for loop
	SgForInitStatement* forInitializer = isSgForInitStatement(astNode);
	if (forInitializer != NULL)
	{
		//We can't have a for loop inside the initializer of another for loop
		ROSE_ASSERT(parentStatementAttribute.forInitializer == NULL);
		newAttribute.forInitializer = forInitializer;
		return newAttribute;
	}

	//We can't insert anything inside the initializer of a for loop. So don't update the insertion statement
	if (parentStatementAttribute.forInitializer != NULL)
	{
		return newAttribute;
	}

	//Update wether we're in a for statement
	SgForStatement* forStatement = isSgForStatement(astNode);
	if (forStatement != NULL)
		newAttribute.forStatement = forStatement;

	//Update the location at which we can insert temporary variables
	SgStatement* statement = isSgStatement(astNode);
	if (statement != NULL)
		newAttribute.tempVarInsertertLocation = statement;

	return newAttribute;
}


/** Perform the actual instrumentatation to extract the function arguments. This is a post-order traversal. */
SynthetizedAttribute ExtractFunctionArguments::evaluateSynthesizedAttribute(SgNode* astNode, StatementInheritedAttribute parentStatementAttribute, SynthesizedAttributesList)
{
	//printf("Synthesized Attribute Visiting Node Type %s: %s\n", astNode->class_name().c_str(), astNode->unparseToString().c_str());
	//We're looking for function calls to rewrite
	SgFunctionCallExp* functionCall = isSgFunctionCallExp(astNode);
	if (functionCall == NULL)
		return false; //The return value is just a dummy

	SgFunctionDeclaration* functionDeclaration = functionCall->getAssociatedFunctionDeclaration();
	SgExprListExp* functionArgs = functionCall->get_args();
	ROSE_ASSERT(functionDeclaration != NULL && functionArgs != NULL);

	SgExpressionPtrList& argumentList = functionArgs->get_expressions();

	printf("\nExtracting arguments from function call to function %s, location %d:%d\n", functionDeclaration->get_name().str(),
			functionCall->get_file_info()->get_line(), functionCall->get_file_info()->get_col());

	//Go over all the function arguments, pull them out
	for (SgExpressionPtrList::const_iterator argIter = argumentList.begin();
			argIter != argumentList.end(); argIter++)
	{
		SgExpression* arg = *argIter;

		//No need to pull out parameters that are not complex expressions and
		//thus don't have side effects
		if (!FunctionArgumentNeedsNormalization(arg))
			continue;

		printf("Found %s, of return type %s.\n", arg->class_name().c_str(), arg->get_type()->unparseToCompleteString().c_str());

		//Get a temporary variable that evaluates to the given expression
		SgScopeStatement* scope = parentStatementAttribute.tempVarInsertertLocation->get_scope();
		SgVariableDeclaration* tempVarDeclaration = CreateTempVariableForExpression(arg, scope);

		//Insert the temporary variable declaration before the current statement
		SageInterface::insertStatement(parentStatementAttribute.tempVarInsertertLocation, tempVarDeclaration);
		SageInterface::replaceExpression(arg, SageBuilder::buildVarRefExp(tempVarDeclaration));
		printf("%s\n", tempVarDeclaration->unparseToString().c_str());
	}

	//Return a dummy value
	return false;
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


/** Given an expression, generates a temporary variable that evaluates to that expression
 * and replaces the expression by a reference to that temporary variable.
 * @param expression Expression which will be replaced by a variable
 * @return declaration of the temporary variable. Its initializer evaluates the original expression. */
SgVariableDeclaration* ExtractFunctionArguments::CreateTempVariableForExpression(SgExpression* expression, SgScopeStatement* scope)
{
	//TODO: this currently doesn't correctly handle const or aliased expressions
	//Also, a copy constructor is onvoked extra times compared to the original code

	//Generate a unique variable name
	SgType *expressionType = expression->get_type();
	SgName name = GenerateUniqueVariableName(scope).c_str();

	//Copy the expression and convert it to an intialization for the temporary variable
	SgTreeCopy copyHelp;
	SgExpression* expressionCopy = isSgExpression(expression->copy(copyHelp));
	ROSE_ASSERT(expressionCopy != NULL);
	SgAssignInitializer* initializer = new SgAssignInitializer(expressionCopy, expressionType);

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

		else if (isSgAssignInitializer(initializer))
		{
			SgAssignInitializer* assignInitializer = isSgAssignInitializer(initializer);
			SgExpression* rhs = assignInitializer->get_operand();
			SgVariableDeclaration* tempVariable = CreateTempVariableForExpression(rhs, forLoop->get_scope());
			SageInterface::insertStatement(forLoop, tempVariable);
			SageInterface::replaceExpression(rhs, SageBuilder::buildVarRefExp(tempVariable));
		}

		else if (isSgConstructorInitializer(initializer))
		{
			//TODO
			ROSE_ASSERT(false);
		}

		else if (isSgAggregateInitializer(initializer))
		{
			//What do we do here?
			ROSE_ASSERT(false);
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