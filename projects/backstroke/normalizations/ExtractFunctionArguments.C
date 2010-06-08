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



	return parentStatementAttribute;
}

/** Perform the actual instrumentatation to extract the function arguments. This is a post-order traversal. */
SynthetizedAttribute ExtractFunctionArguments::evaluateSynthesizedAttribute(SgNode* astNode, StatementInheritedAttribute parentStatementAttribute, SynthesizedAttributesList)
{
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

		//Copy the argument expression and convert it to an intialization for the temporary variable
		SgTreeCopy copyHelp;
		SgExpression* argumentExpressionCopy = isSgExpression(arg->copy(copyHelp));
		ROSE_ASSERT(argumentExpressionCopy != NULL);
		SgAssignInitializer* initializer = new SgAssignInitializer(argumentExpressionCopy, argType);

		//Insert the declaration for the temporary variable right before this statement
		SgVariableDeclaration* tempVarDeclaration = SageBuilder::buildVariableDeclaration(name, argType, initializer);
		ROSE_ASSERT(tempVarDeclaration != NULL);
		SageInterface::insertStatement(parentStatementAttribute.statement, tempVarDeclaration);
		printf("%s\n", tempVarDeclaration->unparseToString().c_str());

		//Replace the function argument with a reference to the variable in question
		functionArgs->replace_expression(arg, SageBuilder::buildVarRefExp(tempVarDeclaration));
	}

	//Return a dummy value
	return false;
}
