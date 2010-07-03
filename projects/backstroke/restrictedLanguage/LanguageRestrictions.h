#pragma once

#include <vector>
#include "rose.h"

namespace backstroke
{
	/** Checks that all the given function definitions conform to the language restrictions.
     * @param eventList definitions of the functions which should conform to the language restrictions
     * @return true if there is a language restriction violation; false if all the functions pass
     */
	bool violatesRestrictionsOnEventFunctions(std::vector<SgFunctionDefinition*> eventList);

	/** Checks a single function conforms to the language restrictions.
	 * @param functionDefintion definition that should be checked for banned language constructs
	 * @return false if the function passed, true if the function contains banned constructs
	 */
	bool violatesRestrictionsOnEventFunctions(SgFunctionDefinition* functionDefinition);
}