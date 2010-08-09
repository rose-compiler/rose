#pragma once

#include "rose.h"
#include <boost/tuple/tuple.hpp>

typedef bool SynthetizedAttribute;

/** Stores a function call expression, along with associated information about its context. */
struct FunctionCallInfo
{
	/** The function call expression. */
	SgFunctionCallExp* functionCall;

	/** When a variable is created to replace one of the arguments of this function, where should it be inserted?
	  * The declaration of the variable will occur right before this statement. */
	SgStatement* tempVarDeclarationLocation;

	/** How a statement should be inserted.   */
	enum InsertionMode 
	{
		/** Insert right before the given statement. */
		INSERT_BEFORE,
		/** Insert at the bottom of the scope defined by the given statement. */
		APPEND_SCOPE,
		INVALID
	};

	/** How to insert the temporary variable declaration. */
	InsertionMode tempVarDeclarationInsertionMode;


	FunctionCallInfo(SgFunctionCallExp* function) : 
		functionCall(function),
		tempVarDeclarationLocation(NULL),
		tempVarDeclarationInsertionMode(INVALID)
		{}
};


struct FunctionCallInheritedAttribute
{
	/** The innermost loop inside of which this AST node resides. It is either a for-loop,
	 a do-looop, or a while-loop. */
	SgScopeStatement* currentLoop;

	/** The last statement encountered before the current node in the AST. */
	SgStatement* lastStatement;

	/** Is the current node inside a for loop structure (not the body). */
	enum { INSIDE_FOR_INIT, INSIDE_FOR_TEST, INSIDE_FOR_INCREMENT, INSIDE_WHILE_CONDITION,
			INSIDE_DO_WHILE_CONDITION, NOT_IN_LOOP }
	loopStatus;

	/** Default constructor. Initializes everything to NULL. */
	FunctionCallInheritedAttribute() : currentLoop(NULL), lastStatement(NULL), loopStatus(NOT_IN_LOOP) {}
};


class FunctionEvaluationOrderTraversal : public AstTopDownBottomUpProcessing<FunctionCallInheritedAttribute, SynthetizedAttribute>
{
public:
	/** Traverses the subtree of the given AST node and finds all function calls in
	 * function-evaluation order. */
	static std::vector<FunctionCallInfo> GetFunctionCalls(SgNode* root);

	/** Visits AST nodes in pre-order */
	FunctionCallInheritedAttribute evaluateInheritedAttribute(SgNode* astNode, FunctionCallInheritedAttribute parentAttribute);

	/** Visits AST nodes in post-order. This is function-evaluation order. */
	SynthetizedAttribute evaluateSynthesizedAttribute(SgNode* astNode, FunctionCallInheritedAttribute parentAttribute, SynthesizedAttributesList);

private:

	FunctionEvaluationOrderTraversal() {}

	/** All the function calls seen so far. */
	std::vector<FunctionCallInfo> functionCalls;
};

class ExtractFunctionArguments
{
public:

	/** Perform the function argument extraction on all function calls in the given subtree of the AST.
	* Returns true on sucess, false on failure (unsupported code). */
	static bool NormalizeTree(SgNode* tree);

private:

	/** Given the expression which is the argument to a function call, returns true if that
	  * expression should be pulled out into a temporary variable on a separate line.
	  * E.g. if the expression contains a function call, it needs to be normalized, while if it
	  * is a constant, there is no need to change it. */
	static bool FunctionArgumentNeedsNormalization(SgExpression* argument);

	/** Returns true if any of the arguments of the given function call will need to
	  * be extracted. */
	static bool FunctionArgsNeedNormalization(SgExprListExp* functionArgs);

	/** Returns true if any function calls in the given subtree will need to be
	  * instrumented. (to extract function arguments). */
	static bool SubtreeNeedsNormalization(SgNode* top);

	/** Given the information about a function call (obtained through a traversal), extract its arguments
	 * into temporary variables where it is necessary.
	 * Returns true on sucess, false on failure (unsupported code). */
	static bool RewriteFunctionCallArguments(const FunctionCallInfo& functionCallInfo);

	/** Insert a new statement in the specified location. The actual insertion can occur either before or after the location
	  * depending on the insertion mode. */
	static void InsertStatement(SgStatement* newStatement, SgStatement* location, FunctionCallInfo::InsertionMode insertionMode);
};
