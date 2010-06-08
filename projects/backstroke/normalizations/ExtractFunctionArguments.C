#include "ExtractFunctionArguments.h"

/** Perform the function argument extraction on all function calls in the given subtree of the AST. */
void ExtractFunctionArguments::NormalizeTree(SgNode* tree)
{
	StatementInheritedAttribute scope(NULL);
	traverse(tree, scope);
}

StatementInheritedAttribute ExtractFunctionArguments::evaluateInheritedAttribute(SgNode* astNode, StatementInheritedAttribute parentStatementAttribute)
{
	//Update the scope if we've entered a new scope
	SgStatement* statement = isSgStatement(astNode);
	if (statement != NULL)
	{
		return StatementInheritedAttribute(statement);
	}

	//We're looking for function calls to rewrite
	SgFunctionCallExp* functionCall = isSgFunctionCallExp(astNode);
	if (functionCall == NULL)
		return parentStatementAttribute;

	SgFunctionDeclaration* functionDeclaration = functionCall->getAssociatedFunctionDeclaration();
	SgExprListExp* functionArgs = functionCall->get_args();
	ROSE_ASSERT(functionDeclaration != NULL && functionArgs != NULL);

	SgExpressionPtrList& argumentList = functionArgs->get_expressions();

	printf("Found a function call to function %s\n", functionDeclaration->get_name().str());

	//Go over all the function arguments, pull them out
	for (size_t i = 0; i < argumentList.size(); i++)
	{
		SgExpression* arg = argumentList[i];

		//No need to pull out parameters that are not complex expressions and
		//thus don't have side effects
		if (isSgVarRefExp(arg) || isSgValueExp(arg))
			continue;

		//Generate a unique variable name
		SgType *argType = arg->get_type();
		Sg_File_Info *location = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
		ROSE_ASSERT(argType != NULL && location != NULL);
		std::ostringstream os;
		os << "__tempVar__" << location;
		SgName name = os.str().c_str();

		printf("Found %s, of return type %s.\n", arg->class_name().c_str(), argType->unparseToCompleteString().c_str());
		printf("Temporary variable name is %s.\n", name.str());

		SgVariableDeclaration* tempVarDeclaration = SageBuilder::buildVariableDeclaration(name, argType);
		ROSE_ASSERT(tempVarDeclaration != NULL);
		SageInterface::insertStatement(parentStatementAttribute.statement, tempVarDeclaration);
	}

	return parentStatementAttribute;
}

