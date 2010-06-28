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