#pragma once

#include "rose.h"
#include <string>

namespace backstroke_util
{
	/** Generate a name that is unique in the current scope and any parent and children scopes.
	  * @param baseName the word to be included in the variable names. */
	std::string GenerateUniqueVariableName(SgScopeStatement* scope, std::string baseName = "temp");

	/** Returns true if the given expression refers to a variable. This could include using the
	  * dot and arrow operator to access member variables and the dereferencing / addressof operators.
	  * Note that an expression which is a variable reference necessarily has no side effects. */
	bool IsVariableReference(SgExpression* expression);
}

