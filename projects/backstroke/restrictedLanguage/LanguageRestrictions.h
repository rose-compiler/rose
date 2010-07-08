#pragma once

#include <vector>
#include "rose.h"

class LanguageRestrictions
{
public:
	/** Checks that all the given function definitions conform to the language restrictions.
     * @param eventList definitions of the functions which should conform to the language restrictions
     * @return true if there is a language restriction violation; false if all the functions pass
     */
	static bool violatesRestrictionsOnEventFunctions(std::vector<SgFunctionDefinition*> eventList);

	/** Checks a single function conforms to the language restrictions.
	 * @param functionDefintion definition that should be checked for banned language constructs
	 * @return false if the function passed, true if the function contains banned constructs
	 */
	static bool violatesRestrictionsOnEventFunctions(SgFunctionDefinition* functionDefinition);

private:

	/** Returns true if the given function declares any local variables that are of a pointer or array type. */
	static bool containsDeclarationsOfPointerVariables(SgFunctionDefinition* functionDefinition);

	/** Returns true if any of the arguments of the given function are passed by reference. */
	static bool takesArgumentsByReference(SgFunctionDefinition* functionDefinition);

	/** Returns true if the function contains continue, goto, or break. The break statement is allowed inside
	  * switch statements. */
	static bool containsJumpStructures(SgFunctionDefinition* functionDefinition);

	/** Returns true if the function has any throw or catch statements. */
	static bool containsExceptionHandling(SgFunctionDefinition* functionDefinition);

	/** Returns true if the given function calls any other functions through function pointers
	  * or through virtual functions. */
	static bool usesFunctionPointersOrVirtualFunctions(SgFunctionDefinition* functionDefinition);
};