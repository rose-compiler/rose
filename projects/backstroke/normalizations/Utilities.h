#pragma once

#include "rose.h"
#include <string>
#include <boost/tuple/tuple.hpp>

namespace backstroke_util
{
	/** Generate a name that is unique in the current scope and any parent and children scopes.
	  * @param baseName the word to be included in the variable names. */
	std::string GenerateUniqueVariableName(SgScopeStatement* scope, std::string baseName = "temp");

	/** Returns true if the given expression refers to a variable. This could include using the
	  * dot and arrow operator to access member variables and the dereferencing / addressof operators.
	  * Note that an expression which is a variable reference necessarily has no side effects. */
	bool IsVariableReference(SgExpression* expression);

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
	boost::tuple<SgVariableDeclaration*, SgAssignOp*, SgExpression* > CreateTempVariableForExpression(SgExpression* expression,
		SgScopeStatement* scope, bool initializeInDeclaration);
}

