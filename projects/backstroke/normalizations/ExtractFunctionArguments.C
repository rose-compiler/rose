#include "ExtractFunctionArguments.h"
#include "utilities/cppDefinesAndNamespaces.h"
#include <sstream>
#include <stdio.h>
#include "utilities/utilities.h"

using namespace std;
using namespace boost;

/** Perform the function argument extraction on all function calls in the given subtree of the AST.
  * Returns true on sucess, false on failure (unsupported code). */
bool ExtractFunctionArguments::NormalizeTree(SgNode* tree)
{
	//Check if there are any constructor calls whose arguments need to be normalized
	//This is not supported by this normalization
	Rose_STL_Container<SgNode*> constructorInitializers = NodeQuery::querySubTree(tree, V_SgConstructorInitializer);
	foreach (SgNode* constructorNode, constructorInitializers)
	{
		SgConstructorInitializer* constructor = isSgConstructorInitializer(constructorNode);
		if (FunctionArgsNeedNormalization(constructor->get_args()))
		{
			fprintf(stderr, "The function argument extraction normalization does not support extracting arguments from"
					"\n constructors. Please rewrite the following constructor call on line %d in file %s:\n%s\n",
					constructor->get_file_info()->get_line(),
					constructor->get_file_info()->get_filename(),
					constructor->unparseToString().c_str());
			return false;
		}
	}

	//Get all functions in function evaluation order
	vector<FunctionCallInfo> functionCalls = FunctionEvaluationOrderTraversal::GetFunctionCalls(tree);

	//Go through all the function calls and replace the arguments of each one
	bool success = true;
	for (vector<FunctionCallInfo>::const_iterator fnCallIter = functionCalls.begin(); fnCallIter != functionCalls.end(); fnCallIter++)
	{
		const FunctionCallInfo& functionCallInfo = *fnCallIter;
		success = success & RewriteFunctionCallArguments(functionCallInfo);
	}

	return success;
}


/** Given the information about a function call (obtained through a traversal), extract its arguments
 * into temporary variables where it is necessary.
 * Returns true on sucess, false on failure (unsupported code). */
bool ExtractFunctionArguments::RewriteFunctionCallArguments(const FunctionCallInfo& functionCallInfo)
{
	SgFunctionCallExp* functionCall = functionCallInfo.functionCall;

	if (SubtreeNeedsNormalization(functionCall->get_function()))
	{
		fprintf(stderr, "%s:%d: The following function call has potential side effects in the function reference expression:\n",
				functionCall->get_file_info()->get_filename(), functionCall->get_file_info()->get_line());
		fprintf(stderr, "\t%s\n", functionCall->unparseToString().c_str());
		fprintf(stderr, "If you are using function pointers, save the function pointer first and then call the function on another line.\n");
		return false;
	}

	SgExprListExp* functionArgs = functionCall->get_args();
	ROSE_ASSERT(functionArgs != NULL);

	SgExpressionPtrList& argumentList = functionArgs->get_expressions();

	//Go over all the function arguments, pull them out
	foreach (SgExpression* arg, argumentList)
	{
		//No need to pull out parameters that are not complex expressions and
		//thus don't have side effects
		if (!FunctionArgumentNeedsNormalization(arg))
			continue;

		//Build a declaration for the temporary variable
		SgScopeStatement* scope = functionCallInfo.tempVarDeclarationLocation->get_scope();
		tuple<SgVariableDeclaration*, SgAssignOp*, SgExpression*> tempVarInfo = BackstrokeUtility::CreateTempVariableForExpression(arg, scope, false);
		SgVariableDeclaration* tempVarDeclaration = tempVarInfo.get<0>();
		SgAssignOp* tempVarAssignment = tempVarInfo.get<1>();
		SgExpression* tempVarReference = tempVarInfo.get<2>();

		//Insert the temporary variable declaration
		InsertStatement(tempVarDeclaration, functionCallInfo.tempVarDeclarationLocation, functionCallInfo.tempVarDeclarationInsertionMode);

		//Replace the argument with the new temporary variable
		SageInterface::replaceExpression(arg, tempVarReference);

		//Build a CommaOp that evaluates the temporary variable and proceeds to the original function call expression
		SgExpression* placeholderExp = SageBuilder::buildIntVal(7);
		SgCommaOpExp* comma = SageBuilder::buildCommaOpExp(tempVarAssignment, placeholderExp);
		SageInterface::replaceExpression(functionCall, comma, true);
		SageInterface::replaceExpression(placeholderExp, functionCall);
	}

	return true;
}


/** Given the expression which is the argument to a function call, returns true if that
 * expression should be pulled out into a temporary variable on a separate line.
 * E.g. if the expression contains a function call, it needs to be normalized, while if it
 * is a constant, there is no need to change it. */
bool ExtractFunctionArguments::FunctionArgumentNeedsNormalization(SgExpression* argument)
{
	//For right now, move everything but a constant value or an explicit variable access
	if (BackstrokeUtility::isVariableReference(argument) || isSgValueExp(argument) || isSgFunctionRefExp(argument))
		return false;

	return true;
}


/** Returns true if any of the arguments of the given function call will need to
 * be extracted. */
bool ExtractFunctionArguments::FunctionArgsNeedNormalization(SgExprListExp* functionArgs)
{
	ROSE_ASSERT(functionArgs != NULL);
	SgExpressionPtrList& argumentList = functionArgs->get_expressions();

	foreach(SgExpression* functionArgument, argumentList)
	{
		if (FunctionArgumentNeedsNormalization(functionArgument))
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
	if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_FOR_INIT
			|| parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_FOR_TEST
			|| parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_FOR_INCREMENT)
	{
		SgForStatement* forLoop = isSgForStatement(parentAttribute.currentLoop);
		ROSE_ASSERT(forLoop != NULL);
		//Temprary variables should be declared before the loop
		functionCallInfo.tempVarDeclarationLocation = forLoop;
		functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
	}
	else if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_WHILE_CONDITION)
	{
		SgWhileStmt* whileLoop = isSgWhileStmt(parentAttribute.currentLoop);
		ROSE_ASSERT(whileLoop != NULL);
		//Temprary variables should be declared before the loop
		functionCallInfo.tempVarDeclarationLocation = whileLoop;
		functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
	}
	else if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::INSIDE_DO_WHILE_CONDITION)
	{
		SgDoWhileStmt* doWhileLoop = isSgDoWhileStmt(parentAttribute.currentLoop);
		ROSE_ASSERT(doWhileLoop);
		//Temprary variables should be declared before the loop
		functionCallInfo.tempVarDeclarationLocation = doWhileLoop;
		functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
	}
	else if (parentAttribute.loopStatus == FunctionCallInheritedAttribute::NOT_IN_LOOP)
	{
		//Assume we're in a basic block. Then just insert right before the current statement
		ROSE_ASSERT(parentAttribute.loopStatus = FunctionCallInheritedAttribute::NOT_IN_LOOP);
		functionCallInfo.tempVarDeclarationLocation = parentAttribute.lastStatement;
		functionCallInfo.tempVarDeclarationInsertionMode = FunctionCallInfo::INSERT_BEFORE;
	}
	else
	{
		//Unhandled condition?!
		ROSE_ASSERT(false);
	}

	functionCalls.push_back(functionCallInfo);
	return false; //dummy return value
}
