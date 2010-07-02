#include "ExtractFunctionArguments.h"
#include "CPPDefinesAndNamespaces.h"
#include <sstream>
#include <stdio.h>
#include "Utilities.h"

using namespace std;
using namespace boost;

/** Perform the function argument extraction on all function calls in the given subtree of the AST. */
void ExtractFunctionArguments::NormalizeTree(SgNode* tree)
{
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

	SgExprListExp* functionArgs = functionCall->get_args();
	ROSE_ASSERT(functionArgs != NULL);

	SgExpressionPtrList& argumentList = functionArgs->get_expressions();

	printf("\nExtracting arguments from function call to function %s, location %d:%d\n", functionCall->get_function()->unparseToString().c_str(),
			functionCall->get_file_info()->get_line(), functionCall->get_file_info()->get_col());

	//Go over all the function arguments, pull them out
	foreach (SgExpression* arg, argumentList)
	{
		//No need to pull out parameters that are not complex expressions and
		//thus don't have side effects
		if (!FunctionArgumentNeedsNormalization(arg))
			continue;

		printf("Found %s, of return type %s.\n", arg->class_name().c_str(), arg->get_type()->unparseToCompleteString().c_str());

		//Build a declaration for the temporary variable
		SgScopeStatement* scope = functionCallInfo.tempVarDeclarationLocation->get_scope();
		tuple<SgVariableDeclaration*, SgAssignOp*, SgExpression*> tempVarInfo = backstroke_util::CreateTempVariableForExpression(arg, scope, false);
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
}


/** Given the expression which is the argument to a function call, returns true if that
 * expression should be pulled out into a temporary variable on a separate line.
 * E.g. if the expression contains a function call, it needs to be normalized, while if it
 * is a constant, there is no need to change it. */
bool ExtractFunctionArguments::FunctionArgumentNeedsNormalization(SgExpression* argument)
{
	//For right now, move everything but a constant value or an explicit variable access
	if (backstroke_util::IsVariableReference(argument) || isSgValueExp(argument))
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
