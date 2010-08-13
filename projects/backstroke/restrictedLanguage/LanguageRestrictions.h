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

        /** Checks a single function conforms to the language restrictions.
         * @param functionDefintion definition that should be checked for banned language constructs
         * @return false if the function passed, true if the function contains banned constructs
         */
        static bool violatesRestrictionsOnEventFunctions(SgFunctionDeclaration* functionDeclaration);

private:

        /** Returns true if the given function declares any local variables that are of a pointer or array type. */
        static bool containsDeclarationsOfPointerVariables(SgFunctionDefinition* functionDefinition);

        /** Returns true if the function contains continue, goto, or break. The break statement is allowed inside
          * switch statements. */
        static bool containsJumpStructures(SgFunctionDefinition* functionDefinition);

        /** Returns true if the function has any throw or catch statements. */
        static bool containsExceptionHandling(SgFunctionDefinition* functionDefinition);

        /** Returns true if the given function calls any other functions through function pointers
          * or through virtual functions. */
        static bool usesFunctionPointersOrVirtualFunctions(SgFunctionDefinition* functionDefinition);

        /** Returns true if the function accesses an array or a pointer through the bracket operator []. */
        static bool usesArrays(SgFunctionDefinition* functionDefinition);

        /** Returns true if any variable of a pointer type (possibly passed as an argument) is written
          * in the function. Note that this refers to the pointer value itself, not to the object it's pointing to. */
        static bool variablesOfPointerTypesAreAssigned(SgFunctionDefinition* functionDefinition);

        /** Returns true if the function uses C++-style dynamic memory allocation. C-style allocation
          * is more difficult to detect because it is a library function and not a language feature. */
        static bool dynamicMemoryAllocationUsed(SgFunctionDefinition* functionDefinition);

        /** Returns true if the function takes a variable number of arguments. */
        static bool hasVariableArguments(SgFunctionDefinition* functionDefinition);

        /** Returns true if the function uses any type that is not a scalar or a struct strictly containing scalars.*/
        static bool usesBannedTypes(SgFunctionDefinition* functionDefinition);

        /** Returns true if the function uses any syntax which is not part of the C++ standard, but the GNU extension.*/
        static bool usesGnuExtensions(SgFunctionDefinition* functionDefinition);

        /** True if the function has return statements before the bottom of the body. */
        static bool returnsBeforeFunctionEnd(SgFunctionDefinition* functionDefinition);
};
