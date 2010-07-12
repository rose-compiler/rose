#include "Utilities.h"
#include "CPPDefinesAndNamespaces.h"

#include "rose.h"
#include <boost/lexical_cast.hpp>


/** Generate a name that is unique in the current scope and any parent and children scopes.
 * @param baseName the word to be included in the variable names. */
string backstroke_util::GenerateUniqueVariableName(SgScopeStatement* scope, std::string baseName)
{
	//TODO: This implementation tends to generate numbers that are unnecessarily high.
	static int counter = 0;

	string name;
	bool collision = false;
	do
	{
		name = "__" + baseName + lexical_cast<string>(counter++) + "__";

		//Look up the name in the parent scopes
		SgSymbol* nameSymbol = SageInterface::lookupSymbolInParentScopes(SgName(name), scope);
		collision = (nameSymbol != NULL);

		//Look up the name in the children scopes
		Rose_STL_Container<SgNode*> childScopes = NodeQuery::querySubTree(scope, V_SgScopeStatement);
		foreach (SgNode* childScope, childScopes)
		{
			SgScopeStatement* childScopeStatement = isSgScopeStatement(childScope);
			nameSymbol = childScopeStatement->lookup_symbol(SgName(name));
			collision = collision || (nameSymbol != NULL);
		}
	} while (collision);

	return name;
}

/** Returns true if the given expression refers to a variable. This could include using the
  * dot and arrow operator to access member variables and the dereferencing / addressof operators. */
bool backstroke_util::IsVariableReference(SgExpression* expression)
{
	if (isSgVarRefExp(expression))
	{
		return true;
	}
	else if (isSgAddressOfOp(expression))
	{
		return IsVariableReference(isSgAddressOfOp(expression)->get_operand());
	}
	else if (isSgDotExp(expression))
	{
		SgDotExp* dotExpression = isSgDotExp(expression);
		return IsVariableReference(dotExpression->get_lhs_operand()) &&
				IsVariableReference(dotExpression->get_rhs_operand());
	}
	else if (isSgArrowExp(expression))
	{
		SgArrowExp* arrowExpression = isSgArrowExp(expression);
		return IsVariableReference(arrowExpression->get_lhs_operand()) &&
				IsVariableReference(arrowExpression->get_rhs_operand());
	}
	else if (isSgCommaOpExp(expression))
	{
		//Comma op where both the lhs and th rhs are variable references.
		//The lhs would be semantically meaningless since it doesn't have any side effects
		SgCommaOpExp* commaOp = isSgCommaOpExp(expression);
		return IsVariableReference(commaOp->get_lhs_operand()) &&
				IsVariableReference(commaOp->get_rhs_operand());
	}
	else
	{
		return false;
	}
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
tuple<SgVariableDeclaration*, SgAssignOp*, SgExpression*> backstroke_util::CreateTempVariableForExpression(SgExpression* expression, SgScopeStatement* scope, bool initializeInDeclaration)
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
