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
