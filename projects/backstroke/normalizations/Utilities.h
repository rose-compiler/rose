#pragma once

#include "rose.h"
#include <string>

namespace backstroke_util
{
	/** Generate a name that is unique in the current scope and any parent and children scopes.
	  * @param baseName the word to be included in the variable names. */
	std::string GenerateUniqueVariableName(SgScopeStatement* scope, std::string baseName = "temp");
}

