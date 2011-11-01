#pragma once

#include <rose.h>

class FunctionCallInfo;

class ExtractFunctionArguments
{
public:

	/** Perform the function argument extraction on all function calls in the given subtree of the AST. */
	static void NormalizeTree(SgNode* tree);

private:

	/** Given the expression which is the argument to a function call, returns true if that
	 * expression should be pulled out into a temporary variable on a separate line.
	 * E.g. if the expression contains a function call, it needs to be normalized, while if it
	 * is a constant, there is no need to change it. */
	static bool FunctionArgumentNeedsNormalization(SgExpression*& argument);

	/** Returns true if any of the arguments of the given function call will need to
	 * be extracted. */
	static bool FunctionArgsNeedNormalization(SgExprListExp* functionArgs);

	/** Returns true if any function calls in the given subtree will need to be
	 * instrumented. (to extract function arguments). */
	static bool SubtreeNeedsNormalization(SgNode* top);

	/** Given the information about a function call (obtained through a traversal), extract its arguments
	 * into temporary variables where it is necessary. */
	static void RewriteFunctionCallArguments(const FunctionCallInfo& functionCallInfo);

	/** Insert a new statement in the specified location. The actual insertion can occur either before or after the location
	 * depending on the insertion mode. */
	static void InsertStatement(SgStatement* newStatement, SgStatement* location, const FunctionCallInfo& insertionMode);
};
